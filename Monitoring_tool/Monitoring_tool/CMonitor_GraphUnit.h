#pragma once
#include"Queue.h"
#define dfMAXCHILD	100
#define Max 100
#define MaxString 100


class CMonitor_GraphUnit
{
public:

	enum TYPE
	{
		BAR_SINGLE_VERT,
		BAR_SINGLE_HORZ,
		BAR_COLUMN_VERT,
		BAR_COLUMN_HORZ,
		LINE_SINGLE,
		LINE_MULTI,
		PIE,
		SWITCH
	};
	typedef struct ST_HWNDtoTHIS
	{
		HWND hWnd[dfMAXCHILD];
		CMonitor_GraphUnit *pThis[dfMAXCHILD];
	}stHWNDtoTHIS;



private:

	Queue<int> **queue;

	HWND hWnd;
	HWND hWnd_Parent;

	HINSTANCE hInst;							// 현재 인스턴스 입니다.
	WCHAR szWindowClass[MaxString];				// 기본 창 클래스 이름입니다.

	

	TYPE GraphType;								// 현재 창 그래프 타입입니다.
	COLORREF BG_Color;
	int Queue_cnt;								// Queue갯수.
	RECT MemSize;

	//메모리 DC를 사용하기 위해 필요한 것들.
	HDC hMemDC;
	HBITMAP hMemDC_Bitmap;
	HBITMAP hMemDC_OldBitmap;

protected :
	
	//======================================================
	//윈도우 프로시저에서 자기자신의 this를 접근하기 위한 테이블 설정과
	//윈도우 핸들, this 포인터 매칭 테이블 관리.
	//======================================================
	BOOL PutThis (void);
	static BOOL RemoveThis (HWND hWnd);
	static CMonitor_GraphUnit *GetThis (HWND hWnd);


public :

	//=========================================
	//생성자. 윈도우를 생성하고 초기화 시켜준다.
	//=========================================
	CMonitor_GraphUnit (HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight, int DataQueue_Num = 1);

	//=========================================
	//파괴자. 동적할당 받은 queue를 파괴하고 삭제한다.
	//=========================================
	~CMonitor_GraphUnit (void)
	{
		int cnt;
		RemoveThis (hWnd);
		for ( cnt = 0; cnt < Queue_cnt; cnt++ )
		{
			delete queue[cnt];
		}
		delete[]queue;

		SelectObject (hMemDC, hMemDC_OldBitmap);
		DeleteObject (hMemDC_Bitmap);
		DeleteObject (hMemDC);
		
	}



	//==============================================
	//윈도우 프로시저
	//==============================================
	static LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//==============================================
	//데이터 입력 함수
	//==============================================
	BOOL InitData (int Data, int Line=0);

	//==============================================
	//Client 화면 크기 변경시 메모리 DC재작업 함수
	//==============================================
	void Flip_MemDC (HDC hdc);


	//==============================================
	//그래프 함수
	//==============================================
	void Line_Single (void);
	
};