/*
 *
 * FaceExchange - exchange users face to another images
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxCv.h"
#include "ofxMultipleApp.h"
#include "ofxFaceTracker.h"
#include "ofxOsc.h"

//#define HOST "localhosts"
//#define RECIEVE_PORT 2012120

class FaceExchange : public ofxSubApp {
public:
	void setup();
    void show();
	void hide();
	void update();
	void draw();
	void keyPressed(int key);
	
    //deug 
	//ofVideoGrabber cam;
    
    ofxFaceTracker tracker;
    ofxFaceTracker imgTracker;
    
    ofImage kinectCam;
    ofImage * faceImages;
    
    ofEasyCam easyCam;
    ofxOscReceiver receiver;
    ofMesh imgMesh;
    int num_displayFace;
    bool findFace;
	bool findImage;
	int screenWidth, screenHeight;
	ofPoint screenLoc;
};

