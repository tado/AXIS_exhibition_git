#pragma once

#include "ofMain.h"
#include "ofxMultipleApp.h"
#include "ofxKinect.h"
#include "ofxOsc.h"

#include "OpeningScene.h"
#include "FaceExchange.h"
#include "KinectCloud.h"

#define PORT 7000

class testApp : public ofBaseApp {
	
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	void changeScene(int n);

	ofxSubApp *currentApp;
	
	//それぞれのシーンのインスタンス
	OpeningScene *openingScene;
	FaceExchange *faceExchange;
	KinectCloud *kinectCloud;
	
	
	//共用するKinectのインスタンス
	ofxKinect kinect;
	float kinectWidth, kinectHeight;
	
	ofxOscReceiver receiver;
};
