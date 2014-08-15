/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_TRUTHRELATEDMUONTRACKPLOTS_H
#define MUONPHYSVALMONITORING_TRUTHRELATEDMUONTRACKPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitDiffPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticle.h"

class TruthRelatedMuonTrackPlots:public PlotBase {
 public:
  TruthRelatedMuonTrackPlots(PlotBase* pParent, std::string sDir);
  
  // Truth related plots
  Trk::ParamPlots                 m_oMatchedPlots;
  Trk::MSHitDiffPlots             m_oMSHitDiffPlots;
  
  // fill methods
  void fill(const xAOD::TruthParticle& truthMu, const xAOD::TrackParticle& muTP);  
};

#endif
