#pragma once
#include "stdafx.h"


template<typename T>
class Queue
{
private:

	T *Que;
	int Front;			//데이터 출력할 위치
	int Rear;			//데이터 입력할 위치
	int QueMax;


public:
	Queue (int Max)			//Queue 초기화
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

	//데이터 뽑기
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

	//데이터 입력
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

	//데이터 보존 그리고 출력
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

	//현재 사용중인 Size
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

	//현재 사용 가능한 Size
	int GetFreeSize (void)
	{
		return QueMax - GetUseSize ();
	}


};