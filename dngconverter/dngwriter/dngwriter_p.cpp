/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG
 *
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C++ includes

#include <cstdio>

// Qt includes

#include <QFile>

// KDE includes

#include <kdebug.h>

// Local includes

#include "dngwriter.h"
#include "dngwriter_p.h"

namespace DNGIface
{

DNGWriter::DNGWriterPrivate::DNGWriterPrivate()
{
    reset();
}

DNGWriter::DNGWriterPrivate::~DNGWriterPrivate()
{
}

void DNGWriter::DNGWriterPrivate::reset()
{
    cancel                  = false;
    jpegLossLessCompression = true;
    updateFileDate          = false;
    backupOriginalRawFile   = false;
    previewMode             = DNGWriter::MEDIUM;
}

void DNGWriter::DNGWriterPrivate::cleanup()
{
    ::remove(QFile::encodeName(outputFile));
}

dng_date_time DNGWriter::DNGWriterPrivate::dngDateTime(const QDateTime& qDT)
{
    dng_date_time dngDT;
    dngDT.fYear   = qDT.date().year();
    dngDT.fMonth  = qDT.date().month();
    dngDT.fDay    = qDT.date().day();
    dngDT.fHour   = qDT.time().hour();
    dngDT.fMinute = qDT.time().minute();
    dngDT.fSecond = qDT.time().second();
    return dngDT;
}

}  // namespace DNGIface
