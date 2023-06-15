#pragma once
#include<iostream>
#include<Windows.h>
#include"HFSP-BVS.h"
#include"Individual.h"

using namespace std;

//�������
void Sequence1(vector<int>& pJobSeq)
{
	random_shuffle(pJobSeq.begin(), pJobSeq.end());
}

//��������ʱ������
void Sequence2(vector<int>& pJobSeq)
{
	sort(pJobSeq.begin(), pJobSeq.end(), [&](int a, int b) {
		return pSetupTime[0][a][a] < pSetupTime[0][b][b];
		});
}

//�����׽׶δ���ʱ��+����ʱ������
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

//���ȷָ�׶�һ�£�
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
			p[i][j] = floor(pLot[i] / MaxSublotQuantity);//floor��������ȡ����
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

//����������׶ηָ�
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

//��������׶ΰ��������ָ�
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

//����ָ�(ȫ�����)
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

//����������׶ν��о��ȷָ�
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
			p[i][j] = floor(pLot[i] / MaxSublotQuantity);//floor��������ȡ����
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

//����ָ�(�׶�һ��)
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


//����������Ƿ�Ϸ�
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


