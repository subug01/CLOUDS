//
// CLOUDS Interactive Documentary
//
// VISUAL SYSTEMS
//
// Welcome to the EMPTY CloudsVisualSystem
//
//
//

#pragma once

#include "CloudsVisualSystem.h"
typedef struct {
	ofVbo* vbo;
	string name;
	int indexCount;
} NamedVbo;

//TODO: rename this to your own visual system
class CloudsVisualSystemLaplacianTunnel : public CloudsVisualSystem {
  public:
    string getSystemName(){
		return "LaplacianTunnel";
	}

	//These methods let us add custom GUI parameters and respond to their events
    void selfSetupGui();
    void selfGuiEvent(ofxUIEventArgs &e);
    
	//Use system gui for global or logical settings, for exmpl
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
	//use render gui for display settings, like changing colors
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);

	// selfSetup is called when the visual system is first instantiated
	// This will be called during a "loading" screen, so any big images or
	// geometry should be loaded here
    void selfSetup();

	// selfBegin is called when the system is ready to be shown
	// this is a good time to prepare for transitions
	// but try to keep it light weight as to not cause stuttering
    void selfBegin();

	// selfPresetLoaded is called whenever a new preset is triggered
	// it'll be called right before selfBegin() and you may wish to
	// refresh anything that a preset may offset, such as stored colors or particles
	void selfPresetLoaded(string presetPath);
    
	//do things like ofRotate/ofTranslate here
	//any type of transformation that doesn't have to do with the camera
    void selfSceneTransformation();
	
	//normal update call
	void selfUpdate();

	// selfDraw draws in 3D using the default ofEasyCamera
	// you can change the camera by returning getCameraRef()
    void selfDraw();
		
    // draw any debug stuff here
	void selfDrawDebug();

	// or you can use selfDrawBackground to do 2D drawings that don't use the 3D camera
	void selfDrawBackground();

	// this is called when your system is no longer drawing.
	// Right after this selfUpdate() and selfDraw() won't be called any more
	void selfEnd();

	// this is called when you should clear all the memory and delet anything you made in setup
    void selfExit();

	//events are called when the system is active
	//Feel free to make things interactive for you, and for the user!
    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);
    
    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
	
    // if you use a custom camera to fly through the scene
	// you must implement this method for the transitions to work properly
	ofCamera& getCameraRef(){
		if(bUseExternalCamera){
			return externalCam;
		}
		return tunnelCam;
	}

  protected:
    
    //  Your Stuff
    //
	ofLight headlight;
	float corkscrewFactor;
	float lightDistance;
	ofVec3f min;
	ofVec3f max;
	ofVec3f center;
	
	int loadMesh(ofVbo &vbo, string path);
	
	
	ofCamera tunnelCam;
	ofxUISuperCanvas* customGui;
	int frameCount;
	float startTime;
	float fps;
	float fogDensity;
	int numReplications;
	float replicationOffset;
	float cameraDistance;
	float cameraSpeed;
	vector<NamedVbo> vbos;
	
	bool bUseExternalCamera;
	bool bDrawPoints;
	bool bPalindrome;
	float growthFPS; //only works in palindrome
	float maxLookAngle;
	ofVec2f currentLookAngle;
	float currentGrowthIndex;
	float lastFrameTime;
	
	ofEasyCam externalCam;

	
	void clear();
	
};
