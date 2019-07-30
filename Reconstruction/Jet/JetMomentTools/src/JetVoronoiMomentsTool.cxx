/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    , m_x_min(-10.)
    , m_x_max( 10.)
    , m_y_min(-4. )
    , m_y_max( 4. )
{
    declareInterface<IJetDecorator>(this);
    declareProperty("AreaXmin",m_x_min);
    declareProperty("AreaXmax",m_x_max);
    declareProperty("AreaYmin",m_y_min);
    declareProperty("AreaYmax",m_y_max);
    declareProperty("VoronoiAreaDecorKey", m_voronoiAreaKey = "");

}

StatusCode JetVoronoiMomentsTool::initialize() {
  ATH_CHECK(m_voronoiAreaKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode JetVoronoiMomentsTool::decorate(const xAOD::JetContainer& jets) const {

    if(m_voronoiAreaKey.empty()){
      ATH_MSG_ERROR("JetVoronoiMomentsTool wasn't given a key for its output decoration!");
      return StatusCode::FAILURE;
    }

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
    SG::WriteDecorHandle<xAOD::JetContainer, float> outputHandle(m_voronoiAreaKey);
    for (const xAOD::Jet *jet : jets ){
      outputHandle(*jet) = voro.getCellArea(jet->eta(), jet->phi());
    }
    return StatusCode::SUCCESS;
}
