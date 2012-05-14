/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TROOT.h"
#include <iostream>
#include <stdlib.h>

#include "SUSYTools/Reco_dpd2.h"
#include "../macros/Test_dpd.C"

using namespace std;

int main(int argc, char *argv[])
{
  if(argc!=2){ cout<<"Usage: susy_1lepton_cutflow.exe inputFile1,inputFile2,inputFileN"<<endl; exit(1); } 
  std::string inFiles=argv[1];
  cout<<"This is the input file list: "<<inFiles<<endl;
  cout<<"Just pass it to Test_dpd and Reco_dpd2..."<<endl;

  cout << "First part: Check if file can be read" << endl << endl;

  Test_dpd(inFiles,0,0);

  cout << endl << "Second part: Produce cutflow" << endl << endl;

  Reco_dpd2 t;
  t.Loop(inFiles);

  return 0;
}
