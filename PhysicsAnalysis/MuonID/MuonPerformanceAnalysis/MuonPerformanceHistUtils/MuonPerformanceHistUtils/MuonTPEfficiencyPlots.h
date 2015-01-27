/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonTPEfficiencyPlots_h
#define MuonTPEfficiencyPlots_h

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "MuonHistUtils/RecoInfoPlots.h"
#include "MuonHistUtils/MuonIsolationPlots.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlotBase.h"
#include "MuonPerformanceHistUtils/MuonTPCutFlowBase.h"

#include "MuonPerformanceHistUtils/Probe.h"

class MuonTPEfficiencyPlots: public MuonTPEfficiencyPlotBase {
 public:
  MuonTPEfficiencyPlots(PlotBase* pParent, std::string sDir, bool isMatched);

  void fill(Probe& probe);

 private:

  bool m_isMatched;

  Trk::ParamPlots                 m_probeTrkKinePlots;
  Trk::ImpactPlots                m_probeTrkImpactPlots;
  Trk::RecoInfoPlots              m_probeTrkRecoInfoPlots;
  Muon::RecoInfoPlots             m_probeMuonRecoInfoPlots;
  Muon::MuonIsolationPlots        m_probeMuonIsolationPlots;
};
 
#endif
