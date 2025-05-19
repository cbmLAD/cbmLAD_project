#include "Clean_Data.h"



Clean_Data::Clean_Data()
{
}


Clean_Data::~Clean_Data()
{
}

vector < vector <double> > Clean_Data::cleanData(vector < vector <double> >& inputVec) {

	bool contradictionFlag = true;
	bool contradictionsExist = false;
	bool displayMessage = false;
	bool done = false;
	vector<unsigned int> alreadyChecked;
	vector<vector<double>>::iterator It = inputVec.begin();
	ofstream contradictions;
	contradictions.open("C:\\cbmHany\\Multi class\\ObservationsToBeChecked.txt");

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
