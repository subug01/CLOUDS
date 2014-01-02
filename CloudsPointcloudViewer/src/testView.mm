#import "testView.h"
#include "CloudsGlobal.h"
#include "CloudsSpeaker.h"

@implementation testView
@synthesize clipTable;
@synthesize interventionTextBox;
@synthesize speakerVolTextBox;
- (void)setup
{

	ofSetVerticalSync(true);
	
	ofBackground(22);

	
	parser.loadFromFiles();

	
	if(ofFile::doesFileExist(GetCloudsDataPath() + "CloudsMovieDirectory.txt")){
		parser.setCombinedVideoDirectory(ofBufferFromFile(GetCloudsDataPath() + "CloudsMovieDirectory.txt").getText());
        cout<<"Clouds Directory is pointing to "<<ofBufferFromFile(GetCloudsDataPath	() + "CloudsMovieDirectory.txt").getText()<<endl;
	}
	else{
		ofSystemAlertDialog("Could not find movie file path. Create a file called CloudsMovieDirectory.txt that contains one line, the path to your movies folder");
	}


	[clipTable setTarget:self];
	[clipTable setDoubleAction:@selector(loadClipFromTable:)];
	[clipTable reloadData];
    [interventionTextBox setTarget:self];   
	[speakerVolTextBox setTarget:self];
    
	rgbdVisualSystem.setup();
	rgbdVisualSystem.setDrawToScreen(false);
	hud.setup();


	rgbdVisualSystem.playSystem();
#ifdef OCULUS_RIFT
	rgbdVisualSystem.loadPresetGUISFromName("RGBDOC");
#else
	rgbdVisualSystem.loadPresetGUISFromName("RGBDMain");
#endif
	srand(ofGetSeconds());
	
	
	
	[self loadClip: parser.getRandomClip(true, false)];
	
}

- (void)update
{
	
	hud.update();
	
//	if(rgbdVisualSystem.getRGBDVideoPlayer().isDone()){
//		cout << "replaying video!" << endl;
//		rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().setPosition(0);
//		rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().play();
//	}
}

- (void)draw
{
	rgbdVisualSystem.selfPostDraw();
	
	hud.draw();
}

- (void) loadClipFromTable:(id)sender
{

	if(clipTable.selectedRow >= 0){
		
		[self loadClip: parser.getAllClips()[ clipTable.selectedRow ] ];
		
	}
}

- (IBAction)loadClip:(CloudsClip&)clip
{
	if(clip.hasMediaAsset && clip.voiceOverAudio && rgbdVisualSystem.getRGBDVideoPlayer().setupVO(clip.voiceOverAudioPath) ){
		
		rgbdVisualSystem.getRGBDVideoPlayer().swapAndPlay();
		rgbdVisualSystem.setupSpeaker( CloudsSpeaker::speakers[clip.person].firstName,
									  CloudsSpeaker::speakers[clip.person].lastName,
									  clip.name );
		
		currentClip = clip;
	}
	else if(clip.hasMediaAsset && rgbdVisualSystem.getRGBDVideoPlayer().setup( clip.combinedVideoPath, clip.combinedCalibrationXMLPath,1,clip.speakerVolume) ){
		cout<<"clip.speakerVolume : "<<clip.speakerVolume<<endl;
		rgbdVisualSystem.getRGBDVideoPlayer().swapAndPlay();
		rgbdVisualSystem.setupSpeaker( CloudsSpeaker::speakers[clip.person].firstName,
									   CloudsSpeaker::speakers[clip.person].lastName,
									   clip.name );
		currentClip = clip;
		
	}
	else{
		ofLogError() << "CloudsPlaybackController::playClip -- folder " << clip.combinedVideoPath << " is not valid";
	}	
}

- (void)exit
{
    parser.saveSpeakersVolume(GetCloudsDataPath()+"sound/SpeakersVolume.txt");
}

- (void)keyPressed:(int)key
{
	if(key == ' ' && rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().isLoaded() ){
		if(rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().isPlaying()){
			rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().stop();
		}
		else{
			rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().play();
		}
	}
	
	if(key == 'P'){
		rgbdVisualSystem.getRGBDVideoPlayer().getPlayer().play();
	}
	
	if(key == 'H'){
		ofShowCursor();
	}
	
	if(key == 'Q'){
		CloudsClip& clip = parser.getRandomClip(false,true);
		if(clip.hasQuestion()){
			rgbdVisualSystem.addQuestion(clip, clip.getTopicsWithQuestions()[0], clip.getQuestions()[0] );
		}
		else {
			cout << "clip " << clip.getLinkName() << " does not have a question!" << endl;
		}
	}
	
	if(key == 'R'){
//		renderer.reloadShader();
	}	
	
}

- (void)keyReleased:(int)key
{

}

- (void)mouseMoved:(NSPoint)p
{
	
}

- (void)mouseDragged:(NSPoint)p button:(int)button
{
}

- (void)mousePressed:(NSPoint)p button:(int)button
{
}

- (void)mouseReleased:(NSPoint)p button:(int)button
{
	
}

- (void)windowResized:(NSSize)size
{

}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	return parser.getAllClips().size();
}

- (std::string)convertString:(NSString *)string
{
    std::string cppString([string UTF8String], [string lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
    return cppString;
}

-(IBAction)addIntervention:(id)sender{

    const char* interventionName =[interventionTextBox.stringValue UTF8String ];
    string name = interventionName;

    if(clipTable.selectedRow >= 0){
        CloudsClip& clip =parser.getAllClips()[[clipTable selectedRow]];
        cout<<" Adding intervention : "<<name<<" to clip "<<clip.getLinkName()<<endl;
    }
}

- (IBAction)updateSpeakerVolume:(id)sender{

    float speakerVol =speakerVolTextBox.floatValue;

    if(clipTable.selectedRow >= 0){
        CloudsClip& clip =parser.getAllClips()[[clipTable selectedRow]];
        parser.setSpeakerVolume(clip.person, speakerVol);
        cout<<" Updating vol for speaker : "<<clip.person<<" new vol : "<<speakerVol<<endl;
        parser.saveSpeakersVolume(GetCloudsDataPath()+"sound/SpeakersVolume.txt");
        
        rgbdVisualSystem.getRGBDVideoPlayer().currentMaxVolume = rgbdVisualSystem.getRGBDVideoPlayer().maxVolume *  speakerVol;
    }
    
}
- (void)tableViewSelectionDidChange:(NSNotification *)aNotification
{
    if (aNotification.object == clipTable) {
        [self updateSpeakerVolumeTextField];
    }
}

- (void) updateSpeakerVolumeTextField{
        if(clipTable.selectedRow >= 0){
            CloudsClip& clip =parser.getAllClips()[[clipTable selectedRow]];
            speakerVolTextBox.floatValue = clip.getSpeakerVolume();
        }
}
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{

	if([@"person" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].person.c_str() ];
	}
	else if([@"clip" isEqualToString:aTableColumn.identifier]){
		return [NSString stringWithUTF8String: parser.getAllClips()[rowIndex].name.c_str() ];
	}
	else if([@"combined" isEqualToString:aTableColumn.identifier]){
		return parser.getAllClips()[rowIndex].hasMediaAsset ? @"YES" : @"NO";
	}
	return @"";
	

}

@end