/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONWCANDIDATEPLOTS_H
#define MUONHISTUTILS_MUONWCANDIDATEPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODMuon/Muon.h"
#include "xAODMissingET/MissingET.h"
#include <string>

class MuonWCandidatePlots: public PlotBase
{
 public:
  MuonWCandidatePlots(PlotBase *pParent, std::string sDir);
  void fill(const xAOD::Muon& mu, const xAOD::MissingET& met);
  
  TH1* me_t;
  TH1* me_x;
  TH1* me_y;

  TH1* mu_pt;
  TH1* mu_eta;
  TH1* mu_phi;

  TH1* w_pt;
  TH1* w_phi;
  TH1* w_mt;

 private:
  void initializePlots();
};

#endif
