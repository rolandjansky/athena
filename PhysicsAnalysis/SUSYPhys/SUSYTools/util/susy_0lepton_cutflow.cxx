/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"
#include <iostream>
#include <stdlib.h>

#include "SUSYTools/Reco0Lep.h"
#include "../macros/Test_dpd.C"

using namespace std;

int main(int argc, char *argv[])
{
  if(argc!=3){ cout<<"Usage: susy_0lepton_cutflow.exe ana(=2 cutA, 3 cutB, 4 cutC, 5cutD, 6 cutE, 7 all) inputFile1,inputFile2,inputFileN"<<endl; exit(1); } 
  int ana=atoi(argv[1]);
  std::string inFiles=argv[2];
//   cout<<"This is the input file list: "<<inFiles<<endl;
//   cout<<"Will perform the 0 lepton analysis (V0543) For Cut Flow computation"<<endl;

//   cout << "First part: Check if file can be read" << endl << endl;

  Test_dpd(inFiles,0,0);

//   cout << endl << "Second part: Produce 0 lepton cutflow" << endl << endl;

  Reco0Lep t;
	if(ana<7)  t.Loop(ana,inFiles);
	else {
	  t.Loop(2,inFiles);
	  t.Loop(3,inFiles);
  	  t.Loop(4,inFiles);
  	  t.Loop(5,inFiles);
  	  t.Loop(6,inFiles);
             }
  return 0;
}
