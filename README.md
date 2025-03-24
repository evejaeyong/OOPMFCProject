## 프로젝트 개요

MFC 기반 윈도우 애플리케이션으로, 미로를 자동 생성하고 사용자의 입력을 통해 미로를 해결하는 프로그램을 구현함.  
DFS 또는 BFS 알고리즘을 이용해 경로를 탐색하며, 애니메이션 효과 및 사용자 인터랙션(출발지/도착지 선택, 마우스 호버링)을 포함함.  
프로그램은 완전 연결된 단일 해답을 갖는 미로를 랜덤하게 생성하며, DFS 기반의 미로 생성 알고리즘을 사용함.

## 주요 기능

- 대화 상자 기반 MFC GUI
- DFS 기반 랜덤 미로 생성
- BFS/DFS 기반 미로 풀이 기능
- 애니메이션을 통한 미로 생성 및 탐색 시각화
- 마우스 호버링 및 클릭을 통한 출발지/도착지 선택
- Double Buffering을 이용한 화면 깜빡임 방지

## 사용 기술

- 언어: C++
- 플랫폼: Microsoft Windows
- 프레임워크: MFC (Microsoft Foundation Classes)
- 알고리즘: DFS (Depth-First Search), BFS (Breadth-First Search)
- 구현 제한: STL 미사용 (vector, queue, stack 등 직접 구현)
