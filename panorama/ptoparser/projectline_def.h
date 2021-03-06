/* ============================================================
 * 
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-04
 * Description : a plugin to create panorama by fusion of several images.
 *               This parser is based on pto file format described here:
 *               http://hugin.sourceforge.net/docs/nona/nona.txt, and
 *               on pto files produced by Hugin's tools.
 *
 * Copyright (C) 2012 by Benjamin Girault <benjamin dot girault at gmail dot com>
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

#ifndef PROJECTLINE_DEF_H
#define PROJECTLINE_DEF_H

#include "projectline.h"


namespace KIPIPanoramaPlugin { namespace PtoParser
{

struct panoFileType_ : qi::symbols<char, PTOType::Project::FileFormat::FileType>
{
    panoFileType_()
    {
        add
        ("PNG",             PTOType::Project::FileFormat::PNG)
        ("TIFF",            PTOType::Project::FileFormat::TIFF)
        ("TIFF_m",          PTOType::Project::FileFormat::TIFF_m)
        ("TIFF_multilayer", PTOType::Project::FileFormat::TIFF_multilayer);
    }
} panoFileType;

struct panoTiffCompression_ : qi::symbols<char, PTOType::Project::FileFormat::CompressionMethod>
{
    panoTiffCompression_()
    {
        add
        ("NONE",    PTOType::Project::FileFormat::NONE)
        ("LZW",     PTOType::Project::FileFormat::LZW)
        ("DEFLATE", PTOType::Project::FileFormat::DEFLATE);
    }
} panoTiffCompression;

struct panoBitDepth_ : qi::symbols<char, PTOType::Project::BitDepth>
{
    panoBitDepth_()
    {
        add
        ("UINT8",   PTOType::Project::UINT8)
        ("UINT16",  PTOType::Project::UINT16)
        ("FLOAT",   PTOType::Project::FLOAT);
    }
} panoBitDepth;

typedef enum {WIDTH, HEIGHT, PROJECTION, HDR, REFERENCE} IntParamater;

struct projectParameterInt_ : qi::symbols<char, IntParamater>
{
    projectParameterInt_()
    {
        add
        ("w",   WIDTH)
        ("h",   HEIGHT)
        ("f",   PROJECTION)
        ("R",   HDR)
        ("k",   REFERENCE);
    }
} projectParameterInt;

void setParameterFromInt(IntParamater p, int i, PTOType::Project& project)
{
    switch (p)
    {
        case WIDTH:
            project.size.setWidth(i);
            break;
        case HEIGHT:
            project.size.setHeight(i);
            break;
        case PROJECTION:
            project.projection = PTOType::Project::ProjectionType(i);
            break;
        case HDR:
            project.hdr = (i != 0);
            break;
        case REFERENCE:
            project.photometricReferenceId = i;
            break;
    }
}

template <typename Iterator>
projectLineGrammar<Iterator>::projectLineGrammar() : projectLineGrammar::base_type(line)
{
    using qi::int_;
    using qi::double_;
    using qi::_val;
    using qi::_1;
    using qi::lit;
    using qi::lexeme;
    using phoenix::bind;
    using phoenix::construct;
    using phoenix::push_back;
    using phoenix::ref;
    typedef PTOType::Project Project;
    typedef PTOType::Project::FileFormat PFF;

    // ------------------------- File type parsing -------------------------

    projectFileType = +(
          "c:" >> panoTiffCompression     [bind(&PFF::compressionMethod, _val) = construct<PFF::CompressionMethod>(_1)]
        | lit("r:CROP")                   [bind(&PFF::cropped, _val) = true]
        | lit("p1")                       [bind(&PFF::savePositions, _val) = true]
        | 'q' >> int_                     [bind(&PFF::quality, _val) = _1]
        | panoFileType                    [bind(&PFF::fileType, _val) = _1]
    );

            // ------------------------- Project line parsing -------------------------

    IntParamater p;
    line = *(
          projectParameterInt               [ref(p) = _1]
            >> int_                         [bind(&setParameterFromInt, ref(p), _1, _val)]
        | 'v' >> double_                    [bind(&Project::fieldOfView, _val) = _1]
        | 'n' >> panoFileType               [bind(&Project::FileFormat::fileType, bind(&Project::fileFormat, _val)) = _1]
        | ("n\"" >> projectFileType         [bind(&Project::fileFormat, _val) = _1]
                 >> '"')
        | 'E' >> double_                    [bind(&Project::exposure, _val) = _1]
        | 'T' >> panoBitDepth               [bind(&Project::bitDepth, _val) = construct<Project::BitDepth>(_1)]
        | 'S' >> rectangle                  [bind(&Project::crop, _val) = _1]
        | string                            [bind(&QStringList::push_back, bind(&Project::unmatchedParameters, _val), _1)]
    );
}

}} /* namespace KIPIPanoramaPlugin::PtoParser */

#endif /* PROJECTLINE_DEF_H */
