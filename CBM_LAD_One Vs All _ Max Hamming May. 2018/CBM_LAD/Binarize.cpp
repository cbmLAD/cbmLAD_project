#include "StdAfx.h"
#include "Binarize.h"

Binarization::Binarization(void)
{
	//litNum=0;
}

Binarization::~Binarization(void)
{

}

ofstream CutPoints("C:\\cbmLAD\\Cut Points.txt", ios::out);
ofstream binaryFile("C:\\cbmLAD\\binary data.txt", ios::out);
bool sortcol(const vector<double>& v1, const vector<double>& v2) {
	return v1[0] < v2[0];
}
//Accepts a 2D vector of raw data and returns a 3D vector: For each attribute, there is
//a 2D vector that contains (for each observation) the value of the attribute, the class 
//type of the attribute, and the value type.

vector<vector<vector<double>>> Binarization::GenerateAttributeColumnVectors(vector<vector<double>> vRawData)
{
	vector<vector<vector<double>>> vNumericalAttributes;
	unsigned int iNumberofAttributes = (int)vRawData.at(0).size();

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
		for (unsigned int j = 1; j<vRawData.size(); j++)
		{
			vector<double> vSingleValueClassType_temp;

			//The attribute value
			vSingleValueClassType_temp.push_back(vRawData.at(j).at(i));

			//The class
			vSingleValueClassType_temp.push_back(vRawData.at(j).at(0));

			//The Binarization type
			vSingleValueClassType_temp.push_back(vRawData.at(0).at(i));

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


vector<vector<vector<double>>> Binarization::SortAttributeValues(vector<vector<vector<double>>> vAllAttributeColumns)
{
	vector<vector<vector<double>>> vSortedNumericalAttributes;
	vector<vector<double>> vSingleAttributeColumn;

	//For each attribute.
	for (unsigned int i = 0; i < vAllAttributeColumns.size(); i++)
	{
		vSingleAttributeColumn = vAllAttributeColumns.at(i);
		sort(vSingleAttributeColumn.begin(), vSingleAttributeColumn.end(), sortcol);
		vSortedNumericalAttributes.push_back(vSingleAttributeColumn);
	}
	return vSortedNumericalAttributes;
}


int Binarization::FindMinimalNumerical(vector<vector<double>> workingVector, int lastSortedElementOutput)
{
	//We initialize minVal at the first value in the vector
	vector <int> v_ret;
	// initialize to the first element in the vector containing the remaining observations
	// of a particular attribute. That vector has values in first column, class in second
	// and data type in third.
	double minVal = workingVector.at(0).at(0);
	int indexVect = 0;
	// indicate the class of the last element in the sorted observation list
	int lastInsertedElementOutput = lastSortedElementOutput;

	v_ret.push_back(indexVect);

	// will give the indices of the observations having minimal value
	for (unsigned int i = 0; i<workingVector.size(); i++)
	{
		if (workingVector.at(i).at(0)<minVal)
		{
			v_ret.erase(v_ret.begin(), v_ret.end());

			indexVect = i;
			v_ret.push_back(indexVect);
			minVal = workingVector.at(i).at(0);
		}
		else if (workingVector.at(i).at(0) == minVal && indexVect != i)
		{
			indexVect = i;
			v_ret.push_back(indexVect);
		}
	}

	// Return the index that maximises sign alternation by alternating the observations
	//that have same value but opposite signs
	int retIndex;
	bool indexFound = false;
	for (unsigned int i = 0; i<v_ret.size() && !indexFound; i++)
	{
		int elementToInsertSign = (int)workingVector.at(v_ret.at(i)).at(1);

		//If we have reached the last position in v_ret and no sign change is possible.
		if (lastInsertedElementOutput != elementToInsertSign || i + 1 == v_ret.size())
		{
			retIndex = v_ret.at(i);
			indexFound = true;
		}
	}
	return retIndex;
}

// returns the position of the observation with the smallest value at a given attribute
int Binarization::FindMinimalOrdered(vector <vector <double> > workingVector)
{
	//We initialize minVal at the first value in the vector
	double minVal = workingVector.at(0).at(0);
	int indexVect = 0;

	for (unsigned int i = 0; i<workingVector.size(); i++)
	{
		if (workingVector.at(i).at(0)<minVal)
		{
			indexVect = i;
			minVal = workingVector.at(i).at(0);
		}
	}
	return indexVect;
}

//Accepts a 3D vector: For each attribute, there is a 2D vector that contains (for 
//each observation) (1) the value of the attribute, (2) the class of the attribute, 
//and (3) the value type. Returns a vector of cutpoints for each attribute.
vector<vector<double>> Binarization::ExtractCutPoints(vector<vector<vector<double>>> v_SortedAttributesWithOutcome, int ClassName)
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
	CutPoints << "       Class  " << ClassName << endl;
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


vector<vector<int>> Binarization::GetBinaryTraining(vector<vector<double>> vCutPoints, vector<vector<double>> vRawData)
{
	

	vector<vector<double>> vObservations;
	vector<vector<int>> vBinarizationResult;

	// STORE IN 2D VECTOR vObservations THE ATTRIBUTE VALUES ONLY. NO DATA TYPES NO CLASSES.
	// START READING FROM SECOND ROW BECAUSE FIRST CONTAINS DATA ATTRIBUTE TYPES 
	for (unsigned int i = 1; i<vRawData.size(); i++)
	{
		vector <double> tmp;
		// START READING FROM SECOND COLUMN BECAUSE FIRST CONTAINS THE CLASSES
		for (unsigned int j = 1; j<vRawData.at(i).size(); j++)
		{
			tmp.push_back(vRawData.at(i).at(j));
		}
		vObservations.push_back(tmp);
	}

	// For each observation and each attribute value in each observation
	for (unsigned int i = 0; i<vObservations.size(); i++)
	{
		//For each observation, one vector will be used to hold all of it's attribute values.
		vector <int> vBinarizedObservation;

		// add +1 becoz first row is not observation
		vBinarizedObservation.push_back((int)vRawData.at(i + 1).at(0));

		for (unsigned int j = 0; j<vObservations.at(i).size(); j++)
		{
			//For each attribute analyze it with respect to the cutpoints.
			double attrValue = vObservations.at(i).at(j);

			//For observation i, attribute j, fetch the cutpoints that qualify the j'th attribute.
			vector <double> attribCutPoints = vCutPoints.at(j);

			//For attribute j, fetch the type of the attribute.
			// add +1 becoz first column is the outcome and means nothing in attribute types row
			int attrType = (int)vRawData.at(0).at(j + 1);
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


	for (unsigned int i = 0; i < vBinarizationResult.size(); i++)
	{
		for (unsigned int j = 0; j < vBinarizationResult.at(i).size(); j++)
		{
			binaryFile << vBinarizationResult.at(i).at(j) << "   ";
		}
		binaryFile << endl;
	}
	binaryFile.close();

	return vBinarizationResult;
}
