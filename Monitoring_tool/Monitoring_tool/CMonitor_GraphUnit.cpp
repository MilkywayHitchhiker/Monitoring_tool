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
	hWnd = CreateWindowW (szWindowClass, NULL, WS_CHILD | WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS,
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
	// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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
//�׷��� �۾� �Լ���
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

	//������ ��ǥ ����
	SetMapMode (DC, MM_TEXT);

	//MM_TEXT��忡���� �۵��Ѵ�.
	SetViewportOrgEx (DC, rect.left, rect.bottom - 5, NULL);
	
	//MM_TEXT������ �۵��� ���Ѵ�. ����ǥ�� ������ǥ�� ���̶����� �߻�.
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
	InvalidateRect (hWnd,NULL,true);
	return true;
}

//==============================================
//Client ȭ�� ũ�� ����� �޸� DC���۾� �Լ�
//==============================================
void CMonitor_GraphUnit::ResetMemDC (HWND hWnd)
{
	RECT rect;

	HDC hdc;
	HDC Buffer_DC;
	HBITMAP Buffer_BitMap;
	HBITMAP Buffer_OldBitMap;

	hdc = GetDC (hWnd);

	//����� Ŭ���̾�Ʈ�� ȭ�� ������ ����.
	GetClientRect (hWnd, &rect);

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

	ReleaseDC (hWnd,hdc);

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