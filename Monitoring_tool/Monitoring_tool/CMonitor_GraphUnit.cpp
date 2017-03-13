#include"stdafx.h"
#include"CMonitor_GraphUnit.h"
#include<time.h>

static CMonitor_GraphUnit::stHWNDtoTHIS Table;
CMonitor_GraphUnit::CMonitor_GraphUnit (WCHAR * Title,HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight)
{
	//����� ������ ����.
	wsprintfW (TitleName, Title);


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
	wcex.lpszClassName = TitleName;
	wcex.hIconSm = NULL;

	RegisterClassExW (&wcex);

	

	//�ν��Ͻ� �ڵ��� Ŭ���� ���������� ����ϰ� ���α׷� â�� ��������� ǥ���մϴ�.


	//�����츦 �����ؼ� �ڵ��� �Ѱ� �ݴϴ�.
	hWnd = CreateWindowW (TitleName, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
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

	GetClientRect (hWnd, &GraphSize);
	GraphSize.bottom -= TitleBarLength;					// Y ���� ����ǥ ��ȯ�� ���ؼ� ������ �����Ƿ� Top�� ���°� �ƴ� Bottom�� ���� Top�� ������!

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
//�׷��� ������ �ʿ��� ������ ���� ������ �ް� �̸� �����Ѵ�. AlretMax�� GraphMax�� ��� 0�� ��� �۵��� ���Ѵ�.
//=========================================
void CMonitor_GraphUnit::DataColumnInfo (int ColumnNum,WCHAR *ColumnName,ULONG ServerID,int Type)
{
	int cnt;
	//�÷� Maxġ���� �÷� ��ȣ�� �Ѿ ���
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



	TitleBrush = CreateSolidBrush (RGB (max ((GetRValue (BG_Color) - 30), 0), max ((GetGValue (BG_Color) - 30), 0), max ((GetBValue (BG_Color) - 30), 0)));			//Ÿ��Ʋ�� �귯��
	TitleFont = CreateFont (15, 0, 0, 0, 1000, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT ("�ü�"));				//Ÿ��Ʋ�� ��Ʈ
	TitleColor = RGB ((255 - GetRValue (BG_Color)), (255 - GetGValue (BG_Color)), (255 - GetBValue (BG_Color)));
	AlarmColor = RGB (220, 20, 60);

	GridFont = CreateFont (15, 0, 0, 0, 5, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT ("���� ���"));		//�׸���� ��Ʈ
	GridPen = CreatePen (PS_SOLID, 1, RGB (28, 28, 28));	//�׸���� ��

	additionFont = GridFont;				//���� ����� ��Ʈ
	additionPen = GridPen;					//���� ����� ��
	additionBrush = CreateSolidBrush (RGB (max ((GetRValue (BG_Color) -50), 0), max ((GetGValue (BG_Color) - 50), 0), max ((GetBValue (BG_Color) - 50), 0)));;


	LinePen[0] = CreatePen (PS_SOLID, 2, RGB (255, 255, 255));								//���ο� ��
	LinePen[1] = CreatePen (PS_SOLID, 2, RGB (155, 155, 130));
	LinePen[2] = CreatePen (PS_SOLID, 2, RGB (100, 100, 70));
	LinePen[3] = CreatePen (PS_SOLID, 2, RGB (50, 50, 50));
	LinePen[4] = CreatePen (PS_SOLID, 2, RGB (0, 155, 155));
	LinePen[5] = CreatePen (PS_SOLID, 2, RGB (100, 0, 50));
	LinePen[6] = CreatePen (PS_SOLID, 2, RGB (30, 70, 60));
	LinePen[7] = CreatePen (PS_SOLID, 2, RGB (20, 20, 20));
	LinePen[8] = CreatePen (PS_SOLID, 2, RGB (40, 50, 20));
	LinePen[9] = CreatePen (PS_SOLID, 2, RGB (190, 155, 175));
	


	
	//�̰ɷ� �����ؾ� �ؽ�Ʈ ����� �����ϰ� ó�� �ȴ�.
	SetBkMode (hMemDC, TRANSPARENT);

	Alarm_SetTime = 0;
	AlarmFlag = false;

}


//==============================================
//�׷��� �۾� �Լ���
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


	//Ÿ��Ʋ �׸���
	Title ();



	//ȭ�� ä���
	OldBrush = ( HBRUSH )SelectObject (hMemDC, BGBrush);
	OldPen = ( HPEN )SelectObject (hMemDC, GetStockObject (NULL_PEN));

	Rectangle (hMemDC, rect.left - 1, rect.top - 1, rect.right + 1, rect.bottom + 1);

	SelectObject (hMemDC, OldBrush);
	SelectObject (hMemDC, OldPen);


	//�׸��� �׸���
	Grid ();

	//ȭ�� ������ ���κ� ���� ��Ÿ����
	MultLine_addition ();

	//������� �׷��� �׸���
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
//Queue������ ���� �Լ�
//==============================================
BOOL CMonitor_GraphUnit::InitData ( ULONG ServerID, int Data, int iType)
{
	int cnt;
	for ( cnt = 0; cnt < Column_Max; cnt++ )
	{
		//���� ���̵� ������ Ȯ��.
		if ( ColumnArray[cnt].u64ServerID == ServerID )
		{
			//�÷��� Ÿ���� �´��� üũ
			if ( ColumnArray[cnt].iType == iType )
			{

				if ( !ColumnArray[cnt].DataArray->EnQueue (Data) )
				{
					int Delete;
					ColumnArray[cnt].DataArray->DeQueue (&Delete);
					ColumnArray[cnt].DataArray->EnQueue (Data);
				}


				//�׷��� Maxġ�� �������� ���� ���
				if ( Graph_Flow )
				{
					if ( Data > Graph_Max )
					{
						Graph_Max += Graph_Max;
					}
				}

				//�˶� �︮�� ��.
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



void CMonitor_GraphUnit::MultLine_addition (void)
{
	int cnt;
	LONG Y_axis;
	int Y;
	Y_axis = MemSize.bottom - dfTitleMax;

	//���� ä���
	OldBrush = ( HBRUSH )SelectObject (hMemDC, additionBrush);
	OldPen = ( HPEN )SelectObject (hMemDC, GetStockObject (NULL_PEN));
	
	Rectangle (hMemDC, GraphSize.right, MemSize.top-1, MemSize.right + 1, Y_axis + 1);


	
	Y = Y_axis - 10;
	//���� �׸��� �̸� ����
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