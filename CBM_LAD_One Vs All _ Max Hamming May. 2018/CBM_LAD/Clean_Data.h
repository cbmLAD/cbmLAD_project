#pragma once
#include "StdAfx.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class Clean_Data
{
public:
	Clean_Data(void);
	~Clean_Data(void);
	vector < vector <double> > cleanData(vector < vector <double> >& inputVec);
};

