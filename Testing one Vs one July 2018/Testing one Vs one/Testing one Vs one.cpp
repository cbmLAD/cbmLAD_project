// Testing Multi-Class with Confusion Matrix.cpp : Defines the entry point for the console application.
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
	ofstream showClassification("C:\\cbmLAD\\Testing results.txt", ios::out);
	ofstream confMatrix("C:\\cbmLAD\\Confusion Matrix.txt", ios::out);
	ofstream confObserv("C:\\cbmLAD\\Confused Observations.txt", ios::out);

	ifstream readPatternMatrix("C:\\cbmLAD\\Pattern matrix.txt");
	ifstream readClassNames("C:\\cbmLAD\\class names.txt");
	ifstream readTestData("C:\\cbmLAD\\test data.txt");
	
	vector <vector<double>> PatternMatrix;
	vector <double> ClassNames;
	vector <vector<double>> TestData;
	

	if (readPatternMatrix)
	{
		string Line;

		while (getline(readPatternMatrix, Line))
		{
			PatternMatrix.push_back(vector<double>());

			// Break down the row into column values
			stringstream split(Line);
			double Value;

			while (split >> Value)
				PatternMatrix.back().push_back(Value);
		}

	}
	

	if (readClassNames)
	{
		string Line;

		while (getline(readClassNames, Line))
		{

			// Break down the row into column values
			stringstream split(Line);
			double Value;

			while (split >> Value)
				ClassNames.push_back(Value);
		}

	}
	
	vector<vector<int>> ConfusionMatrix(*max_element(ClassNames.begin(), ClassNames.end()), vector<int>(*max_element(ClassNames.begin(), ClassNames.end()), 0));
	
	if (readTestData)
	{
		string Line;

		while (getline(readTestData, Line))
		{
			TestData.push_back(vector<double>());

			// Break down the row into column values
			stringstream split(Line);
			double Value;

			while (split >> Value)
				TestData.back().push_back(Value);
		}

	}
	// check if there is duplication or contradicition in the raw data set

	vector <bool> IgnoredObservation(TestData.size());
	vector <int> ObservationInDuplication;
	vector <int> ObservationInContradiction;

	int NumberOfIgnoredObservation = 0;
	for (int i = 0; i < TestData.size() - 1; i++)
	{
		if (IgnoredObservation[i] != 1)
		{
			ObservationInContradiction.clear();
			ObservationInDuplication.clear();

			for (int j = 1; j < TestData.size(); j++)
			{
				if (j>i && IgnoredObservation[j] != 1)
				{

					if (equal(TestData[i].begin() + 1, TestData[i].end(), TestData[j].begin() + 1))
					{
						if (TestData[i][0] == TestData[j][0])
						{
							ObservationInDuplication.push_back(j);
							IgnoredObservation[j] = 1;
							NumberOfIgnoredObservation = NumberOfIgnoredObservation + 1;
						}
						if (TestData[i][0] != TestData[j][0])
						{
							ObservationInContradiction.push_back(j);
							IgnoredObservation[j] = 1;
							IgnoredObservation[i] = 1;
							NumberOfIgnoredObservation = NumberOfIgnoredObservation + 2;
						}
					}
				}
			}

			if (ObservationInDuplication.size() > 0)
			{
				showClassification << "The following observation/s will be ignored because there is a duplication with observation " << i + 1 << ";    "; // +1 is added because the index starts from 0
				for (int k = 0; k < ObservationInDuplication.size(); k++)
					showClassification << ObservationInDuplication[k] + 1 << "  ";
				showClassification << endl;
				showClassification << endl;
			}
			if (ObservationInContradiction.size() > 0)
			{
				showClassification << "There is a contradiction between the following observations, so they will be ignored;  " << i + 1 << "  ";
				for (int k = 0; k < ObservationInContradiction.size(); k++)
					showClassification << ObservationInContradiction[k] + 1 << "  ";
				showClassification << endl;
				showClassification << endl;
			}

		}

	}

	showClassification << endl;
	NumberOfIgnoredObservation = accumulate(IgnoredObservation.begin(), IgnoredObservation.end(), 0);
	showClassification << "Number Of ignored observation due to contradiction is " << NumberOfIgnoredObservation << endl;
	showClassification << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
	showClassification << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;

	// end of checking if there is duplication or contradicition in the raw data set

	vector <vector<double>> CoveredByClass; // Classes Scores
	vector <vector<double>> CoveredByPatterns; // The pattern ID (number)
	vector <vector<double>> CoveredByPatternsClass; // The class which the pattern is included in it
	vector <vector<double>> CoveredByPatternsVsClass;// The Versus Class

	for (int i = 0; i< TestData.size(); i++)
	{
		CoveredByClass.push_back(vector<double>());
		for (int j = 0; j < ClassNames.size(); j++)
		{
			CoveredByClass.at(i).push_back(0);
		}

	}


	//// hany
	// check if the patterns cover the tested observations

	for (int i = 0; i < TestData.size(); i++)
	{
		CoveredByPatternsClass.push_back(vector<double>());
		CoveredByPatternsVsClass.push_back(vector<double>());
		CoveredByPatterns.push_back(vector<double>());
		if (IgnoredObservation[i] != 1)

		{
			for (int h = 0; h < ClassNames.size(); h++)// To be edited, loop for each class inside looping for each class
			{
				for (int j = 0; j < PatternMatrix.size(); j++)
				{
					int Covered = 0;
					if (PatternMatrix[j][0] == ClassNames[h])
					{
						for (int k = 0; k< (PatternMatrix[j].size() - 4) / 3; k++) // each attribute takes 3 places in the pattern this explains the (/3). The first 4 places are resereved for the classes name, pattern number, and pattern weight ( this explains the (-4)
						{
							if (PatternMatrix[j][3 * k + 5] == 1) // 1 means greater than
							{
								if (TestData[i][PatternMatrix[j][3 * k + 4]] > PatternMatrix[j][3 * k + 6])  // keep in mind that test data includes the classification in the first column
									Covered = Covered + 1;

								if (TestData[i][PatternMatrix[j][3 * k + 4]] < PatternMatrix[j][3 * k + 6])
								{
									break;
								}
							}
							if (PatternMatrix[j][3 * k + 5] == -1) //-1 means less than
							{
								if (TestData[i][PatternMatrix[j][3 * k + 4]] < PatternMatrix[j][3 * k + 6])
									Covered = Covered + 1;
								if (TestData[i][PatternMatrix[j][3 * k + 4]] > PatternMatrix[j][3 * k + 6])
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
						CoveredByPatterns.at(i).push_back(PatternMatrix[j][2]);// To be edited
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
								if (TestData[i][PatternMatrix[j][3 * k + 4]] > PatternMatrix[j][3 * k + 6])  // keep in mind that test data includes the classification in the first column
									Covered = Covered + 1;

								if (TestData[i][PatternMatrix[j][3 * k + 4]] < PatternMatrix[j][3 * k + 6])
								{
									break;
								}
							}
							if (PatternMatrix[j][3 * k + 5] == -1) //-1 means less than
							{
								if (TestData[i][PatternMatrix[j][3 * k + 4]] < PatternMatrix[j][3 * k + 6])
									Covered = Covered + 1;
								if (TestData[i][PatternMatrix[j][3 * k + 4]] > PatternMatrix[j][3 * k + 6])
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
	}
	
	vector<double> ClassFound(TestData.size());

	for (unsigned int i = 0; i < CoveredByClass.size(); i++)
	{
		double maxClass = -100;
		for (unsigned int j = 0; j < CoveredByClass.at(i).size(); j++)
		{
			if (CoveredByClass[i][j] > maxClass && CoveredByClass[i][j]> 0)
			{

				maxClass = CoveredByClass[i][j];
				ClassFound[i] = ClassNames[j];

			}

		}
	}



	// end of check if the patterns cover the tested observations


	for (unsigned int i = 0; i < CoveredByClass.size(); i++)
	{
		if (IgnoredObservation[i] != 1)
		{
			showClassification << "[" << i + 1 << "]  is classified into class { " << ClassFound[i] << " }" << ", originally was class { " << TestData[i][0] << " }" << endl;
			
			showClassification << "Classes scores:   ";
			for (unsigned int j = 0; j < CoveredByClass.at(i).size(); j++)
			{
				showClassification << "[" << ClassNames[j] << "]"<< CoveredByClass.at(i).at(j) << "   ";
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

			
			// Osama: Write the confused observations in a new text file(Confused Observations)
			if (ClassFound[i] != TestData[i][0])
			{
				confObserv << "[" << i + 1 << "]    is classified into class { " << ClassFound[i] << " }" << ", originally was class { " << TestData[i][0] << " }" << endl;
				confObserv << "Classes scores:   ";
				for (unsigned int j = 0; j < CoveredByClass.at(i).size(); j++)
				{
					confObserv << "[" << ClassNames[j] << "]" << CoveredByClass.at(i).at(j) << "   ";
				}
				confObserv << endl;
				confObserv << "Covered by patterns:   ";
				for (unsigned int j = 0; j < CoveredByPatterns[i].size(); j++)
				{
					confObserv << "[" << CoveredByPatternsClass[i][j] << "V" << CoveredByPatternsVsClass[i][j] << "]";
					confObserv << CoveredByPatterns[i][j] << "   ";
				}
				confObserv << endl;
				confObserv << " --------------------------------------" << endl;

			}


			if (ClassNames[0] == 0)
			{
				if (ClassFound[i] != 0)
				{
					ConfusionMatrix[TestData[i][0]][ClassFound[i]] = ConfusionMatrix[TestData[i][0]][ClassFound[i]] + 1;

				}
				
			}
			else
			{
				if (ClassFound[i] != 0)
				{
					ConfusionMatrix[TestData[i][0] - 1][ClassFound[i] - 1] = ConfusionMatrix[TestData[i][0] - 1][ClassFound[i] - 1] + 1;
				}
				
			}
							
		}
	}
	double CorrectClassification = 0;
	for (unsigned int i = 0; i < ClassFound.size(); i++)
	{
		if (ClassFound[i] == TestData[i][0])
			CorrectClassification = CorrectClassification + 1;
	}

	for (int i = 0; i < ConfusionMatrix.size(); i++)
	{
		for (int j = 0; j < ConfusionMatrix.size(); j++)
		{
			confMatrix << ConfusionMatrix[i][j] << "	";
		}
		confMatrix << endl;
	}


	double Accuracy = 100 * (CorrectClassification) / (TestData.size() - NumberOfIgnoredObservation);

	showClassification << "Accuracy of testing these data is " << Accuracy << " %" << endl;
	showClassification.close();
}

