#pragma once
#include"Queue.h"
#define dfMAXCHILD	100
#define Max 100
#define MaxString 100


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



private:

	Queue<int> **queue;

	HWND hWnd;
	HWND hWnd_Parent;

	HINSTANCE hInst;							// ���� �ν��Ͻ� �Դϴ�.
	WCHAR szWindowClass[MaxString];				// �⺻ â Ŭ���� �̸��Դϴ�.

	

	TYPE GraphType;								// ���� â �׷��� Ÿ���Դϴ�.
	COLORREF BG_Color;
	int Queue_cnt;								// Queue����.
	RECT MemSize;

	//�޸� DC�� ����ϱ� ���� �ʿ��� �͵�.
	HDC hMemDC;
	HBITMAP hMemDC_Bitmap;
	HBITMAP hMemDC_OldBitmap;

protected :
	
	//======================================================
	//������ ���ν������� �ڱ��ڽ��� this�� �����ϱ� ���� ���̺� ������
	//������ �ڵ�, this ������ ��Ī ���̺� ����.
	//======================================================
	BOOL PutThis (void);
	static BOOL RemoveThis (HWND hWnd);
	static CMonitor_GraphUnit *GetThis (HWND hWnd);


public :

	//=========================================
	//������. �����츦 �����ϰ� �ʱ�ȭ �����ش�.
	//=========================================
	CMonitor_GraphUnit (HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight, int DataQueue_Num = 1);

	//=========================================
	//�ı���. �����Ҵ� ���� queue�� �ı��ϰ� �����Ѵ�.
	//=========================================
	~CMonitor_GraphUnit (void)
	{
		int cnt;
		RemoveThis (hWnd);
		for ( cnt = 0; cnt < Queue_cnt; cnt++ )
		{
			delete queue[cnt];
		}
		delete[]queue;

		SelectObject (hMemDC, hMemDC_OldBitmap);
		DeleteObject (hMemDC_Bitmap);
		DeleteObject (hMemDC);
		
	}



	//==============================================
	//������ ���ν���
	//==============================================
	static LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//==============================================
	//������ �Է� �Լ�
	//==============================================
	BOOL InitData (int Data, int Line=0);

	//==============================================
	//Client ȭ�� ũ�� ����� �޸� DC���۾� �Լ�
	//==============================================
	void Flip_MemDC (HDC hdc);


	//==============================================
	//�׷��� �Լ�
	//==============================================
	void Line_Single (void);
	
};