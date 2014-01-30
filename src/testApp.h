#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "ofxEasyFft.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
		ofxAssimpModelLoader structure;
	
		ofEasyCam cam;
		ofLight light;
	
		bool debug;
	
		ofVec3f CalculateCentroid(ofMeshFace *face);
	
		// OF FFT
		int plotHeight, bufferSize;
		ofxEasyFft fft;
		void musicalLine(vector<float>& buffer, ofVec3f intensity, ofVec3f start, ofVec3f end);
	
	
		// animation
		float musicalLineSpeed;
};
