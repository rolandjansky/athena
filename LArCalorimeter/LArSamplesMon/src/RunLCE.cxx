/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#include "LArSamplesMon/LArCellsEmptyMonitoring.h"
#include <vector>
#include <string> 
#include <iostream>

#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "Cintex/Cintex.h"
#endif

using namespace LArSamples;
using namespace std;

//void RunLCE(const char* inputFile, const char* outputPath, const char* defectLBs, int minCell, int maxCell);

//int main(int argc, char** argv) {
//
//  if (argc<3 || (argc>1 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")))) {
//    std::cout << "Syntax:" << std::endl;
//    std::cout << "RunLCE.exe inFile outFile <defectLBfile>" << std::endl;
//    return -1;
//  }
//    
//  char* defectsLBFileName=nullptr;
//  if (argc>3)
//    defectsLBFileName=argv[3];
//
//  TROOT root ("root", "root");
//#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
//  ROOT::Cintex::Cintex::Enable();
// #endif
// 
//  gSystem->Load("libLArCafJobsDict.so");
//  gSystem->Load("libLArSamplesMonDict.so");
//  
//  RunLCE(argv[1],argv[2],defectsLBFileName,0,0);
//
//  return 0;
//}





void RunLCE(const char* inputFile, const char* defectLBFN)
{

  //  TString workdir = gSystem->Getenv("WORKDIR");
  //  if (workdir == "") 
  //  {
  //  TString user = gSystem->Getenv("USER");
  //  workdir = "/tmp/" + user;
  //  }
  //  TString inputFile = workdir + "/sample_" + runNumber + ".root";

  if (gSystem->AccessPathName(inputFile)) 
    {
      cout << "Cannot access file " << inputFile << ", exiting from RunLCE.C" << endl;
      return;
    }

  // main object - checks LArIdTranslatorHelper within needs LarIdTree.root in $PWD
  LArSamples::LArCellsEmptyMonitoring* lce = new LArSamples::LArCellsEmptyMonitoring();
  
  lce->SetSelectRecurringBadCells();
  lce->SetSaveRootFile(0);
  lce->SetSaveTextFile(1);
  lce->SetAlgo(2);
  lce->SetLumiblockRange();
  lce->SetBadLBRemovalOption(0);
  lce->SetQthreshold();
  lce->SetNoiseNsigma();
  lce->SetQthreshold();
  lce->SetEnergyCountThresholds(1.,50,1,20,10);
  //lce->SetCellRange(minCell,maxCell);

  // If using ntuples which have had defect / non-ATLAS ready LBs removed at the merging stage, use:
  if (defectLBFN) 
    lce->ReadDefectLBList(true,defectLBFN);

  // else read in the defect list manually using:
  //  lce->ReadDefectLBList(1,"/tmp/larmon/DQ.list.defects.dat");
  //  lce->ScanOnlids("/afs/cern.ch/user/o/osimard/scratch0/samples_larCellsEmpty_166466_merged.root");
  printf("==\n");
  //lce->Run(inputFile,outputPath);
  lce->Run(inputFile);

  return;
}
