#pragma once
#include"Queue.h"
#define dfMAXCHILD	100
#define MaxString 100

#define UM_Alret WM_USER+1
#define AlarmMax 500					//�˶��� �۵��Ǹ� �︮��

#define TitleBarLength 30
#define BarNameLength 55
#define BarNamespace 20

#define dfTitleMax 30
#define dfunitMax 30

#define Pen_Max 10
#define Bar_Max 5






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


	typedef struct Culumn_INFO
	{
		ULONG u64ServerID;							// �ش� ����Ϳ��� �޾ƾߵǴ� ServerID ����
		int iType;									// �ش� ����Ϳ��� ���� Ÿ������
		WCHAR Column_Name[dfTitleMax];
		Queue<int> *DataArray;							//�ش� ������� queue
	}stColumnInfo;

	stColumnInfo *ColumnArray;


private:



	HWND hWnd;
	HWND hWnd_Parent;

	HINSTANCE hInst;							// ���� �ν��Ͻ� �Դϴ�.
	WCHAR TitleName[dfTitleMax];				// �ش�â�� Ÿ��Ʋ�� �� �̸��Դϴ�.
	WCHAR unit[dfunitMax];						// �ش� �ν��Ͻ��� ��ġ�� ǥ�� ���� �Դϴ�.

	TYPE GraphType;								// ���� â �׷��� Ÿ���Դϴ�.


	int Column_Max;								// Culumn�ǰ���.
	int Queue_NodeMax;							// Queue���� ����� Max��.

	int Alarm_Max;								// �ش� ��ġ�� �����ϸ� �˶� �߻�.
	int Graph_Max;								// �ش�׷����� Max��.
	bool Graph_Flow;							// Max���� ���� �����׷����Ͻ� true.

	//���� �����
	COLORREF BG_Color;							// ��׶��� �÷��Դϴ�.
	

	HDC hMemDC;									//�޸� DC�� ����ϱ� ���� �ʿ��� �͵�.
	HBITMAP hMemDC_Bitmap;
	HBITMAP hMemDC_OldBitmap;

	RECT MemSize;								//�޸� DC�� ������
	RECT GraphSize;								//�޸� DC�� �׷��� �׷��� ������
		
	HBRUSH BGBrush;								//��׶���� �귯��


	HBRUSH TitleBrush;							//Ÿ��Ʋ�� �귯��
	HFONT TitleFont;							//Ÿ��Ʋ�� ��Ʈ
	COLORREF TitleColor;						//Ÿ��Ʋ�� �ؽ�Ʈ ����
	COLORREF AlarmColor;						//�˶��� �۵��� ��� Ÿ��Ʋ ������ ���������� �ٲ�.
	bool AlarmFlag;								//�˶��� �۵��� ��� true�� �ٲ�.
	
	HFONT GridFont;								//�׸���� ��Ʈ
	HPEN GridPen;								//�׸���� ��

	HFONT additionFont;								//���� �ΰ� ����� ��Ʈ
	HPEN additionPen;								//���� �ΰ� ����� ��
	HBRUSH additionBrush;							//���� �ΰ� ����� �귯��

	HPEN LinePen[Pen_Max];							//���ο� ��
	HBRUSH BARBRUSH[Bar_Max];						//Bar�� ���� �귯��
	HFONT BARDataFont;								//Bar ��� ��ġ ǥ��

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
	CMonitor_GraphUnit (WCHAR * Title, WCHAR *ColumnUnit, HINSTANCE hInstance, HWND hWndParent, COLORREF BackColor, TYPE enType, int iPosX, int iPosY, int iWidth, int iHeight);

	//=========================================
	//�ı���. �����Ҵ� ���� queue�� �ı��ϰ� �����Ѵ�.
	//=========================================
	~CMonitor_GraphUnit (void)
	{
		int cnt;
		RemoveThis (hWnd);
		for ( cnt = 0; cnt < Column_Max; cnt++ )
		{
			delete ColumnArray[cnt].DataArray;
		}
		delete[]ColumnArray;

		//SelectObject�� �⺻ �귯���� �⺻ ��Ʈ, �⺻ ������ �ǵ��� �� ������ �̸� �����س� ��� ��Ʈ�� ��,�귯���� �����Ѵ�.
		SelectObject (hMemDC, OldBrush);
		SelectObject (hMemDC, OldFont);
		SelectObject (hMemDC, OldPen);

		DeleteObject (BGBrush);
		
		DeleteObject (TitleBrush);
		DeleteObject (TitleFont);

		DeleteObject (GridFont);
		DeleteObject (GridPen);

		DeleteObject (additionFont);								//���� �ΰ� ����� ��Ʈ
		DeleteObject (additionPen);									//���� �ΰ� ����� ��
		DeleteObject (additionBrush);								//���� �ΰ� ����� �귯��

		for ( int cnt = 0; cnt < Pen_Max; cnt++ )
		{
			DeleteObject (LinePen[cnt]);
		}
		for ( int cnt = 0; cnt < Bar_Max; cnt++ )
		{
			DeleteObject (BARBRUSH[cnt]);
		}
		DeleteObject (BARDataFont);

		SelectObject (hMemDC, hMemDC_OldBitmap);
		DeleteObject (hMemDC_Bitmap);
		DeleteObject (hMemDC);

	}

	//=========================================
	//�׷��� ������ �ʿ��� ������ ���� ������ �ް� �̸� �����Ѵ�. AlretMax�� GraphMax�� ��� 0�� ��� �۵��� ���Ѵ�.
	//=========================================
	void CMonitorGraphUnit (int CulumnMax = 1, int QueueNodeMax = 2, int GraphMax = 0, int AlretMax = 0);
	void DataColumnInfo (int ColumnNum, WCHAR *ColumnName, ULONG ServerID, int Type);

	//==============================================
	//������ ���ν���
	//==============================================
	static LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//==============================================
	//������ �Է� �Լ�
	//==============================================
	BOOL InitData ( ULONG ServerID, int Data, int iType);
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
	void MultLine_addition (void);
	void Bottom_NameBar (void);

	//==============================================
	//�׷��� �Լ�
	//==============================================
	void Print_Line_Single (void);
	void Print_Line_Multi (void);
	void Print_Bar_Single (void);



};