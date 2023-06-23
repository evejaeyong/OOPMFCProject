#pragma once
// CStaticCanvasControl

class CStaticCanvasControl : public CStatic {
	DECLARE_DYNAMIC(CStaticCanvasControl)

public:
	CStaticCanvasControl();
	virtual ~CStaticCanvasControl();

protected:
	DECLARE_MESSAGE_MAP()

private:
	CRect m_rect_window;
	CBrush m_white_brush;
	CPen m_black_pen;
	const int margin;

	int board_size;
	int rect_size;
	int maze_size;
	int maze_height;
	int maze_width;
	BOOL animate;
	CPoint current_mouse_point;
	bool solve;
	int solving_method;
	int sx;
	int sy;
	int ex;
	int ey;
	CPoint L;
	CPoint R;
	char short_map[63][63];

public:
	afx_msg void OnPaint();

	void DrawMazeOnGeneration(int row, int column, BOOL animate);
	void DrawMaze(int x, int y, int row, int col, BOOL animate);
	void Mouse_Hovering();
	void Maze_Back();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	void ClickSolveMazeButton(int solving_method, BOOL animate);
	void DFS(int sx, int sy, int ex, int ey, BOOL animate);
	void BFS(int sx, int sy, int ex, int ey, BOOL animate);

	void short_road(int ex, int ey);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};


