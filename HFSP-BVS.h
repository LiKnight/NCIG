#pragma once

#include<vector>
#include<iostream>
#include<fstream>
#include"Individual.h"
#include <algorithm>
#include<time.h>
#include<Windows.h>
using namespace std;


//小规模  
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
int BatchStage;//定义批处理阶段
int Da;
int Ra;
double Dvalue;
double Rvalue;


vector<int> pLot;    //每个Job(Lot)的总批量       
vector<vector<vector<int>>> pSublot;  // 每个Lot在各阶段的分批
vector<int> pMachines;   //每阶段的机器数                
vector<vector<int>> pUnitTime;  //每阶段的单位加工时间
vector<vector<vector<int>>>pSetupTime;//序列相关设置时间
vector<vector<int>> pTransTime;//运输时间
vector<int>pData;
vector<int>BatchLimit;
const int MaxSublotQuantity = 5;


class Pair//根据每个工件的完工时间进行排序
{
public:
	int dim;
	int value[MaxSublotQuantity];
};

//从第一个子批次开始比较，时间小的排在前面
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

//暂时没看懂是什么意思
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
//vector<int>ReleaseTime;//发布时间
int Cmax;
//每个工件的交货期
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
	pJobSeq.resize(pJob);//生成job序列
	for (int i = 0; i < pJob; i++)
	{
		pJobSeq[i] = i;
	}
	random_shuffle(pJobSeq.begin(), pJobSeq.end());//打乱顺序

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

	//开始解码
	vector<int> Seq = pJobSeq;
	//用于标记工件每个阶段在哪台机器上处理
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

	//第一个阶段(先判断是不是批处理阶段)
	if (BatchStage == 0)
	{
		for (int j = 0; j < pJob; j++)
		{
			//选出最先空闲机器
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

			//工件批量的调度
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

			//工件批量的调度
			for (int e = 0; e < MaxSublotQuantity; e++)
			{
				//第一个子批有启动时间
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

	//其他阶段
	for (int k = 1; k < pStage; k++)
	{
		if (BatchStage == k)//如果是批处理阶段
		{
			vector<Pair> ch;
			ch.resize(pJob);
			for (int j = 0; j < pJob; j++)
			{
				ch[j].dim = j;
				for (int e = 0; e < MaxSublotQuantity; e++)
				{
					ch[j].value[e] = CTime[k - 1][j][e];  //子批优先
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

				int scheduledUnits = 0;      //已经调度过的加工单元
				int schedulingUnits = 0;     //可以调度的加工单元
				int arrivedUnits = 0;        //已经到达的总数
				int arrivedSublot = 0;      //记录还没到的最邻近的子批

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

							schedulingUnits = arrivedUnits - scheduledUnits;//可以调度的加工单元数量
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

							schedulingUnits = arrivedUnits - scheduledUnits;//可以调度的加工单元数量
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
					ch[j].value[e] = CTime[k - 1][j][e];  //子批优先
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


				int scheduledUnits = 0;      //已经调度过的加工单元
				int schedulingUnits = 0;     //可以调度的加工单元
				int arrivedUnits = 0;        //已经到达的总数
				int arrivedSublot = 0;      //记录还没到的最邻近的子批


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

							schedulingUnits = arrivedUnits - scheduledUnits;//可以调度的加工单元数量
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

							schedulingUnits = arrivedUnits - scheduledUnits;//可以调度的加工单元数量
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
	//默认设置第一个为虚拟
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

	//生成机器
	pMachines.resize(pStage);
	for (int i = 0; i < pStage; i++)
	{
		pMachines[i] = rand() % 5 + 1;
		cout << pMachines[i] << " ";
	}
	cout << endl;

	BatchStage = rand() % pStage;
	//单位加工时间设置
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

	//每个批次的总批量设置
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