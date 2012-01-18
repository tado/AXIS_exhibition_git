//
//  ofxSwapBuffer.h
//  emptyExample
//
//  Created by Patricio González Vivo on 13/01/12.
//  Copyright (c) 2012 PatricioGonzalezVivo.com. All rights reserved.
//

#ifndef OFXSWAPBUFFER
#define OFXSWAPBUFFER

class ofxSwapBuffer {
public:
    void allocate( int _width, int _height, int _internalformat = GL_RGBA, float _dissipation = 1.0f){
        for(int i = 0; i < 2; i++){
            FBOs[i].allocate(_width,_height, _internalformat );
            FBOs[i].begin();
            ofClear(0,255);
            FBOs[i].end();
        }
        
        flag = 0;
        swap();
        flag = 0;
        
        diss = _dissipation;
    }
    
    void swap(){
        src = &(FBOs[(flag)%2]);
        dst = &(FBOs[++(flag)%2]);
    }
    
    ofFbo& operator[]( int n ){ return FBOs[n];}
    
    ofFbo   *src;       // Source       ->  Ping
    ofFbo   *dst;       // Destination  ->  Pong
    
    float   diss;       // Dissipation
    
private:
    ofFbo   FBOs[2];    // Real addresses of ping/pong FBO´s  
    int     flag;       // Integer for making a quick swap
};

#endif
