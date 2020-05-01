/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEXTRUTHMATCHING
#define INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEXTRUTHMATCHING
/**
 * @file InDetPerfPlot_VertexTruthMatching.h
 * @author Valentina Cairo, mbasso
 **/

// local includes
#include "InDetPlotBase.h"
#include "InDetPhysValMonitoringUtilities.h"

// Tracking includes:
#include "xAODTracking/VertexFwd.h"

// xAOD truth object includes:
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthPileupEventContainer.h"
#include "xAODTruth/TruthVertex.h"

// Vertex validation:
#include "InDetTruthVertexValidation/InDetVertexTruthMatchTool.h"
#include "InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h"

// std includes
#include <string>

class TH1;

///class holding plots for truth matched vertices
class InDetPerfPlot_VertexTruthMatching: public InDetPlotBase {
public:
  InDetPerfPlot_VertexTruthMatching(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::Vertex& vertex);
  void fill(const xAOD::VertexContainer& vertexContainer, const std::vector<const xAOD::TruthVertex*>& truthVertices);
private:
  ///truth type
  TH1* m_vx_type_truth;
  ///hardscatter classification
  TH1* m_vx_hs_classification;
  ///vertex reco efficiency
  TProfile* m_vx_nReco_vs_nTruth_inclusive;
  TProfile* m_vx_nReco_vs_nTruth_matched;
  TProfile* m_vx_nReco_vs_nTruth_merged;
  TProfile* m_vx_nReco_vs_nTruth_split;
  TProfile* m_vx_nReco_vs_nTruth_fake;
  TProfile* m_vx_nReco_vs_nTruth_dummy;
  TProfile* m_vx_nReco_vs_nTruth_clean;
  TProfile* m_vx_nReco_vs_nTruth_lowpu;
  TProfile* m_vx_nReco_vs_nTruth_highpu;
  TProfile* m_vx_nReco_vs_nTruth_hssplit;
  TProfile* m_vx_nReco_vs_nTruth_none;
  ///@}

  // plot base has no default implementation of this; we use it to book the histos
  void initializePlots();
};

#endif
