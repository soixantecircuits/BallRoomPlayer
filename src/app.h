#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxMpplr.h"
#include "ofxFensterManager.h"
#ifdef TARGET_OSX
#include "ofxAlphaVideoPlayer.h"
#endif
#include "sxDynaRect.h"
#include <list>


class boxWindow: public ofxFensterListener {
  public:
    boxWindow() {
      _rotX = ofRandom(-20, 20);
      _rotY = ofRandom(-10, 10);
      _buf = NULL;
    }

    boxWindow(ofxMpplrScreen* buf) {
      _rotX = ofRandom(-20, 20);
      _rotY = ofRandom(-10, 10);
      _buf = buf;
    }

    void draw(ofxFenster* f) {
      _buf->drawOutPut(0, 0, f->getWidth(), f->getHeight());
    }

    void mouseMoved(int x, int y) {
      _rotY = ofMap(x, 0, ofGetWidth(), -20, 20);
      _rotX = ofMap(y, 0, ofGetHeight(), 60, -60);
    }

    void keyReleased(int key, ofxFenster* f){
      if(key=='f'){
        f->toggleFullscreen();
      }
    }

    void assignMpplrScreenBuffer(ofxMpplrScreen *buffer){
      _buf = buffer;
    }

    ofxMpplrScreen* _buf;
    float _rotX;
    float _rotY;
};

class App : public ofBaseApp{

  public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);


    void checkForOscMessages();
    ofxOscReceiver            receiver;
    ofVideoPlayer _Movie;

#ifdef TARGET_OSX
    ofxAlphaVideoPlayer      _Incrust;
#else
    ofVideoPlayer _Incrust;
#endif
    ofxMpplrController con;
    ofxMpplrScreen buf;
  
  list<sxDynaRect> _steps;

};
