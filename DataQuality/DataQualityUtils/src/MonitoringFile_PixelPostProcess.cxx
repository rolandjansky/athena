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
      
      TString norm_histName = rno_dir + "num_hits";
      TH1F *h_norm = (TH1F*)infile->Get(norm_histName);
      if(!h_norm){
	std::cerr << "--> PixelPostProcess: could not find normalisation histogram " << norm_histName << std::endl;
	return;
      }

      const static int nlayer = 6;
      float nevents = h_norm->Integral();
      TString layerName[nlayer] = {"IBL", "B0", "B1", "B2", "ECA", "ECC"};
      float npixel[nlayer] = {26880, 46080, 46080, 46080, 46080, 46080};

      const static int nerror = 5;
      TString errorName[nerror] = {"OpticalErrors_", "SEUErrors_", "SyncErrors_", "TimeoutErrors_", "TruncationErrors_"};
      
      TH2F *h_occ[nlayer];
      TH2F *h_occ_new[nlayer];
      TH2F *h_clus[nlayer];
      TH2F *h_clus_new[nlayer];
      TH2F *h_err[nlayer][nerror];
      TH2F *h_err_new[nlayer][nerror];

      for( int i=0; i<nlayer; i++){
         /// initialize 
         h_occ[i] = 0; h_occ_new[i] = 0;
         h_clus[i] = 0; h_clus_new[i] = 0;
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
         }
      }
      
      dir->cd();
      for( int i=0; i<nlayer; i++){
         if( h_occ_new[i] ) h_occ_new[i]->Write();
      }

      clusdir->cd();
      for( int i=0; i<nlayer; i++){
         if( h_clus_new[i] ) h_clus_new[i]->Write();
      }
      
      errdir->cd();
      for( int i=0; i<nlayer; i++){
         for( int j=0; j<nerror; j++){
            if( h_err_new[i][j] ) h_err_new[i][j]->Write();
         }
      }
      
      infile->Write();
      
    }//while
  }
    
}//namespace
