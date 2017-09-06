/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//Methods to process Pixel histograms after merging.
//Author: Masaki ENDO (masaki.endo@cern.ch)
//Date: Aug 2012
/////////////////////////////////////////////////////////////////////

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
#include "TProfile.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"
#include "TBranch.h"


namespace dqutils {

  static const bool rno_debug = false;

  void
  MonitoringFile::PixelPostProcess( std::string inFilename, bool /* isIncremental */ )
  {
    if (rno_debug) std::cout << "Start Pixel post-processing" <<std::endl;

    TFile *infile = TFile::Open(inFilename.c_str(),"UPDATE");
    if(infile==0 || ! infile->IsOpen()){
      std::cerr << "--> PixelPostProcess: Input file not opened" <<std::endl;
      return;
    }
    if(infile->IsZombie()){
      std::cerr << "--> PixelPostProcess: Input file " << inFilename << " cannot be opened" <<std::endl;
      return;
    }
    if(infile->GetSize()<1000){
      std::cerr << "--> PixelPostProcess: Input file empty " <<std::endl;
      return;
    }

    //start postprocessing
    
    TIter next_run(infile->GetListOfKeys());
    TKey *key_run(0);
    key_run = (TKey*)next_run();
    TDirectory *dir0 = dynamic_cast<TDirectory*> (key_run->ReadObj());
    if (!dir0) return; // should never fail
    dir0->cd();

    TString run_dir;
    int times = 1;
    while (times--) {  // just once
      run_dir = dir0->GetName();
      
      TString rno_dir = run_dir + "/Pixel/Hits/";
      TDirectory *dir = infile->GetDirectory(rno_dir);
      if(!dir){
	std::cerr << "--> PixelPostProcess: directory " << rno_dir << " not found " <<std::endl;
	return;
      }

      TString clus_dir = run_dir + "/Pixel/Clusters/";
      TDirectory *clusdir = infile->GetDirectory(clus_dir);
      if(!clusdir){
	std::cerr << "--> PixelPostProcess: directory " << clus_dir << " not found " <<std::endl;
	return;
      }

      TString err_dir = run_dir + "/Pixel/Errors/";
      TDirectory *errdir = infile->GetDirectory(err_dir);
      if(!errdir){
	std::cerr << "--> PixelPostProcess: directory " << err_dir << " not found " <<std::endl;
	return;
      }
      
      TString status_dir = run_dir + "/Pixel/Status/";
      TDirectory *statusdir = infile->GetDirectory(status_dir);
      if(!statusdir){
	std::cerr << "--> PixelPostProcess: directory " << status_dir << " not found " <<std::endl;
	return;
      }
      
      TString norm_histName = rno_dir + "num_hits";
      TH1F *h_norm = (TH1F*)infile->Get(norm_histName);
      if(!h_norm){
	std::cerr << "--> PixelPostProcess: could not find normalisation histogram " << norm_histName << std::endl;
	return;
      }

      const static int nlayer = 8;
      float nevents = h_norm->Integral(0,h_norm->GetNbinsX()+1);
      TString layerName[nlayer] = {"IBL", "B0", "B1", "B2", "ECA", "ECC", "IBL2D", "IBL3D"};
      float npixel[nlayer] = {26880, 46080, 46080, 46080, 46080, 46080, 26880, 26880};
      float nmodule[nlayer] = {280., 286., 494., 676., 144., 144., 168., 112.};

      const static int nerror = 5;
      //TString errorName[nerror] = {"OpticalErrors_", "SEUErrors_", "SyncErrors_", "TimeoutErrors_", "TruncErrors_"};
      TString errorName[nerror] = {"OpticalErrors_", "SEUErrors_", "SyncErrors_", "TimeoutErrors_", "TruncErrors_"};
      
      TH2F *h_occ[nlayer];
      TH2F *h_occ_new[nlayer];
      TH2F *h_clus[nlayer];
      TH2F *h_clus_new[nlayer];
      TH2F *h_err[nlayer][nerror];
      TH2F *h_err_new[nlayer][nerror];
      TH1F *h_disabled_per_lumi[nlayer];
      TH1F *h_syncerr_per_lumi[nlayer];
      TH1F *h_disabled_syncerr_per_lumi[nlayer];

      for( int i=0; i<nlayer; i++){
         /// initialize 
         h_occ[i] = 0; h_occ_new[i] = 0;
         h_clus[i] = 0; h_clus_new[i] = 0;
         h_disabled_per_lumi[i] = 0;
         h_syncerr_per_lumi[i] = 0;
         h_disabled_syncerr_per_lumi[i] = 0;

         /// for hit occupancy
         
         /// get histo
         TString keyname = "Occupancy_";
         TString histName = rno_dir + keyname + layerName[i];
         h_occ[i] = (TH2F*)infile->Get(histName);
         /// normalize
         if( h_occ[i] ){
            TString tmpname = keyname + layerName[i] + "_byPostProcess";
            h_occ_new[i] = (TH2F*)h_occ[i]->Clone( tmpname );
            h_occ_new[i]->Scale( 1.0/(nevents*npixel[i]) );
         }
         /// Write
         dir->cd();
         if( h_occ_new[i] ) h_occ_new[i]->Write();

         /// for cluster occupancy
         
         /// get histo
         keyname = "Cluster_Occupancy_";
         histName = clus_dir + keyname + layerName[i];
         h_clus[i] = (TH2F*)infile->Get(histName);
         /// normalize
         if( h_clus[i] ){
            TString tmpname = keyname + layerName[i] + "_byPostProcess";
            h_clus_new[i] = (TH2F*)h_clus[i]->Clone( tmpname );
            h_clus_new[i]->Scale( 1.0/nevents );
         }
         clusdir->cd();
         if( h_clus_new[i] ) h_clus_new[i]->Write();

         /// for error map
         
         for( int j=0; j<nerror; j++){
            /// initialize
            h_err[i][j] = 0; h_err_new[i][j] = 0;
            /// get histo
            keyname = errorName[j];
            histName = err_dir + keyname + layerName[i];
            h_err[i][j] = (TH2F*)infile->Get(histName);
            /// normalize
            if( h_err[i][j] ){
               TString tmpname = keyname + layerName[i] + "_byPostProcess";
               h_err_new[i][j] = (TH2F*)h_err[i][j]->Clone( tmpname );
               h_err_new[i][j]->Scale( 1.0/nevents );
            }
            /// write
            errdir->cd();
            if( h_err_new[i][j] ) h_err_new[i][j]->Write();
         }

         /// for disabled + sync errors module
         keyname = "SyncErrorsFrac_per_event_";
         histName = err_dir + keyname + layerName[i];
         h_syncerr_per_lumi[i] = (TH1F*)infile->Get(histName);
         keyname = "DisabledModules_per_lumi_";
         histName = status_dir + keyname + layerName[i];
         h_disabled_per_lumi[i] = (TH1F*)infile->Get(histName);
         /// normalize
         if( h_disabled_per_lumi[i] && h_syncerr_per_lumi[i]){
           keyname = "DisabledAndSyncErrorsModules_per_lumi_";
           TString tmpname = keyname + layerName[i] + "_byPostProcess";
           h_disabled_syncerr_per_lumi[i] = new TH1F(tmpname, "Disable and Sync error per module per event;Lumi block;Avg. fraction per event", 2500, 0, 2500);
           //h_disabled_syncerr_per_lumi[i]->Scale( 1.0/nmodule[i] );
           for(int ibin=0; ibin<2500+1 ; ibin++){
             Double_t cont1 = h_disabled_per_lumi[i]->GetBinContent(ibin) / nmodule[i];
             Double_t err1 = h_disabled_per_lumi[i]->GetBinError(ibin) / nmodule[i];
             Double_t cont2 = h_syncerr_per_lumi[i]->GetBinContent(ibin);
             Double_t err2 = h_syncerr_per_lumi[i]->GetBinError(ibin);
             h_disabled_syncerr_per_lumi[i]->SetBinContent(ibin, cont1+cont2 );
             h_disabled_syncerr_per_lumi[i]->SetBinError(ibin, err1+err2 );
           }
         }
         errdir->cd();
         if( h_disabled_syncerr_per_lumi[i] ) h_disabled_syncerr_per_lumi[i]->Write();

      }
      
      
      infile->Write();
    }//while
  }
    
}//namespace
