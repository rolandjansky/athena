/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


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
    : JetModifierBase(name)
      //, m_voronoitool("")
    , m_x_min(-10.)
    , m_x_max( 10.)
    , m_y_min(-4. )
    , m_y_max( 4. )
      //, m_jetcontainer ("")
      //, m_scaleIntFloat (1e3)
{
    declareProperty("AreaXmin",m_x_min);
    declareProperty("AreaXmax",m_x_max);
    declareProperty("AreaYmin",m_y_min);
    declareProperty("AreaYmax",m_y_max);
    //declareProperty("JetContainerName",m_jetcontainer);

}

int JetVoronoiMomentsTool::modify(xAOD::JetContainer& jets) const {
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
    for ( xAOD::Jet *jet : jets ) modifyJet(*jet,voro);
  return StatusCode::SUCCESS;
}

int JetVoronoiMomentsTool::modifyJet(xAOD::Jet&) const {
    ATH_MSG_WARNING("Don't use this function. We need to compare space with other jets.");
    return 0;
}

int JetVoronoiMomentsTool::modifyJet(xAOD::Jet& jet, const Diagram & voro) const {
    // Collate info
    float VoronoiArea  = voro.getCellArea(jet.eta(), jet.phi() );
    // Set info
    jet.setAttribute( "VoronoiArea" , VoronoiArea );
    return 0;
}


