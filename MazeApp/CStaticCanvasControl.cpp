// CStaticCanvasControl.cpp: 구현 파일
//
#include <cstdlib>
#include <time.h>
#include <Windows.h>
#include <string.h>

#include "pch.h"
#include "MazeApp.h"
#include "CStaticCanvasControl.h"

char map[63][63];
char copy_map[63][63];
char copy_copy_map[63][63];
int maze[4][2]{ {0, 2}, {0, -2}, {-2, 0}, {2, 0} };	//상,하,좌,우 순서
int solve_maze[4][2]{ {0, 1}, {0, -1}, {-1, 0}, {1, 0} };	//상,하,좌,우 순서
int _queue[63 * 63 * 2];
int front = 0, rear = 0;
bool check = 0;
int g_row = 0, g_column = 0;
// CStaticCanvasControl

IMPLEMENT_DYNAMIC(CStaticCanvasControl, CStatic)

CStaticCanvasControl::CStaticCanvasControl()	//생성자에서 모든 멤버 변수 초기화
	:margin(1)
	,m_white_brush(RGB(255, 255, 255))
	,m_black_pen(PS_SOLID, 2, RGB(0, 0, 0))
	,board_size(0)
	,rect_size(0)
	,maze_size(0)
	,maze_height(0)
	,maze_width(0)
	,animate(FALSE)
	,current_mouse_point(-1, -1)
    ,solve(0)
	,solving_method(-1)
	,sx(0)
	,sy(0)
	,ex(0)
	,ey(0)
	{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < 64; i++) {		//short_map 멤버변수 초기화
		for (int j = 0; j < 64; j++) {
			short_map[i][j] = '1';
		}
	}
}

CStaticCanvasControl::~CStaticCanvasControl() {
}


BEGIN_MESSAGE_MAP(CStaticCanvasControl, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CStaticCanvasControl 메시지 처리기

void CStaticCanvasControl::OnPaint() {	//흰 색으로 바탕을 모두 칠해주는 함수
	CPaintDC dc(this);
	CRect rectWindow;
	GetClientRect(&rectWindow);
	GetClientRect(&this->m_rect_window);
	CBrush whiteBrush(RGB(255, 255, 255));	//흰 색 브러시
	dc.FillRect(&rectWindow, &whiteBrush);
}

void CStaticCanvasControl::DrawMazeOnGeneration(int row, int column, BOOL animate) {	//Generation 버튼을 누를 경우 실행
	Invalidate();
	CPaintDC dc(this);
	g_row = row;			// row 전역변수에 저장
	g_column = column;		// column 전역변수에 저장
	dc.FillRect(&(this->m_rect_window), &(this->m_white_brush));		//멤버 변수들에 값 저장
	this->board_size = min(this->m_rect_window.Height(), this->m_rect_window.Width()) - (this->margin << 1);
	this->rect_size = board_size / max(row, column);
	this->maze_height = this->rect_size * row;
	this->maze_width = this->rect_size * column;
	this->animate = animate;

	int sy = this->margin + (this->m_rect_window.Height() - maze_height >> 1);	//시작 지점을 잡아줌
	int sx = this->margin + (this->m_rect_window.Width() - maze_width >> 1);

	for (int y = sy; y < sy + maze_height; y += this->rect_size) {		
		for (int x = sx; x < sx + maze_width; x += this->rect_size) {
			CRect r(x, y, x + this->rect_size, y + this->rect_size);
			dc.Rectangle(r);	//입력한 값을 토대로 사각형(미로 틀)을 그려줌
		}
	}

	for (int i = 0; i < row * 2 + 1; i++) {			// 미로를 담을 배열을 1로 초기화
		for (int j = 0; j < column * 2 + 1; j++) {
			map[i][j] = '1';
		}
	}

	this->sx = -1, this->sy = -1, this->ex = -1, this->ey = -1;		// 시작 지점, 끝 지점을 초기화해줌
	this->L.x = -1, this->L.y = -1, this->R.x = -1, this->R.y = -1;

	int random_x = (rand() % column) * 2 + 1;		// 랜덤으로 x, y값을 지정해서 그 위치부터 미로를 뚫어줌
	int random_y = (rand() % row) * 2 + 1;
	map[random_y][random_x] = '0';					//시작 지점 '0'으로 바꿔줌

	this->DrawMaze(random_x, random_y, row, column, animate);	// 미로를 그려주는 함수 호출

	for (int i = 0; i < 64; i++) {					//미로 배열을 카피해줌
		strcpy_s(copy_map[i], map[i]);
	}
}

void CStaticCanvasControl::DrawMaze(int x, int y, int row, int col, BOOL animate) {
	Invalidate();
	CPaintDC dc(this);
	CBrush* old_brush = dc.SelectObject(&this->m_white_brush);
	int new_x, new_y;
	int random = rand() % 4;	//랜덤으로 상, 하, 좌, 우를 뜻하는 값을 만들어줌
	
	for (int i = 0; i < 4; i++) {
		new_x = x + maze[(i + random) % 4][0];	//새로운 x, y좌표는 랜덤으로 바뀜
		new_y = y + maze[(i + random) % 4][1];

		if (((x - 1 < 0) || (x + 1 > col * 2 + 1) || (y - 1 < 0) || (y + 1 > row * 2 + 1)) == 0
			&& map[new_y][new_x] == '1') {	// x, y 좌표가 범위 밖을 벗어나지 않을 조건, 벽이 이미 뚫렸을 때 실행하지 않을 조건
			int rect_x = this->margin + (this->m_rect_window.Width() - maze_width >> 1) + (new_x / 2) * rect_size;
			int rect_y = this->margin + (this->m_rect_window.Height() - maze_height >> 1) + (new_y / 2) * rect_size;

			if (new_x == x) {
				map[new_y][x] = '0';
				if (new_y - y > 0) {		// 상
					map[new_y - 1][x] = '0';
					//4가지의 경우의 수에 따라 벽까지 지워지는 코드 생성
					CRect r(rect_x + 1, rect_y - 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
					dc.FillRect(&r, old_brush);
					if (animate) Sleep(50);	// animate 버튼이 눌려 있는 경우만 Sleep 실행
				}
				else {						// 하
					map[new_y + 1][x] = '0';
					CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size + 1);
					dc.FillRect(&r, old_brush);
					if (animate) Sleep(50);
				}
			}
			else {
				map[y][new_x] = '0';
				if (new_x - x > 0) {		// 우
					map[y][new_x - 1] = '0';
					CRect r(rect_x - 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
					dc.FillRect(&r, old_brush);
					if (animate) Sleep(50);
				}
				else {						// 좌
					map[y][new_x + 1] = '0';
					CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size + 1, rect_y + this->rect_size - 1);
					dc.FillRect(&r, old_brush);
					if (animate) Sleep(50);
				}
			}

			this->DrawMaze(new_x, new_y, row, col, animate);	// 1개의 벽을 뚫고 다음 벽을 재귀함수로 뚫음
		}
	}
}

void CStaticCanvasControl::Mouse_Hovering() {	//마우스 호버링을 구현한 함수
	Invalidate();
	CPaintDC dc(this);
	CPen* old_pen = dc.SelectObject(&this->m_black_pen);
	CBrush* old_brush = dc.SelectObject(&this->m_white_brush);
	int map_x = 1, map_y = 1;
	int sy = this->margin + (this->m_rect_window.Height() - maze_height >> 1);
	int sx = this->margin + (this->m_rect_window.Width() - maze_width >> 1);

	this->Maze_Back();		//탐색한 경로, 벽, 탐색하지 않은 경로를 따로 처리해서 시각화해주는 함수

	int i = 1, j = 1;
	for (int y = sy; y < sy + maze_height; y += this->rect_size) {		// 미로의 사이즈만큼 반복
		j = 1;
		for (int x = sx; x < sx + maze_width; x += this->rect_size) {
			CRect r(x, y, x + this->rect_size, y + this->rect_size);	// 미로 벽을 그릴 사이즈
			CRect r1(x + 1, y + 1, x + this->rect_size - 1, y + this->rect_size - 1);	//벽 안쪽 칸 사이즈
			
			if (x <= this->current_mouse_point.x && this->current_mouse_point.x < x + this->rect_size
				&& y <= this->current_mouse_point.y && this->current_mouse_point.y < y + this->rect_size) {
				CBrush blue_brush(RGB(100, 255, 255));	//현재 마우스 위치가 있는 곳을 파란 색으로 채워줌
				dc.FillRect(&r1, &blue_brush);
			}
			if (x <= this->L.x && this->L.x < x + this->rect_size		//L 빨강점
				&& y <= this->L.y && this->L.y < y + this->rect_size) {
				CBrush red_brush(RGB(255, 0, 0));		// 마우스 좌클릭을 했을 때의 좌표라면 빨간색 원을 그려줌
				dc.SelectObject(&red_brush);
				dc.Ellipse(&r1);
				this->sx = j;		// 현재의 위치를 배열에 저장하기 위해 멤버 변수에 담아줌
				this->sy = i;

			}
			if (x <= this->R.x && this->R.x < x + this->rect_size		//R 파랑점
				&& y <= this->R.y && this->R.y < y + this->rect_size) {
				CBrush blue_brush(RGB(0, 0, 255));		// 마우스 우클릭을 했을 때의 좌표라면 파란색 원을 그려줌
				dc.SelectObject(&blue_brush);
				dc.Ellipse(&r1);
				this->ex = j;
				this->ey = i;
			}
			j += 2;
		}
		i += 2;
	}
	if(solve) this->short_road(ex, ey);		//가장 짧은 길을 표시해주는 함수 호출

	dc.SelectObject(old_brush);
	dc.SelectObject(old_pen);
	UpdateWindow();
}

void CStaticCanvasControl::OnMouseMove(UINT nFlags, CPoint point) {
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	this->current_mouse_point = point;		//현재 마우스 포인터 값을 멤버 변수에 저장
	if(this->board_size)
		this->Mouse_Hovering();				//마우스 호버링 함수 호출
	CStatic::OnMouseMove(nFlags, point);
}

void CStaticCanvasControl::DFS(int sx, int sy, int ex, int ey, BOOL animate) {	// DFS 방식으로 미로를 탐색하는 코드
	Invalidate();
	CPaintDC dc(this);
	CBrush blue_brush(RGB(0, 150, 200));
	int new_x, new_y;
	for (int i = 0; i < 4; i++) {
		new_x = sx + solve_maze[i][0];	// 새로운 좌표 생성
		new_y = sy + solve_maze[i][1];

		int rect_x = this->margin + (this->m_rect_window.Width() - maze_width >> 1) + (new_x / 2) * rect_size;	
		int rect_y = this->margin + (this->m_rect_window.Height() - maze_height >> 1) + (new_y / 2) * rect_size;
		//화면 안에서의 좌표
		if (check) return;	// 목적지를 찾았으면 이전의 반복문도 모두 종료

		if (new_y == ey && new_x == ex) {	//목적지를 찾은 경우 실행
			copy_map[new_y][new_x] = '3';
			check = 1;
			if (new_x == sx) {		//short_map에 어떤 경로에서 왔는지 기록해줌
				if (new_y > sy) {
					short_map[new_y][new_x] = 1;	//상
				}
				else {
					short_map[new_y][new_x] = 0;	//하
				}
			}
			else {
				if (new_x > sx) {
					short_map[new_y][new_x] = 2;	//우
				}
				else {
					short_map[new_y][new_x] = 3;	//좌
				}
			}
			return;	//함수 실행 종료
		}

		if (((sx - 1 < 0) || (sx + 1 > g_column * 2 + 1) || (sy - 1 < 0) || (sy + 1 > g_row * 2 + 1)) == 0
			&& copy_map[new_y][new_x] == '0') {	//배열 밖을 벗어나지 않고, 지나가는 곳이 길인 조건

			if (new_x == sx) {
				copy_map[new_y][sx] = '2';	//지나간 길을 따로 표시해줌
				if (new_y > sy) {			//short_map에 이전 경로 표시
					short_map[new_y][new_x] = 1;	//상
				}
				else {
					short_map[new_y][new_x] = 0;	//하
				}
			}
			else {
				copy_map[sy][new_x] = '2';
				if (new_x > sx) {
					short_map[new_y][new_x] = 2;	//우
				}
				else {
					short_map[new_y][new_x] = 3;	//좌
				}
			}

			if (new_x == sx) {			// 맵에 파란 색으로 경로를 표시해줌
				copy_map[new_y][sx] = '2';
				if ((new_x % 2) == 1 && (new_y % 2) == 1) {
					if (new_y > sy) {							// 상
						CRect r(rect_x + 1, rect_y - 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
						dc.FillRect(&r, &blue_brush);
					}
					else {										// 하
						CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size + 1);
						dc.FillRect(&r, &blue_brush);
					}
					
					if(animate) Sleep(50);	// animate 버튼이 눌려 있을 때만 
				}
			}
			else {
				copy_map[sy][new_x] = '2';
				if ((new_x % 2) == 1 && (new_y % 2) == 1) {
					if (new_x > sx) {							//우
						CRect r(rect_x - 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
						dc.FillRect(&r, &blue_brush);
					}
					else {										//좌
						CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size + 1, rect_y + this->rect_size - 1);
						dc.FillRect(&r, &blue_brush);
					}
					if (animate) Sleep(50);
				}
			}

			this->DFS(new_x, new_y, ex, ey, animate);	//재귀 함수로 DFS를 구현했음, DFS를 새로운 좌표로 호출
		}
	}
}
void CStaticCanvasControl::BFS(int sx, int sy, int ex, int ey, BOOL animate) {
	front = 0;
	rear = 0;
	_queue[rear++] = sx;
	_queue[rear++] = sy;

	Invalidate();
	CPaintDC dc(this);
	CBrush blue_brush(RGB(0, 150, 200));

	while (front != rear) {		// front와 rear이 같아질 때까지 반복
		int x1 = _queue[front++];
		int y1 = _queue[front++];

		for (int i = 0; i < 4; i++) {

			int new_x = x1 + solve_maze[i][0];	// x, y 좌표를 변경해준 새로운 좌표값
			int new_y = y1 + solve_maze[i][1];

			int rect_x = this->margin + (this->m_rect_window.Width() - maze_width >> 1) + (new_x / 2) * rect_size;
			int rect_y = this->margin + (this->m_rect_window.Height() - maze_height >> 1) + (new_y / 2) * rect_size;
			//화면 안에서의 좌표

			if (new_x == ex && new_y == ey) {	// 목적지를 찾은 경우 실행
				copy_map[new_y][new_x] = '3';
				if (new_x == x1) {			// short_map에 이전 경로를 저장해줌
					if (new_y > y1) {
						short_map[new_y][new_x] = 1;	// 상
					}
					else {
						short_map[new_y][new_x] = 0;	// 하
					}
				}
				else {
					if (new_x > x1) {
						short_map[new_y][new_x] = 2;	// 우
					}
					else {
						short_map[new_y][new_x] = 3;	// 좌
					}
				}
				return;
			}

			if (((x1 - 1 < 0) || (x1 + 1 > g_column * 2 + 1) || (y1 - 1 < 0) || (y1 + 1 > g_row * 2 + 1)) == 0
				&& copy_map[new_y][new_x] == '0') {		//배열 밖을 벗어나지 않고, 지나가는 곳이 길인 조건

				if (new_x == x1) {		// short_map에 이전 경로를 저장해줌
					if (new_y > y1) {
						short_map[new_y][new_x] = 1;	// 상
					}
					else {
						short_map[new_y][new_x] = 0;	// 하
					}
				}
				else {
					if (new_x > x1) {
						short_map[new_y][new_x] = 2;	// 우
					}
					else {
						short_map[new_y][new_x] = 3;	// 좌
					}
				}

				if (new_x == x1) {		// 맵에 파란 색으로 경로를 표시해줌
					copy_map[new_y][x1] = '2';
					if ((new_x % 2) == 1 && (new_y % 2) == 1) {
						if (new_y > y1) {							// 상
							CRect r(rect_x + 1, rect_y - 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
							dc.FillRect(&r, &blue_brush);
						}
						else {										// 하
							CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size + 1);
							dc.FillRect(&r, &blue_brush);
						}

						if (animate) Sleep(20);		// animate 버튼이 눌려 있을 때만 실행
					}
				}
				else {
					copy_map[y1][new_x] = '2';
					if ((new_x % 2) == 1 && (new_y % 2) == 1) {
						if (new_x > x1) {							//우
							CRect r(rect_x - 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
							dc.FillRect(&r, &blue_brush);
						}
						else {										//좌
							CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size + 1, rect_y + this->rect_size - 1);
							dc.FillRect(&r, &blue_brush);
						}
						if (animate) Sleep(20);
					}
				}
				_queue[rear++] = new_x;
				_queue[rear++] = new_y;
			}
		}
	}
}

void  CStaticCanvasControl::ClickSolveMazeButton(int solving_method, BOOL animate) {
	Invalidate();
	CPaintDC dc(this);
	CBrush blue_brush(RGB(0, 150, 200));
	this->solving_method = solving_method;
	this->solve = 0;
	for (int i = 0; i < 64; i++) {			//short_map을 초기화해줌
		for (int j = 0; j < 64; j++) {
			short_map[i][j] = '1';
		}
	}

	for (int i = 0; i < 64; i++) {			//map을 copy_map에 복사해줌
		strcpy_s(copy_map[i], map[i]);
	}

	this->Maze_Back();
	int rect_x = this->margin + (this->m_rect_window.Width() - maze_width >> 1) + (sx / 2) * rect_size;
	int rect_y = this->margin + (this->m_rect_window.Height() - maze_height >> 1) + (sy / 2) * rect_size;
	CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
	CBrush red_brush(RGB(255, 0, 0));
	this->Mouse_Hovering();
	dc.FillRect(&r, &blue_brush);
	dc.SelectObject(&red_brush);
	dc.Ellipse(&r);			// 시작 지점을 파란색으로 칠하고 빨간색 점을 표시해줌

	if (solving_method) {	// solving_method에 따라 BFS, DFS를 따로 호출
		check = 0;
		copy_map[sy][sx] = '2';
		this->BFS(sx, sy, ex, ey, animate); 
		this->solve = 1;		//solve를 1로 바꿔줘서 short_road를 호버링에서 호출할지 판단함
		this->Mouse_Hovering();
	}
	else {
		check = 0;
		copy_map[sy][sx] = '2';
		this->DFS(sx, sy, ex, ey, animate);
		this->solve = 1;
		this->Mouse_Hovering();
	}
	
	this->short_road(ex, ey);
}

void CStaticCanvasControl::Maze_Back() {	//미로에서 탐색한 경로, 아닌 경로를 계속 판단할 수 있게 하는 함수
	Invalidate();
	CPaintDC dc(this);
	CPen* old_pen = dc.SelectObject(&this->m_black_pen);
	CBrush* old_brush = dc.SelectObject(&this->m_white_brush);
	CBrush blue_brush(RGB(0, 150, 200));

	int row = g_row;
	int col = g_column;

	for (int i = 0; i < 64; i++) {		//미로 탐색을 한번 더 진행할 예정이기 때문에 새롭게 배열을 카피해줌
		strcpy_s(copy_copy_map[i], copy_map[i]);
	}
	front = 0;
	rear = 0;

	_queue[rear++] = 1;
	_queue[rear++] = 1;

	int rect_x = this->margin + (this->m_rect_window.Width() - maze_width >> 1);
	int rect_y = this->margin + (this->m_rect_window.Height() - maze_height >> 1);

	CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
	if (copy_copy_map[1][1] == '0') dc.FillRect(&r, old_brush);
	else dc.FillRect(&r, &blue_brush);
	
	while (front != rear) {
		int x1 = _queue[front++];
		int y1 = _queue[front++];

		for (int i = 0; i < 4; i++) {
			int new_x = x1 + solve_maze[i][0];	//x, y 좌표를 변경해준 새로운 좌표값
			int new_y = y1 + solve_maze[i][1];

			rect_x = this->margin + (this->m_rect_window.Width() - maze_width >> 1) + (new_x / 2) * rect_size;
			rect_y = this->margin + (this->m_rect_window.Height() - maze_height >> 1) + (new_y / 2) * rect_size;

			if (((x1 - 1 < 0) || (x1 + 1 > col * 2 + 1) || (y1 - 1 < 0) || (y1 + 1 > row * 2 + 1)) == 0
				&& (copy_copy_map[new_y][new_x] == '0' || copy_copy_map[new_y][new_x] == '2' 
					|| copy_copy_map[new_y][new_x] == '3')) {
				//지나간 곳을 '6'으로 두고 벽이나 탐색한 곳이 아니고, 범위를 벗어나지 않을 때 아래 코드 진행
 				if (new_x == x1) {
					if ((new_x % 2) == 1 && (new_y % 2) == 1) {
						if (new_y > y1) {							// 상
							CRect r(rect_x + 1, rect_y - 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
							if (copy_copy_map[new_y][new_x] == '0') dc.FillRect(&r, old_brush);
							//지나간 곳이 아닌 경우 흰색으로 칠하기
							else dc.FillRect(&r, &blue_brush);
							//그게 아니라면 파란 색으로 칠하기
							
						}
						else {										// 하
							CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size + 1);
							if (copy_copy_map[new_y][new_x] == '0') dc.FillRect(&r, old_brush);
							else dc.FillRect(&r, &blue_brush);
						}
					}
				}
				else {
					if ((new_x % 2) == 1 && (new_y % 2) == 1) {
						if (new_x > x1) {							//우
							CRect r(rect_x - 1, rect_y + 1, rect_x + this->rect_size - 1, rect_y + this->rect_size - 1);
							if (copy_copy_map[new_y][new_x] == '0') dc.FillRect(&r, old_brush);
							else dc.FillRect(&r, &blue_brush);
						}
						else {										//좌
							CRect r(rect_x + 1, rect_y + 1, rect_x + this->rect_size + 1, rect_y + this->rect_size - 1);
							if (copy_copy_map[new_y][new_x] == '0') dc.FillRect(&r, old_brush);
							else dc.FillRect(&r, &blue_brush);
						}
					}
				}
				copy_copy_map[new_y][new_x] = '6';
				_queue[rear++] = new_x;		//bfs 방식으로 탐색함
				_queue[rear++] = new_y;
			}
		}
	}

	g_row = row;		//전역변수가 자꾸 이상한 값으로 변경되어서 계속 지정해줌
	g_column = col;

	dc.SelectObject(old_brush);
	dc.SelectObject(old_pen);
	UpdateWindow();
}

void CStaticCanvasControl::OnLButtonDown(UINT nFlags, CPoint point) {
	if (this->board_size) {
		this->L = point;		//현재 마우스 위치를 멤버 변수에 저장
		solve = 0;
		this->Mouse_Hovering();	//마우스 호버링 호출
	}
	CStatic::OnLButtonDown(nFlags, point);
}


void CStaticCanvasControl::OnRButtonDown(UINT nFlags, CPoint point) {
	if (this->board_size) {
		this->R = point;		//현재 마우스 위치를 멤버 변수에 저장
		solve = 0;
		this->Mouse_Hovering();	//마우스 호버링 호출
	}
	CStatic::OnRButtonDown(nFlags, point);
}

void CStaticCanvasControl::short_road(int ex, int ey) {
	Invalidate();
	CPaintDC dc(this);
	CPen* old_pen = dc.SelectObject(&this->m_black_pen);
	CBrush* old_brush = dc.SelectObject(&this->m_white_brush);
	CBrush red_brush(RGB(255, 0, 0));
	
	int new_x = ex + solve_maze[short_map[ey][ex]][0];	//이전 경로를 찾아서 가져옴
	int new_y = ey + solve_maze[short_map[ey][ex]][1];

	int rect_x = this->margin + (this->m_rect_window.Width() - maze_width >> 1) + (new_x / 2) * rect_size + rect_size / 2;
	int rect_y = this->margin + (this->m_rect_window.Height() - maze_height >> 1) + (new_y / 2) * rect_size + rect_size / 2;
	//선을 그어줄 x, y좌표를 지정해줌

	if (new_x % 2 == 1 && new_y % 2 == 1) {	// 배열에서 홀수 칸일 때만 실행
		if (short_map[ey][ex] == 0) {		//상
			CRect r(rect_x - 3, rect_y - 3, rect_x + 3, rect_y - this->rect_size - 3);
			dc.FillRect(&r, &red_brush);
		}
		else if (short_map[ey][ex] == 1) {	//하
			CRect r(rect_x - 3, rect_y - 3, rect_x + 3, rect_y + this->rect_size + 3);
			dc.FillRect(&r, &red_brush);
		}
		else if (short_map[ey][ex] == 2) {	//좌
			CRect r(rect_x, rect_y - 3, rect_x + this->rect_size + 3, rect_y + 3);
			dc.FillRect(&r, &red_brush);
		}
		else if (short_map[ey][ex] == 3) {	//우
			CRect r(rect_x, rect_y - 3, rect_x - this->rect_size - 3, rect_y + 3);
			dc.FillRect(&r, &red_brush);
		}
	}
	
	dc.SelectObject(old_brush);
	dc.SelectObject(old_pen);
	UpdateWindow();

	if ((ex != this->sx) || (ey != this->sy)) short_road(new_x, new_y);	//재귀함수로 새로운 좌표에서 이를 실행
}