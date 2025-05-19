#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>   
#include <set>
#include <numeric>
#include <math.h>
#include <time.h>
#include "Binarize.h"
#include "Pattern_Interpretation.h"
#include "PatternGeneration.h"


using namespace std;
bool sortcol2(const vector<double>& v1, const vector<double>& v2) {
	return v1[1] < v2[1];
}

ofstream PatMatrixFile("C:\\cbmLAD\\Pattern matrix.txt", ios::out);

int main()
{
	Binarization o_Binarization;
	Pattern_Interpretation o_Pattern_Interpretation;
	PatternGeneration o_PatternGeneration;
	clock_t StartTime, EndTime;
	double ElapsedTime;

	StartTime = clock();

	vector<vector<string>> v_TrainingRawData_attribute_names;
	vector<vector<double>> v_TrainingRawData;
	vector<vector<double>> v_CutPoints;
	vector<vector<int>> v_BinaryTrainingData;
	vector<vector<int>> v_BinaryTrainingNoClass;
	vector <vector<vector<double>>> v_AttributeColumnVectors;


	ifstream raw_Data_File("C:\\cbmLAD\\raw data.txt");

	if (raw_Data_File)
	{
		string Line1;

		while (getline(raw_Data_File, Line1))
		{
			v_TrainingRawData_attribute_names.push_back(vector<string>());

			// Break down the row into column values
			stringstream split(Line1);
			string Value1;

			while (split >> Value1)
				v_TrainingRawData_attribute_names.back().push_back(Value1);
		}

	}


	//get the names of the attributes
	vector<string> v_attribute_names(v_TrainingRawData_attribute_names[0].size());

	for (int i = 0; i < v_TrainingRawData_attribute_names[0].size() - 1; i++)
	{
		v_attribute_names[i] = v_TrainingRawData_attribute_names[0][i + 1];

	}

	// delete the first row containing the attributes names 

	v_TrainingRawData_attribute_names.erase(v_TrainingRawData_attribute_names.begin() + 0);

	//

	// convering the raw data from string to double 

	for (int i = 0; i < v_TrainingRawData_attribute_names.size(); i++)
	{
		v_TrainingRawData.push_back(vector<double>());

		for (int j = 0; j < v_TrainingRawData_attribute_names[i].size(); j++)
		{
			v_TrainingRawData[i].push_back(atof(v_TrainingRawData_attribute_names[i][j].c_str()));

		}

	}
	//

	ofstream resultsFile("C:\\cbmLAD\\results.txt", ios::out);
	ofstream DuplandContradicted("C:\\cbmLAD\\Duplicated and Ignored Observations.txt", ios::out);
	ofstream PosPrevalence("C:\\cbmLAD\\Charcteristics of the generated patterns.txt", ios::out);
	PosPrevalence << "Class   " << " Pattern number    " << "Coverage    " << "Relative Prevalence  " << "  Weight  " << endl;
	PosPrevalence.close();

	bool MissingData = 0;
	int RowSize = v_TrainingRawData[0].size();

	for (int i = 0; i<v_TrainingRawData.size(); i++)
	{
		if (v_TrainingRawData[i].size() != RowSize)
		{
			resultsFile << "Row Number  " << i + 1 << " is missing values." << endl;
			MissingData = 1;
		}
	}


	// end of checking of the missing values


	if (MissingData == 1)
		resultsFile << "Update the data file and run the exe file again." << endl;

	if (MissingData == 0)

	{//# 0

	 //ofstream ignoredObservation ("C:\\cbmLAD\\Ignored Observations.txt", ios::out);

		resultsFile << " Number of attributes  " << v_TrainingRawData[0].size() - 1 << endl;
		resultsFile << " Number of observations is  " << v_TrainingRawData.size() - 1 << endl;


		resultsFile << "---------------------------------------------------------------------------------" << endl;

		// check if there is duplication or contradicition in the raw data set

		vector<vector<double>> v_TrainingRawDataSortedAccordingtoFirstAtt;

		vector <int> ObservationInDuplication;
		vector <int> ObservationInContradiction;

		v_TrainingRawDataSortedAccordingtoFirstAtt = v_TrainingRawData;
		v_TrainingRawDataSortedAccordingtoFirstAtt.erase(v_TrainingRawDataSortedAccordingtoFirstAtt.begin() + 0);

		vector <bool> IgnoredObservation(v_TrainingRawDataSortedAccordingtoFirstAtt.size());
		vector <bool> IgnoredObservationForSearching(v_TrainingRawDataSortedAccordingtoFirstAtt.size());
		vector <bool> DuplicatedObservation(v_TrainingRawDataSortedAccordingtoFirstAtt.size());
		bool contradictionexist;
		int NumberOfIgnoredObservation = 0;

		for (int i = 0; i < v_TrainingRawDataSortedAccordingtoFirstAtt.size(); i++)
		{
			v_TrainingRawDataSortedAccordingtoFirstAtt[i].push_back(i);
			//v_TrainingRawDataSortedAccordingtoFirstAtt[i].insert(v_TrainingRawDataSortedAccordingtoFirstAtt.begin(), i+1);
		}

		sort(v_TrainingRawDataSortedAccordingtoFirstAtt.begin(), v_TrainingRawDataSortedAccordingtoFirstAtt.end(), sortcol2);
		int p = v_TrainingRawDataSortedAccordingtoFirstAtt[0].size() - 1;

		for (int i = 0; i < v_TrainingRawDataSortedAccordingtoFirstAtt.size() - 1; i++)
		{
			//if (IgnoredObservationForSearching[i] != 1)
			//{
			ObservationInContradiction.clear();
			ObservationInDuplication.clear();
			contradictionexist = false;

			if (v_TrainingRawDataSortedAccordingtoFirstAtt[i][1] == v_TrainingRawDataSortedAccordingtoFirstAtt[i + 1][1])
			{
				int t = i + 1;
				while (v_TrainingRawDataSortedAccordingtoFirstAtt[i][1] == v_TrainingRawDataSortedAccordingtoFirstAtt[t][1])
				{
					if (equal(v_TrainingRawDataSortedAccordingtoFirstAtt[i].begin() + 1, v_TrainingRawDataSortedAccordingtoFirstAtt[i].end() - 1, v_TrainingRawDataSortedAccordingtoFirstAtt[t].begin() + 1))
					{
						if (v_TrainingRawDataSortedAccordingtoFirstAtt[i][0] == v_TrainingRawDataSortedAccordingtoFirstAtt[t][0] && DuplicatedObservation[t] != 1)
						{
							ObservationInDuplication.push_back(v_TrainingRawDataSortedAccordingtoFirstAtt[t][p]);
							DuplicatedObservation[t] = 1;
							//	IgnoredObservation[j] = 1;
							//	NumberOfIgnoredObservation =NumberOfIgnoredObservation +1;
						}
						else if (v_TrainingRawDataSortedAccordingtoFirstAtt[i][0] != v_TrainingRawDataSortedAccordingtoFirstAtt[t][0])
						{
							ObservationInContradiction.push_back(v_TrainingRawDataSortedAccordingtoFirstAtt[t][p]);
							IgnoredObservationForSearching[t] = 1;


							IgnoredObservation[v_TrainingRawDataSortedAccordingtoFirstAtt[t][p]] = 1;

							//NumberOfIgnoredObservation = NumberOfIgnoredObservation + 1;
							contradictionexist = true;

						}
					}
					t = t + 1;
					if (t > v_TrainingRawDataSortedAccordingtoFirstAtt.size() - 1)
					{
						break;
					}
				}
				if (contradictionexist)
				{
					//NumberOfIgnoredObservation = NumberOfIgnoredObservation + 1;
					IgnoredObservationForSearching[i] = 1;
					IgnoredObservation[v_TrainingRawDataSortedAccordingtoFirstAtt[i][p]] = 1;
				}
				if (ObservationInDuplication.size() > 0)
				{

					DuplandContradicted << "The following observations are duplicated; " << v_TrainingRawDataSortedAccordingtoFirstAtt[i][p]+1 << "  ";
					for (int k = 0; k < ObservationInDuplication.size(); k++)
						DuplandContradicted << ObservationInDuplication[k]+1 << "  ";
					DuplandContradicted << '\n' << '\n';
					//DuplandContradicted << endl;
				}

				if (ObservationInContradiction.size() > 0)
				{
					DuplandContradicted << "There is a contradiction between the following observations, so they will be ignored;  " << v_TrainingRawDataSortedAccordingtoFirstAtt[i][p] +1<< "  ";
					for (int k = 0; k < ObservationInContradiction.size(); k++)
						DuplandContradicted << ObservationInContradiction[k]+1 << "  ";
					DuplandContradicted << '\n' << '\n';
				}

			}

			//}
		}


		resultsFile << endl;
		NumberOfIgnoredObservation = accumulate(IgnoredObservation.begin(), IgnoredObservation.end(), 0);
		resultsFile << "Number Of ignored observation due to contradiction is " << NumberOfIgnoredObservation << '\n' << '\n';
		resultsFile << "---------------------------------------------------------------------------------" << endl;

		v_TrainingRawDataSortedAccordingtoFirstAtt.clear();

		resultsFile.close();

		// end of checking if there is duplication or contradicition in the raw data set


		// Get Number of Classes and their names
		vector<int> vClassNames;

		for (unsigned int n = 1; n<v_TrainingRawData.size(); n++) // n starts form 1 because the first row in the raw data representes the data types
		{
			if (n == 1)
			{
				vClassNames.push_back(v_TrainingRawData[n][0]);
			}
			else
			{
				if (v_TrainingRawData[n][0] != v_TrainingRawData[n - 1][0])
				{
					bool ISNEWCLASS = 1;
					for (unsigned int p = 0; p<vClassNames.size(); p++)
					{
						if (vClassNames.at(p) == v_TrainingRawData[n][0])
						{
							ISNEWCLASS = 0;
							break;
						}
					}
					if (ISNEWCLASS == 1)
					{
						vClassNames.push_back(v_TrainingRawData[n][0]);
					}
				}
			}
		}

		ofstream classNames("C:\\cbmLAD\\class names.txt", ios::out);

		for (unsigned int j = 0; j<vClassNames.size(); j++)
		{
			classNames << vClassNames[j] << "  ";
		}

		// End of Get Number of Classes and their names

	

		/////// start of solving the problem for each class

		for (int a = 0; a < vClassNames.size(); a++)
		{
									
			vector<vector<double>> copy_of_TrainingRawData = v_TrainingRawData;

			for (unsigned int i = 1; i < copy_of_TrainingRawData.size(); i++)
			{
				if (copy_of_TrainingRawData[i][0] == vClassNames[a])
					copy_of_TrainingRawData[i][0] = 1; // make it the positive class ( the class under study for generating patterns)
				else
					copy_of_TrainingRawData.at(i).at(0) = 0; // make it the negative class 
			}

		
			int NumberOfPositiveObservation = 0;
			int NumberOfNegativeObservation = 0;
			
			int NegPatNumber = 0;


			for (int i = 1; i < copy_of_TrainingRawData.size(); i++)
			{

				if (copy_of_TrainingRawData[i][0] == 1) NumberOfPositiveObservation = NumberOfPositiveObservation + 1;
				if (copy_of_TrainingRawData[i][0] == 0) NumberOfNegativeObservation = NumberOfNegativeObservation + 1;
			}

			vector<bool> CoverdObservation(IgnoredObservation.size());

			for (int i = 0; i < IgnoredObservation.size(); i++)
			{
				if (IgnoredObservation[i] == 1) CoverdObservation.at(i) = 1;
				if (IgnoredObservation[i] == 0) CoverdObservation.at(i) = 0;
			}
			resultsFile.open("C:\\cbmLAD\\results.txt", ios::app);

			resultsFile << "*****************************************" << endl;
			resultsFile << "   Generating patterns for class " << vClassNames[a] << endl;
			resultsFile << "*****************************************" << endl;

			resultsFile << " Number of observations in this class is  " << NumberOfPositiveObservation << endl;
			resultsFile << " Number of observations in other classes  " << NumberOfNegativeObservation << endl;

			resultsFile << "Binarization is in progress...  " << endl;
			

			v_AttributeColumnVectors = o_Binarization.GenerateAttributeColumnVectors(copy_of_TrainingRawData);

			vector<vector<vector<double>>> v_SortedAttributeColumnVectors;
			v_SortedAttributeColumnVectors = o_Binarization.SortAttributeValues(v_AttributeColumnVectors);

			v_CutPoints = o_Binarization.ExtractCutPoints(v_SortedAttributeColumnVectors, vClassNames[a]);

			v_BinaryTrainingData = o_Binarization.GetBinaryTraining(v_CutPoints, copy_of_TrainingRawData);

			
			vector<vector<int> > BinaryData;
			
			for (int i = 0; i < v_BinaryTrainingData.size(); i++)
			{
				BinaryData.push_back(vector<int>());
				for (int j = 0; j < v_BinaryTrainingData[i].size(); j++)
				{
					BinaryData[i].push_back(v_BinaryTrainingData[i][j]);
				}
			}


			resultsFile << " Binarization is accomplished, number of binary attributes is  " << BinaryData[0].size() - 1 << endl;
			resultsFile << "--------------------------------" << endl;
			
		
			resultsFile.close();
		
			vector<vector<int>> PositivePatterns;
			PositivePatterns = o_PatternGeneration.GeneratePattern (BinaryData, IgnoredObservation, vClassNames[a], NumberOfPositiveObservation, CoverdObservation, a); // returns the set of generated patterns
						
					
			resultsFile << endl;

					
			vector<vector<vector<double>>> vInterpretedPatterns = o_Pattern_Interpretation.Interpret2ClassPatterns(v_attribute_names, PositivePatterns, v_CutPoints, vClassNames[a]);
			
			vector <double> PatternWeight;

			PatternWeight = o_PatternGeneration.PosPatternWeight[a];

			for (int i = 0; i< vInterpretedPatterns.size(); i++)
			{
				PatMatrixFile << vClassNames[a] << "   " << i + 1 << "   " << PatternWeight[i] << "  ";
				for (int j = 0; j< vInterpretedPatterns[i].size(); j++)
				{
					PatMatrixFile << vInterpretedPatterns.at(i).at(j).at(0) << "  " << vInterpretedPatterns.at(i).at(j).at(2) << "  " << vInterpretedPatterns.at(i).at(j).at(3) << "  ";
				}
				PatMatrixFile << endl;
			}

			
		}    /////// end of solving the problem for each class

		EndTime = clock();
		ElapsedTime = ((float)(EndTime - StartTime)) / CLOCKS_PER_SEC;

		resultsFile.open("C:\\cbmLAD\\results.txt", ios::app);
		resultsFile << endl;
		resultsFile << "Elapsed time is " << ElapsedTime / 60 << " min." << endl;

	} // #0
}
