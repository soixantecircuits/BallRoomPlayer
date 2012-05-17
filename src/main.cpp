#include "ofMain.h"
#include "app.h"
#include "ofxFensterManager.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){

  //ofAppGlutWindow window;
	ofSetupOpenGL(ofxFensterManager::get(), 802,750, OF_WINDOW);			// <-------- setup the GL context
  ofxFenster* win=ofxFensterManager::get()->getPrimaryWindow();
  win->setWindowTitle("Ballroom initial view");
  
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunFensterApp( new App());

}
