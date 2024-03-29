#include "ofxAlphaVideoPlayer.h"
#include "ofUtils.h"

#ifdef TARGET_LINUX
	#include <gst/video/video.h>
#endif

//--------------------------------------------------------------
#ifdef  OF_VIDEO_PLAYER_QUICKTIME
//--------------------------------------------------------------

bool  	ofxCreateMovieFromPath(char * path, Movie &movie);
bool 	ofxCreateMovieFromPath(char * path, Movie &movie){



	Boolean 	isdir			= false;
	OSErr 		result 			= 0;
	FSSpec 		theFSSpec;

	short 		actualResId 	= DoTheRightThing;

	#ifdef TARGET_WIN32
		result = NativePathNameToFSSpec (path, &theFSSpec, 0);
		if (result != noErr) {
			ofLog(OF_LOG_ERROR,"NativePathNameToFSSpec failed %d", result);
			ofLog(OF_LOG_ERROR,"Error loading movie");
			return false;
		}

	#endif



	#ifdef TARGET_OSX
		FSRef 		fsref;
		result = FSPathMakeRef((const UInt8*)path, &fsref, &isdir);
		if (result) {
			ofLog(OF_LOG_ERROR,"FSPathMakeRef failed %d", result);
			ofLog(OF_LOG_ERROR,"Error loading movie");
			return false;
		}
		result = FSGetCatalogInfo(&fsref, kFSCatInfoNone, NULL, NULL, &theFSSpec, NULL);
		if (result) {
			ofLog(OF_LOG_ERROR,"FSGetCatalogInfo failed %d", result);
			ofLog(OF_LOG_ERROR,"Error loading movie");
			return false;
		}
	#endif


	short movieResFile;
	result = OpenMovieFile (&theFSSpec, &movieResFile, fsRdPerm);
	if (result == noErr) {

		short   movieResID = 0;
		result = NewMovieFromFile(&movie, movieResFile, &movieResID, (unsigned char *) 0, newMovieActive, (Boolean *) 0);
		if (result == noErr){
			CloseMovieFile (movieResFile);
		} else {
			ofLog(OF_LOG_ERROR,"NewMovieFromFile failed %d", result);
			return false;
		}
	} else {
		ofLog(OF_LOG_ERROR,"OpenMovieFile failed %d", result);
		return false;
	}

	return true;
}

//--------------------------------------------------------------
bool ofxCreateMovieFromURL(string urlIn,  Movie &movie){
	char * url = (char *)urlIn.c_str();
	Handle urlDataRef;

	OSErr err;

	urlDataRef = NewHandle(strlen(url) + 1);
	if ( ( err = MemError()) != noErr){ ofLog(OF_LOG_ERROR,"ofxCreateMovieFromURL: error creating url handle"); return false;}

	BlockMoveData(url, *urlDataRef, strlen(url) + 1);

	err = NewMovieFromDataRef(&movie, newMovieActive,nil, urlDataRef, URLDataHandlerSubType);
	DisposeHandle(urlDataRef);

	if(err != noErr){
		ofLog(OF_LOG_ERROR,"ofxCreateMovieFromURL: error loading url");
		return false;
	}else{
		return true;
	}

	return false;
}



//--------------------------------------------------------------
OSErr 	ofxDrawCompleteProc(Movie theMovie, long refCon);
OSErr 	ofxDrawCompleteProc(Movie theMovie, long refCon){

	ofxAlphaVideoPlayer * ofvp = (ofxAlphaVideoPlayer *)refCon;

	#if defined(TARGET_OSX) && defined(__BIG_ENDIAN__)
		convertPixels(ofvp->offscreenGWorldPixels, ofvp->pixels, ofvp->width, ofvp->height);
	#endif

	ofvp->bHavePixelsChanged = true;
	if (ofvp->bUseTexture == true){
		ofvp->tex.loadData(ofvp->pixels, ofvp->width, ofvp->height, GL_RGBA);
	}

	return noErr;
}

//--------------------------------------------------------------
#endif
//--------------------------------------------------------------




//---------------------------------------------------------------------------
ofxAlphaVideoPlayer::ofxAlphaVideoPlayer (){

	bLoaded 					= false;
	width 						= 0;
	height						= 0;
	speed 						= 1;
	bUseTexture					= true;
	bStarted					= false;
	pixels						= NULL;
	nFrames						= 0;
	bPaused						= false;



	//--------------------------------------------------------------
    #ifndef  TARGET_LINUX  // !linux = quicktime...
    //--------------------------------------------------------------
    	moviePtr	 				= NULL;
    	allocated 					= false;
        offscreenGWorld				= NULL;
	//--------------------------------------------------------------
	#endif
	//--------------------------------------------------------------

}

//---------------------------------------------------------------------------
unsigned char * ofxAlphaVideoPlayer::getPixels(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
	return pixels;
	//--------------------------------------
	#else
	//--------------------------------------
	return gstUtils.getPixels();
	//--------------------------------------
	#endif
	//--------------------------------------
}

//------------------------------------
//for getting a reference to the texture
ofTexture & ofxAlphaVideoPlayer::getTextureReference(){
	if(!tex.bAllocated() ){
		ofLog(OF_LOG_WARNING, "ofxAlphaVideoPlayer - getTextureReference - texture is not allocated");
	}
	return tex;
}


//---------------------------------------------------------------------------
bool ofxAlphaVideoPlayer::isFrameNew(){
	return bIsFrameNew;


}

//--------------------------------------------------------------------
void ofxAlphaVideoPlayer::update(){
	idleMovie();
}

//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::idleMovie(){

	if (bLoaded == true){

		//--------------------------------------------------------------
		#ifndef TARGET_LINUX  // !linux = quicktime...
		//--------------------------------------------------------------

			#if defined(TARGET_WIN32) || defined(QT_USE_MOVIETASK)
				MoviesTask(moviePtr,0);
			#endif

		//--------------------------------------------------------------
		#else // linux.
		//--------------------------------------------------------------

			gstUtils.update();
			if(gstUtils.isFrameNew()){
				bHavePixelsChanged = true;
				width = gstUtils.getWidth();
				height = gstUtils.getHeight();
				tex.loadData(gstUtils.getPixels(), width, height, GL_RGBA);
			}

		//--------------------------------------------------------------
		#endif
		//--------------------------------------------------------------
	}

	// ---------------------------------------------------
	// 		on all platforms,
	// 		do "new"ness ever time we idle...
	// 		before "isFrameNew" was clearning,
	// 		people had issues with that...
	// 		and it was badly named so now, newness happens
	// 		per-idle not per isNew call
	// ---------------------------------------------------
	
	if (bLoaded == true){

		bIsFrameNew = bHavePixelsChanged;
		if (bHavePixelsChanged == true) {
			bHavePixelsChanged = false;
		}
	}

}

//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::closeMovie(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

	if (bLoaded == true){

	    DisposeMovie (moviePtr);
		DisposeMovieDrawingCompleteUPP(myDrawCompleteProc);

		moviePtr = NULL;
    }

    //--------------------------------------
	#else
	//--------------------------------------

		gstUtils.close();

	//--------------------------------------
	#endif
    //--------------------------------------

    bLoaded = false;

}

//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::close(){
	closeMovie();
}

//---------------------------------------------------------------------------
ofxAlphaVideoPlayer::~ofxAlphaVideoPlayer(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
		closeMovie();
		if(allocated)	delete[] pixels;
		if(allocated)	delete[] offscreenGWorldPixels;
		//if ((offscreenGWorld)) DisposeGWorld((offscreenGWorld));

	 //--------------------------------------
	#else
	//--------------------------------------

		closeMovie();

	//--------------------------------------
	#endif
	//--------------------------------------

	tex.clear();

}




//--------------------------------------
#ifdef OF_VIDEO_PLAYER_QUICKTIME
//--------------------------------------

void ofxAlphaVideoPlayer::createImgMemAndGWorld(){
	Rect movieRect;
	movieRect.top 			= 0;
	movieRect.left 			= 0;
	movieRect.bottom 		= height;
	movieRect.right 		= width;
	offscreenGWorldPixels 	= new unsigned char[4 * width * height + 32];
	pixels					= new unsigned char[width*height*4]; // 4 =>rgba

	#if defined(TARGET_OSX) && defined(__BIG_ENDIAN__)
		cout << "with Big Endian"<<endl;
		QTNewGWorldFromPtr (&(offscreenGWorld), k32ARGBPixelFormat, &(movieRect), NULL, NULL, 0, (offscreenGWorldPixels), 4 * width);		
	#else
		cout << "no Big Endian"<<endl;
		QTNewGWorldFromPtr (&(offscreenGWorld), k32RGBAPixelFormat, &(movieRect), NULL, NULL, 0, (pixels), 4 * width);
	#endif
	/** POSSIBLE PIXEL FORMATS 
	k32BGRAPixelFormat            = 'BGRA', // 32 bit bgra    (Matrox)
	k32ABGRPixelFormat            = 'ABGR', // 32 bit abgr   
	k32RGBAPixelFormat            = 'RGBA', // 32 bit rgba   	
	 ***********/
	//LockPixels(GetGWorldPixMap(offscreenGWorld));
	//SetGWorld (offscreenGWorld, NULL);
	SetMovieGWorld (moviePtr, offscreenGWorld, nil);
	//------------------------------------ texture stuff:
	if (bUseTexture){
		// create the texture, set the pixels to black and
		// upload them to the texture (so at least we see nothing black the callback)
		tex.allocate(width,height,GL_RGBA);
		memset(pixels, 0, width*height*4);
		tex.loadData(pixels, width, height, GL_RGBA);
		allocated = true;
	}
}

//--------------------------------------
#endif
//--------------------------------------




//---------------------------------------------------------------------------
bool ofxAlphaVideoPlayer::loadMovie(string name){


	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

		initializeQuicktime();			// init quicktime
		closeMovie();					// if we have a movie open, close it
		bLoaded = false;				// try to load now

		if( name.substr(0, 7) == "http://" || name.substr(0,7) == "rtsp://" ){
			if(! ofxCreateMovieFromURL(name, moviePtr) ) return false;
		}else{
			name 					= ofToDataPath(name);
			if( !ofxCreateMovieFromPath((char *)name.c_str(), moviePtr) ) return false;
		}

		bool bDoWeAlreadyHaveAGworld = false;
		if (width != 0 && height != 0){
			bDoWeAlreadyHaveAGworld = true;
		}
		Rect 				movieRect;
		GetMovieBox(moviePtr, &(movieRect));
		if (bDoWeAlreadyHaveAGworld){
			// is the gworld the same size, then lets *not* de-allocate and reallocate:
			if (width == movieRect.right &&
				height == movieRect.bottom){
				SetMovieGWorld (moviePtr, offscreenGWorld, nil);
			} else {
				width 	= movieRect.right;
				height 	= movieRect.bottom;
				delete(pixels);
				delete(offscreenGWorldPixels);
				//if ((offscreenGWorld)) DisposeGWorld((offscreenGWorld));
				createImgMemAndGWorld();
			}
		} else {
			width	= movieRect.right;
			height 	= movieRect.bottom;
			createImgMemAndGWorld();
		}

		if (moviePtr == NULL){
			return false;
		}

		//----------------- callback method
	    myDrawCompleteProc = NewMovieDrawingCompleteUPP (ofxDrawCompleteProc);
		SetMovieDrawingCompleteProc (moviePtr, movieDrawingCallWhenChanged,  myDrawCompleteProc, (long)this);

		// ------------- get the total # of frames:
		nFrames				= 0;
		TimeValue			curMovieTime;
		curMovieTime		= 0;
		TimeValue			duration;

		//OSType whichMediaType	= VIDEO_TYPE; // mingw chokes on this
		OSType whichMediaType	= FOUR_CHAR_CODE('vide');

		short flags				= nextTimeMediaSample + nextTimeEdgeOK;

		while( curMovieTime >= 0 ) {
			nFrames++;
			GetMovieNextInterestingTime(moviePtr,flags,1,&whichMediaType,curMovieTime,0,&curMovieTime,&duration);
			flags = nextTimeMediaSample;
		}
		nFrames--; // there's an extra time step at the end of themovie




		// ------------- get some pixels in there ------
		GoToBeginningOfMovie(moviePtr);
		SetMovieActiveSegment(moviePtr, -1,-1);
		MoviesTask(moviePtr,0);

		#if defined(TARGET_OSX) && defined(__BIG_ENDIAN__)
			cout << "convertPixels" <<endl;
			convertPixels(offscreenGWorldPixels, pixels, width, height);
			cout << "converPixels end"<<endl;
		#endif

		if (bUseTexture == true){
			cout << "LOADMOVIE: using texture" <<endl;
			tex.loadData(pixels, width, height, GL_RGBA);
		}

		bStarted 				= false;
		bLoaded 				= true;
		bPlaying 				= false;
		bHavePixelsChanged 		= false;
		speed 					= 1;

		return true;

	//--------------------------------------
	#else
	//--------------------------------------


		if(gstUtils.loadMovie(name)){
			if(bUseTexture){
				tex.allocate(gstUtils.getWidth(),gstUtils.getHeight(),GL_RGBA,false);
				tex.loadData(gstUtils.getPixels(), gstUtils.getWidth(), gstUtils.getHeight(), GL_RGBA);
			}
			bLoaded = true;
			allocated = true;
			ofLog(OF_LOG_VERBOSE,"ofxAlphaVideoPlayer: movie loaded");
			return true;
		}else{
			ofLog(OF_LOG_ERROR,"ofxAlphaVideoPlayer couldn't load movie");
			return false;
		}


	//--------------------------------------
	#endif
	//--------------------------------------



}



//--------------------------------------------------------
void ofxAlphaVideoPlayer::start(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

	if (bLoaded == true && bStarted == false){
		SetMovieActive(moviePtr, true);

		//------------------ set the movie rate to default
		//------------------ and preroll, so the first frames come correct

		TimeValue timeNow 	= 	GetMovieTime(moviePtr, 0);
		Fixed playRate 		=	GetMoviePreferredRate(moviePtr); 		//Not being used!

		PrerollMovie(moviePtr, timeNow, X2Fix(speed));
		SetMovieRate(moviePtr,  X2Fix(speed));
		setLoopState(OF_LOOP_NORMAL);

		// get some pixels in there right away:
		MoviesTask(moviePtr,0);
		#if defined(TARGET_OSX) && defined(__BIG_ENDIAN__)
			convertPixels(offscreenGWorldPixels, pixels, width, height);
		#endif
		bHavePixelsChanged = true;
		if (bUseTexture == true){
			tex.loadData(pixels, width, height, GL_RGBA);
		}

		bStarted = true;
		bPlaying = true;
	}

	//--------------------------------------
	#endif
	//--------------------------------------
}

//--------------------------------------------------------
void ofxAlphaVideoPlayer::play(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

	if (!bStarted){
	 	start();
	}else {
		// ------------ lower level "startMovie"
		// ------------ use desired speed & time (-1,1,etc) to Preroll...
		bPlaying = true;
		TimeValue timeNow;
	   	timeNow = GetMovieTime(moviePtr, nil);
		PrerollMovie(moviePtr, timeNow, X2Fix(speed));
		SetMovieRate(moviePtr,  X2Fix(speed));
		MoviesTask(moviePtr, 0);
	}

	//--------------------------------------
	#else
	//--------------------------------------
		gstUtils.play();
	//--------------------------------------
	#endif
	//--------------------------------------



}

//--------------------------------------------------------
void ofxAlphaVideoPlayer::stop(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

	StopMovie (moviePtr);
	SetMovieActive (moviePtr, false);
	bStarted = false;

	//--------------------------------------
	#else
	//--------------------------------------

	gstUtils.setPaused(true);

	//--------------------------------------
	#endif
	//--------------------------------------
}

//--------------------------------------------------------
void ofxAlphaVideoPlayer::setVolume(int volume){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

	SetMovieVolume(moviePtr, volume);
	//--------------------------------------
	#else
	//--------------------------------------

	gstUtils.setVolume(volume);

	//--------------------------------------
	#endif
	//--------------------------------------

}


//--------------------------------------------------------
void ofxAlphaVideoPlayer::setLoopState(int state){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

		TimeBase myTimeBase;
		long myFlags = 0L;
		myTimeBase = GetMovieTimeBase(moviePtr);
		myFlags = GetTimeBaseFlags(myTimeBase);
		switch (state) {
			case OF_LOOP_NORMAL:
				myFlags |= loopTimeBase;
				myFlags &= ~palindromeLoopTimeBase;
				SetMoviePlayHints(moviePtr, hintsLoop, hintsLoop);
				SetMoviePlayHints(moviePtr, 0L, hintsPalindrome);
				break;
			case OF_LOOP_PALINDROME:
				myFlags |= loopTimeBase;
				myFlags |= palindromeLoopTimeBase;
				SetMoviePlayHints(moviePtr, hintsLoop, hintsLoop);
				SetMoviePlayHints(moviePtr, hintsPalindrome, hintsPalindrome);
				break;
			case OF_LOOP_NONE:
				default:
				myFlags &= ~loopTimeBase;
				myFlags &= ~palindromeLoopTimeBase;
				SetMoviePlayHints(moviePtr, 0L, hintsLoop |
				hintsPalindrome);
				break;
		}
		SetTimeBaseFlags(myTimeBase, myFlags);

	//--------------------------------------
	#else
	//--------------------------------------

		gstUtils.setLoopState(state);

	//--------------------------------------
	#endif
	//--------------------------------------

}


//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::setPosition(float pct){


 	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

	 	TimeRecord tr;
	 	tr.base 		= GetMovieTimeBase(moviePtr);
		long total 		= GetMovieDuration(moviePtr );
		long newPos 	= (long)((float)total * pct);
		SetMovieTimeValue(moviePtr, newPos);
		MoviesTask(moviePtr,0);

	//--------------------------------------
	#else
	//--------------------------------------

		gstUtils.setPosition(pct);

	//--------------------------------------
	#endif
	//--------------------------------------


}

//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::setFrame(int frame){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

	// frame 0 = first frame...

	// this is the simple way...
	//float durationPerFrame = getDuration() / getTotalNumFrames();

	// seems that freezing, doing this and unfreezing seems to work alot
	// better then just SetMovieTimeValue() ;

	if (!bPaused) SetMovieRate(moviePtr, X2Fix(0));

	// this is better with mpeg, etc:
	double frameRate = 0;
	double movieTimeScale = 0;
	MovieGetStaticFrameRate(moviePtr, &frameRate);
	movieTimeScale = GetMovieTimeScale(moviePtr);

	if (frameRate > 0){
		double frameDuration = 1 / frameRate;
		TimeValue t = (TimeValue)(frame * frameDuration * movieTimeScale);
		SetMovieTimeValue(moviePtr, t);
		MoviesTask(moviePtr, 0);
	}

   if (!bPaused) SetMovieRate(moviePtr, X2Fix(speed));

   //--------------------------------------
    #else
   //--------------------------------------

	   gstUtils.setFrame(frame);


   //--------------------------------------
    #endif
   //--------------------------------------

}


//---------------------------------------------------------------------------
float ofxAlphaVideoPlayer::getDuration(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

		return (float) (GetMovieDuration (moviePtr) / (double) GetMovieTimeScale (moviePtr));

	//--------------------------------------
	#else
	//--------------------------------------

		return gstUtils.getDuration();

	//--------------------------------------
	#endif
	//--------------------------------------

}

//---------------------------------------------------------------------------
float ofxAlphaVideoPlayer::getPosition(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

		long total 		= GetMovieDuration(moviePtr);
		long current 	= GetMovieTime(moviePtr, nil);
		float pct 		= ((float)current/(float)total);
		return pct;

	//--------------------------------------
	#else
	//--------------------------------------

		return gstUtils.getPosition();

	//--------------------------------------
	#endif
	//--------------------------------------


}

//---------------------------------------------------------------------------
int ofxAlphaVideoPlayer::getCurrentFrame(){
	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
	int frame = 0;

	// zach I think this may fail on variable length frames...
	float pos = getPosition();


	float  framePosInFloat = ((float)getTotalNumFrames() * pos);
	int    framePosInInt = (int)framePosInFloat;
	float  floatRemainder = (framePosInFloat - framePosInInt);
	if (floatRemainder > 0.5f) framePosInInt = framePosInInt + 1;
	//frame = (int)ceil((getTotalNumFrames() * getPosition()));
	frame = framePosInInt;

	return frame;
	//--------------------------------------
	#else
	//--------------------------------------

	return gstUtils.getCurrentFrame();

	//--------------------------------------
	#endif
	//--------------------------------------

}


//---------------------------------------------------------------------------
bool ofxAlphaVideoPlayer::getIsMovieDone(){

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
		bool bIsMovieDone = (bool)IsMovieDone(moviePtr);
		return bIsMovieDone;
	//--------------------------------------
	#else
	//--------------------------------------
		return gstUtils.getIsMovieDone();
	//--------------------------------------
	#endif
	//--------------------------------------

}

//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::firstFrame(){
	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
	setFrame(0);
	//--------------------------------------
	#else
	//--------------------------------------
	gstUtils.firstFrame();
	//--------------------------------------
	#endif
	//--------------------------------------

}

//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::nextFrame(){
	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
	setFrame(getCurrentFrame() + 1);
	//--------------------------------------
	#else
	//--------------------------------------
	gstUtils.nextFrame();
	//--------------------------------------
	#endif
	//--------------------------------------
}

//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::previousFrame(){
	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
	setFrame(getCurrentFrame() - 1);
	//--------------------------------------
	#else
	//--------------------------------------
	gstUtils.previousFrame();
	//--------------------------------------
	#endif
	//--------------------------------------
}



//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::setSpeed(float _speed){

	speed 				= _speed;

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

		if (bPlaying == true){
			//setMovieRate actually plays, so let's call it only when we are playing
			SetMovieRate(moviePtr, X2Fix(speed));
		}

	//--------------------------------------
	#else
	//--------------------------------------

		gstUtils.setSpeed(_speed);

	//--------------------------------------
	#endif
	//--------------------------------------
}

//---------------------------------------------------------------------------
float ofxAlphaVideoPlayer::getSpeed(){
	return speed;
}

//---------------------------------------------------------------------------
void ofxAlphaVideoPlayer::setPaused(bool _bPause){

	bPaused = _bPause;

	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------

		// there might be a more "quicktime-ish" way (or smarter way)
		// to do this for now, to pause, just set the movie's speed to zero,
		// on un-pause, set the movie's speed to "speed"
		// (and hope that speed != 0...)
		if (bPlaying == true){
			if (bPaused == true) 	SetMovieRate(moviePtr, X2Fix(0));
			else 					SetMovieRate(moviePtr, X2Fix(speed));
		}
	//--------------------------------------
	#else
	//--------------------------------------

		gstUtils.setPaused(_bPause);

	//--------------------------------------
	#endif
	//--------------------------------------

}

//------------------------------------
void ofxAlphaVideoPlayer::setUseTexture(bool bUse){
	bUseTexture = bUse;
}

//we could cap these values - but it might be more useful
//to be able to set anchor points outside the image

//----------------------------------------------------------
void ofxAlphaVideoPlayer::setAnchorPercent(float xPct, float yPct){
    if (bUseTexture)tex.setAnchorPercent(xPct, yPct);
}

//----------------------------------------------------------
void ofxAlphaVideoPlayer::setAnchorPoint(int x, int y){
    if (bUseTexture)tex.setAnchorPoint(x, y);
}

//----------------------------------------------------------
void ofxAlphaVideoPlayer::resetAnchor(){
   	if (bUseTexture)tex.resetAnchor();
}

//------------------------------------
void ofxAlphaVideoPlayer::draw(float _x, float _y, float _w, float _h){
	if (bUseTexture){
		tex.draw(_x, _y, _w, _h);
	}
}

//------------------------------------
void ofxAlphaVideoPlayer::draw(float _x, float _y){
	draw(_x, _y, (float)width, (float)height);
}

//------------------------------------
int ofxAlphaVideoPlayer::getTotalNumFrames(){
	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
	return nFrames;
	//--------------------------------------
	#else
	//--------------------------------------
	return gstUtils.getTotalNumFrames();
	//--------------------------------------
	#endif
	//--------------------------------------

}

//----------------------------------------------------------
float ofxAlphaVideoPlayer::getHeight(){
	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
	return (float)height;
	//--------------------------------------
	#else
	//--------------------------------------
	return gstUtils.getHeight();
	//--------------------------------------
	#endif
	//--------------------------------------
}

//----------------------------------------------------------
float ofxAlphaVideoPlayer::getWidth(){
	//--------------------------------------
	#ifdef OF_VIDEO_PLAYER_QUICKTIME
	//--------------------------------------
	return (float)width;
	//--------------------------------------
	#else
	//--------------------------------------
	return gstUtils.getWidth();
	//--------------------------------------
	#endif
	//--------------------------------------

}
