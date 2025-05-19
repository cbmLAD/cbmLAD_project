#include "Pattern_Interpretation.h"



Pattern_Interpretation::Pattern_Interpretation()
{
}


Pattern_Interpretation::~Pattern_Interpretation()
{
}

ofstream Patternfile("C:\\cbmLAD\\Patterns Interpreted.txt", ios::out);

vector<vector<vector<double>>> Pattern_Interpretation::Interpret2ClassPatterns(vector<string> attribue_Names, vector<vector<int>> vPatterns, vector<vector<double>> vCutPoints, int ClassName)
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
	Patternfile << "  Class     " << ClassName << endl;
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

