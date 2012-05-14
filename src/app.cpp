#include "app.h"
#define PORT 12444



//--------------------------------------------------------------
void App::setup(){
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
  
  //Setup OSC
  receiver.setup( PORT);  
  
	//ofxMpplr Initialize
	buf.setup(1280, 960);
	con.setup(&buf);
	buf.LoadXML(0);
  
  ofxFenster* win=ofxFensterManager::get()->createFenster(1280, 0, 1280, 960, OF_WINDOW);
  win->addListener(new boxWindow(&buf));
  win->setBackgroundColor(0,0, 0);
  
  _Movie.loadMovie("movies/anim_marche.mov");
  _Movie.setVolume(0);
  _Movie.play();
  _Movie.setLoopState(OF_LOOP_NORMAL);
  
  #ifdef TARGET_LINUX
    _Incrust.setPixelFormat(OF_PIXELS_BGRA);
  #endif
  
  
  
  for (int i = 0; i < 960; i+=50){
    _steps.push_back(sxDynaRect(0, 960-i, 1280, 50, ofColor(255,0,0), ofColor(255,0,255)) );
  }

  
  
}

//--------------------------------------------------------------
void App::checkForOscMessages(){
  while (receiver.hasWaitingMessages()){
    ofxOscMessage m;
    receiver.getNextMessage(&m);
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
  _Movie.update();
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
	buf.Begin();
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


	buf.End();
		
	ofSetHexColor(0xFFFFFF);
	ofBackground(0, 0, 0);

	//Monitor out control (deform)
	buf.draw(640, 0, 640, 480);
	
	//Source out control
	buf.drawBuffer(0, 0, 640, 480);
	
	//ofxMpplrController
	con.draw(50, 500);
	
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
