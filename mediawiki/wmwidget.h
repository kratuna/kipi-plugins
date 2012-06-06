/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Parthasarathy Gopavarapu <gparthasarathy93 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef WMWIDGET_H
#define WMWIDGET_H

// Qt includes

#include <QWidget>
#include <QScrollArea>
#include <QComboBox>
#include <QStringList>
#include <QTreeWidgetItem>

//KDE includes

#include <klineedit.h>
#include <ktextedit.h>
#include <kconfig.h>

#include <kurl.h>

class QLabel;
class QSpinBox;
class QCheckBox;

class KVBox;
class KHBox;
class KPushButton;

namespace KIPI
{
    class UploadWidget;
}

namespace KIPIPlugins
{
    class KPImagesList;
    class KPProgressWidget;
}

namespace KDcrawIface
{
    class RExpanderBox;
    class SqueezedComboBox;
}

using namespace KIPI;
using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIWikiMediaPlugin
{

enum WmDownloadType
{
    WmMyAlbum = 0,
    WmFriendAlbum,
    WmPhotosMe,
    WmPhotosFriend
};

class WmWidget : public QWidget
{
    Q_OBJECT

public:

    WmWidget(QWidget* const parent);
    ~WmWidget();

    void updateLabels(const QString& name = QString(), const QString& url = QString());

    void invertAccountLoginBox();

    KPImagesList* imagesList() const;

    KPProgressWidget* progressBar() const;

    QString title() const;
    QString author() const;
    int dimension() const;
    int quality() const;
    bool resize() const;
    QString categories() const;
    QString license() const;
    QString description() const;
    QString date() const;

    QMap <QString,QMap <QString,QString> > allImagesDesc();

    void readSettings(KConfigGroup& group);
    void saveSettings(KConfigGroup& group);
    void loadImageInfoFirstLoad();

Q_SIGNALS:

    void signalChangeUserRequest();
    void signalLoginRequest(const QString& login, const QString& pass, const QUrl& wiki);
    void signalApplyUploadSettings();

private Q_SLOTS:

    void slotResizeChecked();
    void slotChangeUserClicked();
    void slotLoginClicked();
    void slotNewWikiClicked();
    void slotAddWikiClicked();
    void slotLoadImagesDesc(QTreeWidgetItem* item);
    void slotApplyImagesDesc();


private:





    KHBox*                     m_uploadBox;
    QWidget*                   m_fileBox;
    KLineEdit*                 m_titleEdit;
    KLineEdit*                 m_descEdit;
    KLineEdit*                 m_dateEdit;
    KLineEdit*                 m_longitudeEdit;
    KLineEdit*                 m_latitudeEdit;
    KLineEdit*                 m_categoryEdit;

    QCheckBox*                 m_titleCheck;
    QCheckBox*                 m_descCheck;
    QCheckBox*                 m_dateCheck;
    QCheckBox*                 m_longitudeCheck;
    QCheckBox*                 m_latitudeCheck;
    QCheckBox*                 m_categoryCheck;

    KVBox*                     m_userBox;
    QWidget*                   m_loginBox;
    QLabel*                    m_loginHeaderLbl;
    KLineEdit*                 m_nameEdit;
    KLineEdit*                 m_passwdEdit;
    QScrollArea*               m_newWikiSv;
    KLineEdit*                 m_newWikiNameEdit;
    KLineEdit*                 m_newWikiUrlEdit;
    QComboBox*                 m_wikiSelect;

    QWidget*                   m_textBox;

    KLineEdit*                 m_authorEdit;

    KHBox*                     m_accountBox;
    QLabel*                    m_headerLbl;
    QLabel*                    m_userNameDisplayLbl;
    KPushButton*               m_changeUserBtn;

    QWidget*                   m_optionsBox;
    QCheckBox*                 m_resizeChB;
    QSpinBox*                  m_dimensionSpB;
    QSpinBox*                  m_imageQualitySpB;
    QComboBox*          m_licenseComboBox;

    KPProgressWidget*          m_progressBar;

    RExpanderBox*              m_settingsExpander;
    KPImagesList*              m_imgList;
    UploadWidget*              m_uploadWidget;

    QStringList     m_WikisHistory;
    QStringList     m_UrlsHistory;

    QMap <QString,QMap <QString,QString> > m_imagesDescInfo;



    friend class WmWindow;
};

} // namespace KIPIWikiMediaPlugin

#endif // WMWIDGET_H
