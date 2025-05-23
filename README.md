## 프로젝트 개요

이 프로젝트는 MFC(Microsoft Foundation Classes)를 기반으로 동작하는 미로 생성 및 풀이 프로그램이다.  
사용자 입력을 통해 행과 열 크기를 지정한 후 DFS 기반 랜덤 미로를 생성하고, BFS 또는 DFS를 이용해 경로를 탐색할 수 있다.  
GUI 상에서 출발지와 목적지를 마우스로 지정하며, 애니메이션을 통해 생성 및 탐색 과정을 시각적으로 확인할 수 있다.

## 주요 기능

- GUI 구성:
  - 대화 상자(Dialog-based) MFC 애플리케이션
  - 미로 생성, 풀이, 애니메이션 설정, 출발지/도착지 선택 등 인터페이스 포함
- 미로 생성:
  - DFS 기반 랜덤 미로 생성
  - 단 하나의 해답이 존재하며 완전 연결된 미로 생성
  - 미로 생성 애니메이션 지원 (옵션)
- 미로 풀이:
  - BFS 또는 DFS 알고리즘을 사용한 경로 탐색
  - 탐색 구간과 최종 경로 시각화
  - 풀이 과정 애니메이션 지원 (옵션)
- 사용자 인터랙션:
  - 마우스 호버 시 해당 방 강조
  - 클릭을 통한 출발지 및 도착지 지정 (좌클릭: 출발, 우클릭: 도착)
- 기타 기능:
  - 더블 버퍼링을 통한 깜빡임 방지
  - 미로 크기: 4×4 ~ 30×30 정사각형 또는 직사각형 지원

## 사용 기술

- 언어: C++
- 플랫폼: Windows
- 프레임워크: MFC (Microsoft Foundation Classes)
- 주요 알고리즘:
  - DFS (미로 생성)
  - DFS / BFS (미로 탐색)
- 기타 구현 요소:
  - Sleep 함수로 애니메이션 지연 제어
  - WM_MOUSEMOVE, WM_LBUTTONDOWN, WM_RBUTTONDOWN 메시지 핸들링
  - Double buffering
