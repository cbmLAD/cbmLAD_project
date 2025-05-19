#pragma once
#include "StdAfx.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Pattern_Interpretation
{
public:
	Pattern_Interpretation();
	~Pattern_Interpretation();

	vector<vector<vector<double>>>  Interpret2ClassPatterns(vector<string> attribute_Names, vector<vector<int>> vPatterns, vector<vector<double>> vCutPoints, int ClassName);

};

