/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to wikimedia commons
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

#include "wmwidget.moc"
// Qt includes

#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QProgressBar>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QScrollArea>
#include <QMap>
#include <QTabWidget>
#include <QTreeWidgetItem>

// KDE includes

#include <kdebug.h>
#include <kvbox.h>
#include <klocale.h>
#include <kdialog.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kconfiggroup.h>
#include <kcombobox.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/uploadwidget.h>
#include <libkipi/interface.h>

// libKdcraw includes

#include <libkdcraw/rexpanderbox.h>
#include <libkdcraw/version.h>
#include <libkdcraw/squeezedcombobox.h>

// Local includes

#include "kpimageinfo.h"
#include "kpimageslist.h"
#include "kpprogresswidget.h"
#include "kpimageinfo.h"
#include "wmwidget.h"

namespace KIPIWikiMediaPlugin
{

WmWidget::WmWidget(QWidget* const parent)
    : QWidget(parent)
{
    setObjectName("WmWidget");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // -------------------------------------------------------------------

    m_headerLbl = new QLabel(this);
    m_headerLbl->setWhatsThis(i18n("This is a clickable link to open the Wikimedia home page in a web browser."));
    m_headerLbl->setOpenExternalLinks(true);
    m_headerLbl->setFocusPolicy(Qt::NoFocus);

    m_imgList = new KPImagesList(this);
    m_imgList->setControlButtonsPlacement(KPImagesList::ControlButtonsBelow);
    m_imgList->setAllowRAW(true);
    m_imgList->loadImagesFromCurrentSelection();
    m_imgList->listView()->setWhatsThis(i18n("This is the list of images to upload to your Wikimedia account."));
     // *****************************Tab Upload****************************************
    QScrollArea* wrapperScroll = new QScrollArea(this);
    KVBox* wrapperPan   = new KVBox(wrapperScroll->viewport());
    wrapperPan->setAutoFillBackground(false);
    wrapperScroll->setWidget(wrapperPan);
    wrapperScroll->setWidgetResizable(true);
    wrapperScroll->setAutoFillBackground(false);
    wrapperScroll->viewport()->setAutoFillBackground(false);
    wrapperScroll->setVisible(false);

    QWidget* wrapper = new QWidget(wrapperPan);

    QHBoxLayout* wrapperLayout = new QHBoxLayout(wrapper);

    QScrollArea* upload = new QScrollArea(wrapper);
    KVBox* pan = new KVBox(upload->viewport());
    pan->setAutoFillBackground(true);

    upload->setWidget(pan);
    upload->setWidgetResizable(true);
    upload->setAutoFillBackground(false);
    upload->viewport()->setAutoFillBackground(false);

    KVBox* uploadBox = new KVBox(pan);
    QWidget* uploadPanel = new QWidget(uploadBox);
    QVBoxLayout* uploadBoxLayout = new QVBoxLayout(uploadPanel);

    m_fileBox   = new QWidget(uploadBox);
    m_fileBox->setWhatsThis(i18n("This is the login form to your Wikimedia account."));
    QGridLayout* fileBoxLayout = new QGridLayout(m_fileBox);

    loadImageInfoFirstLoad();

    m_titleEdit   = new KLineEdit("Select image", m_fileBox);
    m_dateEdit   = new KLineEdit("Select image", m_fileBox);
    m_categoryEdit   = new KLineEdit("Select image", m_fileBox);
    m_descEdit   = new KLineEdit("Select image", m_fileBox);
    m_longitudeEdit   = new KLineEdit("Select image", m_fileBox);
    m_latitudeEdit   = new KLineEdit("Select image", m_fileBox);

    QLabel* titleLabel     = new QLabel(m_fileBox);
    titleLabel->setText(i18n("Title:"));
    QLabel* dateLabel     = new QLabel(m_fileBox);
    dateLabel->setText(i18n("date:"));
    QLabel* categoryLabel     = new QLabel(m_fileBox);
    categoryLabel->setText(i18n("Category:"));
    QLabel* descLabel     = new QLabel(m_fileBox);
    descLabel->setText(i18n("description:"));
    QLabel* longitudeLabel     = new QLabel(m_fileBox);
    longitudeLabel->setText(i18n("Longitude:"));
    QLabel* latitudeLabel     = new QLabel(m_fileBox);
    latitudeLabel->setText(i18n("Latitude:"));

    m_titleCheck = new QCheckBox(m_fileBox);
    m_dateCheck = new QCheckBox(m_fileBox);
    m_categoryCheck = new QCheckBox(m_fileBox);
    m_descCheck = new QCheckBox(m_fileBox);
    m_longitudeCheck = new QCheckBox(m_fileBox);
    m_latitudeCheck = new QCheckBox(m_fileBox);



    KPushButton* applySelectBtn       = new KPushButton(
            KGuiItem(i18n("Apply"), "list-add",
                     i18n("apply config to the selection")), m_fileBox);



    uploadBoxLayout->setSpacing(KDialog::spacingHint());
    uploadBoxLayout->addWidget(m_fileBox,0,Qt::AlignTop);

    fileBoxLayout->addWidget(titleLabel, 1, 0,1,1);
    fileBoxLayout->addWidget(dateLabel, 2, 0,1,1);
    fileBoxLayout->addWidget(categoryLabel, 3, 0,1,1);
    fileBoxLayout->addWidget(descLabel, 4, 0,1,1);
    fileBoxLayout->addWidget(latitudeLabel, 5, 0,1,1);
    fileBoxLayout->addWidget(longitudeLabel, 6, 0,1,1);

    fileBoxLayout->addWidget(m_titleEdit, 1, 1,1,3);
    fileBoxLayout->addWidget(m_dateEdit, 2, 1,1,3);
    fileBoxLayout->addWidget(m_categoryEdit, 3, 1,1,3);
    fileBoxLayout->addWidget(m_descEdit, 4, 1,1,3);
    fileBoxLayout->addWidget(m_latitudeEdit, 5, 1,1,3);
    fileBoxLayout->addWidget(m_longitudeEdit, 6, 1,1,3);

    fileBoxLayout->addWidget(m_titleCheck, 1, 4,1,1,Qt::AlignCenter);
    fileBoxLayout->addWidget(m_dateCheck, 2, 4,1,1,Qt::AlignCenter);
    fileBoxLayout->addWidget(m_categoryCheck, 3, 4,1,1,Qt::AlignCenter);
    fileBoxLayout->addWidget(m_descCheck, 4, 4,1,1,Qt::AlignCenter);
    fileBoxLayout->addWidget(m_latitudeCheck, 5, 4,1,1,Qt::AlignCenter);
    fileBoxLayout->addWidget(m_longitudeCheck, 6, 4,1,1,Qt::AlignCenter);
    fileBoxLayout->addWidget(applySelectBtn,7,3,1,1);


     // *****************************Tab Config****************************************

    QScrollArea* config = new QScrollArea(wrapper);
    KVBox* panel2    = new KVBox(config->viewport());
    panel2->setAutoFillBackground(false);
    config->setWidget(panel2);
    config->setWidgetResizable(true);
    config->setAutoFillBackground(false);
    config->viewport()->setAutoFillBackground(false);

    m_settingsExpander = new RExpanderBox(panel2);
    m_settingsExpander->setObjectName("MediaWiki Settings Expander");

    m_userBox    = new KVBox(panel2);
    m_loginBox   = new QWidget(m_userBox);
    m_loginBox->setWhatsThis(i18n("This is the login form to your Wikimedia account."));
    QGridLayout* loginBoxLayout = new QGridLayout(m_loginBox);

    m_wikiSelect = new QComboBox(m_loginBox);

    m_nameEdit   = new KLineEdit(m_loginBox);
    m_passwdEdit = new KLineEdit(m_loginBox);
    m_passwdEdit->setEchoMode(KLineEdit::Password);

    m_wikiSelect->addItem(i18n("kamelopedia"),
                                              QString("http://kamelopedia.mormo.org/api.php"));
    m_wikiSelect->addItem(i18n("MediaWiki"),
                                              QString("http://www.mediawiki.org/w/api.php"));
    m_wikiSelect->addItem(i18n("Wikia:foto"),
                                              QString("http://foto.wikia.com/api.php"));
    m_wikiSelect->addItem(i18n("Wikia:uncyclopedia"),
                                              QString("http://uncyclopedia.wikia.com/api.php"));
    m_wikiSelect->addItem(i18n("Wikimedia:commons"),
                                              QString("http://commons.wikimedia.org/w/api.php"));
    m_wikiSelect->addItem(i18n("Wikimedia:meta"),
                                              QString("http://meta.wikimedia.org/w/api.php"));
    m_wikiSelect->addItem(i18n("Wikibooks"),
                                              QString("http://en.wikibooks.org/w/api.php"));
    m_wikiSelect->addItem(i18n("Wikinews"),
                                              QString("http://en.wikinews.org/w/api.php"));
    m_wikiSelect->addItem(i18n("Wikipedia"),
                                              QString("http://en.wikipedia.org/w/api.php"));
    m_wikiSelect->addItem(i18n("Wikiquote"),
                                              QString("http://en.wikiquote.org/w/api.php"));
    m_wikiSelect->addItem(i18n("Wikisource"),
                                              QString("http://en.wikinews.org/w/api.php"));
    m_wikiSelect->addItem(i18n("Wiktionary"),
                                              QString("http://en.wiktionary.org/w/api.php"));


    m_wikiSelect->setEditable(false);


    QLabel* wikiLabel     = new QLabel(m_loginBox);
    wikiLabel->setText(i18n("Wiki:"));

    KPushButton* newWikiBtn       = new KPushButton(
            KGuiItem(i18n("New"), "list-add",
                     i18n("Create new wiki")), m_loginBox);

//---newWiki---

    m_newWikiSv = new QScrollArea(this);
    KVBox* newWikiPanel = new KVBox(m_newWikiSv->viewport());
    newWikiPanel->setAutoFillBackground(false);
    m_newWikiSv->setWidget(newWikiPanel);
    m_newWikiSv->setWidgetResizable(true);
    m_newWikiSv->setAutoFillBackground(false);
    m_newWikiSv->viewport()->setAutoFillBackground(false);
    m_newWikiSv->setVisible(false);

    QWidget* newWikiBox                  = new QWidget(newWikiPanel);
    newWikiBox->setWhatsThis(i18n("These are options for adding a Wiki."));

    QGridLayout* newWikiLayout = new QGridLayout(newWikiBox);

    QLabel*  newWikiNameLabel     = new QLabel(newWikiPanel);
    newWikiNameLabel->setText(i18n("Wiki:"));


    QLabel*  newWikiUrlLabel     = new QLabel(newWikiPanel);
    newWikiUrlLabel->setText(i18n("Url:"));


    m_newWikiNameEdit = new KLineEdit(newWikiPanel);

    m_newWikiUrlEdit = new KLineEdit(newWikiPanel);


    KPushButton* addWikiBtn       = new KPushButton(
            KGuiItem(i18n("Add"), "list-add",
                     i18n("add new wiki")), newWikiPanel);


    newWikiLayout->addWidget(newWikiNameLabel, 0, 0, 1, 1);
    newWikiLayout->addWidget(m_newWikiNameEdit, 0, 1, 1, 1);
    newWikiLayout->addWidget(newWikiUrlLabel, 1, 0, 1, 1);
    newWikiLayout->addWidget(m_newWikiUrlEdit, 1, 1, 1, 1);
    newWikiLayout->addWidget(addWikiBtn, 2, 1, 1, 1);




    QLabel* nameLabel     = new QLabel(m_loginBox);
    nameLabel->setText(i18n( "Login:" ));

    QLabel* passwdLabel   = new QLabel(m_loginBox);
    passwdLabel->setText(i18n("Password:"));

    QPushButton* loginBtn = new QPushButton(m_loginBox);
    loginBtn->setAutoDefault(true);
    loginBtn->setDefault(true);
    loginBtn->setText(i18n("&Log in"));

    loginBoxLayout->addWidget(wikiLabel,    0, 0, 1, 1);
    loginBoxLayout->addWidget(m_wikiSelect, 0, 1, 1, 1);
    loginBoxLayout->addWidget(newWikiBtn, 0, 2, 1, 1);
    loginBoxLayout->addWidget(m_newWikiSv,    1, 1, 3, 3);

    loginBoxLayout->addWidget(nameLabel,    4, 0, 1, 1);
    loginBoxLayout->addWidget(m_nameEdit,   4, 1, 1, 1);
    loginBoxLayout->addWidget(m_passwdEdit, 5, 1, 1, 1);
    loginBoxLayout->addWidget(passwdLabel,  5, 0, 1, 1);
    loginBoxLayout->addWidget(loginBtn,     6, 0, 1, 1);
    loginBoxLayout->setObjectName("m_loginBoxLayout");

    m_accountBox         = new KHBox(m_userBox);
    m_accountBox->setWhatsThis(i18n("This is the Wikimedia account that is currently logged in."));

    QLabel* userNameLbl  = new QLabel(m_accountBox);
    userNameLbl->setText(i18nc("Wikimedia account settings", "Logged as: "));
    m_userNameDisplayLbl = new QLabel(m_accountBox);
    QLabel* space        = new QLabel(m_accountBox);
    m_changeUserBtn      = new KPushButton(KGuiItem(i18n("Change Account"), "system-switch-user",
                               i18n("Logout and change Wikimedia Account used for transfer")),
                               m_accountBox);
    m_accountBox->setStretchFactor(space, 10);
    m_accountBox->hide();

    m_settingsExpander->addItem(m_userBox, i18n("Account"), QString("account"), true);
    m_settingsExpander->setItemIcon(0, SmallIcon("user-properties"));

    // ------------------------------------------------------------------------

    m_textBox                  = new QWidget(panel2);
    m_textBox->setWhatsThis(i18n("This is the login form to your Wikimedia account."));
    QGridLayout* textBoxLayout = new QGridLayout(m_textBox);

    QLabel* aut          = new QLabel(i18n("Author:"), m_textBox);
    m_authorEdit         = new KLineEdit(m_textBox);

    QLabel* licenseLabel = new QLabel(i18n("License:"), m_textBox);
    m_licenseComboBox    = new QComboBox(m_textBox);

    m_licenseComboBox->addItem(i18n("Own work, multi-license with CC-BY-SA-3.0 and GFDL"),
                                QString("{{self|cc-by-sa-3.0|GFDL|migration=redundant}}"));
    m_licenseComboBox->addItem(i18n("Own work, multi-license with CC-BY-SA-3.0 and older"),
                                QString("{{self|cc-by-sa-3.0,2.5,2.0,1.0}}"));
    m_licenseComboBox->addItem(i18n("Creative Commons Attribution-Share Alike 3.0"),
                                QString("{{self|cc-by-sa-3.0}}"));
    m_licenseComboBox->addItem(i18n("Own work, Creative Commons Attribution 3.0"),
                                QString("{{self|cc-by-3.0}}"));
    m_licenseComboBox->addItem(i18n("Own work, release into public domain under the CC-Zero license"),
                                QString("{{self|cc-zero}}"));
    m_licenseComboBox->addItem(i18n("Author died more than 100 years ago"),
                                QString("{{PD-old}}"));
    m_licenseComboBox->addItem(i18n("Photo of a two-dimensional work whose author died more than 100 years ago"),
                                QString("{{PD-art}}"));
    m_licenseComboBox->addItem(i18n("First published in the United States before 1923"),
                                QString("{{PD-US}}"));
    m_licenseComboBox->addItem(i18n("Work of a U.S. government agency"),
                                QString("{{PD-USGov}}"));
    m_licenseComboBox->addItem(i18n("Simple typefaces, individual words or geometric shapes"),
                                QString("{{PD-text}}"));
    m_licenseComboBox->addItem(i18n("Logos with only simple typefaces, individual words or geometric shapes"),
                                QString("{{PD-textlogo}}"));

    textBoxLayout->addWidget(aut,               1, 0, 1, 1);
    textBoxLayout->addWidget(m_authorEdit,      1, 2, 1, 2);
    textBoxLayout->addWidget(licenseLabel,      3, 0, 1, 1);
    textBoxLayout->addWidget(m_licenseComboBox, 3, 2, 1, 2);
    textBoxLayout->setObjectName("m_textBoxLayout");

    m_settingsExpander->addItem(m_textBox, i18n("Information"), QString("information"), true);
    m_settingsExpander->setItemIcon(1, SmallIcon("document-properties"));

    //------------------------------------------------------------------------------------

    m_optionsBox                  = new QWidget(panel2);
    m_optionsBox->setWhatsThis(i18n("These are options that will be applied to photos before upload."));
    QGridLayout* optionsBoxLayout = new QGridLayout(m_optionsBox);

    m_resizeChB = new QCheckBox(m_optionsBox);
    m_resizeChB->setText(i18n("Resize photos before uploading"));
    m_resizeChB->setChecked(false);

    m_dimensionSpB = new QSpinBox(m_optionsBox);
    m_dimensionSpB->setMinimum(0);
    m_dimensionSpB->setMaximum(5000);
    m_dimensionSpB->setSingleStep(10);
    m_dimensionSpB->setValue(600);
    m_dimensionSpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_dimensionSpB->setEnabled(false);
    QLabel* dimensionLbl = new QLabel(i18n("Maximum size:"), m_optionsBox);

    m_imageQualitySpB = new QSpinBox(m_optionsBox);
    m_imageQualitySpB->setMinimum(0);
    m_imageQualitySpB->setMaximum(100);
    m_imageQualitySpB->setSingleStep(1);
    m_imageQualitySpB->setValue(85);
    m_imageQualitySpB->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QLabel* imageQualityLbl = new QLabel(i18n("JPEG quality:"), m_optionsBox);

    optionsBoxLayout->addWidget(m_resizeChB,       0, 0, 1, 2);
    optionsBoxLayout->addWidget(imageQualityLbl,   1, 0, 1, 1);
    optionsBoxLayout->addWidget(m_imageQualitySpB, 1, 1, 1, 1);
    optionsBoxLayout->addWidget(dimensionLbl,      2, 0, 1, 1);
    optionsBoxLayout->addWidget(m_dimensionSpB,    2, 1, 1, 1);
    optionsBoxLayout->setRowStretch(3, 10);
    optionsBoxLayout->setSpacing(KDialog::spacingHint());
    optionsBoxLayout->setMargin(KDialog::spacingHint());

    m_settingsExpander->addItem(m_optionsBox, i18n("Options"), QString("options"), true);
    m_settingsExpander->setItemIcon(2, SmallIcon("system-run"));




    // ------------------------------------------------------------------------
    QTabWidget* tabWidget;
    tabWidget =  new QTabWidget;
    tabWidget->addTab(upload, "Upload");
    tabWidget->addTab(config,"Config");

    // ------------------------------------------------------------------------

    m_progressBar = new KPProgressWidget(this);
    m_progressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_progressBar->hide();

    // ------------------------------------------------------------------------
    wrapperLayout->addWidget(m_imgList);
    wrapperLayout->addWidget(tabWidget);

    mainLayout->addWidget(m_headerLbl);
    //mainLayout->addWidget(m_imgList,1,Qt::AlignRight);
    //mainLayout->addWidget(tabWidget,0,Qt::AlignLeft);
    mainLayout->addWidget(wrapper);
    mainLayout->setSpacing(KDialog::spacingHint());
    mainLayout->addWidget(m_progressBar);
    mainLayout->setMargin(0);

    updateLabels();  // use empty labels until login
// ------------------------------------------------------------------------

    connect(m_resizeChB, SIGNAL(clicked()),
            this, SLOT(slotResizeChecked()));

    connect(m_changeUserBtn, SIGNAL(clicked()),
            this, SLOT(slotChangeUserClicked()));

    connect(loginBtn, SIGNAL(clicked()),
            this, SLOT(slotLoginClicked()));

    connect(newWikiBtn, SIGNAL(clicked()),
            this, SLOT(slotNewWikiClicked()));

    connect(addWikiBtn, SIGNAL(clicked()),
            this, SLOT(slotAddWikiClicked()));
    connect(applySelectBtn, SIGNAL(clicked()),
            this, SLOT(slotApplyImagesDesc()));

    //--Le SIGNAL recup par l'imageList ne semble pas etre le bon ( ou probleme au niveau du QTree passé en param... )

   /*connect(m_imgList, SIGNAL(signalItemClicked(QTreeWidgetItem*)),
            this, SLOT(slotBuildSelectedList(QTreeWidgetItem*)));
            */
    connect(m_imgList, SIGNAL(signalItemClicked(QTreeWidgetItem*)),
            this, SLOT(slotLoadImagesDesc(QTreeWidgetItem*)));

}

WmWidget::~WmWidget()
{
}

void WmWidget::readSettings(KConfigGroup& group)
{
    kDebug() <<  "pass here";

#if KDCRAW_VERSION >= 0x020000
    m_settingsExpander->readSettings(group);
#else
    m_settingsExpander->readSettings();
#endif

    m_resizeChB->setChecked(group.readEntry("Resize",      false));
    m_dimensionSpB->setValue(group.readEntry("Dimension",  600));
    m_imageQualitySpB->setValue(group.readEntry("Quality", 85));
    slotResizeChecked();

    m_WikisHistory = group.readEntry("Wikis history", QStringList());
    m_UrlsHistory = group.readEntry("Urls history", QStringList());

    if(m_UrlsHistory.size() != 0 && m_WikisHistory.size() != 0){
        for(int i = 0 ; i < m_UrlsHistory.size() ; i++){
            m_wikiSelect->addItem(m_WikisHistory.at(i),m_UrlsHistory.at(i));
        }
    }


}

void WmWidget::saveSettings(KConfigGroup& group)
{
    kDebug() <<  "pass here";

#if KDCRAW_VERSION >= 0x020000
    m_settingsExpander->writeSettings(group);
#else
    m_settingsExpander->writeSettings();
#endif

    group.writeEntry("Resize",       m_resizeChB->isChecked());
    group.writeEntry("Dimension",    m_dimensionSpB->value());
    group.writeEntry("Quality",      m_imageQualitySpB->value());


}

KPImagesList* WmWidget::imagesList() const
{
    return m_imgList;
}

KPProgressWidget* WmWidget::progressBar() const
{
    return m_progressBar;
}

void WmWidget::updateLabels(const QString& name, const QString& url)
{
    QString web("http://commons.wikimedia.org");

    if (!url.isEmpty())
        web = url;

    m_headerLbl->setText(QString("<b><h2><a href='%1'>"
                                 "<font color=\"#3B5998\">%2</font>"
                                 "</a></h2></b>").arg(web).arg(i18n("Wikimedia commons")));
    if (name.isEmpty())
    {
        m_userNameDisplayLbl->clear();
    }
    else
    {
        m_userNameDisplayLbl->setText(QString::fromLatin1("<b>%1</b>").arg(name));
    }
}

void WmWidget::invertAccountLoginBox()
{
    if(m_accountBox->isHidden())
    {
        m_loginBox->hide();
        m_accountBox->show();
    }
    else
    {
        m_loginBox->show();
        m_accountBox->hide();
    }
}

void WmWidget::slotResizeChecked()
{
    m_dimensionSpB->setEnabled(m_resizeChB->isChecked());
    m_imageQualitySpB->setEnabled(m_resizeChB->isChecked());
}

void WmWidget::slotChangeUserClicked()
{
    emit signalChangeUserRequest();
}

void WmWidget::slotLoginClicked()
{
     emit signalLoginRequest(m_nameEdit->text(), m_passwdEdit->text(), m_wikiSelect->itemData(m_wikiSelect->currentIndex()).toUrl());

}

void WmWidget::slotNewWikiClicked()
{
    if(m_newWikiSv->isVisible()){
        m_newWikiSv->setVisible(false);

    }else{
        m_newWikiSv->setVisible(true);
    }
}
void WmWidget::slotAddWikiClicked()
{

    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Wikimedia Commons settings"));

    m_UrlsHistory << m_newWikiUrlEdit->userText();
    group.writeEntry("Urls history", m_UrlsHistory);

    m_WikisHistory << m_newWikiNameEdit->userText();
    group.writeEntry("Wikis history", m_WikisHistory);

    m_wikiSelect->addItem(m_newWikiNameEdit->userText(),m_newWikiUrlEdit->userText());
    m_wikiSelect->setCurrentIndex(m_wikiSelect->count()-1);

    this->slotNewWikiClicked();


}

void WmWidget::loadImageInfoFirstLoad(){

    QString currentCategories;
    QString date ;
    KUrl::List urls = m_imgList->imageUrls(false);
    QString title;
    QString description;
    QString longitude;
    QString latitude;

    for(int j = 0; j < urls.size(); j++)
    {
        KPImageInfo info(urls.at(j).path());
        QStringList keywar = info.keywords();
        date = info.date().toString(Qt::ISODate);
        date = date.replace("T", " ",Qt::CaseSensitive);
        title = info.name();
        description = info.title();
        longitude = info.longitude();
        latitude = info.latitude();
        currentCategories = "";
        for( int i = 0; i < keywar.size(); i++)
        {
            if(i==keywar.size()-1){
                currentCategories.append(keywar.at(i));
            }else{
                currentCategories.append(keywar.at(i)+", ");
            }
        }
        QMap<QString, QString> imageMetaData;
        imageMetaData["title"]   = title;
        imageMetaData["time"] = date;
        imageMetaData["categories"] = currentCategories;
        imageMetaData["description"] = description;
        imageMetaData["latitude"]  = latitude;
        imageMetaData["longitude"] = longitude;
        imageMetaData["altitude"]  = QString::number(info.altitude());
        m_imagesDescInfo.insert(urls.at(j).path(),imageMetaData);
    }

}

void WmWidget::slotLoadImagesDesc(QTreeWidgetItem* item){


    KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(item);
    QString date ;
    QString title;
    QString description;
    QString longitude;
    QString latitude;

    QString keywar;
    QMap<QString, QString> imageMetaData = m_imagesDescInfo[l_item->url().path()];
        keywar = imageMetaData["categories"];
        date = imageMetaData["time"];
        date = date.replace("T", " ",Qt::CaseSensitive);
        title = imageMetaData["title"];
        description = imageMetaData["description"];
        longitude = imageMetaData["longitude"];
        latitude = imageMetaData["latitude"];

    m_titleEdit->setText(title);
    m_dateEdit->setText(date);
    m_categoryEdit->setText(keywar);
    m_descEdit->setText(description);
    m_longitudeEdit->setText(longitude);
    m_latitudeEdit->setText(latitude);

}
void WmWidget::slotApplyImagesDesc(){

    //-- enregistrement des edit dans la map

    KUrl::List urls;
    QList<QTreeWidgetItem *> selectedItems=m_imgList->listView()->selectedItems();

    for (int i = 0; i < selectedItems.size(); ++i){
        KPImagesListViewItem* l_item = dynamic_cast<KPImagesListViewItem*>(selectedItems.at(i));
    urls.append(l_item->url());
    }

    for (int i = 0; i < urls.size(); ++i)
    {

        QMap<QString, QString> imageMetaData;
        imageMetaData = m_imagesDescInfo[urls.at(i).path()];

        if(m_titleCheck->isChecked()){
            QString url = urls.at(i).path().split('/').last();
            url = title();
            if(url.split('.').last().isEmpty()){
                url = url +"."+urls.at(i).path().split('.').last();
                m_titleEdit->setText(url);
            }
            kDebug()<<" Url in the if "<<url;
            imageMetaData["title"]   = url;
            kDebug()<<" Url after if "<<url;
        }

        if(m_dateCheck->isChecked()){
            imageMetaData["time"] = date();
        }
        if(m_categoryCheck->isChecked()){
            imageMetaData["categories"] = categories();
        }
        if(m_descCheck->isChecked()){
            imageMetaData["description"] = description();
        }
        m_imagesDescInfo[urls.at(i).path()]=imageMetaData;

    }

}



QMap <QString,QMap <QString,QString> > WmWidget::allImagesDesc()
{

    KUrl::List urls = m_imgList->imageUrls(false);


    for (int i = 0; i < urls.size(); ++i)
    {

        QMap<QString, QString> imageMetaData = m_imagesDescInfo[urls.at(i).path()];

            imageMetaData["author"] = author();
            imageMetaData["license"] = license();

        m_imagesDescInfo[urls.at(i).path()]=imageMetaData;

    }

    return m_imagesDescInfo;
}


QString WmWidget::title() const
{
    kDebug() << "WmWidget::author()";
    return m_titleEdit->text();
}

QString WmWidget::author() const
{
    kDebug() << "WmWidget::author()";
    return m_authorEdit->text();
}
int WmWidget::quality() const
{
    kDebug() << "WmWidget::quality()";
    return m_imageQualitySpB->value();
}

int WmWidget::dimension() const
{
    kDebug() << "WmWidget::dimension()";
    return m_dimensionSpB->value();
}
bool WmWidget::resize() const
{
    kDebug() << "WmWidget::resize()";
    return m_resizeChB->isChecked();
}
QString WmWidget::license() const
{
    kDebug() << "WmWidget::license()";

    return m_licenseComboBox->itemData(m_licenseComboBox->currentIndex()).toString();
}

QString WmWidget::categories() const
{
    kDebug() << "WmWidget::categories()";
    return m_categoryEdit->text();
}
QString WmWidget::description() const
{
    kDebug() << "WmWidget::description()";
    return m_descEdit->text();

}
QString WmWidget::date() const
{
    kDebug() << "WmWidget::date()";
    return m_dateEdit->text();
}

} // namespace KIPIWikimediaPlugin
