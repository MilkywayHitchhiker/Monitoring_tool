#include"stdafx.h"
#include"CMonitor_GraphUnit.h"
#include<time.h>

static CMonitor_GraphUnit::stHWNDtoTHIS Table;
CMonitor_GraphUnit::CMonitor_GraphUnit (HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight)
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
	wcex.hbrBackground = ( HBRUSH )(COLOR_WINDOW + 1);
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
	BG_Color = BackColor;

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


//=========================================
//�׷��� ������ �ʿ��� ������ ���� ������ �ް� �̸� �����Ѵ�. AlretMax�� GraphMax�� ��� 0�� ��� �۵��� ���Ѵ�.
//=========================================
void CMonitor_GraphUnit::SetInformation (WCHAR *TitleName,int CPUID, int DataQueue_Num , int QueueMax , int GraphMax , int AlretMax )
{
	if ( GraphMax == 0 )
	{
		WData.Graph_Flow = true;
		GraphMax = 50;
	}
	else
	{
		WData.Graph_Flow = false;
	}


	Title_Name = TitleName;


	WData._CPUID = CPUID;
	WData.Queue_cnt = DataQueue_Num;
	WData.GraphMax = GraphMax;
	WData.AlretMax = AlretMax;
	WData.Queue_Max = QueueMax;


	//�׷��� Ÿ�Կ� ���� Queue�� ������ �޶���.
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
		queue[0] = new Queue<int> (QueueMax);
		break;
	case LINE_MULTI:
		break;
	}



	GetClientRect (hWnd, &GraphSize);
	GraphSize.bottom -= TitleBarLength;					// Y ���� ����ǥ ��ȯ�� ���ؼ� ������ �����Ƿ� Top�� ���°� �ƴ� Bottom�� ���� Top�� ������!

	BGBrush = CreateSolidBrush (BG_Color);

	

	TitleBrush = CreateSolidBrush (RGB (max ((GetRValue (BG_Color) - 30), 0), max ((GetGValue (BG_Color) - 30), 0), max ((GetBValue (BG_Color) - 30), 0)));			//Ÿ��Ʋ�� �귯��
	TitleFont = CreateFont (15, 0, 0, 0, 1000, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT ("�ü�"));				//Ÿ��Ʋ�� ��Ʈ
	TitleColor = RGB ((255 - GetRValue (BG_Color)), (255 - GetGValue (BG_Color)), (255 - GetBValue (BG_Color)));
	AlarmColor = RGB (220, 20, 60);

	GridFont = 	CreateFont (15, 0, 0, 0, 5, 0, 0, 0,ANSI_CHARSET, OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE,TEXT("���� ���"));		//�׸���� ��Ʈ
	GridPen = CreatePen (PS_SOLID, 1, RGB (28, 28, 28));	//�׸���� ��

	LinePen = CreatePen (PS_SOLID, 1, RGB (0, 0, 0));								//���ο� ��


	//�̰ɷ� �����ؾ� �ؽ�Ʈ ����� �����ϰ� ó�� �ȴ�.
	SetBkMode (hMemDC, TRANSPARENT);

	Alarm_SetTime = 0;
	AlarmFlag = false;

}

//==============================================
//�׷��� �۾� �Լ���
//==============================================
void CMonitor_GraphUnit::Line_Single (void)
{
	int GraphMax = 0;
	int Data;
	float MonitorData;
	int cnt;
	float x_axis;
	float x = 0;
	RECT rect = GraphSize;


	//Ÿ��Ʋ �׸���
	Title ();



	//ȭ�� ä���
	OldBrush = ( HBRUSH )SelectObject (hMemDC, BGBrush);
	OldPen = ( HPEN )SelectObject (hMemDC, GetStockObject (NULL_PEN));

	Rectangle (hMemDC, rect.left-1, rect.top-1, rect.right+1, rect.bottom+1);
	
	SelectObject (hMemDC, OldBrush);
	SelectObject (hMemDC, OldPen);


	//�׸��� �׸���
	Grid ();



	//������� �׷��� �׸���
	x_axis = ( float )rect.right / (WData.Queue_Max - 1);
	OldPen = ( HPEN )SelectObject (hMemDC, LinePen);

	queue[0]->Peek (&Data, 0);
	MonitorData = (float)Data * rect.bottom / WData.GraphMax;

	MoveToEx (hMemDC, ( int )x, (int)MonitorData, NULL);

	for ( cnt = 1; queue[0]->Peek (&Data, cnt); cnt++ )
	{
		x = cnt * x_axis;

		if ( x == rect.right )
		{
			x = 0;

		}
		MonitorData = ( float )Data * rect.bottom / WData.GraphMax;

		LineTo (hMemDC, ( int )x, ( int )MonitorData);


		if ( GraphMax < Data )
		{
			GraphMax = Data;
		}
	}


	if ( WData.Graph_Flow )
	{
		if ( WData.GraphMax > GraphMax && !(WData.GraphMax <= 50) )
		{
			WData.GraphMax -= 50;
		}

	}

	SelectObject (hMemDC, OldPen);

	return;
}



//==============================================
//Queue������ ���� �Լ�
//==============================================
BOOL CMonitor_GraphUnit::InitData (int Data, int CPUID, int Line)
{

	//�޾ƾ� �Ǵ� �����Ͱ� �ƴ϶�� �׳� �ǵ��� ������.
	if ( WData._CPUID != CPUID )
	{
		return false;
	}


	//Queue������ �ʰ��������
	if ( WData.Queue_cnt < Line )
	{
		return false;
	}

	if ( !queue[Line]->EnQueue (Data) )
	{
		int Delete;
		queue[Line]->DeQueue (&Delete);
		queue[Line]->EnQueue (Data);
	}

	//�׷��� Maxġ�� �������� ���� ���
	if ( WData.Graph_Flow )
	{
		if ( Data > WData.GraphMax )
		{
			WData.GraphMax += WData.GraphMax;
		}
	}

	//�˶� �︮�� ��.
	if ( ( WData.AlretMax != 0 ) && ( WData.AlretMax < Data ) )
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

void CMonitor_GraphUnit::Alarm (void)
{
	ULONG64 Alarm_Time;
	Alarm_Time = GetTickCount64 ();

	//�˶��� �︮�� ������ �ѹ� �۵���.
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
//MemDCȭ�� Flip�Լ�
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
//Title�� �۾�
//==============================================
void CMonitor_GraphUnit::Title (void)
{


	//Ÿ��Ʋ�� ĥ�ϱ�
	OldBrush = ( HBRUSH )SelectObject (hMemDC, TitleBrush);
	OldPen = ( HPEN )SelectObject (hMemDC, GetStockObject (NULL_PEN));

	Rectangle (hMemDC, MemSize.left - 1, (GraphSize.bottom - MemSize.top) - 1, MemSize.right + 1, MemSize.bottom);

	SelectObject (hMemDC, OldBrush);
	SelectObject (hMemDC, OldPen);

	//Ÿ��Ʋ ����ϱ�
	OldFont = ( HFONT )SelectObject (hMemDC, TitleFont);
	if ( !AlarmFlag )
	{
		SetTextColor (hMemDC, TitleColor);

	}
	else
	{
		SetTextColor (hMemDC, AlarmColor);
	}
	TextOutW (hMemDC, MemSize.left + 3, MemSize.bottom - (TitleBarLength / 6), Title_Name, lstrlenW (Title_Name));
	
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
	GridData = ( double )WData.GraphMax / 4;
	

	WCHAR GridNum[30] = L"";

	//�׸��� �� �׸���
	OldPen = ( HPEN )SelectObject (hMemDC, GridPen);
	OldFont = ( HFONT )SelectObject (hMemDC, GridFont);

	for ( cnt = 1; cnt <= 4; cnt++ )
	{
		if ( cnt != 4 )
		{

			MoveToEx (hMemDC, GraphSize.left, ( int )Y_axis * cnt, NULL);
			LineTo (hMemDC, GraphSize.right, ( int )Y_axis * cnt);
		}

		//wsprintfW�Լ��� ������ float���� �������� �ʴ´ٴ� ���̴�.
		//�߰����� ������µ� MAX�� ����Ҷ� ����� �ȳ����Ƿ� ���� �ΰ��� ����ؼ� double�� Max���� ������ cnt������ �ٽ� ���ϰ� �������� ����Ҷ��� int������ �ٲ۴�.

		GridDataMax = GridData * cnt;
		wsprintfW (GridNum, L"%d",(int) GridDataMax);
		TextOutW (hMemDC,GraphSize.left, ( int )Y_axis * cnt, GridNum,lstrlenW(GridNum));
	}

	SelectObject (hMemDC, OldPen);
	SelectObject (hMemDC, OldFont);

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