#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(60);
		
	//Kinect初期化
	kinect.setRegistration(true);
	kinect.init();
	kinect.open();
	kinectWidth = kinect.width;
	kinectHeight = kinect.height;
	
	//ofxMultipleAppの初期化
	ofxMultipleApp::init();
	
	//シーンの生成
	openingScene = new OpeningScene();
	faceExchange = new FaceExchange(); //FaceExchange
	kinectCloud = new KinectCloud(); //Kinect

	//それぞれのシーンをofxMultpleAppに追加
	ofxMultipleApp::addApp(openingScene);
	ofxMultipleApp::addApp(faceExchange);
	ofxMultipleApp::addApp(kinectCloud);

	//最初に表示するシーンをFaceTrackerに
	currentApp = openingScene;
	currentApp->show();
	openingScene->enable = true;

//	//フルスクリーン表示
//	ofSetFullscreen(true);
	
	
	//OSC
	receiver.setup( PORT );
}

//--------------------------------------------------------------
void testApp::update(){


	//OSCメッセージを受信
	while( receiver.hasWaitingMessages() ) {
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		if ( m.getAddress() == "/app/init" ) {
			int scene = m.getArgAsInt32(0);
			changeScene(scene);
			cout << "receive OSC" << m.getAddress() << ":" << m.getArgAsInt32(0) << endl;
		}
		
		//FaceExchange 顔交換
		if (m.getAddress() == "/face/number") {
			faceExchange->num_displayFace = m.getArgAsInt32(0); 
			faceExchange->imgTracker.reset();

            printf("faceNumber: %d\n",m.getArgAsInt32(0));
        }
		
		//KinectCloud関連のOSC
		if (currentApp == kinectCloud) {

			if ( m.getAddress() == "/touch/position") {
				kinectCloud->moveCam(m.getArgAsFloat(0), m.getArgAsFloat(1));
				cout << "receive OSC" << m.getAddress() << ":" << m.getArgAsInt32(0) << ", " << m.getArgAsInt32(1) << endl;
			}
			
			if ( m.getAddress() == "/touch/scale") {
				kinectCloud->scale = m.getArgAsFloat(0);
				cout << "receive OSC" << m.getAddress() << ":" << m.getArgAsInt32(0) << endl;
			}
			
		}
	}
}

void testApp::changeScene(int n) {

	//OSCから、シーンを切り替え
	switch (n) {
		case 0: 
			currentApp = faceExchange;
			currentApp->show();
			openingScene->enable = false;
			kinectCloud->enable = false;
			faceExchange->enable = true;
			break;
			
		case 1: 
			currentApp = kinectCloud;
			currentApp->show();
			openingScene->enable = false;
			kinectCloud->enable = true;
			faceExchange->enable = false;
			break;

		case 2: 
			currentApp = openingScene;
			currentApp->show();
			openingScene->enable = true;
			kinectCloud->enable = false;
			faceExchange->enable = false;
			break;
	}
}


//--------------------------------------------------------------
void testApp::draw(){
	//ofSetHexColor(0xffffff);
	//ofDrawBitmapString("[1] Opening Scene, [2] Face Exchange, [3] Kinect Cloud", 20, 20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	//キー入力によって、シーンを切り替え
	switch (key) {
		case '1': 
			// 1キーで、OpeningScene
			currentApp = openingScene;
			currentApp->show();
			openingScene->enable = true;
			kinectCloud->enable = false;
			faceExchange->enable = false;
			break;
			
		case '2': 
			// 2キーで、FaceExchange
			currentApp = faceExchange;
			currentApp->show();
			openingScene->enable = false;
			kinectCloud->enable = false;
			faceExchange->enable = true;
			break;
			
		case '3': 
			// 3キーで、KinectCloud
			currentApp = kinectCloud;
			currentApp->show();
			openingScene->enable = false;
			kinectCloud->enable = true;
			faceExchange->enable = false;
			break;
			
		case 'f':
			ofToggleFullscreen();
			break;
	}
}

void testApp::exit() {
	
	// Kinect終了
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
