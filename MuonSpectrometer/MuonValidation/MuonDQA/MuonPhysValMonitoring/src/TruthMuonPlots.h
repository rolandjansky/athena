/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONPHYSVALMONITORING_TRUTHMUONPLOTS_H
#define MUONPHYSVALMONITORING_TRUTHMUONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/TruthInfoPlots.h"
#include "TrkValHistUtils/TruthTrkExtrapolationPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthParticle.h"

class TruthMuonPlots:public PlotBase {
    public:
      TruthMuonPlots(PlotBase* pParent, std::string sDir);
      void fill(const xAOD::TruthParticle& truthMu);
      
      // Truth related plots
      Trk::ParamPlots                 m_oTruthPlots;
      Trk::TruthInfoPlots             m_oTruthInfoPlots;
      Trk::TruthTrkExtrapolationPlots m_oTruthTrkExtrapolationPlots;
      Trk::MSHitPlots                 m_oTruthMSHitPlots;
};

#endif
