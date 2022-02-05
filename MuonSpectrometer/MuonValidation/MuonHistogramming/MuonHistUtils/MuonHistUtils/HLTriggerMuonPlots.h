/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_HLTRIGGERMUONPLOTS_H
#define MUONHISTUTILS_HLTRIGGERMUONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "xAODMuon/Muon.h"
#include "xAODTrigMuon/L2StandAloneMuon.h"
#include "xAODTrigMuon/L2CombinedMuon.h"
#include "xAODTrigMuon/L2IsoMuon.h"



class HLTriggerMuonPlots:public PlotBase {
    public:
      HLTriggerMuonPlots(PlotBase* pParent, const std::string& sDir);
      void fill(const xAOD::Muon& Trigmu);
      void fill(const xAOD::Muon& mu,const xAOD::Muon& Trigmu);
      void fill(const xAOD::L2StandAloneMuon& L2SAmu);
      void fill(const xAOD::L2CombinedMuon& L2CBmu);
      void fill(const xAOD::L2IsoMuon& L2Isomu);
      //
      Trk::ParamPlots                   m_oHLTriggerPlots;
};

#endif
