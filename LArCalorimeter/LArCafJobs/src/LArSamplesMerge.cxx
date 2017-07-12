
/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*- 

#include "LArCafJobs/PersistentAccessor.h" 
#include <vector>
#include <string> 
#include <iostream>

#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"

using namespace LArSamples;

int main(int argc, char** argv) {

  if (argc<3 || (argc>1 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")))) {
    std::cout << "Syntax:" << std::endl;
    std::cout << "LArSamplesMon.exe file1 file2 ... outFile" << std::endl;
    return -1;
  }
    

  TROOT root ("root", "root");
  gSystem->Load("libLArCafJobsDict.so");

  std::vector<TString> inFileNames;
  std::string outFileName(argv[argc-1]);

  int result=0;

  for (int i=1;i<argc-1;++i) 
    inFileNames.emplace_back(argv[i]);


  std::vector<const PersistentAccessor*> accessors;

  for (const TString& fileName : inFileNames) {
    PersistentAccessor* acc=PersistentAccessor::open(fileName);
    if (!acc) {
      std::cout << "ERROR failed to open " << fileName << std::endl;
      result=-1;
    }
    else {
      std::cout << "Open file " << fileName << " with " << acc->nEvents() << " events." << std::endl;
      accessors.push_back(acc);
    }
  }
  
  const PersistentAccessor* output=PersistentAccessor::merge(accessors,outFileName);
  if (!output) {
    std::cout << "ERROR: Failed to merge files" << std::endl;
    result=-2;
  }
  else
    std::cout << "Wrote output file " << outFileName << " with " << output->nEvents() << " events." << std::endl;

  return result;
}
  


