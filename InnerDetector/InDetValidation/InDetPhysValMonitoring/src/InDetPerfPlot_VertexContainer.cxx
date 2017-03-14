/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file InDetPerfPlot_VertexContainer.cxx
 * @author Simone Pagan Griso
 **/

#include "InDetPerfPlot_VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

InDetPerfPlot_VertexContainer::InDetPerfPlot_VertexContainer(InDetPlotBase* pParent,
                                                             const std::string& sDir) : InDetPlotBase(pParent, sDir),
  m_vx_n_vs_mu{} {
  // nop
}

void
InDetPerfPlot_VertexContainer::initializePlots() {
  const bool prependDirectory(false);

  m_vx_n_vs_mu = Book2D("vx_n_vs_mu", "Number of vertices vs #mu;N vertices;#mu;Entries", 70, 0.0, 70., 100, 0.0, 100,
                        prependDirectory);
}

void
InDetPerfPlot_VertexContainer::fill(const xAOD::VertexContainer& vertices, const xAOD::EventInfo& ei) {
  // fill number of vertices
  int nGoodVertices(0);

  for (const auto& vtx : vertices.stdcont()) {
    if (vtx->vertexType() == xAOD::VxType::NoVtx) {
      continue; // skip dummy vertex
    }
    nGoodVertices++;
  }
  fillHisto(m_vx_n_vs_mu, nGoodVertices, ei.actualInteractionsPerCrossing());
}
