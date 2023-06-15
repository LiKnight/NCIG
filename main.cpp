#include<iostream>
#include"EA.h"
#include"HFSP-BVS.h"
#include"Individual.h"
#include<time.h>
using namespace std;

//说明：五种邻域结构，精英选择+锦标赛选择+每次都扰动
void run(int k)
{
	char name[50] = "EA";
	for (int g = 0; g < 10; g++)
	{
		EA* EA_1 = new EA(10, name);
		EA_1->run(k, g);
		delete EA_1;
	}
}


void main()
{
	int NumberOfInstances = 0;
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 4; i++)
		{

			for (int k = 0; k < 1; k++)
			{

				for (int noi = 0; noi < 5; noi++)
				{
					NumberOfInstances++;
					int InJob = Jobs1[j];
					int InStage = Stages1[i];
					int InType = Types[k];
					GenerateInstances(InJob, InStage, 1, NumberOfInstances + 1234);
					run(InType);
				}
			}
		}
	}


	cout << "................." << endl;
}