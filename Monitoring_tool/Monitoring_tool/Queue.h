#pragma once
#include "stdafx.h"


template<typename T>
class Queue
{
private:

	T *Que;
	int Front;			//������ ����� ��ġ
	int Rear;			//������ �Է��� ��ġ
	int QueMax;


public:
	Queue (int Max)			//Queue �ʱ�ȭ
	{
		Que = new T[Max];
		Front = 0;
		Rear = 0;
		QueMax = Max;
	}
	~Queue ()
	{
		delete[]Que;
	}

	//������ �̱�
	bool DeQueue (T *pOut)
	{

		if ( (Front % QueMax) == Rear )
		{
			return false;
		}

		*pOut = Que[Front];

		Front = (Front + 1) % QueMax;

		return true;
	}

	//������ �Է�
	bool EnQueue (const T pIn)
	{
		if ( (Rear + 1) % QueMax == Front )
		{
			return false;
		}

		Que[Rear] = pIn;

		Rear = (Rear + 1) % QueMax;

		return true;
	}

	//������ ���� �׸��� ���
	bool Peek (T *pOut, int Num)
	{
		int Buffer;
		Buffer = Front;

		if ( (Front + Num) % QueMax == Rear )
		{
			return false;
		}

		if ( (Rear > Front) && (Front + Num > Rear) )
		{
			return false;
		}

		*pOut = Que[(Buffer + Num) % QueMax];

		return true;

	}

	//���� ������� Size
	int GetUseSize (void)
	{
		if ( Front < Rear )
		{
			return Rear - Front;
		}
		else if ( Front > Rear )
		{
			return (QueMax - Front) + Rear;
		}

		return 0;

	}

	//���� ��� ������ Size
	int GetFreeSize (void)
	{
		return QueMax - GetUseSize ();
	}


};