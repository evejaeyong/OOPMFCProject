// MazeAppDlg.h: 헤더 파일
#pragma once
#include "CStaticCanvasControl.h"

// CMazeAppDlg 대화 상자
class CMazeAppDlg : public CDialogEx {
// 생성입니다.
public:
	CMazeAppDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAZEAPP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	afx_msg void OnSolvingRadioGroupClicked(UINT uiID);

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	int m_edit_row;
	int m_edit_column;

	int m_solving_method;
	BOOL m_check_animate_generation;
	BOOL m_check_animate_solve;

	CStaticCanvasControl m_static_canvas_control;

public:
	afx_msg void OnBnClickedButtonGen();
	afx_msg void OnBnClickedButtonSol();
	afx_msg void OnBnClickedExit();
};
