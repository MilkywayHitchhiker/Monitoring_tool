#pragma once
#include"Queue.h"
#define dfMAXCHILD	100
#define MaxString 100

#define UM_Alret WM_USER+1
#define AlarmMax 500					//알람이 작동되면 울리기

#define TitleBarLength 30
#define BarNameLength 55
#define BarNamespace 20

#define dfTitleMax 30
#define dfunitMax 30

#define Pen_Max 10
#define Bar_Max 5






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


	typedef struct Culumn_INFO
	{
		ULONG u64ServerID;							// 해당 모니터에서 받아야되는 ServerID 정보
		int iType;									// 해당 모니터에서 받을 타입정보
		WCHAR Column_Name[dfTitleMax];
		Queue<int> *DataArray;							//해당 모니터의 queue
	}stColumnInfo;

	stColumnInfo *ColumnArray;


private:



	HWND hWnd;
	HWND hWnd_Parent;

	HINSTANCE hInst;							// 현재 인스턴스 입니다.
	WCHAR TitleName[dfTitleMax];				// 해당창의 타이틀에 들어갈 이름입니다.
	WCHAR unit[dfunitMax];						// 해당 인스턴스의 수치의 표시 단위 입니다.

	TYPE GraphType;								// 현재 창 그래프 타입입니다.


	int Column_Max;								// Culumn의갯수.
	int Queue_NodeMax;							// Queue안의 노드의 Max값.

	int Alarm_Max;								// 해당 수치에 도달하면 알람 발생.
	int Graph_Max;								// 해당그래프의 Max값.
	bool Graph_Flow;							// Max값이 없는 유동그래프일시 true.

	//각종 색깔들
	COLORREF BG_Color;							// 백그라운드 컬러입니다.
	

	HDC hMemDC;									//메모리 DC를 사용하기 위해 필요한 것들.
	HBITMAP hMemDC_Bitmap;
	HBITMAP hMemDC_OldBitmap;

	RECT MemSize;								//메모리 DC총 사이즈
	RECT GraphSize;								//메모리 DC에 그려질 그래프 사이즈
		
	HBRUSH BGBrush;								//백그라운드용 브러쉬


	HBRUSH TitleBrush;							//타이틀용 브러쉬
	HFONT TitleFont;							//타이틀용 폰트
	COLORREF TitleColor;						//타이틀용 텍스트 색깔
	COLORREF AlarmColor;						//알람이 작동할 경우 타이틀 색깔이 빨강색으로 바뀜.
	bool AlarmFlag;								//알람이 작동할 경우 true로 바뀜.
	
	HFONT GridFont;								//그리드용 폰트
	HPEN GridPen;								//그리드용 펜

	HFONT additionFont;								//라인 부가 설명용 폰트
	HPEN additionPen;								//라인 부가 설명용 펜
	HBRUSH additionBrush;							//라인 부가 설명용 브러쉬

	HPEN LinePen[Pen_Max];							//라인용 펜
	HBRUSH BARBRUSH[Bar_Max];						//Bar용 색깔 브러쉬
	HFONT BARDataFont;								//Bar 가운데 수치 표시

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
	CMonitor_GraphUnit (WCHAR * Title, WCHAR *ColumnUnit, HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight);

	//=========================================
	//파괴자. 동적할당 받은 queue를 파괴하고 삭제한다.
	//=========================================
	~CMonitor_GraphUnit (void)
	{
		int cnt;
		RemoveThis (hWnd);
		for ( cnt = 0; cnt < Column_Max; cnt++ )
		{
			delete ColumnArray[cnt].DataArray;
		}
		delete[]ColumnArray;

		//SelectObject로 기본 브러쉬와 기본 폰트, 기본 펜으로 되돌린 후 전역에 미리 셋팅해논 모든 폰트와 펜,브러쉬를 삭제한다.
		SelectObject (hMemDC, OldBrush);
		SelectObject (hMemDC, OldFont);
		SelectObject (hMemDC, OldPen);

		DeleteObject (BGBrush);
		
		DeleteObject (TitleBrush);
		DeleteObject (TitleFont);

		DeleteObject (GridFont);
		DeleteObject (GridPen);

		DeleteObject (additionFont);								//라인 부가 설명용 폰트
		DeleteObject (additionPen);									//라인 부가 설명용 펜
		DeleteObject (additionBrush);								//라인 부가 설명용 브러쉬

		for ( int cnt = 0; cnt < Pen_Max; cnt++ )
		{
			DeleteObject (LinePen[cnt]);
		}
		for ( int cnt = 0; cnt < Bar_Max; cnt++ )
		{
			DeleteObject (BARBRUSH[cnt]);
		}
		DeleteObject (BARDataFont);

		SelectObject (hMemDC, hMemDC_OldBitmap);
		DeleteObject (hMemDC_Bitmap);
		DeleteObject (hMemDC);

	}

	//=========================================
	//그래프 구성에 필요한 데이터 수집 정보를 받고 이를 저장한다. AlretMax와 GraphMax의 경우 0일 경우 작동을 안한다.
	//=========================================
	void CMonitorGraphUnit (int CulumnMax = 1, int QueueNodeMax = 2, int GraphMax = 0, int AlretMax = 0);
	void DataColumnInfo (int ColumnNum, WCHAR *ColumnName, ULONG ServerID, int Type);

	//==============================================
	//윈도우 프로시저
	//==============================================
	static LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//==============================================
	//데이터 입력 함수
	//==============================================
	BOOL InitData ( ULONG ServerID, int Data, int iType);
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
	void MultLine_addition (void);
	void Bottom_NameBar (void);

	//==============================================
	//그래프 함수
	//==============================================
	void Print_Line_Single (void);
	void Print_Line_Multi (void);
	void Print_Bar_Single (void);



};