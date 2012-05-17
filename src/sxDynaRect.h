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

    sxDynaRect();
    ~sxDynaRect();

    //Setup params and call frame buffer allocation
    void setup();
    void draw();
    void update();


    //Other sub functions
    void setSize(int width, int height);
    void setPos(ofPoint pos);
    ofPoint getPos();
    bool isActive();
    void bang();
    void setColorOn(int color);
    void setColorOff(int color);
    void setDuration(int duration);
    void setId(int id);
    void loadMovie();

    int _width;
    int _height;
    ofPoint _pos;
    int _colorOn;
    int _colorOff;
    int     _duration;
    int     _startTime;
    int _id;

    ofVideoPlayer _movie;
};

#endif
