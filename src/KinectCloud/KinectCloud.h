/*
 *
 * KinectCloud - show kinect pointcloud
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxMultipleApp.h"
#include "ofxKinect.h"
#include "ofxControlPanel.h"
#include "ofxOsc.h"
#include "ofxFX.h"

class KinectCloud : public ofxSubApp {
public:
    
    void setup();
    void update();
    void draw();
    void exit();
	void show();
	void hide();
    
	void recordPointCloud();
    void drawPointCloud();
	void moveCam(int x, int y);
    
    void keyPressed (int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);

	ofEasyCam easyCam;
    ofxControlPanel gui;
    ofImage kinectCam;
    ofImage kinectDepth;
	int recordCount;
	deque<ofVboMesh> meshs;
	ofVec3f rotate;
	float scale;
	
	//ofxFX
	ofxGlow glow;
	//ofFbo fbo;
};
