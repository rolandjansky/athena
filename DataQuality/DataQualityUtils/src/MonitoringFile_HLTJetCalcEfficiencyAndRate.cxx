/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Methods to perform post-processing on run_nnnnnn/HLT/JetMon* histograms
 * Author   : Venkatesh Kaushik (venkat.kaushik@cern.ch)
 * Date     : Apr 2010
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
  MonitoringFile::HLTJetCalcEfficiencyAndRate(TFile* f, TString& run_dir)
  {

    bool dbgLevel = false;
    if(dbgLevel) std::cout << "--> HLTJetCalcEfficiencyAndRate: Calculate jet trigger efficiency and rate" << std::endl;

    f->cd("/");
    TIter next_run ( f->GetListOfKeys() );
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
      if (!key_run->IsFolder()) continue;
      run_dir = key_run->GetName();
      if (!run_dir.Contains("run") )  { continue;}


      std::string run_dir2 = run_dir.Data();
      //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
      //run_number=run_number;


      TString jetmon_dir = run_dir + "/HLT/JetMon";

      //===HLTJet efficiency histograms
      TString akt4topo_dir = jetmon_dir + "/AntiKt4TopoJets/TrigEff";
      //TString akt6topo_dir = jetmon_dir + "/AntiKt6TopoJets/TrigEff";
      
      TDirectory* dir(0);
      
      if( !(dir = f->GetDirectory(akt4topo_dir)) ) {
        std::cerr << "--> HLTJetCalcEfficiencyAndRate: directory " << akt4topo_dir << " not found." << std::endl;
        return;
      }

      TH1F* hnum(0);
      TH1F* hden(0);


      //==Efficiency
      std::vector<TString> effobs;
      effobs.push_back("_Eff_vs_pt");
      effobs.push_back("_Eff_vs_eta");
      effobs.push_back("_Eff_vs_phi");
      
      std::vector<TString> TrigItems;

      // EF_fj30(_a4_EFFS)  <-- L2_fj25 <-- L1_FJ10
      // EF_j30(_a4_EFFS)   <-- L2_j25 <-- L1_J10
      // EF_j240(_a4_EFFS)  <-- L2_j95 <-- L1_J75

      TrigItems.push_back("EF_fj30");
      TrigItems.push_back("EF_j30");
      TrigItems.push_back("EF_j240");
      //TrigItems.push_back("EF_fj50");
      //TrigItems.push_back("EF_fj75");

      //TrigItems.push_back("EF_j50");
      //TrigItems.push_back("EF_j75");
      //TrigItems.push_back("EF_j95");

      TrigItems.push_back("L1_FJ10");
      TrigItems.push_back("L1_J10");
      TrigItems.push_back("L1_J75");
      //TrigItems.push_back("L1_FJ55");
      //TrigItems.push_back("L1_FJ95");

      //TrigItems.push_back("L1_J30");
      //TrigItems.push_back("L1_J55");
      //TrigItems.push_back("L1_J75");

      //TrigItems.push_back("L2_fj45");
      //TrigItems.push_back("L2_fj70");

      TrigItems.push_back("L2_fj25");
      TrigItems.push_back("L2_j25");
      TrigItems.push_back("L2_j95");
      //TrigItems.push_back("L2_j30");
      //TrigItems.push_back("L2_j45");
      //TrigItems.push_back("L2_j70");
      //TrigItems.push_back("L2_j90");

      TString snum, sden, hnumname;
      for( std::vector<TString>::iterator itT = TrigItems.begin(); itT != TrigItems.end(); ++itT ) {
        for( std::vector<TString>::iterator itO = effobs.begin(); itO != effobs.end(); ++itO ) {

          hnumname = (*itT) + (*itO) + "_num";
          snum   = akt4topo_dir + "/" + hnumname;
          sden   = akt4topo_dir + "/" + (*itT) + (*itO) + "_den";

          if( ! f->Get(snum) ){if(dbgLevel) std::cerr <<"--> HLTJetPostProcess: no such histogram "<< snum   << std::endl; }
          if( ! f->Get(sden) ){if(dbgLevel) std::cerr <<"--> HLTJetPostProcess: no such histogram "<< sden   << std::endl; }

          if( f->Get(snum) && f->Get(sden) ){

            hnum = dynamic_cast<TH1F*>( f->Get(snum) );
            hnum->Sumw2();
            hden = dynamic_cast<TH1F*>( f->Get(sden) );
            hden->Sumw2();

            Int_t nbins_num = hnum->GetNbinsX();
            Int_t nbins_den = hden->GetNbinsX();
            if(nbins_num != nbins_den) {
              if(dbgLevel) std::cerr <<"--> HLTJetPostProcess: cannot divide histogram "<< hnum->GetName() 
                << " by " << hden->GetName() << ". Different number of bins." <<  std::endl;
            }

            // divide num, den with binomial errors
            // note: replacing the numerator by quotient
            hnum->Divide(hnum, hden, 1., 1., "B");

            dir->cd();
            hnum->Write("",TObject::kOverwrite);
          }
        } // for effobs
      } // for TrigItems

      /*
      if( !(dir = f->GetDirectory(akt6topo_dir)) ) {
        std::cerr << "--> HLTJetCalcEfficiencyAndRate: directory " << akt6topo_dir << " not found." << std::endl;
        return;
      }

      for( std::vector<TString>::iterator itT = TrigItems.begin(); itT != TrigItems.end(); ++itT ) {
        for( std::vector<TString>::iterator itO = effobs.begin(); itO != effobs.end(); ++itO ) {
          hnumname = (*itT) + (*itO) + "_num";
          snum   = akt6topo_dir + "/" + hnumname;
          sden   = akt6topo_dir + "/" + (*itT) + (*itO) + "_den";

          if( ! f->Get(snum) ){std::cerr <<"--> HLTJetPostProcess: no such histogram "<< snum   << std::endl; }
          if( ! f->Get(sden) ){std::cerr <<"--> HLTJetPostProcess: no such histogram "<< sden   << std::endl; }

          if( f->Get(snum) && f->Get(sden) ){
            hnum = dynamic_cast<TH1F*>( f->Get(snum) );
            hnum->Sumw2();
            hden = dynamic_cast<TH1F*>( f->Get(sden) );
            hden->Sumw2();

            Int_t nbins_num = hnum->GetNbinsX();
            Int_t nbins_den = hden->GetNbinsX();
            if(nbins_num != nbins_den) {
              std::cerr <<"--> HLTJetPostProcess: cannot divide histogram "<< hnum->GetName() 
                << " by " << hden->GetName() << ". Different number of bins." <<  std::endl;
            }
            // note: replacing the numerator by quotient
            hnum->Divide(hnum, hden, 1., 1., "B");
            dir->cd();
            hnum->Write("",TObject::kOverwrite);
          }
        } // for effobs
      } // for TrigItems
      */

    }//while
  }//MonitoringFile::HLTJetCalcEfficiencyAndRate

  // -------------------------------------------------------------------------------------------------------
  // Compute bin errors a la Ullrich, Paterno and Xu (UPX) variance
  // http://arxiv.org/abs/physics/0701199
  // Input : Numerator, Denominator histograms and an empty std::vector<float> errors
  // Return Value(s): Errors including Overflow/Underflow bins are returned in std::vector<float> errors
  //                  Statuscode is returned as an int
  // StatusCode = -1 => either numerator or denominator histogram doesn't exist
  //            = -2 => number of bins in numerator and denominator are not equal
  //            = -3 => denominator has zero entries
  //            = -4 => at least one of the bins has negative entries
  //            =  1 => ALL OK
  // Alternative to "Binomial" variance used by TH1::Divide option "B"
  // Thanks to D. Casadei for his macro which is implemented here with minor modifications
  // -------------------------------------------------------------------------------------------------------
  int MonitoringFile::ComputeUPXBinErrors(TH1F *hnum, TH1F *hden, std::vector<float>& errors) {

    if(hnum==0 || hden==0) return -1;

    Int_t nbins_num = hnum->GetNbinsX();
    Int_t nbins_den = hden->GetNbinsX();

    if(nbins_num != nbins_den) return -2;

    if(hden->GetEntries() == 0) return -3;

    errors.clear();
    for (Int_t i = -1; i <= nbins_num; ++i ) { // NB: loop includes under- & overflow
      float n = hden->GetBinContent(i);
      if (n==0) continue;
      float k = hnum->GetBinContent(i);
      if(k<0||n<0){
        if(MonitoringFile::getDebugLevel()>0)std::cerr << "--> ComputeUPXBinErrors : ERROR: found negative entries in bin " << i 
          <<  " for histograms " << hnum->GetName() << " , " << hden->GetName() << std::endl;
        break;
      }
      float num1 = (k+1)*(k+2);
      float den1 = (n+2)*(n+3);
      float num2 = (k+1)*(k+1);
      float den2 = (n+2)*(n+2);
      float variance = num1/den1 - num2/den2;
      float err = sqrt(variance);
      errors.push_back(err);
    }
    Int_t num_err = (Int_t) errors.size();
    int retval = 1;
    if(num_err != nbins_num) retval = -4;
    return retval;
  }

}//namespace
