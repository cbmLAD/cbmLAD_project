#pragma once
#include "StdAfx.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <numeric>
#include <math.h>
#include <time.h>

using namespace std;

class PatternGeneration
{
private:
	void FindDifferencesSameClass(vector<int>& ObservationAlpha_1, vector<vector<int>  >& BinaryData_1, vector<bool>& CoverdObservation_1, vector<bool> &IgnoredObservation_1, vector<vector<int>  > &DifferencesInSameClass_1);
	void FindDifferencesOppositeClass(vector<int>& ObservationAlpha_1, vector<vector<int>  >& BinaryData_1, vector<bool>& CoverdObservation_1, vector<bool> &IgnoredObservation_1, vector<vector<int>  > &DifferencesInOppositeClass_1);

public:
	PatternGeneration();
	~PatternGeneration();

	vector<vector<double>> PosPatternWeight;

	vector<vector<int>> GeneratePattern(vector<vector<int> > BinaryData, vector <bool> IgnoredObservation, int ClassName, int NumberOfPositiveObservation, vector<bool> CoverdObservation, int V_a);


};

