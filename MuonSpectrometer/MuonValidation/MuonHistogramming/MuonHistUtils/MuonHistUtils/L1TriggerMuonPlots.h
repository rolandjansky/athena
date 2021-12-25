/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_L1TRIGGERMUONPLOTS_H
#define MUONHISTUTILS_L1TRIGGERMUONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
//#include "TrkValHistUtils/MSHitPlots.h"
//#include "TrkValHistUtils/TruthInfoPlots.h"
//#include "TrkValHistUtils/TruthTrkExtrapolationPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODTrigger/MuonRoI.h"
//#include "xAODTruth/TruthParticle.h"

class L1TriggerMuonPlots:public PlotBase {
    public:
      L1TriggerMuonPlots(PlotBase* pParent, const std::string& sDir);
      void fill(const xAOD::MuonRoI& TrigL1mu);
      
      //
      Trk::ParamPlots               m_oL1TriggerPlots;
};

#endif
