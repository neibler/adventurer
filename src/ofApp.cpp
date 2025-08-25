/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

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

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
//초기 설정
void ofApp::setup() {
	ofSetWindowTitle("adventurer"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	//윈도우 핸들
	hWnd = WindowFromDC(wglGetCurrentDC());

	// ESC 키 눌러도 프로그램 안 꺼지게 설정
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	//메뉴 객체 생성
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS",false,false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS", false, false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Show Dijkstra", false, false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Game 메뉴 추가
	hPopup = menu->AddPopupMenu(hMenu, "Game");
	menu->AddPopupItem(hPopup, "Start Game", false, false);

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		readFile();
	}
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if(title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen)
		{
			isBFS = false;
			isDijkstra = false;
			DFS();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
		
	}

	if(title == "Show BFS") {
		if (isOpen)
		{
			isdfs = false;
			isDijkstra = false;
			BFS();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;

	}
	// Dijkstra 메뉴 추가
	if (title == "Show Dijkstra") {
		if (isOpen)
		{
			isdfs = false;
			isBFS = false;
			Dijkstra();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
	}

	if(title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		ofSystemAlertDialog(
			"Adventure Game Manual\n\n"
			"Use W / A / S / D keys to move the player\n"
			"Reach the destination to finish the game\n"
			"Press R after the game ends to restart\n"
			"Use the View menu to display DFS / BFS / Dijkstra paths\n\n"
			"Based on project example: ofxWinMenu\nhttp://spout.zeal.co"
		);
	}

	//
	// Game menu
	//
	if (title == "Start Game") {
		if (isOpen) {
			player = { 0, 0 };
			score = 0;
			playing = true;
			cout << "game start! move W/A/S/D" << endl;
		}
		else {
			cout << "you must open file first" << endl;
		}
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------

//내부에서 while 처리
void ofApp::draw() {

	char str[256];
	ofSetColor(100);
	ofSetLineWidth(5);

	// 저장된 자료구조를 이용해 미로를 그린다.
	if (isOpen && input != nullptr) {
		int size = 20;

		for (int y = 0; y < HEIGHT * 2 + 1; y++) {
			for (int x = 0; x < WIDTH * 2 + 1; x++) {
				char c = input[y][x];
				int X = x * size;
				int Y = y * size;

				if (c == '+' || c == '-' || c == '|') {
					ofDrawRectangle(X, Y, size, size); //ofDrawofDrawRectangle(int x, int y, int width, int height);
				}
			}
		}
	}

	if (isdfs)
	{
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen)
			dfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	if (isBFS) {
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen)
			bfsdraw();
		else
			cout << "You must open file first" << endl;
	}
	// isDijkstra가 1일 때 Dijkstra 탐색을 그린다.
	if (isDijkstra) {
		ofSetColor(200); 
		ofSetLineWidth(5);
		if (isOpen) {
			dijkstraDraw();
		}
		else {
			cout << "You must open file first" << endl;
		}
	}
	//게임 사용시 사용자 위치 그리기
	if (playing) {
		int size = 20; // 플레이어 위치를 그리기 위한 크기 설정

		// 플레이어 위치를 계산
		int px = (player.x * 2 + 1) * size;
		int py = (player.y * 2 + 1) * size;

		//초록색으로 채움
		ofSetColor(0, 200, 0);
		ofDrawCircle(px + size / 2, py + size / 2, size / 2 - 2); // 사용자 위치에 원 그리기
	}
	
	// 실시간 점수
	if (playing) {
		char buf[128];
		sprintf(buf, "live total: %d", score);
		ofSetColor(0); // 검정색
		myFont.drawString(buf, 15, ofGetHeight() - 40); // 하단에 출력
	}

	//게임 종료
	if (gameEnded && !playing && player.y == HEIGHT - 1 && player.x == WIDTH - 1) {
		char buf[64];
		sprintf(buf, "GAME ENDED! Total date: %d", total);
		ofSetColor(0);
		myFont.drawString(buf, 15, ofGetHeight() - 60);
	}

	if (bShowInfo) {
		sprintf(str, "comsil project");
		myFont.drawString(str, 15, ofGetHeight() - 80);
	}
	// Total date를 출력
	if (isdfs || isBFS || isDijkstra && !playing) {
		char buf[128];
		sprintf(buf, "Total algorithm date: %d", total);
		ofSetColor(0); // 검정색
		myFont.drawString(buf, 15, ofGetHeight() - 20); // 하단에 출력
	}
} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}
} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled but it can be checked here
	if(key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}
	
	// 종료 상태로 설정
	if (player.y == HEIGHT - 1 && player.x == WIDTH - 1) {
		gameEnded = true;
	}

	//게임 재시작
	if (key == 'r' && !playing && player.y == HEIGHT - 1 && player.x == WIDTH - 1) {
		player = { 0, 0 };
		score = 0;
		playing = true;
		cout << "restart" << endl;
		gameEnded = false;
	}


	if (playing) {
		// 방향 설정
		int moveY[4] = { -1, 1, 0, 0 };
		int moveX[4] = { 0, 0, -1, 1 };
		int direction = -1;  // 이동 방향 초기값

		// 키보드 입력을 기반으로 이동 방향 결정
		switch (key) {
		case 'w': direction = 0; break; // 위
		case 's': direction = 1; break; // 아래
		case 'a': direction = 2; break; // 왼쪽
		case 'd': direction = 3; break; // 오른쪽
		default: return;               // 무시
		}

		// 이동하려는 다음 위치 계산
		int nextY = player.y + moveY[direction];
		int nextX = player.x + moveX[direction];

		// 미로 범위를 벗어나면 무시
		if (nextY < 0 || nextY >= HEIGHT || nextX < 0 || nextX >= WIDTH) {
			return;
		} 

		// 벽 위치 계산 
		int wall_y = player.y * 2 + 1 + moveY[direction];
		int wall_x = player.x * 2 + 1 + moveX[direction];

		// 벽이 열려 있는 경우에만 이동 가능
		if (input[wall_y][wall_x] == ' ') {
			player = { nextY, nextX };  // 플레이어 위치 갱신

			// 이동한 통로에 가중치가 있다면 점수 누적
			if (wallWeight[wall_y][wall_x] > 0)
				score += wallWeight[wall_y][wall_x];

			// 도착지에 도달하면 게임 종료
			if (nextY == HEIGHT - 1 && nextX == WIDTH - 1) {
				cout << "total: " << score << endl;
				total = score;     // 최종 점수를 total에 저장
				playing = false;   // 게임 모드 종료
			}
		}
	}
} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;

	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		string fileName = openFileResult.getName();
		printf("file name is %s\n", fileName.c_str());
		filePath = openFileResult.getPath();

		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(filePath);

			if (!file.exists()) {
				cout << "Target file does not exist." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}
			//일시적 데이터 저장
			ofBuffer buffer(file);

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress

			int input_flag = 0;

			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다

			int rows = 0;
			int cols = 0;
			//.maz 파일에서 길이 받아 오기


			for (auto line : buffer.getLines()) { // https://openframeworks.cc/ko/learning/08_other/readfiletostringvector/ 
				if (rows == 0) {
					cols = line.length();
				}
				rows++;
			}

			HEIGHT = (rows - 1) / 2;
			WIDTH = (cols - 1) / 2;
			if (HEIGHT == 0 || WIDTH == 0) {
				input_flag = 1;
				return false;
			}

			input = new char* [rows];

			int i = 0;
			//.maz 파일의 각 줄을 input 배열에 복사하여 저장
			for (auto line : buffer.getLines()) {
				input[i] = new char[cols + 1];
				strcpy(input[i], line.c_str()); // string 클래스의 객체를 char * 타입으로 변환
				i++;
			}
			
			visited = new int* [HEIGHT];
			check = new Point * [HEIGHT];
			dist = new int* [HEIGHT];
			temp = new int * [HEIGHT * 2 + 1];
			for (int i = 0; i < HEIGHT; i++) {
				visited[i] = new int[WIDTH];
				dist[i] = new int[WIDTH];
				check[i] = new Point[WIDTH];
			}
		
			for (int i = 0; i < HEIGHT * 2 + 1; i++) {
				temp[i] = new int[WIDTH * 2 + 1];
			}

			//벽 가중치 설정
			wallWeight = new int* [HEIGHT * 2 + 1];
			for (int i = 0; i < HEIGHT * 2 + 1; ++i) {
				wallWeight[i] = new int[WIDTH * 2 + 1];
				for (int j = 0; j < WIDTH * 2 + 1; ++j) {
					if (input[i][j] == ' ') {
						int weights[] = { 1, 5, 10, 15 }; // 가중치 값 미리 정의
						wallWeight[i][j] = weights[rand() % 4]; // 랜덤 가중치를 weights 4개중 하나로 선정
					}
					else {
						wallWeight[i][j] = -1; // 벽은 -1로 설정
					}
				}
			}
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
	return true;
}

//메모리 해제 함수
void ofApp::freeMemory() {
	if (input != nullptr && temp) {
		for (int i = 0; i < HEIGHT * 2 + 1; ++i) {
			delete[] input[i];
			delete[] temp[i];
			delete[] wallWeight[i];
		}
		delete[] input;
		delete[] temp;
		delete[] wallWeight;
		input = nullptr;
		temp = nullptr;
		wallWeight = nullptr;
	}

	if (check != nullptr) {
		for (int i = 0; i < HEIGHT; ++i)
			delete[] check[i];
		delete[] check;
		check = nullptr;
	}
	if (dist != nullptr) {
		for (int i = 0; i < HEIGHT; ++i)
			delete[] dist[i];
		delete[] dist;
		dist = nullptr;
	}
}

//DFS탐색을 하는 함수
//TO DO
//DFS탐색을 하는 함수 ( 3주차)
bool ofApp::DFS() {
	s.push({ 0, 0 });

	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			visited[i][j] = 0;
			check[i][j] = { -1, -1 };
		}
	}

	for (int i = 0; i < HEIGHT * 2 + 1; ++i) {
		for (int j = 0; j < WIDTH * 2 + 1; ++j) {
			temp[i][j] = 0;
		}
	}

	visited[0][0] = 1;
	Point finish = { -1, -1 };
	int dy[4] = { -1, 0, 1, 0 };
	int dx[4] = { 0, 1, 0, -1 };

	while (!s.empty()) {
		Point cur = s.top();

		if (cur.y == HEIGHT - 1 && cur.x == WIDTH - 1) {
			finish = cur;
			break;
		}

		bool moved = false;

		for (int i = 0; i < 4; ++i) {
			int vy = cur.y + dy[i];
			int vx = cur.x + dx[i];
			int iy = cur.y * 2 + 1 + dy[i];
			int ix = cur.x * 2 + 1 + dx[i];

			if (vy >= 0 && vy < HEIGHT && vx >= 0 && vx < WIDTH) {
				if (!visited[vy][vx] && input[iy][ix] == ' ') {
					visited[vy][vx] = 1;
					check[vy][vx] = { cur.y, cur.x };
					temp[iy][ix] = 1;
					s.push({ vy, vx });
					moved = true;
					break;
				}
			}
		}
		if (!moved) {
			s.pop();
		}
	}

	if (finish.x == -1) {
		return false;
	}

	stack<Point> path;        // 최종 경로를 저장할 스택
	Point cur = finish;       // 도착점부터 시작 (역추적 시작점)
	cost = 0;                 // 누적 이동 비용 초기화

	// 시작점 (0,0)에 도달할 때까지 경로를 역추적
	while (!(cur.y == 0 && cur.x == 0)) {
		Point prev = check[cur.y][cur.x];  // 현재 지점의 이전 위치를 가져옴

		// 이전 위치와 현재 위치의 차이를 통해 방향 계산
		int dx = cur.x - prev.x;
		int dy = cur.y - prev.y;

		// 두 점 사이의 벽 위치(통로 위치) 계산
		int wall_x = prev.x * 2 + 1 + dx;
		int wall_y = prev.y * 2 + 1 + dy;

		// 통로 가중치가 유효한 경우 누적 비용에 더함
		if (wallWeight[wall_y][wall_x] > 0)
			cost += wallWeight[wall_y][wall_x];

		path.push(cur);  // 현재 지점을 경로에 추가
		cur = prev;      // 다음 역추적 위치로 이동
	}

	// 시작점도 경로에 포함
	path.push({ 0, 0 });

	// 최종 경로를 전역 경로 스택에 저장
	s = path;

	// 총 비용 저장
	total = cost;

	isdfs = true;
	isBFS = false;
	isDijkstra = false; 

	return true;
}

//TO DO 
//DFS를 수행한 결과를 그린다. (3주차 내용)
void ofApp::dfsdraw() {
	int size = 20;

	ofSetColor(255, 0, 0);
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (visited[y][x]) {
				int drawX = (x * 2 + 1) * size;
				int drawY = (y * 2 + 1) * size;
				ofDrawRectangle(drawX, drawY, size, size);
			}
		}
	}

	for (int y = 0; y < HEIGHT * 2 + 1; y++) {
		for (int x = 0; x < WIDTH * 2 + 1; x++) {
			if (temp[y][x]) {
				int drawX = x * size;
				int drawY = y * size;
				ofDrawRectangle(drawX, drawY, size, size);
			}
		}
	}

	//최종 경로
	ofSetColor(0, 0, 255);
	stack<Point> S = s; // s는 DFS/BFS/Dijkstra 수행 후 최종 경로가 저장된 스택
	if (S.empty()) return; // 경로가 없으면 함수 종료

	Point cur = S.top(); S.pop(); // 경로의 시작점 pop

	while (!S.empty()) {
		Point next = S.top(); S.pop(); // 다음 점 pop
		
		// 현재 셀 위치를 화면상의 픽셀 위치로 변환하여 사각형으로 그림
		int cx = (cur.x * 2 + 1) * size;
		int cy = (cur.y * 2 + 1) * size;
		ofDrawRectangle(cx, cy, size, size);
		// 두 셀 사이의 이동 방향 계산 (다음 셀 - 현재 셀)
		int dx = next.x - cur.x;
		int dy = next.y - cur.y;

		// 현재 셀에서 다음 셀로 가는 벽의 위치 계산
		int wall_x = cur.x * 2 + 1 + dx;
		int wall_y = cur.y * 2 + 1 + dy;

		// 연결 통로가 실제로 열려 있으면 (' ')
		if (input[wall_y][wall_x] == ' ') {
			// 연결된 벽 사이도 함께 파란색으로 그림
			int wx = wall_x * size;
			int wy = wall_y * size;
			ofDrawRectangle(wx, wy, size, size);
		}

		cur = next; //다음 반복 갱신
	}
	
	//마지막 셀 그리기
	ofDrawRectangle(((WIDTH - 1) * 2 + 1) * size, ((HEIGHT - 1) * 2 + 1) * size, size, size);
}

bool ofApp::BFS() {
	q.push({0, 0});

	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			visited[i][j] = 0;
			check[i][j] = { -1, -1 };
		}
	}
	for (int i = 0; i < HEIGHT * 2 + 1; ++i) {
		for (int j = 0; j < WIDTH * 2 + 1; ++j) {
			temp[i][j] = 0;
		}
	}

	visited[0][0] = 1;
	Point finish = { -1, -1 };
	int dy[4] = { -1, 0, 1, 0 };
	int dx[4] = { 0, 1, 0, -1 };

	while (!q.empty()) {
		Point cur = q.front(); q.pop();

		if (cur.y == HEIGHT - 1 && cur.x == WIDTH - 1) {
			finish = cur;
			break;
		}

		for (int i = 0; i < 4; ++i) {
			int vy = cur.y + dy[i];
			int vx = cur.x + dx[i];
			int iy = cur.y * 2 + 1 + dy[i];
			int ix = cur.x * 2 + 1 + dx[i];

			if (vy >= 0 && vy < HEIGHT && vx >= 0 && vx < WIDTH) {
				if (!visited[vy][vx] && input[iy][ix] == ' ') {
					visited[vy][vx] = 1;
					check[vy][vx] = { cur.y, cur.x };
					temp[iy][ix] = 1;
					q.push({ vy, vx });
				}
			}
		}
	}

	if (finish.x == -1) {
		return false;
	}

	// 경로 역추적
	stack<Point> path; // 최종 경로를 저장할 스택
	Point cur = finish; // 도착점부터 시작
	cost = 0;  // 누적 이동 비용 초기화

	// 시작점 (0,0)에 도달할 때까지 경로를 역추적
	while (!(cur.y == 0 && cur.x == 0)) {
		Point prev = check[cur.y][cur.x]; // 현재 지점의 이전 위치를 가져옴

		// 이전 위치와 현재 위치의 차이를 통해 방향 계산
		int dx = cur.x - prev.x;
		int dy = cur.y - prev.y;
		// 두 점 사이의 벽 위치를 계산
		int wall_x = prev.x * 2 + 1 + dx;
		int wall_y = prev.y * 2 + 1 + dy;
		// 벽의 가중치가 0보다 큰 경우 누적 비용에 추가
		if (wallWeight[wall_y][wall_x] > 0)
			cost += wallWeight[wall_y][wall_x];

		path.push(cur); // 현재 지점을 경로에 추가
		cur = prev; // 다음 역추적 위치로 이동
	}
	// 시작점도 경로에 포함
	path.push({ 0, 0 });
	// 최종 경로를 전역 경로 스택에 저장
	s = path;
	// 총 비용 저장
	total = cost;

	isBFS = true;
	isDijkstra = false; 
	isdfs = false; 
	return true;
}

void ofApp::bfsdraw() {
	int size = 20;

	ofSetColor(255, 0, 0);
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (visited[y][x]) {
				int drawX = (x * 2 + 1) * size;
				int drawY = (y * 2 + 1) * size;
				ofDrawRectangle(drawX, drawY, size, size);
			}
		}
	}
	for (int y = 0; y < HEIGHT * 2 + 1; y++) {
		for (int x = 0; x < WIDTH * 2 + 1; x++) {
			if (temp[y][x]) {
				int drawX = x * size;
				int drawY = y * size;
				ofDrawRectangle(drawX, drawY, size, size);
			}
		}
	}

	ofSetColor(0, 0, 255);
	stack<Point> S = s;
	if (S.empty()) {
		return;
	}
	Point cur = S.top(); S.pop();

	while (!S.empty()) {
		Point next = S.top(); S.pop();

		int cx = (cur.x * 2 + 1) * size;
		int cy = (cur.y * 2 + 1) * size;
		ofDrawRectangle(cx, cy, size, size);

		int dx = next.x - cur.x;
		int dy = next.y - cur.y;
		int wall_x = cur.x * 2 + 1 + dx;
		int wall_y = cur.y * 2 + 1 + dy;

		if (input[wall_y][wall_x] == ' ') {
			int wx = wall_x * size;
			int wy = wall_y * size;
			ofDrawRectangle(wx, wy, size, size);
		}
		cur = next;
	}

	ofDrawRectangle(((WIDTH - 1) * 2 + 1) * size, ((HEIGHT-1) * 2 + 1)*size, size, size);
}

bool ofApp::Dijkstra() {
	//  배열 초기화
	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			dist[i][j] = 10000;  // 최단 거리 배열 초기화
			check[i][j] = { -1, -1 };
		}
	}
	// 경로 시각화용 배열 초기화
	for (int i = 0; i < HEIGHT * 2 + 1; ++i)
		for (int j = 0; j < WIDTH * 2 + 1; ++j)
			temp[i][j] = 0;

	// 우선순위 큐로 Dijkstra 
	priority_queue<State, vector<State>, greater<State>> pq;
	dist[0][0] = 0;
	pq.push({ 0, {0, 0} }); // 시작 위치 삽입

	// 도착점 좌표 저장
	Point finish = { -1, -1 };
	int dy[4] = { -1, 0, 1, 0 };
	int dx[4] = { 0, 1, 0, -1 };

	// Dijkstra 알고리즘 시작
	while (!pq.empty()) {
		State cur = pq.top(); pq.pop();
		int y = cur.p.y, x = cur.p.x;
		if (dist[y][x] < cur.dist) continue;

		if (y == HEIGHT - 1 && x == WIDTH - 1) {
			finish = { y, x };
			break;
		}
		// 4방향으로 이동 가능한 경로 탐색
		for (int i = 0; i < 4; ++i) {
			int ny = y + dy[i], nx = x + dx[i];
			int wall_y = y * 2 + 1 + dy[i], wall_x = x * 2 + 1 + dx[i];
			// 미로 내부 및 통로일 경우
			if (ny >= 0 && ny < HEIGHT && nx >= 0 && nx < WIDTH && input[wall_y][wall_x] == ' ') {
				int cost = wallWeight[wall_y][wall_x]; // 벽의 가중치
				if (cost > 0 && dist[y][x] + cost < dist[ny][nx]) {
					// 더 짧은 경로 발견 시 거리 갱신 및 이전 위치 기록
					dist[ny][nx] = dist[y][x] + cost;
					check[ny][nx] = { y, x };
					temp[wall_y][wall_x] = 1;  // 시각화용 통과 경로 저장
					pq.push({ dist[ny][nx], { ny, nx } });
				}
			}
		}
	}
	// 도달 실패한 경우
	if (finish.x == -1) return false;
	
	// 경로 역추적
	stack<Point> path;
	Point cur = finish;
	while (!(cur.y == 0 && cur.x == 0)) {
		path.push(cur);
		cur = check[cur.y][cur.x];
	}
	path.push({ 0, 0 });
	s = path;

	total = dist[HEIGHT - 1][WIDTH - 1]; //dijkstra의 누적 최소 비용 total에 저장

	isDijkstra = true;
	isBFS = false;
	isdfs = false;
	return true;
}

void ofApp::dijkstraDraw() {
	int size = 20;

	// 방문한 모든 셀 그리기 (BFS/DFS와 유사하지만 시각화를 위해 dist 배열 기반)
	ofSetColor(255, 0, 0); // 방문한 셀은 빨간색
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (dist[y][x] != 10000) { // 방문한 셀만 그리기
				int drawX = (x * 2 + 1) * size;
				int drawY = (y * 2 + 1) * size;
				ofDrawRectangle(drawX, drawY, size, size);
			}
		}
	}

	// 경로 탐색의 일부였던 벽 그리기 (temp 배열)
	for (int y = 0; y < HEIGHT * 2 + 1; y++) {
		for (int x = 0; x < WIDTH * 2 + 1; x++) {
			if (temp[y][x]) {
				int drawX = x * size;
				int drawY = y * size;
				ofDrawRectangle(drawX, drawY, size, size);
			}
		}
	}

	//최종 경로
	ofSetColor(0, 0, 255);
	stack<Point> S = s; // s는 DFS/BFS/Dijkstra 수행 후 최종 경로가 저장된 스택
	if (S.empty()) return; // 경로가 없으면 함수 종료

	Point cur = S.top(); S.pop(); // 경로의 시작점 pop

	while (!S.empty()) {
		Point next = S.top(); S.pop(); // 다음 점 pop

		// 현재 셀 위치를 화면상의 픽셀 위치로 변환하여 사각형으로 그림
		int cx = (cur.x * 2 + 1) * size;
		int cy = (cur.y * 2 + 1) * size;
		ofDrawRectangle(cx, cy, size, size);
		// 두 셀 사이의 이동 방향 계산 (다음 셀 - 현재 셀)
		int dx = next.x - cur.x;
		int dy = next.y - cur.y;

		// 현재 셀에서 다음 셀로 가는 벽의 위치 계산
		int wall_x = cur.x * 2 + 1 + dx;
		int wall_y = cur.y * 2 + 1 + dy;

		// 연결 통로가 실제로 열려 있으면 (' ')
		if (input[wall_y][wall_x] == ' ') {
			// 연결된 벽 사이도 함께 파란색으로 그림
			int wx = wall_x * size;
			int wy = wall_y * size;
			ofDrawRectangle(wx, wy, size, size);
		}

		cur = next; //다음 반복 갱신
	}

	//마지막 셀 그리기
	ofDrawRectangle(((WIDTH - 1) * 2 + 1) * size, ((HEIGHT - 1) * 2 + 1) * size, size, size);
}
