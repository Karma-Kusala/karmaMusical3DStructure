#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	debug = true;
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(150, 50, 50);
	ofEnableAlphaBlending();
	
	// - - - -
	// FFT SETUP
	bufferSize = 512;//2048;
	
	//fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING);
	//fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING, OF_FFT_FFTW);
	//ofSoundStreamListDevices();
	fft.setup(bufferSize);
	
	//drawBins.resize(fft->getBinSize());
	//middleBins.resize(fft->getBinSize());
	//audioBins.resize(fft->getBinSize());
	
	// 0 output channels, 1 = input channel, 44100 samples per second, [bins] samples per buffer, 4 num buffers (latency)
	//ofSoundStreamSetup(0, 1, this, 44100, bufferSize, 4);
	
	// - - - -
	// set camera and environment
	cam.setDistance(-50);
	cam.enableMouseInput();
	light.enable();
	
	//we need to call this for textures to work on structures
    //ofDisableArbTex();
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
	// it makes sure that the back of the structure doesn't show through the front
	ofEnableDepthTest();
	
	//now we load our structure
	//aiSetImportPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, true);
	//AI_CONFIG_PP_PTV_KEEP_HIERARCH
	structure.loadModel("model.dae");
	structure.setScale(1,1,1);
	structure.setRotation(0, 90, 0,0,1);
	structure.optimizeScene();
	structure.update();
	//structure.setPosition(ofGetWidth()*.5, ofGetHeight() * 0.75, 0);
	//structure.enableNormals();
	
	if(debug) cout << "Setup completed!" << endl;
	
	ofSetWindowTitle("Structure Mapping");
	
	glShadeModel(GL_SMOOTH);
	//glEnable(GL_NORMALIZE);
	
}

//--------------------------------------------------------------
void testApp::update(){
	// fft
	// todo: use EQ channels to modiy animation timings and variables
	fft.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	// erase
	ofBackground(0, 0, 0);
	ofSetColor(255, 255, 255, 255);
	
	cam.begin(ofGetCurrentViewport());
	
	//get the position of the structure
	ofVec3f position = structure.getPosition();
	
	//save the current view
	//ofPushMatrix();
	
	//center ourselves there
	//ofTranslate(-position);
	//ofRotate(-ofGetMouseX()/10, .5f, 1, 0);
	//ofTranslate(-position); */
	
	//draw the structure (automatic version)
	//structure.drawWireframe();
	//ofSetColor(255,255,255,80);
	//structure.drawFaces();
	//ofSetHexColor(0xFF0000);
	//structure.drawVertices();
	
	// prepare to manually draw the mesh
	ofVboMesh mesh = structure.getMesh(0);
	
	// stupid matrix transforms, otherwise transform and scale are different; dunno why
	// ressource used: http://ofxfenster.undef.ch/doc/ofxAssimpModelLoader_8cpp_source.html
	ofPushMatrix();
	ofMultMatrix( structure.getModelMatrix() );
	ofPushMatrix();
	//ofRotateX( -structure.getRotationAngle(0) ); // seems to only be necessary using .3ds models
	
	// draw distorted vertices
	ofSetHexColor(0xFF0000);
	ofFill();
	vector<ofVec3f>& verts = mesh.getVertices();
	float liquidness = 2;
	float amplitude = 3;
	float speedDampen = 4;
	for(unsigned int i = 0; i < verts.size(); i++){
		verts[i].x += ofSignedNoise(verts[i].x/liquidness, verts[i].y/liquidness,verts[i].z/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
		 verts[i].y += ofSignedNoise(verts[i].z/liquidness, verts[i].x/liquidness,verts[i].y/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
		 verts[i].z += ofSignedNoise(verts[i].y/liquidness, verts[i].z/liquidness,verts[i].x/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
		
		//ofSetHexColor(0x00FF00);
		//if(i > 0) ofLine(verts[i].x, verts[i].y, verts[i].z, verts[i-1].x, verts[i-1].y, verts[i-1].z);
	}
	
	ofSetHexColor(0xFF0000);
	ofFill();
	
	vector<ofMeshFace> faces = mesh.getUniqueFaces();
	for(unsigned int i = 0; i < faces.size(); i++){
		ofMeshFace face = faces[i];
		
		// hack to force-recalculate face normal
		ofVec3f temp = face.getVertex(0);
		face.setVertex(0,temp);
		
		ofVec3f normal = faces[i].getFaceNormal();
		normal.normalize();
		ofVec3f center = CalculateCentroid(&face);
		
		// draw normal
		ofCircle(center.x, center.y, center.z, .5);
		ofLine(center.x, center.y, center.z, center.x + normal.x*5, center.y + normal.y*5, center.z + normal.z*5);
		
		//ofLine(face.getVertex(2).x, face.getVertex(2).y, face.getVertex(2).z, face.getVertex(1).x, face.getVertex(1).y, face.getVertex(1).z);
		//ofLine(face.getVertex(0).x, face.getVertex(0).y, face.getVertex(0).z, face.getVertex(1).x, face.getVertex(1).y, face.getVertex(1).z);
		//ofLine(face.getVertex(2).x, face.getVertex(2).y, face.getVertex(2).z, face.getVertex(0).x, face.getVertex(0).y, face.getVertex(0).z);
		musicalLine(fft.getBins(), normal*-5.0, face.getVertex(2), face.getVertex(0));
		musicalLine(fft.getBins(), normal*-5.0, face.getVertex(0), face.getVertex(1));
		musicalLine(fft.getBins(), normal*-5.0, face.getVertex(1), face.getVertex(2));
		
		ofCircle(face.getVertex(0).x, face.getVertex(0).y, face.getVertex(0).z, 1);
		ofCircle(face.getVertex(1).x, face.getVertex(1).y, face.getVertex(1).z, 1);
		ofCircle(face.getVertex(2).x, face.getVertex(2).y, face.getVertex(2).z, 1);
		
	}
	
	
	//restore the view position
    ofPopMatrix(); ofPopMatrix();
	
	// tell camera we're done
	cam.end();
	
	// show debugging info
	if(debug){
		ofSetHexColor(0xFFFFFF);
		ofSetColor(250);
		ofNoFill();
		ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate())+"fps", 10, 15);
	}
	
	// idea to use:
	// http://www.openframeworks.cc/documentation/gl/ofVbo.html
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
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

//	CUSTOM FUNCTIONS

// borrowed + adapted from http://coding-experiments.blogspot.fr/2009/09/xna-quest-for-centroid-of-polygon.html
ofVec3f testApp::CalculateCentroid(ofMeshFace *face){
	// for the moment, all ofMeshFace have 3 vertices as only getMode() == OF_PRIMITIVE_TRIANGLES is supported
	int lastPointIndex = 3; // and there's no way of determining the amount of vertices...
	
	ofVec3f result;
	
	// loop trough vertices
    for (int i = 0; i < lastPointIndex; i++){
        result += face->getVertex(i);
	}
	
	result /= lastPointIndex;
	
    return result;
}

// ofxFFT function

// FFT / Graphics
void testApp::musicalLine(vector<float>& buffer, ofVec3f intensity, ofVec3f start, ofVec3f end) {
	ofNoFill();
	
	// calc averages
	int n = buffer.size();
	int nb_eq = 10;
	vector<float> eq;
	vector<int> eqDivider;
	eq.resize(nb_eq);
	eqDivider.resize(nb_eq);
	for (int i = 0; i < n; i++) {
		// eq number is mapped to an exonential scale
		float linear_position = ofMap(i, 0, n, 0, 1);
		float exponential_position = sin(linear_position*HALF_PI);
		
		// eq lookup
		int eqNum = 0;
		while( sin( ofMap(eqNum, 0, nb_eq, 0, HALF_PI) ) < exponential_position && eqNum < nb_eq ) eqNum++;
		
		eq[eqNum] += buffer[i];
		eqDivider[eqNum] ++;
	}
	
	// sum them up, keep average
	for (int i = 0; i < nb_eq; i++) eq[i] /= eqDivider[i]; 
	
	
	ofVec3f step = end - start;
	step /= nb_eq;
	intensity*=2;
	ofBeginShape();
	for (int i = 0; i <= nb_eq; i++) {
		//ofVertex(0, sqrt(buffer[i])*intensity );
		ofVec3f tmp = start + step*i + intensity*sqrt(eq[(i+ofGetFrameNum()/10)%(nb_eq-1)]);
		ofVertex( tmp.x, tmp.y, tmp.z );
	}
	ofEndShape();
}