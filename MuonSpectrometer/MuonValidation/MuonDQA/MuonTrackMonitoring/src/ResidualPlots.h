/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_RESIDUALPLOTS_H
#define MUONTRACKMONITORING_RESIDUALPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonHistUtils/MuonHitResidualPlots.h"
#include "MuonIdHelpers/MuonStationIndex.h"

namespace Muon{
  
class ResidualPlots:public PlotBase {
    public:
      ResidualPlots(PlotBase* pParent, std::string sDir);
      void fill(const Trk::ResidualPull& resPull, int stationPhi, Muon::MuonStationIndex::TechnologyIndex techid, bool measuresPhi);
      
      // Reco only information
      Muon::MuonHitResidualPlots         m_oResidualPlots_Global;	    
      Muon::MuonHitResidualPlots         m_oResidualPlots_MDT;
      Muon::MuonHitResidualPlots         m_oResidualPlots_RPC_eta;
      Muon::MuonHitResidualPlots         m_oResidualPlots_RPC_phi;
      Muon::MuonHitResidualPlots         m_oResidualPlots_TGC_wire;
      Muon::MuonHitResidualPlots         m_oResidualPlots_TGC_strip;
      Muon::MuonHitResidualPlots         m_oResidualPlots_CSC_eta;
      Muon::MuonHitResidualPlots         m_oResidualPlots_CSC_phi;
  
};
 
}
#endif
