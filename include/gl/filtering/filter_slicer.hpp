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

#ifndef PIC_GL_FILTERING_FILTER_SLICER_HPP
#define PIC_GL_FILTERING_FILTER_SLICER_HPP

#include "gl/filtering/filter.hpp"

namespace pic {

class FilterGLSlicer: public FilterGL
{
protected:

    void InitShaders();
    void FragmentShader();

    float s_S, s_R, mul_E;

public:
    //Basic constructors
    FilterGLSlicer(float s_S, float s_R);

    //Change parameters
    void Update(float s_S, float s_R);

    //Processing
    ImageRAWGL *Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut);
};

//Init constructors
FilterGLSlicer::FilterGLSlicer(float s_S, float s_R): FilterGL()
{
    this->s_S = s_S;
    this->s_R = s_R;

    FragmentShader();
    InitShaders();
}

void FilterGLSlicer::FragmentShader()
{
    fragment_source = GLW_STRINGFY
                      (
                          uniform sampler2D	u_tex;
                          uniform sampler3D	u_grid;
                          uniform float		mul_E;
                          uniform float		s_S;

                          out     vec4       f_color;

    void main(void) {
        //Fetch texture color
        ivec2 coordsFrag = ivec2(gl_FragCoord.xy);
        vec4 colRef = texelFetch(u_tex, coordsFrag, 0);

        //Fetch E
        vec3 tSize3 = vec3(textureSize(u_grid, 0));
        float E = dot(colRef.xyz, vec3(1.0)) * mul_E;
        E /= tSize3.z;

        //Fetch from the grid
        vec2 coord = (gl_FragCoord.xy * s_S) / tSize3.xy;
        vec4 sliced = texture(u_grid, vec3(coord.xy, E));

        vec3 color = sliced.w > 0.0 ? sliced.xyz / sliced.w : vec3(0.0);

        f_color = vec4(color.xyz, 1.0);
    }
                      );
}

void FilterGLSlicer::InitShaders()
{
    std::string prefix;
    prefix += glw::version("150");
    prefix += glw::ext_require("GL_EXT_gpu_shader4");
    filteringProgram.setup(prefix, vertex_source, fragment_source);

#ifdef PIC_DEBUG
    printf("[filteringProgram log]\n%s\n", filteringProgram.log().c_str());
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.relink();
    filteringProgram.attribute_source("a_position", 0);
    filteringProgram.fragment_target("f_color",    0);
    glw::bind_program(0);
    Update(s_S, s_R);
}

//Change parameters
void FilterGLSlicer::Update(float s_S, float s_R)
{
    this->s_S = s_S;
    this->s_R = s_R;

    mul_E = s_R / 3.0f;

#ifdef PIC_DEBUG
    printf("Rate S: %f Rate R: %f Mul E: %f\n", s_S, s_R, mul_E);
#endif

    glw::bind_program(filteringProgram);
    filteringProgram.relink();
    filteringProgram.uniform("u_tex",      0);
    filteringProgram.uniform("u_grid",	 1);
    filteringProgram.uniform("s_S",	   s_S);
    filteringProgram.uniform("mul_E",	   mul_E);
    glw::bind_program(0);
}

//Processing
ImageRAWGL *FilterGLSlicer::Process(ImageRAWGLVec imgIn, ImageRAWGL *imgOut)
{
    if(imgIn[0] == NULL) {
        return imgOut;
    }

    int w = imgIn[0]->width;
    int h = imgIn[0]->height;

    if(imgOut == NULL) {
        imgOut = new ImageRAWGL(1, w, h, imgIn[0]->channels, IMG_GPU);
    }

    if(fbo == NULL) {
        fbo = new Fbo();
        fbo->create(w, h, 1, false, imgOut->getTexture());
    }

    if(imgIn.size() < 2) {
        return NULL;
    }

    //Rendering
    fbo->bind();
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    //Shaders
    glw::bind_program(filteringProgram);

    //Textures
    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->bindTexture();

    glActiveTexture(GL_TEXTURE1);
    imgIn[1]->bindTexture();

    //Rendering aligned quad
    quad->Render();

    //Fbo
    fbo->unbind();

    //Shaders
    glw::bind_program(0);

    //Textures
    glActiveTexture(GL_TEXTURE1);
    imgIn[1]->unBindTexture();

    glActiveTexture(GL_TEXTURE0);
    imgIn[0]->unBindTexture();

    return imgOut;
};

} // end namespace pic

#endif /* PIC_GL_FILTERING_FILTER_SLICER_HPP */

/*
void TestSlicerGL(std::string nameIn, std::string nameOut){
	//Bilateral Grid GPU
	ImageRAW imgIn2(nameIn);

	FilterBilateral2DG tmp(16.0f,0.1f);
	ImageRAW *imgFlt = tmp.Process(Single(&imgIn2),NULL);
	imgFlt->Write("test.pfm");

	//BILATERAL GPU
	ImageRAWGL imgIn(nameIn);
	imgIn.Mul(1.0/imgIn.getMaxVal());
	imgIn.generateTextureGL(false);

	//Splatting
	ImageRAW *grid = tmp.Splat(NULL,&imgIn);

	//Blurring
	ImageRAWGL *gridGL = new ImageRAWGL(grid,false);

	FilterGLGaussian3D fltGauss3D(1.0f);
	Fbo *gridBlurFbo = fltGauss3D.Process(SingleGL(gridGL),NULL);
	ImageRAWGL *gridBlurGL = new ImageRAWGL(gridBlurFbo->tex,GL_TEXTURE_3D);

	//Blurring
	FilterGaussian3D *fltG = new FilterGaussian3D(1.0f);
	ImageRAW *gridBlur = fltG->Process(Single(gridGL),NULL);

	//Slicing
	FilterGLSlicer fltSlicer(tmp.s_S,tmp.s_R);

	Fbo *fbo = new Fbo();
	fbo->create(imgIn.width,imgIn.height);

	fltSlicer.Process(DoubleGL(&imgIn, gridBlurGL),fbo);

	//Read from the GPU
	ImageRAWGL *imgOut = new ImageRAWGL(1,imgIn.width,imgIn.height,4);
	imgOut->readFromFBO(fbo);
	imgOut->Write(nameOut);
}*/

