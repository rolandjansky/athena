/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackParticleAuxContainerCnv.cxx 791368 2017-01-05 08:55:07Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// ROOT include(s):
#include <TClass.h>

// Athena/Gaudi include(s):
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "xAODTrackParticleAuxContainerCnv.h"
#include "xAODTrackParticleAuxContainerCnv_v1.h"
#include "xAODTrackParticleAuxContainerCnv_v2.h"
#include "xAODTrackParticleAuxContainerCnv_v3.h"
#include "xAODTrackParticleAuxContainerCnv_v4.h"

// EDM include(s):
#include "xAODTracking/TrackParticleContainer.h"

xAODTrackParticleAuxContainerCnv::
xAODTrackParticleAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackParticleAuxContainerCnvBase( svcLoc ),
     m_compressorTool( "xAODMaker::TrackParticleCompressorTool/"
                       "xAODTrackParticleCompressorTool" ) {

}

StatusCode xAODTrackParticleAuxContainerCnv::initialize() {

   // Retrieve the compression tool:
   CHECK( m_compressorTool.retrieve() );

   // Call the base class's initialize:
   CHECK( xAODTrackParticleAuxContainerCnvBase::initialize() );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

xAOD::TrackParticleAuxContainer*
xAODTrackParticleAuxContainerCnv::
createPersistentWithKey( xAOD::TrackParticleAuxContainer* trans,
                         const std::string& key ) {

   // Load the necessary ROOT class(es):
   static char const* const NAME =
     "std::vector<ElementLink<DataVector<Trk::Track> > >";
   static TClass const* const cls = TClass::GetClass( NAME );
   if( ! cls ) {
     ATH_MSG_ERROR( "Couldn't load dictionary for type: " << NAME );
   }

   // This makes a copy of the container, with any thinning applied.
   std::unique_ptr< xAOD::TrackParticleAuxContainer > result
     ( xAODTrackParticleAuxContainerCnvBase::createPersistentWithKey (trans, key) );


   // Create a helper object for the float compression:
   xAOD::TrackParticleContainer helper;
   for( size_t i = 0; i < result->size(); ++i ) {
      helper.push_back( std::make_unique<xAOD::TrackParticle>() );
   }
   helper.setStore( result.get() );

   // Compress the track particles' payload:
   for( xAOD::TrackParticle* tp : helper ) {
      // Check for possible compression errors:
      if( ! m_compressorTool->compress( *tp ).isSuccess() ) {
         REPORT_MESSAGE( MSG::ERROR )
               << "Failed to compress track particle";
         return nullptr;
      }
   }

   // Return the thinned and compressed object:
   return result.release();
}

