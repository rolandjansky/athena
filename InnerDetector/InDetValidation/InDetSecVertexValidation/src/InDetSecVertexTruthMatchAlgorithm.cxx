/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetSecVertexTruthMatchAlgorithm.h"

#include "xAODTracking/VertexContainer.h"
#include "xAODTruth/TruthVertexContainer.h"


InDetSecVertexTruthMatchAlgorithm::InDetSecVertexTruthMatchAlgorithm( const std::string& name, ISvcLocator* svcLoc )
   : AthAlgorithm( name, svcLoc ),
     m_matchTool( "InDetSecVertexTruthMatchTool/InDetSecVertexTruthMatchTool", this ) {

   declareProperty( "SecVertexSGKey", m_secVtxSGKey = "VrtSecInclusive_SecondaryVertices" );
   declareProperty( "TruthVertexSGKey", m_truthVtxSGKey = "TruthBSMWithDecayVertices" );
   declareProperty( "VertexTruthMatchTool", m_matchTool );
}

StatusCode InDetSecVertexTruthMatchAlgorithm::initialize() {

   // Greet the user:
   ATH_MSG_INFO( "Initialising" );
   ATH_MSG_DEBUG( "SecVertexSGKey = " << m_secVtxSGKey );
   ATH_MSG_DEBUG( "TruthVertexSGKey = " << m_truthVtxSGKey );

   // Retrieve the tools:
   ATH_CHECK( m_matchTool.retrieve() );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

StatusCode InDetSecVertexTruthMatchAlgorithm::execute() {

  //Retrieve the vertices:
  const xAOD::VertexContainer * inSecVert = nullptr;
  ATH_CHECK( evtStore()->retrieve( inSecVert, m_secVtxSGKey ) );

  //Retrieve truth vertices:
  const xAOD::TruthVertexContainer * inTruthVert = nullptr;
  ATH_CHECK( evtStore()->retrieve( inTruthVert, m_truthVtxSGKey ) );

  //pass to the tool for decoration:
  ATH_CHECK( m_matchTool->matchVertices( *inSecVert, *inTruthVert ) );
  ATH_CHECK( m_matchTool->labelTruthVertices( *inTruthVert ) );

  return StatusCode::SUCCESS;

}
