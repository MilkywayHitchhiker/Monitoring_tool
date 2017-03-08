#include"stdafx.h"
#include"CMonitor_GraphUnit.h"
#include<time.h>

static CMonitor_GraphUnit::stHWNDtoTHIS Table;
CMonitor_GraphUnit::CMonitor_GraphUnit (HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight, int DataQueue_Num)
{
	//����� ������ ����.
	static int WinNum;
	wsprintfW (szWindowClass, L"Monitoring Window child %d", WinNum);
	WinNum++;


	//â Ŭ������ ��� �մϴ�.

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

	

	//�ν��Ͻ� �ڵ��� Ŭ���� ���������� ����ϰ� ���α׷� â�� ��������� ǥ���մϴ�.


	//�����츦 �����ؼ� �ڵ��� �Ѱ� �ݴϴ�.
	hWnd = CreateWindowW (szWindowClass, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		iPosX, iPosY, iWidth, iHeight, hWndParent, nullptr, hInstance, nullptr);

	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.
	hWnd_Parent = hWndParent;

	if ( !PutThis () )
	{
		return;
	}

	//�׷��� Ÿ�Կ� ���� Queue�� ������ �޶���.

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

	HDC hdc = GetDC (hWnd);
	ResetMemDC (hdc);
	ReleaseDC (hWnd, hdc);
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
	SetTimer (hWnd, 1, 100, NULL);
	break;

case WM_SIZE:
	InvalidateRect (hWnd, NULL, false);
	break;
case WM_TIMER:
	switch ( wParam )
	{
	case 1:
		InvalidateRect (hWnd, NULL, false);
	}
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
		pThis->Line_Single (hdc);
		break;
	case LINE_MULTI :
		break;
	};
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
//�׷��� �۾� �Լ���
//==============================================
void CMonitor_GraphUnit::Line_Single (HDC hdc)
{
	int Data = 0;
	int cnt;
	int x = 0;
	RECT rect = Size;

	hMemDC = CreateCompatibleDC (hdc);
	hMemDC_Bitmap = CreateCompatibleBitmap (hdc, rect.right, rect.bottom);
	hMemDC_OldBitmap = (HBITMAP) SelectObject (hMemDC, hMemDC_Bitmap);

	// Flip Y axis
	SetMapMode (hMemDC, MM_ISOTROPIC);
	SetViewportOrgEx (hMemDC, 0, rect.bottom, NULL);
	SetWindowExtEx (hMemDC, rect.bottom, rect.right, NULL);
	SetViewportExtEx (hMemDC, rect.bottom, -rect.right, NULL);

	// Required for both contexts
	SetMapMode (hdc, MM_ISOTROPIC);
	SetViewportOrgEx (hdc, 0, rect.bottom, NULL);
	SetWindowExtEx (hdc, rect.bottom, rect.right, NULL);
	SetViewportExtEx (hdc, rect.bottom, -rect.right, NULL);

	//ȭ�� ä���
	FillRect (hMemDC, &Size, CreateSolidBrush(BG_Color));


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


	BitBlt (hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);
	SelectObject (hMemDC, hMemDC_OldBitmap);
	DeleteObject (hMemDC);

	return;
}


//==============================================
//Queue������ ���� �Լ�
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
	return true;
}

//==============================================
//Client ȭ�� ũ�� ����� �޸� DC���۾� �Լ�
//==============================================
void CMonitor_GraphUnit::ResetMemDC (HDC hdc)
{
	RECT rect;

	HDC Buffer_DC;
	HBITMAP Buffer_BitMap;
	HBITMAP Buffer_OldBitMap;


	//����� Ŭ���̾�Ʈ�� ȭ�� ������ ����.
	GetClientRect (hWnd, &rect);

	//hMemDC�� ������ �ȵǾ� �ִٸ� �������ִ� �۾��� ���� �Ѵ�.
	if ( hMemDC == NULL )
	{
		Size = rect;
		hMemDC = CreateCompatibleDC (hdc);
		hMemDC_Bitmap = CreateCompatibleBitmap (hdc, rect.right, rect.bottom);
		hMemDC_OldBitmap = ( HBITMAP )SelectObject (hdc, hMemDC_Bitmap);
		
	}
	else if ((Size.right != rect.right) || (Size.bottom != rect.bottom))
	{
		
		//����� ����� �´� DC�� ��Ʈ�� �����.
		Buffer_DC = CreateCompatibleDC (hdc);
		Buffer_BitMap = CreateCompatibleBitmap (hdc, rect.right, rect.bottom);
		Buffer_OldBitMap = ( HBITMAP )SelectObject (hdc, Buffer_BitMap);

		FillRect (Buffer_DC, &rect, CreateSolidBrush (BG_Color));

		//���� ��Ʈ���� �����ؼ� ���� ���� ��Ʈ�ʿ� �ٿ��ֱ�.
		BitBlt (Buffer_DC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

		//���� ��Ʈ�� ����.
		SelectObject (hMemDC, hMemDC_OldBitmap);
		DeleteObject (hMemDC);


		//���ο� ��Ʈ���� ���� �޸�DC�� ����.
		hMemDC = Buffer_DC;
		hMemDC_Bitmap = Buffer_BitMap;
		hMemDC_OldBitmap = Buffer_OldBitMap;
	}

	return;
}



//==============================================
//Table ���� �Լ���
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