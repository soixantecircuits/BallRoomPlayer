//
//  sxDynaRect.h
//  BallRoomPlayer
//
//  Created by Gabriel Delattre on 13/05/12.
//  Copyright (c) 2012 Soixante circuits. All rights reserved.
//

#ifndef BallRoomPlayer_sxDynaRect_h
#define BallRoomPlayer_sxDynaRect_h

#pragma once
#include "ofMain.h"


class sxDynaRect{
public:
	
  bool    _isActive;
  GLfloat _width;
  GLfloat _height;
  ofPoint _pos;
  
  sxDynaRect();
  sxDynaRect(GLfloat x, GLfloat y, GLfloat width, GLfloat height);
  ~sxDynaRect();
  
	//Setup params and call frame buffer allocation
	void setup(int width,int height);
  void draw();
	void draw(float x,float y);
  
	//Other sub functions
	void setPos(ofPoint pos2D);
	ofPoint getPos();
	void setActive();
  void update();
  
  	
};

#endif
