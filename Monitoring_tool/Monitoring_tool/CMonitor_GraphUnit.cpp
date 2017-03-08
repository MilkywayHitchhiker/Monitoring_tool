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
	hWnd = CreateWindowW (szWindowClass, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		iPosX, iPosY, iWidth, iHeight, hWndParent, nullptr, hInstance, nullptr);

	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
	hWnd_Parent = hWndParent;

	HDC hdc = GetDC (hWnd);

	GetClientRect (hWnd, &MemSize);

	hMemDC = CreateCompatibleDC (hdc);
	hMemDC_Bitmap = CreateCompatibleBitmap (hdc, MemSize.right, MemSize.bottom);
	hMemDC_OldBitmap = ( HBITMAP )SelectObject (hMemDC, hMemDC_Bitmap);

	// Flip Y axis
	SetMapMode (hMemDC, MM_ANISOTROPIC);
	SetViewportOrgEx (hMemDC, 0, MemSize.bottom, NULL);
	SetWindowExtEx (hMemDC, MemSize.bottom, MemSize.right, NULL);
	SetViewportExtEx (hMemDC, MemSize.bottom, -MemSize.right, NULL);

	ReleaseDC (hWnd, hdc);

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
		pThis->Line_Single ();
		break;
	case LINE_MULTI :
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





//==============================================
//그래프 작업 함수들
//==============================================
void CMonitor_GraphUnit::Line_Single (void)
{
	int Data = 0;
	int cnt;
	int x = 0;
	RECT rect = MemSize;

	//화면 채우기
	FillRect (hMemDC, &MemSize, CreateSolidBrush(BG_Color));


	queue[0]->Peek (&Data, 0);
	MoveToEx (hMemDC, x, Data, NULL);

	for ( cnt = 1; queue[0]->Peek (&Data, cnt); cnt++ )
	{
		x = cnt * rect.right / Max ;

		if ( x == rect.right )
		{
			x = 0;

		}

		LineTo (hMemDC, x, Data);
	}


	return;
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

	InvalidateRect (hWnd, NULL, false);
	return true;
}

//==============================================
//MemDC화면 Flip함수
//==============================================
void CMonitor_GraphUnit::Flip_MemDC (HDC hdc)
{

	RECT rect = MemSize;

	// Required for both contexts
	SetMapMode (hdc, MM_ANISOTROPIC);
	SetViewportOrgEx (hdc, 0, rect.bottom, NULL);
	SetWindowExtEx (hdc, rect.bottom, rect.right, NULL);
	SetViewportExtEx (hdc, rect.bottom, -rect.right, NULL);


	BitBlt (hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);
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