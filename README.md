# Adventurer

## 프로젝트 개요

**C++ 및 OpenFrameworks 기반의 인터랙티브 미로 게임 + `.maz` 생성기 포함**

사용자는 미로의 시작점 (0, 0)에서 출발하여 키보드로 캐릭터를 조작하며 도착 지점까지 도달하는 것이 목표입니다.

- 알고리즘: DFS, BFS, Dijkstra 알고리즘 활용해서 경로 탐색 구현 및 시각화
- 게임 모드: 사용자가 직접 움직이며 최단 경로를 체험
- 가중치: 통로마다 무작위 가중치 적용을 통해 모험가가 길을 탐험 가
- `.maz` 생성기 포함: C 기반 makefile 프로젝트로 `.maz` 텍스트 기반 미로 파일 생성 가능

---

## 폴더 구조
```plaintext
adventurer/
├── game/                 # 미로 게임 본체
│   ├── bin/data/         # .maz 파일이 위치해야 하는 곳
│   ├── src/
│   └── adventurer.sln
├── generator/            # .maz 생성기 (C언어 기반, makefile 포함)
│   ├── maze.c
│   ├── makefile
├── README.md
├── .gitignore

---

## 실행 환경

- 개발 도구: Visual Studio 2022  
- 프레임워크: OpenFrameworks 0.12.0  
- 필수 라이브러리: [ofxWinMenu](https://github.com/bangnoise/ofxWinMenu) (메뉴 UI 용)

---

## 실행 방법

1. 본 프로젝트를 OpenFrameworks의 경로에 맞게 복사: C:/openFrameworks/apps/myApps/adventurer/
2. `adventurer.sln` 파일을 Visual Studio 2022로 열기  
3. Debug 또는 Release 모드로 빌드 후 실행

> ⚠️ 프로젝트가 로드되지 않는 경우, `.vcxproj` 파일 내부의 `openFrameworks*.props` 경로가 올바른지 확인하세요.

---

## .maz 파일 생성 및 적용 방법

1. `generator/` 폴더 안의 `makefile`을 이용해 `.maz` 생성기 빌드
    ```bash
    cd generator
    make
    ./maze           # 또는 실행해서 width, height 입력 후 maze.maz 파일 생성
    ```

2. 생성된 `.maz` 파일을 아래 경로로 이동:
    ```
    game/bin/data/maze.maz
    ```

3. 게임 실행 후 `File > Open` 메뉴를 통해 `.maz` 파일 열기
   
---

## 조작 방법

| 동작 | 키 또는 메뉴 |
|------|---------------|
| 미로 파일 열기 | 메뉴: `File > Open` (확장자: `.maz`) | ⚠️필수수
| 정답 경로 보기 | 메뉴: `View > DFS / BFS / Dijkstra` 중 선택 |
| 게임 시작 | 메뉴: `Game > Start` |
| 이동 | 키보드 `W`, `A`, `S`, `D` |
| 재시작 | 키보드 `R` (도착 지점 도달 후) |
| 전체화면 | `F` 키 누르기 |

---

## 주요 기능

- DFS, BFS, Dijkstra 경로 탐색 및 시각화  
- 사용자 직접 조작 + 실시간 점수 누적  
- 가중치 기반 통로 구성  
- 도착 후 결과 메시지 및 점수 출력  
- 메뉴에서 설명(About) 확인 가능  
- 알고리즘 경로 활성화 후 수동 이동으로 실시간 비교 가능

---

## 참고 및 출처

- OpenFrameworks 공식 예제 기반  
- ofxWinMenu: [https://github.com/bangnoise/ofxWinMenu](https://github.com/bangnoise/ofxWinMenu)

---

## 프로젝트 정보

- 제작자: 조형준  
- 제작일: 2025-06-23
