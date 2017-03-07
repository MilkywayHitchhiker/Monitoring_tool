#include"stdafx.h"
#include"CMonitor_GraphUnit.h"
#include<time.h>

static CMonitor_GraphUnit::stHWNDtoTHIS Table;
CMonitor_GraphUnit::CMonitor_GraphUnit (HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight, int DataQueue_Num)
{
	//모니터 윈도우 네임.
	static int WinNum;
	wsprintfW (szWindowClass, L"Monitoring Window child %d", WinNum);
	WinNum++;


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
	wcex.hbrBackground = CreateSolidBrush (BackColor);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = NULL;

	RegisterClassExW (&wcex);

	

	//인스턴스 핸들을 클래스 전역변수에 등록하고 프로그램 창을 만든다음에 표시합니다.


	//윈도우를 생성해서 핸들을 넘겨 줍니다.
	hWnd = CreateWindowW (szWindowClass, NULL, WS_CHILD | WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS,
		iPosX, iPosY, iWidth, iHeight, hWndParent, nullptr, hInstance, nullptr);

	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	hWnd_Parent = hWndParent;

	if ( !PutThis () )
	{
		return;
	}

	//그래프 타입에 따라서 Queue의 갯수가 달라짐.

	GraphType = enType;
	Queue_cnt = DataQueue_Num;
	BG_Color = BackColor;

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
		queue = new Queue<int> *[DataQueue_Num];
		queue[0] = new Queue<int> (Max);
		break;
	case LINE_MULTI:
		break;
	}


}


LRESULT CALLBACK CMonitor_GraphUnit:: WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	CMonitor_GraphUnit *pThis;
	PAINTSTRUCT ps;
	HDC hdc;
	pThis = GetThis (hWnd);

switch ( message )
{
case WM_CREATE:
	break;

case WM_SIZE:

//	pThis->ResetMemDC (hWnd);
	InvalidateRect (hWnd, NULL, TRUE);
	break;

case WM_PAINT:

	hdc = BeginPaint (hWnd, &ps);
	// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
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
		pThis->Line_Single (hWnd);
		break;
	case LINE_MULTI :
		break;
	}
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





//==============================================
//그래프 작업 함수들
//==============================================
void CMonitor_GraphUnit::Line_Single (HWND hWnd)
{
	int Data = 0;
	int cnt;
	int x = 0;
	int sum;
	RECT rect;


	HDC DC = GetDC (hWnd);



	GetClientRect (hWnd, &rect);
	sum = rect.right / 100;

	//윈도우 좌표 변경
	SetMapMode (DC, MM_TEXT);

	//MM_TEXT모드에서만 작동한다.
	SetViewportOrgEx (DC, rect.left, rect.bottom - 5, NULL);
	
	//MM_TEXT에서는 작동을 안한다. 논리좌표와 물리좌표의 차이때문에 발생.
	//SetWindowOrgEx (DC, rect.left, rect.bottom, NULL);


	queue[0]->Peek (&Data, 0);
	MoveToEx (DC, x, -Data, NULL);

	for ( cnt = 1; queue[0]->Peek (&Data, cnt); cnt++ )
	{
		x = x + sum;

		if ( x == rect.right )
		{
			x = 0;

		}

		LineTo (DC, x, -Data);

	}

	ReleaseDC (hWnd, DC);


}


//==============================================
//Queue데이터 관리 함수
//==============================================
BOOL CMonitor_GraphUnit::InitData (int Data, int Line)
{
	if ( Queue_cnt <= Line )
	{
		return false;
	}

	if ( !queue[Line]->EnQueue (Data) )
	{
		int Delete = 0;
		queue[Line]->DeQueue (&Delete);
		queue[Line]->EnQueue (Data);

	}
	InvalidateRect (hWnd,NULL,true);
	return true;
}

//==============================================
//Client 화면 크기 변경시 메모리 DC재작업 함수
//==============================================
void CMonitor_GraphUnit::ResetMemDC (HWND hWnd)
{
	RECT rect;

	HDC hdc;
	HDC Buffer_DC;
	HBITMAP Buffer_BitMap;
	HBITMAP Buffer_OldBitMap;

	hdc = GetDC (hWnd);

	//변경될 클라이언트의 화면 사이즈 구함.
	GetClientRect (hWnd, &rect);

	//변경될 사이즈에 맞는 DC와 비트맵 만들기.
	Buffer_DC = CreateCompatibleDC (hdc);
	Buffer_BitMap = CreateCompatibleBitmap (hdc, rect.right, rect.bottom);
	Buffer_OldBitMap = ( HBITMAP )SelectObject (hdc, Buffer_BitMap);

	FillRect (Buffer_DC, &rect, CreateSolidBrush (BG_Color));

	//기존 비트맵을 복사해서 새로 만든 비트맵에 붙여넣기.
	BitBlt (Buffer_DC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

	//기존 비트맵 삭제.
	SelectObject (hMemDC, hMemDC_OldBitmap);
	DeleteObject (hMemDC);


	//새로운 비트맵을 전역 메모리DC로 연결.
	hMemDC = Buffer_DC;
	hMemDC_Bitmap = Buffer_BitMap;
	hMemDC_OldBitmap = Buffer_OldBitMap;

	ReleaseDC (hWnd,hdc);

	return;
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