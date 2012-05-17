#include "app.h"
#include "ofxFensterManager.h"
#include "windowOut.h"

#define PANEL_TEX	0x00
#define PANEL_VER	0x01
#define PANEL_NONE	0x02

#define PORT 12444



//--------------------------------------------------------------
void App::setup(){
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
  
  //Setup OSC
  _receiver.setup( PORT);  
  
	//ofxMpplr Initialize
	_buffer_map.setup(1280, 960);
	_controller_mapping.setup(&_buffer_map);
	_buffer_map.LoadXML(0);
  
  ofxFenster* win=ofxFensterManager::get()->createFenster(1280, 0, 1280, 960, OF_WINDOW);
  win->setWindowTitle("Ballroom render view");
  win->addListener(new WindowOut(&_buffer_map, "default"));
  win->setBackgroundColor(0,0, 0);

  ofxFenster* win2=ofxFensterManager::get()->createFenster(1280, 0, 1280, 960, OF_WINDOW);
  win2->setWindowTitle("Ballroom mapper view");
  win2->addListener(new WindowOut(&_buffer_map, "mapper"));
  win2->setBackgroundColor(0,0, 0);
  
  _movie.loadMovie("movies/anim_marche.mov");
  _movie.setVolume(0);
  _movie.play();
  _movie.setLoopState(OF_LOOP_NORMAL);
  
  #ifdef TARGET_LINUX
    _incrust.setPixelFormat(OF_PIXELS_BGRA);
  #endif
  
  
  
  for (int i = 0; i < 960; i+=50){
    _steps.push_back(sxDynaRect(0, 960-i, 1280, 50, ofColor(255,0,0), ofColor(255,0,255)) );
  }

  
  
}

//--------------------------------------------------------------
void App::checkForOscMessages(){
  while (_receiver.hasWaitingMessages()){
    ofxOscMessage m;
    _receiver.getNextMessage(&m);
    if (m.getAddress() == "/ballroom/bounce/"){
      int stair = m.getArgAsInt32(0);
      cout << "A ball hit stair #" << stair << endl;
      setSteps(stair);
    }
  }
}

//--------------------------------------------------------------
void App::update(){
  checkForOscMessages();
  _movie.update();
  list<sxDynaRect>::iterator it =_steps.begin();
  while(it!=_steps.end()) {
    it->update();
    ++it;
  }

}

void App::setSteps(int num){
  list<sxDynaRect>::iterator it =_steps.begin();
  int cpt = 0;
  while(it!=_steps.end()) {
    if(cpt == num){
      it->setActiveColor(it->_colorOn);
      it->setActive(true);
    }
    ++it;
    cpt++;
  }

}

//--------------------------------------------------------------
void App::draw(){
	
	//You can draw anything between Begin(); and End();
	_buffer_map.Begin();
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //ofEnableAlphaBlending();
    //ofDisableAlphaBlending();
    list<sxDynaRect>::iterator it =_steps.begin();
    while(it!=_steps.end()) {
      it->draw();
      ++it;
    }
  
  /*ofSetHexColor(0xFF0000);
	for (int i = 0;i < 480;i+=30){
		ofRect(0, ofGetFrameNum()%30+i, 640, 10);
	}*/


	_buffer_map.End();
		
	ofSetHexColor(0xFFFFFF);
	ofBackground(0, 0, 0);

	//Monitor out control (deform)
	//_buffer_map.draw(640, 0, 640, 480);
	
	//Source out control
  /**************************************/
  //Need to be fixed, if an other coordinates are given, the display is offset
  /**************************************/
  
	_buffer_map.drawBuffer(0, 0, 800, 600);
	
	//ofxMpplrController
	_controller_mapping.draw(50, 650);
	
}

//--------------------------------------------------------------
void App::keyPressed(int key){
	if (key == 'f'){
    ofxFensterManager::get()->getPrimaryWindow()->toggleFullscreen();
  }
  if  (key == 'h'){
    cout << "Active" << endl;
    setSteps(ofRandom(17));  
  }
  if  (key == 'j'){
    cout << "All Active" << endl;
    list<sxDynaRect>::iterator it =_steps.begin();
    while(it!=_steps.end()) {
      it->setActive(true,5000,ofColor(ofRandom(255), ofRandom(255), ofRandom(255)));
      ++it;
    }
  }
}

//--------------------------------------------------------------
void App::keyReleased(int key){

}

//--------------------------------------------------------------
void App::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void App::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void App::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void App::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void App::windowResized(int w, int h){

}

//--------------------------------------------------------------
void App::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void App::dragEvent(ofDragInfo dragInfo){ 

}
