/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* Methods to perform post-processing on run_nnnnnn/Muon/MuonSegmMonitoring/EndCapN/Detail/<Collection>/CSC* histograms
 * Author   : Jason Veatch (jason.veatch@cern.ch)
 *            Venkatesh Kaushik (venkat.kaushik@cern.ch)
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

  void
  MonitoringFile::CSCChamberEfficiency(TFile* f, TString& run_dir) {

    bool dbgLevel = false;
    if(dbgLevel) std::cout << "--> CSCChamberEfficiency: Calculate chamber efficiency by layer" << std::endl;
    std::string endc_label[2] = {"A","C"};
    std::string coll_dir_label[1] = {"MuonSegments"};
    std::string coll_hist_label[1] = {"Muon"};

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

      for(std::size_t iec = 0; iec < 2; iec++){
        for(std::size_t icoll = 0; icoll < 1; icoll++){

          TString csc_dir = run_dir + "/Muon/MuonSegmentMonitoring/EndCap" + endc_label[iec];

          //===CSC detail directory
          TString detail_dir = csc_dir + "/Detail/" + coll_dir_label[icoll] + "/CSC";
          
          //===CSC overview directory
          TString oview_dir = csc_dir + "/Overview/" + coll_dir_label[icoll] + "/CSC";

          TDirectory* dir(0);

          if( !(f->GetDirectory(detail_dir)) ) {
            std::cerr << "--> CSCChamberEfficiency: directory " << detail_dir << " not found." << std::endl;
            return;
          }

          if( !(dir = f->GetDirectory(oview_dir)) ) {
            std::cerr << "--> CSCChamberEfficiency: directory " << oview_dir << " not found." << std::endl;
            return;
          }
          
          //===CSC NClus segment histogram
          TString nclust_histo = detail_dir + "/" + coll_hist_label[icoll] + "_Segm_NumNClusSegments_E" + endc_label[iec];

          //===CSC efficiency histogram
          TString eff_histo = oview_dir + "/" + coll_hist_label[icoll] + "_Segm_Efficiency_E" + endc_label[iec];

          if( ! f->Get(nclust_histo) ){
            if(dbgLevel) std::cerr << "--> CSCChamberEfficiency: no such histogram " << nclust_histo   << std::endl;
            return;
          }

          if( ! f->Get(eff_histo) ){
            if(dbgLevel) std::cerr << "--> CSCChamberEfficiency: no such histogram " << eff_histo   << std::endl;
            return;
          }

          TH2F *h2 = (TH2F *) f->Get(nclust_histo);
          TH1F *h1_eff = (TH1F *) f->Get(eff_histo);
            
          for(int layer = 1; layer < 5; layer++){

            if(iec == 0){                                   // Endcap A
              for(int chamber = 1; chamber < 17; chamber++){

                float num = h2->GetBinContent(46,1+5*chamber);
                float num_err = h2->GetBinError(46,1+5*chamber);
                
                float den = h2->GetBinContent(10*layer-4,1+5*chamber);
                float den_err = h2->GetBinError(10*layer-4,1+5*chamber);
                
                float eff = 0., err = 0.;

                GetCscEffAndErr(num,num_err,den,den_err,eff,err,0);
                h1_eff->SetBinContent(5*chamber+layer-4,eff);
                h1_eff->SetBinError(5*chamber+layer-4,err);
              } // for loop over bins
            } else {                                              // Endcap C
              for(int chamber = 1; chamber < 17; chamber++){
                float num = h2->GetBinContent(46,86-5*chamber);
                float num_err = h2->GetBinError(46,86-5*chamber);
                
                float den = h2->GetBinContent(10*layer-4,86-5*chamber);
                float den_err = h2->GetBinError(10*layer-4,86-5*chamber);
                
                float eff = 0., err = 0.; 
                GetCscEffAndErr(num,num_err,den,den_err,eff,err,0);
                h1_eff->SetBinContent(90-layer-5*chamber,eff);
                h1_eff->SetBinError(90-layer-5*chamber,err);
              } // for loop over bins
            } // Endcap C
          } // for layers

          //h1_eff->Sumw2();

          dir->cd();
          h1_eff->Write("",TObject::kOverwrite);

        } // for icolls
      } // for iecs
    }//while
  }//MonitoringFile::CSCChamberEfficiency

  // -------------------------------------------------------------------------------------------------------
  // Compute bin errors a la Ullrich, Paterno and Xu (UPX) variance
  // http://arxiv.org/abs/physics/0701199
  // Alternative to "Binomial" variance used by TH1::Divide option "B"
  // -------------------------------------------------------------------------------------------------------
  void MonitoringFile::GetCscEffAndErr(float& num, float& num_err, 
      float& den, float& den_err, float& eff, float& err, int errtype ) {

    // 0 - binomial
    // 1 - UPX variance
    int binerr = 0;
    if(errtype == 1) binerr = errtype;
    float epsilon = 1.e-3;

    if (den < epsilon) { eff = 0.; err = 0.; return; }
    if(num<(-1.*epsilon)||den<epsilon){
      if(MonitoringFile::getDebugLevel()>0)std::cerr << "--> getCscEffAndErr : ERROR: found negative entries" << num << "\t " << den << std::endl;
      return;
    }

    // quotient
    eff = num/den;

    // error
    if(binerr == 1) {
      // UPX variance
      float num1 = (num+1)*(num+2);
      float den1 = (den+2)*(den+3);
      float num2 = (num+1)*(num+1);
      float den2 = (den+2)*(den+2);
      float variance = num1/den1 - num2/den2;
      err = sqrt(variance);
    } else if(binerr == 0) {
      // Binomial error
      err = fabs( ( (1.-2.*eff)*num_err*num_err + eff*eff*den_err*den_err )/(den*den) );
    } else {
      // Sumw2 error
      err = (num_err*num_err*den*den + den_err*den_err*num*num)/(den*den);
    }

  }

}//namespace
