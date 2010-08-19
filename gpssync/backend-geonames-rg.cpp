/* ============================================================
 *
 * Date        : 2010-05-12
 * Description : Backend for reverse geocoding using geonames.org (non-US)
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
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

#include "backend-geonames-rg.moc"

//Qt includes

#include <QDomDocument>
#include <QMap>
#include <QMessageBox>
#include <QString>
#include <QTimer>

//KDE includes

#include <kio/scheduler.h>
#include <kurl.h>
#include <kio/jobclasses.h>
#include <kdebug.h>
#include <kio/job.h>
#include <klocale.h>
#include <kmessagebox.h>

//local includes

#include "backend-geonames-rg.h"
#include "backend-rg.h"
#include "gpssync_common.h"

namespace KIPIGPSSyncPlugin
{

/** 
 * @class BackendGeonamesRG
 *
 * @brief This class calls Geonames's reverse geocoding service.
 */

class GeonamesInternalJobs
{
public:

    GeonamesInternalJobs()
    : language(),
      request(),
      data(),
      kioJob(0)
    {
    }

    QString language;
    QList<RGInfo> request;
    QByteArray data;
    KIO::Job* kioJob;
};


class BackendGeonamesRG::BackendGeonamesRGPrivate
{
public:

    BackendGeonamesRGPrivate()
    : itemCounter(0),
      itemCount(0),
      jobs(),
      errorMessage()
    {
    }

    int itemCounter;
    int itemCount;
    QList<GeonamesInternalJobs> jobs;
    QString errorMessage;
};

/**
 * Constructor
 * @param parent Parent object.
 */ 
BackendGeonamesRG::BackendGeonamesRG(QObject* const parent)
: RGBackend(parent), d(new BackendGeonamesRGPrivate())
{
}

/**
 * Destructor
 */
BackendGeonamesRG::~BackendGeonamesRG()
{
    delete d;
}

/**
 * This function calls Geonames's reverse geocoding service for each image.
 */
void BackendGeonamesRG::nextPhoto()
{
    KUrl jobUrl("http://ws.geonames.org/findNearbyPlaceName");
    jobUrl.addQueryItem("lat", d->jobs.first().request.first().coordinates.latString());
    jobUrl.addQueryItem("lng", d->jobs.first().request.first().coordinates.lonString());
    jobUrl.addQueryItem("lang", d->jobs.first().language);

    d->jobs.first().kioJob = KIO::get(jobUrl, KIO::NoReload, KIO::HideProgressInfo);

    d->jobs.first().kioJob->addMetaData("User-Agent", getKipiUserAgentName());

    connect(d->jobs.first().kioJob, SIGNAL(data(KIO::Job*, const QByteArray&)), 
            this, SLOT(dataIsHere(KIO::Job*,const QByteArray &)));
    connect(d->jobs.first().kioJob, SIGNAL(result(KJob*)),
            this, SLOT(slotResult(KJob*)));
}

/**
 * Takes coordinates from each image and then connects to Open Street Map's reverse geocoding service.
 * @param rgList A list containing information needed in reverse geocoding process. At this point, it contains only coordinates.
 * @param language The language in which the data will be returned.
 */
void BackendGeonamesRG::callRGBackend(const QList<RGInfo>& rgList, const QString& language)
{
    d->errorMessage.clear();

    for ( int i = 0; i < rgList.count(); ++i)
    {
            bool foundIt = false;
            for ( int j=0; j < d->jobs.count(); j++)
            {
                if (d->jobs[j].request.first().coordinates.sameLonLatAs(rgList[i].coordinates))
                {
                    d->jobs[j].request << rgList[i];
                    d->jobs[j].language = language;
                    foundIt = true;
                    break;
                }   
            }
            if (!foundIt)
            {
                GeonamesInternalJobs newJob;
                newJob.request << rgList.at(i);
                newJob.language = language;
                d->jobs << newJob;
            }
    }
    nextPhoto();
}

void BackendGeonamesRG::dataIsHere(KIO::Job* job, const QByteArray & data)
{
    for (int i = 0; i < d->jobs.count(); ++i)
    {
        if (d->jobs.at(i).kioJob == job)
        {
            d->jobs[i].data.append(data);
            break;
        }
    }
}

/**
 * The data is returned from Open Street Map in a XML. This function translates the XML into a QMap.
 * @param xmlData The returned XML.
 */
QMap<QString,QString> BackendGeonamesRG::makeQMapFromXML(const QString& xmlData)
{
    QMap<QString, QString> mappedData;
    QString resultString;
    QDomDocument doc;

    doc.setContent(xmlData);

    QDomElement docElem =  doc.documentElement();
    QDomNode n = docElem.firstChild().firstChild();

    while (!n.isNull())
    {
        const QDomElement e = n.toElement();
        if (!e.isNull())
        {
            if ( (e.tagName().compare(QString("countryName")) == 0) ||
                (e.tagName().compare(QString("name")) == 0) )
            {
                mappedData.insert(e.tagName(), e.text());
                resultString.append(e.tagName() + ":" + e.text() + "\n");
            }
        }
        n = n.nextSibling();
    }
    return mappedData;
}

/**
 * @return Error message, if any.
 */ 
QString BackendGeonamesRG::getErrorMessage()
{
    return d->errorMessage;
}

/**
 * @return Backend name.
 */ 
QString BackendGeonamesRG::backendName()
{
    return QString("Geonames");
}

void BackendGeonamesRG::slotResult(KJob* kJob)
{
    KIO::Job* kioJob = qobject_cast<KIO::Job*>(kJob);

    if (kioJob->error())
    {

        d->errorMessage = kioJob->errorString();
        emit(signalRGReady(d->jobs.first().request));
        d->jobs.clear();
        return;
    }

    for (int i = 0;i < d->jobs.count(); ++i)
    {
        if (d->jobs.at(i).kioJob == kioJob)
        {
            QString dataString;
            dataString = QString::fromUtf8(d->jobs[i].data.constData(),qstrlen(d->jobs[i].data.constData()));
            int pos = dataString.indexOf("<geonames");
            dataString.remove(0,pos);
            dataString.chop(1);

            QMap<QString,QString> resultMap = makeQMapFromXML(dataString);

            for (int j = 0; j < d->jobs[i].request.count(); ++j)
            {
                d->jobs[i].request[j].rgData =  resultMap;
            }
            emit(signalRGReady(d->jobs[i].request));

            d->jobs.removeAt(i);
            if (!d->jobs.empty())
            {
                QTimer::singleShot(500, this, SLOT(nextPhoto()));
            }
            break;
        }
    }
}

} //KIPIGPSSyncPlugin
