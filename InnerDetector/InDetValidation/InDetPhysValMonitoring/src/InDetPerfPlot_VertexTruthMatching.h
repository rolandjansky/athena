/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEXTRUTHMATCHING
#define INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEXTRUTHMATCHING
/**
 * @file InDetPerfPlot_VertexTruthMatching.h
 * @author Valentina Cairo
 **/

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/VertexFwd.h"

// std includes
#include <string>

class TH1;

///class holding plots for truth matched vertices
class InDetPerfPlot_VertexTruthMatching: public InDetPlotBase {
public:
  InDetPerfPlot_VertexTruthMatching(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::Vertex& vertex);
private:
  ///truth type
  TH1* m_vx_type_truth;
  ///vertex reco efficiency
  TEfficiency* m_vx_reco_efficiency;
  ///@}

  // plot base has no default implementation of this; we use it to book the histos
  void initializePlots();
};




#endif
