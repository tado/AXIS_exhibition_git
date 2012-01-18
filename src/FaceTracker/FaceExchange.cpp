#include "FaceExchange.h"
#include "testApp.h"

const int num_face = 15;

using namespace ofxCv;

void FaceExchange::setup() {
	
    //setup for face image	
	
	num_displayFace = 0;
    faceImages = new ofImage [num_face];
    
    for (int i = 0; i < num_face; i++) {
        
        char * imageName = new char [255];
        sprintf(imageName,"images/%03d.png",i+1);
        
		faceImages[i].allocate(640, 480, OF_IMAGE_COLOR);
        faceImages[i].loadImage(imageName);
        
        delete [] imageName;
        
    }
	 
	tracker.setup();
    imgTracker.setup();
    imgTracker.update(toCv(faceImages[num_displayFace]));
	 
    findFace = false;
}

void FaceExchange::show(){
	screenHeight = ofGetHeight();
	screenWidth = screenHeight / 3 * 4;
	screenLoc.x = (ofGetWidth() - screenWidth) / 2;
	screenLoc.y = 0;
}

void FaceExchange::update() {
	
    ((testApp*) ofGetAppPtr())->kinect.update();
	
    if (((testApp*) ofGetAppPtr())->kinect.isFrameNew()) {
        kinectCam.setFromPixels(((testApp*) ofGetAppPtr())->kinect.getPixels(), 
								((testApp*) ofGetAppPtr())->kinectWidth, 
								((testApp*) ofGetAppPtr())->kinectHeight, 
								OF_IMAGE_COLOR);
		tracker.update(toCv(kinectCam));
        imgTracker.update(toCv(faceImages[num_displayFace]));
    }
    
    if(tracker.getFound()) {
		findFace = true;
	} else {
		findFace = false;		
	}
		
	if(imgTracker.getFound()){
		findImage = true;	
	} else {
		findImage = false;	
	}
    
    if(findFace && findImage){
        
        ofMesh objectMesh = tracker.getObjectMesh();
        imgMesh = imgTracker.getObjectMesh();
        
        for (int i = 0; i < objectMesh.getNumVertices() ; i++) {
            imgMesh.setVertex(i, objectMesh.getVertex(i));
        }
    }
}

void FaceExchange::draw() {
    
	ofSetColor(255);

	ofSetRectMode(OF_RECTMODE_CORNER);
    ((testApp*) ofGetAppPtr())->kinect.draw(screenLoc.x, screenLoc.y, screenWidth, screenHeight);
    
    //ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
	
	if (findFace) {
        ofSetupScreenOrtho(ofGetWidth(), ofGetHeight(), OF_ORIENTATION_DEFAULT, true, -1000, 1000);
        
        ofVec2f positon = tracker.getPosition();
        float scale = tracker.getScale();
        ofVec3f orientation = tracker.getOrientation();
        
        ofPushMatrix();
        
        //use kinect
		ofTranslate(positon.x * (float)screenWidth / ((testApp*) ofGetAppPtr())->kinectWidth + screenLoc.x, 
					positon.y * (float)screenHeight / ((testApp*) ofGetAppPtr())->kinectHeight + screenLoc.y);
		
		ofScale(scale * (float)screenWidth / ((testApp*) ofGetAppPtr())->kinectWidth, 
				scale * (float)screenHeight / ((testApp*) ofGetAppPtr())->kinectHeight);
		
        ofRotateX(orientation.x * 45.0f);
        ofRotateY(orientation.y * 45.0f);
        ofRotateZ(orientation.z * 45.0f);
        
        ofPushStyle();
        ofEnableBlendMode(OF_BLENDMODE_ALPHA);
        ofSetColor(255, 255, 255, 150);
        faceImages[num_displayFace].getTextureReference().bind();
		imgMesh.draw();
        faceImages[num_displayFace].getTextureReference().unbind();
        ofDisableBlendMode();
        ofPopStyle();
        ofPopMatrix();
    }
}

void FaceExchange::hide(){
	
}

void FaceExchange::keyPressed(int key) {
	if(key == 'r') {

		tracker.reset();
        imgTracker.reset();
        imgTracker.update(toCv(faceImages[num_displayFace]));
        
	}
}