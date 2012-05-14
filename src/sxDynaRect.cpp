//
//  sxDynaRect.cpp
//  BallRoomPlayer
//
//  Created by Gabriel Delattre on 13/05/12.
//  Copyright (c) 2012 Soixante circuits. All rights reserved.
//

#include <iostream>
#include "sxDynaRect.h"

sxDynaRect::sxDynaRect(){

}

sxDynaRect::sxDynaRect(GLfloat x, GLfloat y, GLfloat width, GLfloat height, ofColor colorOn, ofColor colorOff){
  _pos.x = x;
  _pos.y = y;
  _width = width;
  _height = height;
  _colorOn.set(colorOn);
  _colorOff.set(colorOff);
  _isActive = false;
  _activeColor.set(colorOff);
  _timeActive = 50;
}


void sxDynaRect::setup(int width, int height){
  _width = width;
  _height = height;
}


void sxDynaRect::update(){
  if(isActive()){
    //_activeColor.set(_colorOn);
    _timeActive--;
    
    if(_timeActive < 0){
      _timeActive = 50;
      _isActive = false;
    }
  }else{
    _activeColor.set(_colorOff);
  }
}

bool sxDynaRect::isActive(){
  return _isActive;
}

void sxDynaRect::draw(){
  ofSetColor(_activeColor);
 // ofSetHexColor(0xFF0000);

  //cout << "_activeColor.getHex() : " << _activeColor.getHex() << endl;
  ofRect(_pos, _width, _height);
}

void sxDynaRect::setPos(ofPoint pos2D){
  
}

ofPoint sxDynaRect::getPos(){
  
}

void sxDynaRect::setActive(bool state){
  _isActive = state;
}

void sxDynaRect::setActive(bool state, int count, ofColor aColor){
  _timeActive = count;
  _isActive = state;
  _activeColor.set(aColor);
}

void sxDynaRect::setActiveColor(ofColor aColor){
  _activeColor.set(aColor);
}

sxDynaRect::~sxDynaRect(){

}
