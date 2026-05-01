🏰 Random Defense

플레이어가 직접 벽을 설치해 미로를 만드는 BFS 기반 타워 디펜스 게임

C++ / openFrameworks로 구현한 데스크톱 디펜스 게임입니다. 일반적인 디펜스 게임과 달리 고정된 경로가 없고, 플레이어가 맵 위에 벽을 자유롭게 설치하면 적이 BFS 알고리즘으로 매번 최단 경로를 다시 계산해 거점을 향해 다가옵니다.
서강대학교 소프트웨어개발도구 및 환경실습 최종 프로젝트로 제작되었습니다.

🎮 게임 플레이

목표: 적이 거점(파란 블록)에 도달하지 못하도록 벽과 포탑으로 막기
진행: 정비 단계에서 벽/포탑을 배치 → 라운드 시작 → 적 처치 → 보상으로 다음 라운드 준비
승리 조건: 15라운드까지 거점 방어 성공
패배 조건: HP가 0이 되면 게임 오버 (적이 거점 도달 시 HP -1)

핵심 규칙

거점은 매 게임 시작마다 랜덤한 위치에 2×2 크기로 생성됩니다
적은 항상 (행 6, 열 0) 위치에서 출발합니다
벽으로 적의 경로를 완전히 차단할 수 없습니다 (BFS로 검증)
정비 단계(라운드 음수)에만 벽/포탑 설치·철거가 가능합니다


✨ 주요 기능
기능설명🧱 벽 설치/철거20코인으로 설치, 10코인에 판매🗼 포탑 설치50코인으로 벽 위에 설치⬆️ 포탑 업그레이드Lv1 → Lv2 (100코인) → Lv3 (200코인)🛣️ 경로 미리보기정비 시간 동안 적의 이동 예정 경로를 빨간 점으로 표시💰 코인 시스템라운드 클리어 시 250 × 라운드 코인 획득❤️ HP 시스템초기 3, 적이 거점 도달 시 -1
포탑 스펙
레벨사거리공격속도공격력비용Lv 11.515 frame3550 코인Lv 22.512 frame50+100 코인Lv 33.58 frame100+200 코인

🛠️ 기술 스택

언어: C++
프레임워크: openFrameworks
IDE: Visual Studio
플랫폼: Windows
애드온: ofxWinMenu — Windows 메뉴바 생성


📂 프로젝트 구조
src/
├── main.cpp           # 진입점, 윈도우 1440x960 생성
├── ofApp.h            # 메인 클래스 헤더 (struct, 전역 변수 정의)
├── ofApp.cpp          # 게임 로직 (setup/update/draw/mousePressed/BFS)
├── ofxWinMenu.h       # 윈도우 메뉴 애드온
└── ofxWinMenu.cpp
핵심 함수
함수역할setup()변수 초기화, 동적 메모리 할당, 메뉴 생성update()적 이동, 포탑 공격, 라운드 진행 (20 FPS)draw()맵·UI·적·포탑 렌더링 (20 FPS)mousePressed()버튼 클릭 / 맵 셀 클릭 처리BFS()적의 최단 경로 계산 + 경로 가능 여부 검증

🧠 핵심 알고리즘: BFS
이 프로젝트의 심장은 **너비 우선 탐색(BFS)**입니다. 다음 세 가지 상황에서 호출됩니다.

게임 시작 시 — 초기 경로 계산
벽/포탑 설치 시 — "이 벽을 놓아도 거점까지 길이 있는가?" 검증
벽 철거 / 라운드 시작 시 — 경로 재계산

동작 방식
cppqueue<pair<int,int>> q;
q.push({6, 0});               // 시작점
visited[6][0] = 1;

while (!q.empty() && !found) {
    auto [sy, sx] = q.front(); q.pop();
    
    if (거점 영역 도달) found = true;
    
    for (4방향) {
        if (맵 밖 || 방문함 || 벽) skip;
        visited[ny][nx] = 1;
        prev[ny][nx] = {sy, sx};   // 부모 추적
        q.push({ny, nx});
    }
}

// prev를 거꾸로 따라가며 path 복원
prev 배열로 BFS 트리에서 거점부터 거꾸로 거슬러 올라가 path를 만듭니다. 적의 위치는 좌표가 아닌 path 배열의 인덱스로 표현되므로, enemypos-- 한 줄로 전진을 구현할 수 있습니다.
벽 설치 검증 트릭
cppmaze[y][x] = 1;        // 일단 벽 놓아보고
if (!BFS()) {          // 경로가 사라졌으면
    maze[y][x] = 0;    // 롤백
    BFS();
    return;
}

📊 자료구조 설계
Maze 셀 값 인코딩
maze[y][x] 한 값으로 블록 종류 + 상태를 함께 표현합니다.
값의미0빈 칸1벽2 / 20Lv1 포탑 (쿨타임 중 / 공격 가능)3 / 30Lv2 포탑4 / 40Lv3 포탑100거점
ones digit으로 쿨타임 상태를 구분해 draw()에서 색상의 불투명도를 다르게 줍니다.
라운드 부호 트릭
round 변수의 부호로 게임 페이즈를 표현합니다.

round <= 0 — 정비 시간 (벽/포탑 배치 가능)
round > 0 — 전투 중 (적 이동, 포탑 공격)

라운드 시작 시 round = abs(round) + 1, 종료 시 round *= -1 한 줄로 페이즈 전환이 끝납니다.
프레임 기반 타이밍
별도 타이머 없이 frame % N == 0 패턴만으로 모든 시간 제어를 합니다.
cppframe = (frame + 1) % 10000;          // 매 update 증가, 오버플로 방지
if (frame % 10 == 0)         { 적 이동 }
if (frame % towers[i].rate == 0) { 포탑 공격 }
포탑 레벨이 올라가면 rate가 줄어 (15→12→8) 자연스럽게 공격 속도가 빨라집니다.

🚀 빌드 및 실행
사전 요구사항

Windows 10 이상
Visual Studio 2017 이상
openFrameworks 0.11.x for Visual Studio

빌드 방법
bash# 1. openFrameworks 설치 후 apps/myApps/ 디렉토리에 프로젝트 클론
cd of_v0.11.x_vs_release/apps/myApps/
git clone https://github.com/<your-username>/random-defense.git

# 2. ofxWinMenu 애드온을 of_v0.11.x_vs_release/addons/ 에 설치

# 3. Visual Studio로 .sln 파일 열기 후 빌드 (Release / x64)
조작법
입력동작메뉴 → File → Start게임 시작add wall 버튼벽 추가 모드 (셀 클릭으로 설치)sell mod 버튼철거 모드 (벽/포탑 판매)add tower 버튼포탑 설치 모드 (벽 위에만 가능)upgrade tower 버튼업그레이드 모드round start 버튼라운드 시작메뉴 → Help → Rule게임 규칙 안내

⏱️ 시간복잡도
함수복잡도setup, appMenuFunctionO(1) — 맵 크기 고정 (13 × 26)updateO(t × e) — 포탑 수 × 적 수drawO(p + e) — 경로 길이 + 적 수mousePressedO(e + t)BFSO(H × W) — 최악의 경우 전체 맵 순회

🎨 창의적 구현 포인트

미로 빌더 + 디펜스 융합 — 고정 경로가 아닌, 플레이어가 직접 미로를 설계하는 새로운 디펜스 경험
BFS의 이중 활용 — 최단 경로 계산뿐 아니라 "벽 설치 가능 여부 검증"에도 같은 함수를 재사용
인덱스 기반 적 위치 — 좌표 대신 path[] 인덱스를 쓴 덕분에 적 이동이 enemypos-- 한 줄
프레임 모듈로 타이밍 — 타이머 라이브러리 없이 frame % rate만으로 포탑 쿨다운, 적 이동 속도 구현
라운드 부호 트릭 — 정비/전투 페이즈를 별도 enum 없이 부호 하나로 구분


🚧 향후 개선 아이디어

 포탑 공격 시 총알 이펙트 추가
 라운드별 적 난이도 곡선 (HP 증가, 다양한 적 종류)
 특수 능력 포탑 (슬로우, 광역, 관통)
 게임 밸런스 조정 (코인 보상, 포탑 비용)
 메인 메뉴 / 게임 오버 화면 디자인 개선
 BGM 및 효과음
 적 이동 거리 계산식의 미세 버그 수정 (dy*dy + dx + dx → dy*dy + dx*dx)


📖 참고 자료

openFrameworks Documentation
ofxWinMenu by Lynn Jarvis
BFS 알고리즘 — 너비 우선 탐색은 큐(FIFO) 자료구조로 구현하며, 가중치 없는 그래프에서 최단 경로를 보장합니다.


👤 Author
윤찬영 (20241630)
서강대학교 컴퓨터공학과

📝 License
This project was created for educational purposes as part of a university coursework.
The ofxWinMenu addon is licensed under LGPL v3 — see ofxWinMenu.h for details.
