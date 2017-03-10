#pragma once
#include"Queue.h"
#define dfMAXCHILD	100
#define MaxString 100

#define UM_Alret WM_USER+1
#define AlarmMax 500					//�˶��� �۵��Ǹ� �︮��

#define TitleBarLength 30






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


	struct DataRef
	{
		int _CPUID;									// �ش� ����Ϳ��� �޾ƾߵǴ� CPUID ����
		int AlretMax;								// �ش� ��ġ�� �����ϸ� �˶� �߻�.
		int GraphMax;								// �ش�׷����� Max��.
		bool Graph_Flow;							// Max���� ���� �����׷����Ͻ� true.
		int Queue_cnt;								// Queue����.
		int Queue_Max;
	};

	DataRef WData;


private:


	Queue<int> **queue;

	HWND hWnd;
	HWND hWnd_Parent;

	HINSTANCE hInst;							// ���� �ν��Ͻ� �Դϴ�.
	WCHAR szWindowClass[MaxString];				// �⺻ â Ŭ���� �̸��Դϴ�.



	TYPE GraphType;								// ���� â �׷��� Ÿ���Դϴ�.
	WCHAR *Title_Name;

	//���� �����
	COLORREF BG_Color;							// ��׶��� �÷��Դϴ�.
	COLORREF GRAY_Color;						



	HDC hMemDC;									//�޸� DC�� ����ϱ� ���� �ʿ��� �͵�.
	HBITMAP hMemDC_Bitmap;
	HBITMAP hMemDC_OldBitmap;

	RECT MemSize;								//�޸� DC�� ������
	RECT GraphSize;								//�޸� DC�� �׷��� �׷��� ������
		
	HBRUSH BGBrush;								//��׶���� �귯��

	HBRUSH TitleBrush;							//Ÿ��Ʋ�� �귯��
	HFONT TitleFont;							//Ÿ��Ʋ�� ��Ʈ
	COLORREF TitleColor;						//Ÿ��Ʋ�� ��
	
	HFONT GridFont;								//�׸���� ��Ʈ
	HPEN GridPen;								//�׸���� ��

	HPEN LinePen;								//���ο� ��

	HBRUSH OldBrush;
	HFONT OldFont;
	HPEN OldPen;

	ULONG64 Alarm_SetTime;


protected:

	//======================================================
	//������ ���ν������� �ڱ��ڽ��� this�� �����ϱ� ���� ���̺� ������
	//������ �ڵ�, this ������ ��Ī ���̺� ����.
	//======================================================
	BOOL PutThis (void);
	static BOOL RemoveThis (HWND hWnd);
	static CMonitor_GraphUnit *GetThis (HWND hWnd);




public:

	//=========================================
	//������. �����츦 �����ϰ� �ʱ�ȭ �����ش�.
	//=========================================
	CMonitor_GraphUnit (HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight);

	//=========================================
	//�ı���. �����Ҵ� ���� queue�� �ı��ϰ� �����Ѵ�.
	//=========================================
	~CMonitor_GraphUnit (void)
	{
		int cnt;
		RemoveThis (hWnd);
		for ( cnt = 0; cnt < WData.Queue_cnt; cnt++ )
		{
			delete queue[cnt];
		}
		delete[]queue;


		//SelectObject�� �⺻ �귯���� �⺻ ��Ʈ, �⺻ ������ �ǵ��� �� ������ �̸� �����س� ��� ��Ʈ�� ��,�귯���� �����Ѵ�.
		SelectObject (hMemDC, OldBrush);
		SelectObject (hMemDC, OldFont);
		SelectObject (hMemDC, OldPen);

		DeleteObject (BGBrush);
		
		DeleteObject (TitleBrush);
		DeleteObject (TitleFont);

		DeleteObject (GridFont);
		DeleteObject (GridPen);

		DeleteObject (LinePen);

		SelectObject (hMemDC, hMemDC_OldBitmap);
		DeleteObject (hMemDC_Bitmap);
		DeleteObject (hMemDC);

	}

	//=========================================
	//�׷��� ������ �ʿ��� ������ ���� ������ �ް� �̸� �����Ѵ�. AlretMax�� GraphMax�� ��� 0�� ��� �۵��� ���Ѵ�.
	//=========================================
	void SetInformation (WCHAR *TitleName, int CPUID, int DataQueue_Num = 1, int QueueMax = 2, int GraphMax = 0, int AlretMax = 0);

	//==============================================
	//������ ���ν���
	//==============================================
	static LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//==============================================
	//������ �Է� �Լ�
	//==============================================
	BOOL InitData (int Data, int CPUID, int Line = 0);
	void Alarm (void);


	//==============================================
	//Client ȭ�� ũ�� ����� �޸� DC���۾� �Լ�
	//==============================================
	void Flip_MemDC (HDC hdc);

	//==============================================
	//Title��, Greed �۾�
	//==============================================
	void Title (void);
	void Grid (void);


	//==============================================
	//�׷��� �Լ�
	//==============================================
	void Line_Single (void);



};