// Classification One Vs. One.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>   
#include <set>
#include <map>
#include <numeric>
#include <math.h>
#include <time.h>
#include <stdio.h>


using namespace std;

int main()
{
	vector <vector<double>> PatternMatrix;
	vector <vector<double>> ClassificationData;


	ofstream showClassification("C:\\cbmLAD\\Classification results.txt", ios::out);


	ifstream readClassNames("C:\\cbmLAD\\class names.txt");
	ifstream readPatternMatrix("C:\\cbmLAD\\pattern matrix.txt");


	if (readPatternMatrix)
	{
		string Line;

		while (getline(readPatternMatrix, Line))
		{
			PatternMatrix.push_back(vector<double>());

			stringstream split(Line);
			double Value;

			while (split >> Value)
				PatternMatrix.back().push_back(Value);
		}

	}

	vector <double> ClassNames;

	if (readClassNames)
	{
		string Line;

		while (getline(readClassNames, Line))
		{

			stringstream split(Line);
			double Value;

			while (split >> Value)
				ClassNames.push_back(Value);
		}

	}
	ifstream readClassificationData("C:\\cbmLAD\\classification data.txt");

	if (readClassificationData)
	{
		string Line;

		while (getline(readClassificationData, Line))
		{
			ClassificationData.push_back(vector<double>());

			stringstream split(Line);
			double Value;

			while (split >> Value)
				ClassificationData.back().push_back(Value);
		}

	}

	vector <vector<double>> CoveredByClass; // Classes Scores
	vector <vector<double>> CoveredByPatterns; // The pattern ID (number)
	vector <vector<double>> CoveredByPatternsClass; // The class which the pattern is included in it
	vector <vector<double>> CoveredByPatternsVsClass;// The Versus Class

	for (int i = 0; i< ClassificationData.size(); i++)
	{
		CoveredByClass.push_back(vector<double>());

		for (int j = 0; j < ClassNames.size(); j++)
		{
			CoveredByClass.at(i).push_back(0);
		}

	}

	// check if the patterns cover the tested observations

	for (int i = 0; i < ClassificationData.size(); i++)
	{
		CoveredByPatternsClass.push_back(vector<double>());
		CoveredByPatternsVsClass.push_back(vector<double>());
		CoveredByPatterns.push_back(vector<double>());

		for (int h = 0; h < ClassNames.size(); h++)
		{
			for (int j = 0; j < PatternMatrix.size(); j++)
			{
				int Covered = 0;
				if (PatternMatrix[j][0] == ClassNames[h])
				{
					for (int k = 0; k< (PatternMatrix[j].size() - 4) / 3; k++) // each attribute takes 3 places in the pattern this explains the (/3). The first 3 places are resereved for the class name, pattern number, and pattern weight ( this explains the (-3)
					{
						if (PatternMatrix[j][3 * k + 5] == 1) // 1 means greater than
						{
							if (ClassificationData[i][PatternMatrix[j][3 * k + 4] - 1] > PatternMatrix[j][3 * k + 6])  // keep in mind that test data includes the classification in the first column
								Covered = Covered + 1;

							if (ClassificationData[i][PatternMatrix[j][3 * k + 4] - 1] <= PatternMatrix[j][3 * k + 6])
							{
								break;
							}
						}
						if (PatternMatrix[j][3 * k + 5] == -1) //-1 means less than
						{
							if (ClassificationData[i][PatternMatrix[j][3 * k + 4] - 1] < PatternMatrix[j][3 * k + 6])
								Covered = Covered + 1;
							if (ClassificationData[i][PatternMatrix[j][3 * k + 4] - 1] >= PatternMatrix[j][3 * k + 6])
							{
								break;
							}
						}
					}
				}
				if (Covered == (PatternMatrix[j].size() - 4) / 3)
				{
					CoveredByClass[i][h] = CoveredByClass[i][h] + PatternMatrix[j][3];
					CoveredByPatternsClass.at(i).push_back(ClassNames[h]);
					CoveredByPatternsVsClass.at(i).push_back(PatternMatrix[j][1]);
					CoveredByPatterns.at(i).push_back(PatternMatrix[j][2]);
				}
			}
			// Osama: determine the negative side of the discriminant functions
			for (int j = 0; j < PatternMatrix.size(); j++)
			{
				int Covered = 0;
				if (PatternMatrix[j][1] == ClassNames[h]) // 1 instead of 0
				{
					for (int k = 0; k< (PatternMatrix[j].size() - 4) / 3; k++) // each attribute takes 3 places in the pattern this explains the (/3). The first 4 places are resereved for the classes name, pattern number, and pattern weight ( this explains the (-4)
					{
						if (PatternMatrix[j][3 * k + 5] == 1) // 1 means greater than
						{
							if (ClassificationData[i][PatternMatrix[j][3 * k + 4] - 1] > PatternMatrix[j][3 * k + 6])  // keep in mind that test data includes the classification in the first column
								Covered = Covered + 1;

							if (ClassificationData[i][PatternMatrix[j][3 * k + 4] - 1] < PatternMatrix[j][3 * k + 6])
							{
								break;
							}
						}
						if (PatternMatrix[j][3 * k + 5] == -1) //-1 means less than
						{
							if (ClassificationData[i][PatternMatrix[j][3 * k + 4] - 1] < PatternMatrix[j][3 * k + 6])
								Covered = Covered + 1;
							if (ClassificationData[i][PatternMatrix[j][3 * k + 4] - 1] > PatternMatrix[j][3 * k + 6])
							{
								break;
							}
						}
					}
				}
				if (Covered == (PatternMatrix[j].size() - 4) / 3)
				{
					CoveredByClass[i][h] = CoveredByClass[i][h] - PatternMatrix[j][3];
					//CoveredByPatternsClass.at(i).push_back(ClassNames[h]);

					//CoveredByPatterns.at(i).push_back(PatternMatrix[j][2]);// To be edited
				}
			}
		}

	}


	// end of check if the patterns cover the tested observations

	vector<double> ClassFound(ClassificationData.size());

	for (unsigned int i = 0; i < CoveredByClass.size(); i++)
	{
		double maxClass = -1;
		for (unsigned int j = 0; j < CoveredByClass.at(i).size(); j++)
		{
			if (CoveredByClass[i][j] > maxClass && CoveredByClass[i][j]> 0)
			{
				maxClass = CoveredByClass[i][j];
				ClassFound[i] = ClassNames[j];
			}

		}
	}

	for (unsigned int i = 0; i < CoveredByClass.size(); i++)
	{

		showClassification << "[" << i + 1 << "]  is classified into class { " << ClassFound[i] << " }"<< endl;

		showClassification << "Classes scores:   ";
		for (unsigned int j = 0; j < CoveredByClass.at(i).size(); j++)
		{
			showClassification << "[" << ClassNames[j] << "]" << CoveredByClass.at(i).at(j) << "   ";
		}
		showClassification << endl;
		showClassification << "Covered by patterns:   ";
		for (unsigned int j = 0; j < CoveredByPatterns[i].size(); j++)
		{
			showClassification << "[" << CoveredByPatternsClass[i][j] << "V" << CoveredByPatternsVsClass[i][j] << "]";
			showClassification << CoveredByPatterns[i][j] << "   ";
		}
		showClassification << endl;
		showClassification << " --------------------------------------" << endl;

	}


}


