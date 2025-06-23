#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon

typedef struct enemy {
	int enemypos;
	int enemyhp;
	int maxhp;
	bool isactive;
	int delay;
}enemy;

typedef struct tower {
	int y, x;
	double range;
	int cooltime;
	int	rate;
	int power;
	int level;

}tower;




class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		int** maze;
		int HEIGHT = 13;
		int WIDTH = 26;
		int start;
		int coin;
		int endrow;
		int endcol;
		int blockmod = 0;
		int** visited;
		int offset[4][2] = {
			{1, 0},
			{0, 1},
			{-1, 0},
			{0, -1}
		};
		vector<pair<int, int>> path;
		pair<int, int>** prev;
		int round = 0;
		int frame = 0;
		int hp = 3;
		enemy enemys[400];
		int enemynum = 0;
		tower towers[400];
		int towernum = 0;
		double tl[3][3] = {//range rate power
			{1.5, 15, 35},
			{2.5, 12, 50},
			{3.5, 8, 100}
		};


		//ÇÔ¼ö
		bool BFS();

		// Menu
		ofxWinMenu* menu; // Menu object
		void appMenuFunction(string title, bool bChecked); // Menu return function

		// Used by example app
		ofTrueTypeFont myFont;
		ofImage myImage;
		float windowWidth, windowHeight;
		HWND hWnd; // Application window
		HWND hWndForeground; // current foreground window

};
