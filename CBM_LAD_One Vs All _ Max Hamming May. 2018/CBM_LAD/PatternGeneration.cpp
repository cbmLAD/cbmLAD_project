#include "PatternGeneration.h"



PatternGeneration::PatternGeneration()
{
}


PatternGeneration::~PatternGeneration()
{
}

ofstream PatternCoverageFile("C:\\cbmLAD\\Coverage of Multi-Class Patterns.txt", ios::out);
ofstream PosPrevalence("C:\\cbmLAD\\Charcteristics of the generated patterns.txt", ios::app);
ofstream resultsFile("C:\\cbmLAD\\results.txt", ios::out | ios::app);


// start of solving the problem

vector<vector<int>> PatternGeneration::GeneratePattern(vector<vector<int> > v_BinaryData, vector <bool> v_IgnoredObservation, int v_ClassName, int v_NumberOfPositiveObservation, vector<bool> v_CoverdObservation, int V_a)
{
    vector<vector<int>> v_PositivePatternSet;
	vector<vector<int>> ReducedPositivePatterns;
	vector <int> Hamming_distance(v_BinaryData.size());
	vector<double> CoveragePos;
	double TotalCoveragePos = 0;
	vector<vector<int>> PosPatternCoverage;
	int PosPatNumber = 0;

	//		resultsFile << "Hamming matrix" << endl;
	//	for (int i = 0; i < Hamming_distance.size(); i++)
	//		resultsFile << Hamming_distance[i] << endl;
	//	resultsFile << v_BinaryData.size() << endl;
	//	resultsFile << v_BinaryData[1].size() << endl;

	for (int i = 0; i < v_BinaryData.size(); i++)
	{
		if (v_BinaryData[i][0] == 1 && v_IgnoredObservation[i] != 1)
		{
			//	resultsFile << "i  " <<i << endl;
			for (int ii = 0; ii < v_BinaryData.size(); ii++)
			{
				if (ii != i && v_IgnoredObservation[ii] != 1 && v_BinaryData[i][0] != v_BinaryData[ii][0])
				{
					//		resultsFile << "ii   "<<ii << endl;
					for (int j = 1; j < v_BinaryData[1].size(); j++)

					{
						
						if (v_BinaryData[i][j] != v_BinaryData[ii][j])
							{
								Hamming_distance[i] = Hamming_distance[i] + 1;
							}
						
					}
				}
			}
		}
	}

	//for (int i = 0; i < v_BinaryData.size(); i++)
		//resultsFile << Hamming_distance[i] << endl;

while (accumulate(v_CoverdObservation.begin(), v_CoverdObservation.end(), 0) < (v_NumberOfPositiveObservation))
{  //1 stops when all the observations are covered


	
	
	int MyCount;

	vector<int> ObservationAlpha(v_BinaryData[1].size());

	int Max = 0;
	for (int i = 0; i < Hamming_distance.size(); i++)
	{
		if (v_CoverdObservation[i] == 0 && v_IgnoredObservation[i] != 1 && Hamming_distance[i]> Max)
			Max = Hamming_distance[i];

	}


	for (int i = 0; i < Hamming_distance.size(); i++)
	{
		if (v_CoverdObservation[i] == 0 && v_IgnoredObservation[i] != 1 && Hamming_distance[i] == Max)
		{
			for (int j = 0; j < v_BinaryData[i].size(); j++)
				ObservationAlpha.at(j) = v_BinaryData[i].at(j);

			v_CoverdObservation[i] = 1;
		//	resultsFile << "alpha is " << i +1 << " " << ObservationAlpha.at(0)<< endl;
			break;
		}
	}
	
	vector<bool> DeletedColumns(v_BinaryData[1].size() - 1);
	vector<vector<int>  > DifferencesInSameClass;
	FindDifferencesSameClass(ObservationAlpha, v_BinaryData, v_CoverdObservation, v_IgnoredObservation, DifferencesInSameClass);
	
	vector<vector<int>  > DifferencesInOppositeClass;

	FindDifferencesOppositeClass(ObservationAlpha, v_BinaryData, v_CoverdObservation, v_IgnoredObservation, DifferencesInOppositeClass);
	
	for (int i = 0; i < v_BinaryData[1].size() - 1; i++)
	{
	
		
		if (DifferencesInOppositeClass[i].size() == 0)  // deletes redundant columns that do not cover any row in SCP
		{
			DeletedColumns[i] = 1;
			
		}
	
	}
	
	// start of steps that delete a column which covers a set of obnservations that can be covered by another column at lower cost


	bool Answer;


	for (int i = 0; i < v_BinaryData[1].size() - 2; i++)
	{
		if (DeletedColumns.at(i) == 0)
		{
			for (int j = 0; j < v_BinaryData[1].size() - 1; j++)
			{
				if (j != i)
				{

					if (DeletedColumns.at(j) == 0)
					{
						if (DifferencesInOppositeClass[i].size() <= DifferencesInOppositeClass[j].size())
						{

							Answer = includes(DifferencesInOppositeClass[j].begin(), DifferencesInOppositeClass[j].end(), DifferencesInOppositeClass[i].begin(), DifferencesInOppositeClass[i].end());
						

							if ((Answer == 1) && (DifferencesInSameClass[i].size() >= DifferencesInSameClass[j].size()))
							{
								DeletedColumns[i] = 1;
							
							}
						}
						if (DeletedColumns[i] == 1) break;

					}

				}

			}
		}
	}

	

	// end of steps that delete a column which covers a set of obnservations that can be covered by another column at lower cost	





	///////// start of steps that find the columns covering given observations 

	map <int, vector<int> > CoverdByColumns; 	//shows the rows covered (constraints satisfied)  by coulmns in the SCP
	vector<bool> FoundBefore(v_BinaryData.size());

	for (int i = 0; i < v_BinaryData[1].size() - 1; i++)
	{
		for (int j = 0; j < DifferencesInOppositeClass[i].size(); j++)
		{
			if (FoundBefore.at(DifferencesInOppositeClass[i][j]) != 1)
			{
		  

				for (int k = 0; k < v_BinaryData[1].size() - 1; k++)
				{
					if (DeletedColumns[k] != 1)
					{
						if (find(DifferencesInOppositeClass[k].begin(), DifferencesInOppositeClass[k].end(), DifferencesInOppositeClass[i][j]) != DifferencesInOppositeClass[k].end())
						{
							CoverdByColumns[DifferencesInOppositeClass[i][j]].push_back(k);
									  
						}
					}
				}
				
				FoundBefore.at(DifferencesInOppositeClass[i][j]) = 1;
			}
		}
		
	}

	//// end of steps that find the columns covering given observations 

	

	int EndLoopIndexM;
	int EndLoopIndexJ;

	//////////// Start of finding the list of best attributes
	vector<int> BestAttributes(v_BinaryData[1].size() - 1);
	vector<int> ListedAttributes(v_BinaryData[1].size() - 1);
	int MinimumCost = 1000000;
	int StoredJ;
	for (int i = 0; i < v_BinaryData[1].size() - 1; i++)
	{
		ListedAttributes.at(i) = 0;
	}

	for (int i = 0; i < v_BinaryData[1].size() - 1; i++)
	{
		for (int j = 0; j < v_BinaryData[1].size() - 1; j++)
		{
			if (ListedAttributes.at(j) == 0)
			{
			
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
					
					}
					if (DifferencesInOppositeClass[j].size() <= DifferencesInOppositeClass[StoredJ].size())
					{
						BestAttributes[i] = StoredJ;
						
					}

				}
			}

		}
		ListedAttributes.at(BestAttributes[i]) = 1;
		//  resultsFile << i << "   " << BestAttributes[i] << endl;
		MinimumCost = 1000000;
	}
	//////////// end of finding the list of best attributes
	
	
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
	
		if (CoverdByColumnsAfterDelete[it->first].size() == 1)
		{
			MyCount = count(MandatoryColumns.begin(), MandatoryColumns.end(), CoverdByColumnsAfterDelete[it->first][0]);
		
			if (MyCount == 0)
			{
				MandatoryColumns.push_back(CoverdByColumnsAfterDelete[it->first][0]);
				
			}
			MyCount = 1;
		}
	}


	//// end of steps that finds columns inclusion

	
	

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
			

				MyCount = count(CoveredRow.begin(), CoveredRow.end(), it->first);

				if (CoverdByColumnsAfterDelete[it->first][i] == MandatoryColumns[j] && MyCount == 0) // to see if the row is covered by one of the manadatory columns
				{
					CoveredRow.push_back(it->first);
				
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
	
	//end of steps that check if the manadatory columns cover all rows , and store the uncovered rows


	//// Check if deletion and inclusion of columns results in a complete soltuion

	int SumOfElementsInMandatoryColumns;
	

	int SumOfElementsInDeletedColumns;
	SumOfElementsInDeletedColumns = accumulate(DeletedColumns.begin(), DeletedColumns.end(), 0);
	


	if (UnCoveredRow.size() == 0)
	{ // 2 this will show the feasible results of the initilization, no need for ACO to solve this Alpha problem, go to the next alpha problem


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



		// end of steps that find the observations having same class as Alpha observation and uncovered by the generated Alpha pattern

		// start of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern
		for (int i = 0; i < v_BinaryData.size(); i++)
		{
			if (v_BinaryData[i][0] == ObservationAlpha[0] && v_IgnoredObservation[i] != 1)
			{
				MyCount = count(SameClassUnCoveredObservations.begin(), SameClassUnCoveredObservations.end(), i);
				if (MyCount == 0)
				{
					SameClassCoveredObservations.push_back(i);
					v_CoverdObservation[i] = 1;
				}
				MyCount = 1;
			}
		}

		// writting the pattern
		if (ObservationAlpha[0] == 1)
		{
			v_PositivePatternSet.push_back(vector <int>());
			PosPatternCoverage.push_back(vector<int>());
			for (int i = 0; i < v_BinaryData[0].size() - 1; i++)
			{
				v_PositivePatternSet[PosPatNumber].push_back(-100);
			}

			resultsFile << "Pattern Number  " << PosPatNumber + 1 << " has;" << endl;
			CoveragePos.push_back(SameClassCoveredObservations.size());
			

		}

		for (int i = 0; i< MandatoryColumns.size(); i++)
		{
			resultsFile << " Binary attribute number " << MandatoryColumns[i] + 1 << " equals " << ObservationAlpha[MandatoryColumns[i] + 1] << endl;

			if (ObservationAlpha[0] == 1)
				v_PositivePatternSet[PosPatNumber][MandatoryColumns[i]] = ObservationAlpha[MandatoryColumns[i] + 1];

		}


		resultsFile << endl;

		// end writting



		resultsFile << " Coverage of this pattern is  " << SameClassCoveredObservations.size() << endl;


		resultsFile << " Observations covered by this pattern are:  ";


		for (int i = 0; i < SameClassCoveredObservations.size(); i++)
		{
			resultsFile << SameClassCoveredObservations[i] + 1 << "   ";

			PosPatternCoverage[PosPatNumber].push_back(SameClassCoveredObservations[i]);


		}

		resultsFile << endl;


		PosPatNumber = PosPatNumber + 1;


		// end of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern

	} // 2


	
	  ////////////////////////////////////////////////// ACO starts here

	if (UnCoveredRow.size() > 0)

	{ // 2 this will apply ACO when the intilization steps do no give feasible solution

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



		Que = (float)rand() / RAND_MAX; // generate random number between 0 and 1

										

		// intialize pheromone trail values and parameters of the ACO

		for (int i = 0; i < ColumnsInACO.size(); i++)
		{
			PhermonOnColumns[ColumnsInACO[i]] = 0.01;
		
		}

		// end of intilization

		

		for (int i = 0; i < MandatoryColumns.size(); i++)
		{

			
			for (int j = 0; j < DifferencesInSameClass[MandatoryColumns[i]].size(); j++) // find observations in the same class as Alpha-observation and uncovered by the mandatory columns

			{
				MyCount = count(ObservationsUncoveredByMandatoryColumns.begin(), ObservationsUncoveredByMandatoryColumns.end(), DifferencesInSameClass[MandatoryColumns[i]][j]);

				if (MyCount == 0)
				{
					ObservationsUncoveredByMandatoryColumns.push_back(DifferencesInSameClass[MandatoryColumns[i]][j]);

				}
			}
		}

	
		// end of steps that find observations in the same class as Alpha-observation and uncovered by the mandatory columns

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

		

			StatisticOfColumns[ColumnsInACO[i]] = (Coverage / (0.0001 + ObservationsUncoveredByEachColumnInACO[ColumnsInACO[i]].size())); // 0.0001 is added to avoid deviding over zero
																																		  //	resultsFile << "Statistic Of this Column is "<< StatisticOfColumns[ColumnsInACO[i]]<< endl;
																																		  //	resultsFile <<  endl;
		}

		// end of finding statistics


		ObjectiveValueOfACO = 10000000;

		while (Termination < NumberOfIterations)
		{ //3
			Termination = Termination + 1;
		
			AntSelectedColumn.clear();

			for (int i = 0; i < MandatoryColumns.size(); i++) // add the mandatory columns to the ant tour
			{
				for (int j = 0; j < NumberOfAnts; j++)
				{
					
					AntSelectedColumn[j].push_back(MandatoryColumns[i]);
					
				}
			}

			AntVisitedRow.clear();
			for (int i = 0; i <NumberOfAnts; i++)  // assign an uncovered row to an ant
			{
				AntVisitedRow[i].push_back(UnCoveredRow[i]);
				
			}                                    // end of assignment

			for (int i = 0; i <NumberOfAnts; i++)   // loop for each ant
			{ //4
		

				for (int j = 0; j < UnCoveredRow.size(); j++) // each ant constructs a solution until a feasible solution is reached (all the uncovered rows are visited by the ant)
				{//5

				
					StrongestColumn = 0;
					SumColumnStrength = 0;

					// steps that selects one column to cover the current row

				

					for (int k = 0; k < CoverdByColumnsAfterDelete[AntVisitedRow[i].back()].size(); k++) // calculating the strength of each column, this will be used later in calculationg the probabilities and in column selection
					{

						ColumnStrength[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]] = PhermonOnColumns[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]] * pow(StatisticOfColumns[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]], Beta);
						SumColumnStrength = SumColumnStrength + ColumnStrength[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]];
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

								CumProbabilty.push_back(ProbabiltyOfColumn[CoverdByColumnsAfterDelete[AntVisitedRow[i].back()][k]] + CumProbabilty[k]);
							}
							RandomProbability = (float)rand() / RAND_MAX; // generate a random number between 0 and 1
																		  
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
								
								MyCount = count(UnCoveredObservationsByAntSolution.begin(), UnCoveredObservationsByAntSolution.end(), DifferencesInSameClass[AntSelectedColumn[i][k]][m]);
								if (MyCount == 0)

									UnCoveredObservationsByAntSolution.push_back(DifferencesInSameClass[AntSelectedColumn[i][k]][m]); // objective function is the size of the vector UnCoveredObservationsByAntSolution
							}
						}

						
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
				
			}//4


		
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

				
			}


			// end of updating the phermone values

		} //3

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

		
		// end of steps that find the observations having same class as Alpha observation and uncovered by the generated Alpha pattern

		// start of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern
		for (int i = 0; i < v_BinaryData.size(); i++)
		{
			if (v_BinaryData[i][0] == ObservationAlpha[0] && v_IgnoredObservation[i] != 1)
			{
				MyCount = count(SameClassUnCoveredObservations.begin(), SameClassUnCoveredObservations.end(), i);
				if (MyCount == 0)
				{
					SameClassCoveredObservations.push_back(i);
					v_CoverdObservation[i] = 1;
				}
				MyCount = 1;
			}
		}
		
		// writting the pattern

		if (ObservationAlpha[0] == 1)
		{
			v_PositivePatternSet.push_back(vector <int>());
			PosPatternCoverage.push_back(vector<int>());
			for (int i = 0; i < v_BinaryData[0].size() - 1; i++)
			{
				v_PositivePatternSet[PosPatNumber].push_back(-100);
			}

			resultsFile << "Pattern Number  " << PosPatNumber + 1 << " has;" << endl;
			CoveragePos.push_back(SameClassCoveredObservations.size());
			//TotalCoveragePos = TotalCoveragePos +  SameClassCoveredObservations.size();
		}



		for (int i = 0; i< ColumnsInTheBestSolution.size(); i++)
		{
			resultsFile << " Binary attribute number " << ColumnsInTheBestSolution[i] + 1 << " equals " << ObservationAlpha[ColumnsInTheBestSolution[i] + 1] << endl;

			if (ObservationAlpha[0] == 1)

				v_PositivePatternSet[PosPatNumber][ColumnsInTheBestSolution[i]] = ObservationAlpha[ColumnsInTheBestSolution[i] + 1];
		}

		resultsFile << endl;



		// end writting

		// storing the pattern in the patterns sets

		//
		resultsFile << " Coverage of this pattern is  " << SameClassCoveredObservations.size() << endl;

		resultsFile << " Observations covered by this pattern are:  ";

		for (int i = 0; i < SameClassCoveredObservations.size(); i++)
		{
			resultsFile << SameClassCoveredObservations[i] + 1 << "   ";
			PosPatternCoverage[PosPatNumber].push_back(SameClassCoveredObservations[i]);

		}


		PosPatNumber = PosPatNumber + 1;

		
		
		resultsFile << endl;
		// end of steps that find the observations having same class as Alpha observation and covered by the generated Alpha pattern



	} // 2

	  // ACO ends here

	  ////////////////////////////////////////////////////////////////////End of generating patterns using ACO



	float 	StartPatterSetTime;
	StartPatterSetTime = clock();


	
} // 1  while loop , end of solving the problem

vector<int> CoveredObservationByPositivePattern;  // shows if the row is covered or not in this SCP


for (int i = 0; i < v_BinaryData.size(); i++)
{
	if (v_BinaryData[i][0] == 1 && v_IgnoredObservation[i] == 1)

		CoveredObservationByPositivePattern.push_back(i); // give a pseudo coverage to ignored observations
}


int MyCount2;

vector<int> ReducedPosPatternSet;



while (CoveredObservationByPositivePattern.size() < v_NumberOfPositiveObservation) // loop until all the constriants are covered
{
	// calculate the coverage of each attribute form the uncovered constraints and select the binary attribute that has the maximum coverage

	int CoverageOfBestPattern = -100;
	int SelectedPattern;

	vector<vector<int>> CurrentCoverOfPosPattern;

	for (int i = 0; i < v_PositivePatternSet.size(); i++)
		CurrentCoverOfPosPattern.push_back(vector<int>()); // define a row for each attribute


	for (int j = 0; j< v_PositivePatternSet.size(); j++)
	{
		MyCount2 = count(ReducedPosPatternSet.begin(), ReducedPosPatternSet.end(), j);
		if (MyCount2 == 0)
		{

			for (int i = 0; i < v_BinaryData.size(); i++)
			{
				if (v_BinaryData[i][0] == 1)
				{
					MyCount2 = count(CoveredObservationByPositivePattern.begin(), CoveredObservationByPositivePattern.end(), i);
					if (MyCount2 == 0)
					{
						MyCount2 = count(PosPatternCoverage[j].begin(), PosPatternCoverage[j].end(), i);
						if (MyCount2 == 1)
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



	for (int i = 0; i < CurrentCoverOfPosPattern[SelectedPattern].size(); i++)
	{
		CoveredObservationByPositivePattern.push_back(CurrentCoverOfPosPattern[SelectedPattern][i]); // state the covering of the constriants covered by the selected attribute by changing the their status from 0 to 1
	}

	// end of calculating the coverage of each attribute form the uncovered constraints and select the binary attribute that has the minimum 1/coverage


}


// end of selecting Pos pattern set

vector<int> CoveredObservationByNegativePattern;  // shows if the row is covered or not in this SCP

for (int i = 0; i < v_BinaryData.size(); i++)
{
	if (v_BinaryData[i][0] == 0 && v_IgnoredObservation[i] == 1)

		CoveredObservationByNegativePattern.push_back(i); // give a pseudo coverage to ignored observations

}


resultsFile << "ReducedPatternSet" << endl;


for (int i = 0; i < ReducedPosPatternSet.size(); i++)
{
	resultsFile << ReducedPosPatternSet[i] + 1 << "  ";
}
resultsFile << endl;



for (int i = 0; i <v_PositivePatternSet.size(); i++)
{
	MyCount2 = count(ReducedPosPatternSet.begin(), ReducedPosPatternSet.end(), i);
	if (MyCount2 == 1)
		ReducedPositivePatterns.push_back(v_PositivePatternSet[i]);
}


/// end of set covering to select a set of patterns


for (int i = 0; i <ReducedPosPatternSet.size(); i++)
{
	TotalCoveragePos = TotalCoveragePos + PosPatternCoverage[ReducedPosPatternSet[i]].size();
}

PatternCoverageFile << "Class " << v_ClassName << " ;  " << endl; // to save the coverage and weight of each pattern in a file
int counter = 1;
int counter2 = 0;
resultsFile << "Weights of the generated patterns; " << endl;

PosPatternWeight.push_back(vector<double>());

for (int i = 0; i < PosPatternCoverage.size(); i++)
{

	MyCount2 = count(ReducedPosPatternSet.begin(), ReducedPosPatternSet.end(), i);
	if (MyCount2 == 1)
	{
		PatternCoverageFile << "Pattern number " << counter << " ;  its weight is " << PosPatternCoverage[i].size() / TotalCoveragePos << " and its covered observations are : " << endl;
		counter = counter + 1;
		counter2 = counter2 + 1;
		for (int j = 0; j< PosPatternCoverage[i].size(); j++)
		{
			PatternCoverageFile << PosPatternCoverage[i][j] + 1 << "   ";
		}
		PatternCoverageFile << endl;
		PosPatternWeight[V_a].push_back(PosPatternCoverage[i].size() / TotalCoveragePos);
		resultsFile << i + 1 << "  " << PosPatternCoverage[i].size() / TotalCoveragePos << endl;
		PosPrevalence << "   " << v_ClassName << "           " << counter2 << "              " << PosPatternCoverage[i].size() << "                " << (double(PosPatternCoverage[i].size()) / double(v_NumberOfPositiveObservation)) << "                 " << (PosPatternCoverage[i].size() / TotalCoveragePos) << endl;

	}


}

resultsFile << " Total coverage of the generated patterns is: " << TotalCoveragePos << endl;
resultsFile << endl;
resultsFile << "--------------------------------" << endl;



return  ReducedPositivePatterns;
}

void PatternGeneration::FindDifferencesSameClass(vector<int>& ObservationAlpha_1, vector<vector<int>  >& BinaryData_1, vector<bool>& CoverdObservation_1, vector<bool>& IgnoredObservation_1, vector<vector<int>  > &DifferencesInSameClass_1)
{
	int my_count;
	for (int j = 1; j < BinaryData_1[1].size(); j++) // j starts form 1 to ignore the classification column (first column) given in the data 
	{
		for (int i = 0; i < BinaryData_1.size(); i++)
		{
			
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

void PatternGeneration::FindDifferencesOppositeClass(vector<int>& ObservationAlpha_1, vector<vector<int>  >& BinaryData_1, vector<bool>& CoverdObservation_1, vector<bool>& IgnoredObservation_1, vector<vector<int>  > &DifferencesInOppositeClass_1)
{
	int my_count;
	for (int j = 1; j < BinaryData_1[1].size(); j++)
	{
		for (int i = 0; i < BinaryData_1.size(); i++)
		{
		
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