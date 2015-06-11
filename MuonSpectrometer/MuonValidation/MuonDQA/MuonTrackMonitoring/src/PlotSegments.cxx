/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackMonitoring/PlotSegments.h"

PlotSegments::PlotSegments(PlotBase* pParent, std::string sDir):PlotBase(pParent, sDir),
    m_plots_Segments(this, "/")
{}

void PlotSegments::fill(const xAOD::Muon& muon){

  std::cout << muon.pt() << std::endl;
  // segments = get_associated_segments(muon)
  // for segment in segments:
  //     fill_segments(segment)

}

void PlotSegments::fill(const xAOD::MuonSegment& segment){

  std::cout << segment.sector() << std::endl;
    // m_plots_Segments.fill(segment);

}

