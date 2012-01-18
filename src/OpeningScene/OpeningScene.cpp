#include "OpeningScene.h"

void OpeningScene::setup(){
	openingImage.loadImage("images/opening.png");
}

void OpeningScene::update(){

}

void OpeningScene::draw(){
	ofBackgroundHex(0x000000);
	ofSetHexColor(0xFFFFFF);
	ofSetRectMode(OF_RECTMODE_CENTER);
	openingImage.draw(ofGetWidth()/2, ofGetHeight()/2);
	ofSetRectMode(OF_RECTMODE_CORNER);
}

void OpeningScene::show() {}
void OpeningScene::hide() {}
void OpeningScene::remove() {}
