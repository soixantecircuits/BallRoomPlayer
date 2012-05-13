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

sxDynaRect::sxDynaRect(GLfloat x, GLfloat y, GLfloat width, GLfloat height){
  _pos.x = x;
  _pos.y = y;
  _width = width;
  _height = height;
  
}


void sxDynaRect::setup(int width, int height){
  _width = width;
  _height = height;
}

void sxDynaRect::draw()

sxDynaRect::~sxDynaRect(){

}
