/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEXCONTAINER
#define INDETPHYSVALMONITORING_INDETPERFPLOT_VERTEXCONTAINER
/**
 * @file InDetPerfPlot_VertexContainer.h
 * @author Simone Pagan Griso
 **/


// std includes
#include <string>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODEventInfo/EventInfo.h"

class TH2;

///class holding general plots for xAOD::Vertex
class InDetPerfPlot_VertexContainer: public InDetPlotBase {
public:
  InDetPerfPlot_VertexContainer(InDetPlotBase* pParent, const std::string& dirName);

  // Fills vertex container information; also uses EventInfo
  void fill(const xAOD::VertexContainer& vertices, const xAOD::EventInfo& ei);
private:
  ///@name Number of vertices vs mu
  ///@{
  TH2* m_vx_n_vs_mu;
  ///@}

  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
};




#endif
