#include "app.h"
#include "windowOut.h"

#define PANEL_TEX	0x00
#define PANEL_VER	0x01
#define PANEL_NONE	0x02

#define PORT 12444
#define WIDTH 640
#define HEIGHT 480

//#define DASHBOARDHOST "localhost"//"192.168.1.103"
#define DASHBOARDHOST "192.168.1.101"

//--------------------------------------------------------------
void App::setup(){
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
  
  //Setup OSC
  _receiver.setup( PORT);  
	// open an outgoing connection to HOST:PORT
	_sender.setup( DASHBOARDHOST, 12045 );
  
	//ofxMpplr Initialize
	_buffer_map.setup(WIDTH, HEIGHT);
	_controller_mapping.setup(&_buffer_map);
	_buffer_map.LoadXML(0);
  
  ofxFenster* win=ofxFensterManager::get()->createFenster(2000, 0, WIDTH, HEIGHT, OF_WINDOW);
  win->setWindowTitle("Ballroom render view");
  win->addListener(new WindowOut(&_buffer_map, "default"));
  win->setBackgroundColor(0,0, 0);
  //win->setFullscreen(true);

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
  _bgSound.loadSound("sounds/2.dub.wav");
  _bgSound.setVolume(0.75f);
  _bgSound.setMultiPlay(false);
  _bgSound.setLoop(true); 
  _bgSound.play(); 
  _bounceSound.loadSound("sounds/rebondBon3.wav");
  _bounceSound.setVolume(0.75f);
  _bounceSound.setMultiPlay(true);
  _bounceSound.setLoop(false); 
  _bounceNotGoodSound.loadSound("sounds/rebondPasBon.wav");
  _bounceNotGoodSound.setVolume(0.75f);
  _bounceNotGoodSound.setMultiPlay(false);
  _bounceNotGoodSound.setLoop(false); 
  _scoreSound.loadSound("sounds/score.wav");
  _scoreSound.setVolume(1.0f);
  _scoreSound.setMultiPlay(false);
  _scoreSound.setLoop(false); 
  _startSound.loadSound("sounds/start.wav");
  _startSound.setVolume(0.75f);
  _startSound.setMultiPlay(false);
  _startSound.setLoop(false); 
  
  _stateMachine = BR_PLAYING;
}

//--------------------------------------------------------------
void App::checkForOscMessages(){
  while (_receiver.hasWaitingMessages()){
    ofxOscMessage m;
    _receiver.getNextMessage(&m);
    if (m.getAddress() == "/ballroom/bounce/"){
      int stair = m.getArgAsInt32(0);
      cout << "A ball hit stair #" << stair << endl;
      bangStair(stair);
    }
  }
}

//--------------------------------------------------------------
void App::startTada(){
  //send message to dashboard
  ofxOscMessage m;
  m.setAddress("/ballroom/tada/");
  _sender.sendMessage(m);
}

//--------------------------------------------------------------
void App::startScore(){
  _scoreSound.play();
  gameOver();
}

//--------------------------------------------------------------
void App::startPlaying(){
  for (unsigned int i = 0; i < _stairs.size(); i++){
      _stairs[i]->setPermanent(false);
  }
  _startSound.play();
  //send message to dashboard
  ofxOscMessage m;
  m.setAddress("/ballroom/start/");
  _sender.sendMessage(m);
}

//--------------------------------------------------------------
void App::gameOver(){
  //send message to dashboard
  ofxOscMessage m;
  m.setAddress("/ballroom/gameover/");
  _sender.sendMessage(m);
}

//--------------------------------------------------------------
void App::bangStair(int stair){
  if (_stateMachine == BR_PLAYING){  
    if (!_stairs[stair]->isPermanent()){
      _stairs[stair]->setPermanent(true);
      _bounceSound.play(); 
      //animation
      for (unsigned int i = stair; i < _stairs.size(); i++){
        _stairs[i]->setDuration(200);
        _stairs[i]->bangDelay(20*(i-stair));
      }
      //check if all stairs are permanent
      bool isFinished = true;
      for (unsigned int i = 0; i < _stairs.size(); i++){
        if (!_stairs[i]->isPermanent()){
          isFinished = false;
          break;
        }
      }
      if (isFinished){
        setState(BR_SCORE);
      }
      //send message to dashboard
      ofxOscMessage m;
      m.setAddress("/ballroom/bounce/");
      m.addIntArg(stair);
      _sender.sendMessage(m);

    } else {
      _bounceNotGoodSound.play(); 
      _bounceSound.stop(); 
        setState(BR_BOUNCENOTGOOD);
    }
  }
}

//--------------------------------------------------------------
void App::updateState(){
  if (_stateMachine == BR_BOUNCENOTGOOD){
    if (ofGetElapsedTimeMillis() - _startOfStateTime > 1000){
      setState(BR_SCORE);
    }
  }
  else if (_stateMachine == BR_SCORE){
    if (ofGetElapsedTimeMillis() - _startOfStateTime > 1250){
      setState(BR_TADA);
    }
  }
  else if (_stateMachine == BR_TADA){
    if (ofGetElapsedTimeMillis() - _startOfStateTime > 4000){
      setState(BR_PLAYING);
    }
  }
}

//--------------------------------------------------------------
void App::setState(gameState state){
  _startOfStateTime = ofGetElapsedTimeMillis();
  _stateMachine = state;
  if (state == BR_PLAYING){
    startPlaying();
  }
  else if (state == BR_SCORE){
    startScore();
  }
  else if (state == BR_TADA){
    startTada();
  }
}

//--------------------------------------------------------------
void App::update(){
	ofSoundUpdate();
  updateState();
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
  else if  (key == 'j'){
    cout << "All Active" << endl;
    for (unsigned int i = 0; i < _stairs.size(); i++){
      //_stairs[i]->setDuration(10000);
      _stairs[i]->bang();
    }
  }
  else if (key == '1'){
    bangStair(1);
  }
  else if (key == '2'){
    bangStair(2);
  }
  else if (key == '3'){
    bangStair(3);
  }
  else if (key == '4'){
    bangStair(4);
  }
  else if (key == '5'){
    bangStair(5);
  }
  else if (key == '6'){
    bangStair(6);
  }
  else if (key == '7'){
    bangStair(7);
  }
  else if (key == '8'){
    bangStair(8);
  }
  else if (key == '9'){
    bangStair(9);
  }
  else if (key == '0'){
    bangStair(0);
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
