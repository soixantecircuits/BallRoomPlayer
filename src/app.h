#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxMpplr.h"
#ifdef TARGET_OSX
#include "ofxAlphaVideoPlayer.h"
#endif
#include "sxDynaRect.h"
#include "ofxFensterManager.h"
#include <list>


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

    void bangStair(int stair);
    ofxFenster* _mapperView;
  
    ofxOscReceiver      _receiver;
    ofxOscSender        _sender;
    ofVideoPlayer       _movie;

#ifdef TARGET_OSX
    ofxAlphaVideoPlayer _incrust;
#else
    ofVideoPlayer       _incrust;
#endif
    ofImage _backgroundImage;
    ofxMpplrController  _controller_mapping;
    ofxMpplrScreen      _buffer_map;
  
    vector<sxDynaRect*>    _stairs;

		ofSoundPlayer  _bgSound;
		ofSoundPlayer  _bounceSound;
		ofSoundPlayer  _scoreSound;
};
