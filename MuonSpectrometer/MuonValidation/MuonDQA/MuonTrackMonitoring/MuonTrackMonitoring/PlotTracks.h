/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_PLOTTRACKS_H
#define MUONTRACKMONITORING_PLOTTRACKS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"

class PlotTracks:public PlotBase{

    public:

        PlotTracks(PlotBase* pParent, std::string sDir);
        
        void fill(const xAOD::Muon&          muon);
        void fill(const xAOD::TrackParticle& track);
        
        Trk::ImpactPlots    m_plots_Impact;
        Trk::RecoInfoPlots  m_plots_TrkRecoInfo;
        Trk::MSHitPlots     m_plots_MSHit;
        
};

#endif
