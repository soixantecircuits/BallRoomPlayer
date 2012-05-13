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
}


void sxDynaRect::setup(int width, int height){
  _width = width;
  _height = height;
}


void sxDynaRect::update(){
  if(isActive()){
    
  }
}

bool sxDynaRect::isActive(){
  return _isActive;
}

void sxDynaRect::draw(){
  ofRect(_pos, _width, _height);
}

void sxDynaRect::setPos(ofPoint pos2D){
  
}

ofPoint sxDynaRect::getPos(){
  
}

void sxDynaRect::setActive(bool state){
  _isActive = state;
}



sxDynaRect::~sxDynaRect(){

}
