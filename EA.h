#define _CRT_SECURE_NO_WARNINGS
#include "Individual.h"
#include"Algorithm.h"
#include <Windows.h>
#include"HFSP-BVS.h"
#include <random>
#include <unordered_set>
#include"BatchStage.h"
long StartTime = 0;
long EndTime = 0;
class EA :public Algorithm
{
public:
	virtual void InitializePop();
	virtual void EvolvePop();
	virtual void OutputBest(int k, int rep, double time);
	virtual void run(int k, int rep);
public:
	EA(int CvndIter, char* name);
	void UpdateBestSoFar(Individual& Ind);
	bool UpdatePrimary(Individual& Ind);
	void UpdateArchive(Individual& Ind);
	void CollaborativeOperator(Individual& Ind);
	void DestructionAndReconstruction(Individual& Ind);
	void ArchiveSetPermutate();
	void AssociativeHeuristic();
	~EA();
public:
	vector<int>DeSeq;
public:
	int Popsize = 9;
	int CvndIter;
	char* name;
public:
	int CountForNeighborhood;//计数器，邻域搜索计数
	int ArchiveAge = 0;
	Individual Primary;
	Individual BestSoFar;
	vector<Individual>Archive;
};

EA::EA(int CvndIter, char* name)
{
	this->CvndIter = CvndIter;
	this->name = name;
}

EA::~EA()
{

}

void EA::InitializePop()
{
	AssociativeHeuristic();
	Archive.resize(Popsize);
	ArchiveAge = 0;
	BestSoFar.TPF = INT_MAX;
	BestSoFar.makespan = INT_MAX;
	CollaborativeOperator(Primary);
	cout << Primary.TPF << endl;
	UpdateBestSoFar(Primary);
}

void EA::EvolvePop()
{
	long Time = GetTickCount() - StartTime;
	long TimeLimit = (80 * pJob * pStage);
	long InitTime = GetTickCount();
	int cnt = 0;
	int NeighborhoodIndex = 1;

	while (GetTickCount() - InitTime < TimeLimit)
	{
		DestructionAndReconstruction(Primary);
		UpdateBestSoFar(Primary);

		//局部搜索
		Individual Ind = Primary;
		Individual Neighbor;
		int Index = 1;
		while (Index <= 5)
		{
			int VndSearchCount = 0;
			while (VndSearchCount < CvndIter)
			{
				Neighbor = Ind;
				Ind.getNeighborhood(Index, Neighbor);
				if (Neighbor.TPF < Ind.TPF)
				{
					Index = 1;
					Ind = Neighbor;
					VndSearchCount = 0;
				}
				else
				{
					VndSearchCount++;
				}
			}
			Index++;
		}

		double Ae = 0.8 - ((GetTickCount() - InitTime) / TimeLimit * 0.7);
		bool Is = UpdatePrimary(Ind);
		if (Is)
		{
			UpdateBestSoFar(Primary);
			UpdateArchive(Primary);
		}
		else
		{
			if (rand() * 1.0 / RAND_MAX < Ae)
			{
				Primary = Ind;
			}
			else
			{
				CollaborativeOperator(Primary);
			}
		}
		cnt++;

	}
	cout << cnt << endl;
}

void EA::OutputBest(int k, int rep, double time)
{
	cout << pJob << "_" << pStage << "_" << k << "_" << rep << ":" << BestSoFar.TPF << "  " << BestSoFar.makespan << "   时间：" << time << endl;
	//文件输出
	ofstream output;
	output.open("number.txt", ios::app);
	output << BestSoFar.TPF << " ";
	if ((rep + 1) % 10 == 0)
		output << '\n';
	output.close();
}

void EA::run(int k, int rep)
{
	StartTime = GetTickCount();
	InitializePop();
	EvolvePop();
	EndTime = GetTickCount();
	double UsedTime = (EndTime - StartTime) * 1.0 / 1000;
	OutputBest(k, rep, UsedTime);
}

void EA::AssociativeHeuristic()
{
	Archive.resize(Popsize);

	//第一种
	Sequence1(Archive[0].pJobSeq);
	Sublot1(Archive[0].pJobSplit);
	NEH(Archive[0]);

	//第一种
	Sequence1(Archive[1].pJobSeq);
	Sublot2(Archive[1].pJobSplit);
	NEH(Archive[1]);

	//第一种
	Sequence1(Archive[2].pJobSeq);
	Sublot6(Archive[2].pJobSplit);
	NEH(Archive[2]);

	//第一种
	Sequence2(Archive[3].pJobSeq);
	Sublot1(Archive[3].pJobSplit);
	NEH(Archive[3]);

	//第一种
	Sequence2(Archive[4].pJobSeq);
	Sublot2(Archive[4].pJobSplit);
	NEH(Archive[4]);

	//第一种
	Sequence2(Archive[5].pJobSeq);
	Sublot6(Archive[5].pJobSplit);
	NEH(Archive[5]);

	//第一种
	Sequence3(Archive[6].pJobSeq);
	Sublot1(Archive[6].pJobSplit);
	NEH(Archive[6]);

	//第一种
	Sequence3(Archive[7].pJobSeq);
	Sublot2(Archive[7].pJobSplit);
	NEH(Archive[7]);

	//第一种
	Sequence3(Archive[8].pJobSeq);
	Sublot6(Archive[8].pJobSplit);
	NEH(Archive[8]);
}

void EA::ArchiveSetPermutate()
{
	//选择一半的个体进行替换
	int a = floor(Popsize / 2);

	vector<int> temp;


	int count = 0;
	do
	{
		int r = rand() % Popsize;
		bool isFound = false;
		for (vector<int>::iterator it = temp.begin(); it != temp.end(); it++)
		{
			if (*it == r)
			{
				isFound = true;
				break;
			}
		}
		if (!isFound)
		{
			temp.push_back(r);
			count++;
		}

	} while (count < a);//一共执行a次，生成a个范围内随机数

	//随机产生a个个体进行替换

	for (int i = 0; i < a; i++)
	{
		Individual it;

		//子批划分
		for (int k = 0; k < pJob; k++)
		{
			int RemainingSize = pLot[k];
			for (int j = 0; j < MaxSublotQuantity; j++)//分批
			{
				if (j == 0)
				{
					do
					{
						it.pJobSplit[0][k][j] = rand() % RemainingSize;
					} while (it.pJobSplit[0][k][j] == 0);

					RemainingSize -= it.pJobSplit[0][k][j];

				}
				if (j > 0 && j < MaxSublotQuantity - 1)
				{
					it.pJobSplit[0][k][j] = rand() % RemainingSize;
					RemainingSize -= it.pJobSplit[0][k][j];
				}
				if (j == MaxSublotQuantity - 1)
				{
					it.pJobSplit[0][k][j] = RemainingSize;
				}

			}
		}

		for (int i = 0; i < pStage; i++)
		{
			it.pJobSplit[i] = it.pJobSplit[0];
		}

		it.getdecoding();//求解时间

		UpdateBestSoFar(it);//最优解更新

		Archive[temp[i]] = it;//归档集更新，更新的为temp[i]位置上的解
	}
}

bool EA::UpdatePrimary(Individual& Ind)
{
	if (Primary.TPF > Ind.TPF)
	{
		Primary = Ind;
		Primary.age = 0;
		return true;
	}
	//else if (Primary.TPF == Ind.TPF && Primary.makespan > Ind.makespan)
	//{
		//Primary = Ind;
		//Primary.age = 0;
		//return true;
	//}
	else
	{
		Primary.age++;
		return false;
	}
}

void EA::UpdateBestSoFar(Individual& Ind)
{
	if (BestSoFar.TPF > Ind.TPF)
	{
		BestSoFar = Ind;
	}
	else if (BestSoFar.TPF == Ind.TPF && BestSoFar.makespan > Ind.makespan)
	{
		BestSoFar = Ind;
	}
}

//更新归档中最坏的解决方案
void EA::UpdateArchive(Individual& Ind)
{
	//选出最差的
	int pos = 0;
	for (int i = 1; i < Popsize; i++)
	{
		if (Archive[i].TPF > Archive[pos].TPF)
		{
			pos = i;
		}
		else if (Archive[i].TPF == Archive[pos].TPF && Archive[i].makespan > Archive[pos].makespan)
		{
			pos = i;
		}
	}

	if (Archive[pos].TPF > Ind.TPF)
	{
		Archive[pos] = Ind;
		ArchiveAge = 0;
	}
	else if (Archive[pos].TPF == Ind.TPF && Archive[pos].makespan > Ind.makespan)
	{
		Archive[pos] = Ind;
		ArchiveAge = 0;
	}
	else
	{
		ArchiveAge++;
	}
}



void EA::CollaborativeOperator(Individual& Ind)
{
	vector<Individual>ArchiveSet = Archive;
	int pt1, pt2;
	do
	{
		pt1 = rand() % Popsize;
		pt2 = rand() % Popsize;
	} while (pt1 == pt2);
	for (int i = 0; i < pJob; i++)
	{
		Ind.pJobSeq[i] = -1;
	}
	int a, b, m;
	int temp;
	do
	{
		a = rand() % pJob;
		b = rand() % pJob;
	} while (a == b);

	if (a > b)
	{
		m = a;
		a = b;
		b = m;
	}
	if (a != 0)
	{
		temp = 0;
	}
	else
	{
		temp = b + 1;
	}
	for (int i = a; i <= b; i++)
	{
		Ind.pJobSeq[i] = ArchiveSet[pt1].pJobSeq[i];
	}
	for (int i = 0; i < pJob; i++)
	{
		vector<int>::iterator found = find(Ind.pJobSeq.begin(), Ind.pJobSeq.end(), ArchiveSet[pt2].pJobSeq[i]);
		if (found == Ind.pJobSeq.end())
		{
			Ind.pJobSeq[temp] = ArchiveSet[pt2].pJobSeq[i];
			temp++;

			if (temp == a)
			{
				temp = b + 1;
			}
		}
	}


	for (int i = 0; i < pJob; i++)
	{
		do
		{
			a = rand() % Popsize;
			b = rand() % Popsize;
		} while (a == b);
		if (Archive[a].TPF < Archive[b].TPF)
		{
			Ind.pJobSplit[0][i] = Archive[a].pJobSplit[0][i];
		}
		else if (Archive[a].TPF == Archive[b].TPF && Archive[a].makespan < Archive[b].makespan)
		{
			Ind.pJobSplit[0][i] = Archive[a].pJobSplit[0][i];
		}
		else
		{
			Ind.pJobSplit[0][i] = Archive[b].pJobSplit[0][i];
		}
	}

	for (int i = 0; i < pStage; i++)
	{
		Ind.pJobSplit[i] = Ind.pJobSplit[0];
	}

	Ind.getdecoding();
}



void EA::DestructionAndReconstruction(Individual& Ind)
{
	vector<Individual>dc;
	dc.resize(6);
	for (int i = 0; i < 6; i++)
	{
		dc[i] = Ind;
	}

	BatchCH2(dc[0]);
	dc[0].getdecoding();

	BatchCH3(dc[1]);
	dc[1].getdecoding();

	BatchCH4(dc[2]);
	dc[2].getdecoding();

	BatchCH2(dc[3]);
	BatchCH(dc[3].pJobSplit);
	dc[3].getdecoding();

	BatchCH3(dc[4]);
	BatchCH(dc[4].pJobSplit);
	dc[4].getdecoding();

	BatchCH4(dc[5]);
	BatchCH(dc[5].pJobSplit);
	dc[5].getdecoding();


	for (int i = 0; i < 6; i++)
	{
		if (dc[i].TPF < Ind.TPF)
		{
			Ind = dc[i];
		}
		else if (dc[i].TPF == Ind.TPF && dc[i].makespan < Ind.makespan)
		{
			Ind = dc[i];
		}
	}

}
