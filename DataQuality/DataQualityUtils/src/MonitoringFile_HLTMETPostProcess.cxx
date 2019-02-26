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
  MonitoringFile::HLTMETPostProcess( std::string inFilename, bool /* isIncremental */ ) 
  {
    bool dbgLevel = false;
    if(dbgLevel) std::cout << "--> HLTMETPostProcess: Begin HLTMET post-processing" << std::endl;

    //open root file
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");

    //check files are loaded.

    if (f == 0 || ! f->IsOpen() ) {
      std::cerr << "--> HLTMETPostProcess: Input file not opened" << std::endl;
      return;
    }

    if(f->IsZombie()) {
      std::cerr << "--> HLTMETPostProcess: Input file " << inFilename << " cannot be opened. " << std::endl;
      return;
    }
  
    //check file size is not too small.
    if(f->GetSize()<1000.) {
      std::cerr << "--> HLTMETPostProcess: Input file empty" << std::endl;
      f->Close();
      return; 
    }

    //start postprocessing
    TString run_dir;
    HLTMETAveragePhivsEtaMaps( f, run_dir);
    HLTMETDQFlagSummary(f, run_dir);
    //close root file
    f->Close();
    delete f;

    if(dbgLevel) std::cout << "--> HLTMETPostProcess: End HLTMET post-processing "<<std::endl;

    return;

  }

  /*unsigned int MonitoringFile::HLTMETGetDQLBNRange(TDirectory*& run_dir) {

    if(!run_dir) return 0;
    std::cout << "--> : HLTMETGetDQLBNRange: looking for lb_* directories inside " << run_dir->GetName() << " for [L2/EF]_MET_status flags." << std::endl;
    run_dir->cd();

    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(run_dir->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    //dir0->pwd(); //dir0->ls();
    dir0->cd();


    TIter next_lbn ( run_dir->GetListOfKeys() );
    TKey* key_lbn(0);

    // loop over all objects
    std::vector<TString> lbnDirs;
    while ((key_lbn = dynamic_cast<TKey*> ( next_lbn() )) !=0 ) {
      TObject* obj_lbn = key_lbn->ReadObj();
      TDirectory* tdir_lbn = dynamic_cast<TDirectory*>( obj_lbn );
      if (tdir_lbn ==0 ){ delete obj_lbn; continue; }
      TString lbn_dir = tdir_lbn->GetName();
      if (!lbn_dir.Contains("lb_") )  {delete obj_lbn; continue;}
      std::cout << "--> HLTMETGetDQLBNRange: found lbn directory named " << lbn_dir << std::endl;
      lbnDirs.push_back(lbn_dir);
    }
    return lbnDirs.size();
  }*/



}//namespace

