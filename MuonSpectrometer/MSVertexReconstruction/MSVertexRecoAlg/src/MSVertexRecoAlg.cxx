/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MSVertexRecoAlg/MSVertexRecoAlg.h"
#include "GaudiKernel/MsgStream.h"

#include "MSVertexUtils/Tracklet.h"
#include "MSVertexUtils/MSVertex.h"


//** ----------------------------------------------------------------------------------------------------------------- **//


MSVertexRecoAlg::MSVertexRecoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_vertexTrackletTool("Muon::MSVertexTrackletTool/MSVertexTrackletTool"),
  m_vertexRecoTool("Muon::MSVertexRecoTool/MSVertexRecoTool")
{
  declareProperty("MSVertexTrackletTool", m_vertexTrackletTool);
  declareProperty("MSVertexRecoTool", m_vertexRecoTool);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


MSVertexRecoAlg::~MSVertexRecoAlg() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode MSVertexRecoAlg::initialize(){

  //* Retrieve MSVertexRecoTool *//
  StatusCode sc = m_vertexRecoTool.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Failed to locate tool " << m_vertexRecoTool );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG( "Retrieved tool " << m_vertexRecoTool );

  //* Retrieve MSVertexTrackletTool *//
  sc = m_vertexTrackletTool.retrieve();
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Failed to locate tool " << m_vertexTrackletTool );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG( "Retrieved tool " << m_vertexTrackletTool );

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode MSVertexRecoAlg::execute() {

  std::vector<Tracklet> m_tracklets;
  std::vector<MSVertex*> m_vertices;

  StatusCode sc = m_vertexTrackletTool->findTracklets(m_tracklets);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Failed vertex tracklet " );
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_DEBUG ("Tracklet reconstruction tool called");

  sc = m_vertexRecoTool->findMSvertices(m_tracklets, m_vertices);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Failed vertex reco " );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG ("Vertex reconstruction tool called");

  ATH_MSG_DEBUG ("Found " << m_tracklets.size() << " tracklets and " << m_vertices.size() << " vertices in the MS");

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode MSVertexRecoAlg::finalize() {
  
  return StatusCode::SUCCESS;
}
