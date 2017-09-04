/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTaggingAuxFixerAlg.cxx 802614 2017-04-10 13:30:46Z krasznaa $

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthLinks/ElementLink.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// Local include(s):
#include "BTaggingAuxFixerAlg.h"

namespace xAODMaker {

   /// Convenience type definition for the TrackParticle links
   typedef std::vector< ElementLink< xAOD::TrackParticleContainer > >
      TPLinks_t;
   /// Convenience type definition for the TrackParticle links
   typedef std::vector< ElementLink< xAOD::VertexContainer > >
      VxLinks_t;

   BTaggingAuxFixerAlg::BTaggingAuxFixerAlg( const std::string& name,
                                             ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ) {

      // Declare the algorithm's properties:
      declareProperty( "Containers", m_containers );
      declareProperty( "TPLinkNames",
                       m_tpLinkNames = { "SV1_TrackParticleLinks",
                                         "SV1_badTracksIP",
                                         "MSV_badTracksIP",
                                         "JetFitter_tracksAtPVlinks" } );
      declareProperty( "VxLinkNames",
                       m_vxLinkNames = { "SV1_vertices", "MSV_vertices" } );
   }

   StatusCode BTaggingAuxFixerAlg::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "Will be fixing dynamic variables in container(s):" );
      ATH_MSG_DEBUG( "  " << m_containers );

      // The aux type registry:
      auto& reg = SG::AuxTypeRegistry::instance();

      // Initialise the TraclParticle link names and IDs:
      for( const std::string& name : m_tpLinkNames ) {
         m_tpLinkNameIDs.push_back(
                  std::make_pair( name,
                                  reg.getAuxID< TPLinks_t >( name ) ) );
      }

      // Initialise the Vertex link names and IDs:
      for( const std::string& name : m_vxLinkNames ) {
         m_vxLinkNameIDs.push_back(
                  std::make_pair( name,
                                  reg.getAuxID< VxLinks_t >( name ) ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode BTaggingAuxFixerAlg::execute() {

      // Loop over the specified containers:
      for( const std::string& cname : m_containers ) {

         // Retrieve the container with a constant pointer. Since we probably
         // can't get a non-const pointer to it anyway.
         const xAOD::AuxContainerBase* store = nullptr;
         ATH_CHECK( evtStore()->retrieve( store, cname ) );

         // If the store is empty, stop here:
         if( ! store->size() ) {
            continue;
         }

         // Remember the store's size:
         const size_t size = store->size();

         // Fix the track particle link variables:
         for( const auto& nameID : m_tpLinkNameIDs ) {

            // Get the std::vector variable:
            const void* ptr = store->getIOData( nameID.second );
            if( ! ptr ) {
               // Apparently the variable doesn't exist...
               continue;
            }
            const std::vector< TPLinks_t >* clinks =
                  reinterpret_cast< const std::vector< TPLinks_t >* >( ptr );

            // If it's of the right size, then don't bother it:
            if( clinks->size() == size ) {
               continue;
            }

            // If not, then we have to forcefully resize it:
            std::vector< TPLinks_t >* links =
                  const_cast< std::vector< TPLinks_t >* >( clinks );
            links->resize( size );

            // Tell the user what happened:
            ATH_MSG_VERBOSE( "Fixed variable: \"" << cname << nameID.first
                             << "\"" );
         }

         // Fix the vertex link variables:
         for( const auto& nameID : m_vxLinkNameIDs ) {

            // Get the std::vector variable:
            const void* ptr = store->getIOData( nameID.second );
            if( ! ptr ) {
               // Apparently the variable doesn't exist...
               continue;
            }
            const std::vector< VxLinks_t >* clinks =
                  reinterpret_cast< const std::vector< VxLinks_t >* >( ptr );

            // If it's of the right size, then don't bother it:
            if( clinks->size() == size ) {
               continue;
            }

            // If not, then we have to forcefully resize it:
            std::vector< VxLinks_t >* links =
                  const_cast< std::vector< VxLinks_t >* >( clinks );
            links->resize( size );

            // Tell the user what happened:
            ATH_MSG_VERBOSE( "Fixed variable: \"" << cname << nameID.first
                             << "\"" );
         }

      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
