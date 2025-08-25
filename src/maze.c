#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maze.h"

stat **maze;
int N, M;

int main() {
    srand(time(NULL));
	printf("Width : ");
	scanf("%d",&N);
	printf("Height : ");
	scanf("%d",&M);

    initMaze(N, M);
    
    makeMaze();

    printMaze("output.maz");
    freeMaze(N, M);
    
    printf("Successful\n");

    return 0;
}

// 미로 생성 전체 과정
void makeMaze() {
    //첫 줄 초기화
    for (int j = 0; j < N; j++) {
        maze[0][j].set = j;
    }

    if(M == 1){
        for (int i = 0; i < N; i++) {
            maze[0][i].right = 0;
        }
        return;
    }else if(N == 1){
        for(int i = 0; i < M; i++){
            maze[i][0].down = 0;
        }
    }

    //첫 줄
    initrightwall();
    downwall(0);

    // 1 ~ m - 2까지
    for(int i = 1; i < M - 1; i++){
        rightwall(i);
        downwall(i);
    }
    
    //마지막 줄
    rightwall(M - 1);
}

// 0 또는 1 반환
int myrandom() {
    return rand() % 2;
}

// 초기 right
void initrightwall() {
    for (int i = 0; i < N - 1; i++) {
        if (maze[0][i].set != maze[0][i+1].set) {
            //벽 없는 경우
            if (myrandom()) {
                maze[0][i].right = 0;
                maze[0][i+1].set = maze[0][i].set;
            }
        }
    }
}

void rightwall(int row) {
    for (int i = 0; i < N - 1; i++) {
        if (maze[row][i].set != maze[row][i+1].set) {
            if (row == M - 1 || (maze[row-1][i+1].set != maze[row][i+1].set) || myrandom()) {
                maze[row][i].right = 0;
                int nextset = maze[row][i+1].set;
                int curset = maze[row][i].set;
                for (int k = 0; k < N; k++) {
                    if (maze[row][k].set == nextset) {
                        maze[row][k].set = curset;
                    }
                }
            }
        }
    }
}

void downwall(int row) {
    int i = 0;
    while (i < N) {
        int group_start = i;

        // 같은 set 그룹 찾기
        while (i + 1 < N && maze[row][i + 1].set == maze[row][i].set) {
            i++;
        }
        int group_end = i;

        int check = 0;
        int prev_down = 0; // 연속 방지용

        // 후보들을 순차적으로 확인
        for (int j = group_start; j <= group_end; j++) {
            //연속 방지
            if (prev_down == 1) {
                prev_down = 0; 
                continue;
            }
            if (myrandom()) {
                maze[row][j].down = 0;
                maze[row + 1][j].set = maze[row][j].set;
                check = 1;
                prev_down = 1;
            } else{
                prev_down = 0;
            }
        }
        // 하나도 선택되지 않았으면 하나 선택 (연속 방지도 고려)
        if (!check) {
            maze[row][group_start].down = 0;
            maze[row + 1][group_start].set = maze[row][group_start].set;
        }

        i = group_end + 1;
    }
}

// 미로 초기화
void initMaze(int width, int height) {
    int count = 1;
    maze = (stat**)malloc(sizeof(stat*) * height);
    for (int i = 0; i < height; i++) {
        maze[i] = (stat*)malloc(sizeof(stat) * width);
        for (int j = 0; j < width; j++) {
            maze[i][j].right = 1;
            maze[i][j].down = 1;
            maze[i][j].set = count++;
        }
    }
}

// 미로 출력
void printMaze(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fail to open");
        return;
    }

    // 윗 경계 출력
    fprintf(fp, "+");
    for (int j = 0; j < N; j++) {
        fprintf(fp, "---");
        fprintf(fp, "+");
    }
    fprintf(fp, "\n");

    for (int i = 0; i < M; i++) {
        
        // 미로 내부(세로벽)
        fprintf(fp, "|");
        for (int j = 0; j < N; j++) {
            fprintf(fp, "   ");
            
            if(maze[i][j].right || j == N-1){
                fprintf(fp, "|");
            }
            else {
                fprintf(fp, " ");
            }
        }
        fprintf(fp, "\n");
        
        // 가로벽
        fprintf(fp, "+");
        for (int j = 0; j < N; j++) {
            if(maze[i][j].down || i == M - 1){
                fprintf(fp, "---");
            }
            else {
                fprintf(fp, "   ");
            }
            fprintf(fp, "+");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void freeMaze(int width, int height) {
    for (int i = 0; i < height; i++) {
        free(maze[i]);  
    }
    free(maze);
}

