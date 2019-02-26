/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/* Methods to perform post-processing on run_nnnnnn/HLT/METMon* histograms
 * Author   : Venkatesh Kaushik (venkat.kaushik@cern.ch)
 * Date     : Feb 2010
 */

#include "DataQualityUtils/MonitoringFile.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <string>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"
#include "TBranch.h"



namespace dqutils {

  void 
  MonitoringFile::HLTJetPostProcess( std::string inFilename, bool /* isIncremental */ ) 
  {
    bool dbgLevel = false;

    if(dbgLevel) std::cout << "--> HLTJetPostProcess: Begin HLTJet post-processing" << std::endl;

    //open root file
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");

    //check files are loaded.

    if (f == 0 || ! f->IsOpen() ) {
      std::cerr << "--> HLTJetPostProcess: Input file not opened" << std::endl;
      return;
    }

    if(f->IsZombie()) {
      std::cerr << "--> HLTJetPostProcess: Input file " << inFilename << " cannot be opened. " << std::endl;
      return;
    }
  
    //check file size is not too small.
    if(f->GetSize()<1000.) {
      std::cerr << "--> HLTJetPostProcess: Input file empty" << std::endl;
      f->Close();
      return; 
    }

    //start postprocessing
    TString run_dir;
    HLTJetCalcEfficiencyAndRate( f, run_dir);
    //close root file
    f->Close();
    delete f;

    if(dbgLevel) std::cout << "--> HLTJetPostProcess: End HLTJet post-processing "<<std::endl;

    return;

  }

}//namespace

