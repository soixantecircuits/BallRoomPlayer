//
//  sxDynaRect.cpp
//  BallRoomPlayer
//
//  Created by Gabriel Delattre on 13/05/12.
//  Copyright (c) 2012 Soixante circuits. All rights reserved.
//

#include <iostream>
#include "sxDynaRect.h"

//--------------------------------------------------------------
sxDynaRect::sxDynaRect(){

}

//--------------------------------------------------------------
void sxDynaRect::setup(){
  _startTime = 0;
  _duration = 500;
  _colorOn = 0xFF0000;
  _colorOff = 0xFFFF00;
}

//--------------------------------------------------------------
void sxDynaRect::setSize(int width, int height){
  _width = width;
  _height = height;
}

//--------------------------------------------------------------
void sxDynaRect::update(){
  if (_movie.isLoaded()){
    _movie.update();
  }
}

//--------------------------------------------------------------
void sxDynaRect::loadMovie(){
  int id = (_id % 4) + 1;
  string path = "movies/marches/" + ofToString(id) + ".mov";
  if ( id < 10) {
    path = "movies/marches/0" + ofToString(id) + ".mov";
  }
  cout << "loading file: " << path << endl;
  _movie.loadMovie(path);
  //_movie.setLoopState(OF_LOOP_NORMAL);
  _movie.setLoopState(OF_LOOP_NONE);
  _movie.play();
  _movie.setPaused(true);
}

//--------------------------------------------------------------
bool sxDynaRect::isActive(){
  return ofGetElapsedTimeMillis() - _startTime < _duration;
}

//--------------------------------------------------------------
void sxDynaRect::draw(){
  if (!_movie.isLoaded()){
    if (isActive()){
      ofSetHexColor(_colorOn);
    } else {
      ofSetHexColor(_colorOff);
    }
    ofRect(_pos.x, _pos.y, _width, _height);
  } else {
    _movie.draw(_pos.x, _pos.y, _width, _height);
  }
}

//--------------------------------------------------------------
void sxDynaRect::setId(int id){
  _id = id;
}

//--------------------------------------------------------------
void sxDynaRect::setPos(ofPoint pos){
  _pos = pos;
}

//--------------------------------------------------------------
ofPoint sxDynaRect::getPos(){
 return _pos;
}

//--------------------------------------------------------------
void sxDynaRect::setColorOn(int color){
  _colorOn =  color;
}

//--------------------------------------------------------------
void sxDynaRect::setColorOff(int color){
  _colorOff = color;
}

//--------------------------------------------------------------
void sxDynaRect::setDuration(int duration){
  _duration = duration;
}

//--------------------------------------------------------------
void sxDynaRect::bang(){
  if (_movie.isLoaded()){
    _movie.firstFrame();
    _movie.play();
  } else {
    _startTime = ofGetElapsedTimeMillis();
  }
}

//--------------------------------------------------------------
sxDynaRect::~sxDynaRect(){

}
