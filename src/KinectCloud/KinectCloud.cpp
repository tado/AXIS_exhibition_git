#include "KinectCloud.h"
#include "testApp.h"

void KinectCloud::setup() {
	//画面設定
	ofBackgroundHex(0x000000);
	
	//Mesh 初期設定
	rotate.x = rotate.y = rotate.z = 0;
	scale = 2.5;
	
	//cam.setFarClip(1000);
	//cam.setNearClip(0.1);
	
	easyCam.setNearClip(10.0);
	
	// テクスチャー画像を設定
	//ofDisableArbTex();
	//texture.loadImage("particle32.png");
	
	// GUI
	gui.setup("ofxKinect Point Cloud", 0, 0, 400, ofGetHeight());
	gui.addPanel("Kinect Params", 1);
	gui.setWhichPanel(0);
	gui.setWhichColumn(0);
	gui.addToggle("Pick Color", "pick_color", false);
	gui.addToggle("Blendmode Add", "blend_add", false);
	gui.addToggle("Point smooth", "point_smooth", false);
	gui.addSlider("Kinect Tilt Angle", "angle", 0, -40, 40, true);
	gui.addSlider("Point size", "point_size", 5, 1, 40, false);
	gui.addSlider("Step size", "step", 2, 1, 10, true);
	gui.addSlider("Z threshold", "thresh", 1000, 0, 4000, true);
	gui.addSlider("Mesh scale", "scale", 1.0, 0.5, 4.0, false);
	gui.addSlider("Mesh posz", "posz", 0, -2000, 2000, true);
	//gui.addSlider("Color Hue", "color_h", 1.0, 0, 4.0, false);
	gui.addSlider("Color Saturation", "color_s", 1.0, 0, 2.0, false);
	gui.addSlider("Color brightness", "color_b", 1.0, 0, 2.0, false);
	//gui.addSlider("Mesh rotate X", "rotx", 0, -100, 100, false);
	//gui.addSlider("Mesh rotate Y", "roty", 0, -100, 100, false);
	gui.addSlider("Record buffer size", "rec_size", 10, 1, 20, true);
	gui.addSlider("Record step", "rec_step", 10, 1, 20, true);
	//gui.addSlider("Point cloud random", "cloud_rand", 0.0, 0.0, 2.0, false);
	//gui.addSlider("Point cloud alpha", "cloud_alpha", 0.2, 0.0, 1.0, false);
	
	gui.addPanel("Kinect Images", 1);
	gui.setWhichPanel(1);
	gui.setWhichColumn(0);
	//gui.addDrawableRect("Kinect Cam", &kinectCam, 320, 240);
	//gui.addDrawableRect("Kinect Depth", &kinectDepth, 320, 240);
	gui.loadSettings("controlPanelSettings.xml");
	gui.toggleView();
}


void KinectCloud::show() {
	easyCam.reset();
	rotate.x = 0;
	rotate.y = 0;
	scale = 2.5;
	
	//fbo.allocate(ofGetWidth(), ofGetHeight());
	glow.allocate(ofGetWidth(), ofGetHeight());
	glow.setPasses(3);
	glow.setRadius(140);
	
	//blur.allocate(ofGetWidth(), ofGetHeight());
	//blur.setPasses(6);
	//blur.setRadius(20);
}

void KinectCloud::update() {
	
	// Kinect更新
	((testApp*)ofGetAppPtr())->kinect.update();
	((testApp*)ofGetAppPtr())->kinect.setCameraTiltAngle(gui.getValueI("angle"));
	
	
	if (((testApp*)ofGetAppPtr())->kinect.isFrameNew()) {
		
		kinectCam.setFromPixels(((testApp*)ofGetAppPtr())->kinect.getPixels(), 
								((testApp*)ofGetAppPtr())->kinectWidth, 
								((testApp*)ofGetAppPtr())->kinectHeight,
								OF_IMAGE_COLOR);
		
		kinectDepth.setFromPixels(((testApp*)ofGetAppPtr())->kinect.getDepthPixels(), 
								  ((testApp*)ofGetAppPtr())->kinectWidth, 
								  ((testApp*)ofGetAppPtr())->kinectHeight,
								  OF_IMAGE_GRAYSCALE);
	}
   
    //ポイントクラウド記録
	recordPointCloud();
    
    //Glow
	glow.begin();
    
    //ポイントクラウドの描画
	drawPointCloud();

    glow.end();
	glow.update();
	
	//GUI
	gui.update();
}

void KinectCloud::draw() {

    //Glow効果をDraw
	glow.draw();

	//GUIf
	ofPushStyle();
	gui.draw();
	ofPopStyle();
}

void KinectCloud::recordPointCloud() {
	// 画面の幅と高さ
	int w = ((testApp*)ofGetAppPtr())->kinectWidth;
	int h = ((testApp*)ofGetAppPtr())->kinectHeight;
	// メッシュを生成
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	
	// ブレンド設定
	if (gui.getValueB("blend_add")) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
	}
	
	// 設定した間隔で、画面の深度情報と色を取得してメッシュの頂点に設定
	int step = gui.getValueI("step");
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(((testApp*)ofGetAppPtr())->kinect.getDistanceAt(x, y) < gui.getValueI("thresh")) {
				if (gui.getValueB("pick_color")) {
					ofFloatColor col = ((testApp*)ofGetAppPtr())->kinect.getColorAt(x, y);
					//col = ofFloatColor(col.r, col.g, col.b, gui.getValueF("cloud_alpha"));
					
					//HSBを調整
					ofColor tmpColor = ofColor(col.r * 255, col.g * 255, col.b * 255);
					float hue = tmpColor.getHue();
					float sat = tmpColor.getSaturation() * gui.getValueF("color_s");
					float br = tmpColor.getBrightness() * gui.getValueF("color_b");
					tmpColor.setHsb(hue, sat, br);
					col.r = (float)tmpColor.r / 255.0;
					col.g = (float)tmpColor.g / 255.0;
					col.b = (float)tmpColor.b / 255.0;
					col.a = 0.9;
					mesh.addColor(col);
				} else {
					mesh.addColor(ofFloatColor(255,255,255));
				}
				ofVec3f loc = ((testApp*)ofGetAppPtr())->kinect.getWorldCoordinateAt(x, y);
				//float rand = gui.getValueF("cloud_rand");
				//loc = ofVec3f(loc.x + ofRandom(-rand, rand), loc.y + ofRandom(-rand, rand), loc.z);
				//loc = ofVec3f(loc.x, loc.y, loc.z + ofRandom(-rand, rand));
				mesh.addVertex(loc);
			}
		}
	}
	
	//dequeに追加
	meshs.push_back(mesh);
	int rec_step = gui.getValueF("rec_step");
	int rec_size = gui.getValueF("rec_size");
	if (meshs.size() > rec_size * rec_step) {
		meshs.pop_front();
	}
}

void KinectCloud::drawPointCloud() {
	
	// ドラッグで視線を変更できるように(ofEasyCam)
	easyCam.begin();
	
	// メッシュの頂点を描画
	ofPushMatrix();
	ofScale(scale, -scale, -scale);
		
	ofRotateX(rotate.x);
	ofRotateY(-rotate.y);
	ofRotateZ(180);
		
	ofTranslate(0, 0, gui.getValueI("posz"));
	
	glEnable(GL_DEPTH_TEST);
	
	if (gui.getValueB("point_smooth")) {
		glEnable(GL_POINT_SMOOTH);
	} else {
		glDisable(GL_POINT_SMOOTH);
	}
	
	//glEnable(GL_LINE_SMOOTH);
	//ofEnableAlphaBlending();
	
	// パーティクルのZ軸の位置によって大きさを変化させる
	// static GLfloat distance[] = { 0.0, 0.0, 1.0 }; 
	// glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance);
	// glPointSize(gui.getValueF("point_size") * 1000);
	
	glPointSize(gui.getValueF("point_size"));
	
	if (gui.getValueB("blend_add")) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
	} else {
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	}
	
	// メッシュ描画
	ofSetLineWidth(0);
	for (int i = 0; i < meshs.size(); i += gui.getValueI("rec_step") ) {
		meshs[i].drawVertices();
	}
	meshs[meshs.size()-1].drawVertices();
	
	//glDisable(GL_POINT_SMOOTH);

	glDisable(GL_DEPTH_TEST);
	ofDisableBlendMode();
	ofDisableAlphaBlending();
	ofPopMatrix();
	
	easyCam.end();
}

void KinectCloud::moveCam(int x, int y) {
	rotate.x = -y;
	rotate.y = x;
}

void KinectCloud::hide() {
	
}

void KinectCloud::exit() {
	
}

void KinectCloud::keyPressed (int key) {
	if (key == ' ') {
		gui.toggleView();
	}
}

void KinectCloud::mouseDragged(int x, int y, int button){
	gui.mouseDragged(x, y, button);
}

void KinectCloud::mousePressed(int x, int y, int button){
	gui.mousePressed(x, y, button);
}

void KinectCloud::mouseReleased(int x, int y, int button){
	gui.mouseReleased();
}

void KinectCloud::windowResized(int w, int h)
{}
