//
//  ofxWater.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/26/11.
//  Copyright 2011 http://www.kalwaltart.it/ All rights reserved.
//

#include "ofxWater.h"

ofxWater::ofxWater(){
    passes = 1;
    nTextures = 3;
    internalFormat = GL_RGB;
    
    threshold = 0.3;
    blurFade = 0.0005;
    density = 1.0;
    
    fragmentShader = "#version 120\n \
    #extension GL_ARB_texture_rectangle : enable\n \
    \
    uniform sampler2DRect backbuffer;\
    uniform sampler2DRect tex0;\
    \
    uniform float damping;\
    \
    vec2 offset[4];\
    \
    void main(){\
        vec2 st = gl_TexCoord[0].st;\
        \
        offset[0] = vec2(-1.0, 0.0);\
        offset[1] = vec2(1.0, 0.0);\
        offset[2] = vec2(0.0, 1.0);\
        offset[3] = vec2(0.0, -1.0);\
        \
        vec3 sum = vec3(0.0, 0.0, 0.0);\
        \
        for (int i = 0; i < 4 ; i++){\
            sum += texture2DRect(tex0, st + offset[i]).rgb;\
        }\
        \
        sum = (sum / 2.0) - texture2DRect(backbuffer, st).rgb;\
        sum *= damping;\
        \
        gl_FragColor = vec4(sum, 1.0);\
    }";
    
    string fragmentRenderShader = "#version 120\n \
    #extension GL_ARB_texture_rectangle : enable\n \
    \
    uniform sampler2DRect tex0;\
    uniform sampler2DRect tex1;\
    \
    void main(){\
        vec2 st = gl_TexCoord[0].st;\
        \
        float offsetX = texture2DRect(tex1, st + vec2(-1.0, 0.0)).r - texture2DRect(tex1, st + vec2(1.0, 0.0)).r;\
        float offsetY = texture2DRect(tex1, st + vec2(0.0,- 1.0)).r - texture2DRect(tex1, st + vec2(0.0, 1.0)).r;\
        \
        float shading = offsetX;\
        \
        vec3 pixel = texture2DRect(tex0, st + vec2(offsetX, offsetY)).rgb;\
        \
        pixel.r += shading;\
        pixel.g += shading;\
        pixel.b += shading;\
        \
        gl_FragColor.rgb =  pixel;\
        gl_FragColor.a = 1.0;\
    }";
    
    string fragmentBlurShader = "#version 120\n \
    #extension GL_ARB_texture_rectangle : enable\n \
    \
    float kernel[9];\
    \
    uniform sampler2DRect tex1;\
    uniform float fade_const;\
    \
    vec2 offset[9];\
    \
    void main(void){\
        vec2  st = gl_TexCoord[0].st;\
        vec4 sum = vec4(0.0);\
        \
        offset[0] = vec2(-1.0, -1.0);\
        offset[1] = vec2(0.0, -1.0);\
        offset[2] = vec2(1.0, -1.0);\
        \
        offset[3] = vec2(-1.0, 0.0);\
        offset[4] = vec2(0.0, 0.0);\
        offset[5] = vec2(1.0, 0.0);\
        \
        offset[6] = vec2(-1.0, 1.0);\
        offset[7] = vec2(0.0, 1.0);\
        offset[8] = vec2(1.0, 1.0);\
        \
        kernel[0] = 1.0/16.0;   kernel[1] = 2.0/16.0;   kernel[2] = 1.0/16.0;\
        kernel[3] = 2.0/16.0;   kernel[4] = 4.0/16.0;   kernel[5] = 2.0/16.0;\
        kernel[6] = 1.0/16.0;   kernel[7] = 2.0/16.0;   kernel[8] = 1.0/16.0;\
        \
        int i = 0;\
        for (i = 0; i < 4; i++){\
            vec4 tmp = texture2DRect(tex1, st + offset[i]);\
            sum += tmp * kernel[i];\
        }\
        \
        for (i = 5; i < 9; i++){\
            vec4 tmp = texture2DRect(tex1, st + offset[i]);\
            sum += tmp * kernel[i];\
        }\
        \
        vec4 color0 = texture2DRect(tex1, st + offset[4]);\
            sum += color0 * kernel[4];\
        \
        gl_FragColor = (1.0 - fade_const) * color0 +  fade_const * vec4(sum.rgb, color0.a);\
    }";
    
    shader.unload();
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    shader.linkProgram();
    
    blurShader.unload();
    blurShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentBlurShader);
    blurShader.linkProgram();
    
    renderShader.unload();
    renderShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentRenderShader);
    renderShader.linkProgram();
}

ofxWater& ofxWater::loadBackground(string file){
    ofImage backgroundImage;
    backgroundImage.loadImage(file); 
    allocate(backgroundImage.getWidth(), backgroundImage.getHeight());
    
    textures[0].begin();
    backgroundImage.draw(0,0);
    textures[0].end();
    
    return * this;
}

ofxWater& ofxWater::linkBackground(ofTexture * _backText){
    textures[0].begin();
    _backText->draw(0,0);
    textures[0].end();
    
    return * this;
}

void ofxWater::begin() {
    ofPushStyle();
    ofPushMatrix();
    pingPong.src->begin();	
}

void ofxWater::end() {
    pingPong.src->end();
    ofPopMatrix();
    ofPopStyle();
}

void ofxWater::update(){
    // Calculate the difference between buffers and spread the waving    
    //updateFbo.begin();
    textures[1].begin();
    ofClear(0);
    shader.begin();
    shader.setUniformTexture("backbuffer", pingPong.dst->getTextureReference(), 0);
    shader.setUniformTexture("tex0", pingPong.src->getTextureReference(), 1);
    shader.setUniform1f("damping", (float)density );
    renderFrame();
    shader.end();
    //updateFbo.end();
    textures[1].end();
    
    // Blur the waving in order to make it smooth
    pingPong.dst->begin();
    blurShader.begin();
    blurShader.setUniformTexture("tex1", textures[1].getTextureReference(), 0);
    blurShader.setUniform1f("fade_const", (float)(blurFade));
    renderFrame();
    blurShader.end();
    pingPong.dst->end();
    
    // Use the buffer as a bumpmap to morph the surface of the background texture
    textures[2].begin();
    ofClear(0);
    renderShader.begin();
    renderShader.setUniformTexture("tex0", textures[0].getTextureReference(), 0);
    renderShader.setUniformTexture("tex1", textures[1].getTextureReference(), 1);
    renderFrame();
    renderShader.end();
    textures[2].end();
    
    // Switch buffers
    pingPong.swap();
}

void ofxWater::draw(int x, int y, float _width, float _height){
    if (_width == -1) _width = width;
    if (_height == -1) _height = height;
    textures[2].draw(x,y, _width, _height);
}

