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

using namespace std;

bool sortcol(const vector<double>& v1, const vector<double>& v2) {
	return v1[0] < v2[0];
}
bool sortcol2(const vector<double>& v1, const vector<double>& v2) {
	return v1[1] < v2[1];
}
vector<vector<vector<double>>> GenerateAttributeColumnVectors(vector<vector<double>>& two_class_Training);
vector<vector<vector<double>>> SortAttributeValues(vector<vector<vector<double>>>& vAllAttributeColumns);

vector<vector<double>> ExtractCutPoints(vector<vector<vector<double>>>& vAllAttributes, int ClassName, int VsClassName);
vector<vector<int>> GetBinaryTraining(vector<vector<double>>& vCutPoints, vector<vector<double>>& two_class_Training);
vector<vector<int>> Reduce2DBinaryTrainingVector(vector<vector<int>> vOriginalVector, vector<unsigned int> vRetainedAttributes);
vector < vector <double> > cleanData(vector < vector <double> >& inputVec);

void FindDifferencesSameClass(vector<int>& ObservationAlpha_1, vector<vector<int>  >& BinaryData_1, vector<bool>& CoverdObservation_1, vector<bool> &IgnoredObservation_1, vector<vector<int>  > &DifferencesInSameClass_1);
void FindDifferencesOppositeClass(vector<int>& ObservationAlpha_1, vector<vector<int>  >& BinaryData_1, vector<bool>& CoverdObservation_1, vector<bool> &IgnoredObservation_1, vector<vector<int>  > &DifferencesInOppositeClass_1);

vector<vector<vector<double>>>  Interpret2ClassPatterns(vector<string> attribute_Names, vector<vector<int>> vPatterns, vector<vector<double>> vCutPoints, int ClassName, int VsClassName);




ofstream PatternCoverageFile("C:\\cbmLAD\\Coverage of Multi-Class Patterns.txt", ios::out);
ofstream PosPrevalence("C:\\cbmLAD\\Charcteristics of the generated patterns.txt", ios::out);
ofstream Patternfile("C:\\cbmLAD\\Patterns Interpreted.txt", ios::out);
ofstream PatMatrixFile("C:\\cbmLAD\\Pattern matrix.txt", ios::out);
ofstream CutPoints("C:\\cbmLAD\\Cut points.txt", ios::out);
ofstream DuplandContradicted("C:\\cbmLAD\\Duplicated and Ignored Observations.txt", ios::out);

int main()
{

	clock_t StartTime, EndTime, StartPtrGen, EndPtrGen;
	double ElapsedTime, PtrGenElapsedTime;

	StartTime = clock();

	vector<vector<string>> v_TrainingRawData_attribute_names;
	vector<vector<double>> v_TrainingRawData;
	vector<vector<double>> v_CutPoints;
	vector<vector<int>> BinaryData;
	vector<vector<int>> v_BinaryTrainingNoClass;
	vector <vector<vector<double>>> v_AttributeColumnVectors;
	vector<int> v_NumberOfObservationsPerClass;



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
	{
		resultsFile << "Udate the data file and run the exe file again!!" << endl;
		return 0;
	}
		

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
							DuplandContradicted << ObservationInDuplication[k] +1<< "  ";
						DuplandContradicted << '\n' << '\n';
						//DuplandContradicted << endl;
					}

					if (ObservationInContradiction.size() > 0)
					{
						DuplandContradicted << "There is a contradiction between the following observations, so they will be ignored;  " << v_TrainingRawDataSortedAccordingtoFirstAtt[i][p]+1 << "  ";
						for (int k = 0; k < ObservationInContradiction.size(); k++)
							DuplandContradicted << ObservationInContradiction[k]+1<< "  ";
						DuplandContradicted << '\n' << '\n';
					}

				}

			//}
		}
		

		resultsFile << endl;
		NumberOfIgnoredObservation = accumulate(IgnoredObservation.begin(), IgnoredObservation.end(), 0);
		resultsFile << "Number Of ignored observation due to contradiction is " << NumberOfIgnoredObservation << '\n'<<'\n';
		resultsFile << "---------------------------------------------------------------------------------" << endl;

		v_TrainingRawDataSortedAccordingtoFirstAtt.clear();

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

		PosPrevalence << "Class  , " << " Pattern number  ,  " << "Coverage  ,  " << "Relative Prevalence  ," << "  Weight  " << endl;

		//////////////////////////////////////////////////////////////////////// start of solving the problem for each class

		for (int a = 0; a < vClassNames.size(); a++)
		{
			//resultsFile << vClassNames[a] << endl;


			//vector<vector<double>> copy_of_TrainingRawData = v_TrainingRawData;




			for (int aa = 0; aa < vClassNames.size(); aa++)
			{
				if (aa != a)

				{
					StartPtrGen = clock();
					vector<int> rank_in_TrainingRawData; // to rank the observations of the one vs. one dataset according to the multi-class dataset
					
					resultsFile << "*****************************************" << endl;
					resultsFile << "   Generating patterns for class " << vClassNames[a] << "  VS. class  " << vClassNames[aa] << endl;
					resultsFile << "*****************************************" << endl;
					
					vector<vector<double>> two_class_TrainingRawData;  // define a matrix for the two class  (one vs. one)
					vector<bool>two_class_IgnoredObservations;

					int countera = 0;
					two_class_TrainingRawData.push_back(vector<double>());
					for (int j = 0; j < v_TrainingRawData[0].size(); j++)
						two_class_TrainingRawData[countera].push_back(v_TrainingRawData[0][j]);

					countera = countera + 1;

					for (unsigned int i = 1; i < v_TrainingRawData.size(); i++)
					{
						if (v_TrainingRawData[i][0] == vClassNames[a])
						{

							//copy_of_TrainingRawData[i][0] = 1; // make it the positive class ( the class under study for generating patterns)

							two_class_TrainingRawData.push_back(vector<double>());

							for (int j = 0; j < v_TrainingRawData[i].size(); j++)
							{
								two_class_TrainingRawData[countera].push_back(v_TrainingRawData[i][j]);

							}
							two_class_IgnoredObservations.push_back(IgnoredObservation[i-1]);
							two_class_TrainingRawData[countera][0] = 1;
							rank_in_TrainingRawData.push_back(i);
							//copy_of_TrainingRawData[i][0] = vClassNames[a];

							countera = countera + 1;
						}



						else if (v_TrainingRawData[i][0] == vClassNames[aa])
						{

							//v_TrainingRawData[i][0] = 0; // make it the negative class 

							two_class_TrainingRawData.push_back(vector<double>());

							for (int j = 0; j < v_TrainingRawData[i].size(); j++)
							{
								two_class_TrainingRawData[countera].push_back(v_TrainingRawData[i][j]);

							}
							two_class_IgnoredObservations.push_back(IgnoredObservation[i - 1]);
							two_class_TrainingRawData[countera][0] = 0;
							rank_in_TrainingRawData.push_back(i);
							//copy_of_TrainingRawData[i][0] = vClassNames[aa];
							
							countera = countera + 1;
						}

					}

					//	   for (unsigned int i = 0; i < rank_in_TrainingRawData.size(); i++)
					//	   resultsFile << rank_in_TrainingRawData[i] << endl;

					vector<vector<int>> v_PositivePatterns;
					vector<vector<int>> PosPatternCoverage;
					vector<vector<int>> PosPatternCoverage_rankInMultiClass;
					double TotalCoveragePos = 0;
					vector<double> PosPatternWeight;
					vector<double> CoveragePos;

					//		for (int i = 0; i < two_class_TrainingRawData.size(); i++)
					//	{
					//		for (int j = 0; j < two_class_TrainingRawData[i].size(); j++)
					//		{
					//			resultsFile << two_class_TrainingRawData[i][j] << "  ";

					//		}
					//		resultsFile << endl;
					//	}



					int NumberOfPositiveObservation = 0;
					int NumberOfNegativeObservation = 0;
					int PosPatNumber = 0;
					int NegPatNumber = 0;

					
					for (int i = 1; i < two_class_TrainingRawData.size(); i++) // bug found in 30/4/2018 (i starts in 1 to ignore the first row in raw data file)
					{

						if (two_class_TrainingRawData[i][0] == 1) NumberOfPositiveObservation = NumberOfPositiveObservation + 1;
						if (two_class_TrainingRawData[i][0] == 0) NumberOfNegativeObservation = NumberOfNegativeObservation + 1;
					}

					vector<bool> CoverdObservation(two_class_TrainingRawData.size()-1);
					fill(CoverdObservation.begin(), CoverdObservation.end(), 0);
					//for (int i = 0; i < two_class_TrainingRawData.size(); i++)
						//CoverdObservation.at(i) = 0;

					//  resultsFile << " hi"   << endl;	

					resultsFile << " Number of observations in class " << vClassNames[a] << " is  " << NumberOfPositiveObservation << endl;
					resultsFile << " Number of observations in class " << vClassNames[aa] << " is  " << NumberOfNegativeObservation << endl;

					//ofstream binaryFile2("C:\\cbmLAD\\binary data2.txt", ios::out);

					//for (unsigned int i = 0; i < two_class_TrainingRawData.size(); i++)
					//{
					//	for (unsigned int j = 0; j < two_class_TrainingRawData.at(i).size(); j++)
					//	{
					//		binaryFile2 << two_class_TrainingRawData.at(i).at(j) << "   ";
					//	}
					//	binaryFile2 << endl;
					//}
					//binaryFile2.close();


					resultsFile << "Binarization is in progress...  " << endl;

					v_AttributeColumnVectors = GenerateAttributeColumnVectors(two_class_TrainingRawData);

					

					
					vector<vector<vector<double>>> v_SortedAttributeColumnVectors; 
					
					

					v_SortedAttributeColumnVectors = SortAttributeValues(v_AttributeColumnVectors);

					

					v_CutPoints = ExtractCutPoints(v_SortedAttributeColumnVectors, vClassNames[a], vClassNames[aa]);

					BinaryData = GetBinaryTraining(v_CutPoints, two_class_TrainingRawData);


					//ofstream binaryFile("C:\\cbmLAD\\binary data.txt", ios::out);

					//for (unsigned int i = 0; i < BinaryData.size(); i++)
					//{
					//	for (unsigned int j = 0; j < BinaryData.at(i).size(); j++)
					//	{
					//		binaryFile << BinaryData.at(i).at(j) << " ";
					//	}
					//	binaryFile << '\n';
					//}
					//binaryFile.close();

					//vector<vector<int> > BinaryData;


					


					resultsFile << " Binarization is accomplished, number of binary attributes is  " << BinaryData[0].size() - 1 << endl;
					resultsFile << "--------------------------------" << endl;

					////////////////////////////////////////////////////////////////////generating patterns using ACO




					// start of solving the problem

					while (accumulate(CoverdObservation.begin(), CoverdObservation.end(), 0) < (NumberOfPositiveObservation))
					{  //1 stops when all the observations are covered

						int MyCount;
						resultsFile << '\n';

						vector<int> ObservationAlpha(BinaryData[1].size());

						int AlphaIsFound = 0;

						for (int i = 0; i < BinaryData.size(); i++)
						{
							//	 resultsFile << " observation i; " << i+1<< endl;
							if (BinaryData[i][0] == 1 && CoverdObservation[i] == 0 && two_class_IgnoredObservations[i] != 1)
							{
								//	resultsFile << "Observation number  " << i + 1 << "  is the Alpha observation " << endl;
								AlphaIsFound = 1;
								for (int j = 0; j < BinaryData[i].size(); j++)
								{

									ObservationAlpha.at(j) = BinaryData[i].at(j);
									//	resultsFile << ObservationAlpha.at(j) << "  ";
								}
								CoverdObservation[i] = 1;
								break;
							}

						}

						if (AlphaIsFound == 0)
							break;

						vector<bool> DeletedColumns(BinaryData[1].size() - 1);
						vector<vector<int>  > DifferencesInSameClass;
						FindDifferencesSameClass(ObservationAlpha, BinaryData, CoverdObservation, two_class_IgnoredObservations, DifferencesInSameClass);
						//   resultsFile << "DifferencesInSameClass " << endl;
						//for (int i = 0; i < BinaryData[1].size() - 1; i++)

						//{
							//	   resultsFile << "attribute number " << i << endl;
						//	for (int j = 0; j < DifferencesInSameClass[i].size(); j++)
							//{
								//			  resultsFile << DifferencesInSameClass[i][j] << "  ";
							//}
							//	 resultsFile << '\n';
						//}
						//	 resultsFile << " end of finding DifferencesInSameClass"<<'\n';

						vector<vector<int>  > DifferencesInOppositeClass;
						
						


						FindDifferencesOppositeClass(ObservationAlpha, BinaryData, CoverdObservation, two_class_IgnoredObservations, DifferencesInOppositeClass);

						//	  resultsFile << "DifferencesInOppositeClass " << endl;
						for (int i = 0; i < BinaryData[1].size() - 1; i++)
						{
							//		  resultsFile << "attribute number " << i << endl;
							//for (int j = 0; j < DifferencesInOppositeClass[i].size(); j++)
							//{
								//		  resultsFile << DifferencesInOppositeClass[i][j] << "  ";
							//}
							if (DifferencesInOppositeClass[i].size() == 0)  // deletes redundant columns that do not cover any row in SCP
							{
								DeletedColumns[i] = 1;
								//    		resultsFile << "Binary attribute number  " << i+1 << "  is deleted (redundant)"<< endl;
							}
							//	 		resultsFile << '\n';
						}
						//	resultsFile << '\n';

						//	  resultsFile << " end of finding DifferencesInOppositeClass"<<'\n';

						// start of steps that delete a column which covers a set of obnservations that can be covered by another column at lower cost


						bool Answer;


						for (int i = 0; i < BinaryData[1].size() - 2; i++)
						{
							if (DeletedColumns.at(i) == 0)
							{
								for (int j = 0; j < BinaryData[1].size() - 1; j++)
								{
									if (j != i)
									{

										if (DeletedColumns.at(j) == 0)
										{
											if (DifferencesInOppositeClass[i].size() <= DifferencesInOppositeClass[j].size())
											{

												Answer = includes(DifferencesInOppositeClass[j].begin(), DifferencesInOppositeClass[j].end(), DifferencesInOppositeClass[i].begin(), DifferencesInOppositeClass[i].end());
												//resultsFile << " Answer is " << Answer << endl;

												if ((Answer == 1) && (DifferencesInSameClass[i].size() >= DifferencesInSameClass[j].size()))
												{
													DeletedColumns[i] = 1;
													//						resultsFile << i  << " is included in " << j <<endl;
													//						resultsFile << "Binary attribute number  " << i <<"   is deleted  "<<endl;
												}
											}
											if (DeletedColumns[i] == 1) break;

										}

									}

								}
							}
						}

						// resultsFile << " Deletion in first step is done" << endl;


						// end of steps that delete a column which covers a set of obnservations that can be covered by another column at lower cost	

						//resultsFile << endl;
						//resultsFile << endl;



						///////// start of steps that find the columns covering given observations 

						map <int, vector<int> > CoverdByColumns; 	//shows the rows covered (constraints satisfied)  by coulmns in the SCP
						vector<bool> FoundBefore(BinaryData.size());

						for (int i = 0; i < BinaryData[1].size() - 1; i++)
						{
							for (int j = 0; j < DifferencesInOppositeClass[i].size(); j++)
							{
								if (FoundBefore.at(DifferencesInOppositeClass[i][j]) != 1)
								{
									//  resultsFile << " row  " << DifferencesInOppositeClass[i][j] << " covered by   ";	  

									for (int k = 0; k < BinaryData[1].size() - 1; k++)
									{
										if (DeletedColumns[k] != 1)
										{
											if (find(DifferencesInOppositeClass[k].begin(), DifferencesInOppositeClass[k].end(), DifferencesInOppositeClass[i][j]) != DifferencesInOppositeClass[k].end())
											{
												CoverdByColumns[DifferencesInOppositeClass[i][j]].push_back(k);
												// resultsFile << k << "  ";				  
											}
										}
									}
									//	  resultsFile << '\n';
									FoundBefore.at(DifferencesInOppositeClass[i][j]) = 1;
								}
							}
							//resultsFile << '\n';
						}

						//// end of steps that find the columns covering given observations 

						//	resultsFile  << " CoverdByColumns matrix " << endl;
						// for(map <int, vector<int> >::const_iterator it = CoverdByColumns.begin();it != CoverdByColumns.end(); it++)
						{
							// resultsFile << it->first << "   ";
							//	for (int j = 0; j < CoverdByColumns[it->first].size(); j++) 
							{
								//    resultsFile << " " << CoverdByColumns[it->first][j];
							}
							//resultsFile<< endl;
						}
						//resultsFile<< '\n';
						// resultsFile<< '\n';

						int EndLoopIndexM;
						int EndLoopIndexJ;

						//////////// Start of finding the list of best attributes
						vector<int> BestAttributes(BinaryData[1].size() - 1);
						vector<int> ListedAttributes(BinaryData[1].size() - 1);
						int MinimumCost = 1000000;
						int StoredJ;
						for (int i = 0; i < BinaryData[1].size() - 1; i++)
						{
							ListedAttributes.at(i) = 0;
						}

						for (int i = 0; i < BinaryData[1].size() - 1; i++)
						{
							for (int j = 0; j < BinaryData[1].size() - 1; j++)
							{
								if (ListedAttributes.at(j) == 0)
								{
									//resultsFile<< j << "  " << " size  " << DifferencesInSameClass[j].size() << "  " << endl;
									if (DifferencesInSameClass[j].size() < MinimumCost)
									{
										BestAttributes[i] = j;
										StoredJ = j;
										MinimumCost = DifferencesInSameClass[j].size();

									}

									if (DifferencesInSameClass[j].size() == MinimumCost)
									{

										if (DifferencesInOppositeClass[j].size() > DifferencesInOppositeClass[StoredJ].size())
										{
											BestAttributes[i] = j;
											//resultsFile<< " BestAttributes[i]  " << BestAttributes[i] <<endl;
										}
										if (DifferencesInOppositeClass[j].size() <= DifferencesInOppositeClass[StoredJ].size())
										{
											BestAttributes[i] = StoredJ;
											//	resultsFile<< " BestAttributes[i]  " << BestAttributes[i] <<endl;
										}

									}
								}

							}
							ListedAttributes.at(BestAttributes[i]) = 1;
							//  resultsFile << i << "   " << BestAttributes[i] << endl;
							MinimumCost = 1000000;
						}
						//////////// end of finding the list of best attributes





						//	 resultsFile << " Number of deleted columns is " << accumulate(DeletedColumns.begin(),DeletedColumns.end(),0) <<endl;

						// resultsFile<< "Deleted Columns" << '\n';
						//for (int j = 0; j < BinaryData[1].size()-1; j++) 
						//resultsFile<< j << "  " << DeletedColumns[j] << '\n';
						//	resultsFile<< '\n';

						//// start of steps that finds columns inclusion
						vector<int> MandatoryColumns;
						vector<int> CoveredRow;
						map <int, vector<int> > CoverdByColumnsAfterDelete;
						for (map <int, vector<int> >::const_iterator it = CoverdByColumns.begin(); it != CoverdByColumns.end(); it++)
						{
							for (int j = 0; j < CoverdByColumns[it->first].size(); j++)
							{
								if (DeletedColumns[CoverdByColumns[it->first][j]] == 0)
								{
									CoverdByColumnsAfterDelete[it->first].push_back(CoverdByColumns[it->first][j]);
								}
							}
							// resultsFile<< endl;
							if (CoverdByColumnsAfterDelete[it->first].size() == 1)
							{
								MyCount = count(MandatoryColumns.begin(), MandatoryColumns.end(), CoverdByColumnsAfterDelete[it->first][0]);
								// resultsFile  << " MyCount " << MyCount<< ",  ";
								if (MyCount == 0)
								{
									MandatoryColumns.push_back(CoverdByColumnsAfterDelete[it->first][0]);
									//	 			 resultsFile<< "Column number "<< CoverdByColumnsAfterDelete[it->first][0] << " must be in the optimal solution to cover row number  " << it->first <<endl;
									// CoveredRow.push_back(it->first);
									// resultsFile<< "row number "<< it->first << " is deleted from the data  " << endl;
								}
								MyCount = 1;
							}
						}

						//resultsFile << " end of steps that finds columns inclusion" << endl;
						//// end of steps that finds columns inclusion


						//resultsFile  << " CoverdByColumns matrix " << endl;
						//for(map <int, vector<int> >::const_iterator it = CoverdByColumns.begin();it != CoverdByColumns.end(); it++)
						//{
						//   resultsFile << it->first << "   ";
						//for (int j = 0; j < CoverdByColumns[it->first].size(); j++) 
						//	{
						//   resultsFile << " " << CoverdByColumns[it->first][j];
						// }
						//  resultsFile<< endl;
						//}
						//	resultsFile<< '\n';
						//	 resultsFile<< '\n';


						// resultsFile  << " CoverdByColumnsAfterDelete matrix " << endl;
						// for(map <int, vector<int> >::const_iterator it = CoverdByColumnsAfterDelete.begin();it != CoverdByColumnsAfterDelete.end(); it++)
						{
							// resultsFile << it->first << "   ";
							//for (int j = 0; j < CoverdByColumnsAfterDelete[it->first].size(); j++) 
							{
								//    resultsFile << " " << CoverdByColumnsAfterDelete[it->first][j];
							}
							// resultsFile<< endl;
						}
						//	resultsFile<< '\n';



						//steps that check if the manadatory columns cover all rows, and store the uncovered rows
						int MySum = 0;
						vector<int> UnCoveredRow;

						for (map <int, vector<int> >::const_iterator it = CoverdByColumnsAfterDelete.begin(); it != CoverdByColumnsAfterDelete.end(); it++)
						{
							for (int i = 0; i < CoverdByColumnsAfterDelete[it->first].size(); i++)
							{
								EndLoopIndexJ = 0;
								for (int j = 0; j< MandatoryColumns.size(); j++)
								{
									// resultsFile << "it->first = "<<it->first << endl;

									MyCount = count(CoveredRow.begin(), CoveredRow.end(), it->first);

									if (CoverdByColumnsAfterDelete[it->first][i] == MandatoryColumns[j] && MyCount == 0) // to see if the row is covered by one of the manadatory columns
									{
										CoveredRow.push_back(it->first);
										//MySum = MySum+1; // counts the number of rows covered by the manadatory columns
										EndLoopIndexJ = 1;
										break;
									}
									if (EndLoopIndexJ == 1) break;
								}
							}

							MyCount = count(CoveredRow.begin(), CoveredRow.end(), it->first);
							if (MyCount == 0)
								UnCoveredRow.push_back(it->first);
						}
						//resultsFile << "MySum = "<<MySum << endl;

						// resultsFile << "end of steps that check if the manadatory columns cover all rows , and store the uncovered rows " <<endl;


						//end of steps that check if the manadatory columns cover all rows , and store the uncovered rows

						//resultsFile << " Covered rows are:  ";

						//for (int i = 0; i < CoveredRow.size(); i++)
						//{
							//resultsFile << CoveredRow[i] << "  ";
						//}

						//resultsFile << endl;

						//// Check if deletion and inclusion of columns results in a complete soltuion

						int SumOfElementsInMandatoryColumns;
						//SumOfElementsInMandatoryColumns = accumulate(MandatoryColumns.begin(),MandatoryColumns.end(),0);
						//resultsFile<< "number of mandatory columns  " <<MandatoryColumns.size() << '\n';
						//resultsFile<< '\n';

						//	resultsFile<< "Mandatory Columns:  " ;
						//for (int j = 0; j < MandatoryColumns.size(); j++) 
						//resultsFile<< MandatoryColumns[j] << "  ";
						//resultsFile<< '\n';

						int SumOfElementsInDeletedColumns;
						SumOfElementsInDeletedColumns = accumulate(DeletedColumns.begin(), DeletedColumns.end(), 0);
						//	resultsFile<<  "number of deleted columns  " << SumOfElementsInDeletedColumns << '\n';

						//if(MandatoryColumns.size() + SumOfElementsInDeletedColumns == BinaryData[1].size()-1)
						//{
						//	resultsFile << "Deletion and inclusion of columns results in a complete soltuion" << endl;
						//}
						// resultsFile  <<  endl;


						/// end of checking

						// show results of initialization stage


						//	resultsFile << CoveredRow.size() << "  " <<  CoverdByColumns.size()<< "  "<< accumulate(CoverdObservation.begin(), CoverdObservation.end(),0) << "  "<< BinaryData.size() - NumberOftwo_class_IgnoredObservations << "  "<< MandatoryColumns.size()  << endl;



						//if ((CoveredRow.size() == CoverdByColumns.size()) || (accumulate(CoverdObservation.begin(), CoverdObservation.end(),0) == BinaryData.size() - NumberOfIgnoredObservation && MandatoryColumns.size() > 0 )) 



						if (UnCoveredRow.size() == 0)
						{ // 2 this will show the feasible results of the initilization, no need for ACO to solve this Alpha problem, go to the next alpha problem

						  	//resultsFile << "No need for applying the algorithm for this Alpha-problem" << endl;

						  // steps that find the observations having same class as Alpha observation and uncovered by the generated Alpha pattern

							vector<int> SameClassUnCoveredObservations;
							vector<int> SameClassCoveredObservations;
							for (int i = 0; i < MandatoryColumns.size(); i++)
							{
								for (int j = 0; j < DifferencesInSameClass[MandatoryColumns[i]].size(); j++)
								{
									MyCount = count(SameClassUnCoveredObservations.begin(), SameClassUnCoveredObservations.end(), DifferencesInSameClass[MandatoryColumns[i]][j]);
									if (MyCount == 0)
									{
										SameClassUnCoveredObservations.push_back(DifferencesInSameClass[MandatoryColumns[i]][j]);
									}
									MyCount = 1;
								}
							}

							//resultsFile << " SameClassUnCoveredObservations" << endl;
							//for (int i=0; i < SameClassUnCoveredObservations.size(); i++) 
							//{
							//resultsFile << SameClassUnCoveredObservations[i] << "   ";
							//}

							//resultsFile <<endl;

							// end of steps that find the observations having same class as Alpha observation and uncovered by the generated Alpha pattern

							// start of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern
							for (int i = 0; i < BinaryData.size(); i++)
							{
								if (BinaryData[i][0] == ObservationAlpha[0] && two_class_IgnoredObservations[i] != 1)
								{
									MyCount = count(SameClassUnCoveredObservations.begin(), SameClassUnCoveredObservations.end(), i);
									if (MyCount == 0)
									{
										SameClassCoveredObservations.push_back(i);
										CoverdObservation[i] = 1;
									}
									MyCount = 1;
								}
							}

							// writting the pattern
							if (ObservationAlpha[0] == 1)
							{
								v_PositivePatterns.push_back(vector <int>());
								PosPatternCoverage.push_back(vector<int>());
								PosPatternCoverage_rankInMultiClass.push_back(vector<int>());
								for (int i = 0; i < BinaryData[0].size() - 1; i++)
								{
									v_PositivePatterns[PosPatNumber].push_back(-100);
								}

								resultsFile << "Pattern Number  " << PosPatNumber + 1 << " has;" << endl;
								CoveragePos.push_back(SameClassCoveredObservations.size());
								//TotalCoveragePos = TotalCoveragePos +  SameClassCoveredObservations.size();

							}

							for (int i = 0; i< MandatoryColumns.size(); i++)
							{
								resultsFile << " Binary attribute number " << MandatoryColumns[i] + 1 << " equals " << ObservationAlpha[MandatoryColumns[i] + 1] << endl;

								if (ObservationAlpha[0] == 1)
									v_PositivePatterns[PosPatNumber][MandatoryColumns[i]] = ObservationAlpha[MandatoryColumns[i] + 1];

							}


							resultsFile << endl;

							// end writting



							resultsFile << " Coverage of this pattern is  " << SameClassCoveredObservations.size() << endl;


							resultsFile << " Observations covered by this pattern are:  ";


							for (int i = 0; i < SameClassCoveredObservations.size(); i++)
							{
								resultsFile << rank_in_TrainingRawData[SameClassCoveredObservations[i]] << "   ";
								PosPatternCoverage[PosPatNumber].push_back(SameClassCoveredObservations[i]);
								PosPatternCoverage_rankInMultiClass[PosPatNumber].push_back(rank_in_TrainingRawData[SameClassCoveredObservations[i]]);
							}

							resultsFile << endl;


							PosPatNumber = PosPatNumber + 1;

							//for (int i=0; i < CoverdObservation.size(); i++) 
							{
								//	resultsFile << i << "  " << CoverdObservation[i] << "   "<< endl;
							}

							// resultsFile <<endl;
							// end of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern

						} // 2



						  // resultsFile << "uncovered row size" << UnCoveredRow.size()<< endl;

						  //	for (int i=0; i < UnCoveredRow.size(); i++) 
						  //{
						  //	resultsFile << "uncovered row" << UnCoveredRow[i] << endl;
						  //}


						  ////////////////////////////////////////////////// ACO starts here

						if (UnCoveredRow.size() > 0)

						{
							resultsFile << "The algorithm will be applied as the intilization steps do not give a feasible solution" << endl;

							vector<int> ColumnsInACO; // columns considered in the selection process of ACO

							for (int i = 0; i < DeletedColumns.size(); i++)  // to ignore deleted columns in ACO
							{
								if (DeletedColumns[i] == 0)
									ColumnsInACO.push_back(i);
							}

							for (int i = 0; i < DeletedColumns.size(); i++)  // to ignore mandatory columns in the selection process in ACO
							{
								MyCount = count(MandatoryColumns.begin(), MandatoryColumns.end(), i);
								if (MyCount == 1)
								{
									ColumnsInACO.erase(remove(ColumnsInACO.begin(), ColumnsInACO.end(), i), ColumnsInACO.end());
								}
							}


							//	for (int i=0; i < ColumnsInACO.size(); i++) 
							{
								//	resultsFile << "ColumnsInACO " << ColumnsInACO[i] << endl;
							}

							//	resultsFile << "number of ColumnsInACO " << ColumnsInACO.size() << endl;

							map <int, vector<int> > AntVisitedRow;
							map <int, vector<int> > AntSelectedColumn;
							map <int, float> PhermonOnColumns;
							map <int, double> DeltaPhermonOnColumns;
							map <int, float> StatisticOfColumns; // heuristic information

							float Beta = 5.0;
							float Rho = 0.01;
							float Eta = 0.005;
							//int NumberOfAnts = UnCoveredRow.size();  // equal number of the uncovered rows
							int Termination = 0; // index of ACO iterations
							vector<int> ObservationsUncoveredByMandatoryColumns;
							map <int, vector<int> > ObservationsUncoveredByEachColumnInACO; // shows the cost of each column
							int SelectedColumn;
							vector<int> UnCoveredObservationsByAntSolution;
							vector<int> UnCoveredObservationsInTheBestSolution;
							vector<int> ColumnsInTheBestSolution;
							float TauMax; // maximum value of pheromone
							float TauMin; // minimum value of phermone
							map <int, float> ColumnStrength;
							float StrongestColumn;
							float Que;
							float Que0 = 0.50;
							float RandomProbability;
							vector<float> CumProbabilty;
							map <int, float> ProbabiltyOfColumn;
							float CumSum;
							float SumColumnStrength;
							int ObjectiveValueOfACO;



							int NumberOfAnts = UnCoveredRow.size();  // equal number of the uncovered rows
							NumberOfAnts = min(NumberOfAnts, 100);

							int NumberOfIterations = int(500 / NumberOfAnts);

							//int NumberOfIterations = 4;

							// resultsFile << "NumberOfIterations = "<<NumberOfIterations << " , " << "NumberOfAnts = " << NumberOfAnts<<endl;

							Que = (float)rand() / RAND_MAX; // generate random number between 0 and 1

															// resultsFile << "Que = " << Que << endl;


															// intialize pheromone trail values and parameters of the ACO

							for (int i = 0; i < ColumnsInACO.size(); i++)
							{
								PhermonOnColumns[ColumnsInACO[i]] = 0.01;
								//	resultsFile << ColumnsInACO[i] << "  " << PhermonOnColumns[ColumnsInACO[i]]<< endl;
							}

							// end of intilization

							//resultsFile << "Number of ants is  " <<  NumberOfAnts << endl;

							for (int i = 0; i < MandatoryColumns.size(); i++)
							{

								//for(int j =0; j < NumberOfAnts; j++) // add the mandatory columns to the ant tour
								//{
								//	resultsFile << "ant " << j << endl;
								//	AntSelectedColumn[j].push_back(MandatoryColumns[i]);
								//	resultsFile << "ant selected mandatory column " << MandatoryColumns[i] << endl;
								//}

								for (int j = 0; j < DifferencesInSameClass[MandatoryColumns[i]].size(); j++) // find observations in the same class as Alpha-observation and uncovered by the mandatory columns

								{
									MyCount = count(ObservationsUncoveredByMandatoryColumns.begin(), ObservationsUncoveredByMandatoryColumns.end(), DifferencesInSameClass[MandatoryColumns[i]][j]);

									if (MyCount == 0)
									{
										ObservationsUncoveredByMandatoryColumns.push_back(DifferencesInSameClass[MandatoryColumns[i]][j]);

									}
								}
							}

							//resultsFile<< " Observations Uncovered By Mandatory Columns  " << endl;

							//for (int i=0; i <ObservationsUncoveredByMandatoryColumns.size(); i++)
							//{
								//resultsFile<< ObservationsUncoveredByMandatoryColumns[i] << "  " ;

							//}

							//resultsFile<< endl ;

							// end of steps that find observations in the same class as Alpha-observation and uncovered by the mandatory columns

							//resultsFile << "end of steps that find observations in the same class as Alpha-observation and uncovered by the mandatory columns" << endl;

							float Coverage; // shows the number of uncovered rows that can be covered by the column

											// find statistics (eta) of each column, eta = (rows covered by the column)/(combined cost of the new column and the mandatory columns)


							for (int i = 0; i < ColumnsInACO.size(); i++)
							{
								Coverage = 0;
								for (int j = 0; j<DifferencesInOppositeClass[ColumnsInACO[i]].size(); j++) // find coverage of each column from the uncovered rows in SCP
								{
									MyCount = count(UnCoveredRow.begin(), UnCoveredRow.end(), DifferencesInOppositeClass[ColumnsInACO[i]][j]);

									if (MyCount == 1)
										Coverage = Coverage + 1;
								}
								//	resultsFile<<  "  coverage of column "<< ColumnsInACO[i] << " is " << Coverage << endl ;

								for (int j = 0; j < ObservationsUncoveredByMandatoryColumns.size(); j++) // adding the observations uncovered by mandatory columns to cost
								{
									ObservationsUncoveredByEachColumnInACO[ColumnsInACO[i]].push_back(ObservationsUncoveredByMandatoryColumns[j]);

								}

								for (int j = 0; j<DifferencesInSameClass[ColumnsInACO[i]].size(); j++)
								{
									MyCount = count(ObservationsUncoveredByMandatoryColumns.begin(), ObservationsUncoveredByMandatoryColumns.end(), DifferencesInSameClass[ColumnsInACO[i]][j]);
									if (MyCount == 0)
										ObservationsUncoveredByEachColumnInACO[ColumnsInACO[i]].push_back(DifferencesInSameClass[ColumnsInACO[i]][j]);
								}

								//	resultsFile<< " Observations Uncovered By selected Columns  " << ColumnsInACO[i]<< endl;

								for (int j = 0; j <ObservationsUncoveredByEachColumnInACO[ColumnsInACO[i]].size(); j++)
								{
									//   resultsFile << ObservationsUncoveredByEachColumnInACO[ColumnsInACO[i]][j] << "  " ;

								}
								//	resultsFile << endl;

								StatisticOfColumns[ColumnsInACO[i]] = (Coverage / (0.0001 + ObservationsUncoveredByEachColumnInACO[ColumnsInACO[i]].size())); // 0.0001 is added to avoid deviding over zero
																																							  //	resultsFile << "Statistic Of this Column is "<< StatisticOfColumns[ColumnsInACO[i]]<< endl;
																																							  //	resultsFile <<  endl;
							}

							// end of finding statistics

							//resultsFile<< " end of finding statistics" << endl;

							ObjectiveValueOfACO = 10000000;

							while (Termination < NumberOfIterations)
							{ //3
								Termination = Termination + 1;
								//	resultsFile <<  " ///// iteration number //////  " << Termination << endl;
								AntSelectedColumn.clear();

								for (int i = 0; i < MandatoryColumns.size(); i++) // add the mandatory columns to the ant tour
								{
									for (int j = 0; j < NumberOfAnts; j++)
									{
										//  resultsFile << "ant " << j << endl;
										AntSelectedColumn[j].push_back(MandatoryColumns[i]);
										//   resultsFile << "ant selected mandatory column " << MandatoryColumns[i] << endl;
									}
								}

								AntVisitedRow.clear();
								for (int i = 0; i <NumberOfAnts; i++)  // assign an uncovered row to an ant
								{
									AntVisitedRow[i].push_back(UnCoveredRow[i]);
									//	resultsFile << "Ant " << i << " is assigned row number " << UnCoveredRow[i]<< endl;
								}                                    // end of assignment

								for (int i = 0; i <NumberOfAnts; i++)   // loop for each ant
								{ //4
								  //		resultsFile << "ant number  " << i << endl;



									for (int j = 0; j < UnCoveredRow.size(); j++) // each ant constructs a solution until a feasible solution is reached (all the uncovered rows are visited by the ant)
									{//5

									 //		resultsFile << "step number  " << j << endl;
										StrongestColumn = 0;
										SumColumnStrength = 0;

										// steps that selects one column to cover the current row

										//	  resultsFile << "UnCoveredRow[j]  " << AntVisitedRow[i].back() << endl;

										for (int k = 0; k < CoverdByColumnsAfterDelete[AntVisitedRow[i].back()].size(); k++) // calculating the strength of each column, this will be used later in calculationg the probabilities and in column selection
										{

											ColumnStrength[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]] = PhermonOnColumns[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]] * pow(StatisticOfColumns[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]], Beta);
											SumColumnStrength = SumColumnStrength + ColumnStrength[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]];
											//			resultsFile << "ColumnStrength  " << CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]<< "  is  " << ColumnStrength[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]]<< endl;
											//				resultsFile << "SumColumnStrength  " << SumColumnStrength<< endl;
										}  // end of calculating the strength of each column


										if (Que <= Que0)  // selecting a column based expoliting the learned knowledge ( selecting the strongest column)
										{

											for (int k = 0; k < CoverdByColumnsAfterDelete[AntVisitedRow[i].back()].size(); k++)
											{
												MyCount = count(AntSelectedColumn[i].begin(), AntSelectedColumn[i].end(), CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]);
												if (MyCount == 0) // to ensure that the column is not selected before
												{
													if (ColumnStrength[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]]  > StrongestColumn)
													{
														StrongestColumn = ColumnStrength[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]];
														SelectedColumn = CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k];
													}
												}
											}

										}

										if (Que > Que0) // selecting a column based exploring new routs ( this will be done randomly using the roulette selection method)

										{
											CumSum = 0;
											CumProbabilty.clear();
											CumProbabilty.push_back(0);
											ProbabiltyOfColumn.clear();
											//		resultsFile << "(Que > Que0)  " << endl;
											bool AcolumnIsSelected = 0;

											while (AcolumnIsSelected != 1)
											{
												for (int k = 0; k < CoverdByColumnsAfterDelete[AntVisitedRow[i].back()].size(); k++)
												{
													ProbabiltyOfColumn[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]] = ColumnStrength[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]] / SumColumnStrength;

													//			  resultsFile << "Probabilty Of Column  " << CoverdByColumnsAfterDelete[UnCoveredRow[j]][k]<< "  is  " <<  ProbabiltyOfColumn[CoverdByColumnsAfterDelete[UnCoveredRow[j]][k]] << endl;
													CumProbabilty.push_back(ProbabiltyOfColumn[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]] + CumProbabilty[k]);
													//	          resultsFile << "Cum. Probabilty Of Column  " << CoverdByColumnsAfterDelete[UnCoveredRow[j]][k]<< "  is  " <<  ProbabiltyOfColumn[CoverdByColumnsAfterDelete[UnCoveredRow[j]][k]]+ CumProbabilty[k] << endl;
												}
												RandomProbability = (float)rand() / RAND_MAX; // generate a random number between 0 and 1
																							  //			  resultsFile << "RandomProbability  " << RandomProbability<< endl;
												for (int k = 1; k< CumProbabilty.size(); k++)
												{
													if (RandomProbability >= CumProbabilty[k - 1] && RandomProbability <= CumProbabilty[k])
													{
														SelectedColumn = CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k - 1];
														MyCount = count(AntSelectedColumn[i].begin(), AntSelectedColumn[i].end(), SelectedColumn);
														if (MyCount == 0) // to ensure that the column is not selected before
														{
															AcolumnIsSelected = 1;
															break;
														}
													}
												}
											}
										}

										//		resultsFile << "SelectedColumn is  " << SelectedColumn << endl;

										AntSelectedColumn[i].push_back(SelectedColumn);

										for (int k = 0; k < DifferencesInOppositeClass[SelectedColumn].size(); k++) // This adds rows covered by the selected coulmn as ant visisted rows
										{
											MyCount = count(AntVisitedRow[i].begin(), AntVisitedRow[i].end(), DifferencesInOppositeClass[SelectedColumn][k]);

											if (MyCount == 0)
											{
												AntVisitedRow[i].push_back(DifferencesInOppositeClass[SelectedColumn][k]);
											}
										}


										// end of steps that selects one column to cover the current row


										// assign a new uncovered row to the ant

										MyCount = 1;
										for (int k = 0; k< UnCoveredRow.size(); k++)
										{
											MyCount = count(AntVisitedRow[i].begin(), AntVisitedRow[i].end(), UnCoveredRow[k]);
											if (MyCount == 0)
											{
												AntVisitedRow[i].push_back(UnCoveredRow[k]);
												//			resultsFile << "The new uncovered row assigned to the ant is row number  " << UnCoveredRow[k]<< endl;
												break;

											}

										}
										// end of assign a new uncovered row to the ant

										if (MyCount == 1)  // means that no more rows are uncovered and a feasible solution is reached
										{
											// calculate Objective function of this feasible solution


											//// start of deleting  redundant columns in ACO feasible solution 


											map <int, vector<int> > CanBeReplacedByinACO;
											map <int, vector<int> > ElementsInCanBeReplacedByinACO; // shows cost of alternative columns
											map <int, int>	DeletedColumnsinACO;
											int EndLoopIndexS;

											for (int r = 0; r < AntSelectedColumn[i].size(); r++)     // select a column
											{
												if (DifferencesInOppositeClass[AntSelectedColumn[i][r]].size() < 2)
												{
													if (DeletedColumnsinACO[AntSelectedColumn[i][r]] != 1)
													{

														for (int s = 0; s < DifferencesInOppositeClass[AntSelectedColumn[i][r]].size(); s++)  //select one row covered by this column(attribute)

														{
															EndLoopIndexS = 1;


															for (int m = 0; m < BestAttributes.size(); m++)    // select one attribute from the best set to check if it covers the selected row
															{
																EndLoopIndexM = 0;


																MyCount = count(AntSelectedColumn[i].begin(), AntSelectedColumn[i].end(), BestAttributes[m]); // check if this best attribute is existing in the ant selected attributes

																if (BestAttributes[m] != AntSelectedColumn[i][r] && DeletedColumns[BestAttributes[m]] != 1 && DeletedColumnsinACO[AntSelectedColumn[i][r]] != 1 && MyCount == 1)
																{
																	for (int n = 0; n < CoverdByColumns[DifferencesInOppositeClass[AntSelectedColumn[i][r]][s]].size(); n++) // check if the selected row is covered by one of the best attributes
																	{
																		if (CoverdByColumns[DifferencesInOppositeClass[AntSelectedColumn[i][r]][s]][n] != AntSelectedColumn[i][r])
																		{
																			if (BestAttributes[m] == CoverdByColumns[DifferencesInOppositeClass[AntSelectedColumn[i][r]][s]].at(n))
																			{
																				EndLoopIndexS = 0;
																				CanBeReplacedByinACO[AntSelectedColumn[i][r]].push_back(BestAttributes[m]);  // record the best column that can replace the selected column i to cover row j

																				for (int k = 0; k < DifferencesInSameClass[BestAttributes[m]].size(); k++)
																				{
																					MyCount = count(ElementsInCanBeReplacedByinACO[AntSelectedColumn[i][r]].begin(), ElementsInCanBeReplacedByinACO[AntSelectedColumn[i][r]].end(), DifferencesInSameClass[BestAttributes[m]][k]);

																					if (MyCount == 0)
																					{
																						ElementsInCanBeReplacedByinACO[AntSelectedColumn[i][r]].push_back(DifferencesInSameClass[BestAttributes[m]][k]); // the price of replacing column i by the recoreded best attributes

																					}
																					MyCount = 1;
																				}


																				EndLoopIndexM = 1;
																				break;
																			}
																		}
																	}
																}
																if (EndLoopIndexM == 1)  break;
															}

															if (EndLoopIndexS != 0)
															{
																break;
															}
														}

													}


													if (DifferencesInSameClass[AntSelectedColumn[i][r]].size() > ElementsInCanBeReplacedByinACO[AntSelectedColumn[i][r]].size() && CanBeReplacedByinACO[AntSelectedColumn[i][r]].size() == DifferencesInOppositeClass[AntSelectedColumn[i][r]].size()) // a column will be deleted if all of its rows are covered by other columns which collectively have lower cost than the column
													{

														DeletedColumnsinACO[AntSelectedColumn[i][r]] = 1;
														//  resultsFile  << " deleted  redundant columns in ACO feasible solution  " << AntSelectedColumn[i][r] << endl;

														AntSelectedColumn[i].erase(AntSelectedColumn[i].begin() + r);
													}
												}
											}


											/// end of deleting  redundant columns in ACO feasible solution 



											UnCoveredObservationsByAntSolution.clear(); // erase contents of the previous feasible solution

											for (int k = 0; k < AntSelectedColumn[i].size(); k++) // to find the uncovered observation in the feasible solution of ant i
											{
												for (int m = 0; m < DifferencesInSameClass[AntSelectedColumn[i][k]].size(); m++)
												{
													//	resultsFile << " Considering column " <<  AntSelectedColumn[i][k]<< " and observation "<<DifferencesInSameClass[AntSelectedColumn[i][k]][m]<< endl;

													MyCount = count(UnCoveredObservationsByAntSolution.begin(), UnCoveredObservationsByAntSolution.end(), DifferencesInSameClass[AntSelectedColumn[i][k]][m]);
													if (MyCount == 0)

														UnCoveredObservationsByAntSolution.push_back(DifferencesInSameClass[AntSelectedColumn[i][k]][m]); // objective function is the size of the vector UnCoveredObservationsByAntSolution
												}
											}

											// end of calculating Objective function of this feasible solution

											//		resultsFile << "UnCoveredObservationsByAntSolution" <<endl;

											//	for (int k=0; k < UnCoveredObservationsByAntSolution.size(); k++)
											//	{
											//		resultsFile << UnCoveredObservationsByAntSolution[k] << "  ";
											//	}
											//	resultsFile << endl;

											// record the best solution
											if (UnCoveredObservationsByAntSolution.size() < ObjectiveValueOfACO)
											{
												UnCoveredObservationsInTheBestSolution.clear();
												ColumnsInTheBestSolution.clear();

												for (int k = 0; k < UnCoveredObservationsByAntSolution.size(); k++)
												{
													UnCoveredObservationsInTheBestSolution.push_back(UnCoveredObservationsByAntSolution[k]);
												}

												ObjectiveValueOfACO = UnCoveredObservationsInTheBestSolution.size();

												for (int k = 0; k < AntSelectedColumn[i].size(); k++)
												{
													ColumnsInTheBestSolution.push_back(AntSelectedColumn[i][k]);
												}

											}
											// end of recording the best solution
											break; // all the uncovered rows are visited by this ant, so break loop j.

												   // end of assigning a new uncovered row to the ant
										}

									}//5
									 //	resultsFile << "Ant Visited Rows are  " << endl ;
									 //	for (int j =0; j< AntVisitedRow[i].size(); j++)
									 //    resultsFile << AntVisitedRow[i][j] << "  " << endl ;

									 //    resultsFile << endl ;
									 //	resultsFile << " AntSelectedColumn   ";
									 //		for (int k=0; k < AntSelectedColumn[i].size(); k++)
									 //			resultsFile << AntSelectedColumn[i][k] << "  ";
									 //		resultsFile << endl;
								}//4


								 //resultsFile << " Objective function of the ACO at this iteration  "  <<  ObjectiveValueOfACO << " uncovered observation." <<endl;

								if (ObjectiveValueOfACO == 0)
									break; // this best solution is optimal 

										   // update pheromone values using MMAS equation (14-17) page 778 Ren et al.(2010)

								TauMax = 1 / (Rho*UnCoveredObservationsInTheBestSolution.size());
								TauMin = TauMax * Eta;

								//	resultsFile << " TauMax " << TauMax << " ,    TauMin "<<TauMin << endl;

								for (int i = 0; i < ColumnsInACO.size(); i++)
								{
									MyCount = count(ColumnsInTheBestSolution.begin(), ColumnsInTheBestSolution.end(), ColumnsInACO[i]);
									if (MyCount == 1)
										DeltaPhermonOnColumns[ColumnsInACO[i]] = 1 / UnCoveredObservationsInTheBestSolution.size();
									if (MyCount == 0)
										DeltaPhermonOnColumns[ColumnsInACO[i]] = 0;
									//		resultsFile << " delta tau  of column " << i << " is "<<DeltaPhermonOnColumns[ColumnsInACO[i]] << endl;

									PhermonOnColumns[ColumnsInACO[i]] = Rho * PhermonOnColumns[ColumnsInACO[i]] + DeltaPhermonOnColumns[ColumnsInACO[i]];

									if (PhermonOnColumns[ColumnsInACO[i]] <TauMin)
										PhermonOnColumns[ColumnsInACO[i]] = TauMin;

									if (PhermonOnColumns[ColumnsInACO[i]] >TauMax)
										PhermonOnColumns[ColumnsInACO[i]] = TauMax;

									//		resultsFile << " delta tau of column " << i << " is "<<DeltaPhermonOnColumns[ColumnsInACO[i]] << endl;

								}


								// end of updating the phermone values

							} //3

							  //	resultsFile << "Solution of the algorithm  " << endl;
							  //	resultsFile << "selected columns  " ;
							  //	for (int i =0; i < ColumnsInTheBestSolution.size(); i++)
							  //		resultsFile << ColumnsInTheBestSolution[i] << "  " ;

							  //	resultsFile << endl;

							  //	resultsFile << " Objective function of the ACO:  "  <<  ObjectiveValueOfACO << " uncovered observation." <<endl;


							  // steps that find the observations having same class as Alpha observation and uncovered by the generated Alpha pattern

							vector<int> SameClassUnCoveredObservations;
							vector<int> SameClassCoveredObservations;
							for (int i = 0; i < ColumnsInTheBestSolution.size(); i++)
							{
								for (int j = 0; j < DifferencesInSameClass[ColumnsInTheBestSolution[i]].size(); j++)
								{
									MyCount = count(SameClassUnCoveredObservations.begin(), SameClassUnCoveredObservations.end(), DifferencesInSameClass[ColumnsInTheBestSolution[i]][j]);
									if (MyCount == 0)
									{
										SameClassUnCoveredObservations.push_back(DifferencesInSameClass[ColumnsInTheBestSolution[i]][j]);
									}
									MyCount = 1;
								}
							}

							//resultsFile << "end of steps that find the observations having same class as Alpha observation and uncovered by the generated Alpha pattern" << endl;

							//resultsFile << " SameClassUnCoveredObservations" << endl;
							//for (int i=0; i < SameClassUnCoveredObservations.size(); i++) 
							//{
							//resultsFile << SameClassUnCoveredObservations[i] << "   ";
							//}

							//resultsFile <<endl;

							// end of steps that find the observations having same class as Alpha observation and uncovered by the generated Alpha pattern

							// start of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern
							for (int i = 0; i < BinaryData.size(); i++)
							{
								if (BinaryData[i][0] == ObservationAlpha[0] && two_class_IgnoredObservations[i] != 1)
								{
									MyCount = count(SameClassUnCoveredObservations.begin(), SameClassUnCoveredObservations.end(), i);
									if (MyCount == 0)
									{
										SameClassCoveredObservations.push_back(i);
										CoverdObservation[i] = 1;
									}
									MyCount = 1;
								}
							}
							//resultsFile << "end of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern" << endl;

							// writting the pattern
							if (ObservationAlpha[0] == 1)
							{
								v_PositivePatterns.push_back(vector <int>());
								PosPatternCoverage.push_back(vector<int>());
								PosPatternCoverage_rankInMultiClass.push_back(vector<int>());
								for (int i = 0; i < BinaryData[0].size() - 1; i++)
								{
									v_PositivePatterns[PosPatNumber].push_back(-100);
								}

								resultsFile << "Pattern Number  " << PosPatNumber + 1 << " has;" << endl;
								CoveragePos.push_back(SameClassCoveredObservations.size());
								//TotalCoveragePos = TotalCoveragePos +  SameClassCoveredObservations.size();
							}



							for (int i = 0; i< ColumnsInTheBestSolution.size(); i++)
							{
								resultsFile << " Binary attribute number " << ColumnsInTheBestSolution[i] + 1 << " equals " << ObservationAlpha[ColumnsInTheBestSolution[i] + 1] << endl;

								if (ObservationAlpha[0] == 1)

									v_PositivePatterns[PosPatNumber][ColumnsInTheBestSolution[i]] = ObservationAlpha[ColumnsInTheBestSolution[i] + 1];
							}

							resultsFile << endl;



							// end writting

							// storing the pattern in the patterns sets

							//
							resultsFile << " Coverage of this pattern is  " << SameClassCoveredObservations.size() << endl;

							resultsFile << " Observations covered by this pattern are:  ";

							for (int i = 0; i < SameClassCoveredObservations.size(); i++)
							{
								resultsFile << rank_in_TrainingRawData[SameClassCoveredObservations[i]] << "   ";
								PosPatternCoverage[PosPatNumber].push_back(SameClassCoveredObservations[i]);
								PosPatternCoverage_rankInMultiClass[PosPatNumber].push_back(rank_in_TrainingRawData[SameClassCoveredObservations[i]]);

							}


							PosPatNumber = PosPatNumber + 1;



							//for (int i=0; i < CoverdObservation.size(); i++) 
							{
								//	resultsFile << i << "  " << CoverdObservation[i] << "   "<< endl;
							}

							resultsFile << endl;
							// end of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern



						} // 2

						  // ACO ends here


						resultsFile << "--------------------------------" << endl;

					} // 1  while loop , end of solving the problem





					  ////////////////////////////////////////////////////////////////////End of generating patterns using ACO


					  //// set covering to select a set of positive patterns
					  //if ( PosPatNumber > 1)
					  //{


					float 	StartPatterSetTime;
					StartPatterSetTime = clock();


					vector<int> CoveredObservationByPositivePattern;  // shows if the row is covered or not in this SCP


					for (int i = 0; i < BinaryData.size(); i++)
					{
						if (BinaryData[i][0] == 1 && two_class_IgnoredObservations[i] == 1)

							CoveredObservationByPositivePattern.push_back(i); // give a pseudo coverage to ignored observations
					}


					int MyCount;

					vector<int> ReducedPosPatternSet;

					while (CoveredObservationByPositivePattern.size() < NumberOfPositiveObservation) // loop until all the constriants are covered
					{
						// calculate the coverage of each attribute form the uncovered constraints and select the binary attribute that has the maximum coverage

						int CoverageOfBestPattern = -100;
						int SelectedPattern;

						vector<vector<int>> CurrentCoverOfPosPattern;

						for (int i = 0; i < v_PositivePatterns.size(); i++)
							CurrentCoverOfPosPattern.push_back(vector<int>()); // define a row for each attribute


						for (int j = 0; j< v_PositivePatterns.size(); j++)
						{
							MyCount = count(ReducedPosPatternSet.begin(), ReducedPosPatternSet.end(), j);
							if (MyCount == 0)
							{

								for (int i = 0; i < BinaryData.size(); i++)
								{
									if (BinaryData[i][0] == 1)
									{
										MyCount = count(CoveredObservationByPositivePattern.begin(), CoveredObservationByPositivePattern.end(), i);
										if (MyCount == 0)
										{
											MyCount = count(PosPatternCoverage[j].begin(), PosPatternCoverage[j].end(), i);
											if (MyCount == 1)
											{
												CurrentCoverOfPosPattern[j].push_back(i);

											}

										}
									}
								}

								int DD = CurrentCoverOfPosPattern[j].size();

								if (CoverageOfBestPattern < DD)
								{

									CoverageOfBestPattern = CurrentCoverOfPosPattern[j].size();
									SelectedPattern = j;
								}
							}
						}

						ReducedPosPatternSet.push_back(SelectedPattern);

						//  resultsFile <<	"selected pos pattern " << SelectedPattern << endl;

						for (int i = 0; i < CurrentCoverOfPosPattern[SelectedPattern].size(); i++)
						{
							CoveredObservationByPositivePattern.push_back(CurrentCoverOfPosPattern[SelectedPattern][i]); // state the covering of the constriants covered by the selected attribute by changing the their status from 0 to 1
						}

						// resultsFile <<	"CoveredObservationByPositivePattern size " << CoveredObservationByPositivePattern.size() << endl;
						// end of calculating the coverage of each attribute form the uncovered constraints and select the binary attribute that has the minimum 1/coverage


					}


					// end of selecting Pos pattern set




					resultsFile << "ReducedPatternSet" << endl;


					for (int i = 0; i < ReducedPosPatternSet.size(); i++)
					{
						resultsFile << ReducedPosPatternSet[i] + 1 << "  ";
					}
					resultsFile << endl;



					//vector<vector<int>> PositivePatterns;



					//for (int i = 0; i <v_PositivePatterns.size(); i++)
					//{
						//MyCount = count(ReducedPosPatternSet.begin(), ReducedPosPatternSet.end(), i);
						//if (MyCount == 1)
							//PositivePatterns.push_back(v_PositivePatterns[i]);
					//}






					/// end of set covering to select a set of patterns



					for (int i = 0; i <ReducedPosPatternSet.size(); i++)
					{
						TotalCoveragePos = TotalCoveragePos + PosPatternCoverage[ReducedPosPatternSet[i]].size();
					}


					vector<vector<vector<double>>> vInterpretedPatterns = Interpret2ClassPatterns(v_attribute_names, v_PositivePatterns, v_CutPoints, vClassNames[a], vClassNames[aa]);

					//for (int i = 0; i< vInterpretedPatterns.size(); i++)
					//{
						
					//}



					PatternCoverageFile << "Class " << vClassNames[a] << " VS." << " Class " << vClassNames[aa] << " ;  " << endl;  // to save the coverage and weight of each pattern in a file
					int counter = 1;
					int counter2 = 0;
					resultsFile << "Weights of the generated patterns; " << endl;


					for (int i = 0; i < PosPatternCoverage.size(); i++)
					{

						MyCount = count(ReducedPosPatternSet.begin(), ReducedPosPatternSet.end(), i);
						if (MyCount == 1)
						{
							PatternCoverageFile << "Pattern number " << counter << " ;  its weight is " << PosPatternCoverage[i].size() / TotalCoveragePos << " and its covered observations are : " << endl;
							counter = counter + 1;
							counter2 = counter2 + 1;
							for (int j = 0; j< PosPatternCoverage[i].size(); j++)
							{
								PatternCoverageFile << PosPatternCoverage_rankInMultiClass[i][j] << "   ";
							}
							PatternCoverageFile << endl;
							PosPatternWeight.push_back(PosPatternCoverage[i].size() / TotalCoveragePos);
							resultsFile << i + 1 << "  " << PosPatternCoverage[i].size() / TotalCoveragePos << endl;
							PosPrevalence << vClassNames[a] << "   " << vClassNames[aa] << "  ,   " << counter2 << " , " << PosPatternCoverage[i].size() << " , " << (double(PosPatternCoverage[i].size()) / double(NumberOfPositiveObservation)) << " , " << (PosPatternCoverage[i].size() / TotalCoveragePos) << endl;

							PatMatrixFile << vClassNames[a] << "   " << vClassNames[aa] << "   " << counter2 << "   " << PosPatternCoverage[i].size() / TotalCoveragePos << "  ";
							for (int j = 0; j< vInterpretedPatterns[i].size(); j++)
							{
								PatMatrixFile << vInterpretedPatterns.at(i).at(j).at(0) << "  " << vInterpretedPatterns.at(i).at(j).at(2) << "  " << vInterpretedPatterns.at(i).at(j).at(3) << "  ";
							}
							PatMatrixFile << endl;
						}

					}



					PatternCoverageFile << " ----------------------------------------- " << endl;
					resultsFile << endl;



					resultsFile << " Total coverage of the generated patterns is: " << TotalCoveragePos << endl<<endl;
					resultsFile << endl;
					EndPtrGen = clock();
					PtrGenElapsedTime = ((float)(EndPtrGen - StartPtrGen)) / CLOCKS_PER_SEC;
					resultsFile << "Elapsed time for Class " << vClassNames[a] <<" Vs. Class "<< vClassNames[aa] <<" is " << PtrGenElapsedTime  / 60 << " min." << endl<<endl;
				}
			}
		}// end of solving the problem for each class

		EndTime = clock();
		ElapsedTime = ((float)(EndTime - StartTime)) / CLOCKS_PER_SEC;

		resultsFile << endl;


		resultsFile << "Total elapsed time is " << ElapsedTime / 60 << " min." << endl;

	} // #0
}

//Accepts a 2D vector of raw data and returns a 3D vector: For each attribute, there is
//a 2D vector that contains (for each observation) the value of the attribute, the class 
//type of the attribute, and the value type.

vector<vector<vector<double>>> GenerateAttributeColumnVectors(vector<vector<double>>& two_class_Training)
{
	vector<vector<vector<double>>> vNumericalAttributes;
	unsigned int iNumberofAttributes = (int)two_class_Training.at(0).size();

	// FOR EACH ATTRIBUTE
	// We exclude the first COLUMN as it holds the class which will be stored simultaneously 
	// with the values and the data type in 3D vectors below. Hence we start from the second
	// column at i=1.
	for (unsigned int i = 1; i<iNumberofAttributes; i++)
	{
		vector <vector<double>> vAllValueClassType_temp;

		// FOR EACH OBSERVATION
		// We exclude the first ROW as it holds the data types which will be stored with the
		// class and values in 3D vectors. Hence we start with the second row at j=1.
		for (unsigned int j = 1; j<two_class_Training.size(); j++)
		{
			vector<double> vSingleValueClassType_temp;

			//The attribute value
			vSingleValueClassType_temp.push_back(two_class_Training.at(j).at(i));

			//The class
			vSingleValueClassType_temp.push_back(two_class_Training.at(j).at(0));

			//The Binarization type
			vSingleValueClassType_temp.push_back(two_class_Training.at(0).at(i));

			//Each of the Values and outcome for one given attribute. 
			vAllValueClassType_temp.push_back(vSingleValueClassType_temp);
		}
		// Place the 2D vector of one attribute formed from 3 columns representing the class per
		// observation, the value per observation, and the data type, into the 3D vector of
		// all attributes.
		vNumericalAttributes.push_back(vAllValueClassType_temp);
	}
	return vNumericalAttributes;
}

vector<vector<vector<double>>> SortAttributeValues(vector<vector<vector<double>>>& vAllAttributeColumns)
{
	vector<vector<vector<double>>> vSortedNumericalAttributes;
	vector<vector<double>> vSingleAttributeColumn;

	//For each attribute.
	for (unsigned int i = 0; i<vAllAttributeColumns.size(); i++)
	{
		vSingleAttributeColumn = vAllAttributeColumns.at(i);
		sort(vSingleAttributeColumn.begin(), vSingleAttributeColumn.end(), sortcol);
		vSortedNumericalAttributes.push_back(vSingleAttributeColumn);

	}
	return vSortedNumericalAttributes;

}


vector<vector<double>> ExtractCutPoints(vector<vector<vector<double>>>& v_SortedAttributesWithOutcome, int ClassName, int VsClassName)
{
	vector <double> v_cutPointsIndivAttribute;
	vector<vector<double>> v_cutPoints;
	double previousValue = -1.0;
	int previousSign = -1;
	bool samePreviousValues_DifferentSignFlag = false;

	//for each attribute i
	for (unsigned int i = 0; i<v_SortedAttributesWithOutcome.size(); i++)
	{
		v_cutPointsIndivAttribute.erase(v_cutPointsIndivAttribute.begin(), v_cutPointsIndivAttribute.end());

		if (v_SortedAttributesWithOutcome.at(i).at(0).at(2) == 3)
			//the attribute is numerical
		{
			previousValue = -1.0;
			previousSign = -1;
			samePreviousValues_DifferentSignFlag = false;
			//for each observation j of attribute i
			for (unsigned j = 0; j<v_SortedAttributesWithOutcome.at(i).size(); j++)
			{
				//First value in the vector
				if (previousValue != -1.0)
				{
					if (previousSign != v_SortedAttributesWithOutcome.at(i).at(j).at(1))
					{
						//Different class, different value: it's a cutpoint.
						if (previousValue != v_SortedAttributesWithOutcome.at(i).at(j).at(0))
						{
							v_cutPointsIndivAttribute.push_back((previousValue + v_SortedAttributesWithOutcome.at(i).at(j).at(0)) / 2);
							samePreviousValues_DifferentSignFlag = false;
							previousValue = v_SortedAttributesWithOutcome.at(i).at(j).at(0);
							if (v_SortedAttributesWithOutcome.at(i).at(j - 1).at(0) == v_SortedAttributesWithOutcome.at(i).at(j).at(0) && previousSign != v_SortedAttributesWithOutcome.at(i).at(j).at(1))
							{
								samePreviousValues_DifferentSignFlag = true;
							}
						}
						//It's not a cutpoint but it should raise a flag to
						//generate a cutpoint when the value changes even if the sign
						//doesn't change, because we already had a previousSign that was different.
						else
						{
							samePreviousValues_DifferentSignFlag = true;
						}
					}
					//No change in class.
					else
					{
						//If the value Changes and the samePreviousValuesDifferentSignFlag is true.
						if (samePreviousValues_DifferentSignFlag && previousValue != v_SortedAttributesWithOutcome.at(i).at(j).at(0))
						{
							//INSERT CUT POINTS
							v_cutPointsIndivAttribute.push_back((previousValue + v_SortedAttributesWithOutcome.at(i).at(j).at(0)) / 2);
							samePreviousValues_DifferentSignFlag = false;
							previousValue = v_SortedAttributesWithOutcome.at(i).at(j).at(0);
						}
					}
				}
				if (j < (v_SortedAttributesWithOutcome.at(i).size() - 1))
				{
					if (v_SortedAttributesWithOutcome.at(i).at(j).at(0) != v_SortedAttributesWithOutcome.at(i).at(j + 1).at(0) || previousValue == -1)
					{
						previousValue = v_SortedAttributesWithOutcome.at(i).at(j).at(0);
					}
				}


				previousSign = (int)v_SortedAttributesWithOutcome.at(i).at(j).at(1);
			}
		}
		// When its an ordered discrete attribute then each attribute value is a cutpoint except the last one
		else if (v_SortedAttributesWithOutcome.at(i).at(0).at(2) == 2)
		{
			double PreviousValue = -1;
			for (unsigned j = 0; j<v_SortedAttributesWithOutcome.at(i).size(); j++)
			{
				if (PreviousValue != v_SortedAttributesWithOutcome.at(i).at(j).at(0))
				{
					v_cutPointsIndivAttribute.push_back(v_SortedAttributesWithOutcome.at(i).at(j).at(0));
				}
				PreviousValue = v_SortedAttributesWithOutcome.at(i).at(j).at(0);
			}
			v_cutPointsIndivAttribute.pop_back();
		}
		else
		{
			double PreviousValue = -1;
			for (unsigned j = 0; j<v_SortedAttributesWithOutcome.at(i).size(); j++)
			{
				if (PreviousValue != v_SortedAttributesWithOutcome.at(i).at(j).at(0))
				{
					v_cutPointsIndivAttribute.push_back(v_SortedAttributesWithOutcome.at(i).at(j).at(0));
				}
				PreviousValue = v_SortedAttributesWithOutcome.at(i).at(j).at(0);
			}
		}
		v_cutPoints.push_back(v_cutPointsIndivAttribute);
	}
	// disposable code to test vbinkind START





	CutPoints << "-------------------------" << endl;
	CutPoints << "  Class " << ClassName << " Vs. "<<"Class " << VsClassName << endl;
	CutPoints << "-------------------------" << "\n";

	for (int i = 0; i < v_cutPoints.size(); i++)
	{
		for (int j = 0; j < v_cutPoints[i].size(); j++)
		{
			CutPoints << "  " << v_cutPoints[i][j];
		}
		CutPoints << "\n";
	}

	// disposable code to test vbindkind END

	return v_cutPoints;
}

vector<vector<int>> GetBinaryTraining(vector<vector<double>>& vCutPoints, vector<vector<double>>& two_class_Training)
{

	vector<vector<double>> vObservations;
	vector<vector<int>> vBinarizationResult;

	// STORE IN 2D VECTOR vObservations THE ATTRIBUTE VALUES ONLY. NO DATA TYPES NO CLASSES.
	// START READING FROM SECOND ROW BECAUSE FIRST CONTAINS DATA ATTRIBUTE TYPES 
	for (unsigned int i = 1; i<two_class_Training.size(); i++)
	{
		vector <double> tmp;
		// START READING FROM SECOND COLUMN BECAUSE FIRST CONTAINS THE CLASSES
		for (unsigned int j = 1; j<two_class_Training.at(i).size(); j++)
		{
			tmp.push_back(two_class_Training.at(i).at(j));
		}
		vObservations.push_back(tmp);
	}

	// For each observation and each attribute value in each observation
	for (unsigned int i = 0; i<vObservations.size(); i++)
	{
		//For each observation, one vector will be used to hold all of it's attribute values.
		vector <int> vBinarizedObservation;

		// add +1 becoz first row is not observation
		vBinarizedObservation.push_back((int)two_class_Training.at(i + 1).at(0));

		for (unsigned int j = 0; j<vObservations.at(i).size(); j++)
		{
			//For each attribute analyze it with respect to the cutpoints.
			double attrValue = vObservations.at(i).at(j);

			//For observation i, attribute j, fetch the cutpoints that qualify the j'th attribute.
			vector <double> attribCutPoints = vCutPoints.at(j);

			//For attribute j, fetch the type of the attribute.
			// add +1 becoz first column is the outcome and means nothing in attribute types row
			int attrType = (int)two_class_Training.at(0).at(j + 1);
			if (attrType == 3 || attrType == 2)
			{
				//For each of the cutpoints of the j'th attribute.
				for (unsigned int k = 0; k<attribCutPoints.size(); k++)
				{
					// Analyse limit binarization.
					if (attrValue > attribCutPoints.at(k))
					{
						vBinarizedObservation.push_back(1);
					}
					else
					{
						vBinarizedObservation.push_back(0);
					}
				}
			}
			else if (attrType == 1)
			{
				//For each of the cutpoints of the j'th attribute.
				for (unsigned int k = 0; k<attribCutPoints.size(); k++)
				{
					// Analyse limit binarization.
					if (attrValue == attribCutPoints.at(k))
					{
						vBinarizedObservation.push_back(1);
					}
					else
					{
						vBinarizedObservation.push_back(0);
					}
				}
			}
		}
		vBinarizationResult.push_back(vBinarizedObservation);
	}
	return vBinarizationResult;
}

void FindDifferencesSameClass(vector<int>& ObservationAlpha_1, vector<vector<int>  >& BinaryData_1, vector<bool>& CoverdObservation_1, vector<bool>& IgnoredObservation_1, vector<vector<int>  > &DifferencesInSameClass_1)
{
	int my_count;
	for (int j = 1; j < BinaryData_1[1].size(); j++) // j starts form 1 to ignore the classification column (first column) given in the data 
	{
		for (int i = 0; i < BinaryData_1.size(); i++)
		{
			//	my_count = count (CoverdObservation_1.begin(),CoverdObservation_1.end(), i);
			//	if (CoverdObservation_1[i] == 0)
			{
				if (IgnoredObservation_1[i] != 1 && BinaryData_1[i][0] == ObservationAlpha_1[0])
				{
					DifferencesInSameClass_1.push_back(vector <int>());
					if (BinaryData_1[i][j] != ObservationAlpha_1[j])
					{
						DifferencesInSameClass_1[j - 1].push_back(i);

					}
				}
			}
		}
	}
}

void FindDifferencesOppositeClass(vector<int>& ObservationAlpha_1, vector<vector<int>  >& BinaryData_1, vector<bool>& CoverdObservation_1, vector<bool>& IgnoredObservation_1, vector<vector<int>  > &DifferencesInOppositeClass_1)
{
	int my_count;
	for (int j = 1; j < BinaryData_1[1].size(); j++)
	{
		for (int i = 0; i < BinaryData_1.size(); i++)
		{
			//my_count = count (CoverdObservation_1.begin(),CoverdObservation_1.end(), i);
			//	if (CoverdObservation_1[i] == 0)
			{
				if (IgnoredObservation_1[i] != 1 && BinaryData_1[i][0] != ObservationAlpha_1[0])
				{
					DifferencesInOppositeClass_1.push_back(vector <int>());
					if (BinaryData_1[i][j] != ObservationAlpha_1[j])
					{
						DifferencesInOppositeClass_1[j - 1].push_back(i);
					}
				}
			}
		}
	}

}

vector<vector<vector<double>>> Interpret2ClassPatterns(vector<string> attribue_Names, vector<vector<int>> vPatterns, vector<vector<double>> vCutPoints, int ClassName, int VsClassName)
{
	vector<vector<vector<double>>> vInterpretedPatterns;
	vector<vector<double>> vOneInterpretedPatternTemp;
	vector<double> vInterpretedLiteralTemp;

	// For all the Patterns
	for (unsigned int i = 0; i<vPatterns.size(); i++)
	{
		vOneInterpretedPatternTemp.clear();
		// For each element in the Pattern
		for (unsigned int j = 0; j<vPatterns.at(i).size(); j++)
		{
			vInterpretedLiteralTemp.clear();
			// Only the elements that exist
			if (vPatterns.at(i).at(j) != -100)
			{
				unsigned int iFoundCutPointPosition = 0;
				double dFoundCutPoint = 0;
				unsigned int iFindCutPoint = 0;
				unsigned int iFoundAttribute = 0;
				unsigned int iCutPointLocalPosition = -1;
				// Match The CutPoint
				for (unsigned int m = 0; m<vCutPoints.size(); m++)
				{
					iFoundAttribute++;
					for (unsigned int n = 0; n<vCutPoints.at(m).size(); n++)
					{
						if (j == iFindCutPoint)
						{
							iFoundCutPointPosition = iFindCutPoint + 1;
							dFoundCutPoint = vCutPoints.at(m).at(n);
							iCutPointLocalPosition = n + 1;
							break;
						}
						else
							iFindCutPoint++;
					}
					if (iFoundCutPointPosition != 0)
						break;
				}
				vInterpretedLiteralTemp.push_back(iFoundAttribute);
				vInterpretedLiteralTemp.push_back(iCutPointLocalPosition/*iFoundCutPointPosition*/);
				if (vPatterns.at(i).at(j) == 1)
					vInterpretedLiteralTemp.push_back(1);
				else if (vPatterns.at(i).at(j) == 0)
					vInterpretedLiteralTemp.push_back(-1);
				vInterpretedLiteralTemp.push_back(dFoundCutPoint);

				// Add interpreted literal the 2d interpretation vector of the pattern
				vOneInterpretedPatternTemp.push_back(vInterpretedLiteralTemp);
			}
		}
		vInterpretedPatterns.push_back(vOneInterpretedPatternTemp);
	}




	Patternfile << "-------------------------\n";
	Patternfile << "  Class " << ClassName << " Vs. " << "Class " << VsClassName << endl;
	Patternfile << "-------------------------\n";
	for (unsigned int x = 0; x<vInterpretedPatterns.size(); x++)
	{
		Patternfile << "Pattern  " << x + 1 << endl;
		for (unsigned int y = 0; y<vInterpretedPatterns.at(x).size(); y++)
		{
			Patternfile << attribue_Names[vInterpretedPatterns.at(x).at(y).at(0) - 1].c_str();

			if (vInterpretedPatterns.at(x).at(y).at(2) == 1)
			{
				Patternfile << "  Greater Than   " << vInterpretedPatterns.at(x).at(y).at(3);

			}
			else if (vInterpretedPatterns.at(x).at(y).at(2) == -1)
			{
				Patternfile << "  Less Than  " << vInterpretedPatterns.at(x).at(y).at(3);

			}
			Patternfile << "\n";
		}
		Patternfile << "\n";
	}



	return vInterpretedPatterns;
}

vector<vector<int>> Reduce2DBinaryTrainingVector(vector<vector<int>> vOriginalVector, vector<unsigned int> vRetainedAttributes)
{
	vector<vector<int>> vNewVector;

	for (unsigned int i = 0; i<vOriginalVector.size(); i++)
	{
		vector<int> vNewObservation;
		vNewObservation.push_back(vOriginalVector.at(i).at(0));
		for (unsigned int j = 1; j<vOriginalVector.at(i).size(); j++)
		{
			for (unsigned int k = 0; k<vRetainedAttributes.size(); k++)
			{
				if (j == k + 1)
					vNewObservation.push_back(vOriginalVector.at(i).at(j));
			}
		}
		vNewVector.push_back(vNewObservation);
	}
	return vNewVector;
}

vector < vector <double> > cleanData(vector < vector <double> >& inputVec) {

	bool contradictionFlag = true;
	bool contradictionsExist = false;
	bool displayMessage = false;
	bool done = false;
	vector<unsigned int> alreadyChecked;
	vector<vector<double>>::iterator It = inputVec.begin();
	ofstream contradictions;
	contradictions.open("C:\\cbmLAD\\ObservationsToBeChecked.txt");

	for (unsigned int i = 0; i < inputVec.size(); i++) {
		//to check of the vector was already checked or not
		done = false;
		for (unsigned int ii = 0; ii < alreadyChecked.size(); ii++) {
			if (i == alreadyChecked.at(ii)) {
				done = true;
				break;
			}
		}
		if (done)
			continue;
		//now we check vector j against vector i to see if they contradict
		contradictionsExist = false;
		for (unsigned int j = i + 1; j < inputVec.size(); j++) {
			contradictionFlag = true;
			//we start by k=1 to skip the first element which is the observation's class
			for (unsigned int k = 1; k < inputVec.at(j).size(); k++) {
				if (inputVec.at(i).at(k) != inputVec.at(j).at(k)) {
					contradictionFlag = false;
					break;
				}
			}
			if (contradictionFlag) {
				contradictionsExist = true;
				alreadyChecked.push_back(j);
				//contradictions<<"this is j"<<endl;
				contradictions << "Observation " << j << ":" << endl;
				for (unsigned int c = 0; c<inputVec.at(j).size(); c++)
					contradictions << inputVec.at(j).at(c) << ",";
				contradictions << endl;
				//It = inputVec.begin();
				//It += j;
				//inputVec.erase(It);
			}
		}
		if (contradictionsExist) {
			displayMessage = true;
			//contradictions<<"this is i*********"<<endl;
			contradictions << "Observation " << i << ":" << endl;
			for (unsigned int c = 0; c<inputVec.at(i).size(); c++)
				contradictions << inputVec.at(i).at(c) << ",";
			contradictions << endl;
			contradictions << "----------------------------------------------------" << endl;
			//It = inputVec.begin();
			//It += i;
			//inputVec.erase(It);
		}
	}

	contradictions.close();
	return inputVec;
}