#pragma once

#include<vector>
#include<iostream>
#include<fstream>
#include"Individual.h"
#include <algorithm>
#include<time.h>
#include<Windows.h>
using namespace std;


//С��ģ  
int Jobs[4] = { 4, 5, 6, 7 };
int Stages[3] = { 2, 3, 4 };
int Types[5] = { 1,2,3,4,5 };
int Jobs1[5] = { 20,40,60,80,100 };
int Stages1[4] = { 3, 5,8,10 };
double value[5] = { 0.1,0.3,0.5,0.7,1.0 };

int NumberDate = 5;

int pJob;
int pStage;
int pType;
int BatchStage;//����������׶�
int Da;
int Ra;
double Dvalue;
double Rvalue;


vector<int> pLot;    //ÿ��Job(Lot)��������       
vector<vector<vector<int>>> pSublot;  // ÿ��Lot�ڸ��׶εķ���
vector<int> pMachines;   //ÿ�׶εĻ�����                
vector<vector<int>> pUnitTime;  //ÿ�׶εĵ�λ�ӹ�ʱ��
vector<vector<vector<int>>>pSetupTime;//�����������ʱ��
vector<vector<int>> pTransTime;//����ʱ��
vector<int>pData;
vector<int>BatchLimit;
const int MaxSublotQuantity = 5;


class Pair//����ÿ���������깤ʱ���������
{
public:
	int dim;
	int value[MaxSublotQuantity];
};

//�ӵ�һ�������ο�ʼ�Ƚϣ�ʱ��С������ǰ��
class PairLess
{
public:
	bool operator() (Pair& a, Pair& b)
	{
		bool result = false;
		int i = 0;
		do
		{
			if (a.value[i] < b.value[i])
			{
				result = true;
			}
			else if (a.value[i] > b.value[i])
			{
				result = false;
			}
			i++;
		} while (a.value[i - 1] == b.value[i - 1] && i < MaxSublotQuantity);
		return result;

	}
};

//��ʱû������ʲô��˼
class PairLess2
{
public:
	bool operator() (Pair& a, Pair& b)
	{

		if (a.value[0] < b.value[0])
		{
			return true;
		}
		else if (a.value[1] == b.value[1])
		{
			if (a.value[1] < b.value[1])
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

	}
};

vector<int>Dk;
//vector<int>ReleaseTime;//����ʱ��
int Cmax;
//ÿ�������Ľ�����
vector<int>DeliveryDates;


void getDeliverDates()
{
	DeliveryDates.resize(NumberDate + 1);
	DeliveryDates[0] = 0;
	DeliveryDates[1] = Dvalue * Cmax / NumberDate;
	for (int k = 2; k < NumberDate + 1; k++)
	{
		DeliveryDates[k] = Dk[1] * k;
	}
	for (int i = 0; i < NumberDate + 1; i++)
	{
		cout << DeliveryDates[i] << " ";
	}
	cout << endl;
}

void getDate()
{
	//Initialzie the pJobSeq
	vector<int>pJobSeq;
	pJobSeq.resize(pJob);//����job����
	for (int i = 0; i < pJob; i++)
	{
		pJobSeq[i] = i;
	}
	random_shuffle(pJobSeq.begin(), pJobSeq.end());//����˳��

	vector<vector<vector<int>>>pJobSplit;
	pJobSplit.resize(pStage);
	for (int i = 0; i < pStage; i++)
	{
		pJobSplit[i].resize(pJob);
	}
	for (int k = 0; k < pStage; k++)
	{
		for (int i = 0; i < pJob; i++)
		{
			pJobSplit[k][i].resize(MaxSublotQuantity);
		}
	}

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

	//��ʼ����
	vector<int> Seq = pJobSeq;
	//���ڱ�ǹ���ÿ���׶�����̨�����ϴ���
	vector<vector<vector<int>>> machineAssignment;
	machineAssignment.resize(pStage);
	for (int k = 0; k < pStage; k++)
	{
		machineAssignment[k].resize(pMachines[k]);
	}

	vector<vector<int>> mIdleTime;
	mIdleTime.resize(pStage);
	for (int k = 0; k < pStage; k++)
	{
		mIdleTime[k].resize(pMachines[k]);
	}

	vector<vector<vector<int>>> STime, CTime;
	STime.resize(pStage);
	for (int k = 0; k < pStage; k++)
	{
		STime[k].resize(pJob);
	}
	for (int k = 0; k < pStage; k++)
	{
		for (int j = 0; j < pJob; j++)
		{
			STime[k][j].resize(MaxSublotQuantity);
		}
	}
	CTime = STime;

	//��һ���׶�(���ж��ǲ���������׶�)
	if (BatchStage == 0)
	{
		for (int j = 0; j < pJob; j++)
		{
			//ѡ�����ȿ��л���
			int mpt;
			int	minIdle = INT_MAX;
			for (int i = 0; i < pMachines[0]; i++)
			{
				if (mIdleTime[0][i] < minIdle)
				{
					minIdle = mIdleTime[0][i];
					mpt = i;
				}
			}
			machineAssignment[0][mpt].push_back(Seq[j]);

			//���������ĵ���
			for (int e = 0; e < MaxSublotQuantity; e++)
			{
				if (e == 0)
				{
					if (machineAssignment[0][mpt].size() == 1)
					{
						STime[0][Seq[j]][e] = mIdleTime[0][mpt] + pSetupTime[0][Seq[j]][Seq[j]];
						CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]];
						mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
					}
					else
					{
						STime[0][Seq[j]][e] = mIdleTime[0][mpt] + pSetupTime[0][machineAssignment[0][mpt][machineAssignment[0][mpt].size() - 2]][Seq[j]];
						CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]];
						mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
					}
				}
				else
				{
					STime[0][Seq[j]][e] = mIdleTime[0][mpt];
					CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]];
					mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
				}

			}
		}
	}
	else
	{
		for (int j = 0; j < pJob; j++)
		{
			int mpt;
			int	minIdle = INT_MAX;
			for (int i = 0; i < pMachines[0]; i++)
			{
				if (mIdleTime[0][i] < minIdle)
				{
					minIdle = mIdleTime[0][i];
					mpt = i;
				}
			}
			machineAssignment[0][mpt].push_back(Seq[j]);

			//���������ĵ���
			for (int e = 0; e < MaxSublotQuantity; e++)
			{
				//��һ������������ʱ��
				if (e == 0)
				{
					if (machineAssignment[0][mpt].size() == 1)
					{
						STime[0][Seq[j]][e] = mIdleTime[0][mpt] + pSetupTime[0][Seq[j]][Seq[j]];
						CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]] * pJobSplit[0][Seq[j]][e];
						mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
					}
					else
					{
						STime[0][Seq[j]][e] = mIdleTime[0][mpt] + pSetupTime[0][machineAssignment[0][mpt][machineAssignment[0][mpt].size() - 2]][Seq[j]];
						CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]] * pJobSplit[0][Seq[j]][e];
						mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
					}
				}
				else
				{
					STime[0][Seq[j]][e] = mIdleTime[0][mpt];
					CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]] * pJobSplit[0][Seq[j]][e];
					mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
				}
			}
		}
	}

	//�����׶�
	for (int k = 1; k < pStage; k++)
	{
		if (BatchStage == k)//�����������׶�
		{
			vector<Pair> ch;
			ch.resize(pJob);
			for (int j = 0; j < pJob; j++)
			{
				ch[j].dim = j;
				for (int e = 0; e < MaxSublotQuantity; e++)
				{
					ch[j].value[e] = CTime[k - 1][j][e];  //��������
				}
			}
			sort(ch.begin(), ch.end(), PairLess());
			for (int j = 0; j < pJob; j++)
			{
				Seq[j] = ch[j].dim;
			}

			//schedule jobs
			for (int j = 0; j < pJob; j++)
			{
				//Select a machine
				int mpt;
				int minIdle = INT_MAX;
				for (int i = 0; i < pMachines[k]; i++)
				{
					if (mIdleTime[k][i] < minIdle)
					{
						minIdle = mIdleTime[k][i];
						mpt = i;
					}
				}
				machineAssignment[k][mpt].push_back(Seq[j]);

				int scheduledUnits = 0;      //�Ѿ����ȹ��ļӹ���Ԫ
				int schedulingUnits = 0;     //���Ե��ȵļӹ���Ԫ
				int arrivedUnits = 0;        //�Ѿ����������
				int arrivedSublot = 0;      //��¼��û�������ڽ�������

				for (int e = 0; e < MaxSublotQuantity; e++)
				{
					if (e == 0)
					{
						if (machineAssignment[k][mpt].size() == 1)
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								if (mIdleTime[k][mpt] + pSetupTime[k][Seq[j]][Seq[j]] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
								{
									arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
								}
								else
								{
									arrivedSublot = f;
									break;
								}
							}

							schedulingUnits = arrivedUnits - scheduledUnits;//���Ե��ȵļӹ���Ԫ����
							if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
							{
								STime[k][Seq[j]][e] = mIdleTime[k][mpt] + pSetupTime[k][Seq[j]][Seq[j]];
							}
							else
							{
								for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
								{
									arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
									schedulingUnits += pJobSplit[k - 1][Seq[j]][f];

									if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
									{
										STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
										arrivedSublot = f + 1;
										break;
									}
								}
							}
							CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]];
							mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
							scheduledUnits += pJobSplit[k][Seq[j]][e];
							schedulingUnits -= pJobSplit[k][Seq[j]][e];
						}
						else
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								if (mIdleTime[k][mpt] + pSetupTime[k][machineAssignment[k][mpt][machineAssignment[k][mpt].size() - 2]][Seq[j]] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
								{
									arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
								}
								else
								{
									arrivedSublot = f;
									break;
								}
							}

							schedulingUnits = arrivedUnits - scheduledUnits;//���Ե��ȵļӹ���Ԫ����
							if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
							{
								STime[k][Seq[j]][e] = mIdleTime[k][mpt] + pSetupTime[k][machineAssignment[k][mpt][machineAssignment[k][mpt].size() - 2]][Seq[j]];
							}
							else
							{
								for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
								{
									arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
									schedulingUnits += pJobSplit[k - 1][Seq[j]][f];

									if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
									{
										STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
										arrivedSublot = f + 1;
										break;
									}
								}
							}
							CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]];
							mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
							scheduledUnits += pJobSplit[k][Seq[j]][e];
							schedulingUnits -= pJobSplit[k][Seq[j]][e];
						}
					}
					else
					{
						for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
						{
							if (mIdleTime[k][mpt] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
							{
								arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
							}
							else
							{
								arrivedSublot = f;
								break;
							}
						}
						schedulingUnits = arrivedUnits - scheduledUnits;
						if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
						{
							STime[k][Seq[j]][e] = mIdleTime[k][mpt];
						}
						else
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
								schedulingUnits += pJobSplit[k - 1][Seq[j]][f];

								if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
								{
									STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
									arrivedSublot = f + 1;
									break;
								}
							}
						}
						CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]];
						mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
						scheduledUnits += pJobSplit[k][Seq[j]][e];
						schedulingUnits -= pJobSplit[k][Seq[j]][e];
					}
				}
			}
		}
		else
		{
			vector<Pair> ch;
			ch.resize(pJob);

			//Sorting jobs according to the completion time of the first sublot of jobs in previous stage
			for (int j = 0; j < pJob; j++)
			{
				ch[j].dim = j;
				for (int e = 0; e < MaxSublotQuantity; e++)
				{
					ch[j].value[e] = CTime[k - 1][j][e];  //��������
				}

			}

			sort(ch.begin(), ch.end(), PairLess());
			for (int j = 0; j < pJob; j++)
			{
				Seq[j] = ch[j].dim;
			}

			//schedule jobs
			for (int j = 0; j < pJob; j++)
			{
				//Select a machine
				int mpt;
				int minIdle = INT_MAX;
				for (int i = 0; i < pMachines[k]; i++)
				{
					if (mIdleTime[k][i] < minIdle)
					{
						minIdle = mIdleTime[k][i];
						mpt = i;
					}
				}
				machineAssignment[k][mpt].push_back(Seq[j]);


				int scheduledUnits = 0;      //�Ѿ����ȹ��ļӹ���Ԫ
				int schedulingUnits = 0;     //���Ե��ȵļӹ���Ԫ
				int arrivedUnits = 0;        //�Ѿ����������
				int arrivedSublot = 0;      //��¼��û�������ڽ�������


				for (int e = 0; e < MaxSublotQuantity; e++)
				{

					if (e == 0)
					{
						if (machineAssignment[k][mpt].size() == 1)
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								if (mIdleTime[k][mpt] + pSetupTime[k][Seq[j]][Seq[j]] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
								{
									arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
								}
								else
								{
									arrivedSublot = f;
									break;
								}
							}

							schedulingUnits = arrivedUnits - scheduledUnits;//���Ե��ȵļӹ���Ԫ����
							if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
							{
								STime[k][Seq[j]][e] = mIdleTime[k][mpt] + pSetupTime[k][Seq[j]][Seq[j]];
							}
							else
							{
								for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
								{
									arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
									schedulingUnits += pJobSplit[k - 1][Seq[j]][f];

									if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
									{
										STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
										arrivedSublot = f + 1;
										break;
									}
								}
							}
							CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]] * pJobSplit[k][Seq[j]][e];
							mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
							scheduledUnits += pJobSplit[k][Seq[j]][e];
							schedulingUnits -= pJobSplit[k][Seq[j]][e];
						}
						else
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								if (mIdleTime[k][mpt] + pSetupTime[k][machineAssignment[k][mpt][machineAssignment[k][mpt].size() - 2]][Seq[j]] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
								{
									arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
								}
								else
								{
									arrivedSublot = f;
									break;
								}
							}

							schedulingUnits = arrivedUnits - scheduledUnits;//���Ե��ȵļӹ���Ԫ����
							if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
							{
								STime[k][Seq[j]][e] = mIdleTime[k][mpt] + pSetupTime[k][machineAssignment[k][mpt][machineAssignment[k][mpt].size() - 2]][Seq[j]];
							}
							else
							{
								for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
								{
									arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
									schedulingUnits += pJobSplit[k - 1][Seq[j]][f];

									if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
									{
										STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
										arrivedSublot = f + 1;
										break;
									}
								}
							}
							CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]] * pJobSplit[k][Seq[j]][e];
							mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
							scheduledUnits += pJobSplit[k][Seq[j]][e];
							schedulingUnits -= pJobSplit[k][Seq[j]][e];
						}

					}
					else
					{
						for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
						{
							if (mIdleTime[k][mpt] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
							{
								arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
							}
							else
							{
								arrivedSublot = f;
								break;
							}
						}

						schedulingUnits = arrivedUnits - scheduledUnits;
						if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
						{
							STime[k][Seq[j]][e] = mIdleTime[k][mpt];
						}
						else
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								arrivedUnits += pJobSplit[k - 1][Seq[j]][f];
								schedulingUnits += pJobSplit[k - 1][Seq[j]][f];

								if (pJobSplit[k][Seq[j]][e] <= schedulingUnits)
								{
									STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
									arrivedSublot = f + 1;
									break;
								}
							}
						}

						CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]] * pJobSplit[k][Seq[j]][e];
						mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
						scheduledUnits += pJobSplit[k][Seq[j]][e];
						schedulingUnits -= pJobSplit[k][Seq[j]][e];
					}

				}
			}
		}
	}

	Cmax = 0;
	for (int i = 0; i < pJob; i++)
	{
		if (CTime[pStage - 1][i][MaxSublotQuantity - 1] > Cmax)
		{
			Cmax = CTime[pStage - 1][i][MaxSublotQuantity - 1];
		}
	}

	cout << Cmax << "||" << endl;

}

void getDkAndRk()
{
	Dk.resize(NumberDate + 1);
	Dk[0] = 0;
	//Ĭ�����õ�һ��Ϊ����
	Dk[1] = Dvalue * (Cmax / NumberDate);
	for (int i = 2; i < NumberDate + 1; i++)
	{
		Dk[i] = i * Dk[1];
	}
	for (int i = 0; i < NumberDate + 1; i++)
	{
		cout << Dk[i] << " ";
	}
	cout << endl;
}

void GenerateInstances(int InJob, int InStage, int InType, int Seed)
{
	srand(Seed);

	pJob = InJob;
	pStage = InStage;

	//���ɻ���
	pMachines.resize(pStage);
	for (int i = 0; i < pStage; i++)
	{
		pMachines[i] = rand() % 5 + 1;
		cout << pMachines[i] << " ";
	}
	cout << endl;

	BatchStage = rand() % pStage;
	//��λ�ӹ�ʱ������
	pUnitTime.resize(InStage);
	for (int i = 0; i < pStage; i++)
	{
		pUnitTime[i].resize(pJob);
	}
	for (int i = 0; i < pStage; i++)
	{
		if (i == BatchStage)
		{
			for (int j = 0; j < pJob; j++)
			{
				pUnitTime[BatchStage][j] = 50 + rand() % 101;
			}
		}
		else
		{
			for (int j = 0; j < pJob; j++)
			{
				pUnitTime[i][j] = 1 + rand() % 10;
			}
		}
	}

	pSetupTime.resize(InStage);
	for (int i = 0; i < pStage; i++)
	{
		pSetupTime[i].resize(pJob);
	}
	for (int i = 0; i < pStage; i++)
	{
		for (int j = 0; j < pJob; j++)
		{
			pSetupTime[i][j].resize(pJob);
		}
	}
	for (int i = 0; i < pStage; i++)
	{
		for (int j = 0; j < pJob; j++)
		{
			for (int j1 = 0; j1 < pJob; j1++)
			{
				pSetupTime[i][j][j1] = 50 + rand() % 51;
			}
		}
	}

	pTransTime.resize(pStage);
	for (int i = 0; i < pStage; i++)
	{
		pTransTime[i].resize(pJob);
	}
	for (int i = 0; i < pStage; i++)
	{
		for (int j = 0; j < pJob; j++)
		{
			pTransTime[i][j] = 10 + rand() % 11;
		}
	}

	//ÿ�����ε�����������
	pLot.resize(pJob);
	for (int j = 0; j < pJob; j++)
	{
		pLot[j] = 50 + rand() % 51;
	}

	Da = rand() % 5;
	Dvalue = value[Da];

	BatchLimit.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		BatchLimit[i] = rand() % 11 + 20;
	}

	getDate();
	getDkAndRk();
	getDeliverDates();
}