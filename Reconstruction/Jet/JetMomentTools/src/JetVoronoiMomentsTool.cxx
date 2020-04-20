/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/WriteDecorHandle.h"
#include "JetMomentTools/JetVoronoiMomentsTool.h"

// helpers
#include "JetMomentTools/JetVoronoiDiagramHelpers.h"


using JetVoronoiDiagramHelpers::coord;
using JetVoronoiDiagramHelpers::Point;
using JetVoronoiDiagramHelpers::Segment;
using JetVoronoiDiagramHelpers::SegmentList;
using JetVoronoiDiagramHelpers::Polygon;
using JetVoronoiDiagramHelpers::PolygonList;
using JetVoronoiDiagramHelpers::Diagram;


JetVoronoiMomentsTool::JetVoronoiMomentsTool(const std::string& name)
    : asg::AsgTool(name)
{
  declareInterface<IJetDecorator>(this);
}

StatusCode JetVoronoiMomentsTool::initialize() {

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetVoronoiMomentsTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }

  // Prepend jet collection name
  m_voronoiAreaKey = m_jetContainerName + "." + m_voronoiAreaKey.key();

  ATH_CHECK(m_voronoiAreaKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode JetVoronoiMomentsTool::decorate(const xAOD::JetContainer& jets) const {

    SG::WriteDecorHandle<xAOD::JetContainer, float> outputHandle(m_voronoiAreaKey);

    // setup diagram class
    Diagram voro ("VoronoiDiagram");
    voro.m_x_min = m_x_min;
    voro.m_x_max = m_x_max;
    voro.m_y_min = m_y_min;
    voro.m_y_max = m_y_max;
    if (voro.initialize().isFailure()) ATH_MSG_WARNING( "Initialization of diagram failed. This should never happen.");
    // add jets / set number of points
    for ( const xAOD::Jet * jet : jets ) {
        voro.m_voro_vtx.Add(jet->eta(), jet->phi());
    }
    // create diagram and calculate area per cell
    if ( voro.createVoronoiDiagram().isFailure() ) {
        ATH_MSG_WARNING("Could not calculate Voronoi diagram");
    }
    // decorate the jets
    for (const xAOD::Jet *jet : jets ){
      outputHandle(*jet) = voro.getCellArea(jet->eta(), jet->phi());
    }
    return StatusCode::SUCCESS;
}
