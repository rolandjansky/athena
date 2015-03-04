/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_RECORELATEDTRIGGERMUONPLOTS_H
#define MUONPHYSVALMONITORING_RECORELATEDTRIGGERMUONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
//#include "TrkValHistUtils/ParamPlots.h"
#include "MuonHistUtils/MuonResolutionPlots.h"
#include "MuonHistUtils/MuonTree.h"

#include "xAODMuon/Muon.h"

class RecoRelatedTriggerMuonPlots:public PlotBase {
 public:
  RecoRelatedTriggerMuonPlots(PlotBase* pParent, std::string sDir,bool doBinnedResolutionPlots, bool doMuonTree=false);
  ~RecoRelatedTriggerMuonPlots();
  void fill(const xAOD::Muon& Trigmu, const xAOD::Muon& Recomu);
  bool m_doMuonTree;
  // Truth related plots

  Muon::MuonResolutionPlots       m_oMuonResolutionPlots;

};

#endif
