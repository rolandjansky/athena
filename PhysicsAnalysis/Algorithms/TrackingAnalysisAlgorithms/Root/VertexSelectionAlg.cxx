//
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//

// Local include(s):
#include "TrackingAnalysisAlgorithms/VertexSelectionAlg.h"

// EDM include(s):
#include "xAODTracking/VertexContainer.h"

namespace CP {

   VertexSelectionAlg::VertexSelectionAlg( const std::string& name,
                                           ISvcLocator* svcLoc )
   : EL::AnaAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties:
      declareProperty( "VertexContainer", m_vertexKey = "PrimaryVertices",
                       "Vertex container to check" );
      declareProperty( "MinVertices", m_minVertices = 1,
                       "Minimum number of vertices required" );
      declareProperty( "MinTracks", m_minTracks = 0,
                       "Minimum number of track particles required per "
                       "vertex" );
   }

   StatusCode VertexSelectionAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Requiring " << m_minVertices << " vertex/vertices from \""
                    << m_vertexKey << "\" with " << m_minTracks
                    << " track(s) each" );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode VertexSelectionAlg::execute() {

      // Retrieve the vertex container:
      const xAOD::VertexContainer* vertices = nullptr;
      ATH_CHECK( evtStore()->retrieve( vertices, m_vertexKey ) );

      // The number of "good" vertices found:
      unsigned goodVertices = 0;

      // Check how many "good" vertices we have in the current event:
      for( const xAOD::Vertex* vx : *vertices ) {
         // It has to be either a primary or a pileup vertex:
         if( ( vx->vertexType() != xAOD::VxType::PriVtx ) &&
             ( vx->vertexType() != xAOD::VxType::PileUp ) ) {
            continue;
         }
         // With the minimum number of tracks specified:
         if( vx->nTrackParticles() < m_minTracks ) {
            continue;
         }
         // Apparently this is a "good" vertex:
         ++goodVertices;
      }

      // Decide about the event:
      setFilterPassed( goodVertices >= m_minVertices );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace CP
