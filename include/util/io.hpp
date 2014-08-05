/*

PICCANTE
The hottest HDR imaging library!
http://vcg.isti.cnr.it/piccante

Copyright (C) 2014
Visual Computing Laboratory - ISTI CNR
http://vcg.isti.cnr.it
First author: Francesco Banterle

PICCANTE is free software; you can redistribute it and/or modify
under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation; either version 3.0 of
the License, or (at your option) any later version.

PICCANTE is distributed in the hope that it will be useful, but
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License
( http://www.gnu.org/licenses/lgpl-3.0.html ) for more details.

*/

#ifndef PIC_UTIL_IO_HPP
#define PIC_UTIL_IO_HPP

#include <string>
#include "base.hpp"

namespace pic {

enum LABEL_IO_EXTENSION {IO_TMP, IO_PFM, IO_HDR, IO_EXR, IO_VOL, IO_BMP, IO_PPM, IO_TGA, IO_JPG, IO_PNG, IO_PGM, IO_NULL};

//HDR
PIC_INLINE LABEL_IO_EXTENSION getLabelHDRExtension(std::string nameFile)
{
    size_t posTMP = nameFile.find(".tmp");

    if(posTMP != std::string::npos)	{
        return IO_TMP;
    }

    size_t posPFM = nameFile.find(".pfm");

    if(posPFM != std::string::npos)	{
        return IO_PFM;
    }

    size_t posHDR = nameFile.find(".hdr");

    if(posHDR != std::string::npos)	{
        return IO_HDR;
    }

    size_t posPIC = nameFile.find(".pic");

    if(posPIC != std::string::npos)	{
        return IO_HDR;
    }

    size_t posEXR = nameFile.find(".exr");

    if(posEXR != std::string::npos)	{
        return IO_EXR;
    }

    size_t posVOL = nameFile.find(".vol");

    if(posVOL != std::string::npos)	{
        return IO_VOL;
    }

    return IO_NULL;
}

//LDR
PIC_INLINE LABEL_IO_EXTENSION getLabelLDRExtension(std::string nameFile)
{
    size_t posBMP = nameFile.find(".bmp");

    if(posBMP != std::string::npos)	{
        return IO_BMP;
    }

    size_t posPPM = nameFile.find(".ppm");

    if(posPPM != std::string::npos)	{
        return IO_PPM;
    }

    size_t posPGM = nameFile.find(".pgm");

    if(posPGM != std::string::npos)	{
        return IO_PGM;
    }

    size_t posTGA = nameFile.find(".tga");

    if(posTGA != std::string::npos)	{
        return IO_TGA;
    }

    size_t posJPG = nameFile.find(".jpg");

    if(posJPG != std::string::npos)	{
        return IO_JPG;
    }

    posJPG = nameFile.find(".JPG");

    if(posJPG != std::string::npos)	{
        return IO_JPG;
    }

    posJPG = nameFile.find(".jpeg");

    if(posJPG != std::string::npos)	{
        return IO_JPG;
    }

    size_t posPNG = nameFile.find(".png");

    if(posPNG != std::string::npos)	{
        return IO_PNG;
    }

    return IO_NULL;
}

} // end namespace pic

#endif /* PIC_UTIL_IO_HPP */

