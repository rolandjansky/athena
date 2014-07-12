/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



///////////////////////////////////////////////////////////////////////////////////
//
//Post processing algorithm for Muon Track Physics Monitoring
//
///////////////////////////////////////////////////////////////////////////////////

#include "DataQualityUtils/MonitoringFile.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <map>
#include <iomanip>
#include <set>

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"
#include "TMath.h"
#include "TF1.h"
#include "TTree.h"
#include "TBranch.h"
#include "TList.h"
#include "TProfile.h"
#include "TMinuit.h"

void DGausFit(TH1 * Histogram, float * results, float fitWindow/*sigmas*/=3, bool overwrite = true, float th_A=100, float th_B=300);
void SimpleMean(TH1 * Mean, TH1 * Entries, TH1 * Weights, TH1 * Sigma = NULL, bool Eff = false);
void SimpleMean(TH2 * Mean, TH2 * Entries, TH2 * Weights, TH1 * Sigma = NULL, bool Eff = false);

void DGausFit(TH1 * Histogram, float * results, float fitWindow/*sigmas*/,  bool overwrite, float th_A, float th_B){
  
  
  if (results == NULL) {
    //    std::cerr << "DGausFit(): "
    //    << "Uninitialized result pointer was passed. \n";
    return;
  }
  
  if (Histogram == NULL) {
    results[0] = 0.0;
    results[1] = 0.0;
    results[2] = 0.0;
    results[3] = 0.0;
    //    std::cerr << "DGausFit(): "
    //    << "Uninitialized histogram pointer was passed. \n";
    return;
  }
  
  // minimum hit requirement
  if (Histogram->GetEntries() < 50) {
    
    results[0] = 0.0;
    results[1] = FLT_MAX/2;
    results[2] = 0.0;
    results[3] = FLT_MAX/2;//Error
    return;
    
  }
  
  //Low Stats - Return MEAN/RMS
  if (Histogram->GetEntries() < th_A) {
    
    results[0] = Histogram->GetMean();
    results[1] = 0.0;
    results[2] = fabs(Histogram->GetRMS());
    results[3] = 0.0;//Error
    return;
    
  }else if (Histogram->GetEntries() < th_B) {
    
    float mean         = Histogram->GetMean();
    float width        = Histogram->GetRMS();
    float max          = Histogram->GetMaximum();
    
    float range_high = (mean+fitWindow*width/2 > Histogram->GetXaxis()->GetXmax() ? Histogram->GetXaxis()->GetXmax() : mean+fitWindow*width/2);
    float range_low  = (mean-fitWindow*width/2 < Histogram->GetXaxis()->GetXmin() ? Histogram->GetXaxis()->GetXmin() : mean-fitWindow*width/2);
    
    TF1 *gaus = new TF1("f1", "gaus(0)", range_low, range_high);
    
    gaus->SetParameters(max,mean,width);
    
    gaus->SetParLimits(0, max*(0.75), max*(1.25));
    gaus->SetParLimits(1, mean-width/4, mean+width/4);
    gaus->SetParLimits(2, width*(0.05), width*(1.0));
    
    Histogram->Fit(gaus,"QRU");
    
    Double_t parameters[3];
    gaus->GetParameters(&parameters[0]);
    
//    std::cout << Histogram->GetName() << " G  " << gMinuit->fCstatu << " " << gaus->GetProb() << " " << gaus->GetChisquare() << std::endl;
    
    
    if(gMinuit->fCstatu == "CONVERGED "){
      float pRelErr[3] = {float(std::fabs(gaus->GetParError(0))/std::fabs(parameters[0])),
                          float(std::fabs(gaus->GetParError(1))/std::fabs(parameters[1])),
                          float(std::fabs(gaus->GetParError(2))/std::fabs(parameters[2]))};
      float pAmp    = parameters[0];
      float pMean   = parameters[1];
      float pSigma  = parameters[2];
      
      bool Choose = true;
      
      if (pAmp   <  0.25*max)         Choose *= false;
      if (std::fabs(pSigma) > 
          std::fabs(width))          Choose *= false;
      if (pMean  > range_high)       Choose *= false;
      if (pMean  < range_low)        Choose *= false; 
      if (pRelErr[0]  > 0.15)        Choose *= false; 
      if (pRelErr[1]  > 0.15)        Choose *= false; 
      if (pRelErr[2]  > 0.15)        Choose *= false;           
      
      
      if (Choose) {
        results[0] = (float)parameters[1];//Mean
        results[1] = (float)fabs(gaus->GetParError(1));
        results[2] = (float)fabs(parameters[2]);//Width
        results[3] = (float)fabs(gaus->GetParError(2));
      }
      else{
        // Retry with "Low Stats"
        DGausFit(Histogram, results, fitWindow/*sigmas*/,  overwrite, FLT_MAX, FLT_MAX);
        return;
      }
      
      
    }
    else{
      // Retry with "Low Stats"
      DGausFit(Histogram, results, fitWindow/*sigmas*/,  overwrite, FLT_MAX, FLT_MAX);
      return;
    }
    
    if(overwrite) Histogram->Write("",TObject::kOverwrite);
    return;
    
  }else {
    
    
    
    // Same Mean
    float mean         = Histogram->GetMean();
    
    // Sharp Peak
    float width        = Histogram->GetRMS()*1/5;
    float max          = Histogram->GetMaximum()*4/5;
    
    // Wide Peak
    float width_2       = Histogram->GetRMS()*4/5;
    float max_2         = Histogram->GetMaximum()*1/5;
    
    //Ranges
    float range_high = (mean+fitWindow*width*3 > Histogram->GetXaxis()->GetXmax() ? Histogram->GetXaxis()->GetXmax() : mean+fitWindow*width*3);
    float range_low  = (mean-fitWindow*width*3 < Histogram->GetXaxis()->GetXmin() ? Histogram->GetXaxis()->GetXmin() : mean-fitWindow*width*3);
    
    TF1 *doublegaus = new TF1("f1", "gaus(0) + gaus(3)", range_low, range_high);
    doublegaus->SetParameters(max,mean,width,max_2,mean,width_2);

    doublegaus->SetParLimits(0, max*(0.7), max*(1.2));
    doublegaus->SetParLimits(1, mean-width/4, mean+width/4);
    doublegaus->SetParLimits(2, width*(0.05), width*(.7));
    doublegaus->SetParLimits(0, 0, max*(.7));
    doublegaus->SetParLimits(1, mean-width/2, mean+width/2);
    doublegaus->SetParLimits(2, width*(.7), width*(2.0));
    
    Histogram->Fit(doublegaus,"QRU");
    
    Double_t parameters[6];
    doublegaus->GetParameters(&parameters[0]);
    
    
    //std::cout << Histogram->GetName() << " DG " << gMinuit->fCstatu << " " << doublegaus->GetProb() << " " << doublegaus->GetChisquare() << std::endl;
    
    if(gMinuit->fCstatu == "CONVERGED "){         
      
      float pRelErr[2] = {float(std::fabs(doublegaus->GetParError(0))/std::fabs(parameters[0])),
                          float(std::fabs(doublegaus->GetParError(3))/std::fabs(parameters[3]))};
      float pAmp[2]    = {float(parameters[0]),float(parameters[3])};
      float pMean[2]   = {float(parameters[1]),float(parameters[4])};
      float pSigma[2]  = {float(parameters[2]),float(parameters[5])};
      
      bool Choose[2] = {true,true};
      
      for (int i = 0; i < 2; i++) {
        //Veto Bad Gaussians
        if (pAmp[i]   <  0.25*max )        Choose[i] *= false;
        if (std::fabs(pSigma[i]) > 
            std::fabs(width))             Choose[i] *= false;
        if (pMean[i]  > range_high)       Choose[i] *= false;
        if (pMean[i]  < range_low)        Choose[i] *= false; 
        if (pRelErr[i]  > 0.15)           Choose[i] *= false;             
      }
      
      if (Choose[0] && Choose[1]) {
        if (std::fabs(pAmp[0]-Histogram->GetEntries()) <
            std::fabs(pAmp[1]-Histogram->GetEntries())) {
          Choose[1] *= false;
        }else {
          Choose[0] *= false;
        }
      }     
      
      if(Choose[0] && !Choose[1]){
        
        results[0] = (float)parameters[1];//Mean
        results[1] = (float)fabs(doublegaus->GetParError(1));
        results[2] = (float)fabs(parameters[2]);//Width
        results[3] = (float)fabs(doublegaus->GetParError(2));
        
      }
      else if (Choose[1] && !Choose[0]){
        
        results[0] = (float)parameters[4];//Mean
        results[1] = (float)fabs(doublegaus->GetParError(4));
        results[2] = (float)fabs(parameters[5]);//Width
        results[3] = (float)fabs(doublegaus->GetParError(5));
        
      }
      //If neither are reasonably measured, The one with the greatest amplitude is chosen
      else {
        // Retry with "Low-er Stats"
        DGausFit(Histogram, results, fitWindow/*sigmas*/,  overwrite, th_A, FLT_MAX);
        return;
      } 
    }
    else{
      // Retry with "Low-er Stats"
      DGausFit(Histogram, results, fitWindow/*sigmas*/,  overwrite, th_A, FLT_MAX);
      return;
    }
    
    
    if(overwrite) Histogram->Write("",TObject::kOverwrite);
    return;
  }
  
  results[0] = 0.0;
  results[1] = 0.0;
  results[2] = 0.0;
  results[3] = 0.0;
  return;
  
}

void SimpleMean(TH1 * Mean, TH1 * Weights, TH1 * Entries, TH1 * Sigma, bool Eff ){
  
  if (Mean == NULL || Entries == NULL || Weights == NULL) {
    return;
  }
  
  float Sigma_val = 0.0;
  
  if (Sigma) {
    Sigma_val = Sigma->GetRMS(1);
  }
  
  
  for(int bin_itr = 0; bin_itr < Mean->GetNbinsX(); bin_itr++){
    int i = bin_itr+1;
    
    Mean->SetBinContent(i,0.0);
    Mean->SetBinError(i,0.0);
    
    if (Entries->GetBinContent(i)) {
      
      float Mean_val = Weights->GetBinContent(i)/(Entries->GetBinContent(i));
      Mean->SetBinContent(i,Mean_val);
      Mean->SetBinError(i,0.0);
      
      //Change Error to basic statistical approximation
      if (Sigma_val) {
        Mean->SetBinError(i,Sigma_val/sqrt(Entries->GetBinContent(i)));
      }
      
      //Change Error to binomial function
      if(Eff){
        Mean->SetBinError(i,sqrt((1.0-Mean_val)*Mean_val/Entries->GetBinContent(i)));
      }
      
    }
    
  }
  
  Mean->Write("",TObject::kOverwrite);
  
  return;
  
}

void SimpleMean(TH2 * Mean, TH2 * Weights, TH2 * Entries, TH1 * Sigma, bool Eff ){
  
  if (Mean == NULL || Entries == NULL || Weights == NULL) {
    return;
  }
  
  float Sigma_val = 0.0;
  
  if (Sigma) {
    Sigma_val = Sigma->GetRMS(1);
  }
  
  
  for(int bin_itrX = 0; bin_itrX < Mean->GetNbinsX(); bin_itrX++){
    int i = bin_itrX+1;
    
    for(int bin_itrY = 0; bin_itrY < Mean->GetNbinsY(); bin_itrY++){
      int j = bin_itrY+1;
      
      Mean->SetBinContent(i,j,0.0);
      Mean->SetBinError(i,j,0.0);
      
      if (Entries->GetBinContent(i,j)) {
        
        float Mean_val = Weights->GetBinContent(i,j)/(Entries->GetBinContent(i,j));
        Mean->SetBinContent(i,j,Mean_val);
        Mean->SetBinError(i,j,0.0);
        
        //Change Error to basic statistical approximation
        if (Sigma_val) {
          Mean->SetBinError(i,j,Sigma_val/sqrt(Entries->GetBinContent(i,j)));
        }
        
        //Change Error to binomial function
        if(Eff){
          Mean->SetBinError(i,j,sqrt((1.0-Mean_val)*Mean_val/Entries->GetBinContent(i,j)));
        }
        
      }
    }
    
  }  
  Mean->Write("",TObject::kOverwrite);
  
  return;
  
}



namespace dqutils {
  
  
  void MonitoringFile::MuonTrkPhys( std::string inFilename, bool isIncremental){
    
    MonitoringFile::MuonTrkPhys_Zmass( inFilename );
    MonitoringFile::MuonTrkPhys_Tracking( inFilename );
    MonitoringFile::MuonTrkPhys_DetectorSpecific( inFilename );
    MonitoringFile::MuonTrkPhys_BField( inFilename );
    MonitoringFile::MuonTrkPhys_Alignment( inFilename );
    
    if (isIncremental) {
      
      return;
      
    }
    
    return;
    
  }
  
  void MonitoringFile::MuonTrkPhys_Zmass(std::string inFilename){
    
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
    
    if (f == 0) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file not opened \n";
      return;
    }
    if(f->GetSize()<1000.) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file empty \n";
      return; 
    }
    
    //Define Constants
    std::vector<TString> region_strings;
    region_strings.push_back("EC_EC");
    region_strings.push_back("EC_BC");
    region_strings.push_back("EC_BA");
    region_strings.push_back("EC_EA");
    region_strings.push_back("BC_EC");
    region_strings.push_back("BC_BC");
    region_strings.push_back("BC_BA");
    region_strings.push_back("BC_EA");
    region_strings.push_back("BA_EC");
    region_strings.push_back("BA_BC");
    region_strings.push_back("BA_BA");
    region_strings.push_back("BA_EA");
    region_strings.push_back("EA_EC");
    region_strings.push_back("EA_BC");
    region_strings.push_back("EA_BA");
    region_strings.push_back("EA_EA");
    
    
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    if (key0 == 0) return;
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    if (dir0 == 0) return;
    dir0->cd();
    ///
    
    TString runNumber = dir0->GetName();
    //std::cout<<"jk: runNumber = "<<runNumber<<endl;
    TString mDir =  runNumber+"/Muon/MuonTrkPhysMonitoring/NoTrigger/TrackingPerformance/Z_Signal/";
    if (! f->cd(mDir)) return;
    //std::cout<<"jk: in directory "<<mDir<<endl;
    TIter nextcd1(gDirectory->GetListOfKeys());
    while(TKey* key1 = dynamic_cast<TKey*>(nextcd1())) {//While in Staco/Muid/Third chain
      TString recalg_fullStr = mDir+key1->GetName();
      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
      if(!dir1) continue;
      dir1->cd();
      
      // Divide the efficiency histograms
      TH2F* m_Z_EffNumerator = (TH2F*)dir1->Get("m_Z_EffNumerator");
      TH2F* m_Z_EffDenominator = (TH2F*)dir1->Get("m_Z_EffDenominator");
      TH2F* m_Z_Efficiency = (TH2F*)dir1->Get("m_Z_Efficiency");
      
      SimpleMean(m_Z_Efficiency,m_Z_EffNumerator,m_Z_EffDenominator,NULL,true);
      
      
      // Divide the efficiency histograms
      TH1F* m_Z_EffNumerator_eta = (TH1F*)dir1->Get("m_Z_EffNumerator_eta");
      TH1F* m_Z_EffDenominator_eta = (TH1F*)dir1->Get("m_Z_EffDenominator_eta");
      TH1F* m_Z_Efficiency_eta = (TH1F*)dir1->Get("m_Z_Efficiency_eta");
      
      SimpleMean(m_Z_Efficiency_eta,m_Z_EffNumerator_eta,m_Z_EffDenominator_eta,NULL,true);
      
      
      // Fit the Mass histograms to BW + gaussian
      TH1F* m_M_Z_Mean = (TH1F*)dir1->Get("m_M_Z_Mean");      
      TH1F* m_M_Z_Sigma = (TH1F*)dir1->Get("m_M_Z_Sigma");
      
      if(m_M_Z_Mean && m_M_Z_Sigma){
        TH1F* h_M_Z_region = 0;
        
        for(unsigned int region_it = 0; region_it < region_strings.size(); region_it++){
          //cout<<"jk, processing histogram "<<region_strings.at(region_it)<<endl;
          
          TString histname = "m_M_Z_"+region_strings.at(region_it);
          h_M_Z_region = (TH1F*)dir1->Get(histname);
          
          if(!h_M_Z_region){
            //std::cout<<"Could not find histogram "<<histname<<endl;
            continue;
          }
          
          
          //reset
          m_M_Z_Mean->SetBinContent(region_it+1,0.0);
          m_M_Z_Sigma->SetBinContent(region_it+1,0.0);
          m_M_Z_Mean->SetBinError(region_it+1,0.0);
          m_M_Z_Sigma->SetBinError(region_it+1,0.0);
          
          float results[4] = {0.0,0.0,0.0,0.0};
          h_M_Z_region->Rebin(4);
          DGausFit(h_M_Z_region,results,7);
          
          //Get Results
          m_M_Z_Mean->SetBinContent(region_it+1, results[0]);
          m_M_Z_Sigma->SetBinContent(region_it+1,results[2]);
          
          m_M_Z_Mean->SetBinError(region_it+1,   results[1]);
          m_M_Z_Sigma->SetBinError(region_it+1,  results[3]);
          
        }
        
        m_M_Z_Mean->Write("",TObject::kOverwrite);
        m_M_Z_Sigma->Write("",TObject::kOverwrite);
        
      }
      
    } //ends staco/muid
    
    f->Close();
    delete f;
    
    return;
    
  } //ends Z-mass function
  
  
  void MonitoringFile::MuonTrkPhys_Tracking( std::string inFilename ){
    
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
    
    if (f == 0) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file not opened \n";
      return;
    }
    if(f->GetSize()<1000.) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file empty \n";
      return; 
    }
    
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    if (key0 == 0) return;
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    if (dir0 == 0) return;
    dir0->cd();
    ///
    
    TString runNumber = dir0->GetName();
    TString mDir =  runNumber+"/Muon/MuonTrkPhysMonitoring/NoTrigger/TrackingPerformance/General/";
    if (! f->cd(mDir)) return;
    //std::cout<<"jk: in directory "<<mDir<<std::endl;
    TIter nextcd1(gDirectory->GetListOfKeys());
    while(TKey* key1 = dynamic_cast<TKey*>(nextcd1())) {//While in Staco/Muid/Third chain
      TString recalg_fullStr = mDir+key1->GetName();
      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
      if(!dir1) continue;
      dir1->cd();
      
      //Counting
      {
        //Hits Per Track
        TH2F* m_N_HpT_pLB_CSC = (TH2F*)dir1->Get("m_N_HpT_pLB_CSC");
        TH2F* m_N_HpT_pLB_CSC_Weights = (TH2F*)dir1->Get("m_N_HpT_pLB_CSC_Weights");
        TH2F* m_N_HpT_pLB_CSC_Entries = (TH2F*)dir1->Get("m_N_HpT_pLB_CSC_Entries");
        
        SimpleMean(m_N_HpT_pLB_CSC,m_N_HpT_pLB_CSC_Weights,m_N_HpT_pLB_CSC_Entries,NULL,false);
        
        TH2F* m_N_HpT_pLB_TGC = (TH2F*)dir1->Get("m_N_HpT_pLB_TGC");
        TH2F* m_N_HpT_pLB_TGC_Weights = (TH2F*)dir1->Get("m_N_HpT_pLB_TGC_Weights");
        TH2F* m_N_HpT_pLB_TGC_Entries = (TH2F*)dir1->Get("m_N_HpT_pLB_TGC_Entries");
        
        SimpleMean(m_N_HpT_pLB_TGC,m_N_HpT_pLB_TGC_Weights,m_N_HpT_pLB_TGC_Entries,NULL,false);
        
        TH2F* m_N_HpT_pLB_MDT = (TH2F*)dir1->Get("m_N_HpT_pLB_MDT");
        TH2F* m_N_HpT_pLB_MDT_Weights = (TH2F*)dir1->Get("m_N_HpT_pLB_MDT_Weights");
        TH2F* m_N_HpT_pLB_MDT_Entries = (TH2F*)dir1->Get("m_N_HpT_pLB_MDT_Entries");
        
        SimpleMean(m_N_HpT_pLB_MDT,m_N_HpT_pLB_MDT_Weights,m_N_HpT_pLB_MDT_Entries,NULL,false);
        
        TH2F* m_N_HpT_pLB_RPC = (TH2F*)dir1->Get("m_N_HpT_pLB_RPC");
        TH2F* m_N_HpT_pLB_RPC_Weights = (TH2F*)dir1->Get("m_N_HpT_pLB_RPC_Weights");
        TH2F* m_N_HpT_pLB_RPC_Entries = (TH2F*)dir1->Get("m_N_HpT_pLB_RPC_Entries");
        
        SimpleMean(m_N_HpT_pLB_RPC,m_N_HpT_pLB_RPC_Weights,m_N_HpT_pLB_RPC_Entries,NULL,false);
        
    
        //Precision hits
        TH1F* m_N_Precision = (TH1F*)dir1->Get("m_N_Precision");
        TH1F* m_N_Precision_entries = (TH1F*)dir1->Get("m_N_Precision_entries");
        TH1F* m_N_Precision_weights = (TH1F*)dir1->Get("m_N_Precision_weights");
        TH1F* m_N_Precision_mean = (TH1F*)dir1->Get("m_N_Precision_mean");
        
        SimpleMean(m_N_Precision_mean,m_N_Precision_weights,m_N_Precision_entries,m_N_Precision,false);
        
        //Phi hits
        TH1F* m_N_Phi_Hits = (TH1F*)dir1->Get("m_N_Phi_Hits");
        TH1F* m_N_Phi_Hits_entries = (TH1F*)dir1->Get("m_N_Phi_Hits_entries");
        TH1F* m_N_Phi_Hits_weights = (TH1F*)dir1->Get("m_N_Phi_Hits_weights");
        TH1F* m_N_Phi_Hits_mean = (TH1F*)dir1->Get("m_N_Phi_Hits_mean");
        
        SimpleMean(m_N_Phi_Hits_mean,m_N_Phi_Hits_weights,m_N_Phi_Hits_entries,m_N_Phi_Hits,false);
        
        //Hits on segment
        TH1F* m_N_Hits_Segment = (TH1F*)dir1->Get("m_N_Hits_Segment");
        TH1F* m_N_Hits_Segment_entries = (TH1F*)dir1->Get("m_N_Hits_Segment_entries");
        TH1F* m_N_Hits_Segment_weights = (TH1F*)dir1->Get("m_N_Hits_Segment_weights");
        TH1F* m_N_Hits_Segment_mean = (TH1F*)dir1->Get("m_N_Hits_Segment_mean");
        
        SimpleMean(m_N_Hits_Segment_mean,m_N_Hits_Segment_weights,m_N_Hits_Segment_entries,m_N_Hits_Segment,false);
        
        //Segments per track
        TH1F* m_N_Segment_Trk = (TH1F*)dir1->Get("m_N_Segment_Trk");
        TH1F* m_N_Segment_Trk_entries = (TH1F*)dir1->Get("m_N_Segment_Trk_entries");
        TH1F* m_N_Segment_Trk_weights = (TH1F*)dir1->Get("m_N_Segment_Trk_weights");
        TH1F* m_N_Segment_Trk_mean = (TH1F*)dir1->Get("m_N_Segment_Trk_mean");
        
        SimpleMean(m_N_Segment_Trk_mean,m_N_Segment_Trk_weights,m_N_Segment_Trk_entries,m_N_Segment_Trk,false);
      }
      
      //T0
      {
        //t-zero vs. eta & phi
        TH1F* m_t0_eta_phi = (TH1F*)dir1->Get("m_t0_eta_phi");
        
        TH2F* m_t0_eta_phi_entries = (TH2F*)dir1->Get("m_t0_eta_phi_entries");
        TH2F* m_t0_eta_phi_weights = (TH2F*)dir1->Get("m_t0_eta_phi_weights");
        TH2F* m_t0_eta_phi_mean = (TH2F*)dir1->Get("m_t0_eta_phi_mean");
        
        SimpleMean(m_t0_eta_phi_mean,m_t0_eta_phi_weights,m_t0_eta_phi_entries,m_t0_eta_phi,false);
        
        TH1F* m_t0_eta_phi_i = (TH1F*)dir1->Get("m_t0_eta_phi_i");
        TH2F* m_t0_eta_phi_i_entries = (TH2F*)dir1->Get("m_t0_eta_phi_i_entries");
        TH2F* m_t0_eta_phi_i_weights = (TH2F*)dir1->Get("m_t0_eta_phi_i_weights");
        TH2F* m_t0_eta_phi_i_mean = (TH2F*)dir1->Get("m_t0_eta_phi_i_mean");
        
        SimpleMean(m_t0_eta_phi_i_mean,m_t0_eta_phi_i_weights,m_t0_eta_phi_i_entries,m_t0_eta_phi_i,false);
        
        TH1F* m_t0_eta_phi_m = (TH1F*)dir1->Get("m_t0_eta_phi_m");
        TH2F* m_t0_eta_phi_m_entries = (TH2F*)dir1->Get("m_t0_eta_phi_m_entries");
        TH2F* m_t0_eta_phi_m_weights = (TH2F*)dir1->Get("m_t0_eta_phi_m_weights");
        TH2F* m_t0_eta_phi_m_mean = (TH2F*)dir1->Get("m_t0_eta_phi_m_mean");
        
        SimpleMean(m_t0_eta_phi_m_mean,m_t0_eta_phi_m_weights,m_t0_eta_phi_m_entries,m_t0_eta_phi_m,false);
        
        TH1F* m_t0_eta_phi_o = (TH1F*)dir1->Get("m_t0_eta_phi_o");
        TH2F* m_t0_eta_phi_o_entries = (TH2F*)dir1->Get("m_t0_eta_phi_o_entries");
        TH2F* m_t0_eta_phi_o_weights = (TH2F*)dir1->Get("m_t0_eta_phi_o_weights");
        TH2F* m_t0_eta_phi_o_mean = (TH2F*)dir1->Get("m_t0_eta_phi_o_mean");
        
        SimpleMean(m_t0_eta_phi_o_mean,m_t0_eta_phi_o_weights,m_t0_eta_phi_o_entries,m_t0_eta_phi_o,false);
        
        
        //Resolved by Lumi Block and Large Detector Region
        
        TH1F* m_t0_LB_EA = (TH1F*)dir1->Get("m_Good_t0_EA");
        TH1F* m_t0_LB_EA_entries = (TH1F*)dir1->Get("m_t0_LB_EA_entries");
        TH1F* m_t0_LB_EA_weights = (TH1F*)dir1->Get("m_t0_LB_EA_weights");
        TH1F* m_t0_LB_EA_mean = (TH1F*)dir1->Get("m_t0_LB_EA_mean");
        
        SimpleMean(m_t0_LB_EA_mean,m_t0_LB_EA_weights,m_t0_LB_EA_entries,m_t0_LB_EA,false);
        
        TH1F* m_t0_LB_BA = (TH1F*)dir1->Get("m_Good_t0_BA");
        TH1F* m_t0_LB_BA_entries = (TH1F*)dir1->Get("m_t0_LB_BA_entries");
        TH1F* m_t0_LB_BA_weights = (TH1F*)dir1->Get("m_t0_LB_BA_weights");
        TH1F* m_t0_LB_BA_mean = (TH1F*)dir1->Get("m_t0_LB_BA_mean");
        
        SimpleMean(m_t0_LB_BA_mean,m_t0_LB_BA_weights,m_t0_LB_BA_entries,m_t0_LB_BA,false);
        
        TH1F* m_t0_LB_BC = (TH1F*)dir1->Get("m_Good_t0_BC");
        TH1F* m_t0_LB_BC_entries = (TH1F*)dir1->Get("m_t0_LB_BC_entries");
        TH1F* m_t0_LB_BC_weights = (TH1F*)dir1->Get("m_t0_LB_BC_weights");
        TH1F* m_t0_LB_BC_mean = (TH1F*)dir1->Get("m_t0_LB_BC_mean");
        
        SimpleMean(m_t0_LB_BC_mean,m_t0_LB_BC_weights,m_t0_LB_BC_entries,m_t0_LB_BC,false);
        
        TH1F* m_t0_LB_EC = (TH1F*)dir1->Get("m_Good_t0_EC");
        TH1F* m_t0_LB_EC_entries = (TH1F*)dir1->Get("m_t0_LB_EC_entries");
        TH1F* m_t0_LB_EC_weights = (TH1F*)dir1->Get("m_t0_LB_EC_weights");
        TH1F* m_t0_LB_EC_mean = (TH1F*)dir1->Get("m_t0_LB_EC_mean");
        
        SimpleMean(m_t0_LB_EC_mean,m_t0_LB_EC_weights,m_t0_LB_EC_entries,m_t0_LB_EC,false);
      }
      
      // This Needs to be figured out!
      //Muons per Run number: make it pretty
      /*
       TH1F* m_N_Mu_Lumi_Run = (TH1F*)dir1->Get("m_N_Mu_Lumi_Run");
       if (m_N_Mu_Lumi_Run) {
       int n_runs = 0;
       std::vector<float> mu_vec;
       std::vector<TString> runs;
       
       int firstrun = m_N_Mu_Lumi_Run->GetBinLowEdge(1);
       int lastrun = m_N_Mu_Lumi_Run->GetBinLowEdge( m_N_Mu_Lumi_Run->GetNbinsX() + 1 );
       for (int run = firstrun; run != lastrun; run++){
       int bin = m_N_Mu_Lumi_Run->FindBin(run);
       float n_mu = m_N_Mu_Lumi_Run->GetBinContent(bin);
       if(n_mu != 0){
       n_runs++;
       mu_vec.push_back(n_mu);
       stringstream runno;
       runno << run;
       runs.push_back(runno.str());
       }
       }
       //std::cout<<__FILE__<<":"<<__LINE__<<endl;
       if(n_runs != 0){
       TH1F* Muons_per_Run = new TH1F("Muons_per_Run","Muons_per_Run",n_runs,0,n_runs);
       for(int j = 0; j < n_runs; j++){
       Muons_per_Run->SetBinContent(j+1,mu_vec.at(j));
       Muons_per_Run->GetXaxis()->SetBinLabel(j+1,runs.at(j));
       }
       Muons_per_Run->Write("",TObject::kOverwrite);
       }
       }
       //std::cout<<__FILE__<<":"<<__LINE__<<endl;
       */
      
      
      // Good t0 Fits
      {
        std::vector<TString> string_vec;
        string_vec.clear();
        string_vec.push_back("EC");
        string_vec.push_back("BC");
        string_vec.push_back("BA");
        string_vec.push_back("EA");
        //std::cout<<__FILE__<<":"<<__LINE__<<endl;
        TH1F* m_Good_t0 = (TH1F*)dir1->Get("m_Good_t0");
        TH1F* h_t0 = 0;
        if(m_Good_t0){
          for(unsigned int st_itr = 0; st_itr < string_vec.size(); st_itr++){
            
            h_t0 = (TH1F*)dir1->Get("m_Good_t0_"+string_vec.at(st_itr));
            if (!h_t0) {
              continue;
            }
            
            float results[4] = {0.0,0.0,0.0,0.0};
            h_t0->Rebin(5);
            DGausFit(h_t0,results,5);
            
            //Clean Up
            m_Good_t0->SetBinContent(st_itr+1,0.0);//Mean
            m_Good_t0->SetBinError  (st_itr+1,0.0);//Rms
            
            //Assign Fit Values
            m_Good_t0->SetBinContent(st_itr+1,results[0]);//Mean
            m_Good_t0->SetBinError  (st_itr+1,results[2]/sqrt(h_t0->GetEntries()));//Rms
            
            m_Good_t0->Write("",TObject::kOverwrite);
          } 
        }
      }
      
      
    }
    
    
    f->Close();
    delete f;
    
    return;
    
  } //End Tracking general function
  
  
  void MonitoringFile::MuonTrkPhys_DetectorSpecific( std::string inFilename ){
    
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
    
    if (f == 0) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file not opened \n";
      return;
    }
    if(f->GetSize()<1000.) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file empty \n";
      return; 
    }
    
    //Define Constants
    std::vector<TString> phi_vec;
    phi_vec.push_back("S01");
    phi_vec.push_back("S02");
    phi_vec.push_back("S03");
    phi_vec.push_back("S04");
    phi_vec.push_back("S05");
    phi_vec.push_back("S06");
    phi_vec.push_back("S07");
    phi_vec.push_back("S08");
    phi_vec.push_back("S09");
    phi_vec.push_back("S10");
    phi_vec.push_back("S11");
    phi_vec.push_back("S12");
    phi_vec.push_back("S13");
    phi_vec.push_back("S14");
    phi_vec.push_back("S15");
    phi_vec.push_back("S16");
    
    std::vector<TString> side_vec;
    side_vec.push_back("A");
    side_vec.push_back("C");  
    
    std::vector<TString> string_vec;
    string_vec.push_back("EC");
    string_vec.push_back("BC");
    string_vec.push_back("BA");
    string_vec.push_back("EA");
    
    
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    if (key0 == 0) return;
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    if (dir0 == 0) return;
    dir0->cd();
    ///
    
    TString runNumber = dir0->GetName();
    TString mDir =  runNumber+"/Muon/MuonTrkPhysMonitoring/NoTrigger/Detector_Specific/";
    if (! f->cd(mDir)) return;
    //std::cout<<"jk: in directory "<<mDir<<std::endl;
    TIter nextcd1(gDirectory->GetListOfKeys());
    while(TKey* key1 = dynamic_cast<TKey*>(nextcd1())) {//While in Staco/Muid/Third chain
      TString recalg_fullStr = mDir+key1->GetName();
      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
      if(!dir1) continue;
      dir1->cd();
      
      
      
      //MDT HitResiduals:
      
      //std::cout<<"ab: MDT HitResiduals!\n";
      
      TH2F* m_HitRes_Eta_Phi_Mean_I = (TH2F*)dir1->Get("m_HitRes_Eta_Phi_Mean_I");
      TH2F* m_HitRes_Eta_Phi_Mean_M = (TH2F*)dir1->Get("m_HitRes_Eta_Phi_Mean_M");
      TH2F* m_HitRes_Eta_Phi_Mean_O = (TH2F*)dir1->Get("m_HitRes_Eta_Phi_Mean_O");
      TH2F* m_HitRes_Eta_Phi_Width_I = (TH2F*)dir1->Get("m_HitRes_Eta_Phi_Width_I");
      TH2F* m_HitRes_Eta_Phi_Width_M = (TH2F*)dir1->Get("m_HitRes_Eta_Phi_Width_M");
      TH2F* m_HitRes_Eta_Phi_Width_O = (TH2F*)dir1->Get("m_HitRes_Eta_Phi_Width_O");
      
      TH1D* HitResProfile = 0;
      TH2F* HitResHist = 0;
      
      for(int phi_itr = 0; phi_itr < m_HitRes_Eta_Phi_Mean_I->GetNbinsY(); phi_itr++){
        int j = phi_itr+1;
        
        for(int eta_itr = 0; eta_itr < m_HitRes_Eta_Phi_Mean_I->GetNbinsX(); eta_itr++){
          int i = eta_itr+1;
          
          if (m_HitRes_Eta_Phi_Mean_I && m_HitRes_Eta_Phi_Width_I) {
            HitResHist = (TH2F*)dir1->Get("m_HR_Eta_"+phi_vec.at(phi_itr)+"_I");
            if(HitResHist) HitResProfile = HitResHist->ProjectionX("_pfx",eta_itr+1,eta_itr+1);
              
            m_HitRes_Eta_Phi_Mean_I->SetBinContent(i,j,0.0);
            m_HitRes_Eta_Phi_Mean_I->SetBinError  (i,j,0.0);
            m_HitRes_Eta_Phi_Width_I->SetBinContent(i,j,0.0);
            m_HitRes_Eta_Phi_Width_I->SetBinError  (i,j,0.0);
            
            float results[4] = {0.0,0.0,0.0,0.0};
            if(HitResProfile) HitResProfile->Rebin(8);
            DGausFit(HitResProfile,results,5,false);
            
            m_HitRes_Eta_Phi_Mean_I->SetBinContent (i,j,results[0]);
            m_HitRes_Eta_Phi_Mean_I->SetBinError   (i,j,results[1]);
            m_HitRes_Eta_Phi_Width_I->SetBinContent(i,j,results[2]);
            m_HitRes_Eta_Phi_Width_I->SetBinError  (i,j,results[3]);
            
            delete HitResProfile;
            
          }
          
          //------
          
          if (m_HitRes_Eta_Phi_Mean_M && m_HitRes_Eta_Phi_Width_M) {
            HitResHist = (TH2F*)dir1->Get("m_HR_Eta_"+phi_vec.at(phi_itr)+"_M");
            if(HitResHist) HitResProfile = HitResHist->ProjectionX("_pfx",eta_itr+1,eta_itr+1);
            
            m_HitRes_Eta_Phi_Mean_M->SetBinContent(i,j,0.0);
            m_HitRes_Eta_Phi_Mean_M->SetBinError  (i,j,0.0);
            m_HitRes_Eta_Phi_Width_M->SetBinContent(i,j,0.0);
            m_HitRes_Eta_Phi_Width_M->SetBinError  (i,j,0.0);
            
            float results[4] = {0.0,0.0,0.0,0.0};
            if(HitResProfile) HitResProfile->Rebin(8);
            DGausFit(HitResProfile,results,5,false);
            
            m_HitRes_Eta_Phi_Mean_M->SetBinContent (i,j,results[0]);
            m_HitRes_Eta_Phi_Mean_M->SetBinError   (i,j,results[1]);
            m_HitRes_Eta_Phi_Width_M->SetBinContent(i,j,results[2]);
            m_HitRes_Eta_Phi_Width_M->SetBinError  (i,j,results[3]);
           
            
            delete HitResProfile;
          }
          
          //------
          if (m_HitRes_Eta_Phi_Mean_O && m_HitRes_Eta_Phi_Width_O) {
            HitResHist = (TH2F*)dir1->Get("m_HR_Eta_"+phi_vec.at(phi_itr)+"_O");
            if(HitResHist) HitResProfile = HitResHist->ProjectionX("_pfx",eta_itr+1,eta_itr+1);
            
            m_HitRes_Eta_Phi_Mean_O->SetBinContent(i,j,0.0);
            m_HitRes_Eta_Phi_Mean_O->SetBinError  (i,j,0.0);
            m_HitRes_Eta_Phi_Width_O->SetBinContent(i,j,0.0);
            m_HitRes_Eta_Phi_Width_O->SetBinError  (i,j,0.0);
            
            float results[4] = {0.0,0.0,0.0,0.0};
            if(HitResProfile) HitResProfile->Rebin(8);
            DGausFit(HitResProfile,results,5,false);
            
            m_HitRes_Eta_Phi_Mean_O->SetBinContent (i,j,results[0]);
            m_HitRes_Eta_Phi_Mean_O->SetBinError   (i,j,results[1]);
            m_HitRes_Eta_Phi_Width_O->SetBinContent(i,j,results[2]);
            m_HitRes_Eta_Phi_Width_O->SetBinError  (i,j,results[3]);
            
            
            delete HitResProfile;
          }
        }
      }
      
      if(m_HitRes_Eta_Phi_Mean_I)  m_HitRes_Eta_Phi_Mean_I->Write("",TObject::kOverwrite);
      if(m_HitRes_Eta_Phi_Mean_M)  m_HitRes_Eta_Phi_Mean_M->Write("",TObject::kOverwrite);
      if(m_HitRes_Eta_Phi_Mean_O)  m_HitRes_Eta_Phi_Mean_O->Write("",TObject::kOverwrite);
      if(m_HitRes_Eta_Phi_Width_I) m_HitRes_Eta_Phi_Width_I->Write("",TObject::kOverwrite);
      if(m_HitRes_Eta_Phi_Width_M) m_HitRes_Eta_Phi_Width_M->Write("",TObject::kOverwrite);
      if(m_HitRes_Eta_Phi_Width_O) m_HitRes_Eta_Phi_Width_O->Write("",TObject::kOverwrite);
      
      // BEE
      //std::cout<<"ab: BEE HitResiduals!\n";
      for (unsigned int side_itr = 0; side_itr < side_vec.size(); side_itr++) {
        
        TH1F* h_UHitRes = (TH1F*)dir1->Get("m_"+side_vec.at(side_itr)+"_BEE_UHitRes");
        if(!h_UHitRes) continue;
        
        TH1F* h_residual= 0;
        
        for(unsigned int phi_itr = 0; phi_itr < phi_vec.size(); phi_itr++){
          
          h_residual = (TH1F*)dir1->Get("m_"+side_vec.at(side_itr)+"_BEE_"+phi_vec.at(phi_itr));
          if (!h_residual) continue;
          h_residual->Rebin(8);
          
          if (phi_itr%2 == 0) {
            h_UHitRes->SetBinContent (phi_itr+1,0.0);//Mean
            h_UHitRes->SetBinError   (phi_itr+1,0.0);//Width
            continue;
          }
          
          h_UHitRes->SetBinContent(phi_itr+1,0.0);
          h_UHitRes->SetBinError  (phi_itr+1,0.0);
          
          float results[4] = {0.0,0.0,0.0,0.0};
          
          DGausFit(h_residual,results,4.);
          
          h_UHitRes->SetBinContent (phi_itr+1,results[0]);//Mean
          h_UHitRes->SetBinError   (phi_itr+1,results[2]/sqrt(h_residual->GetEntries()));//Width
        }
        
        h_UHitRes->Write("",TObject::kOverwrite);
        
      }
      
      //EE
      //std::cout<<"ab: EE HitResiduals!\n";
      for (unsigned int side_itr = 0; side_itr < side_vec.size(); side_itr++) {
        
        TH1F* h_UHitRes = (TH1F*)dir1->Get("m_"+side_vec.at(side_itr)+"_EE_UHitRes");
        if(!h_UHitRes) continue;
        
        TH1F* h_residual;
        
        for(unsigned int phi_itr = 0; phi_itr < phi_vec.size(); phi_itr++){
          
          h_residual = (TH1F*)dir1->Get("m_"+side_vec.at(side_itr)+"_EE_"+phi_vec.at(phi_itr));
          if (!h_residual) continue; 
          h_residual->Rebin(8);
          
          //only 3, 5, 11, 13 installed on A side
          if (side_itr == 0 && !(phi_itr == 2 || phi_itr == 4 || phi_itr == 10 || phi_itr == 12) )
          {
            h_UHitRes->SetBinContent (phi_itr+1,0.0);//Mean
            h_UHitRes->SetBinError   (phi_itr+1,0.0);//Width
            continue;
          }
          
          h_UHitRes->SetBinContent(phi_itr+1,0.0);
          h_UHitRes->SetBinError  (phi_itr+1,0.0);
          
          float results[4] = {0.0,0.0,0.0,0.0};
          
          DGausFit(h_residual,results,5./2.);
          
          h_UHitRes->SetBinContent (phi_itr+1,results[0]);//Mean
          h_UHitRes->SetBinError   (phi_itr+1,results[2]/sqrt(h_residual->GetEntries()));//Width
        }
        
        h_UHitRes->Write("",TObject::kOverwrite);
      }
      
      //CSC
      //std::cout<<"ab: CSC HitResiduals!\n";
      for (unsigned int side_itr = 0; side_itr < side_vec.size(); side_itr++) {
        
        TH1F* h_UHitRes = (TH1F*)dir1->Get("m_"+side_vec.at(side_itr)+"_CSC_UHitRes");
        if(!h_UHitRes) continue;
        
        TH1F* h_residual;
        
        for(unsigned int phi_itr = 0; phi_itr < phi_vec.size(); phi_itr++){
          
          h_residual = (TH1F*)dir1->Get("m_"+side_vec.at(side_itr)+"_CSC_UHitRes_"+phi_vec.at(phi_itr));
          if (!h_residual) continue;
          h_residual->Rebin(8);
          
          h_UHitRes->SetBinContent(phi_itr+1,0.0);
          h_UHitRes->SetBinError  (phi_itr+1,0.0);
          
          float results[4] = {0.0,0.0,0.0,0.0};
          
          DGausFit(h_residual,results,5./2.);
          
          h_UHitRes->SetBinContent (phi_itr+1,results[0]);//Mean
          h_UHitRes->SetBinError   (phi_itr+1,results[2]/sqrt(h_residual->GetEntries()));//Width
        }
        
        h_UHitRes->Write("",TObject::kOverwrite);
      }
      
      
      //std::cout<<"ab: MDT Track Residuals!\n";    
      TH1F* m_MDT_TrackResiduals = (TH1F*)dir1->Get("m_MDT_TrackResiduals");
      TH1F * h_residual = 0;
      
      if(m_MDT_TrackResiduals){
        for(unsigned int st_itr = 0; st_itr < string_vec.size(); st_itr++){
          
          h_residual = (TH1F*)dir1->Get("m_MDT_TR_"+string_vec.at(st_itr));
          if (!h_residual) continue;
          h_residual->Rebin(8);
          
          m_MDT_TrackResiduals->SetBinContent(st_itr+1,0.0);
          m_MDT_TrackResiduals->SetBinError  (st_itr+1,0.0);
          
          float results[4] = {0.0,0.0,0.0,0.0};
          
          DGausFit(h_residual,results,3.);
          
          m_MDT_TrackResiduals->SetBinContent (st_itr+1,results[0]);//Mean
          m_MDT_TrackResiduals->SetBinError   (st_itr+1,results[2]/sqrt(h_residual->GetEntries()));//Width
          
        }
        
        m_MDT_TrackResiduals->Write("",TObject::kOverwrite);
      }
      
      
    }
    
    f->Close();
    delete f;
    
    return;
    
  } //End Detector-specific function
  
  
  void MonitoringFile::MuonTrkPhys_BField( std::string inFilename ){
    
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
    
    if (f == 0) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file not opened \n";
      return;
    }
    if(f->GetSize()<1000.) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file empty \n";
      return; 
    }
    
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    if (key0 == 0) return;
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    if (dir0 == 0) return;
    dir0->cd();
    ///
    
    TString runNumber = dir0->GetName();
    TString mDir =  runNumber+"/Muon/MuonTrkPhysMonitoring/NoTrigger/B_Field_Alignment/";
    if (! f->cd(mDir)) return;
    //std::cout<<"jk: in directory "<<mDir<<std::endl;
    TIter nextcd1(gDirectory->GetListOfKeys());
    while(TKey* key1 = dynamic_cast<TKey*>(nextcd1())) {//While in Staco/Muid/Third chain
      TString recalg_fullStr = mDir+key1->GetName();
      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
      if(!dir1) continue;
      dir1->cd();
      
      TH1F* m_oPt_ms_id_sum = (TH1F*)dir1->Get("m_oPt_ms_id_sum");
      TH1F* m_oPt_ms_id_sum_entries = (TH1F*)dir1->Get("m_oPt_ms_id_sum_entries");
      TH1F* m_oPt_ms_id_sum_weights = (TH1F*)dir1->Get("m_oPt_ms_id_sum_weights");
      TH1F* m_oPt_ms_id_sum_mean = (TH1F*)dir1->Get("m_oPt_ms_id_sum_mean");
      
      SimpleMean(m_oPt_ms_id_sum_mean,m_oPt_ms_id_sum_weights,m_oPt_ms_id_sum_entries,m_oPt_ms_id_sum);
      
      TH1F* m_LSag          = (TH1F*)dir1->Get("m_LSag");
      TH1F* m_LSag_entries  = (TH1F*)dir1->Get("m_LSag_entries");
      TH1F* m_LSag_weights  = (TH1F*)dir1->Get("m_LSag_weights");
      TH1F* m_LSag_mean     = (TH1F*)dir1->Get("m_LSag_mean");
      
      SimpleMean(m_LSag_mean,m_LSag_weights,m_LSag_entries,m_LSag);
      
      
      TH2F* m_LSag_eta_phi_entries  = (TH2F*)dir1->Get("m_LSag_eta_phi_entries");
      TH2F* m_LSag_eta_phi_weights  = (TH2F*)dir1->Get("m_LSag_eta_phi_weights");
      TH2F* m_LSag_eta_phi_mean     = (TH2F*)dir1->Get("m_LSag_eta_phi_mean");
      
      SimpleMean(m_LSag_eta_phi_mean,m_LSag_eta_phi_weights,m_LSag_eta_phi_entries,m_LSag);
      
    }
    
    f->Close();
    delete f;
    
    return;
    
  } //End B-Field function
  
  
  void MonitoringFile::MuonTrkPhys_Alignment( std::string inFilename ){
    
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
    
    if (f == 0) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file not opened \n";
      return;
    }
    if(f->GetSize()<1000.) {
      std::cerr << "MuonTrkPhys(): "
      << "Input file empty \n";
      return; 
    }
    
    //Set Constants
    //delta z0 by eta, phi
    std::vector<TString> sectors;
    sectors.push_back("_S01");
    sectors.push_back("_S02");
    sectors.push_back("_S03");
    sectors.push_back("_S04");
    sectors.push_back("_S05");
    sectors.push_back("_S06");
    sectors.push_back("_S07");
    sectors.push_back("_S08");
    sectors.push_back("_S09");
    sectors.push_back("_S10");
    sectors.push_back("_S11");
    sectors.push_back("_S12");
    sectors.push_back("_S13");
    sectors.push_back("_S14");
    sectors.push_back("_S15");
    sectors.push_back("_S16");
    
    // get run directory name
    //Seemingly unnecessary lines are necessary
    TIter nextcd0(gDirectory->GetListOfKeys());
    TKey *key0 = (TKey*)nextcd0();
    if (key0 == 0) return;
    TDirectory *dir0= dynamic_cast<TDirectory*> (key0->ReadObj());
    if (dir0 == 0) return;
    dir0->cd();
    ///
    
    TString runNumber = dir0->GetName();
    TString mDir =  runNumber+"/Muon/MuonTrkPhysMonitoring/NoTrigger/MS_ID_Alignment/";
    if (! f->cd(mDir)) return;
    //std::cout<<"jk: in directory "<<mDir<<std::endl;
    TIter nextcd1(gDirectory->GetListOfKeys());
    while(TKey* key1 = dynamic_cast<TKey*>(nextcd1())) {//While in Staco/Muid/Third chain
      TString recalg_fullStr = mDir+key1->GetName();
      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
      if(!dir1) continue;
      dir1->cd();
      
      //Q/pT, positive
      TH1F* m_QoPt_ms_id_sum_pos = (TH1F*)dir1->Get("m_QoPt_ms_id_sum_pos");
      TH1F* m_QoPt_ms_id_sum_entries_pos = (TH1F*)dir1->Get("m_QoPt_ms_id_sum_entries_pos");
      TH1F* m_QoPt_ms_id_sum_weights_pos = (TH1F*)dir1->Get("m_QoPt_ms_id_sum_weights_pos");
      TH1F* m_QoPt_ms_id_sum_mean_pos = (TH1F*)dir1->Get("m_QoPt_ms_id_sum_mean_pos");
      
      SimpleMean(m_QoPt_ms_id_sum_mean_pos,m_QoPt_ms_id_sum_weights_pos,m_QoPt_ms_id_sum_entries_pos,m_QoPt_ms_id_sum_pos);
      
      //Q/pT, negative
      TH1F* m_QoPt_ms_id_sum_neg = (TH1F*)dir1->Get("m_QoPt_ms_id_sum_neg");
      TH1F* m_QoPt_ms_id_sum_entries_neg = (TH1F*)dir1->Get("m_QoPt_ms_id_sum_entries_neg");
      TH1F* m_QoPt_ms_id_sum_weights_neg = (TH1F*)dir1->Get("m_QoPt_ms_id_sum_weights_neg");
      TH1F* m_QoPt_ms_id_sum_mean_neg = (TH1F*)dir1->Get("m_QoPt_ms_id_sum_mean_neg");
      
      SimpleMean(m_QoPt_ms_id_sum_mean_neg,m_QoPt_ms_id_sum_weights_neg,m_QoPt_ms_id_sum_entries_neg,m_QoPt_ms_id_sum_neg);
      
      
      //1/pT, positive
      TH1F* m_oPt_sum_pos = (TH1F*)dir1->Get("m_oPt_sum_pos");
      TH1F* m_oPt_sum_entries_pos = (TH1F*)dir1->Get("m_oPt_sum_entries_pos");
      TH1F* m_oPt_sum_weights_pos = (TH1F*)dir1->Get("m_oPt_sum_weights_pos");
      TH1F* m_oPt_sum_mean_pos = (TH1F*)dir1->Get("m_oPt_sum_mean_pos");
      
      SimpleMean(m_oPt_sum_mean_pos,m_oPt_sum_weights_pos,m_oPt_sum_entries_pos,m_oPt_sum_pos);
      
      
      //1/pT, negative
      TH1F* m_oPt_sum_neg = (TH1F*)dir1->Get("m_oPt_sum_neg");
      TH1F* m_oPt_sum_entries_neg = (TH1F*)dir1->Get("m_oPt_sum_entries_neg");
      TH1F* m_oPt_sum_weights_neg = (TH1F*)dir1->Get("m_oPt_sum_weights_neg");
      TH1F* m_oPt_sum_mean_neg = (TH1F*)dir1->Get("m_oPt_sum_mean_neg");
      
      SimpleMean(m_oPt_sum_mean_neg,m_oPt_sum_weights_neg,m_oPt_sum_entries_neg,m_oPt_sum_neg);
      
      
      //Relative p, positive
      TH1F* m_rel_p_ms_id_sum_pos = (TH1F*)dir1->Get("m_rel_p_ms_id_sum_pos");
      TH2F* m_rel_p_ms_id_sum_entries_pos = (TH2F*)dir1->Get("m_rel_p_ms_id_sum_entries_pos");
      TH2F* m_rel_p_ms_id_sum_weights_pos = (TH2F*)dir1->Get("m_rel_p_ms_id_sum_weights_pos");
      TH2F* m_rel_p_ms_id_sum_mean_pos = (TH2F*)dir1->Get("m_rel_p_ms_id_sum_mean_pos");
      
      SimpleMean(m_rel_p_ms_id_sum_mean_pos,m_rel_p_ms_id_sum_weights_pos,m_rel_p_ms_id_sum_entries_pos,m_rel_p_ms_id_sum_pos);
      
      
      
      //Relative p, negative
      TH1F* m_rel_p_ms_id_sum_neg = (TH1F*)dir1->Get("m_rel_p_ms_id_sum_neg");
      TH2F* m_rel_p_ms_id_sum_entries_neg = (TH2F*)dir1->Get("m_rel_p_ms_id_sum_entries_neg");
      TH2F* m_rel_p_ms_id_sum_weights_neg = (TH2F*)dir1->Get("m_rel_p_ms_id_sum_weights_neg");
      TH2F* m_rel_p_ms_id_sum_mean_neg = (TH2F*)dir1->Get("m_rel_p_ms_id_sum_mean_neg");
      
      SimpleMean(m_rel_p_ms_id_sum_mean_neg,m_rel_p_ms_id_sum_weights_neg,m_rel_p_ms_id_sum_entries_neg,m_rel_p_ms_id_sum_neg);
      
      
      // 1/pT: positive - negative
      TH1F* m_oPt_Diff = (TH1F*)dir1->Get("m_oPt_Diff");
      if(m_oPt_Diff){
        
        for(int j = 1; j <= m_oPt_Diff->GetNbinsX(); j++){
          m_oPt_Diff->SetBinContent(j, m_oPt_sum_mean_pos->GetBinContent(j) - m_oPt_sum_mean_neg->GetBinContent(j));
          m_oPt_Diff->SetBinError(j, sqrt(m_oPt_sum_mean_pos->GetBinError(j)*m_oPt_sum_mean_pos->GetBinError(j)+
                                          m_oPt_sum_mean_neg->GetBinError(j)*m_oPt_sum_mean_neg->GetBinError(j)));
        }
        m_oPt_Diff->Write("",TObject::kOverwrite);
      }
      
      //chi2 per ndof
      TH1F* m_chi2ndof_CB = (TH1F*)dir1->Get("m_chi2ndof_CB");
      TH1F* m_chi2ndof_CB_entries = (TH1F*)dir1->Get("m_chi2ndof_CB_entries");
      TH1F* m_chi2ndof_CB_weights = (TH1F*)dir1->Get("m_chi2ndof_CB_weights");
      TH1F* m_chi2ndof_CB_mean = (TH1F*)dir1->Get("m_chi2ndof_CB_mean");
      
      SimpleMean(m_chi2ndof_CB_mean,m_chi2ndof_CB_weights,m_chi2ndof_CB_entries,m_chi2ndof_CB);
      
      
      
      TH2F* m_deltaZ0_CB_Eta_Phi = (TH2F*)dir1->Get("m_deltaZ0_CB_Eta_Phi");
      TH2F* m_deltaZ0_CB_Eta_Phi_Width = (TH2F*)dir1->Get("m_deltaZ0_CB_Eta_Phi_Width");
      if(m_deltaZ0_CB_Eta_Phi && m_deltaZ0_CB_Eta_Phi_Width){
        for(int isec = 0; isec < m_deltaZ0_CB_Eta_Phi->GetNbinsY(); isec++){
          TH2F* deltaZ0 = (TH2F*)dir1->Get("m_deltaZ0_CB"+sectors.at(isec));
          
          TH1D * HitResProfile;
          
          for(int j = 1; j <= deltaZ0->GetNbinsX(); j++){
            HitResProfile = deltaZ0->ProjectionX("_pfx",j,j);
            
            float results[4] = {0.0,0.0,0.0,0.0};
            DGausFit(HitResProfile,results,5,false,FLT_MAX,FLT_MAX);
            
            m_deltaZ0_CB_Eta_Phi->SetBinContent(j, isec+1, results[0]);
            m_deltaZ0_CB_Eta_Phi_Width->SetBinContent(j, isec+1, results[2]);            
          }
          
        }
        
        
        m_deltaZ0_CB_Eta_Phi->Write("",TObject::kOverwrite);
        m_deltaZ0_CB_Eta_Phi_Width->Write("",TObject::kOverwrite);
      }
      
      TH2F* m_deltaZ0_CB = (TH2F*)dir1->Get("m_deltaZ0_CB");
      TH1F* m_deltaZ0_CB_Eta_AllSectors = (TH1F*)dir1->Get("m_deltaZ0_CB_Eta_AllSectors");
      
      if(m_deltaZ0_CB && m_deltaZ0_CB_Eta_AllSectors){
        
        TH1D * HitResProfile;
        
        for(int j = 1; j <= m_deltaZ0_CB_Eta_AllSectors->GetNbinsX(); j++){
          HitResProfile = m_deltaZ0_CB->ProjectionX("_pfx",j,j);
          
          
          float results[4] = {0.0,0.0,0.0,0.0};
          DGausFit(HitResProfile,results,5,false,FLT_MAX,FLT_MAX);
          
          m_deltaZ0_CB_Eta_AllSectors->SetBinContent(j,results[0]);
          m_deltaZ0_CB_Eta_AllSectors->SetBinError(j,HitResProfile->GetEntries() ? results[2]/std::sqrt(HitResProfile->GetEntries()) : 0.0);
        }
        
        
        m_deltaZ0_CB_Eta_AllSectors->Write("",TObject::kOverwrite);
      }
      
    }
    
    f->Close();
    delete f;
    
  } //End MS-ID Alignment
  
  
  Double_t MonitoringFile::MuonTrkPhys_BWgaus(Double_t *x, Double_t *par) 
  {
    //printf("BreitGaus input parameter %f %f %f \n",par[0],par[1],par[2]);
    //Fit parameters:
    //par[0]=Width (scale) Breit-Wigner
    //par[1]=Most Probable (MP, location) Breit mean
    //par[2]=Width (sigma) of convoluted Gaussian function
    //
    //
    //In the Landau distribution (represented by the CERNLIB approximation),
    //the maximum is located at x=-0.22278298 with the location parameter=0.
    //This shift is corrected within this function, so that the actual
    //maximum is identical to the MP parameter.
    
    // Numeric constants
    Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
                                           //Double_t twoPi = 6.2831853071795;//2Pi
    
    // Control constants
    Double_t np = 100.0;      // number of convolution steps
    Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
    
    // Variables
    Double_t xx;
    Double_t fland;
    Double_t sum = 0.0;
    Double_t xlow,xupp;
    Double_t step;
    Double_t i;
    
    // Range of convolution integral
    xlow = x[0] - sc * par[2];
    xupp = x[0] + sc * par[2];
    
    step = (xupp-xlow) / np;
    
    // Convolution integral of Breit and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
      xx = xlow + (i-.5) * step;
      fland = TMath::BreitWigner(xx,par[1],par[0]);
      sum += fland * TMath::Gaus(x[0],xx,par[2]);
      
      xx = xupp - (i-.5) * step;
      fland = TMath::BreitWigner(xx,par[1],par[0]);
      sum += fland * TMath::Gaus(x[0],xx,par[2]);
    } 
    return (step * sum * invsq2pi / par[2]);
  }
  
  Double_t MonitoringFile::MuonTrkPhys_fitFunctionS(Double_t *x, Double_t *par) {return par[0]*(MuonTrkPhys_BWgaus(x,&par[1])) + par[4];}
  
} //namespace
