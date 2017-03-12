// Monitoring_tool.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Monitoring_tool.h"
#include "CMonitor_GraphUnit.h"
#include <time.h>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
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

    // TODO: 여기에 코드를 입력합니다.
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof (WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_MONITORING_TOOL));
	wcex.hCursor = LoadCursor (nullptr, IDC_ARROW);
	wcex.hbrBackground =(HBRUSH) GetStockObject (BLACK_BRUSH);
	wcex.lpszMenuName = MAKEINTRESOURCEW (IDC_MONITORING_TOOL);
	wcex.lpszClassName = L"main";
	wcex.hIconSm = LoadIcon (wcex.hInstance, MAKEINTRESOURCE (IDI_SMALL));

	RegisterClassExW (&wcex);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MONITORING_TOOL));

    MSG msg;

    // 기본 메시지 루프입니다.
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
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//


CMonitor_GraphUnit *p1;
CMonitor_GraphUnit *p2;
CMonitor_GraphUnit *p3;
CMonitor_GraphUnit *p4;
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
		p1 = new CMonitor_GraphUnit (L"CPU",hInst, hWnd, RGB (200, 200, 10), CMonitor_GraphUnit::LINE_SINGLE, 10, 10, 200, 200);
		p2 = new CMonitor_GraphUnit (L"Memory",hInst, hWnd, RGB (199, 10, 10), CMonitor_GraphUnit::LINE_SINGLE, 220, 10, 200, 200);
		p3 = new CMonitor_GraphUnit (L"RAM",hInst, hWnd, RGB (100, 100, 100), CMonitor_GraphUnit::LINE_SINGLE, 430, 10, 400, 200);
		p4 = new CMonitor_GraphUnit (L"Multi", hInst, hWnd, RGB (155, 75, 100), CMonitor_GraphUnit::LINE_MULTI, 840, 10, 400, 200);
	
		p1->CMonitorGraphUnit (1, 50, 0, 75);
		p1->DataColumnInfo (0,L"Monitor1",1, 1);

		p2->CMonitorGraphUnit (1, 60, 100, 0);
		p2->DataColumnInfo (0,L"Monitor2",2, 1);

		p3->CMonitorGraphUnit (1, 100, 100, 0);
		p3->DataColumnInfo (0,L"Monitor3",3, 1);

		p4->CMonitorGraphUnit (3,100,100,0);
		p4->DataColumnInfo (0, L"Test1", 1, 1);
		p4->DataColumnInfo (1, L"Test2", 2, 1);
		p4->DataColumnInfo (2, L"Test3", 3, 1);


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
			int cnt;
			int Type;
			Type = 1;
			for ( cnt = 1; cnt < 4; cnt++ )
			{
				int Data;
				switch ( cnt )
				{
				case 1:
					Data = LastInit;
					break;
				case 2:
					Data = LastInit2;
					break;
				case 3:
					Data = LastInit3;
					break;
				}

				p1->InitData (cnt, Data, Type);
				p2->InitData (cnt, Data, Type);
				p3->InitData (cnt, Data, Type);
				p4->InitData (cnt, Data, Type);

			}
			break;
		}
		if ( AlarmFlag )
		{
			EndAlarm = GetTickCount64 ();
			if ( EndAlarm - SetAlarm > AlarmMax )
			{
				hdc = GetDC (hWnd);
				OldBrush = ( HBRUSH )SelectObject (hdc, GetStockObject(BLACK_BRUSH));
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
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
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

// 정보 대화 상자의 메시지 처리기입니다.
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
