#pragma once
#include<vector>
#include "HFSP-BVS.h"
#include <algorithm>
#include<time.h>
#include"Heuristics.h"
#include<Windows.h>


class Individual
{
public:
	//构造函数
	Individual();
	~Individual();
public:
	vector<int> pJobSeq;  //job
	vector<vector<vector<int>>> pJobSplit;  //job-sublot
	vector<int>C;//标记每个工件的完工时间
	int makespan;
	int TPF;
	int age;
public:
	void getNeighborhood(int NeighborhoodIndex, Individual& Neighbor);
	void Check(vector<vector<vector<int>>>& BJobSplit);
public:
	void getdecoding();
	void getdecoding(Individual& Ind);
	int decodeNEH(Individual& Ind, vector<int>& OSeq);
};

//构造函数
Individual::Individual()
{
	//Initialzie the pJobSeq
	pJobSeq.resize(pJob);//生成job序列
	for (int i = 0; i < pJob; i++)
	{
		pJobSeq[i] = i;
	}
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

	Sequence1(pJobSeq);
	Sublot1(pJobSplit);

	age = 0;
	getdecoding();
}

//析构函数 
Individual::~Individual()
{

}

void Individual::getNeighborhood(int NeighborhoodIndex, Individual& Neighbor)
{
	if (NeighborhoodIndex == 1)
	{
		int a = rand() % pJob;
		int b = rand() % pJob;
		while (a == b)
		{
			a = rand() % pJob;
			b = rand() % pJob;
		}//随机选择两个工件位置

		if (b > a)
		{
			int tempValue = Neighbor.pJobSeq[b];
			for (int i = b; i > a; i--)
			{
				Neighbor.pJobSeq[i] = Neighbor.pJobSeq[i - 1];
			}
			Neighbor.pJobSeq[a] = tempValue;
		}//前插
		else
		{
			int tempValue = Neighbor.pJobSeq[b];
			for (int i = b; i < a; i++)
			{
				Neighbor.pJobSeq[i] = Neighbor.pJobSeq[i + 1];
			}
			Neighbor.pJobSeq[a] = tempValue;
		}//后插

		Neighbor.getdecoding();//编码
	}
	else if (NeighborhoodIndex == 2)
	{
		int a = rand() % pJob;
		int b = rand() % pJob;
		while (a == b)
		{
			a = rand() % pJob;
			b = rand() % pJob;
		}

		swap(Neighbor.pJobSeq[a], Neighbor.pJobSeq[b]);

		Neighbor.getdecoding();
	}
	else if (NeighborhoodIndex == 3)
	{
		//任选一个批次
		int r = rand() % pJob;
		int k = 0;
		//任选这个批次的两个子批
		int a, b;
		do
		{
			a = rand() % MaxSublotQuantity;
			b = rand() % MaxSublotQuantity;
		} while (a == b);
		int temp1 = Neighbor.pJobSplit[k][r][a];
		int temp2 = Neighbor.pJobSplit[k][r][b];
		if (Neighbor.pJobSplit[k][r][a] > 0 || Neighbor.pJobSplit[k][r][b] > 0)
		{
			//定义一个步长
			int step = rand() % 3 + 1;
			int r2 = rand() * 1.0 / RAND_MAX;
			if (r2 < 0.5)
			{
				Neighbor.pJobSplit[k][r][a] += step;
				Neighbor.pJobSplit[k][r][b] -= step;
			}
			else
			{
				Neighbor.pJobSplit[k][r][a] -= step;
				Neighbor.pJobSplit[k][r][b] += step;
			}

			if (Neighbor.pJobSplit[k][r][0] == 0 || Neighbor.pJobSplit[k][r][a] < 0 || Neighbor.pJobSplit[k][r][b] < 0)
			{
				Neighbor.pJobSplit[k][r][a] = temp1;
				Neighbor.pJobSplit[k][r][b] = temp2;
			}

		}
		for (int i = k; i < pStage; i++)
		{
			Neighbor.pJobSplit[i] = Neighbor.pJobSplit[k];
		}
		Neighbor.getdecoding();

	}
	else if (NeighborhoodIndex == 4)
	{
		int a = rand() % pJob;
		int b = rand() % pJob;
		while (a == b)
		{
			a = rand() % pJob;
			b = rand() % pJob;
		}

		if (b > a)
		{
			int tempValue = Neighbor.pJobSeq[b];
			for (int i = b; i > a; i--)
			{
				Neighbor.pJobSeq[i] = Neighbor.pJobSeq[i - 1];
			}
			Neighbor.pJobSeq[a] = tempValue;
		}
		else
		{
			int tempValue = Neighbor.pJobSeq[b];
			for (int i = b; i < a; i++)
			{
				Neighbor.pJobSeq[i] = Neighbor.pJobSeq[i + 1];
			}
			Neighbor.pJobSeq[a] = tempValue;
		}


		//任选一个子批数量大于1的批次
		int r = rand() % pJob;
		int k = 0;
		//任选这个批次的两个子批
		do
		{
			a = rand() % MaxSublotQuantity;
			b = rand() % MaxSublotQuantity;
		} while (a == b);

		int temp1 = Neighbor.pJobSplit[k][r][a];
		int temp2 = Neighbor.pJobSplit[k][r][b];
		if (Neighbor.pJobSplit[k][r][a] > 0 || Neighbor.pJobSplit[k][r][b] > 0)
		{
			//定义一个步长
			int step = rand() % 3 + 1;
			int r2 = rand() * 1.0 / RAND_MAX;
			if (r2 < 0.5)
			{
				Neighbor.pJobSplit[k][r][a] += step;
				Neighbor.pJobSplit[k][r][b] -= step;
			}
			else
			{
				Neighbor.pJobSplit[k][r][a] -= step;
				Neighbor.pJobSplit[k][r][b] += step;
			}

			if (Neighbor.pJobSplit[k][r][0] == 0 || Neighbor.pJobSplit[k][r][a] < 0 || Neighbor.pJobSplit[k][r][b] < 0)
			{
				Neighbor.pJobSplit[k][r][a] = temp1;
				Neighbor.pJobSplit[k][r][b] = temp2;
			}

		}
		for (int i = k; i < pStage; i++)
		{
			Neighbor.pJobSplit[i] = Neighbor.pJobSplit[k];
		}

		Neighbor.getdecoding();


	}
	else if (NeighborhoodIndex == 5)
	{
		int a = rand() % pJob;
		int b = rand() % pJob;
		while (a == b)
		{
			a = rand() % pJob;
			b = rand() % pJob;
		}

		swap(Neighbor.pJobSeq[a], Neighbor.pJobSeq[b]);

		//任选一个子批数量大于1的批次
		int r = rand() % pJob;
		int k = 0;
		//任选这个批次的两个子批
		do
		{
			a = rand() % MaxSublotQuantity;
			b = rand() % MaxSublotQuantity;
		} while (a == b);

		int temp1 = Neighbor.pJobSplit[k][r][a];
		int temp2 = Neighbor.pJobSplit[k][r][b];
		if (Neighbor.pJobSplit[k][r][a] > 0 || Neighbor.pJobSplit[k][r][b] > 0)
		{
			//定义一个步长
			int step = rand() % 3 + 1;
			int r2 = rand() * 1.0 / RAND_MAX;
			if (r2 < 0.5)
			{
				Neighbor.pJobSplit[k][r][a] += step;
				Neighbor.pJobSplit[k][r][b] -= step;
			}
			else
			{
				Neighbor.pJobSplit[k][r][a] -= step;
				Neighbor.pJobSplit[k][r][b] += step;
			}

			if (Neighbor.pJobSplit[k][r][0] == 0 || Neighbor.pJobSplit[k][r][a] < 0 || Neighbor.pJobSplit[k][r][b] < 0)
			{
				Neighbor.pJobSplit[k][r][a] = temp1;
				Neighbor.pJobSplit[k][r][b] = temp2;
			}

		}
		for (int i = k; i < pStage; i++)
		{
			Neighbor.pJobSplit[i] = Neighbor.pJobSplit[k];
		}

		Neighbor.getdecoding();

	}
}

void Individual::Check(vector<vector<vector<int>>>& BJobSplit)
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

//解码 子批次优先
void Individual::getdecoding()
{
	Check(pJobSplit);
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
					ch[j].value[e] = CTime[k - 1][j][e] + pTransTime[k - 1][j];  //子批优先
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
					ch[j].value[e] = CTime[k - 1][j][e] + pTransTime[k - 1][j];  //子批优先
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

	makespan = 0;
	for (int i = 0; i < pJob; i++)
	{
		if (makespan < CTime[pStage - 1][i][MaxSublotQuantity - 1])
		{
			makespan = CTime[pStage - 1][i][MaxSublotQuantity - 1];
		}
	}


	C.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		C[Seq[i]] = CTime[pStage - 1][Seq[i]][MaxSublotQuantity - 1];
	}


	//计算累计收益
	vector<int>PF;
	PF.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		for (int j = 1; j < NumberDate + 1; j++)
		{
			if (C[i] < DeliveryDates[j])
			{
				PF[i] = NumberDate - (NumberDate - j + 1);
				break;
			}
			if (j == NumberDate && C[i] > DeliveryDates[j])
			{
				PF[i] = NumberDate;
			}
		}
	}
	TPF = 0;
	for (int i = 0; i < pJob; i++)
	{
		TPF += PF[i];
	}
}

int Individual::decodeNEH(Individual& Ind, vector<int>& OSeq)
{
	vector<int> Seq = OSeq;
	int length = Seq.size();

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

	//all machines are availale at 0  其实可以不用要 默认都为0
	for (int k = 0; k < pStage; k++)
	{
		for (int i = 0; i < pMachines[k]; i++)
		{
			mIdleTime[k][i] = 0;
		}
	}

	vector<vector<vector<int>>> STime, CTime;
	STime.resize(pStage);
	for (int k = 0; k < pStage; k++)
	{
		STime[k].resize(pJob);
	}

	for (int k = 0; k < pStage; k++)
	{
		for (int j = 0; j < length; j++)
		{
			STime[k][Seq[j]].resize(MaxSublotQuantity);
		}
	}
	CTime = STime;

	if (BatchStage == 0)
	{
		for (int j = 0; j < length; j++)
		{
			//Select a machine
			int mpt;
			int	minIdle = INT_MAX;
			//选出最先空闲机器
			for (int i = 0; i < pMachines[0]; i++)
			{
				if (mIdleTime[0][i] < minIdle)
				{
					minIdle = mIdleTime[0][i];
					mpt = i;
				}
			}

			machineAssignment[0][mpt].push_back(Seq[j]);

			for (int e = 0; e < MaxSublotQuantity; e++)
			{
				//第一个子批有启动时间
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
		for (int j = 0; j < length; j++)
		{
			//Select a machine
			int mpt;
			int	minIdle = INT_MAX;
			//选出最先空闲机器
			for (int i = 0; i < pMachines[0]; i++)
			{
				if (mIdleTime[0][i] < minIdle)
				{
					minIdle = mIdleTime[0][i];
					mpt = i;
				}
			}

			machineAssignment[0][mpt].push_back(Seq[j]);

			for (int e = 0; e < MaxSublotQuantity; e++)
			{
				//第一个子批有启动时间
				if (e == 0)
				{
					if (machineAssignment[0][mpt].size() == 1)
					{
						STime[0][Seq[j]][e] = mIdleTime[0][mpt] + pSetupTime[0][Seq[j]][Seq[j]];
						CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]] * Ind.pJobSplit[0][Seq[j]][e];
						mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
					}
					else
					{
						STime[0][Seq[j]][e] = mIdleTime[0][mpt] + pSetupTime[0][machineAssignment[0][mpt][machineAssignment[0][mpt].size() - 2]][Seq[j]];
						CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]] * Ind.pJobSplit[0][Seq[j]][e];
						mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
					}
				}
				else
				{
					STime[0][Seq[j]][e] = mIdleTime[0][mpt];
					CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]] * Ind.pJobSplit[0][Seq[j]][e];
					mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
				}

			}
		}
	}

	for (int k = 1; k < pStage; k++)
	{
		if (BatchStage == k)//如果是批处理阶段
		{
			vector<Pair> ch;
			ch.resize(length);
			//Sorting jobs according to the completion time of the first sublot of jobs in previous stage
			for (int j = 0; j < length; j++)
			{
				ch[j].dim = Seq[j];
				for (int e = 0; e < MaxSublotQuantity; e++)
				{
					ch[j].value[e] = CTime[k - 1][Seq[j]][e] + pTransTime[k - 1][Seq[j]];  //子批优先
				}

			}

			sort(ch.begin(), ch.end(), PairLess());
			for (int j = 0; j < length; j++)
			{
				Seq[j] = ch[j].dim;
			}

			for (int j = 0; j < length; j++)
			{
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
								if (mIdleTime[k][mpt] + pSetupTime[k][Seq[j]][Seq[j]] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
								{
									arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
								}
								else
								{
									arrivedSublot = f;
									break;
								}
							}

							schedulingUnits = arrivedUnits - scheduledUnits;//可以调度的加工单元数量
							if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
							{
								STime[k][Seq[j]][e] = mIdleTime[k][mpt] + pSetupTime[k][Seq[j]][Seq[j]];
							}
							else
							{
								for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
								{
									arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
									schedulingUnits += Ind.pJobSplit[k - 1][Seq[j]][f];


									if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
									{
										STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
										arrivedSublot = f + 1;
										break;
									}
								}
							}
							CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]];
							mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
							scheduledUnits += Ind.pJobSplit[k][Seq[j]][e];
							schedulingUnits -= Ind.pJobSplit[k][Seq[j]][e];
						}
						else
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								if (mIdleTime[k][mpt] + pSetupTime[k][machineAssignment[k][mpt][machineAssignment[k][mpt].size() - 2]][Seq[j]] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
								{
									arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
								}
								else
								{
									arrivedSublot = f;
									break;
								}
							}

							schedulingUnits = arrivedUnits - scheduledUnits;//可以调度的加工单元数量
							if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
							{
								STime[k][Seq[j]][e] = mIdleTime[k][mpt] + pSetupTime[k][machineAssignment[k][mpt][machineAssignment[k][mpt].size() - 2]][Seq[j]];
							}
							else
							{
								for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
								{
									arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
									schedulingUnits += Ind.pJobSplit[k - 1][Seq[j]][f];

									if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
									{
										STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
										arrivedSublot = f + 1;
										break;
									}
								}
							}
							CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]];
							mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
							scheduledUnits += Ind.pJobSplit[k][Seq[j]][e];
							schedulingUnits -= Ind.pJobSplit[k][Seq[j]][e];
						}

					}
					else
					{
						for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
						{
							if (mIdleTime[k][mpt] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
							{
								arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
							}
							else
							{
								arrivedSublot = f;
								break;
							}
						}

						schedulingUnits = arrivedUnits - scheduledUnits;

						if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
						{
							STime[k][Seq[j]][e] = mIdleTime[k][mpt];
						}
						else
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
								schedulingUnits += Ind.pJobSplit[k - 1][Seq[j]][f];

								if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
								{
									STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
									arrivedSublot = f + 1;
									break;
								}
							}
						}

						CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]];
						mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
						scheduledUnits += Ind.pJobSplit[k][Seq[j]][e];
						schedulingUnits -= Ind.pJobSplit[k][Seq[j]][e];
					}
				}
			}
		}
		else
		{
			vector<Pair> ch;
			ch.resize(length);
			//Sorting jobs according to the completion time of the first sublot of jobs in previous stage
			for (int j = 0; j < length; j++)
			{
				ch[j].dim = Seq[j];
				for (int e = 0; e < MaxSublotQuantity; e++)
				{
					ch[j].value[e] = CTime[k - 1][Seq[j]][e] + pTransTime[k - 1][Seq[j]];  //子批优先
				}

			}

			sort(ch.begin(), ch.end(), PairLess());
			for (int j = 0; j < length; j++)
			{
				Seq[j] = ch[j].dim;
			}

			for (int j = 0; j < length; j++)
			{
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
								if (mIdleTime[k][mpt] + pSetupTime[k][Seq[j]][Seq[j]] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
								{
									arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
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
								if (mIdleTime[k][mpt] + pSetupTime[k][machineAssignment[k][mpt][machineAssignment[k][mpt].size() - 2]][Seq[j]] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
								{
									arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
								}
								else
								{
									arrivedSublot = f;
									break;
								}
							}

							schedulingUnits = arrivedUnits - scheduledUnits;//可以调度的加工单元数量
							if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
							{
								STime[k][Seq[j]][e] = mIdleTime[k][mpt] + pSetupTime[k][machineAssignment[k][mpt][machineAssignment[k][mpt].size() - 2]][Seq[j]];
							}
							else
							{
								for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
								{
									arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
									schedulingUnits += Ind.pJobSplit[k - 1][Seq[j]][f];

									if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
									{
										STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
										arrivedSublot = f + 1;
										break;
									}
								}
							}
							CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]] * Ind.pJobSplit[k][Seq[j]][e];
							mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
							scheduledUnits += Ind.pJobSplit[k][Seq[j]][e];
							schedulingUnits -= Ind.pJobSplit[k][Seq[j]][e];
						}
					}
					else
					{
						for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
						{
							if (mIdleTime[k][mpt] >= CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0))
							{
								arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
							}
							else
							{
								arrivedSublot = f;
								break;
							}
						}

						schedulingUnits = arrivedUnits - scheduledUnits;
						if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
						{
							STime[k][Seq[j]][e] = mIdleTime[k][mpt];
						}
						else
						{
							for (int f = arrivedSublot; f < MaxSublotQuantity; f++)
							{
								arrivedUnits += Ind.pJobSplit[k - 1][Seq[j]][f];
								schedulingUnits += Ind.pJobSplit[k - 1][Seq[j]][f];

								if (Ind.pJobSplit[k][Seq[j]][e] <= schedulingUnits)
								{
									STime[k][Seq[j]][e] = CTime[k - 1][Seq[j]][f] + pTransTime[k - 1][Seq[j]] * ((Ind.pJobSplit[k - 1][Seq[j]][f] > 0) ? 1 : 0);
									arrivedSublot = f + 1;
									break;
								}
							}
						}

						CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]] * Ind.pJobSplit[k][Seq[j]][e];
						mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
						scheduledUnits += Ind.pJobSplit[k][Seq[j]][e];
						schedulingUnits -= Ind.pJobSplit[k][Seq[j]][e];
					}

				}

			}

		}
	}

	Ind.makespan = 0;

	for (int i = 0; i < length; i++)
	{
		if (CTime[pStage - 1][Seq[i]][MaxSublotQuantity - 1] > Ind.makespan)
		{
			Ind.makespan = CTime[pStage - 1][Seq[i]][MaxSublotQuantity - 1];
		}
	}

	Ind.C.resize(pJob);
	for (int i = 0; i < length; i++)
	{
		Ind.C[Seq[i]] = CTime[pStage - 1][Seq[i]][MaxSublotQuantity - 1];
	}

	vector<int>PF;
	PF.resize(pJob);
	for (int i = 0; i < length; i++)
	{
		for (int j = 1; j < NumberDate + 1; j++)
		{
			if (Ind.C[Seq[i]] < DeliveryDates[j])
			{
				PF[Seq[i]] = NumberDate - (NumberDate - j + 1);
				break;
			}
			if (j == NumberDate && Ind.C[Seq[i]] > DeliveryDates[j])
			{
				PF[Seq[i]] = NumberDate;
			}
		}
	}
	Ind.TPF = 0;
	for (int i = 0; i < length; i++)
	{
		Ind.TPF += PF[Seq[i]];
	}

	return Ind.TPF;

}