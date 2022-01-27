/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Methods to perform post-processing on run_nnnnnn/Muon/MuonSegmentMonitoring/EndCapN/Detail/<Collection>/CSC* histograms
 * Author   : Jason Veatch (jason.veatch@cern.ch)
 * Date     : March 2011
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

  static const bool csc_debug = false;

  void 
  MonitoringFile::CSCPostProcess( const std::string & inFilename, bool /* isIncremental */ ) 
  {
    if(csc_debug) std::cout << "--> CSCPostProcess: Begin CSC post-processing" << std::endl;

    //open root file
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");

    //check files are loaded.

    if (f == 0 || ! f->IsOpen() ) {
      std::cerr << "--> CSCPostProcess: Input file not opened" << std::endl;
      return;
    }

    if(f->IsZombie()) {
      std::cerr << "--> CSCPostProcess: Input file " << inFilename << " cannot be opened. " << std::endl;
      return;
    }
  
    //check file size is not too small.
    if(f->GetSize()<1000.) {
      std::cerr << "--> CSCPostProcess: Input file empty" << std::endl;
      f->Close();
      return; 
    }

    //start postprocessing
    TString run_dir;
    CSCChamberEfficiency( f, run_dir);
    CSCClusterFit( f, run_dir);
    //close root file
    f->Close();
    delete f;

    if(csc_debug) std::cout << "--> CSCPostProcess: End CSC post-processing "<<std::endl;

    return;

  }

}//namespace

