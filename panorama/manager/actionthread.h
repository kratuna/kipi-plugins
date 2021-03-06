/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a plugin to create panorama by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011-2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes

#include <QThread>

// KDE includes

#include <kurl.h>
#include <kprocess.h>
#include <threadweaver/JobCollection.h>

// LibKDcraw includes

#include <libkdcraw/rawdecodingsettings.h>

// Local includes

#include "actions.h"
#include "ptotype.h"
#include "kpactionthreadbase.h"

using namespace KDcrawIface;

namespace KIPIPanoramaPlugin
{

struct ActionData;

class ActionThread : public KIPIPlugins::KPActionThreadBase
{
    Q_OBJECT

public:

    explicit ActionThread(QObject* const parent);
    ~ActionThread();

    void preProcessFiles(const KUrl::List& urlList, ItemUrlsMap& preProcessedMap, KUrl& cpCleanPtoUrl,
                         PTOType& cpCleanPtoData, bool celeste, bool hdr,
                         PanoramaFileType fileType, const RawDecodingSettings& settings,
                         const QString& cpCleanPath, const QString& cpFindPath);
    void optimizeProject(KUrl& ptoUrl, KUrl& optimizePtoUrl, bool levelHorizon,
                         bool optimizeProjectionAndSize, const QString& autooptimiserPath);
    void generatePanoramaPreview(const KUrl& ptoUrl, KUrl& previewUrl,
                                 const ItemUrlsMap& preProcessedUrlsMap,
                                 const QString& makePath, const QString& pto2mkPath,
                                 const QString& enblendPath, const QString& nonaPath);
    void compileProject(const KUrl& ptoUrl, KUrl& panoUrl,
                        const ItemUrlsMap& preProcessedUrlsMap,
                        PanoramaFileType fileType, const QString& makePath, const QString& pto2mkPath,
                        const QString& enblendPath, const QString& nonaPath);
    void copyFiles(const KUrl& ptoUrl, const KUrl& panoUrl, const KUrl& finalPanoUrl,
                   const ItemUrlsMap& preProcessedUrlsMap, bool savePTO);

Q_SIGNALS:

    void starting(const KIPIPanoramaPlugin::ActionData& ad);
    void stepFinished(const KIPIPanoramaPlugin::ActionData& ad);
    void finished(const KIPIPanoramaPlugin::ActionData& ad);

    void optimizePtoReady(const KUrl& optimizePtoUrl);
    void previewFileReady(const KUrl& previewFileUrl);
    void panoFileReady(const KUrl& panoFileUrl);

private Q_SLOTS:

    void slotDone(ThreadWeaver::Job* j);
    void slotStepDone(ThreadWeaver::Job* j);
    void slotStarting(ThreadWeaver::Job* j);

    void deletePtoUrl();
    void deleteCPFindPtoUrl();
    void deletePreviewPtoUrl();
    void deleteMkUrl();

private:

    void appendStitchingJobs(Job* prevJob, JobCollection* jc, const KUrl& ptoUrl,
                             KUrl& outputUrl, const ItemUrlsMap& preProcessedUrlsMap,
                             PanoramaFileType fileType, const QString& makePath, const QString& pto2mkPath,
                             const QString& enblendPath, const QString& nonaPath, bool preview);

private:

    struct ActionThreadPriv;
    ActionThreadPriv* const d;

};

}  // namespace KIPIPanoramaPlugin

#endif /* ACTIONTHREAD_H */
