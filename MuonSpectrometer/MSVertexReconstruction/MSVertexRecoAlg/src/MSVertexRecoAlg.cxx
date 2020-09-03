/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MSVertexRecoAlg/MSVertexRecoAlg.h"

#include "MSVertexUtils/Tracklet.h"
#include "MSVertexUtils/MSVertex.h"

//** ----------------------------------------------------------------------------------------------------------------- **//


MSVertexRecoAlg::MSVertexRecoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator) {
}

//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode MSVertexRecoAlg::initialize(){

  //* Retrieve MSVertexRecoTool *//
  ATH_CHECK( m_vertexRecoTool.retrieve() );
  ATH_MSG_DEBUG( "Retrieved tool " << m_vertexRecoTool );

  //* Retrieve MSVertexTrackletTool *//
  ATH_CHECK( m_vertexTrackletTool.retrieve() );
  ATH_MSG_DEBUG( "Retrieved tool " << m_vertexTrackletTool );

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode MSVertexRecoAlg::execute (const EventContext& ctx) const {

  std::vector<Tracklet> tracklets;
  std::vector<MSVertex*> vertices;

  StatusCode sc = m_vertexTrackletTool->findTracklets(tracklets, ctx);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Failed vertex tracklet " );
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_DEBUG ("Tracklet reconstruction tool called");

  sc = m_vertexRecoTool->findMSvertices(tracklets, vertices, ctx);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Failed vertex reco " );
    return StatusCode::FAILURE;
  } else
    ATH_MSG_DEBUG ("Vertex reconstruction tool called");

  ATH_MSG_DEBUG ("Found " << tracklets.size() << " tracklets and " << vertices.size() << " vertices in the MS");

  return StatusCode::SUCCESS;
}
