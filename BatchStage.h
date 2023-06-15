#pragma once
#include<iostream>
#include<Windows.h>
#include"HFSP-BVS.h"
#include"Individual.h"

//均匀分割
void BatchCH2(Individual& Ind)
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


	Ind.pJobSplit[BatchStage] = p;
}

//按照容量分割
void BatchCH3(Individual& Ind)
{
	vector<vector<int>> p;
	p.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		p[i].resize(MaxSublotQuantity);
	}

	for (int j = 0; j < pJob; j++)
	{
		int sum = pLot[j];
		for (int e = 0; e < MaxSublotQuantity; e++)
		{
			if (sum >= BatchLimit[j])
			{
				p[j][e] = BatchLimit[j];
				sum -= BatchLimit[j];
			}
			else if (sum > 0 && sum < BatchLimit[j])
			{
				p[j][e] = sum;
				sum = 0;
			}
			else
			{
				p[j][e] = 0;
			}
		}
	}

	Ind.pJobSplit[BatchStage] = p;
}

//随机分割
void BatchCH4(Individual& Ind)
{
	vector<vector<int>> p;
	p.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		p[i].resize(MaxSublotQuantity);
	}

	for (int i = 0; i < pJob; i++)
	{
		int sum = 0;
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			if (j == MaxSublotQuantity - 1)
			{
				p[i][j] = pLot[i] - sum;
			}
			else
			{
				p[i][j] = rand() % (pLot[i] - sum);
				sum += p[i][j];
			}
		}

		sum = 0;
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			if (p[i][j] > BatchLimit[i])
			{
				sum += p[i][j] - BatchLimit[i];
				p[i][j] = BatchLimit[i];

			}
		}

		int num = 0;
		while (sum > 0 && num < MaxSublotQuantity)
		{
			if (p[i][num] < BatchLimit[i])
			{
				int n = BatchLimit[i] - p[i][num];
				if (n >= sum)
				{
					p[i][num] += sum;
					sum = 0;
				}
				else
				{
					p[i][num] = BatchLimit[i];
					sum -= n;
				}
			}
			num++;
		}

		for (int i = 0; i < pJob; i++)
		{
			int k = 0, j = MaxSublotQuantity - 1;
			while (k < j) {
				if (p[i][k] == 0) {
					swap(p[i][k], p[i][j]);
					j--;
				}
				else {
					k++;
				}
			}
		}


		Ind.pJobSplit[BatchStage] = p;
	}

}

void BatchCH(vector<vector<vector<int>>>& pJobSplit)
{
	for (int i = BatchStage + 1; i < pStage; i++)
	{
		pJobSplit[i] = pJobSplit[BatchStage];
	}
}

void NEH(Individual& Ind)
{
	vector<int> SubSeq;
	SubSeq.push_back(Ind.pJobSeq[0]);
	for (int i = 1; i < pJob; i++)
	{
		int BestPosition = 0;
		int BestSubMS = INT_MAX;
		for (int j = 0; j < SubSeq.size() + 1; j++)
		{
			vector<int> tempSeq = SubSeq;
			tempSeq.insert(tempSeq.begin() + j, Ind.pJobSeq[i]);

			int SubMS = Ind.decodeNEH(Ind, tempSeq);

			if (SubMS < BestSubMS)
			{
				BestSubMS = SubMS;
				BestPosition = j;
			}
		}
		SubSeq.insert(SubSeq.begin() + BestPosition, Ind.pJobSeq[i]);
	}
	Ind.pJobSeq = SubSeq;
	Ind.age = 0;
}

