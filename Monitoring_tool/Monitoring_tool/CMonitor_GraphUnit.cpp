#include"stdafx.h"
#include"CMonitor_GraphUnit.h"
#include<time.h>

static CMonitor_GraphUnit::stHWNDtoTHIS Table;
CMonitor_GraphUnit::CMonitor_GraphUnit (WCHAR * Title,HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight)
{
	//모니터 윈도우 네임.
	wsprintfW (TitleName, Title);


	//창 클래스를 등록 합니다.

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof (WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = ( HBRUSH )(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TitleName;
	wcex.hIconSm = NULL;

	RegisterClassExW (&wcex);

	

	//인스턴스 핸들을 클래스 전역변수에 등록하고 프로그램 창을 만든다음에 표시합니다.


	//윈도우를 생성해서 핸들을 넘겨 줍니다.
	hWnd = CreateWindowW (TitleName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		iPosX, iPosY, iWidth, iHeight, hWndParent, nullptr, hInstance, nullptr);

	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	hWnd_Parent = hWndParent;


	if ( !PutThis () )
	{
		return;
	}


	//그래프 타입에 따라서 Queue의 갯수가 달라짐.

	GraphType = enType;
	BG_Color = BackColor;

	HDC hdc = GetDC (hWnd);

	GetClientRect (hWnd, &MemSize);

	hMemDC = CreateCompatibleDC (hdc);
	hMemDC_Bitmap = CreateCompatibleBitmap (hdc, MemSize.right, MemSize.bottom);
	hMemDC_OldBitmap = ( HBITMAP )SelectObject (hMemDC, hMemDC_Bitmap);

	GetClientRect (hWnd, &GraphSize);
	GraphSize.bottom -= TitleBarLength;					// Y 축을 논리좌표 변환을 통해서 뒤집어 졌으므로 Top을 빼는게 아닌 Bottom을 빼야 Top이 빠진다!

	// Flip Y axis
	SetMapMode (hMemDC, MM_ANISOTROPIC);
	SetViewportOrgEx (hMemDC, 0, MemSize.bottom, NULL);
	SetWindowExtEx (hMemDC, MemSize.bottom, MemSize.right, NULL);
	SetViewportExtEx (hMemDC, MemSize.bottom, -MemSize.right, NULL);

	ReleaseDC (hWnd, hdc);

}


LRESULT CALLBACK CMonitor_GraphUnit:: WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	CMonitor_GraphUnit *pThis;
	PAINTSTRUCT ps;
	HDC hdc;


	pThis = GetThis (hWnd);
	if ( NULL == pThis )
	{
		return DefWindowProc (hWnd, message, wParam, lParam);
	}

switch ( message )
{
case WM_CREATE:
	break;
case WM_COMMAND:
	break;
case WM_PAINT:

	hdc = BeginPaint (hWnd, &ps);
	switch ( pThis->GraphType )
	{
	case BAR_SINGLE_VERT :
		break;
	case BAR_SINGLE_HORZ :
		break;
	case BAR_COLUMN_VERT :
		break;
	case BAR_COLUMN_HORZ :
		break;
	case LINE_SINGLE :
		pThis->Print_Line_Single ();
		break;
	case LINE_MULTI :
		pThis->Print_Line_Multi ();
		break;
	};

	pThis->Flip_MemDC (hdc);
	EndPaint (hWnd, &ps);
	break;

case WM_DESTROY:
	PostQuitMessage (0);
	KillTimer (hWnd, 1);
	break;

default:

	return DefWindowProc (hWnd, message, wParam, lParam);

}
return 0;
}


//=========================================
//그래프 구성에 필요한 데이터 수집 정보를 받고 이를 저장한다. AlretMax와 GraphMax의 경우 0일 경우 작동을 안한다.
//=========================================
void CMonitor_GraphUnit::DataColumnInfo (int ColumnNum,WCHAR *ColumnName,ULONG ServerID,int Type)
{
	int cnt;
	//컬럼 Max치보다 컬럼 번호가 넘어갈 경우
	if ( ColumnNum >= Column_Max )
	{
		return;
	}

	for ( cnt = 0; cnt < Column_Max; cnt++ )
	{
		if ( cnt == ColumnNum )
		{
			lstrcpyW (ColumnArray[cnt].Column_Name,ColumnName);
			ColumnArray[cnt].u64ServerID = ServerID;
			ColumnArray[cnt].iType = Type;
		}
	}
	return;

}

void CMonitor_GraphUnit::CMonitorGraphUnit (int CulumnMax, int QueueNodeMax, int DataMax, int AlretMax)
{

	Column_Max = CulumnMax;
	Graph_Max = DataMax;
	Alarm_Max = AlretMax;
	Queue_NodeMax = QueueNodeMax;



	if ( Graph_Max == 0 )
	{
		Graph_Flow = true;
		Graph_Max = 50;
	}
	else
	{
		Graph_Flow = false;
	}

	//그래프 타입에 따라서 Queue의 갯수가 달라짐.
	switch ( GraphType )
	{
	case BAR_SINGLE_VERT:
		break;
	case BAR_SINGLE_HORZ:
		break;
	case BAR_COLUMN_VERT:
		break;
	case BAR_COLUMN_HORZ:
		break;
	case LINE_SINGLE:
		ColumnArray = new stColumnInfo[CulumnMax];
		ColumnArray[0].DataArray = new Queue<int> (Queue_NodeMax);
		break;
	case LINE_MULTI:
		ColumnArray = new stColumnInfo[CulumnMax];
		for ( int cnt = 0; cnt < CulumnMax; cnt++ )
		{
			ColumnArray[cnt].DataArray = new Queue<int> (Queue_NodeMax);
		}
		GraphSize.right -= BarNameLength;
		break;
	}



	BGBrush = CreateSolidBrush (BG_Color);



	TitleBrush = CreateSolidBrush (RGB (max ((GetRValue (BG_Color) - 30), 0), max ((GetGValue (BG_Color) - 30), 0), max ((GetBValue (BG_Color) - 30), 0)));			//타이틀용 브러쉬
	TitleFont = CreateFont (15, 0, 0, 0, 1000, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT ("궁서"));				//타이틀용 폰트
	TitleColor = RGB ((255 - GetRValue (BG_Color)), (255 - GetGValue (BG_Color)), (255 - GetBValue (BG_Color)));
	AlarmColor = RGB (220, 20, 60);

	GridFont = CreateFont (15, 0, 0, 0, 5, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT ("맑은 고딕"));		//그리드용 폰트
	GridPen = CreatePen (PS_SOLID, 1, RGB (28, 28, 28));	//그리드용 펜

	additionFont = GridFont;				//라인 설명용 폰트
	additionPen = GridPen;					//라인 설명용 펜
	additionBrush = CreateSolidBrush (RGB (max ((GetRValue (BG_Color) -50), 0), max ((GetGValue (BG_Color) - 50), 0), max ((GetBValue (BG_Color) - 50), 0)));;


	LinePen[0] = CreatePen (PS_SOLID, 2, RGB (255, 255, 255));								//라인용 펜
	LinePen[1] = CreatePen (PS_SOLID, 2, RGB (155, 155, 130));
	LinePen[2] = CreatePen (PS_SOLID, 2, RGB (100, 100, 70));
	LinePen[3] = CreatePen (PS_SOLID, 2, RGB (50, 50, 50));
	LinePen[4] = CreatePen (PS_SOLID, 2, RGB (0, 155, 155));
	LinePen[5] = CreatePen (PS_SOLID, 2, RGB (100, 0, 50));
	LinePen[6] = CreatePen (PS_SOLID, 2, RGB (30, 70, 60));
	LinePen[7] = CreatePen (PS_SOLID, 2, RGB (20, 20, 20));
	LinePen[8] = CreatePen (PS_SOLID, 2, RGB (40, 50, 20));
	LinePen[9] = CreatePen (PS_SOLID, 2, RGB (190, 155, 175));
	


	
	//이걸로 설정해야 텍스트 배경이 투명하게 처리 된다.
	SetBkMode (hMemDC, TRANSPARENT);

	Alarm_SetTime = 0;
	AlarmFlag = false;

}


//==============================================
//그래프 작업 함수들
//==============================================
void CMonitor_GraphUnit::Print_Line_Single (void)
{
	int GraphMax = Graph_Max;
	int DataMax = 0;
	int Data;
	float MonitorData;
	int cnt;
	float x_axis;
	float x = 0;
	RECT rect = GraphSize;

	//타이틀 그리기
	Title ();



	//화면 채우기
	OldBrush = ( HBRUSH )SelectObject (hMemDC, BGBrush);
	OldPen = ( HPEN )SelectObject (hMemDC, GetStockObject (NULL_PEN));

	Rectangle (hMemDC, rect.left-1, rect.top-1, rect.right+1, rect.bottom+1);
	
	SelectObject (hMemDC, OldBrush);
	SelectObject (hMemDC, OldPen);


	//그리드 그리기
	Grid ();



	//여기부터 그래프 그리기
	x_axis = ( float )rect.right / (Queue_NodeMax - 2);
	OldPen = ( HPEN )SelectObject (hMemDC, LinePen[0]);

	if ( !ColumnArray[0].DataArray->Peek (&Data, 0) )
	{
		return;
	}

	MonitorData = (float)Data * rect.bottom / GraphMax;

	MoveToEx (hMemDC, ( int )x, (int)MonitorData, NULL);

	for ( cnt = 1; ColumnArray[0].DataArray->Peek (&Data, cnt); cnt++ )
	{
		x = cnt * x_axis;

		MonitorData = ( float )Data * rect.bottom / GraphMax;

		LineTo (hMemDC, ( int )x, ( int )MonitorData);


		if ( DataMax < Data )
		{
			DataMax = Data;
		}
	}


	if ( Graph_Flow )
	{
		if ( DataMax < Graph_Max/2 && GraphMax > 50 )
		{
			Graph_Max = Graph_Max/2;
		}
	}

	SelectObject (hMemDC, OldPen);

	return;
}

void CMonitor_GraphUnit::Print_Line_Multi (void)
{
	int GraphMax = Graph_Max;
	int ColumnNum;
	int Data;
	float MonitorData;
	int cnt;
	float x_axis;
	float x = 0;
	RECT rect = GraphSize;


	//타이틀 그리기
	Title ();



	//화면 채우기
	OldBrush = ( HBRUSH )SelectObject (hMemDC, BGBrush);
	OldPen = ( HPEN )SelectObject (hMemDC, GetStockObject (NULL_PEN));

	Rectangle (hMemDC, rect.left - 1, rect.top - 1, rect.right + 1, rect.bottom + 1);

	SelectObject (hMemDC, OldBrush);
	SelectObject (hMemDC, OldPen);


	//그리드 그리기
	Grid ();

	//화면 우측에 라인별 설명 나타내기
	MultLine_addition ();

	//여기부터 그래프 그리기
	x_axis = ( float )rect.right / (Queue_NodeMax - 2);
	OldPen = ( HPEN )SelectObject (hMemDC, LinePen[0]);

	for ( ColumnNum = 0; ColumnNum < Column_Max; ColumnNum++ )
	{
		SelectObject (hMemDC, LinePen[ColumnNum]);

		if ( !ColumnArray[ColumnNum].DataArray->Peek (&Data, 0) )
		{
			return;
		}

		x = 0;
		MonitorData = ( float )Data * rect.bottom / GraphMax;

		MoveToEx (hMemDC, ( int )x, ( int )MonitorData, NULL);

		for ( cnt = 1; ColumnArray[ColumnNum].DataArray->Peek (&Data, cnt); cnt++ )
		{
			x = cnt * x_axis;

			MonitorData = ( float )Data * rect.bottom / GraphMax;

			LineTo (hMemDC, ( int )x, ( int )MonitorData);

		}
	}
	

	SelectObject (hMemDC, OldPen);


}





//==============================================
//Queue데이터 관리 함수
//==============================================
BOOL CMonitor_GraphUnit::InitData ( ULONG ServerID, int Data, int iType)
{
	int cnt;
	for ( cnt = 0; cnt < Column_Max; cnt++ )
	{
		//서버 아이디가 같은지 확인.
		if ( ColumnArray[cnt].u64ServerID == ServerID )
		{
			//컬럼의 타입이 맞는지 체크
			if ( ColumnArray[cnt].iType == iType )
			{

				if ( !ColumnArray[cnt].DataArray->EnQueue (Data) )
				{
					int Delete;
					ColumnArray[cnt].DataArray->DeQueue (&Delete);
					ColumnArray[cnt].DataArray->EnQueue (Data);
				}


				//그래프 Max치가 존재하지 않을 경우
				if ( Graph_Flow )
				{
					if ( Data > Graph_Max )
					{
						Graph_Max += Graph_Max;
					}
				}

				//알람 울리는 거.
				if ( (Alarm_Max != 0) && (Alarm_Max < Data) )
				{
					Alarm ();
				}
				else
				{
					ULONG64 Alarm_Time = GetTickCount64 ();
					if ( Alarm_Time - Alarm_SetTime > AlarmMax )
					{
						AlarmFlag = false;
					}
				}


				InvalidateRect (hWnd, NULL, false);
				return true;

			}

		}
	}

	return false;


}

void CMonitor_GraphUnit::Alarm (void)
{
	ULONG64 Alarm_Time;
	Alarm_Time = GetTickCount64 ();

	//알람이 울리는 최초의 한번 작동함.
	if ( Alarm_SetTime == 0 )
	{
		Alarm_SetTime = GetTickCount64 ();
		SendMessage (hWnd_Parent, UM_Alret, NULL, NULL);
		AlarmFlag = true;
	}
	

	if ( (Alarm_Time - Alarm_SetTime)  > AlarmMax )
	{
		SendMessage (hWnd_Parent, UM_Alret, NULL, NULL);
		Alarm_SetTime = Alarm_Time;
		AlarmFlag = true;
	}

	return;
}

//==============================================
//MemDC화면 Flip함수
//==============================================
void CMonitor_GraphUnit::Flip_MemDC (HDC hdc)
{

	RECT rect = MemSize;

	// Required for MemDC both contexts
	SetMapMode (hdc, MM_ANISOTROPIC);
	SetViewportOrgEx (hdc, 0, rect.bottom, NULL);
	SetWindowExtEx (hdc, rect.bottom, rect.right, NULL);
	SetViewportExtEx (hdc, rect.bottom, -rect.right, NULL);


	BitBlt (hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);
}

//==============================================
//Title바 작업
//==============================================
void CMonitor_GraphUnit::Title (void)
{


	//타이틀바 칠하기
	OldBrush = ( HBRUSH )SelectObject (hMemDC, TitleBrush);
	OldPen = ( HPEN )SelectObject (hMemDC, GetStockObject (NULL_PEN));

	Rectangle (hMemDC, MemSize.left - 1, (GraphSize.bottom - MemSize.top) - 1, MemSize.right + 1, MemSize.bottom);

	SelectObject (hMemDC, OldBrush);
	SelectObject (hMemDC, OldPen);

	//타이틀 출력하기
	OldFont = ( HFONT )SelectObject (hMemDC, TitleFont);
	if ( !AlarmFlag )
	{
		SetTextColor (hMemDC, TitleColor);

	}
	else
	{
		SetTextColor (hMemDC, AlarmColor);
	}
	TextOutW (hMemDC, MemSize.left + 3, MemSize.bottom - (TitleBarLength / 6), TitleName, lstrlenW (TitleName));
	
	SetTextColor (hMemDC, RGB (0, 0, 0));
	SelectObject (hMemDC, OldFont);

	return;

}

void CMonitor_GraphUnit::Grid (void)
{
	int cnt = 1;
	double Y_axis;
	double GridData;
	double GridDataMax;

	Y_axis =(double) GraphSize.bottom / 4.0;
	GridData = ( double )Graph_Max / 4;
	

	WCHAR GridNum[30] = L"";

	//그리드 선 그리기
	OldPen = ( HPEN )SelectObject (hMemDC, GridPen);
	OldFont = ( HFONT )SelectObject (hMemDC, GridFont);

	for ( cnt = 1; cnt <= 4; cnt++ )
	{
		if ( cnt != 4 )
		{

			MoveToEx (hMemDC, GraphSize.left, ( int )Y_axis * cnt, NULL);
			LineTo (hMemDC, GraphSize.right, ( int )Y_axis * cnt);
		}

		//wsprintfW함수의 단점은 float형을 지원하지 않는다는 것이다.
		//중간값은 상관없는데 MAX값 출력할때 제대로 안나오므로 인자 두개를 사용해서 double로 Max값을 나눈뒤 cnt값으로 다시 곱하고 마지막에 출력할때만 int형으로 바꾼다.

		GridDataMax = GridData * cnt;
		wsprintfW (GridNum, L"%d",(int) GridDataMax);
		TextOutW (hMemDC,GraphSize.left, ( int )Y_axis * cnt, GridNum,lstrlenW(GridNum));
	}

	SelectObject (hMemDC, OldPen);
	SelectObject (hMemDC, OldFont);

}



void CMonitor_GraphUnit::MultLine_addition (void)
{
	int cnt;
	LONG Y_axis;
	int Y;
	Y_axis = MemSize.bottom - dfTitleMax;

	//바탕 채우기
	OldBrush = ( HBRUSH )SelectObject (hMemDC, additionBrush);
	OldPen = ( HPEN )SelectObject (hMemDC, GetStockObject (NULL_PEN));
	
	Rectangle (hMemDC, GraphSize.right, MemSize.top-1, MemSize.right + 1, Y_axis + 1);


	
	Y = Y_axis - 10;
	//라인 그리고 이름 적기
	OldFont =( HFONT ) SelectObject (hMemDC, additionFont);

	for ( cnt = 0; cnt < Column_Max; cnt++ )
	{
	
		Y = Y - 10;

		SelectObject (hMemDC, LinePen[cnt]);

		MoveToEx (hMemDC,GraphSize.right + 5, Y,NULL);
		LineTo (hMemDC, GraphSize.right + 15, Y);

		TextOutW (hMemDC, GraphSize.right + 20, Y + 7, ColumnArray[cnt].Column_Name, lstrlenW (ColumnArray[cnt].Column_Name));
	}

	SelectObject (hMemDC, OldBrush);
	SelectObject (hMemDC, OldPen);
	SelectObject (hMemDC, OldFont);
}


//==============================================
//Table 관리 함수들
//==============================================
BOOL CMonitor_GraphUnit::PutThis (void)
{
	int cnt;
	for ( cnt = 0; cnt < dfMAXCHILD; cnt++ )
	{
		if ( Table.hWnd[cnt] == NULL )
		{
			Table.hWnd[cnt] = hWnd;
			Table.pThis[cnt] = this;
			return true;
		}
	}
	return false;
}

BOOL CMonitor_GraphUnit::RemoveThis (HWND hWnd)
{
	int cnt;
	for ( cnt = 0; cnt < dfMAXCHILD; cnt++ )
	{
		if ( Table.hWnd[cnt] == hWnd )
		{
			Table.pThis[cnt] = NULL;
			Table.hWnd[cnt] = NULL;
			return true;
		}
	}
	return false;
}

CMonitor_GraphUnit * CMonitor_GraphUnit::GetThis (HWND hWnd)
{
	int cnt;
	for ( cnt = 0; cnt < dfMAXCHILD; cnt++ )
	{
		if ( Table.hWnd[cnt] == hWnd )
		{
			return Table.pThis[cnt];
			
		}
	}
	return nullptr;
}