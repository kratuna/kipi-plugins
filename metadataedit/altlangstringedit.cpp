/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-18
 * Description : a widget to edit a tag with multiple alternative
 *               language string entries.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT includes.

#include <Q3Header>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

// KDE includes.

#include <k3listview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kdialog.h>
#include <ktextedit.h>
#include <klanguagebutton.h>
#include <kiconloader.h>

// LibKExiv2 includes. 

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "altlangstringedit.h"
#include "altlangstringedit.moc"

namespace KIPIMetadataEditPlugin
{

class AltLangStringsEditPriv
{
public:

    AltLangStringsEditPriv()
    {
        addValueButton = 0;
        delValueButton = 0;
        repValueButton = 0;
        listView       = 0;
        valueCheck     = 0;
        textEdit       = 0;
        languageBtn    = 0;
    }

    AltLangStringsEdit::AltLangDataList  oldValues;

    QPushButton                         *addValueButton;
    QPushButton                         *delValueButton;
    QPushButton                         *repValueButton;

    QCheckBox                           *valueCheck;

    K3ListView                          *listView;

    KLanguageButton                     *languageBtn;

    KTextEdit                           *textEdit;
};

AltLangStringsEdit::AltLangStringsEdit(QWidget* parent, const QString& title,
                                       const QString& desc)
                  : QWidget(parent)
{
    d = new AltLangStringsEditPriv;

    QGridLayout *grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->valueCheck = new QCheckBox(title, this);    

    d->addValueButton = new QPushButton(this);
    d->delValueButton = new QPushButton(this);
    d->repValueButton = new QPushButton(this);
    d->addValueButton->setIcon(SmallIcon("edit-add"));
    d->delValueButton->setIcon(SmallIcon("edit-delete"));
    d->repValueButton->setIcon(SmallIcon("view-refresh"));
    d->addValueButton->setWhatsThis(i18n("Add a new value to the list"));
    d->delValueButton->setWhatsThis(i18n("Remove the current selected value from the list"));
    d->repValueButton->setWhatsThis(i18n("Replace the current selected value from the list"));
    d->delValueButton->setEnabled(false);
    d->repValueButton->setEnabled(false);

    d->listView = new K3ListView(this);
    d->listView->header()->hide();
    d->listView->addColumn("Language"); // No need i18n here
    d->listView->addColumn("Text");     // No need i18n here
    d->listView->setResizeMode(Q3ListView::AllColumns);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setSorting(-1);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setSelectionMode(Q3ListView::Single);
    d->listView->setFullWidth(true);

    new K3ListViewItem(d->listView, d->listView->lastItem(), QString("default"), 
                       QString("zobby la mouche"));
    new K3ListViewItem(d->listView, d->listView->lastItem(), QString("French"), 
                       QString("zobby la mouche"));
    new K3ListViewItem(d->listView, d->listView->lastItem(), QString("English"), 
                       QString("zobby la mouche"));

    d->languageBtn   = new KLanguageButton(this);
    d->languageBtn->setMaximumHeight( fontMetrics().height()+2 );
    QStringList list = KGlobal::locale()->allLanguagesList();
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
        d->languageBtn->insertLanguage(*it);

    d->textEdit = new KTextEdit(this);
    d->textEdit->setWhatsThis(desc);
    d->textEdit->setMaximumHeight( fontMetrics().height()*3 ); // 3 lines used to edit text.

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->valueCheck, 0, 0, 1, 1 );
    grid->addWidget(d->addValueButton, 0, 2, 1, 1);
    grid->addWidget(d->delValueButton, 0, 3, 1, 1);
    grid->addWidget(d->repValueButton, 0, 4, 1, 1);
    grid->addWidget(new QLabel(i18n("Language:")), 1, 0, 1, 1);
    grid->addWidget(d->languageBtn, 1, 1, 1, 4);
    grid->addWidget(d->textEdit, 2, 0, 1, 5);
    grid->addWidget(d->listView, 0, 5, 3, 1);
    grid->setColumnStretch(1, 10);   
    grid->setColumnStretch(5, 100);                     
    grid->setMargin(0);
    grid->setSpacing(KDialog::spacingHint());    
                                         
    // --------------------------------------------------------

    connect(d->listView, SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
    
    connect(d->addValueButton, SIGNAL(clicked()),
            this, SLOT(slotAddValue()));
    
    connect(d->delValueButton, SIGNAL(clicked()),
            this, SLOT(slotDeleteValue()));

    connect(d->repValueButton, SIGNAL(clicked()),
            this, SLOT(slotReplaceValue()));

    // --------------------------------------------------------

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalToggled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->textEdit, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->addValueButton, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->delValueButton, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->repValueButton, SLOT(setEnabled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            d->listView, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->addValueButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
    
    connect(d->delValueButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));

    connect(d->repValueButton, SIGNAL(clicked()),
            this, SIGNAL(signalModified()));
}

AltLangStringsEdit::~AltLangStringsEdit()
{
    delete d;
}

void AltLangStringsEdit::slotDeleteValue()
{
/*    QListWidgetItem *item = d->valueBox->currentItem();
    if (!item) return;
    d->valueBox->takeItem(d->valueBox->row(item));
    delete item;*/
}

void AltLangStringsEdit::slotReplaceValue()
{
/*    QString newValue = d->valueEdit->text();
    if (newValue.isEmpty()) return;

    if (!d->valueBox->selectedItems().isEmpty())
    {
        d->valueBox->selectedItems()[0]->setText(newValue);
        d->valueEdit->clear();
    }*/
}

void AltLangStringsEdit::slotSelectionChanged()
{
/*    if (!d->valueBox->selectedItems().isEmpty())
    {
        d->valueEdit->setText(d->valueBox->selectedItems()[0]->text());
        d->delValueButton->setEnabled(true);
        d->repValueButton->setEnabled(true);
    }
    else
    {
        d->delValueButton->setEnabled(false);
        d->repValueButton->setEnabled(false);
    }*/
}

void AltLangStringsEdit::slotAddValue()
{
/*    QString newValue = d->valueEdit->text();
    if (newValue.isEmpty()) return;

    bool found = false;
    for (int i = 0 ; i < d->valueBox->count(); i++)
    {
        QListWidgetItem *item = d->valueBox->item(i);
        if (newValue == item->text()) 
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->valueBox->insertItem(d->valueBox->count(), newValue);
        d->valueEdit->clear();
    }*/
}

void AltLangStringsEdit::setValues(const AltLangDataList& values)
{
    blockSignals(true);
    d->oldValues = values;

/*    d->valueBox->clear();
    d->valueCheck->setChecked(false);
    if (!d->oldValues.isEmpty())
    {
        d->valueBox->insertItems(0, d->oldValues);
        d->valueCheck->setChecked(true);
    }
    d->valueEdit->setEnabled(d->valueCheck->isChecked());
    d->valueBox->setEnabled(d->valueCheck->isChecked());
    d->addValueButton->setEnabled(d->valueCheck->isChecked());
    d->delValueButton->setEnabled(d->valueCheck->isChecked());*/

    blockSignals(false);
}

bool AltLangStringsEdit::getValues(AltLangDataList& oldValues, AltLangDataList& newValues)
{
    oldValues = d->oldValues;

    newValues.clear();
/*    for (int i = 0 ; i < d->valueBox->count(); i++)
    {
        QListWidgetItem *item = d->valueBox->item(i);
        newValues.append(item->text());
    }*/

    return d->valueCheck->isChecked();
}

}  // namespace KIPIMetadataEditPlugin
