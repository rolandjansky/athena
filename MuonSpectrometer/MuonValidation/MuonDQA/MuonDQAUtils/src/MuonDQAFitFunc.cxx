/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : MuonDQAFitFunc
// Authors:   N. Benekos(Illinois) - A. Cortes (Illinois) 
// Aug. 2008
///////////////////////////////////////////////////////////////////////////////////////////////////////////// 

#include "MuonDQAUtils/MuonDQAFitFunc.h"

#include "TF1.h"
#include "TH1.h"

namespace {
  constexpr double Zmass = 91.1876;
}

namespace Muon {
 
  MuonDQAFitFunc::MuonDQAFitFunc(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_minMuonEffWindow(0.8),
      m_maxMuonEffWindow(1.05)
  {
    declareProperty("ZmumuPDGmass",     m_ZmumuPDGmass=Zmass);
    declareProperty("minMuonEffWindow", m_minMuonEffWindow);
    declareProperty("maxMuonEffWindow", m_maxMuonEffWindow);
    declareInterface<MuonDQAFitFunc>(this);
  }
  
  //================================================================
  void MuonDQAFitFunc::ZmumuFitHistograms(TH1F* hmass, TH1F* hwidth, TH1F* h1[], int nbins)                                                      
  { 
    double mass=0.;
    double mass_error=0.;
    double width=0.;
    double width_error=0.;
    double maxmass=0;
    double maxwidth=0;
    TF1 *ZmumFit = new TF1("ZmumFit","gaus",89.,93.);
    for (int i=0; i<nbins; i++) {
      if (h1[i]->GetEntries()>50) {
	ZmumFit->SetParameter(1,3.097);
	ZmumFit->SetParameter(2,0.05);   
	h1[i]->Fit("ZmumFit","RQNM");
	mass=ZmumFit->GetParameter(1)-m_ZmumuPDGmass;
	mass_error=ZmumFit->GetParError(1);
	width=ZmumFit->GetParameter(2);
	width_error=ZmumFit->GetParError(2);
	hmass->SetBinContent(i+1,mass);
	hmass->SetBinError(i+1,mass_error); 
	hwidth->SetBinContent(i+1,width);
	hwidth->SetBinError(i+1,width_error); 
	if( (std::abs(mass)+mass_error)>maxmass ) maxmass=std::abs(mass)+mass_error;
	if( (std::abs(width)+width_error)>maxwidth ) maxwidth=std::abs(width)+width_error;
      }
    }
    if (maxmass>0.1) maxmass=0.1;
    if (maxwidth>0.2) maxwidth=0.2;
    hmass->SetAxisRange(-maxmass,maxmass,"Y");
    hwidth->SetAxisRange(0.,maxwidth,"Y");
  
    delete ZmumFit;
  }
  //================================================================
  void MuonDQAFitFunc::MuonEffHisto1D(TH1F* h_Num, TH1F* h_Denom, TProfile* h_Eff) const
  { 
    int nBins;
    nBins = h_Num->GetNbinsX();
    for (int bin=0; bin!=nBins; ++bin) {
      int nPass = int(h_Num->GetBinContent(bin+1));
      int nFail = int(h_Denom->GetBinContent(bin+1)) - nPass;
      double x = h_Denom->GetBinCenter(bin+1);
      for (int pass=0; pass!=nPass; ++pass) {
	h_Eff->Fill(x,1.);
      }
      for (int fail=0; fail!=nFail; ++fail) {
	h_Eff->Fill(x,0.);
      }
    }  
    MinWindow1Set_from_TProf(h_Eff, m_minMuonEffWindow, m_maxMuonEffWindow); 

  }

  //================================================================
  void MuonDQAFitFunc::MuonEffHisto2D(TH2F* h_Num, TH2F* h_Denom, TProfile2D* h_Eff) const 
  { 
    int nBins;
    int nBinsY;
    nBins = h_Num->GetNbinsX();
    nBinsY = h_Num->GetNbinsY();
    for (int bin=0; bin!=nBins; ++bin) {
      for (int binY=0; binY!=nBinsY; ++binY) {
	int nPass = int(h_Num->GetBinContent(bin+1,binY+1));
	int nFail = int(h_Denom->GetBinContent(bin+1,binY+1)) - nPass;
	double x = h_Denom->GetXaxis()->GetBinCenter(bin+1);
	double y = h_Denom->GetYaxis()->GetBinCenter(binY+1);
	for (int pass=0; pass!=nPass; ++pass) {
	  h_Eff->Fill(x,y,1.);
	}
	for (int fail=0; fail!=nFail; ++fail) {
	  h_Eff->Fill(x,y,0.);
	}
      }
    }
    MinWindow2Set_from_TProf(h_Eff, m_minMuonEffWindow, m_maxMuonEffWindow); 
  
  }

  //================================================================
  // Establishes a minimim window for the TProfile 
  //================================================================
  void MuonDQAFitFunc::MinWindow1Set_from_TProf(TProfile* hProf, float windowMin, float windowMax) const 
  {
    float min=hProf->GetMinimum();
    float max=hProf->GetMaximum();
    float margin=0.05*(max-min);
    if(min > windowMin) min=windowMin-margin;
    if(max < windowMax) max=windowMax+margin;
    hProf->SetMinimum(min);
    hProf->SetMaximum(max);
 
  }

  //================================================================ 
  void MuonDQAFitFunc::MinWindow2Set_from_TProf(TProfile2D* hProf, float windowMin, float windowMax) const 
  {
    float min=hProf->GetMinimum();
    float max=hProf->GetMaximum();
    float margin=0.05*(max-min);
    if(min > windowMin) min=windowMin-margin;
    if(max < windowMax) max=windowMax+margin;
    hProf->SetMinimum(min);
    hProf->SetMaximum(max);
  }

  //================================================================
  void MuonDQAFitFunc::FillMeanRmsProj(TH2F* h2d, TH1F* h1d,int MeanRms) const 
  {

    // fills a 1-D histogram with either the mean or RMS of the residual distribution for each layer in the barrel. 
    // Does this by projecting a 2-D histo of residual vs layer. 
 
    int nBins_2d = h2d->GetNbinsX();
    int nBins_1d = h1d->GetNbinsX();

    if(nBins_2d!=nBins_1d) ATH_MSG_DEBUG("Mean/RMS Histograms not set up correctly - nBins mismatch");
 
    //calling this means that the histogram bin content is flagged 
    //as being an average and so adding histos from different jobs 
    //will produce weighted mean
   
    h1d->SetBit(TH1::kIsAverage);
 
    for(int j = 1; j!=nBins_2d+1; j++){

      TH1F* hproj = (TH1F*)h2d->ProjectionY("Proj",j,j,"e");
 
      //do not fill if there are no entries in the bin
      if(hproj->GetEntries()<=0) {
	delete hproj;
	continue;
      }
 
      if(MeanRms==0){
	h1d->SetBinContent(j,hproj->GetMean());
	h1d->SetBinError(j,hproj->GetMeanError());
       
      }
      else if(MeanRms==1){
	h1d->SetBinContent(j,hproj->GetRMS());
	h1d->SetBinError(j,hproj->GetRMSError());
      }
      else ATH_MSG_DEBUG("Incorrect switch in MeanRMSProjectionsBarrel()");
 
      delete hproj;
    }
 
    return;
  } 
  //================================================================

  void MuonDQAFitFunc::FillRMSFromProfile(TProfile* hProf, TH1F* hRms) const
  {
    //Uses the spread information in an already filled TProfile   
    //to fill a second TH1F with the (correctly weigthed by N events) RMS of each bin
 
    int nBins = hProf->GetNbinsX();
 
    for(int j = 1; j!=nBins+1; j++){
 
      float bincentre = (float)hProf->GetBinCenter(j);
  
      //By default this is RMS/sqrt(N)
      float binerr = (float)hProf->GetBinError(j);

      //getting binentries
      float binentries = (float)hProf->GetBinEntries(j);
 
      //calculating the RMS from the above quantities
      float rms = (float)binerr*(std::sqrt(binentries));
      
      hRms->Fill(bincentre,rms);	
    } // Loop over bins
    
    return; 
  }

  //================================================================

  void MuonDQAFitFunc::FillRMSFromCharProfile(TProfile* hProf_char, TH1F* hRms_char) const
  {
 
    //Uses the spread information in an already filled TProfile (with character bins) 
    //to fill a second TH1F with the (correctly weigthed by N events) RMS of each bin
 
    int nBins = hProf_char->GetNbinsX();
 
    for(int j = 1; j!=nBins+1; j++){
 
      //float bincentre = (float)hProf->GetBinCenter(j);
      const char*  BinLabel_char = hProf_char->GetXaxis()->GetBinLabel(j);
  
      //By default this is RMS/sqrt(N)
      float binerr = (float)hProf_char->GetBinError(j);
 
      //getting binentries
      float binentries = (float)hProf_char->GetBinEntries(j);
 
      //calculating the RMS from the above quantities
      float rms = (float)binerr*(std::sqrt(binentries));
     
      hRms_char->SetCanExtend(TH1::kAllAxes);
      hRms_char->Fill(BinLabel_char,rms);

    } // Loop over bins
    
    hRms_char->LabelsDeflate("X");
    if (hRms_char->GetEntries() > 0) hRms_char->LabelsOption("v");
    return;
  }

  //================================================================
  void MuonDQAFitFunc::FillGausMeanOrWidth(TH2F* h2d, TH1F* h1d, float MinFit, float MaxFit, int iopt) const
  {


    //Makes a Gaussian fit to each bin of a TH2F and fills a TH1F with 
    //the either the mean or width of this Gaussian
 
    //calling this means that the histogram bin content is flagged 
    //as being an average and so adding histos from different jobs 
    //will produce weighted mean
    h1d->SetBit(TH1::kIsAverage);
 
    int nBins_2d = h2d->GetNbinsX();
    int nBins_1d = h1d->GetNbinsX();
 
    if(nBins_2d!=nBins_1d) ATH_MSG_DEBUG("Mean/Width Histograms not set up correctly - nBins mismatch");
 
    for(int i = 1; i!=nBins_2d+1; i++){

      TH1F* hProj = (TH1F*)h2d->ProjectionY("Proj",i,i,"e");
 
      //do not fill if there are no entries in the bin
      if(hProj->GetEntries()<=0) {
	delete hProj;
	continue;
      }
     
      TF1 *fit = new TF1("fit","gaus",MinFit,MaxFit);
 
      hProj->Fit("fit","RQNM");
      float Mean = fit->GetParameter(1);
      float MeanSigma = fit->GetParError(1);
      float Width = fit->GetParameter(2);
      float WidthSigma = fit->GetParError(2);
 
      if(iopt==0){
	h1d->SetBinContent(i,Mean);
	h1d->SetBinError(i,MeanSigma);
      }
      else if(iopt==1){
	h1d->SetBinContent(i,Width);
	h1d->SetBinError(i,WidthSigma);
      }
      else ATH_MSG_DEBUG("Incorrect switch in FillGausMeanOrWidth");
 
      delete hProj;
      delete fit;
    }
 
    return;

  }


  //================================================================
  void MuonDQAFitFunc::FillGaussMeanAndWidth(TH2F* h2d, TH1F* h1d, float MinFit, float MaxFit) const
  {
        
        
    //Makes a Gaussian fit to each bin of a TH2F and fills a TH1F,
    // the mean of this Gaussian, and the errors the width.
            
    //calling this means that the histogram bin content is flagged
    //as being an average and so adding histos from different jobs
    //will produce weighted mean
    h1d->SetBit(TH1::kIsAverage);
    
    int nBins_2d = h2d->GetNbinsX();
    int nBins_1d = h1d->GetNbinsX();
      
    if(nBins_2d!=nBins_1d) ATH_MSG_DEBUG("Mean/Width Histograms not set up correctly - nBins mismatch");
      
    for(int i = 1; i!=nBins_2d+1; i++){
      
      TH1F* hProj = (TH1F*)h2d->ProjectionY("Proj",i,i,"e");
      
      int numNonEmptyBins = 0;
      for (int ii = 0; ii < (int)hProj->GetNbinsX()+1; ++ii)
      {
         if (hProj->GetBinContent(ii)>0.) ++numNonEmptyBins;
      }
    
      //do not fill if there are no entries in the bin
      //if(hProj->GetEntries()<=0) {
      if((hProj->GetEntries()<=0)||(numNonEmptyBins<2)) {
        delete hProj;
        continue;
      }

      TF1 *fit = new TF1("fit","gaus",MinFit,MaxFit);
    
      hProj->Fit("fit","RQNM");
      float Mean = fit->GetParameter(1);
      // float MeanSigma = fit->GetParError(1);
      float Width = fit->GetParameter(2);
      // float WidthSigma = fit->GetParError(2);
    
      h1d->SetBinContent(i,Mean);
      h1d->SetBinError(i, Width);
      
      delete hProj;
      delete fit;
    } 
      
    h1d->SetEntries(h2d->GetEntries());
    return;
    
  }
  
  

  
  //================================================================
  void MuonDQAFitFunc::FillGaussMeanAndWidth(TH1F* h1, TH1F* h2, int nbin, float MinFit, float MaxFit) const
  {
    // Makes a Gaussian fit to a TH1F and fills the given bin of another TH1F
    // with the mean of this Gaussian, and the errors with the width.
            
    //calling this means that the histogram bin content is flagged
    //as being an average and so adding histos from different jobs
    //will produce weighted mean
    h2->SetBit(TH1::kIsAverage);
          
    //do not fill if there are no entries in h1
    if(h1->GetEntries()<=0) return;
    
    TF1 *fit = new TF1("fit","gaus",MinFit,MaxFit);
    h1->Fit("fit","RQNM");
    float Mean = fit->GetParameter(1);
    // float MeanSigma = fit->GetParError(1);
    float Width = fit->GetParameter(2);
    // float WidthSigma = fit->GetParError(2);
    
    h2->SetBinContent(nbin,Mean);
    h2->SetBinError(nbin,Width);
    delete fit;      
    h2->SetEntries(h1->GetEntries());
    return;
  }

} // namespace
