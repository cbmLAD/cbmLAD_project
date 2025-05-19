#pragma once
#include "StdAfx.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define TRAINING		0
#define TESTING			1


class Binarization
{
protected:

	//char* fileNam;

	vector <vector <int> > res_binarization;
	vector <vector <int> > res_binarization_testSet;
	//int litNum;


public:
	Binarization(void);
	~Binarization(void);

	//** in CBMDoc now** vector <vector<double>>	ReadOriginalFile(char* fileNam);
	vector<vector<vector<double>>> GenerateAttributeColumnVectors(vector<vector<double>> vRawData);
	vector<vector<vector<double>>> SortAttributeValues(vector<vector<vector<double>>> vAllAttributeColumns);
	int FindMinimalNumerical(vector <vector <double> > workingVector, int lastSortedElementOutput);
	int FindMinimalOrdered(vector <vector <double> > workingVector);
	vector<vector<double>> ExtractCutPoints(vector<vector<vector<double>>> vAllAttributes, int ClassName);
	vector<vector<int>> GetBinaryTraining(vector<vector<double>> vCutPoints, vector<vector<double>> vRawData);
	};

