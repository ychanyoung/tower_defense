#define _CRT_SECURE_NO_WARNINGS
#include "ofApp.h"
#include <iostream>
#include <time.h>

using namespace std;








//--------------------------------------------------------------
void ofApp::setup(){
	//세팅
	start = 0;
	round = 0;
	frame = 0;
	hp = 3;
	maze = (int**)malloc(HEIGHT * (sizeof(int*)));
	visited = (int**)malloc(HEIGHT * (sizeof(int*)));
	prev = (pair<int, int>**)malloc(HEIGHT * sizeof(pair<int, int>*));
	for (int i = 0; i < HEIGHT; i++) {
		maze[i] = (int*)calloc(WIDTH, sizeof(int));
		visited[i] = (int*)calloc(WIDTH, sizeof(int));
		prev[i] = (pair<int, int>*)malloc(WIDTH * sizeof(pair<int, int>));
	}
	ofSetWindowTitle("Random Defense");
	//ofSetFrameRate(15); //초당 프레임 제한
	ofBackground(230); // 배경 색깔
	ofSetFrameRate(20);
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	myFont.loadFont("verdana.ttf", 12, true, true);
	hWnd = WindowFromDC(wglGetCurrentDC());
	ofSetEscapeQuitsApp(false);
	menu = new ofxWinMenu(this, hWnd);

	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	HMENU hMenu = menu->CreateWindowMenu();


	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");
	menu->AddPopupItem(hPopup, "Start", false, false); // Not checked and not auto-checked

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);


	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "Rule", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

}
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Start") {
		start = 1;
		hp = 3;
		round = 0;
		frame = 0;
		coin = 250;
		enemynum = 0;
		towernum = 0;
		for (int i = 0; i < HEIGHT; i++) {
			for (int j = 0; j < WIDTH; j++) {
				maze[i][j] = 0;
				visited[i][j] = 0;
				blockmod = 0;
			}
		}
		endrow = 3 + rand() % 5;
		endcol = 3 + rand() % 18;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				maze[endrow + i][endcol + j] = 100;

			}
		}
		cout << endrow << " " << endcol << "\n";
		BFS();

	}
	if (title == "Exit") {
		ofExit(); // Quit the application
	}

	if (title == "Rule") {
		ofSystemAlertDialog("Enemies will spawn from the red block.\nThe enemies will head for the blue block(the base) via the shortest path.\nThe player can build walls to block or delay the enemies' approach as much as possible.\nCompletely blocking the path is not allowed.");
	}

} // end appMenuFunction
//--------------------------------------------------------------
void ofApp::update(){
	frame = (frame + 1) % 10000;
	if (!start) return;
	if (hp <= 0) {
		return;
	}
	if (round <= -15) {
		start = 2;
		return;
	}
	
	if (round > 0) {
		if (frame % 10 == 0) {
			for (int i = 0; i < enemynum; i++) {
				if (enemys[i].enemyhp <= 0) enemys[i].isactive = false;
				if (enemys[i].isactive == false) continue;
				if (enemys[i].delay > 0) {
					enemys[i].delay -= 1;
					continue;
				}


				enemys[i].enemypos -= 1;
				if (enemys[i].enemypos < 0) {
					enemys[i].isactive = false;
					hp--;

				}
				if (enemys[enemynum - 1].enemypos < 0) {
					coin += 250 * round;
					round *= -1;
				}

			}
		}

		for (int i = 0; i < towernum; i++) {
			if (towers[i].cooltime > 0) {
				if (frame % towers[i].rate == 0) {
					maze[towers[i].y][towers[i].x] = towers[i].level + 1;
					towers[i].cooltime -= 1;
				}
				continue;
			}
			maze[towers[i].y][towers[i].x] = (towers[i].level + 1) * 10;
			int idx = -1;
			int min = 10000;
			for (int j = 0; j < enemynum; j++) {
				if (enemys[j].isactive == false) continue;
				//제일 가까운 거 && range 안에 들어 있는 적
				int dy = abs(path[enemys[j].enemypos].first - towers[i].y);
				int dx = abs(path[enemys[j].enemypos].second - towers[i].x);
				int dr;
				dr = dy * dy + dx + dx;
				if (dr <= towers[i].range * towers[i].range) {
					if (min > dr) {
						idx = j;
						min = dr;
					}
				}

			}
			if (idx >= 0) {
				enemys[idx].enemyhp -= towers[i].power;
				towers[i].cooltime = towers[i].rate;
			}

		}
		int flag = 0;
		for (int i = 0; i < enemynum; i++) {
			if (enemys[i].isactive == true) flag = 1;
		}
		if (!flag) {
			coin += 250 * round;
			round *= -1;
		}


	}

}

//--------------------------------------------------------------
void ofApp::draw(){
	if (!start) return;

	//블록 하나 40*40
	//칸
	ofSetColor(170);
	ofSetLineWidth(2);
	for (int i = 1; i < HEIGHT; i++) {
		ofDrawLine(200, 50 + 40 * i, 200 + 26 * 40, 50 + 40 * i);
	}
	for (int i = 1; i < WIDTH; i++) {
		ofDrawLine(200 + 40 *i, 50, 200 + 40 * i, 50 + 13 * 40);
	}

	//테두리
	ofSetColor(30);
	ofSetLineWidth(5);
	ofDrawLine(198, 50, 202 + 26 * 40, 50);
	ofDrawLine(200, 50, 200, 52 + 13 * 40);
	ofDrawLine(200, 50 + 13 * 40, 200 + 26 * 40, 50 + 13 * 40);
	ofDrawLine(200 + 26 * 40, 50, 200 + 26 * 40, 52 + 13 * 40);

	//코인
	ofSetColor(0,200,0,50);
	ofDrawRectangle(100, 650, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("coin : " + ofToString(coin), 110, 676);

	//목숨
	ofSetColor(0, 200, 0, 50);
	ofDrawRectangle(100, 750, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("hp : " + ofToString(hp), 110, 776);

	//버튼
	ofSetColor(0, 0, 0, 50);
	ofDrawRectangle(250, 650, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("add wall\n20 coin", 260, 676);

	ofSetColor(0, 0, 0, 50);
	ofDrawRectangle(250, 750, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("sell mod\nhalf", 260, 776);

	//출발
	ofSetColor(0, 0, 0, 50);
	ofDrawRectangle(400, 650, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("round start", 410, 676);

	//포탑
	ofSetColor(0, 0, 0, 50);
	ofDrawRectangle(400, 750, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("add tower\n50 coin", 410, 776);

	//업그레이드
	ofSetColor(0, 0, 0, 50);
	ofDrawRectangle(550, 750, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("upgrade tower\n100/ 200 coin", 560, 776);

	//라운드
	ofSetColor(0);
	ofDrawBitmapString("round : " + ofToString(abs(round)), 650, 600);


	//적 출발
	ofSetColor(200, 0, 0, 50);
	ofDrawRectangle(160, 50 + 6* 40, 40, 40);

	if (round <= 0) {
		for (auto it = path.rbegin(); it != path.rend(); ++it) {
			ofSetColor(200, 0, 0, 40);
			ofDrawCircle(220 + it->second * 40, 70 + it->first * 40, 3);
		}



	}
	

	//벽과 거점
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			switch (maze[i][j]) {
			case 1:
				ofSetColor(0, 0, 0, 90);
				ofDrawRectangle(200 + j * 40, 50 + i * 40, 40, 40);
				break;

			case 2:
				ofSetColor(0, 0, 0, 90);
				ofDrawRectangle(200 + j * 40, 50 + i * 40, 40, 40);
				ofSetColor(0, 0, 200, 50);
				ofDrawCircle(220 + j * 40, 70 + i * 40, 17);
				break;

			case 20:
				ofSetColor(0, 0, 0, 90);
				ofDrawRectangle(200 + j * 40, 50 + i * 40, 40, 40);
				ofSetColor(0, 0, 200, 100);
				ofDrawCircle(220 + j * 40, 70 + i * 40, 17);
				break;


			case 3:
				ofSetColor(0, 0, 0, 90);
				ofDrawRectangle(200 + j * 40, 50 + i * 40, 40, 40);
				ofSetColor(0, 200, 0, 50);
				ofDrawCircle(220 + j * 40, 70 + i * 40, 17);
				break;

			case 30:
				ofSetColor(0, 0, 0, 90);
				ofDrawRectangle(200 + j * 40, 50 + i * 40, 40, 40);
				ofSetColor(0, 200, 0, 100);
				ofDrawCircle(220 + j * 40, 70 + i * 40, 17);
				break;

			case 4:
				ofSetColor(0, 0, 0, 90);
				ofDrawRectangle(200 + j * 40, 50 + i * 40, 40, 40);
				ofSetColor(200, 0, 200, 50);
				ofDrawCircle(220 + j * 40, 70 + i * 40, 17);
				break;

			case 40:
				ofSetColor(0, 0, 0, 90);
				ofDrawRectangle(200 + j * 40, 50 + i * 40, 40, 40);
				ofSetColor(200, 0, 200, 100);
				ofDrawCircle(220 + j * 40, 70 + i * 40, 17);
				break;

			case 100:
				ofSetColor(0, 0, 200, 50);
				ofDrawRectangle(200 + j*40, 50 + i * 40, 40, 40);
				break;
			}
		}
	}

	if (hp <= 0) {
		ofSetColor(0);
		ofDrawBitmapString("Game Over", 400, 600);
		return;
	}
	if (start == 2) {
		ofSetColor(0);
		ofDrawBitmapString("Game Win", 400, 600);
		return;
	}

	//적
	if (round > 0) {
		for (int i = 0; i < enemynum; i++) {
			if (enemys[i].isactive == false) continue;
			if (enemys[i].delay > 0) continue;

			if (enemys[i].enemypos >= 0) {
				ofSetColor(240, 0, 0, 90);
				ofDrawCircle(220 + 40 * path[enemys[i].enemypos].second, 73 + 40 * path[enemys[i].enemypos].first, 15);
				ofSetColor(240, 0, 0, 200);
				ofSetLineWidth(4);
				ofDrawLine(205 + 40 * path[enemys[i].enemypos].second, 55 + 40 * path[enemys[i].enemypos].first, 235 + 40 * path[enemys[i].enemypos].second - (float)((float)(enemys[i].maxhp - enemys[i].enemyhp) / enemys[i].maxhp) * 30, 55 + 40 * path[enemys[i].enemypos].first);

				

			}

			



		}

	}



}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if (!start) return;
	ofSetColor(0, 0, 0, 50);
	ofDrawRectangle(250, 650, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("add mod", 260, 676);

	ofSetColor(0, 0, 0, 50);
	ofDrawRectangle(250, 750, 100, 50);
	ofSetColor(10);
	ofDrawBitmapString("crash mod", 260, 776);
	//add 
	// 250 650 ~ 350 700
	//crash
	// 250 750 ~ 350 800
	//blockmod 
	//default 0		add 1		crash 2

	if (hp <= 0) {

		return;
	}
	if (start == 2) {

		return;
	}



	if (x >= 250 && x <= 350 && y >= 650 && y <= 700) {
		cout << "add\n";
		blockmod = 1;
	}
	else if (x >= 250 && x <= 350 && y >= 750 && y <= 800) {
		cout << "crash\n";
		blockmod = 2;
	}
	else if (x >= 400 && x <= 500 && y >= 650 && y <= 700) {
		if (round <= 0) {
			cout << "round start\n";
			BFS();
			round = abs(round) + 1;
			enemynum = round;
			for (int i = 0; i < enemynum; i++) {
				enemys[i].enemypos = path.size() - 1;
				enemys[i].maxhp = 100;
				enemys[i].enemyhp = enemys[i].maxhp;
				enemys[i].isactive = true;
				enemys[i].delay = i;
			}
			for (int i = 0; i < towernum; i++) {
				towers[i].cooltime = 0;
			}
		}
		//출발
	}
	else if (x >= 400 && x <= 500 && y >= 750 && y <= 800) {
		cout << "tower\n";
		blockmod = 3;



	}
	else if (x >= 550 && x <= 650 && y >= 750 && y <= 800) {
		cout << "upgrade\n";
		blockmod = 4;

	}
	else if (x >= 200 && x <= 200 + 26 * 40 && y >= 50 && y <= 50 + 13 * 40) {
		if (blockmod == 1 && round <= 0) {
			if (maze[(y - 50) / 40][(x - 200) / 40] != 0) return;
			cout << (y - 50) / 40 << ", " << (x - 200) / 40;
			if (coin >= 20) {
				maze[(y - 50) / 40][(x - 200) / 40] = 1;
				if (!BFS()) {//이거 알고리즘 추가
					cout << "   설치불가\n";
					maze[(y - 50) / 40][(x - 200) / 40] = 0;
					BFS();
					return;
				}
				cout << "   벽추가\n";
				coin -= 20;
			}
			else {
				cout << "   돈부족\n";
			}
			//추가

		}
		else if (blockmod == 2 && round <= 0) {
			if (maze[(y - 50) / 40][(x - 200) / 40] == 1) {
				cout << (y - 50) / 40 << ", " << (x - 200) / 40;
				cout << "   벽판매\n";
				coin += 10;
				maze[(y - 50) / 40][(x - 200) / 40] = 0;
				BFS();
			}
			else if (maze[(y - 50) / 40][(x - 200) / 40] >= 2) {
				cout << (y - 50) / 40 << ", " << (x - 200) / 40;
				cout << "   타워판매\n";
				if (maze[(y - 50) / 40][(x - 200) / 40] == 2 || maze[(y - 50) / 40][(x - 200) / 40] == 20) {
					coin += 25;
				}
				else if (maze[(y - 50) / 40][(x - 200) / 40] == 3 || maze[(y - 50) / 40][(x - 200) / 40] == 30) {
					coin += 75;
				}
				else if (maze[(y - 50) / 40][(x - 200) / 40] == 4 || maze[(y - 50) / 40][(x - 200) / 40] == 40) {
					coin += 175;
				}
				maze[(y - 50) / 40][(x - 200) / 40] = 1;
				int idx = -1;
				for (int i = 0; i < towernum; i++) {
					if (towers[i].y == (y - 50) / 40 && towers[i].x == (x - 200) / 40) {
						idx = i;
						break;
					}

				}
				for (int i = idx; i < towernum-1; i++) {
					towers[i] = towers[i + 1];
				}
				towernum--;
			}

		}
		else if (blockmod == 3 && round <= 0) {
			if (maze[(y - 50) / 40][(x - 200) / 40] == 1) {
				cout << (y - 50) / 40 << ", " << (x - 200) / 40;
				if (coin >= 50) {
					maze[(y - 50) / 40][(x - 200) / 40] = 2;


					towers[towernum].y = (y - 50) / 40;
					towers[towernum].x = (x - 200) / 40;
					towers[towernum].level = 1;
					towers[towernum].range = tl[towers[towernum].level - 1][0];
					towers[towernum].rate = tl[towers[towernum].level - 1][1];
					towers[towernum].power = tl[towers[towernum].level - 1][2];
					towers[towernum].cooltime = 0;
					towernum++;


					cout << "   타워추가\n";
					coin -= 50;
				}
				else {
					cout << "   돈부족\n";
				}



			}



		}
		else if (blockmod == 4 && round <= 0) {
			switch (maze[(y - 50) / 40][(x - 200) / 40]) {
			case 0:
			case 1:
			case 40:
			case 100:
			case 4:
				break;

			case 20:
			case 2:
				if (coin >= 100) {
					int idx = -1;
					for (int i = 0; i < towernum; i++) {
						if (towers[i].y == (y - 50) / 40 && towers[i].x == (x - 200) / 40) {
							idx = i;
							break;
						}
					}
					maze[(y - 50) / 40][(x - 200) / 40] = 3;
					towers[idx].level = 2;
					towers[idx].range = tl[towers[idx].level-1][0];
					towers[idx].rate = tl[towers[idx].level-1][1];
					towers[idx].power = tl[towers[idx].level-1][2];
					towers[idx].cooltime = 0;
					coin -= 100;


				}
				else {
					cout << "   돈부족\n";
				}

				break;

			case 30:
			case 3:
				
				if (coin >= 200) {

					int idx = -1;
					for (int i = 0; i < towernum; i++) {
						if (towers[i].y == (y - 50) / 40 && towers[i].x == (x - 200) / 40) {
							idx = i;
							break;
						}
					}
					maze[(y - 50) / 40][(x - 200) / 40] = 4;
					towers[idx].level = 3;
					towers[idx].range = tl[towers[idx].level - 1][0];
					towers[idx].rate = tl[towers[idx].level - 1][1];
					towers[idx].power = tl[towers[idx].level - 1][2];
					towers[idx].cooltime = 0;
					coin -= 200;
				}
				else {
					cout << "   돈부족\n";
				}

				break;

			}

			
		}
		
	}
	else{
		cout << "click " << x << " " << y << "\n";
		blockmod = 0;
	}



}

bool ofApp::BFS()//BFS탐색을 하는 함수
{

	queue<pair<int, int>> q;
	int sx = 0;
	int sy = 6;
	if (maze[sy][sx] == 1) return false;
	path.clear();

	bool found = false;
	q.push({ sy,sx });

	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) visited[i][j] = 0;
	}
	visited[sy][sx] = 1;
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) prev[i][j] = { -1,-1 };
	}
	

	while (!q.empty() && !found) {
		auto [sy, sx] = q.front();
		q.pop();

		if ((sy == endrow || sy == endrow + 1) && (sx == endcol || sx == endcol + 1)) found = true;
		if (found) break;

		for (int i = 0; i < 4; i++) {
			int ny = sy + offset[i][0];
			int nx = sx + offset[i][1];

			if (nx < 0 || nx >= WIDTH || ny < 0 || ny >= HEIGHT) continue;
			if (visited[ny][nx] == 1)continue;
			if (maze[ny][nx] != 0 && maze[ny][nx] != 100)continue;

			if ((ny == endrow || ny == endrow + 1) && (nx == endcol || nx == endcol + 1)) {
				found = true;
			}
			visited[ny][nx] = 1;
			q.push({ ny,nx });
			prev[ny][nx] = { sy, sx };
			if (found) break;
		}


	}


	if (found) {
		int y=-1;
		int x=-1;
		int f = 1;
		for (int i = 0; i < 2 && f==1; i++) {
			for (int j = 0; j < 2 && f==1; j++) {
				if (visited[endrow + i][endcol + j] != 0) {
					y = endrow + i;
					x = endcol + j;
					f = 0;
				}
			}
		}

		while (x != -1 && y != -1) {
			path.push_back({ y,x });
			tie(y, x) = prev[y][x];
		}

		//for (auto it = path.rbegin(); it != path.rend(); ++it) {
		//	cout << it->first << ", " << it->second << "\n";
		//}


	}


	return found;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
