/*
 *
 * Opening scene
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxMultipleApp.h"

class OpeningScene : public ofxSubApp {
	
	void setup();
	void update();
	void draw();
	
	void show();
    void hide();
    void remove();
	
	ofEasyCam cam;
	ofLight light;
	float rot;
	ofImage openingImage;
};
