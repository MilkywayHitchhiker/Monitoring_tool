#pragma once
#include"Queue.h"
#define dfMAXCHILD	100
#define MaxString 100

#define UM_Alret WM_USER+1
#define AlarmMax 500					//알람이 작동되면 울리기

#define TitleBarLength 30






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


	struct DataRef
	{
		int _CPUID;									// 해당 모니터에서 받아야되는 CPUID 정보
		int AlretMax;								// 해당 수치에 도달하면 알람 발생.
		int GraphMax;								// 해당그래프의 Max값.
		bool Graph_Flow;							// Max값이 없는 유동그래프일시 true.
		int Queue_cnt;								// Queue갯수.
		int Queue_Max;
	};

	DataRef WData;


private:


	Queue<int> **queue;

	HWND hWnd;
	HWND hWnd_Parent;

	HINSTANCE hInst;							// 현재 인스턴스 입니다.
	WCHAR szWindowClass[MaxString];				// 기본 창 클래스 이름입니다.



	TYPE GraphType;								// 현재 창 그래프 타입입니다.
	WCHAR *Title_Name;

	//각종 색깔들
	COLORREF BG_Color;							// 백그라운드 컬러입니다.
	COLORREF GRAY_Color;						



	HDC hMemDC;									//메모리 DC를 사용하기 위해 필요한 것들.
	HBITMAP hMemDC_Bitmap;
	HBITMAP hMemDC_OldBitmap;

	RECT MemSize;								//메모리 DC총 사이즈
	RECT GraphSize;								//메모리 DC에 그려질 그래프 사이즈
		
	HBRUSH BGBrush;								//백그라운드용 브러쉬

	HBRUSH TitleBrush;							//타이틀용 브러쉬
	HFONT TitleFont;							//타이틀용 폰트
	COLORREF TitleColor;						//타이틀용 펜
	
	HFONT GridFont;								//그리드용 폰트
	HPEN GridPen;								//그리드용 펜

	HPEN LinePen;								//라인용 펜

	HBRUSH OldBrush;
	HFONT OldFont;
	HPEN OldPen;

	ULONG64 Alarm_SetTime;


protected:

	//======================================================
	//윈도우 프로시저에서 자기자신의 this를 접근하기 위한 테이블 설정과
	//윈도우 핸들, this 포인터 매칭 테이블 관리.
	//======================================================
	BOOL PutThis (void);
	static BOOL RemoveThis (HWND hWnd);
	static CMonitor_GraphUnit *GetThis (HWND hWnd);




public:

	//=========================================
	//생성자. 윈도우를 생성하고 초기화 시켜준다.
	//=========================================
	CMonitor_GraphUnit (HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight);

	//=========================================
	//파괴자. 동적할당 받은 queue를 파괴하고 삭제한다.
	//=========================================
	~CMonitor_GraphUnit (void)
	{
		int cnt;
		RemoveThis (hWnd);
		for ( cnt = 0; cnt < WData.Queue_cnt; cnt++ )
		{
			delete queue[cnt];
		}
		delete[]queue;


		//SelectObject로 기본 브러쉬와 기본 폰트, 기본 펜으로 되돌린 후 전역에 미리 셋팅해논 모든 폰트와 펜,브러쉬를 삭제한다.
		SelectObject (hMemDC, OldBrush);
		SelectObject (hMemDC, OldFont);
		SelectObject (hMemDC, OldPen);

		DeleteObject (BGBrush);
		
		DeleteObject (TitleBrush);
		DeleteObject (TitleFont);

		DeleteObject (GridFont);
		DeleteObject (GridPen);

		DeleteObject (LinePen);

		SelectObject (hMemDC, hMemDC_OldBitmap);
		DeleteObject (hMemDC_Bitmap);
		DeleteObject (hMemDC);

	}

	//=========================================
	//그래프 구성에 필요한 데이터 수집 정보를 받고 이를 저장한다. AlretMax와 GraphMax의 경우 0일 경우 작동을 안한다.
	//=========================================
	void SetInformation (WCHAR *TitleName, int CPUID, int DataQueue_Num = 1, int QueueMax = 2, int GraphMax = 0, int AlretMax = 0);

	//==============================================
	//윈도우 프로시저
	//==============================================
	static LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//==============================================
	//데이터 입력 함수
	//==============================================
	BOOL InitData (int Data, int CPUID, int Line = 0);
	void Alarm (void);


	//==============================================
	//Client 화면 크기 변경시 메모리 DC재작업 함수
	//==============================================
	void Flip_MemDC (HDC hdc);

	//==============================================
	//Title바, Greed 작업
	//==============================================
	void Title (void);
	void Grid (void);


	//==============================================
	//그래프 함수
	//==============================================
	void Line_Single (void);



};