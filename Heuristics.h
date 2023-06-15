#pragma once
#include<iostream>
#include<Windows.h>
#include"HFSP-BVS.h"
#include"Individual.h"

using namespace std;

//随机序列
void Sequence1(vector<int>& pJobSeq)
{
	random_shuffle(pJobSeq.begin(), pJobSeq.end());
}

//按照设置时间升序
void Sequence2(vector<int>& pJobSeq)
{
	sort(pJobSeq.begin(), pJobSeq.end(), [&](int a, int b) {
		return pSetupTime[0][a][a] < pSetupTime[0][b][b];
		});
}

//按照首阶段处理时间+设置时间升序
void Sequence3(vector<int>& pJobSeq)
{
	if (BatchStage == 0)
	{
		sort(pJobSeq.begin(), pJobSeq.end(), [&](int a, int b) {
			return pSetupTime[0][a][a] + pUnitTime[0][a] < pSetupTime[0][b][b] + pUnitTime[0][b];
			});
	}
	else
	{
		sort(pJobSeq.begin(), pJobSeq.end(), [&](int a, int b) {
			return pSetupTime[0][a][a] + pUnitTime[0][a] * pLot[a] < pSetupTime[0][b][b] * pUnitTime[0][b] * pLot[b];
			});
	}

}

//均匀分割（阶段一致）
void Sublot1(vector<vector<vector<int>>>& pJobSplit)
{
	vector<vector<int>> p;
	p.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		p[i].resize(MaxSublotQuantity);
	}
	for (int i = 0; i < pJob; i++)
	{
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			p[i][j] = floor(pLot[i] / MaxSublotQuantity);//floor函数向下取整数
		}
	}

	vector<int> r;
	r.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		int sum = 0;
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			sum += p[i][j];
		}
		r[i] = pLot[i] - sum;
		int rIndex = rand() % MaxSublotQuantity;
		p[i][rIndex] += r[i];
	}

	for (int i = 0; i < pStage; i++)
	{
		pJobSplit[i] = p;
	}
}

//按照批处理阶段分割
void Sublot2(vector<vector<vector<int>>>& pJobSplit)
{
	for (int j = 0; j < pJob; j++)
	{
		int sum = pLot[j];
		for (int e = 0; e < MaxSublotQuantity; e++)
		{
			if (sum >= BatchLimit[j])
			{
				pJobSplit[0][j][e] = BatchLimit[j];
				sum -= BatchLimit[j];
			}
			else if (sum > 0 && sum < BatchLimit[j])
			{
				pJobSplit[0][j][e] = sum;
				sum = 0;
			}
			else
			{
				pJobSplit[0][j][e] = 0;
			}
		}
	}

	for (int i = 1; i < pStage; i++)
	{
		pJobSplit[i] = pJobSplit[0];
	}
}

//仅批处理阶段按照容量分割
void Sublot3(vector<vector<vector<int>>>& pJobSplit)
{
	for (int j = 0; j < pJob; j++)
	{
		int sum = pLot[j];
		for (int e = 0; e < MaxSublotQuantity; e++)
		{
			if (sum >= BatchLimit[j])
			{
				pJobSplit[BatchStage][j][e] = BatchLimit[j];
				sum -= BatchLimit[j];
			}
			else if (sum > 0 && sum < BatchLimit[j])
			{
				pJobSplit[BatchStage][j][e] = sum;
				sum = 0;
			}
			else
			{
				pJobSplit[BatchStage][j][e] = 0;
			}
		}
	}

}

//随机分割(全部随机)
void Sublot4(vector<vector<vector<int>>>& pJobSplit)
{
	for (int k = 0; k < pStage; k++)
	{
		for (int i = 0; i < pJob; i++)
		{
			int sum = 0;
			for (int j = 0; j < MaxSublotQuantity; j++)
			{
				if (j == MaxSublotQuantity - 1)
				{
					pJobSplit[k][i][j] = pLot[i] - sum;
				}
				else
				{
					pJobSplit[k][i][j] = rand() % (pLot[i] - sum);
					sum += pJobSplit[k][i][j];
				}
			}
		}
	}
}

//仅对批处理阶段进行均匀分割
void Sublot5(vector<vector<vector<int>>>& pJobSplit)
{
	vector<vector<int>> p;
	p.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		p[i].resize(MaxSublotQuantity);
	}
	for (int i = 0; i < pJob; i++)
	{
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			p[i][j] = floor(pLot[i] / MaxSublotQuantity);//floor函数向下取整数
		}
	}

	vector<int> r;
	r.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		int sum = 0;
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			sum += p[i][j];
		}
		r[i] = pLot[i] - sum;
		int rIndex = rand() % MaxSublotQuantity;
		p[i][rIndex] += r[i];
	}

	pJobSplit[BatchStage] = p;
}

//随机分割(阶段一致)
void Sublot6(vector<vector<vector<int>>>& pJobSplit)
{
	for (int i = 0; i < pJob; i++)
	{
		int sum = 0;
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			if (j == MaxSublotQuantity - 1)
			{
				pJobSplit[0][i][j] = pLot[i] - sum;
			}
			else
			{
				pJobSplit[0][i][j] = rand() % (pLot[i] - sum);
				sum += pJobSplit[0][i][j];
			}
		}
	}

	for (int i = 1; i < pStage; i++)
	{
		pJobSplit[i] = pJobSplit[0];
	}
}


//检查解决方案是否合法
void Check(vector<vector<vector<int>>>& BJobSplit)
{
	bool Is = true;
	for (int i = 0; i < pJob; i++)
	{
		for (int e = 0; e < MaxSublotQuantity; e++)
		{
			if (BJobSplit[BatchStage][i][e] > BatchLimit[i])
			{
				Is = false;
			}
		}
	}

	if (Is == false)
	{
		if (rand() * 1.0 / RAND_MAX < 0.5)
		{
			Sublot3(BJobSplit);
		}
		else
		{
			Sublot5(BJobSplit);
		}
	}
}


