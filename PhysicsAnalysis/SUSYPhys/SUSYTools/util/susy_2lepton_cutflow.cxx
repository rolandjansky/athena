/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "SUSYTools/Reco2Lep.h"

using namespace std;

int main(int argc, char *argv[]){

	cout << "-----------------------------------------\n";
	cout << "--- Welcome to SUSY 2-Lepton Cutflow! ---\n";
	cout << "-----------------------------------------" << endl;

	// Check Input
	if(argc!=4){ 
		cout<< "Usage: susy_2lepton_cutflow mode confFile inputFile1,inputFile2,inputFileN" << endl; 
		cout<< "       where mode = 0 for data;" << endl;
		cout<< "                  = 1xxx for MC;" << endl;
		cout<< "                     ^^^" << endl;
		cout<< "                     |||---- lepton scaling ? 1:0 " << endl;
		cout<< "                     ||----- trigger weighting ? 1:0 " << endl;
		cout<< "                     |------ pileup ? 1:0" << endl;
		exit(1); 
	} 

	int mode=atoi(argv[1]);
	int kIsData;
	bool pileup=false;
	bool trgwgt=false;
	bool lsf=false;

	// Parse mode
	int leftover =  mode;
	if(mode>=1000){
		kIsData = 0;
		leftover-=1000;
		if(leftover>=100){
			leftover-=100;
			pileup = true;
		}
		if(leftover>=10){
			leftover-=10;
			trgwgt = true;
		}
		if(leftover==1) lsf = true;
		else if(leftover!=0){
			cout << "Unknown mode!" << endl;
			exit(1);
		}
	} else {
		if(mode==0) kIsData = 1;
		else{
			cout << "Unknown mode!" << endl;
			exit(1);
		}
	}

	// Additional parameters - Edit the following accordingly
	// --- Config files should have 7 lines of extra input.
	// --- See ~rueno/public/2lep_config.txt as an example
	string outputFile;
	string elPileUpFile1, elPileUpFile2, muPileUpFile1, muPileUpFile2;
	string trgwgtFile, grlFile;

	string configFile=argv[2];
	string line;
	ifstream paramfile(configFile.c_str());
	if(paramfile.is_open()){
		getline(paramfile, outputFile, ' '); 	getline(paramfile, line);
		getline(paramfile, elPileUpFile1, ' ');   getline(paramfile, line);
		getline(paramfile, elPileUpFile2, ' ');   getline(paramfile, line);
		getline(paramfile, muPileUpFile1, ' ');   getline(paramfile, line);
		getline(paramfile, muPileUpFile2, ' ');   getline(paramfile, line);
		getline(paramfile, trgwgtFile, ' ');      getline(paramfile, line);
		getline(paramfile, grlFile, ' ');         getline(paramfile, line);
	    
		cout << "outputFile=" << outputFile << endl;
		cout << "elPileUpFile1=" << elPileUpFile1 << endl;
		cout << "elPileUpFile2=" << elPileUpFile2 << endl;
		cout << "muPileUpFile1=" << muPileUpFile1 << endl;
		cout << "muPileUpFile2=" << muPileUpFile2 << endl;
		cout << "trgwgtFile=" << trgwgtFile << endl;
		cout << "grlFile=" << grlFile << endl;

		cout << "\nSuccessfully read the parameter file!" << endl;
	} else cout << "Unable to open file" << endl;
	  
	if(kIsData == 1)
		cout << "---Using Configuration: Data" << endl;
	else 
		cout << "---Using Configuration: MC with: Pileup=" << pileup <<" / Trigger Weighting=" << trgwgt << " / Lepton Scaling=" << lsf << endl;


	string inFiles=argv[3];
//	cout << "This is the input file list: " << inFiles << endl;
//	cout << "Just pass it to Test_dpd and Reco2Lep..." << endl;

//	cout << "First part: Check if file can be read\n" << endl;
//	Test_dpd(inFiles,0,0);

//	cout << "Second part: Produce cutflow\n" << endl;

	Reco2Lep t;
	t.SetParams(kIsData, pileup, trgwgt, lsf, 
					outputFile, elPileUpFile1, elPileUpFile2, muPileUpFile1,
					muPileUpFile2, trgwgtFile, grlFile);
	t.Loop(inFiles);
	
	return 0;
}
