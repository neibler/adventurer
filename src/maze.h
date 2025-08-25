#ifndef MAZE_H
#define MAZE_H

// 구조체 정의
typedef struct {
    int right;  // 오른쪽 벽 (1: 있음, 0: 없음)
    int down;   // 아래쪽 벽 (1: 있음, 0: 없음)
    int set;    // 집합 번호
} stat;

// 함수 선언
void makeMaze(void);
int myrandom(void);
void initrightwall();
void rightwall(int row);
void downwall(int row);
void initMaze(int width, int height);
void printMaze(const char *filename);
void freeMaze(int width, int height);

#endif
