/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/Fits.h"
#include "TH1.h"

using namespace std;

void Fits::Initialise(ITHistSvc *thistsvc,string Name_in,
		      int NBins_in, double lower_in, double upper_in, 
		      int NBins_res_in, double lower_res_in, double upper_res_in)
{
  m_Name=Name_in;
  m_NBins=NBins_in;
  m_lower=lower_in;
  m_upper=upper_in;
  m_NBins_res=NBins_res_in;
  m_lower_res=lower_res_in;
  m_upper_res=upper_res_in;
  m_FitHists_tot=new TH1D("hist", "hist", m_NBins_res, m_lower_res, m_upper_res);
  m_FitHists=new vector<TH1D*>;

  string dirname = "/AANT/";
  dirname = dirname + m_Name + "/";

  for(int i=0; i<m_NBins; i++) {
    TString dummy("ResolutionInBin_");
    dummy+=(i);
    TH1D *dumb = (TH1D *)(m_FitHists_tot->Clone());
    dumb->SetName(dummy);
    dumb->SetTitle(dummy);
    //FitHists->push_back((TH1D *)(FitHists_tot->Clone()));
    m_FitHists->push_back(dumb);
    thistsvc->regHist((std::string)(dirname+dummy), m_FitHists->at(i));
  }

  m_Mean=new TH1D("Mean", "Mean", m_NBins, m_lower, m_upper); 
  thistsvc->regHist(dirname+"/Mean", m_Mean);

  m_Sigma=new TH1D("Sigma", "Sigma", m_NBins, m_lower, m_upper); 
  thistsvc->regHist(dirname+"/Sigma", m_Sigma);

  m_Chisq=new TH1D("Chisq", "Chisq", m_NBins, m_lower, m_upper); 
  thistsvc->regHist(dirname+"/Chisq", m_Chisq);

  m_RawVariable=new TH1D("RawVariable", "RawVariable", m_NBins, m_lower, m_upper);
  thistsvc->regHist(dirname+"/RawVariable", m_RawVariable);

  m_FitHists_tot->SetName("TotalResolution");
  m_FitHists_tot->SetTitle("TotalResolution");
  thistsvc->regHist(dirname + "/TotalResolution", m_FitHists_tot);
}

void Fits::FillHists(double Resolution, double xval) {

  int group = -1;
  double range=m_upper-m_lower; 
  for (int i=0; i<m_NBins; i++){ 
    double inc=range/m_NBins; 
    if((m_lower+(i*inc) <= xval)&& 
       ((m_lower+((i+1)*inc)) > xval)) 
      group = i; 
  } 

  if(group==-1) return;
  if(group>m_NBins) return;

  m_FitHists->at(group)->Fill(Resolution);
  m_FitHists_tot->Fill(Resolution);
  m_RawVariable->Fill(xval);
}

void Fits::Fit(){
  TF1 *FitFunc = new TF1("FitFunc", "gaus");
  for (int i=0; i<m_NBins; i++){
    m_FitHists->at(i)->Fit("FitFunc", "Q");
    m_Mean->SetBinContent(i+1, FitFunc->GetParameter(1));
    m_Mean->SetBinError(i+1, FitFunc->GetParError(1));
    m_Sigma->SetBinContent(i+1, FitFunc->GetParameter(2));
    m_Sigma->SetBinError(i+1, FitFunc->GetParError(2));
    m_Chisq->SetBinContent(i+1, FitFunc->GetChisquare());
  }
}

void Fits::Enter(){
  for (int i=0; i<m_NBins; i++){
    m_Mean->SetBinContent(i+1, m_FitHists->at(i)->GetMean());
    m_Mean->SetBinError(i+1, m_FitHists->at(i)->GetMeanError());
    m_Sigma->SetBinContent(i+1, m_FitHists->at(i)->GetRMS());
    m_Sigma->SetBinError(i+1, m_FitHists->at(i)->GetRMSError());
  }
}
