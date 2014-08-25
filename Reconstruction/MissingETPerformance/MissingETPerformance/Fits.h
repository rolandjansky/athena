/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Fits_h
#define Fits_h
#include "TH1D.h"
#include "TF1.h"
#include <vector>
#include <string>
#include "GaudiKernel/ITHistSvc.h"

class Fits  {
 public:
  Fits()
    : m_NBins(0),
      m_lower(0),
      m_upper(0),
      m_NBins_res(0),
      m_lower_res(0),
      m_upper_res(0),
      m_FitHists(0),
      m_FitHists_tot(0),
      m_Mean(0),
      m_Sigma(0),
      m_Chisq(0),
      m_RawVariable(0)
  {}
  ~Fits() {}
  inline TH1D* GetMeanHist(){return m_Mean;}
  inline double GetMean(int i){return m_Mean->GetBinContent(i);}
  inline TH1D* GetSigmaHist(){return m_Sigma;}
  inline double GetSigma(int i){return m_Sigma->GetBinContent(i);}
  inline TH1D* GetChisqHist(){return m_Chisq;}
  inline std::vector<TH1D*> *GetFitHists(){return m_FitHists;}
  inline void FitSigmaHist(){m_Sigma->Fit("function", "R");}
    
  void Initialise(ITHistSvc *thistsvc,std::string HistName_in, 
		  int NBins_in, double lower_in, double upper_in,
		  int NBins_res_in, double lower_res_in, double upper_res_in);
  void FillHists(double EtMiss_res, double SumEt);
  void Fit();
  void Enter();
  //void WriteHists(TString filename);

 private:
  int m_NBins;
  double m_lower;
  double m_upper;
  int m_NBins_res;
  double m_lower_res;
  double m_upper_res;
  //Vector1D<TH1D*> *FitHists;
  std::vector<TH1D*> *m_FitHists;
  TH1D* m_FitHists_tot;
  TH1D *m_Mean;
  TH1D *m_Sigma;
  TH1D *m_Chisq;
  TH1D *m_RawVariable;
  TString m_Name;
};
#endif
