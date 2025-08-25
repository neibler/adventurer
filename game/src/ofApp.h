/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/

	#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon

class ofApp : public ofBaseApp {

	public:
		struct Point {
			int y, x;
		};
		stack<Point> s;
		queue<Point> q;
		Point** check;
		int** temp;
		

		bool makeKey = false;	//열쇠 만들었는지 여부(초기화)

		// Dijkstra
		struct State {
			int dist;
			ofApp::Point p;
			bool operator>(const State& other) const {
				return dist > other.dist;
			}
		};

		int** dist; // 시작점에서 각 셀까지의 최단 거리 저장.
		int** wallWeight; // 벽의 가중치를 저장하는 배열 -1이면 벽, 통로면 가중치

		void setup();
		void update();
		void draw(); 

		void keyPressed(int key); // Traps escape key if exit disabled
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		bool readFile();
		void freeMemory();
		bool DFS(); // DFS탐색을 하는 함수
		void dfsdraw(); // DFS탐색을 그리는 함수

		int HEIGHT;//미로의 높이
		int WIDTH;//미로의 너비
		char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
		int** visited;//방문여부를 저장할 포인
		int maze_col;//미로칸의 열의 인덱스를 가리킨다.
		int maze_row;//미로칸의 행의 인덱스를 가리킨다.
		int k; 
		int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
		int isDFS;//DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
		int isBFS;//BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
		int isDijkstra; //다익스트라 함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
		int total; // 누적 최소 비용을 저장하는 변수
		int cost; // 현재 경로의 비용을 저장하는 변수
		bool BFS(); // BFS탐색을 하는 함수
		bool Dijkstra(); // 다익스트라 탐색을 하는 함수
		void bfsdraw(); // BFS탐색을 그리는 함수
		void dijkstraDraw(); // 다익스트라 탐색을 그리는 함수
		bool gameEnded = false; // 게임 종료 상태 표시용
	
		// Menu
		ofxWinMenu * menu; // Menu object
		void appMenuFunction(string title, bool bChecked); // Menu return function

		// Used by example app
		ofTrueTypeFont myFont;
        ofImage myImage;
		float windowWidth, windowHeight;
		HWND hWnd; // Application window
		HWND hWndForeground; // current foreground window

		// Example menu variables
		bool bShowInfo;
		bool bFullscreen;
		bool bTopmost;
		bool isdfs;
		// Example functions
 		void doFullScreen(bool bFull);
		void doTopmost(bool bTop);

		Point player;         // 현재 사용자 위치
		bool playing = false; // 게임 모드 활성 상태
		int score = 0; // 실시간 누적 점수



};
