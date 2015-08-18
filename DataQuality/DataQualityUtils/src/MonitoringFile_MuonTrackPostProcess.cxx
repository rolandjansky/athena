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

void TwoDto2D_Eff(TH2 * Numerator, TH2 * Denominator, TH2 * Efficiency){
  //the input histograms must have the same dimension!
  if (Numerator == NULL || Denominator == NULL || Efficiency == NULL) {return;}
  // if(rebin2d){
  //   Numerator->Rebin2D();//here change the default binnning of eta-phi
  //   Efficiency->Rebin2D();//here change the default binnning of eta-phi
  // }
  //then check the dimensions
  int n_xbins = Numerator->GetNbinsX();
  if (Denominator->GetNbinsX() != n_xbins|| Efficiency->GetNbinsX() != n_xbins) {return;}
  int n_ybins = Numerator->GetNbinsY();
  if (Denominator->GetNbinsY() != n_ybins|| Efficiency->GetNbinsY() != n_ybins) {return;}

  //after protection
  for(int bin_itrX = 1; bin_itrX < Efficiency->GetNbinsX() + 1; bin_itrX++){
    for(int bin_itrY = 1; bin_itrY < Efficiency->GetNbinsY() + 1; bin_itrY++){
      if (Denominator->GetBinContent(bin_itrX, bin_itrY) == 0) continue;
      Efficiency->SetBinContent(bin_itrX, bin_itrY, 
        Numerator->GetBinContent(bin_itrX, bin_itrY)/Denominator->GetBinContent(bin_itrX, bin_itrY));
    }
  } 
  Efficiency->Write("",TObject::kOverwrite);
  return;
}

void TwoDto1D_Mean(TH2 * m_parent, TH1 * m_child, int rebinning = 2){
  //the input histograms must have the same dimension!
  if (m_parent == NULL || m_child == NULL ) {return;}
  if (m_parent->GetNbinsX() != m_child->GetNbinsX()){return;}
  //after protection
  for(int bin_itrX = 1; bin_itrX < m_parent->GetNbinsX() + 1; bin_itrX++){
    double parent_event = 0;
    double parent_sum = 0;
    for(int bin_itrY = 1; bin_itrY < m_parent->GetNbinsY() + 1; bin_itrY++){
      parent_event += m_parent->GetBinContent(bin_itrX, bin_itrY);
      parent_sum += m_parent->GetBinContent(bin_itrX, bin_itrY) * m_parent->GetYaxis()->GetBinCenter(bin_itrY);
    }
    if (parent_event == 0){continue;}
    m_child->SetBinContent(bin_itrX, parent_sum/parent_event);
  } 
  TString sHistTitle = m_child->GetTitle();
  m_child->Rebin(rebinning);
  m_child->SetTitle(sHistTitle + " per event");
  m_child->Write("",TObject::kOverwrite);
  return;
}

void TwoDto1D_Sum(TH2 * m_parent, TH1 * m_child, int rebinning = 2){
  //the input histograms must have the same dimension!
  if (m_parent == NULL || m_child == NULL ) {return;}
  if (m_parent->GetNbinsX() != m_child->GetNbinsX()){return;}
  //after protection
  for(int bin_itrX = 1; bin_itrX < m_parent->GetNbinsX() + 1; bin_itrX++){
    double parent_sum = 0;
    for(int bin_itrY = 1; bin_itrY < m_parent->GetNbinsY() + 1; bin_itrY++){
      parent_sum += m_parent->GetBinContent(bin_itrX, bin_itrY) * m_parent->GetYaxis()->GetBinCenter(bin_itrY);
    }
    if(parent_sum == 0){continue;}
    m_child->SetBinContent(bin_itrX, parent_sum);
  }
  m_child->Rebin(rebinning); 
  m_child->Write("",TObject::kOverwrite);
  return;
}

void SetMassInfo(int iBin, TH1* InputHist, TH1* OutMean, TH1* OutSigma){
	if (InputHist == NULL || OutMean == NULL || OutSigma == NULL) {return;}
    OutMean-> SetBinContent(iBin, InputHist->GetMean(1));
    OutMean-> SetBinError(  iBin, InputHist->GetMeanError(1));
    OutSigma->SetBinContent(iBin, InputHist->GetRMS(1));
    return;
}

namespace dqutils {
  
  //main function
  void MonitoringFile::MuonTrackPostProcess( std::string inFilename, bool isIncremental){
    if (isIncremental) {return;}
    MonitoringFile::MuonTrack_Main( inFilename , "");
    MonitoringFile::MuonTrack_Main( inFilename,  "NoTrig/");
    return;
  }
  
  //subfunctions
  void MonitoringFile::MuonTrack_Main(std::string inFilename, TString dirname){
    TString plotdirname = dirname;//set the plottting dir anme
    plotdirname.ReplaceAll("/", "_");//name clean
    dirname = "MuonPhysics/" + dirname;//give it the full path
    TFile* f = TFile::Open(inFilename.c_str(),"UPDATE");
    
    if (f == 0) {
      std::cerr << "MuonTrackMonitoring(): " << "Input file not opened \n";
      return;
    }
    if(f->GetSize() < 1000.) {
      std::cerr << "MuonTrackMonitoring(): " << "Input file empty \n";
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
    TString runNumber = dir0->GetName();
    TString motherDir = runNumber + "/" + dirname;
    ///finish getting basci information
    
    //Do the segment part
    TString mDir =  motherDir + "Segments/";
    if (! f->cd(mDir)) return;
    TIter nextcd1(gDirectory->GetListOfKeys());
    while(TKey* key1 = dynamic_cast<TKey*>(nextcd1())) {
      //While in the segments
      TString recalg_path = key1->GetName();
      TString recalg_fullStr = mDir + key1->GetName();
      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
      if(!dir1) continue;
      dir1->cd();
      
      // Divide the efficiency histograms
      TH2F* m_EffNumerator = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_chamberIndex_perSector_numerator", plotdirname.Data(), recalg_path.Data()));
      TH2F* m_EffDenominator = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_chamberIndex_perSector_denominator", plotdirname.Data(), recalg_path.Data()));
      TH2F* m_Efficiency = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_chamberIndex_perSector", plotdirname.Data(), recalg_path.Data()));
      
      TwoDto2D_Eff(m_EffNumerator, m_EffDenominator, m_Efficiency);
    }//ends different subfolder for segment efficiency
    
    //Do the muon part
    TString mDir_muons = motherDir + "Muons/";
    if (! f->cd(mDir_muons)) return;
    TIter nextcd_muons(gDirectory->GetListOfKeys());
    while(TKey* key1 = dynamic_cast<TKey*>(nextcd_muons())){
      //While in the segments
      TString recalg_path = key1->GetName();
      TString recalg_fullStr = mDir_muons + key1->GetName();
      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
      if(!dir1) continue;
      dir1->cd();
      
      TString muonqualstr[4] = {"Tight", "Medium", "Loose", "Veryloose"};
      // Divide the efficiency histograms
      TH2F* m_EffDenominator = (TH2F*)dir1->Get(Form("%sMuons_%s_eta_phi", plotdirname.Data(), recalg_path.Data()));
      //m_EffDenominator->Rebin2D();//here change the default binnning of eta-phi
      for (int i = 0; i < 4; i++){
        TH2F* m_EffNumerator = (TH2F*)dir1->Get(Form("%sMuons_%s_%s_eta_phi", plotdirname.Data(), recalg_path.Data(), muonqualstr[i].Data()));
        TH2F* m_Efficiency = (TH2F*)dir1->Get(Form("%sMuons_%s_%s_eff", plotdirname.Data(), recalg_path.Data(), muonqualstr[i].Data()));
        TwoDto2D_Eff(m_EffNumerator, m_EffDenominator, m_Efficiency);//here change the default binnning of eta-phi
      }
    }//ends different subfolder for muon efficiency


    //Do the luminoisty part
    TString mDir_lb = motherDir + "Overview/";
    if (! f->cd(mDir_lb)) return;
    TIter nextcd_lb(gDirectory->GetListOfKeys());
    while(TKey* key1 = dynamic_cast<TKey*>(nextcd_lb())){
      //While in the segments
      TString recalg_path = key1->GetName();
      TString recalg_fullStr = mDir_lb + key1->GetName();
      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
      if(!dir1) continue;
      dir1->cd();
      
      TString montype[3] = {"Segment", "MuonTrack", "Muon"};
      // Divide the efficiency histograms
      for (int i = 0; i < 3; i++){
        TH2F* m_parent_lb = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_LB_2D", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        TH1F* m_child_lb = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_LB", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        TH2F* m_parent_inst = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_Inst_2D", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        TH1F* m_child_inst = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_Inst", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        TH2F* m_parent_intlumi = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_IntLumi_2D", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        TH1F* m_child_intlumi = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_IntLumi", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        TwoDto1D_Mean(m_parent_lb, m_child_lb, 5);
        TwoDto1D_Mean(m_parent_inst, m_child_inst);
        TwoDto1D_Mean(m_parent_intlumi, m_child_intlumi);
      }
      TString resonance[2] = {"Z", "Jpsi"};
      // Divide the efficiency histograms
      for (int i = 0; i < 2; i++){
        TH2F* m_parent_lb = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_LB_2D", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        TH1F* m_child_lb = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_LB", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        TH2F* m_parent_inst = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_Inst_2D", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        TH1F* m_child_inst = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_Inst", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        TH2F* m_parent_intlumi = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_IntLumi_2D", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        TH1F* m_child_intlumi = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_IntLumi", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        TwoDto1D_Sum(m_parent_lb, m_child_lb, 5);
        TwoDto1D_Sum(m_parent_inst, m_child_inst);
        TwoDto1D_Sum(m_parent_intlumi, m_child_intlumi);
      }
    }//ends different subfolder for luminosity


    //Do the muon part; only for the main directory!
    if (!dirname.Contains("NoTrig")){
      //std::cout << "get to trackphys " << std::endl;
	    TString mDir_phys = motherDir + "MuonTrkPhys/";
	    if (! f->cd(mDir_phys)) return;
	    TIter nextcd_phys(gDirectory->GetListOfKeys());
	    while(TKey* key1 = dynamic_cast<TKey*>(nextcd_phys())){
	      //While in the segments
	      TString recalg_path = key1->GetName();
	      TString recalg_fullStr = mDir_phys + key1->GetName();
	      TDirectory* dir1 = f->GetDirectory(recalg_fullStr);
	      if(!dir1) continue;
	      dir1->cd();
	      
	      TH1* m_Mass_Mean = (TH1F*)dir1->Get(Form("m_%s_M_Mean", recalg_path.Data()));
	      TH1* m_Mass_Sigma = (TH1F*)dir1->Get(Form("m_%s_M_Sigma", recalg_path.Data()));
	      // Get each of the mass histograms
	      TString det_region[4] = {"EC", "BC", "BA", "EA"};
	      for (int i = 0; i < 4; i++){
	        for(int j = 0; j< 4; j++){
	        	TH1* m_Mass_region = (TH1F*)dir1->Get(Form("m_%s_M_%s_%s", recalg_path.Data(), 
	        		det_region[i].Data(), det_region[j].Data()));
	        	//std::cout << " bin " << i * 4 + (j + 1) << " content " << det_region[i] << " " << det_region[j] << std::endl;
	        	SetMassInfo(i * 4 + (j + 1), m_Mass_region, m_Mass_Mean, m_Mass_Sigma);
	        }
	      }
	      if (m_Mass_Mean != NULL) m_Mass_Mean->Write("",TObject::kOverwrite);
	      if (m_Mass_Sigma != NULL) m_Mass_Sigma->Write("",TObject::kOverwrite);
	    }
    }//ends different subfolder for muon efficiency

    f->Close();
    delete f;
    return;
  } //ends function
  
  
  
} //namespace
