#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();

	secondaryDisplay.setup();
    testAllClips = false;
    timer = true;
    debug = false;
    currentTestClip = 1200;
    currentProjEx = 0;
    testProjEx = false;
    startTime = 0;

	//ofSetWindowPosition(1920 + 1920*.5,1080*.5);
	fullscreened = false;


}

//--------------------------------------------------------------
void testApp::update(){

	if(ofGetFrameNum() > 10 && !fullscreened){
		ofToggleFullscreen();
		fullscreened = true;
	}

#ifdef CLOUDS_RELEASE
    ofHideCursor();
#endif
	secondaryDisplay.update();
    
    if(testAllClips && timer){
        //wait for 3 seconds
        secondaryDisplay.respondToClip( secondaryDisplay.parser.getAllClips()[currentTestClip] );
        //goto next clip
        timer = false;
        startTime = ofGetElapsedTimef();
		
        cout<<"Secondary Display::currentTestClip = "<<currentTestClip<<endl;
    }
    
    if(testAllClips && ofGetElapsedTimef() - startTime >= 3.5f){
        timer = true;
        if(currentTestClip == secondaryDisplay.parser.getAllClips().size()-1)currentTestClip=0;
        else currentTestClip++;
    }

    
}

//--------------------------------------------------------------
void testApp::draw(){
	secondaryDisplay.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ']'){
		ofPixels p;
		CloudsVisualSystem::getStaticRenderTarget().readToPixels(p);
		ofSaveImage(p,"clusterbackground.png");
	}
    if(key == 'd'){
        debug = !debug;
        secondaryDisplay.debug = debug;
    }
    else if(key == 'c'){
        secondaryDisplay.color = !secondaryDisplay.color;
    }
    else if(key == OF_KEY_RIGHT){
        secondaryDisplay.tx += .1;
        cout << "tx: "<<secondaryDisplay.tx<<endl;
    }
    else if(key == OF_KEY_LEFT){
        secondaryDisplay.tx -= .1;
        cout << "tx: "<<secondaryDisplay.tx<<endl;
    }
    
    if(key == 'a') testAllClips = true;
    if(key == 's') secondaryDisplay.respondToClip( secondaryDisplay.parser.getAllClips()[0] );
    
	
	if(key == 'C'){
		secondaryDisplay.respondToClip( secondaryDisplay.parser.getRandomClip() );
	}
	
	if(key == 'E'){
		projectExampleIndecs.clear();
		for(int i = 0; i < secondaryDisplay.parser.getAllClips().size(); i++){
			if(secondaryDisplay.parser.getAllClips()[i]->hasProjectExample){
				projectExampleIndecs.push_back(i);
			}
		}
        
        testProjEx = !testProjEx;
        testAllClips = false;
        
        if(testProjEx){
            CloudsClip* clip = secondaryDisplay.parser.getAllClips()[projectExampleIndecs[currentProjEx]];
            secondaryDisplay.respondToClip( clip );
            cout<<"Current Project Example Clip ID: "<<clip->getID()<<endl;
        }
        
		
//		if(projectExampleIndecs.size() > 0){
//			int exampleIndex = projectExampleIndecs[ ofRandom(projectExampleIndecs.size()) ];
//			secondaryDisplay.respondToClip( secondaryDisplay.parser.getAllClips()[exampleIndex] );
////			cout << "SENT CLIP " << parser.getAllClips()[exampleIndex].getLinkName() << " WITH EXAMPLE " << parser.getAllClips()[exampleIndex].projectExampleTitle << endl;
//		}
	}
    
    if(key == 'S'){
        //reload shader
        secondaryDisplay.reloadShader();
    }
    
    if(key == 'H'){
        //reload shader
        secondaryDisplay.hideGUI();
    }
	
    if(key == OF_KEY_RIGHT){
        if(testProjEx){
            if(currentProjEx >= projectExampleIndecs.size()-1) currentProjEx = 0;
            else currentProjEx++;
            CloudsClip* clip = secondaryDisplay.parser.getAllClips()[projectExampleIndecs[currentProjEx]];
            secondaryDisplay.respondToClip( clip );
            cout<<"Current Project Example Clip ID: "<<clip->getID()<<endl;
        }
    }
    
    if(key == OF_KEY_LEFT){
        if(testProjEx){
            if(currentProjEx <= 0) currentProjEx = projectExampleIndecs.size()-1;
            else currentProjEx--;
            CloudsClip* clip = secondaryDisplay.parser.getAllClips()[projectExampleIndecs[currentProjEx]];
            secondaryDisplay.respondToClip( clip );
            cout<<"Current Project Example Clip ID: "<<clip->getID()<<endl;
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}