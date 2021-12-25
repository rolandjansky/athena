/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_RECOMUONTRACKPLOTORGANIZER_H
#define MUONHISTUTILS_RECOMUONTRACKPLOTORGANIZER_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"

#include "TrkValHistUtils/ImpactPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "MuonHistUtils/IDHitSummaryPlots.h"
#include "xAODTracking/TrackParticle.h"
#include <vector>

namespace Muon {

  enum RECOTRKPLOTCLASS {TRK_PARAM,TRK_RECOINFO,TRK_IMPACT,TRK_MSHITS,TRK_IDHITS, MAX_RECOTRKPLOTCLASS};
  
  class RecoMuonTrackPlotOrganizer:public PlotBase {
  public:
    RecoMuonTrackPlotOrganizer(PlotBase* pParent, const std::string& sDir,std::vector<int> *selPlots=0);
    ~RecoMuonTrackPlotOrganizer();
    
    std::vector<PlotBase*> m_allPlots;
    std::vector<int> m_selPlots;  
    
    void fill(const xAOD::TrackParticle& tp, float weight=1.0);
    
    // Reco only information
    Trk::ParamPlots                 *m_oTrkParamPlots;
    Trk::RecoInfoPlots              *m_oTrkRecoInfoPlots;	  
    Trk::ImpactPlots                *m_oImpactPlots;
    Trk::MSHitPlots                 *m_oMSHitPlots;
    Muon::IDHitSummaryPlots         *m_oIDHitPlots;    
  };
 
}

#endif
