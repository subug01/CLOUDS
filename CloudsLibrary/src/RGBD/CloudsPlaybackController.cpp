
#include "CloudsPlaybackController.h"

#include "CloudsVisualSystemStandin.h"
#include "CloudsVisualSystemComputationTicker.h"

CloudsPlaybackController::CloudsPlaybackController(){
	eventsRegistered = false;
	playingCombinedVideo = false;
	currentVisualSystem = NULL;
	showingVisualSystem = false;
}

CloudsPlaybackController::~CloudsPlaybackController(){
}

//--------------------------------------------------------------------
void CloudsPlaybackController::exit(ofEventArgs & args){
	if(eventsRegistered){
		eventsRegistered = false;
		ofRemoveListener(storyEngine->getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
		ofRemoveListener(storyEngine->getEvents().clipChanged, this, &CloudsPlaybackController::clipChanged);
		ofRemoveListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		
		ofUnregisterMouseEvents(this);
		ofUnregisterKeyEvents(this);
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::setup(CloudsStoryEngine& storyEngine){
	if(!eventsRegistered){
		this->storyEngine = &storyEngine;
		
		ofAddListener(storyEngine.getEvents().storyBegan, this, &CloudsPlaybackController::storyBegan);
		ofAddListener(storyEngine.getEvents().clipChanged, this, &CloudsPlaybackController::clipChanged);
		ofAddListener(ofEvents().exit, this, &CloudsPlaybackController::exit);
		
		ofRegisterKeyEvents(this);
		ofRegisterMouseEvents(this);
		
		eventsRegistered = true;
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::keyPressed(ofKeyEventArgs & args){
	if(args.key == 'v'){
		if(showingVisualSystem){
			hideVisualSystem();
		}
		else{
			showVisualSystem();
		}
	}
}

void CloudsPlaybackController::keyReleased(ofKeyEventArgs & args){
	
}

void CloudsPlaybackController::mouseDragged(ofMouseEventArgs & args){
	
}

void CloudsPlaybackController::mouseMoved(ofMouseEventArgs & args){
	
}

void CloudsPlaybackController::mousePressed(ofMouseEventArgs & args){

}

void CloudsPlaybackController::mouseReleased(ofMouseEventArgs & args){
	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::update(){
	player.update();
	if(player.isPlaying()){
		if(player.getCurrentFrame() >= currentClip.endFrame){
			storyEngine->selectNewClip();
		}
	}
}


//--------------------------------------------------------------------
void CloudsPlaybackController::draw(){
	
	if(!showingVisualSystem && player.isPlaying()){
		if(playingCombinedVideo){
			renderer.drawPointCloud();
		}
		else {
			player.draw(0,0,960,540);
		}
	}
}

#pragma story engine
//--------------------------------------------------------------------
void CloudsPlaybackController::storyBegan(CloudsStoryEventArgs& args){
	playClip(args.chosenClip);
}

//--------------------------------------------------------------------
void CloudsPlaybackController::clipChanged(CloudsStoryEventArgs& args){
	playClip(args.chosenClip);	
}

//--------------------------------------------------------------------
void CloudsPlaybackController::playClip(CloudsClip& clip){
	
	if(clip.hasCombinedVideo){
		if(player.loadMovie(relinkMovieFilepath(clip.combinedVideoFilePath))){
			playingCombinedVideo = true;
			renderer.setup(relinkMovieFilepath(clip.combinedVideoCalibrationXml));
			renderer.setTexture(player);
		}
	}
	else{
		playingCombinedVideo = false;
		player.loadMovie( relinkMovieFilepath(clip.sourceVideoFilePath) );
	}
	
	player.setFrame(clip.startFrame);
	player.play();
	
	currentClip = clip;
}


#pragma visual systems
//--------------------------------------------------------------------
void CloudsPlaybackController::populateVisualSystems(){
	
	CloudsVisualSystemStandin* standIn = new CloudsVisualSystemStandin();
	CloudsVisualSystemComputationTicker* computation = new CloudsVisualSystemComputationTicker();
	
	visualSystems.push_back(computation);
	visualSystems.push_back(standIn);

}

void CloudsPlaybackController::showVisualSystem(){
	
	if(showingVisualSystem){
		hideVisualSystem();
	}
	
	for(int i = 0; i < visualSystems.size(); i++){

		if(visualSystems[i]->isReleventToKeyword(storyEngine->getCurrentTopic())){
			currentVisualSystem = visualSystems[i];
			cout << "selected visual system " << currentVisualSystem->getSystemName() << " for topic " << storyEngine->getCurrentTopic() << endl;
			showingVisualSystem = true;
			currentVisualSystem->setCurrentKeyword(storyEngine->getCurrentTopic());
			currentVisualSystem->playSystem();
			
			break;
		}
	}
}

//--------------------------------------------------------------------
void CloudsPlaybackController::hideVisualSystem(){
	if(currentVisualSystem != NULL){
		showingVisualSystem = false;
		currentVisualSystem->end();
	}
}

#pragma util
//--------------------------------------------------------------------
string CloudsPlaybackController::relinkMovieFilepath(string filePath){
	
	if( !ofFile(filePath).exists() ){
		//		cout << "Switched clip from " << clipFilePath;
		ofStringReplace(filePath, "Nebula_backup", "Seance");
		ofStringReplace(filePath, "Nebula", "Seance");
		//		cout << " to " << clipFilePath << endl;
	}
	return filePath;
}


