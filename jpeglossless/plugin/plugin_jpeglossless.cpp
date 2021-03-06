/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-09-26
 * Description : loss less images transformations plugin.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "plugin_jpeglossless.moc"

// C++ includes

#include <iostream>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kshortcut.h>
#include <kstandardguiitem.h>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpbatchprogressdialog.h"
#include "actionthread.h"

using namespace KIPIPlugins;

namespace KIPIJPEGLossLessPlugin
{

K_PLUGIN_FACTORY( JPEGLosslessFactory, registerPlugin<Plugin_JPEGLossless>(); )
K_EXPORT_PLUGIN ( JPEGLosslessFactory("kipiplugin_jpeglossless") )

class Plugin_JPEGLossless::Plugin_JPEGLosslessPriv
{
public:

    Plugin_JPEGLosslessPriv()
    {
        total                    = 0;
        current                  = 0;
        action_Convert2GrayScale = 0;
        action_AutoExif          = 0;
        action_RotateImage       = 0;
        action_FlipImage         = 0;
        progressDlg              = 0;
        thread                   = 0;
        failed                   = false;
    }

    bool                   failed;

    int                    total;
    int                    current;

    KAction*               action_Convert2GrayScale;
    KAction*               action_AutoExif;

    KActionMenu*           action_RotateImage;
    KActionMenu*           action_FlipImage;

    KUrl::List             images;

    KPBatchProgressDialog* progressDlg;

    ActionThread*          thread;
};

Plugin_JPEGLossless::Plugin_JPEGLossless(QObject* const parent, const QVariantList&)
    : Plugin(JPEGLosslessFactory::componentData(), parent, "JPEGLossless"),
      d(new Plugin_JPEGLosslessPriv)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_JPEGLossless plugin loaded";
}

Plugin_JPEGLossless::~Plugin_JPEGLossless()
{
    delete d->progressDlg;
    delete d;
}

void Plugin_JPEGLossless::setup(QWidget* widget)
{
    Plugin::setup( widget );

    d->action_RotateImage = new KActionMenu(KIcon("object-rotate-right"), i18n("Rotate"), actionCollection());
    d->action_RotateImage->setObjectName("jpeglossless_rotate");

    KAction* left = actionCollection()->addAction("rotate_ccw");
    left->setText(i18nc("rotate image left", "Left"));
    left->setShortcut(KShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_Left));
    connect(left, SIGNAL(triggered(bool)),
            this, SLOT(slotRotateLeft()));
    d->action_RotateImage->addAction(left);

    KAction* right = actionCollection()->addAction("rotate_cw");
    right->setText(i18nc("rotate image right", "Right"));
    right->setShortcut(KShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_Right));
    connect(right, SIGNAL(triggered(bool)),
            this, SLOT(slotRotateRight()));
    d->action_RotateImage->addAction(right);

    addAction(d->action_RotateImage);

    // -----------------------------------------------------------------------------------

    d->action_FlipImage = new KActionMenu(KIcon("flip-horizontal"), i18n("Flip"), actionCollection());
    d->action_FlipImage->setObjectName("jpeglossless_flip");

    KAction* hori = actionCollection()->addAction("flip_horizontal");
    hori->setText(i18n("Horizontally"));
    hori->setShortcut(KShortcut(Qt::CTRL+Qt::Key_Asterisk));
    connect(hori, SIGNAL(triggered(bool)),
            this, SLOT(slotFlipHorizontally()));
    d->action_FlipImage->addAction(hori);

    KAction* verti = actionCollection()->addAction("flip_vertical");
    verti->setText(i18n("Vertically"));
    verti->setShortcut(KShortcut(Qt::CTRL+Qt::Key_Slash));
    connect(verti, SIGNAL(triggered(bool)),
            this, SLOT(slotFlipVertically()));
    d->action_FlipImage->addAction(verti);

    addAction(d->action_FlipImage);

    // -----------------------------------------------------------------------------------

    d->action_AutoExif = actionCollection()->addAction("rotate_exif");
    d->action_AutoExif->setText(i18n("Auto Rotate/Flip Using Exif Information"));
    connect(d->action_AutoExif, SIGNAL(triggered(bool)),
            this, SLOT(slotRotateExif()));

    addAction(d->action_AutoExif);

    // -----------------------------------------------------------------------------------

    d->action_Convert2GrayScale = actionCollection()->addAction("jpeglossless_convert2grayscale");
    d->action_Convert2GrayScale->setText(i18n("Convert to Black && White"));
    d->action_Convert2GrayScale->setIcon(KIcon("grayscaleconvert"));
    connect(d->action_Convert2GrayScale, SIGNAL(triggered(bool)),
            this, SLOT(slotConvert2GrayScale()));

    addAction(d->action_Convert2GrayScale);

    // -----------------------------------------------------------------------------------

    Interface* interface = dynamic_cast<Interface*>(parent());
    if (!interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    d->thread = new ActionThread(this);

    connect( d->thread, SIGNAL(starting(KUrl, int)),
             this, SLOT(slotStarting(KUrl, int)));

    connect( d->thread, SIGNAL(finished(KUrl, int)),
             this, SLOT(slotFinished(KUrl, int)));

    connect( d->thread, SIGNAL(failed(KUrl, int, QString)),
             this, SLOT(slotFailed(KUrl, int, QString)));

    bool hasSelection = interface->currentSelection().isValid();

    d->action_AutoExif->setEnabled( hasSelection );
    connect( interface, SIGNAL(selectionChanged(bool)),
             d->action_AutoExif, SLOT(setEnabled(bool)) );

    d->action_RotateImage->setEnabled( hasSelection );
    connect( interface, SIGNAL(selectionChanged(bool)),
             d->action_RotateImage, SLOT(setEnabled(bool)) );

    d->action_FlipImage->setEnabled( hasSelection );
    connect( interface, SIGNAL(selectionChanged(bool)),
             d->action_FlipImage, SLOT(setEnabled(bool)) );

    d->action_Convert2GrayScale->setEnabled( hasSelection );
    connect( interface, SIGNAL(selectionChanged(bool)),
             d->action_Convert2GrayScale, SLOT(setEnabled(bool)) );
}

void Plugin_JPEGLossless::slotFlipHorizontally()
{
    flip(FlipHorizontal, i18n("horizontally"));
}

void Plugin_JPEGLossless::slotFlipVertically()
{
    flip(FlipVertical, i18n("vertically"));
}

void Plugin_JPEGLossless::flip(FlipAction action, const QString& title)
{
    KUrl::List items = images();
    if (items.count() <= 0) return;

    d->thread->flip(items, action);

    d->total   = items.count();
    d->current = 0;
    d->failed  = false;

    delete d->progressDlg;
    d->progressDlg = 0;

    d->progressDlg = new KPBatchProgressDialog(kapp->activeWindow(),
                         i18n("Flip images %1", title));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();

    if (!d->thread->isRunning())
        d->thread->start();
}

void Plugin_JPEGLossless::slotRotateRight()
{
    rotate(Rot90, i18n("right (clockwise)"));
}

void Plugin_JPEGLossless::slotRotateLeft()
{
    rotate(Rot270, i18n("left (counterclockwise)"));
}

void Plugin_JPEGLossless::slotRotateExif()
{
    rotate(Rot0, i18n("using Exif orientation tag"));
}

void Plugin_JPEGLossless::rotate(RotateAction action, const QString& title)
{
    KUrl::List items = images();
    if (items.count() <= 0) return;

    d->thread->rotate(items, action);

    d->total   = items.count();
    d->current = 0;
    d->failed  = false;

    delete d->progressDlg;
    d->progressDlg = 0;
    d->progressDlg = new KPBatchProgressDialog(kapp->activeWindow(),
                         i18n("Rotate images %1", title));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();

    if (!d->thread->isRunning())
        d->thread->start();
}

void Plugin_JPEGLossless::slotConvert2GrayScale()
{
    KUrl::List items = images();
    if (items.count() <= 0 ||
        KMessageBox::No == KMessageBox::warningYesNo(kapp->activeWindow(),
                     i18n("<p>Are you sure you wish to convert the selected image(s) to "
                         "black and white? This operation <b>cannot</b> be undone.</p>")))
        return;

    QString from(sender()->objectName());

    d->total   = items.count();
    d->current = 0;
    d->failed  = false;

    delete d->progressDlg;
    d->progressDlg = 0;

    d->progressDlg = new KPBatchProgressDialog(kapp->activeWindow(),
                         i18n("Convert images to black & white"));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();

    d->thread->convert2grayscale(items);
    if (!d->thread->isRunning())
        d->thread->start();
}

void Plugin_JPEGLossless::slotCancel()
{
    d->thread->cancel();

    Interface* interface = dynamic_cast<Interface*>( parent() );

    if ( !interface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    interface->refreshImages( d->images );
}

void Plugin_JPEGLossless::slotStarting(const KUrl& url, int action)
{
    QString text;
    QString filePath = url.toLocalFile();

    switch ((Action)action)
    {
        case(Rotate):
        {
            text = i18n("Rotating Image \"%1\"", filePath.section('/', -1));
            break;
        }
        case(Flip):
        {
            text = i18n("Flipping Image \"%1\"", filePath.section('/', -1));
            break;
        }
        case(GrayScale):
        {
            text = i18n("Converting to Black & White \"%1\"", filePath.section('/', -1));
            break;
        }
        default:
        {
            kWarning() << "KIPIJPEGLossLessPlugin: Unknown event";
            break;
        }
    }

    d->progressDlg->progressWidget()->addedAction(text, StartingMessage);
}

void Plugin_JPEGLossless::slotFinished(const KUrl& url, int action)
{
    Q_UNUSED(url);

    QString text;

    switch ((Action)action)
    {
        case(Rotate):
        {
            text = i18n("Rotate image complete");
            break;
        }
        case(Flip):
        {
            text = i18n("Flip image complete");
            break;
        }
        case(GrayScale):
        {
            text = i18n("Convert to Black & White complete");
            break;
        }
        default:
        {
            kWarning() << "KIPIJPEGLossLessPlugin: Unknown event";
            break;
        }
    }

    d->progressDlg->progressWidget()->addedAction(text, SuccessMessage);

    oneTaskCompleted();
}

void Plugin_JPEGLossless::slotFailed(const KUrl& url, int action, const QString& errString)
{
    Q_UNUSED(url);

    d->failed = true;
    QString text;

    switch ((Action)action)
    {
        case(Rotate):
        {
            text = i18n("Failed to Rotate image");
            break;
        }
        case(Flip):
        {
            text = i18n("Failed to Flip image");
            break;
        }
        case(GrayScale):
        {
            text = i18n("Failed to convert image to Black & White");
            break;
        }
        default:
        {
            kWarning() << "KIPIJPEGLossLessPlugin: Unknown event";
            break;
        }
    }

    d->progressDlg->progressWidget()->addedAction(text, WarningMessage);

    if (!errString.isEmpty())
        d->progressDlg->progressWidget()->addedAction(errString, WarningMessage);

    oneTaskCompleted();
}

void Plugin_JPEGLossless::oneTaskCompleted()
{
    d->current++;
    d->progressDlg->progressWidget()->setProgress(d->current, d->total);

    if (d->current >= d->total)
    {
        d->current = 0;

        if (d->failed)
        {
            d->progressDlg->setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());

            disconnect(d->progressDlg, SIGNAL(cancelClicked()),
                       this, SLOT(slotCancel()));
        }
        else
        {
            slotCancel();
            d->progressDlg->close();
            d->progressDlg = 0;
        }

        Interface* interface = dynamic_cast<Interface*>( parent() );

        if ( !interface )
        {
           kError() << "Kipi interface is null!";
           return;
        }

        interface->refreshImages( d->images );
    }
}

Category Plugin_JPEGLossless::category(KAction* action) const
{
    if (action == d->action_AutoExif)
        return ImagesPlugin;
    else if ( action == d->action_RotateImage )
       return ImagesPlugin;
    else if ( action == d->action_FlipImage )
       return ImagesPlugin;
    else if ( action == d->action_Convert2GrayScale )
       return ImagesPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ImagesPlugin; // no warning from compiler, please
}

KUrl::List Plugin_JPEGLossless::images()
{
    Interface* interface = dynamic_cast<Interface*>( parent() );

    if ( !interface )
    {
        kError() << "Kipi interface is null!";
        return KUrl::List();
    }

    ImageCollection images = interface->currentSelection();
    if ( !images.isValid() )
        return KUrl::List();

    // We don't want the set of images to change before we are done
    // and tells the host app to refresh the images.
    d->images = images.images();
    return images.images();
}

} // namespace KIPIJPEGLossLessPlugin
