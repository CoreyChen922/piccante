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

#ifndef PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP
#define PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP

#include "gl/filtering/filter_npasses.hpp"
#include "gl/filtering/filter_gaussian_1d.hpp"

namespace pic {

class FilterGLGaussian2D: public FilterGLNPasses
{
protected:
    FilterGLGaussian1D		*filter;

    void InitShaders() {}
    void FragmentShader() {}

public:
    //Basic constructors
    FilterGLGaussian2D();
    //Init constructors
    FilterGLGaussian2D(float sigma);

    //Update
    void Update(float sigma);

    static ImageRAWGL *Execute(std::string nameIn, std::string nameOut, float sigma)
    {
        ImageRAWGL imgIn(nameIn);
        imgIn.generateTextureGL(false);

        FilterGLGaussian2D *filter = new FilterGLGaussian2D(sigma);

#ifdef PIC_DEBUG
        GLuint testTQ1 = glBeginTimeQuery();
#endif

        ImageRAWGL *imgOut = filter->Process(SingleGL(&imgIn), NULL);

#ifdef PIC_DEBUG
        GLuint64EXT timeVal = glEndTimeQuery(testTQ1);
        printf("Gaussian 2D Filter on GPU time: %g ms\n",
               float(timeVal) / 100000000.0f);
#endif

        //Read from the GPU
        imgOut->loadToMemory();
        imgOut->Write(nameOut);
        return imgOut;
    }
};

//Basic constructor
FilterGLGaussian2D::FilterGLGaussian2D(): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;

    filter = new FilterGLGaussian1D(0.0f, 0, target);
    InsertFilter(filter);
    InsertFilter(filter);
}

//Constructor
FilterGLGaussian2D::FilterGLGaussian2D(float sigma): FilterGLNPasses()
{
    target = GL_TEXTURE_2D;

    filter = new FilterGLGaussian1D(sigma, 0, target);
    InsertFilter(filter);
    InsertFilter(filter);
}

void FilterGLGaussian2D::Update(float sigma)
{
    filter->Update(sigma);
}

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_GAUSSIAN_2D_HPP */

