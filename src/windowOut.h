//
//  windowOut.h
//  BallRoomPlayer
//
//  Created by Gabriel Delattre on 17/05/12.
//  Copyright (c) 2012 Soixante circuits. All rights reserved.
//

#ifndef BallRoomPlayer_windowOut_h
#define BallRoomPlayer_windowOut_h
#include "ofMain.h"
#include "ofxFensterManager.h"
#include "ofxMpplr.h"

class WindowOut: public ofxFensterListener {
  public:
    ofxMpplrScreen* _buffer_screen;
    string _type_of_display;
    WindowOut();
    WindowOut(ofxMpplrScreen* buf, string type);
    void draw(ofxFenster* fenster);
    void mouseMoved(int x, int y);  
    void keyReleased(int key, ofxFenster* fenster);
    void assignMpplrScreenBuffer(ofxMpplrScreen *buffer);
};


#endif
