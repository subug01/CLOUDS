//
//  IntroSequence.cpp
//  ComputerTicker
//
//  Created by James George on 5/26/13.
//
//

#include "CloudsIntroSequence.h"
#include "ofxObjLoader.h"
#include "CloudsGlobal.h"

CloudsIntroSequence::CloudsIntroSequence(){
	selectedQuestion = NULL;
	showingQuestions = false;
	useDebugCamera = false;
	paused = false;
}

CloudsIntroSequence::~CloudsIntroSequence(){
	
}

string CloudsIntroSequence::getSystemName(){
	return "_Intro";
}

void CloudsIntroSequence::selfSetup(){

	font.loadFont(getDataPath() + "font/materiapro_light.ttf", 18);
	
	loadedQuestions.push_back("Shiffman_NOC_3");
	loadedQuestions.push_back("Lauren_how_we_communicate,_shorter");
	loadedQuestions.push_back("Ramsey_computation_is_interpretation_of_rules");
	
	perlinOffset = 0;
	
	camera.setup();
	camera.autosavePosition = true;
	camera.loadCameraPosition();
	
//	ofxObjLoader::load(getVisualSystemDataPath() + "OBJ/ParticleCube_supertight.obj", tunnelMeshTight);
//	ofxObjLoader::load(getVisualSystemDataPath() + "OBJ/ParticleCube_loose.obj", tunnelMeshLoose);
	
//	ofxObjLoader::load(getDataPath() + "intro/OBJ/CLOUDS_type_thin_02.obj",thinTypeMesh);
	ofxObjLoader::load(getVisualSystemDataPath() + "OBJ/CLOUDS_type_thick.obj",thickTypeMesh);

	thinTypeMesh.load(getVisualSystemDataPath() + "OBJ/CLOUDS_type_thin_02.ply");
	
	thinTypeMesh.clearColors();
	
	ofDisableArbTex();
	sprite.loadImage(getVisualSystemDataPath() + "images/dot.png");
	ofEnableArbTex();
	
	currentFontExtrusion = -1;
	currentFontSize = -1;
	
	reloadShaders();

}

void CloudsIntroSequence::selfPresetLoaded(string presetPath){
//	tunnelMax = tunnelMin = tunnelMeshLoose.getVertices()[0];
//	for(int i = 1; i < tunnelMeshLoose.getVertices().size(); i++){
//		tunnelMax = ofVec3f(MAX(tunnelMax.x, tunnelMeshLoose.getVertices()[i].x),
//							MAX(tunnelMax.y, tunnelMeshLoose.getVertices()[i].y),
//							MAX(tunnelMax.z, tunnelMeshLoose.getVertices()[i].z));
//		tunnelMin = ofVec3f(MIN(tunnelMin.x, tunnelMeshLoose.getVertices()[i].x),
//							MIN(tunnelMin.y, tunnelMeshLoose.getVertices()[i].y),
//							MIN(tunnelMin.z, tunnelMeshLoose.getVertices()[i].z));
//	}

	
	generateTunnel();
	
	cout << "Tunnel min is " << tunnelMin << " tunnel max is " << tunnelMax << endl;
	

	warpCamera.setPosition(0, 0, 0);
	warpCamera.lookAt(ofVec3f(0,0,tunnelMax.z));
	
	positionStartQuestions();
}

void CloudsIntroSequence::reloadShaders(){
	tunnelShader.load(getVisualSystemDataPath() + "shaders/IntroTunnel");
	questionShader.load(getVisualSystemDataPath() + "shaders/Question");
	chroma.load("",getVisualSystemDataPath() + "shaders/BarrelChromaAb.fs");
	CloudsQuestion::reloadShader();
}

void CloudsIntroSequence::selfUpdate(){
	
	camera.applyRotation = camera.applyTranslation = useDebugCamera && !cursorIsOverGUI();
	
	if(!paused){
		warpCamera.dolly(-cameraForwardSpeed);
	}
	
	ofVec2f mouseNode(ofGetMouseX(),ofGetMouseY());
	for(int i = 0; i < startQuestions.size(); i++){
		
		startQuestions[i].radius = questionSize;
		startQuestions[i].update();
		
		if(startQuestions[i].position.z < warpCamera.getPosition().z){
			startQuestions[i].position.z += questionWrapDistance;
		}
		
		if(startQuestions[i].position.z - warpCamera.getPosition().z < questionTugMinDepth){
			float distanceToQuestion = startQuestions[i].currentScreenPoint.distance(mouseNode);
			if(caughtQuestion == NULL){
				if( distanceToQuestion < questionTugMaxDistance){
					startQuestions[i].position.z += ofMap(distanceToQuestion, questionTugMaxDistance, questionTugMinDistance, 0, cameraForwardSpeed);
					if(distanceToQuestion < questionTugMinDistance){
						caughtQuestion = &startQuestions[i];
						caughtQuestion->startHovering();
					}
				}
			}
			//we have a caught question make sure it's still close
			else if(caughtQuestion == &startQuestions[i]){
				startQuestions[i].position.z += cameraForwardSpeed;
				if( caughtQuestion->isSelected() && ofGetMousePressed()){
					selectedQuestion = caughtQuestion;
				}
				else if(distanceToQuestion > questionTugMinDistance){
					caughtQuestion->stopHovering();
					caughtQuestion = NULL;
				}
			}
		}
	}
	
	if(currentFontSize != fontSize ||
	   currentFontExtrusion != fontExtrusion)
	{
		currentFontSize = fontSize;
		currentFontExtrusion = fontExtrusion;
//		cloudsTypeMesh.init(getDataPath() + "/font/materiapro_light.ttf", currentFontSize, currentFontExtrusion, "CLOUDS");
	}

}

void CloudsIntroSequence::setStartQuestions(vector<CloudsClip>& possibleStartQuestions){

	selectedQuestion = NULL;
	startQuestions.clear();
	
	for(int i = 0; i < possibleStartQuestions.size(); i++){
		
		CloudsQuestion q;
		q.cam = &warpCamera;
		q.font = &font;
		q.clip = possibleStartQuestions[i];
//		q.topic = q.clip.getAllTopicsWithQuestion()[0];
//		q.question = q.clip.getQuestionForTopic(q.topic);
		
		q.setup();
		
		startQuestions.push_back(q);
	}
}

void CloudsIntroSequence::generateTunnel(){

	// loose tunnel, with lines
	tunnelMeshLoose.clear();
	
	float looseTunnelStepZ = tunnelMax.z / looseTunnelResolutionZ;
	float looseTunnelStepY = tunnelMax.y / looseTunnelResolutionX;
	float looseTunnelStepX = tunnelMax.x / looseTunnelResolutionX;
	
	float tightTunnelStepZ = looseTunnelStepZ/2.0;
	float tightTunnelStepY = looseTunnelStepY/2.0;
	float tightTunnelStepX = looseTunnelStepX/2.0;

	for(float z = 0; z < tunnelMax.z; z += looseTunnelStepZ){
		//add invisible connector point
		tunnelMeshLoose.addColor(ofFloatColor(0,0));
		tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,-tunnelMax.y, z));
		
		//draw the top
		for(float x = -tunnelMax.x; x <= tunnelMax.x; x += tightTunnelStepX){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(x,-tunnelMax.y,z));
		}
		//draw right side
		for(float y = -tunnelMax.y; y <= tunnelMax.y; y += tightTunnelStepY){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(tunnelMax.x,y,z));
		}
		//draw bottom
		for(float x = tunnelMax.x; x >= -tunnelMax.x; x -= tightTunnelStepX){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(x,tunnelMax.y,z));
		}
		//draw the left side
		for(float y = tunnelMax.y; y >= -tunnelMax.y; y -= tightTunnelStepY){
			tunnelMeshLoose.addColor(ofFloatColor::white);
			tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,y,z));
		}
		tunnelMeshLoose.addColor(ofFloatColor(0,0));
		tunnelMeshLoose.addVertex(ofVec3f(-tunnelMax.x,-tunnelMax.y, z));
	}
	tunnelMeshLoose.setMode(OF_PRIMITIVE_LINE_STRIP);
	
	
	// tight tunnel, dots only
	tunnelMeshTight.clear();
	for(float z = 0; z < tunnelMax.z; z += tightTunnelStepZ){
		//draw the top
		for(float x = -tunnelMax.x; x <= tunnelMax.x; x += tightTunnelStepX){
			tunnelMeshTight.addVertex(ofVec3f(x,-tunnelMax.y,z));
		}
		//draw right side
		for(float y = -tunnelMax.y; y <= tunnelMax.y; y += tightTunnelStepY){
			tunnelMeshTight.addVertex(ofVec3f(tunnelMax.x,y,z));
		}
		//draw bottom
		for(float x = tunnelMax.x; x >= -tunnelMax.x; x -= tightTunnelStepX){
			tunnelMeshTight.addVertex(ofVec3f(x,tunnelMax.y,z));
		}
		//draw the left side
		for(float y = tunnelMax.y; y >= -tunnelMax.y; y -= tightTunnelStepY){
			tunnelMeshTight.addVertex(ofVec3f(-tunnelMax.x,y,z));
		}
	}	
}

void CloudsIntroSequence::positionStartQuestions(){

	//set the start questions along a random tunnel
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].position = ofVec3f(0, ofRandom(questionTunnelInnerRadius, tunnelMax.y), 0);
		startQuestions[i].position.rotate(ofRandom(360), ofVec3f(0,0,1));
		startQuestions[i].position.z = tunnelMax.z*.5 + ofRandom(questionWrapDistance);
		startQuestions[i].orientToCenter();
	}
	
}

bool CloudsIntroSequence::isStartQuestionSelected(){
	return selectedQuestion != NULL;
}

CloudsQuestion* CloudsIntroSequence::getSelectedQuestion(){
	return selectedQuestion;
}

void CloudsIntroSequence::selfDrawBackground(){

}

void CloudsIntroSequence::selfDrawDebug(){


}

void CloudsIntroSequence::timelineBangEvent(ofxTLBangEventArgs& args){
	//testing for now
	CloudsVisualSystem::timelineBangEvent(args);
//	if(args.flag == "TriggerQ"){
//		//selectedQuestion = &startQuestions[0];
//		showingQuestions = true;
//	}
}

void CloudsIntroSequence::selfDraw(){
	
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);	// allows per-point size
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_DEPTH_TEST);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	drawCloudsType();
	
	ofPushStyle();
	
	tunnelShader.begin();
	tunnelShader.setUniform1f("minPointSize", pointSize.min);
	tunnelShader.setUniform1f("maxPointSize", pointSize.max);
	tunnelShader.setUniform1f("minDistance", distanceRange.min);
	tunnelShader.setUniform1f("maxDistance", distanceRange.max);
	tunnelShader.setUniform1f("cameraZ", warpCamera.getPosition().z);
	tunnelShader.setUniform1f("tunnelDepth", tunnelMax.z);

	tunnelShader.setUniform1f("noiseAmplitude", perlinAmplitude);
	tunnelShader.setUniform1f("noiseDensity", perlinDensity);
	perlinOffset += perlinSpeed;
	tunnelShader.setUniform1f("noisePosition", perlinOffset);
	tunnelShader.setUniform1f("colorAttenuate", 1.0);
	ofFloatColor tinted = ofFloatColor::fromHsb(tint.r, tint.g, tint.b);
	tunnelShader.setUniform4f("tint",tinted.r,tinted.g,tinted.b, 1.0);
	
	ofSetColor(255);
	ofEnablePointSprites();
	tunnelShader.setUniformTexture("sprite", sprite, 0);
	tunnelShader.setUniform1f("useSprite", 1);
	tunnelMeshTight.drawVertices();
	ofDisablePointSprites();
	
	ofSetLineWidth(1);
	tunnelShader.setUniform1f("colorAttenuate", wireframeAlpha);
	tunnelShader.setUniform1f("useSprite", 0);
	tunnelMeshLoose.draw();

	tunnelShader.end();
		
	ofPopStyle();
	
	ofPushStyle();
	ofFloatColor questionTint = ofFloatColor::fromHsb(questionNodeTint.r, questionNodeTint.g, questionNodeTint.b);
	ofFloatColor loadedQuestionTint = ofFloatColor::fromHsb(questionNodeTint.r, questionNodeTint.g*1.2, questionNodeTint.b*1.3);

	CloudsQuestion::startShader();
	CloudsQuestion::shader.setUniform1f("minDistance", distanceRange.min);	
	CloudsQuestion::shader.setUniform1f("maxDistance", distanceRange.max);
	CloudsQuestion::shader.setUniform1f("attenuateFade", 1.0);

	for(int i = 0; i < startQuestions.size(); i++){
		if( ofContains(loadedQuestions,startQuestions[i].clip.getID()) ){
			CloudsQuestion::shader.setUniform4f("color",1.0, loadedQuestionTint.g, 1.0, 1.0);
		}
		else{
		   CloudsQuestion::shader.setUniform4f("color",questionTint.r, questionTint.g, questionTint.b, 1.0);
		}
		startQuestions[i].draw();
	}

	CloudsQuestion::endShader();
	
	ofPopStyle();
	

}

void CloudsIntroSequence::drawCloudsType(){
	ofPushMatrix();
	
	ofRotate(180, 0, 1, 0);
	ofScale(fontScale, fontScale, fontScale);
	
	ofTranslate(0, 0, -tunnelMax.z);
	thinTypeMesh.draw();
	
	ofPopMatrix();
}

void CloudsIntroSequence::selfDrawOverlay(){
}

void CloudsIntroSequence::selfPostDraw(){
	chroma.begin();
	chroma.setUniform2f("resolution", ofGetWidth(),ofGetHeight());
	chroma.setUniform1f("max_distort", maxChromaDistort);
	CloudsVisualSystem::selfPostDraw();
	chroma.end();
	
	ofPushStyle();
	for(int i = 0; i < startQuestions.size(); i++){
		startQuestions[i].drawOverlay();
	}
	ofPopStyle();
	
}

void CloudsIntroSequence::selfExit(){
	
}

void CloudsIntroSequence::selfBegin(){
	
}

void CloudsIntroSequence::selfEnd(){
	
}

void CloudsIntroSequence::selfKeyPressed(ofKeyEventArgs & args){

	if(args.key == 'q'){
		pauseAtBeginning();
	}
	if(args.key == 'R'){
		reloadShaders();
	}
}

void CloudsIntroSequence::selfKeyReleased(ofKeyEventArgs & args){
	
}

void CloudsIntroSequence::selfMouseDragged(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMouseMoved(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMousePressed(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfMouseReleased(ofMouseEventArgs& data){
	
}

void CloudsIntroSequence::selfGuiEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "arrange questions" && ((ofxUIButton*)e.widget)->getValue()){
		positionStartQuestions();
	}
	else if(e.widget->getName() == "generate tunnel" && ((ofxUIButton*)e.widget)->getValue()){
		generateTunnel();
	}
}

void CloudsIntroSequence::pauseAtBeginning(){
	warpCamera.setPosition(0, 0, 0);
	warpCamera.lookAt(ofVec3f(0,0,tunnelMax.z));
	paused = !paused;
}

void CloudsIntroSequence::selfSetupSystemGui(){
	sysGui->addButton("reset debug camera", false);
	sysGui->addButton("reload shader", false);
}

void CloudsIntroSequence::selfSetupCameraGui(){
	camGui->addToggle("use debug camera", &useDebugCamera);
	camGui->addSlider("debug camera speed", 1, 20, &camera.speed);
	camGui->addSlider("camera fwd force", 0, 10, &cameraForwardSpeed);
}

void CloudsIntroSequence::guiSystemEvent(ofxUIEventArgs &e){
	if(e.widget->getName() == "reload shader" && ((ofxUIButton*)e.widget)->getValue()){
		cout << "Loaded shader" << endl;
		reloadShaders();
	}
	else if(e.widget->getName() == "reset debug camera" && ((ofxUIButton*)e.widget)->getValue()){
		camera.setPosition(0, 0, 0);
		camera.setOrientation(ofQuaternion());
		camera.rotate(180, ofVec3f(0,1,0));
		camera.setAnglesFromOrientation();
	}
}

void CloudsIntroSequence::selfSetupRenderGui(){
	
//	rdrGui->addSlider("Min Point Size", 0, 7, &pointSize.min);
	rdrGui->addSlider("Max Point Size", 0, 7, &pointSize.max);
	rdrGui->addSlider("Min Distance", 0, 500, &distanceRange.min);
	rdrGui->addSlider("Max Distance", 0, 500, &distanceRange.max);

	rdrGui->addSlider("Chroma Max Distort", 0, 10, &maxChromaDistort);
	
	rdrGui->addSlider("Perlin Amplitude", 0, 10, &perlinAmplitude);
	rdrGui->addSlider("Perlin Density", 0, 50, &perlinDensity);
	rdrGui->addSlider("Perlin Speed", 0, .1, &perlinSpeed);
	
//	rdrGui->addSlider("Font Size", 1, 10, &fontSize);
//	rdrGui->addSlider("Font Extrusion", 0, 10, &fontExtrusion);
	rdrGui->addSlider("Font Scale", .1, 10, &fontScale);
	
	rdrGui->addSlider("Wireframe Alpha", 0, 1.0, &wireframeAlpha);
}


void CloudsIntroSequence::selfSetupGuis(){
	
	questionGui = new ofxUISuperCanvas("QUESTIONS", gui);
	questionGui->copyCanvasStyle(gui);
	questionGui->copyCanvasProperties(gui);
	questionGui->setName("Questions");
	questionGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	questionGui->addSlider("Size", 1, 5, &questionSize);
	questionGui->addSlider("Wrap Distance", 100, 4000, &questionWrapDistance);
	questionGui->addSlider("Inner Radius", 2, 20, &questionTunnelInnerRadius);
	questionGui->addSlider("Tug Min Distance", 10, 300, &questionTugMinDistance);
	questionGui->addSlider("Tug Max Distance", 10, 300, &questionTugMaxDistance);
	questionGui->addSlider("Tug Min Depth", 100, 1000, &questionTugMinDepth);

	questionGui->addSlider("Question Tint H",  0, 1.0, &questionNodeTint.r);
	questionGui->addSlider("Question Tint S",  0, 1.0, &questionNodeTint.g);
	questionGui->addSlider("Question Tint B",  0, 1.0, &questionNodeTint.b);

	questionGui->addButton("arrange questions", false);
	//	questionGui->addToggle("Custom Toggle", &customToggle);
	ofAddListener(questionGui->newGUIEvent, this, &CloudsIntroSequence::selfGuiEvent);
	
	guis.push_back(questionGui);
	guimap[questionGui->getName()] = questionGui;
	
	
	tunnelGui = new ofxUISuperCanvas("TUNNEL", gui);
	tunnelGui->copyCanvasStyle(gui);
	tunnelGui->copyCanvasProperties(gui);
	tunnelGui->setName("Tunnel");
	tunnelGui->setWidgetFontSize(OFX_UI_FONT_SMALL);
	
	tunnelGui->addSlider("Tunnel Width",  10, 40, &tunnelMax.x);
	tunnelGui->addSlider("Tunnel Height", 10, 25, &tunnelMax.y);
	tunnelGui->addSlider("Tunnel Depth",  100, 1000, &tunnelMax.z);

	tunnelGui->addSlider("Tunnel Tint H",  0, 1.0, &tint.r);
	tunnelGui->addSlider("Tunnel Tint S",  0, 1.0, &tint.g);
	tunnelGui->addSlider("Tunnel Tint B",  0, 1.0, &tint.b);
	
	tunnelGui->addSlider("Loose Rez X", 5, 20, &looseTunnelResolutionX);
	tunnelGui->addSlider("Loose Rez Z", 5, 50, &looseTunnelResolutionZ);
//	tunnelGui->addSlider("Tight Rez Mult", 1, 5, &tightResMult);

	tunnelGui->addButton("generate tunnel", false);
	
	ofAddListener(tunnelGui->newGUIEvent, this, &CloudsIntroSequence::selfGuiEvent);
	
	guis.push_back(tunnelGui);
	guimap[tunnelGui->getName()] = tunnelGui;
	
}

void CloudsIntroSequence::guiRenderEvent(ofxUIEventArgs &e){
	
}

