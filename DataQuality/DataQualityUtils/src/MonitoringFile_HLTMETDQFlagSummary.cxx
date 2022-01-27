/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* HLTMET Post Processing Method: In the HLT/MET[Mon/Mon_allCells/Mon_FEB]/DQPlots directory, there are 
 *                                three summary histograms: L2_MET-status, EF_MET_status and and compN_compSumEt_lin 
 *                                and lumi-block based histogram: trmet_lbn_flag 
 *                                This post-processing method fills these histograms appropriately
 * Author   : Venkatesh Kaushik <venkat.kaushik@cern.ch>
 * Date     : Mar 2010
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
#include "TH1I.h"
#include "TH2F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"
#include "TBranch.h"

namespace dqutils {

  //---------------------------------------------------------------------------------------------------
  // Method to obtain summary DQFlags 
  //---------------------------------------------------------------------------------------------------
  void MonitoringFile::HLTMETDQFlagSummary(TFile* f, TString& run_dir) {

      bool dbgLevel = false;
      if(dbgLevel) std::cout << "--> HLTMETDQFlagSummary: Updating histograms in HLT/METMon*/DQPlots  " << std::endl;

      f->cd("/");
      TIter next_run ( f->GetListOfKeys() );
      TKey* key_run(0);
      while ((key_run = dynamic_cast<TKey*> ( next_run() )) !=0 ) {
        if (!key_run->IsFolder()) continue;
        run_dir = key_run->GetName();
        if (!run_dir.Contains("run") )  { continue;}

        TDirectory* tdir_run = dynamic_cast<TDirectory*>( key_run );

        std::string run_dir2 = run_dir.Data();

        // all merged root files have the structure "rootfile.root:/run_NNNNNNN"
        // use that to extract run number
        //int run_number = atoi( (run_dir2.substr(4, run_dir2.size()-4 )).c_str() );
        //run_number=run_number;

        // begin HLTMET
        // note 1: prefix all dirs and hists with '/'
        // note 2: missing dir => return
        // note 3: missing hist => continue
        TString hlt_top = run_dir + "/HLT"; // toplevel
        TString met_efdir = "/EFMissingET_Fex"; // EF dir
        TString met_l2dir = "/L2MissingET_Fex"; // L2 dir
        TString dqflag_dir = "/DQFlags"; // DQ flags dir

        std::vector<TString> met_fexs, met_l2hists, met_efhists;
        // expect the following fex dirs
        met_fexs.push_back("/METMon");
        //met_fexs.push_back("/METMon_FEB");
        //met_fexs.push_back("/METMon_allCells");

        // expect the following histograms
        met_l2hists.push_back("/L2_MET_status");
        met_l2hists.push_back("/L2_MEx_log");
        met_l2hists.push_back("/L2_MEy_log");
        met_l2hists.push_back("/L2_MET_log");
        met_l2hists.push_back("/L2_SumEt_log");

        met_efhists.push_back("/EF_MET_status");
        met_efhists.push_back("/EF_MET_lin");
        met_efhists.push_back("/EF_MET_lin1");
        met_efhists.push_back("/EF_SumEt_lin");
        met_efhists.push_back("/EF_MET_phi");
        met_efhists.push_back("/EF_MET_phi1");
        met_efhists.push_back("/EF_MEx_log");
        met_efhists.push_back("/EF_MEy_log");
        met_efhists.push_back("/compN_compSumEt_lin");
        met_efhists.push_back("/compN_EF_MET_status");

        std::vector<TString> lbnDirs;
        TString lbn_dqhist = "/trmet_lbn_flag";
        size_t lbn_range = HLTMETGetDQLBNRange(tdir_run, lbnDirs);

        //std::cout << "lbn_range = " << lbn_range << std::endl;
        
        // Get EF/L2 status histograms for default Fex
        for( std::vector<TString>::iterator itFex = met_fexs.begin(); itFex != met_fexs.end(); ++itFex ) {
          TString theL2Path = hlt_top + (*itFex) + met_l2dir;
          TString theEFPath = hlt_top + (*itFex) + met_efdir;
          TString theDQPath = hlt_top + (*itFex) + dqflag_dir;

          TDirectory* dirl2 = f->GetDirectory(theL2Path);
          if(!dirl2){
            std::cerr << "--> HLTMETDQFlagSummary: directory " << theL2Path << " not found" << std::endl;
            return;
          }
          TDirectory* diref = f->GetDirectory(theEFPath);
          if(!diref){
            std::cerr << "--> HLTMETDQFlagSummary: directory " << theEFPath << " not found" << std::endl;
            return;
          }
          TDirectory* dirdq = f->GetDirectory(theDQPath);
          if(!dirdq){
            std::cerr << "--> HLTMETDQFlagSummary: directory " << theDQPath << " not found" << std::endl;
            return;
          }
          // loop over L2 hists and copy to DQFlags 
          for(std::vector<TString>::iterator itHist =  met_l2hists.begin(); itHist != met_l2hists.end(); ++itHist) {
            TString histL2 = (theL2Path + *itHist);
            TString histL2C = (theDQPath + *itHist);
            TH1 *hl2(0), *hl2c(0);
            if(!f->Get(histL2)) { if(dbgLevel) std::cerr << "--> HLTMETDQFlagSummary: no histogram " << histL2 << std::endl; continue;}
            if(!f->Get(histL2C)) { if(dbgLevel) std::cerr << "--> HLTMETDQFlagSummary: no histogram " << histL2C << std::endl; continue;}
            hl2 = (TH1 *) (f->Get(histL2));
            // check if MET_status histogram is emtpy
            bool save_status_hist = false;
            if(*itHist == "/L2_MET_status") {
              if(hl2->GetMaximum() < 1.e-3) {
                save_status_hist = true;
                for(int ibin = 1; ibin <= hl2->GetNbinsX(); ibin++) {
                  hl2->SetBinContent(ibin,0.05);
                }
              }
            }
            hl2c = (TH1 *) (f->Get(histL2C));
            hl2c->Reset();
            hl2c->Add(hl2);
            //hl2c->Sumw2();
            //std::cout << "--> HLTMETDQFlagSummary: added histograms " << histL2 << "\t and " << histL2C << std::endl;
            dirdq->cd();
            hl2c->Write("",TObject::kOverwrite);
            if(save_status_hist) {
              if(f->cd(theL2Path))
                hl2->Write("",TObject::kOverwrite);
            }

          } // end loop over l2 hists
          
          // loop over EF hists and copy to DQFlags 
          for(std::vector<TString>::iterator itHist =  met_efhists.begin(); itHist != met_efhists.end(); ++itHist ) {
            TString histEF = (theEFPath + *itHist);
            TString histEFC = (theDQPath + *itHist);
            TH1 *hef(0), *hefc(0);
            if(!f->Get(histEF)) { if(dbgLevel) std::cerr << "--> HLTMETDQFlagSummary: no histogram " << histEF << std::endl; continue;}
            if(!f->Get(histEFC)) { if(dbgLevel) std::cerr << "--> HLTMETDQFlagSummary: no histogram " << histEFC << std::endl; continue;}
            hef = (TH1 *) (f->Get(histEF));
            bool save_status_hist = false;
            if(*itHist == "/EF_MET_status") {
              if(hef->GetMaximum() < 1.e-3) {
                save_status_hist = true;
                for(int ibin = 1; ibin <= hef->GetNbinsX(); ibin++) {
                  hef->SetBinContent(ibin,0.05);
                }
              }
            }
            hefc = (TH1 *) (f->Get(histEFC));
            hefc->Reset();
            hefc->Add(hef);
            //hefc->Sumw2();
            dirdq->cd();
            if(save_status_hist) {
              if(f->cd(theEFPath))
                hef->Write("",TObject::kOverwrite);
            }
            hefc->Write("",TObject::kOverwrite);
          } // end loop over ef hists

          // resize lbn based histogram
          if(lbn_range > 0) {
            TString histLBN = theDQPath + lbn_dqhist;
            TH1 *hlb(0);
            if(!f->Get(histLBN)) { if(dbgLevel) std::cerr << "--> HLTMETDQFlagSummary: no histogram " << histLBN << std::endl; continue;}
            hlb = (TH1 *) (f->Get(histLBN));
            unsigned int nbinx = (unsigned int) hlb->GetNbinsX(), nbiny = (unsigned int) hlb->GetNbinsY(), k = 1;
            if(nbinx != nbiny) continue;

            if(lbn_range < nbinx) {
              hlb->GetXaxis()->SetRangeUser(0,Double_t(lbn_range));
              hlb->GetYaxis()->SetRangeUser(0,Double_t(lbn_range));
            }
            TH1I *hlbstat(0);
            for(std::vector<TString>::iterator it = lbnDirs.begin(); it != lbnDirs.end(); ++it, ++k) {
              if(k > nbinx) continue;
              TString histLBstat = run_dir + TString("/") + *it + TString("/HLT") + (*itFex) + "/lbnstatus/EF_MET_status";
              if(!f->Get(histLBstat)) { if(dbgLevel) std::cerr << "--> HLTMETDQFlagSummary: no histogram " << histLBstat << std::endl; continue; }
              hlbstat = (TH1I *) (f->Get(histLBstat));
              int flag = HLTMETGetStatusPerBin(hlbstat,17,29,32,32);
              //std::cout << "--> HLTMETDQFlagSummary: " << *it << "\t: flag = " << flag << std::endl; 
              TString label = *it;
              label.ReplaceAll("lowStat_","");
              hlb->GetXaxis()->SetBinLabel(k,label.Data());
              hlb->GetYaxis()->SetBinLabel(k,label.Data());
              hlb->SetBinContent(k,k,flag);
            } // end for
            hlb->Write("",TObject::kOverwrite);
          } else {
            std::cerr << "--> HLTMETDQFlagSummary: lowStat_* directories: found none " << std::endl;
          }



        } // end loop over fexs



      } // end while loop over all keys
    } // end method MonitoringFile::HLTMETDQFlagSummary

  //---------------------------------------------------------------------------------------------------
  // Method to get lowStat_* directories
  //---------------------------------------------------------------------------------------------------
  size_t MonitoringFile::HLTMETGetDQLBNRange(TDirectory*& run_dir, std::vector<TString>& lbnDirs) {

    if(!run_dir) return 0;
    lbnDirs.clear();

    bool dbgLevel = false;
    if(dbgLevel) std::cout << "--> HLTMETGetDQLBNRange: lowStat_* directories: ";
    run_dir->cd();

    TIter next_lbn ( run_dir->GetListOfKeys() );
    TKey* key_lbn(0);
    TString lbn_dir="";
    // loop over all objects
    while ((key_lbn = dynamic_cast<TKey*> ( next_lbn() )) !=0 ) {
      if (!key_lbn->IsFolder()) continue;
      lbn_dir = key_lbn->GetName();
      if (!lbn_dir.Contains("lowStat_") )  { continue; }
      lbnDirs.push_back(lbn_dir);
      //std::cout << "dir = " << lbn_dir << "\tuuid = "  << uuid.AsString() << "\ttime = " << dtme.GetTime() << std::endl;
    }
    unsigned int nLBNDirs = lbnDirs.size();
    if(dbgLevel) std::cout << "found " << nLBNDirs << std::endl;
    return nLBNDirs;
  } // end method MonitoringFile::HLTMETGetDQLBNRange

  //---------------------------------------------------------------------------------------------------
  // Method to Get Status flags per lbn block 
  //---------------------------------------------------------------------------------------------------
  int MonitoringFile::HLTMETGetStatusPerBin(TH1I *&hist, int yellmin, int yellmax, int redmin, int redmax) {
    if(!hist) return 0;
    TString hname = hist->GetName();
    int flag = 10; // 1 == GREEN, 2 == YELLOW, 3 == RED
    float epsilon = 1.e-3;
    int nbins = (int) hist->GetNbinsX();
    yellmin = (yellmin > 0 && yellmin <= nbins) ? yellmin : 1;
    yellmax = (yellmax > 0 && yellmax <= nbins) ? yellmax : nbins;
    redmin = (redmin > 0 && redmin <= nbins) ? redmin : 1;
    redmax = (redmax > 0 && redmax <= nbins) ? redmax : nbins;
    for(int k = 1; k <= nbins; k++) {
      float theVal = hist->GetBinContent(k);
      // at least one bin > 0
      if(theVal > epsilon) {
        if(k >= yellmin && k <= yellmax) flag = 100;
        if(k >= redmin && k <= redmax) flag = 1000;
      } // end if
    } // end for
    return flag;
  } // end method MonitoringFile::HLTMETGetStatusPerBin

} // end namespace
