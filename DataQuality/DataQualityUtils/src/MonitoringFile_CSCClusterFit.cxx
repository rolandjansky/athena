/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* Methods to perform post-processing on run_nnnnnn/Muon/MuonRawDataMonitoring/CSC/CSCE[AC]/Overview/lfitmean histograms
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
  MonitoringFile::CSCClusterFit(TFile* f, TString& run_dir) {

    bool dbgLevel = false;
    if(dbgLevel) std::cout << "--> CSCClusterFit: Landau fit to cluster qsum per layer" << std::endl;
    
    std::vector<std::string> endc_label;
    endc_label.push_back("A");
    endc_label.push_back("C");
    
    std::vector<std::string> coll_dir_label;
    coll_dir_label.push_back("MuonSegments");

    std::vector<std::string> coll_hist_label;
    coll_hist_label.push_back("Muon");

    f->cd("/");
    TIter next_run ( f->GetListOfKeys() );
    TKey* key_run(0);
    while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
      if (!key_run->IsFolder()) continue;
      run_dir = key_run->GetName();
      if (!run_dir.Contains("run") )  { continue;}

      std::string run_dir2 = run_dir.Data();

      for(std::size_t iec = 0; iec < endc_label.size(); iec++){
        for(std::size_t icoll = 0; icoll < coll_dir_label.size(); icoll++){

          TString csc_segmdir = run_dir + "/Muon/MuonSegmentMonitoring/EndCap" + endc_label.at(iec);
          TString csc_rawdir = run_dir + "/Muon/MuonRawDataMonitoring/CSC/Overview/CSCE" + endc_label.at(iec) ;

          //===CSC segment overview directory
          TString oview_dir = csc_segmdir + "/Overview/" + coll_dir_label.at(icoll) + "/CSC";
          
          //===CSC cluster directory
          TString csc_clusdir = csc_rawdir + "/Cluster";

          TDirectory *dir(0);

          if( !(f->GetDirectory(oview_dir)) ) {
            std::cerr << "--> CSCClusterFit: directory " << oview_dir << " not found." << std::endl;
            return;
          }
          
          if( !(dir = f->GetDirectory(csc_clusdir)) ) {
            std::cerr << "--> CSCClusterFit: directory " << oview_dir << " not found." << std::endl;
            return;
          }
          
          //===CSC etaClusQSum on segment histogram
          TString etaClusQsumHist = oview_dir + "/" + coll_hist_label.at(icoll) + "_Segm_QSumEtaGoodClusPerLayer_E" + endc_label.at(iec);

          //===CSC MPV landau fit per layer histogram
          TString lfitMPVHist = csc_clusdir + "/h1csc_clus_qsum_signal_E" + endc_label.at(iec) + "_lfitmean";

          if( ! f->Get(etaClusQsumHist) ){
            if(dbgLevel) std::cerr << "--> CSCClusterFit: no such histogram " << etaClusQsumHist   << std::endl;
            return;
          }

          if( ! f->Get(lfitMPVHist) ){
            if(dbgLevel) std::cerr << "--> CSCClusterFit: no such histogram " << lfitMPVHist   << std::endl;
            return;
          }

          TH2F *h2 = (TH2F *) f->Get(etaClusQsumHist);
          TH1F *h1 = (TH1F *) f->Get(lfitMPVHist);
          TH1F *hl(0); TF1 *fl1(0);
          int sec = 0;
          float mpv = 0, mpv_err = 0;
          for(int j = 5; j<=85; j++) {
            if (j%5 == 0) { 
              sec++; 
            } else {
              mpv = 0; mpv_err = 0;
              hl = (TH1F *) h2->ProjectionX("_px",j,j);
              if(hl) {
                hl->SetDirectory(0);
                int fit_status = 0;
                float flmin = 50, flmax = 700; // fixed range
                int entries = hl->GetEntries();
                if(entries > 10) {  
                  if(!fl1) fl1 = new TF1("fland","landau",flmin,flmax);
                  fit_status = hl->Fit(fl1,"LRQN", "goff"); // log-Likelihood, iMprove, Range, Quiet, No-draw
                  if(fit_status == 0) {
                    mpv = fl1->GetParameter(1); // 1 = MPV, 2 = sigma, 0 = Norm
                    mpv_err = fl1->GetParError(1); // 1 = MPV, 2 = sigma, 0 = Norm
                  }
                }
              } // hl
              h1->SetBinContent(j,mpv);
              h1->SetBinError(j,mpv_err);
            } // else
          } // j

          // clean up
          if(hl) { delete hl; hl = 0; }
          if(fl1) { delete fl1; fl1 = 0; }

          if(dir) {
            dir->cd();
            h1->Write("",TObject::kOverwrite);
          }

        } // for icolls
      } // for iecs
    }//while
  }//MonitoringFile::CSCClusterFit

}//namespace
