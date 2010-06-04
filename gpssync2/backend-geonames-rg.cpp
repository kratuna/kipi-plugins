
#include "backend-geonames-rg.moc"


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
#include "gpsreversegeocodingwidget.h"
#include "backend-rg.h"
#include "gpssync2_common.h"

//Qt includes
#include <QDomDocument>
#include <QMap>
#include <QMessageBox>
#include <QString>
#include <qtimer.h>

namespace KIPIGPSSyncPlugin
{


class GeonamesInternalJobs {

public:

    GeonamesInternalJobs()
    : request(),
      data(),
      kioJob(0)
    {
    }

    QString language;
    QList<RGInfo> request;
    QByteArray data;
    KIO::Job* kioJob;
};


class BackendGeonamesRGPrivate
{

public:

    BackendGeonamesRGPrivate()
    :jobs()
    {
    }

    int itemCounter;
    int itemCount;
    QList<GeonamesInternalJobs> jobs;
};

BackendGeonamesRG::BackendGeonamesRG(QObject* const parent)
: RGBackend(parent), d(new BackendGeonamesRGPrivate())
{
}

BackendGeonamesRG::~BackendGeonamesRG()
{
    delete d;
}

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

void BackendGeonamesRG::callRGBackend(QList<RGInfo> rgList, QString language)
{

    kDebug()<<"Entering Geonames backend";

    for( int i = 0; i < rgList.count(); ++i){

            bool foundIt = false;
            for( int j=0; j < d->jobs.count(); j++){

                if(d->jobs[j].request.first().coordinates.sameLonLatAs(rgList[i].coordinates)){

                    d->jobs[j].request << rgList[i];
                    d->jobs[j].language = language;
                    foundIt = true;
                    break;

                }   


            }

            if(!foundIt){
            
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
    
    for(int i = 0; i < d->jobs.count(); ++i){

        if(d->jobs.at(i).kioJob == job){
            
            d->jobs[i].data.append(data);
            break;

        }
    }
}


QMap<QString,QString> BackendGeonamesRG::makeQMapFromXML(QString xmlData)
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
            mappedData.insert(e.tagName(), e.text());
            resultString.append(e.tagName() + ":" + e.text() + "\n");
        }

        n = n.nextSibling();

    }
    
    return mappedData;
}


void BackendGeonamesRG::slotResult(KJob* kJob)
{


    KIO::Job* kioJob = qobject_cast<KIO::Job*>(kJob);


    for(int i = 0;i < d->jobs.count(); ++i)
    {

        if(d->jobs.at(i).kioJob == kioJob)
        {

            QString dataString;
            dataString = QString::fromUtf8(d->jobs[i].data.constData(),qstrlen(d->jobs[i].data.constData()));
            int pos = dataString.indexOf("<geonames");
            dataString.remove(0,pos);
            dataString.chop(1);


            QMap<QString,QString> resultMap = makeQMapFromXML(dataString);

            for(int j = 0; j < d->jobs[i].request.count(); ++j){

                d->jobs[i].request[j].rgData =  resultMap;

            }
            emit(signalRGReady(d->jobs[i].request));

            d->jobs.removeAt(i);

            if(!d->jobs.empty()){
            
                QTimer::singleShot(500, this, SLOT(nextPhoto()));

            }
            
            break;
        }

    }
    

}



} //KIPIGPSSyncPlugin
