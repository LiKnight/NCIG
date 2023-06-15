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
	//���캯��
	Individual();
	~Individual();
public:
	vector<int> pJobSeq;  //job
	vector<vector<vector<int>>> pJobSplit;  //job-sublot
	vector<int>C;//���ÿ���������깤ʱ��
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

//���캯��
Individual::Individual()
{
	//Initialzie the pJobSeq
	pJobSeq.resize(pJob);//����job����
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

//�������� 
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
		}//���ѡ����������λ��

		if (b > a)
		{
			int tempValue = Neighbor.pJobSeq[b];
			for (int i = b; i > a; i--)
			{
				Neighbor.pJobSeq[i] = Neighbor.pJobSeq[i - 1];
			}
			Neighbor.pJobSeq[a] = tempValue;
		}//ǰ��
		else
		{
			int tempValue = Neighbor.pJobSeq[b];
			for (int i = b; i < a; i++)
			{
				Neighbor.pJobSeq[i] = Neighbor.pJobSeq[i + 1];
			}
			Neighbor.pJobSeq[a] = tempValue;
		}//���

		Neighbor.getdecoding();//����
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
		//��ѡһ������
		int r = rand() % pJob;
		int k = 0;
		//��ѡ������ε���������
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
			//����һ������
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


		//��ѡһ��������������1������
		int r = rand() % pJob;
		int k = 0;
		//��ѡ������ε���������
		do
		{
			a = rand() % MaxSublotQuantity;
			b = rand() % MaxSublotQuantity;
		} while (a == b);

		int temp1 = Neighbor.pJobSplit[k][r][a];
		int temp2 = Neighbor.pJobSplit[k][r][b];
		if (Neighbor.pJobSplit[k][r][a] > 0 || Neighbor.pJobSplit[k][r][b] > 0)
		{
			//����һ������
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

		//��ѡһ��������������1������
		int r = rand() % pJob;
		int k = 0;
		//��ѡ������ε���������
		do
		{
			a = rand() % MaxSublotQuantity;
			b = rand() % MaxSublotQuantity;
		} while (a == b);

		int temp1 = Neighbor.pJobSplit[k][r][a];
		int temp2 = Neighbor.pJobSplit[k][r][b];
		if (Neighbor.pJobSplit[k][r][a] > 0 || Neighbor.pJobSplit[k][r][b] > 0)
		{
			//����һ������
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

//���� ����������
void Individual::getdecoding()
{
	Check(pJobSplit);
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
					ch[j].value[e] = CTime[k - 1][j][e] + pTransTime[k - 1][j];  //��������
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
					ch[j].value[e] = CTime[k - 1][j][e] + pTransTime[k - 1][j];  //��������
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


	//�����ۼ�����
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

	//all machines are availale at 0  ��ʵ���Բ���Ҫ Ĭ�϶�Ϊ0
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
			//ѡ�����ȿ��л���
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
				//��һ������������ʱ��
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
			//ѡ�����ȿ��л���
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
				//��һ������������ʱ��
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
		if (BatchStage == k)//�����������׶�
		{
			vector<Pair> ch;
			ch.resize(length);
			//Sorting jobs according to the completion time of the first sublot of jobs in previous stage
			for (int j = 0; j < length; j++)
			{
				ch[j].dim = Seq[j];
				for (int e = 0; e < MaxSublotQuantity; e++)
				{
					ch[j].value[e] = CTime[k - 1][Seq[j]][e] + pTransTime[k - 1][Seq[j]];  //��������
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

							schedulingUnits = arrivedUnits - scheduledUnits;//���Ե��ȵļӹ���Ԫ����
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

							schedulingUnits = arrivedUnits - scheduledUnits;//���Ե��ȵļӹ���Ԫ����
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
					ch[j].value[e] = CTime[k - 1][Seq[j]][e] + pTransTime[k - 1][Seq[j]];  //��������
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

							schedulingUnits = arrivedUnits - scheduledUnits;//���Ե��ȵļӹ���Ԫ����
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