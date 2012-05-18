#include "app.h"
#include "windowOut.h"

#define PANEL_TEX	0x00
#define PANEL_VER	0x01
#define PANEL_NONE	0x02

#define PORT 12444
#define WIDTH 640
#define HEIGHT 480


//--------------------------------------------------------------
void App::setup(){
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
  
  //Setup OSC
  _receiver.setup( PORT);  
  
	//ofxMpplr Initialize
	_buffer_map.setup(WIDTH, HEIGHT);
	_controller_mapping.setup(&_buffer_map);
	//_buffer_map.LoadXML(0);
  
  ofxFenster* win=ofxFensterManager::get()->createFenster(WIDTH, 0, WIDTH, HEIGHT, OF_WINDOW);
  win->setWindowTitle("Ballroom render view");
  win->addListener(new WindowOut(&_buffer_map, "default"));
  win->setBackgroundColor(0,0, 0);

  _mapperView = ofxFensterManager::get()->createFenster(400, 0, WIDTH, HEIGHT, OF_WINDOW);
  _mapperView->setWindowTitle("Ballroom mapper view");
  _mapperView->addListener(new WindowOut(&_buffer_map, "mapper"));
  _mapperView->setBackgroundColor(0,0, 0);
  
  _movie.loadMovie("movies/anim_marche.mov");
  _movie.setVolume(0);
  _movie.play();
  _movie.setLoopState(OF_LOOP_NORMAL);
  
  #ifdef TARGET_LINUX
    _incrust.setPixelFormat(OF_PIXELS_BGRA);
  #endif
  
  
  _backgroundImage.loadImage("data/bgImage.jpg");
  
  int stairWidth = WIDTH;
  float stairHeight = HEIGHT/17.;

  _stairs.resize(17);
  for (int i = 0; i < 17; i++){
    sxDynaRect *stair = new sxDynaRect();
    stair->setup();
    stair->setPos(ofPoint(0, (16-i)*stairHeight));
    stair->setSize(stairWidth, stairHeight);
    stair->setId(i);
    int rainbow = 0;
    if (i % 4 == 0){
      rainbow = 0x00ffff;
    }
    if (i % 4 == 1){
      rainbow = 0xff0000;
    }
    if (i % 4 == 2){
      rainbow = 0xffff00;
    }
    if (i % 4 == 3){
      rainbow = 0xff0099;
    }
    stair->setColorOn(rainbow);
    stair->setColorOff(0x000000);
    /*
    if ( i < 4){
      stair->setColorOff(0xffff00);
    } else {
      stair->setColorOff(0x000000);
    }
    */

    //stair->loadMovie();
    _stairs[i] = stair;
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
      _stairs[stair]->bang();
    }
  }
}

//--------------------------------------------------------------
void App::update(){
  checkForOscMessages();
  _movie.update();
  for (unsigned int i = 0; i < _stairs.size(); i++){
    _stairs[i]->update();
  }
  //resize window
  _mapperView->setWindowShape(_mapperView->getHeight()/HEIGHT*WIDTH, _mapperView->getHeight());
}

//--------------------------------------------------------------
void App::draw(){
	
	//You can draw anything between Begin(); and End();
	_buffer_map.Begin();
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //ofEnableAlphaBlending();
    //ofDisableAlphaBlending();
    _backgroundImage.draw(0, 0, WIDTH, HEIGHT);
    for (unsigned int i = 0; i < _stairs.size(); i++){
      _stairs[i]->draw();
    }
    //ofSetHexColor(0xFF0099);
    //ofRect(200, 200, 200, 200);
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
  if  (key == 'j'){
    cout << "All Active" << endl;
    for (unsigned int i = 0; i < _stairs.size(); i++){
      //_stairs[i]->setDuration(10000);
      _stairs[i]->bang();
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
