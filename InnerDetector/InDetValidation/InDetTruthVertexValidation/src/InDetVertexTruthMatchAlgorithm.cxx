/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetVertexTruthMatchAlgorithm.h"

#include "xAODTracking/VertexContainer.h"


InDetVertexTruthMatchAlgorithm::InDetVertexTruthMatchAlgorithm( const std::string& name, ISvcLocator* svcLoc )
   : AthAlgorithm( name, svcLoc ),
     m_matchTool( "InDetVertexTruthMatchTool/InDetVertexTruthMatchTool", this ) {

   declareProperty( "SGKey", m_sgKey = "PrimaryVertices" );

   declareProperty( "VertexTruthMatchTool", m_matchTool );
}

StatusCode InDetVertexTruthMatchAlgorithm::initialize() {

   // Greet the user:
   ATH_MSG_INFO( "Initialising" );
   ATH_MSG_DEBUG( "SGKey = " << m_sgKey );

   // Retrieve the tools:
   ATH_CHECK( m_matchTool.retrieve() );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

StatusCode InDetVertexTruthMatchAlgorithm::execute() {

  //Retrieve the vertices:
  const xAOD::VertexContainer * vxContainer = nullptr;
  ATH_CHECK( evtStore()->retrieve( vxContainer, m_sgKey ) );

  //pass to the tool for decoration:
  ATH_CHECK( m_matchTool->matchVertices( *vxContainer ) );

  return StatusCode::SUCCESS;

}
