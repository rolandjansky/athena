/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SEMILEPTONICCORR_SEMILCORR_H
#define SEMILEPTONICCORR_SEMILCORR_H

#include "TH2F.h"
#include "TFile.h"
#include <iostream>
#include "TLorentzVector.h"
#include <vector>


class semilCorr{

public:
  enum Systematics{
    TAGGINGWEIGHT = 1,
    FRAGMENTATION = 2,
    DECAY = 3,
    MSRESO = 4,
    IDRESO = 5,
    DECAYREW = 6,
    MUONSPECTRUM = 7,
    ALL = 8
  };
  
private:
  std::vector<std::vector<TH1F*> > m_histos; 
  std::vector<float> m_etas;

  TFile* m_f;

  bool m_Debug;

  float getResponse(float pt, float eta, std::vector<TH1F*> h);  
  float getSemilCorrToIncl(TLorentzVector jet, TLorentzVector mu,
			   std::vector<TH1F*> histos);
  std::vector<int> getHistoIndeces(semilCorr::Systematics syst);


public:
  semilCorr(TString fIn, std::string suffix = "", bool DebugIn = false);
  ~semilCorr(); 

  float getSemilCorrToIncl(TLorentzVector jet, TLorentzVector mu);
  float getBjetCorrToIncl(TLorentzVector jet, TLorentzVector mu);

  float getSemilCorrToInclSyst(TLorentzVector jet, TLorentzVector mu, 
			       semilCorr::Systematics syst = semilCorr::ALL);

};

#endif
