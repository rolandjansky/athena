/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKMONITORING_PLOTSEGMENTS_H
#define MUONTRACKMONITORING_PLOTSEGMENTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "MuonHistUtils/MuonSegmentPlots.h"

#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonSegment.h"

class PlotSegments:public PlotBase{

    public:

        PlotSegments(PlotBase* pParent, std::string sDir);

        void fill(const xAOD::Muon&        muon);
        void fill(const xAOD::MuonSegment& segment);
        
        Muon::MuonSegmentPlots m_plots_Segments;
        
};

#endif
