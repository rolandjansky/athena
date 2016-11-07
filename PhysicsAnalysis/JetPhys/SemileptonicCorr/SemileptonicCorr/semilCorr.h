/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __SEMIL_CORR__
#define __SEMIL_CORR__

#include "TH2F.h"
#include "TFile.h"
#include <iostream>
#include "TLorentzVector.h"
#include <vector>

using namespace std;

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
  vector<vector<TH1F*> > m_histos; 
  vector<float> etas;

  TFile* f;

  bool Debug;

  float getResponse(float pt, float eta, vector<TH1F*> h);  
  float getSemilCorrToIncl(TLorentzVector jet, TLorentzVector mu,
			   vector<TH1F*> histos);
  vector<int> getHistoIndeces(semilCorr::Systematics syst);


public:
  semilCorr(TString fIn, string suffix = "", bool DebugIn = false);
  ~semilCorr(); 

  float getSemilCorrToIncl(TLorentzVector jet, TLorentzVector mu);
  float getBjetCorrToIncl(TLorentzVector jet, TLorentzVector mu);

  float getSemilCorrToInclSyst(TLorentzVector jet, TLorentzVector mu, 
			       semilCorr::Systematics syst = semilCorr::ALL);

};

#endif
