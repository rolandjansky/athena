/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONFITQUALITYPLOTS_H
#define MUONHISTUTILS_MUONFITQUALITYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"
#include <string>

namespace Muon{

class MuonFitQualityPlots: public PlotBase
{
 public:
  MuonFitQualityPlots(PlotBase *pParent, std::string sDir, std::string sType);
  void fill(const xAOD::Muon& mu,xAOD::Muon::ParamDef eChi2, xAOD::Muon::ParamDef eNdof);
  void fill(float fChi2,int fNdf);  
  std::string m_sType;
  
  TH1* chi2;
  TH1* ndof;
  TH1* chi2ndof;
  TH1* chi2prob;
  
 private:
  void initializePlots();
  
};

}

#endif
