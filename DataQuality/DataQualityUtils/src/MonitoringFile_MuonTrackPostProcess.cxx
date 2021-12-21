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

const char *SegStationName[17] = {"BIS", "BIL", "BMS", "BML", "BOS", "BOL", "BEE", 
                              "EIS", "EIL", "EMS", "EML", "EOS", "EOL", "EES", 
                              "EEL", "CSS", "CSL"};//For filling in monitoring plots

double breitgausfun(double *x, double *par){
	 //Fit parameters:
	 //par[0]=Width (scale) Breit-Wigner
	 //par[1]=Most Probable (MP, location) Breit mean
	 //par[2]=Total area (integral -inf to inf, normalization constant)
	 //par[3]=Width (sigma) of convoluted Gaussian function

	 // Numeric constants
	 double invsq2pi = 0.3989422804014; // (2 pi)^(-1/2)
	 // Control constants
	 double np = 100.0; // number of convolution steps
	 double sc = 4; // convolution extends to +-sc Gaussian sigmas
	 
	 // Variables
	 double xx;
	 double fland;
	 double sum = 0.0;
	 double xlow, xupp;
	 double step;
	 double i;
	 
	 // Range of convolution integral
	 xlow = x[0] - sc * par[3];
	 xupp = x[0] + sc * par[3];
	 step = (xupp-xlow) / np;
	 
	 // Convolution integral of Breit and Gaussian by sum
	 for(i=1.0; i<=np/2; i++) {
		 xx = xlow + (i-.5) * step;
		 fland = TMath::BreitWigner(xx,par[1],par[0]);
		 sum += fland * TMath::Gaus(x[0],xx,par[3]);
		 
		 xx = xupp - (i-.5) * step;
		 fland = TMath::BreitWigner(xx,par[1],par[0]);
		 sum += fland * TMath::Gaus(x[0],xx,par[3]);
	 }
	 return (par[2] * step * sum * invsq2pi / par[3]);
}

void TwoDto2D_Eff(TH2 * Numerator, TH2 * Denominator, TH2 * Efficiency, bool rebin2d = false){
  //the input histograms must have the same dimension!
  if (Numerator == NULL || Denominator == NULL || Efficiency == NULL) {return;}
  if(rebin2d){
    Numerator->Rebin2D();//here change the default binnning of eta-phi
    Efficiency->Rebin2D();//here change the default binnning of eta-phi
  }
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

void TwoDto1D_Mean(TH2 * h_parent, TH1 * h_child, int rebinning = 1){
  //the input histograms must have the same dimension!
  if (h_parent == NULL || h_child == NULL ) {return;}
  if (h_parent->GetNbinsX() != h_child->GetNbinsX()){return;}
  //after protection
  for(int bin_itrX = 1; bin_itrX < h_parent->GetNbinsX() + 1; bin_itrX++){
    double parent_event = 0;
    double parent_sum = 0;
    for(int bin_itrY = 1; bin_itrY < h_parent->GetNbinsY() + 1; bin_itrY++){
      parent_event += h_parent->GetBinContent(bin_itrX, bin_itrY);
      parent_sum += h_parent->GetBinContent(bin_itrX, bin_itrY) * h_parent->GetYaxis()->GetBinCenter(bin_itrY);
    }
    if (parent_event == 0){continue;}
    h_child->SetBinContent(bin_itrX, parent_sum/parent_event);
  } 
  TString sHistTitle = h_child->GetTitle();
  h_child->Rebin(rebinning);
  h_child->SetTitle(sHistTitle + " per event");
  h_child->Write("",TObject::kOverwrite);
  return;
}

void TwoDto1D_Sum(TH2 * h_parent, TH1 * h_child, int rebinning = 2){
  //the input histograms must have the same dimension!
  if (h_parent == NULL || h_child == NULL ) {return;}
  if (h_parent->GetNbinsX() != h_child->GetNbinsX()){return;}
  //after protection
  for(int bin_itrX = 1; bin_itrX < h_parent->GetNbinsX() + 1; bin_itrX++){
    double parent_sum = 0;
    for(int bin_itrY = 1; bin_itrY < h_parent->GetNbinsY() + 1; bin_itrY++){
      parent_sum += h_parent->GetBinContent(bin_itrX, bin_itrY) * h_parent->GetYaxis()->GetBinCenter(bin_itrY);
    }
    if(parent_sum == 0){continue;}
    h_child->SetBinContent(bin_itrX, parent_sum);
  }
  h_child->Rebin(rebinning); 
  h_child->Write("",TObject::kOverwrite);
  return;
}

void SetMassInfo(int iBin, TH1* InputHist, TH1* OutMean, TH1* OutSigma, TString recalg_path){
	if (InputHist == NULL || OutMean == NULL || OutSigma == NULL) {return;}
	if (InputHist->GetMaximum() > 10.0){
		if (recalg_path == "Z"){//only for Z
			TF1 *fit1 = new TF1("fit1",breitgausfun, 76, 106, 4);
	    fit1->SetLineColor(kRed);
			fit1->SetParameter(0, 3.0);//par[0]=Width (scale) Breit-Wigner
			fit1->SetParameter(1, 91.2);//par[1]=Most Probable (MP, location) Breit mean
			fit1->SetParameter(2, InputHist->GetEntries());//par[2]=Total area (integral -inf to inf, normalization constant)
			fit1->SetParameter(3, 1.0);//par[3]=Width (sigma) of convoluted Gaussian function
			InputHist->Fit("fit1", "q", "", 77, 105);
	   		fit1->Draw();
	    	OutMean-> SetBinContent(iBin, fit1->GetParameter(1));
	    	OutMean-> SetBinError(  iBin, fit1->GetParError(1));
	    	OutSigma->SetBinContent(iBin, fit1->GetParameter(0));
	    	OutSigma->SetBinError(  iBin, fit1->GetParError(0));
		}
		if (recalg_path == "Jpsi"){
			InputHist->Fit("gaus", "q", "", 2.95, 3.25);
			TF1 *fit1 = (TF1*)InputHist->GetFunction("gaus");
	   		fit1->Draw();
	    	OutMean-> SetBinContent(iBin, fit1->GetParameter(1));
	    	OutMean-> SetBinError(  iBin, fit1->GetParError(1));
	    	OutSigma->SetBinContent(iBin, fit1->GetParameter(2));
	    	OutSigma->SetBinError(  iBin, fit1->GetParError(2));
		}
    }
    else{
    	OutMean-> SetBinContent(iBin, InputHist->GetMean(1));
    	OutMean-> SetBinError(  iBin, InputHist->GetMeanError(1));
    	OutSigma->SetBinContent(iBin, InputHist->GetRMS(1));
    	OutSigma->SetBinError(  iBin, InputHist->GetRMSError(1));
    }
    return;
}

namespace dqutils {
  
  //main function
  void MonitoringFile::MuonTrackPostProcess( const std::string & inFilename, bool isIncremental){
    if (isIncremental) {return;}
    MonitoringFile::MuonTrack_Main( inFilename , "");
    MonitoringFile::MuonTrack_Main( inFilename,  "NoTrig/");
    return;
  }
  
  //subfunctions
  void MonitoringFile::MuonTrack_Main(const std::string & inFilename, TString dirname){
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
      TH2F* h_EffNumerator = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_chamberIndex_perSector_numerator", plotdirname.Data(), recalg_path.Data()));
      TH2F* h_EffDenominator = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_chamberIndex_perSector_denominator", plotdirname.Data(), recalg_path.Data()));
      TH2F* h_Efficiency = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_chamberIndex_perSector", plotdirname.Data(), recalg_path.Data()));
      
      TwoDto2D_Eff(h_EffNumerator, h_EffDenominator, h_Efficiency);

      //add the efficiency for precision
      for(int i=0; i < 17; i++){
        TH2F* seg_prec_EffNumerator = (TH2F*)dir1->Get(Form("%sSegments_%s_%s_etastation_nPrechit", plotdirname.Data(), recalg_path.Data(), SegStationName[i]));
        TH2F* seg_prec_EffDenominator = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_%s_etastation_nPrechit", plotdirname.Data(), recalg_path.Data(), SegStationName[i]));
        TH2F* seg_prec_Efficiency = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_%s_etastation_nPrechit", plotdirname.Data(), recalg_path.Data(), SegStationName[i]));
        
        TwoDto2D_Eff(seg_prec_EffNumerator, seg_prec_EffDenominator, seg_prec_Efficiency);

        TH2F* seg_trig_EffNumerator = (TH2F*)dir1->Get(Form("%sSegments_%s_%s_etastation_nTrighit", plotdirname.Data(), recalg_path.Data(), SegStationName[i]));
        TH2F* seg_trig_EffDenominator = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_%s_etastation_nTrighit", plotdirname.Data(), recalg_path.Data(), SegStationName[i]));
        TH2F* seg_trig_Efficiency = (TH2F*)dir1->Get(Form("%sSegments_%s_eff_%s_etastation_nTrighit", plotdirname.Data(), recalg_path.Data(), SegStationName[i]));
        
        TwoDto2D_Eff(seg_trig_EffNumerator, seg_trig_EffDenominator, seg_trig_Efficiency);
      }
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
      TH2F* h_EffDenominator = (TH2F*)dir1->Get(Form("%sMuons_%s_Origin_eta_phi", plotdirname.Data(), recalg_path.Data()));
      //m_EffDenominator->Rebin2D();//here change the default binnning of eta-phi! disabled once we are in 64 bins
      for (int i = 0; i < 4; i++){
        TH2F* h_EffNumerator = (TH2F*)dir1->Get(Form("%sMuons_%s_%s_eta_phi", plotdirname.Data(), recalg_path.Data(), muonqualstr[i].Data()));
        TH2F* h_Efficiency = (TH2F*)dir1->Get(Form("%sMuons_%s_%s_eff", plotdirname.Data(), recalg_path.Data(), muonqualstr[i].Data()));
        TwoDto2D_Eff(h_EffNumerator, h_EffDenominator, h_Efficiency);//here change the default binnning of eta-phi
      }

      TH2F* eff_nPrec    = (TH2F*)dir1->Get(Form("%sMuons_%s_eff_nPrec",    plotdirname.Data(), recalg_path.Data()));
      TH2F* eff_nPhi     = (TH2F*)dir1->Get(Form("%sMuons_%s_eff_nPhi",     plotdirname.Data(), recalg_path.Data()));
      TH2F* eff_nTrigEta = (TH2F*)dir1->Get(Form("%sMuons_%s_eff_nTrigEta", plotdirname.Data(), recalg_path.Data()));
      TH2F* eff_ndof     = (TH2F*)dir1->Get(Form("%sMuons_%s_eff_ndof",     plotdirname.Data(), recalg_path.Data()));
      TH2F* eff_chi2     = (TH2F*)dir1->Get(Form("%sMuons_%s_eff_chi2",     plotdirname.Data(), recalg_path.Data()));
      TH2F* ID_eff_ndof  = (TH2F*)dir1->Get(Form("%sMuons_%s_ID_eff_ndof",  plotdirname.Data(), recalg_path.Data()));
      TH2F* ID_eff_chi2  = (TH2F*)dir1->Get(Form("%sMuons_%s_ID_eff_chi2",  plotdirname.Data(), recalg_path.Data()));
      TH2F* MS_eff_ndof  = (TH2F*)dir1->Get(Form("%sMuons_%s_MS_eff_ndof",  plotdirname.Data(), recalg_path.Data()));
      TH2F* MS_eff_chi2  = (TH2F*)dir1->Get(Form("%sMuons_%s_MS_eff_chi2",  plotdirname.Data(), recalg_path.Data()));

      TH2F* avg_hits_precision_inner    = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_precision_inner",    plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_precision_middle   = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_precision_middle",   plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_precision_outer    = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_precision_outer",    plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_precision_extended = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_precision_extended", plotdirname.Data(), recalg_path.Data()));

      TH2F* avg_hits_trigger_layer1 = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_trigger_layer1", plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_trigger_layer2 = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_trigger_layer2", plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_trigger_layer3 = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_trigger_layer3", plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_trigger_layer4 = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_trigger_layer4", plotdirname.Data(), recalg_path.Data()));

      TH2F* avg_hits_ibl = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_ibl", plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_pix = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_pix", plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_sct = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_sct", plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_hits_trt = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_hits_trt", plotdirname.Data(), recalg_path.Data()));

      TH2F* avg_ddpt_idme = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_ddpt_idme", plotdirname.Data(), recalg_path.Data()));
      TH2F* avg_dptsignif = (TH2F*)dir1->Get(Form("%sMuons_%s_avg_dptsignif", plotdirname.Data(), recalg_path.Data()));

      TwoDto2D_Eff(eff_nPrec,    h_EffDenominator, eff_nPrec);
      TwoDto2D_Eff(eff_nPhi,     h_EffDenominator, eff_nPhi);
      TwoDto2D_Eff(eff_nTrigEta, h_EffDenominator, eff_nTrigEta);
      TwoDto2D_Eff(eff_ndof,     h_EffDenominator, eff_ndof);
      TwoDto2D_Eff(eff_chi2,     h_EffDenominator, eff_chi2);
      TwoDto2D_Eff(ID_eff_ndof,  h_EffDenominator, ID_eff_ndof);
      TwoDto2D_Eff(ID_eff_chi2,  h_EffDenominator, ID_eff_chi2);
      TwoDto2D_Eff(MS_eff_ndof,  h_EffDenominator, MS_eff_ndof);
      TwoDto2D_Eff(MS_eff_chi2,  h_EffDenominator, MS_eff_chi2);

      TwoDto2D_Eff(avg_hits_precision_inner,    h_EffDenominator, avg_hits_precision_inner);
      TwoDto2D_Eff(avg_hits_precision_middle,   h_EffDenominator, avg_hits_precision_middle);
      TwoDto2D_Eff(avg_hits_precision_outer,    h_EffDenominator, avg_hits_precision_outer);
      TwoDto2D_Eff(avg_hits_precision_extended, h_EffDenominator, avg_hits_precision_extended);

      TwoDto2D_Eff(avg_hits_trigger_layer1, h_EffDenominator, avg_hits_trigger_layer1);
      TwoDto2D_Eff(avg_hits_trigger_layer2, h_EffDenominator, avg_hits_trigger_layer2);
      TwoDto2D_Eff(avg_hits_trigger_layer3, h_EffDenominator, avg_hits_trigger_layer3);
      TwoDto2D_Eff(avg_hits_trigger_layer4, h_EffDenominator, avg_hits_trigger_layer4);

      TwoDto2D_Eff(avg_hits_ibl, h_EffDenominator, avg_hits_ibl);
      TwoDto2D_Eff(avg_hits_pix, h_EffDenominator, avg_hits_pix);
      TwoDto2D_Eff(avg_hits_sct, h_EffDenominator, avg_hits_sct);
      TwoDto2D_Eff(avg_hits_trt, h_EffDenominator, avg_hits_trt);

      TwoDto2D_Eff(avg_ddpt_idme, h_EffDenominator, avg_ddpt_idme);
      TwoDto2D_Eff(avg_dptsignif, h_EffDenominator, avg_dptsignif);

 
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
        TH2F* h_parent_lb = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_LB_2D", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        TH1F* h_child_lb = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_LB", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        //TH2F* h_parent_inst = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_Inst_2D", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        //TH1F* h_child_inst = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_Inst", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        //TH2F* h_parent_intlumi = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_IntLumi_2D", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        //TH1F* h_child_intlumi = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_IntLumi", plotdirname.Data(), recalg_path.Data(), montype[i].Data()));
        TwoDto1D_Mean(h_parent_lb, h_child_lb);
        //TwoDto1D_Mean(h_parent_inst, h_child_inst);
        //TwoDto1D_Mean(h_parent_intlumi, h_child_intlumi);
      }
      TString resonance[2] = {"Z", "Jpsi"};
      // Divide the efficiency histograms
      for (int i = 0; i < 2; i++){
        TH2F* h_parent_lb = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_LB_2D", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        TH1F* h_child_lb = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_LB", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        //TH2F* h_parent_inst = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_Inst_2D", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        //TH1F* h_child_inst = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_Inst", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        //TH2F* h_parent_intlumi = (TH2F*)dir1->Get(Form("%sOverview_%s_n%s_IntLumi_2D", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        //TH1F* h_child_intlumi = (TH1F*)dir1->Get(Form("%sOverview_%s_n%s_IntLumi", plotdirname.Data(), recalg_path.Data(), resonance[i].Data()));
        TwoDto1D_Sum(h_parent_lb, h_child_lb);
        //TwoDto1D_Sum(h_parent_inst, h_child_inst);
        //TwoDto1D_Sum(h_parent_intlumi, h_child_intlumi);
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
	      
	      TH1* h_Mass_Mean = (TH1F*)dir1->Get(Form("m_%s_M_Mean", recalg_path.Data()));
	      TH1* h_Mass_Sigma = (TH1F*)dir1->Get(Form("m_%s_M_Sigma", recalg_path.Data()));
	      // Get each of the mass histograms
	      TString det_region[4] = {"EC", "BC", "BA", "EA"};
	      for (int i = 0; i < 4; i++){
	        for(int j = 0; j< 4; j++){
	        	TH1* h_Mass_region = (TH1F*)dir1->Get(Form("m_%s_M_%s_%s", recalg_path.Data(), 
	        		det_region[i].Data(), det_region[j].Data()));
	        	//std::cout << " bin " << i * 4 + (j + 1) << " content " << det_region[i] << " " << det_region[j] << std::endl;
	        	SetMassInfo(i * 4 + (j + 1), h_Mass_region, h_Mass_Mean, h_Mass_Sigma, recalg_path);
	        	if (h_Mass_region != NULL) h_Mass_region->Write("",TObject::kOverwrite);
	        }
	      }
	      if (h_Mass_Mean != NULL) h_Mass_Mean->Write("",TObject::kOverwrite);
	      if (h_Mass_Sigma != NULL) h_Mass_Sigma->Write("",TObject::kOverwrite);
	    }
    }//ends different subfolder for muon efficiency

    f->Close();
    delete f;
    return;
  } //ends function
  
  
  
} //namespace
