// Monitoring_tool.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Monitoring_tool.h"
#include "CMonitor_GraphUnit.h"
#include <time.h>

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof (WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MONITORING_TOOL));
	wcex.hCursor = LoadCursor (nullptr, IDC_ARROW);
	wcex.hbrBackground = ( HBRUSH )(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"main";
	wcex.hIconSm = LoadIcon (wcex.hInstance, MAKEINTRESOURCE (IDI_SMALL));

	RegisterClassExW (&wcex);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MONITORING_TOOL));

    MSG msg;

    // �⺻ �޽��� �����Դϴ�.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}




//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   HWND hWnd = CreateWindowW(L"main", L"Monitor", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//


CMonitor_GraphUnit *p1;
CMonitor_GraphUnit *p2;
CMonitor_GraphUnit *p3;
int LastInit;
int LastInit2;
int LastInit3;


bool AlarmFlag = false;
ULONG64 SetAlarm;
ULONG64 EndAlarm;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;


	hdc = GetDC (hWnd);

	HBRUSH Red = CreateSolidBrush (RGB (220, 20, 60));
	HBRUSH White = CreateSolidBrush (RGB (255, 255, 255));
	HBRUSH OldBrush;
	GetClientRect (hWnd, &rect);

	ReleaseDC (hWnd, hdc);

    switch (message)
    {
	case WM_CREATE :
		p1 = new CMonitor_GraphUnit (hInst, hWnd, RGB (200, 200, 10), CMonitor_GraphUnit::LINE_SINGLE, 10, 10, 200, 200);
		p2 = new CMonitor_GraphUnit (hInst, hWnd, RGB (199, 10, 10), CMonitor_GraphUnit::LINE_SINGLE, 220, 10, 200, 200);
		p3 = new CMonitor_GraphUnit (hInst, hWnd, RGB (100, 100, 100), CMonitor_GraphUnit::LINE_SINGLE, 430, 10, 400, 200);
		
		p1->SetInformation (L"Monitor1",1, 1, 50, 0, 75);
		p2->SetInformation (L"Monitor2",2, 1, 60, 200, 0);
		p3->SetInformation (L"Monitor3",3, 1, 100, 150, 0);

		srand (time (NULL) % 100);
		SetTimer (hWnd, 1, 100, NULL);

		LastInit = 0;
		LastInit2 = 100;
		LastInit3 = 50;
				
		break;
	case WM_TIMER:
		switch ( wParam )
		{
		case 1:

			LastInit++;
			if ( LastInit > 100 )
			{
				LastInit = 0;
			}


			LastInit2--;

			if ( LastInit2 < 0 )
			{
				LastInit2 = 100;
			}

			LastInit3++;

			if ( LastInit3 > 100 )
			{
				LastInit3 = 50;
			}

			for ( int cnt = 1; cnt < 4; cnt++ )
			{
				int Num;
				switch ( cnt )
				{
				case 1:
					Num = LastInit;
					break;
				case 2:
					Num = LastInit2;
					break;
				case 3:
					Num = LastInit3;
					break;
				}

				p1->InitData (Num, cnt, 0);
				p2->InitData (Num, cnt, 0);
				p3->InitData (Num, cnt, 0);

			}
			break;
		}
		if ( AlarmFlag )
		{
			EndAlarm = GetTickCount64 ();
			if ( EndAlarm - SetAlarm > AlarmMax )
			{
				hdc = GetDC (hWnd);
				OldBrush = ( HBRUSH )SelectObject (hdc, White);
				Rectangle (hdc, rect.left, rect.top, rect.right, rect.bottom);
				SelectObject (hdc, OldBrush);
				ReleaseDC (hWnd, hdc);

				AlarmFlag = false;
			}

		}

		break;
    case WM_PAINT:
        {

            hdc = BeginPaint(hWnd, &ps);
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
			EndPaint(hWnd, &ps);
        }
        break;
	case UM_Alret:
		AlarmFlag = true;
		SetAlarm = GetTickCount64 ();
		
		hdc = GetDC (hWnd);

		OldBrush = ( HBRUSH )SelectObject (hdc, Red);
		Rectangle (hdc, rect.left, rect.top, rect.right, rect.bottom);
		SelectObject (hdc, OldBrush);

		ReleaseDC (hWnd, hdc);

		break;
    case WM_DESTROY:
		KillTimer (hWnd, 1);
		DeleteObject (Red);
		PostQuitMessage(0);
		
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
