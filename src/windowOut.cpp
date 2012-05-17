//
//  windowOut.cp
//  BallRoomPlayer
//
//  Created by Gabriel Delattre on 17/05/12.
//  Copyright (c) 2012 Soixante circuits. All rights reserved.
//

#include "windowOut.h"
#define PANEL_TEX	0x00
#define PANEL_VER	0x01
#define PANEL_NONE	0x02


WindowOut::WindowOut() {
    _buffer_screen = NULL;
  }
  
WindowOut::WindowOut(ofxMpplrScreen* buf, string type = string("default")) {
    _type_of_display = type;
    _buffer_screen = buf;
}
  
void WindowOut::draw(ofxFenster* fenster) {
  if(_type_of_display == "mapper"){
    _buffer_screen->draw(0, 0, fenster->getWidth(), fenster->getHeight());
  }else {
    _buffer_screen->drawOutPut(0, 0, fenster->getWidth(), fenster->getHeight());
  }
    
}
  
void WindowOut::mouseMoved(int x, int y) {
  if(_type_of_display == "mapper"){
    _buffer_screen->setMode(PANEL_VER);
    _buffer_screen->setMouseXY(x, y);
  }else {
    _buffer_screen->setMode(PANEL_NONE);
  }
}
  
void WindowOut::keyReleased(int key, ofxFenster* fenster){
    if(key=='f'){
      fenster->toggleFullscreen();
    }
}
  
void WindowOut::assignMpplrScreenBuffer(ofxMpplrScreen *buffer){
    _buffer_screen = buffer;
}


