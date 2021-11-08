/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* HLTCalo Post Processing on run_nnnn/HLT/HLTcalo histogram 
 * Author   : Gareth Brown <gabrown@CERN.CH> based on HLTMET Post Processing
 * Date     : Oct 2011  
 */

#include "DataQualityUtils/MonitoringFile.h"
#include <iostream>
#include "TFile.h"
#include "TString.h"

namespace dqutils {

  void MonitoringFile::HLTCaloPostProcess( const std::string & inFilename, bool isIncremental  ) {
    if (isIncremental == true) {
      return;
    }

    int debugLevel = MonitoringFile::getDebugLevel();
    if(debugLevel>1){
      std::cout << "--> HLTCaloPostProcess: Begin HLTCalo post-processing" << std::endl;
    }

    //open root file
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");

    //check files are loaded.
    if (f == 0 || ! f->IsOpen() ) {
      std::cerr << "--> HLTCaloPostProcess: Input file not opened" << std::endl;
      return;
    }

    if(f->IsZombie()) {
      std::cerr << "--> HLTCaloPostProcess: Input file " << inFilename << " cannot be opened. " << std::endl;
      return;
    }

    //check file size is not too small.
    if(f->GetSize()<1000.) {
      std::cerr << "--> HLTCaloPostProcess: Input file empty" << std::endl;
      f->Close();
      return; 
    }

    //start postprocessing
    TString run_dir;
    HLTCaloAveragePtPhiEtaMaps( f, run_dir);
    //close root file
    f->Close();
    delete f;

    if(debugLevel>1){
      std::cout << "--> HLTCaloPostProcess: End HLTCalo post-processing "<<std::endl;
    }

    return;
  }

}//namespace

