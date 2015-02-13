/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_EFTRIGGERMUONPLOTS_H
#define MUONHISTUTILS_EFTRIGGERMUONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
//#include "TrkValHistUtils/MSHitPlots.h"
//#include "TrkValHistUtils/TruthInfoPlots.h"
//#include "TrkValHistUtils/TruthTrkExtrapolationPlots.h"

#include "xAODMuon/Muon.h"
//#include "xAODMuon/MuonContainer.h"
//#include "xAODTruth/TruthParticle.h"
//#include "xAODTrigger/MuonRoI.h"
//#include "xAODTrigger/MuonRoIContainer.h"
//#include "xAODMuon/MuonAuxContainer.h"

class EFTriggerMuonPlots:public PlotBase {
    public:
      EFTriggerMuonPlots(PlotBase* pParent, std::string sDir);
      void fill(const xAOD::Muon& mu,const xAOD::Muon& Trigmu); 
      
      //
      Trk::ParamPlots                   m_oHLTriggerPlots;
};

#endif
