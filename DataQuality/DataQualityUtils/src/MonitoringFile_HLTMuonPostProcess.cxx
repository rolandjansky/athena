/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
//Methods to Process HLTMuon Histograms after merging.
//Author: Akimasa Ishikawa (akimasa.ishikawa@cern.ch)
//Date:   Jan 2009
////////////////////////////////////////////////////////////////////////////////////////////////////

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

  static const bool fpdbg = false;
  // static const bool fpdbg = true;

  void 
  MonitoringFile::HLTMuonPostProcess( const std::string & inFilename, bool /* isIncremental */ ) 
  {
    if (fpdbg) std::cout << "Start HLTMuon post-processing" << std::endl;

    //start postprocessing
    TString run_dir;
    HLTMuonHistogramDivision(inFilename , run_dir);

    //trigger overlap matrix
    HLTMuonTriggerOverlapMatrix(inFilename , run_dir);

    if (fpdbg) std::cout << "Finish HLTMuon post-processing"<<std::endl;

    return;
  }//MonitoringFile::HLTMuonPostProcess

  bool
  MonitoringFile::HLTMuonCheckHistogram(TFile* f, TString& hname)
  {
    if (!(f->Get(hname))) {
      if (fpdbg) {
	std::cerr << "HLTMuon PostProcessing: no such histogram!! : "<< hname << std::endl;
	gDirectory->pwd();
	gDirectory->ls();
      }
      return false;
    }
    return true;
  }//MonitoringFile::HLTMuonCheckHistogram

  void
  MonitoringFile::HLTMuonTriggerOverlapMatrix(const std::string & inFilename, TString& run_dir)
  {
    if (fpdbg) std::cout << "  Start to fill HLTMuon Trigger Overlap Matrix" << std::endl;

    PostProcessorFileWrapper mf( inFilename , "HLT Trigger Overlap Matrix");
    if (!mf.IsOpen()) {
      std::cerr << "HLTMuonPostProcess(): "
		<< "Input file not opened \n";
      return;
    }
    if(mf.GetSize()<1000.) {
      std::cerr << "HLTMuonPostProcess(): "
		<< "Input file empty \n";
      return; 
    }
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    dir0->cd();
    ///

    TIter next_run (mf.GetListOfKeys());
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) { //== the while commented out at$
      if (!key_run->IsFolder()) continue;
      run_dir = key_run->GetName();
      if (!run_dir.Contains("run") )  { continue;}
      break;
    }
    if (! run_dir.Contains("run") ) {
      std::cerr << "HLTMuon: unable to find run directory ..." << std::endl;
      return;
    }
    int times = 1;
    while (times--) {  // just once
      //run_dir = dir0->GetName();
      if (fpdbg) {
	std::cout << "HLTMuon: run directory is " << run_dir << std::endl;
      }

      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      //run_number=run_number;

      //===HLTMuon
      // TString muon_dir = run_dir + "/HLT/MuonMon/";
      TString muon_dir = "/" + run_dir + "/HLT/MuonMon/";

      TString cm_dir = muon_dir + "Common/";

      TString sol = cm_dir + "Trigger_Overlap";
      TString solpt = cm_dir + "Trigger_Overlap_Including_Passthrough";
      TString solf = cm_dir + "Trigger_Overlap_Fraction";
      TString solfpt = cm_dir + "Trigger_Overlap_Fraction_Including_Passthrough";

      TH2F* hol(0);
      TH2F* holpt(0);
      TH2F* holf(0);
      TH2F* holfpt(0);

      //Matrix
      TDirectory* dir = mf.GetDirectory(cm_dir);
      // if (fpdbg) { std::cerr << "matrix directory: " << cm_dir << std::endl; }
      if(!dir){
	if (fpdbg) {
	  std::cerr<< "HLTMuonTriggerOverlapMatrix: directory "<<cm_dir<<" not found"<<std::endl;
	}
        return;
      }
      
      mf.get( sol, hol );
      // if (fpdbg) { std::cerr << "matrix file: " << sol << std::endl; }
      if (!hol) {
	if (fpdbg) {
	  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< sol << std::endl;
	}
	continue;
      }
      mf.get( solpt, holpt );
      if (!holpt) {
	if (fpdbg) {
	  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< solpt << std::endl;
	}
	continue;
      }
      mf.get( solf, holf );
      if (!holf) {
	if (fpdbg) {
	  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< solf << std::endl;
	}
	continue;
      }
      mf.get( solfpt, holfpt );
      if (!holfpt) {
	if (fpdbg) {
	  std::cerr <<"HLTMuon PostProcessing: no such histogram!! "<< solfpt << std::endl;
	}
	continue;
      }

      if( hol && holpt && holf && holfpt ){
        holf->Reset();
        holfpt->Reset();

        for( int i = 0 ; i < hol->GetXaxis()->GetNbins(); i ++ ){

          Float_t diag   = hol->GetBinContent(i+1, i+1);
          Float_t diagpt = holpt->GetBinContent(i+1, i+1);
          
          for( int j = 0 ; j < hol->GetXaxis()->GetNbins(); j ++ ){

            Float_t offdiag   = hol->GetBinContent(i+1, j+1);
            Float_t offdiagpt = holpt->GetBinContent(i+1, j+1);

            Float_t frac  = 0.;
            Float_t efrac = 0.;

            if( diag != 0. ){
              frac = offdiag/diag;
              efrac = sqrt(frac*(1.-frac)/diag);
            }

            Float_t fracpt  = 0.;
            Float_t efracpt = 0.;

            if( diagpt != 0. ){
              fracpt  = offdiagpt/diagpt;
              efracpt = sqrt(fracpt*(1.-fracpt)/diagpt);
            }

            holf->SetBinContent(i+1,j+1, frac);
            holf->SetBinError(i+1,j+1, efrac);
      
            holfpt->SetBinContent(i+1,j+1, fracpt);
            holfpt->SetBinError(i+1,j+1, efracpt);
      
          }//j
        }//i

        dir->cd();
        holf->Write("",TObject::kOverwrite);
        holfpt->Write("",TObject::kOverwrite);

      }//if
      mf.Write();

    }//while
  }//HLTMuonTriggerOverlapMatrix

}//namespace

