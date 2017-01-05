/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackParticleAuxContainerCnv.cxx 791368 2017-01-05 08:55:07Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// ROOT include(s):
#include <TClass.h>

// Athena/Gaudi include(s):
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "xAODTrackParticleAuxContainerCnv.h"
#include "xAODTrackParticleAuxContainerCnv_v1.h"
#include "xAODTrackParticleAuxContainerCnv_v2.h"

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
createPersistent( xAOD::TrackParticleAuxContainer* trans ) {

   // Load the necessary ROOT class(es):
   static bool dictLoaded = false;
   if( ! dictLoaded ) {
      static const char* NAME =
         "std::vector<ElementLink<DataVector<Trk::Track> > >";
      if( ! TClass::GetClass( NAME ) ) {
         ATH_MSG_ERROR( "Couldn't load dictionary for type: " << NAME );
      }
      dictLoaded = true;
   }

   // Create a copy of the container:
   std::unique_ptr< xAOD::TrackParticleAuxContainer > result(
            SG::copyThinned( *trans, IThinningSvc::instance() ) );

   // Create a helper object for the float compression:
   xAOD::TrackParticleContainer helper;
   for( size_t i = 0; i < result->size(); ++i ) {
      helper.push_back( new xAOD::TrackParticle() );
   }
   helper.setStore( result.get() );

   // Compress the track particles' payload:
   for( xAOD::TrackParticle* tp : helper ) {
      // Check for possible compression errors:
      if( ! m_compressorTool->compress( *tp ).isSuccess() ) {
         REPORT_MESSAGE( MSG::ERROR )
               << "Failed to compress track particle";
         return 0;
      }
   }

   // Return the thinned and compressed object:
   return result.release();
}

xAOD::TrackParticleAuxContainer*
xAODTrackParticleAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "C3B01EA0-CA87-4C96-967F-E0F9A75BD370" );
  static const pool::Guid v2_guid( "53031BE5-2156-41E8-B70C-41A1C0572FC5" );
  static const pool::Guid v3_guid( "F41DF744-242D-11E6-B472-02163E010CEC" );

  // Check which version of the container we're reading:
  if ( compareClassGuid( v3_guid ) ){
    return poolReadObject< xAOD::TrackParticleAuxContainer >(); 
  } else if ( compareClassGuid( v2_guid ) ){
    // It's the latest version, read it directly:
    // Read in the v1 version: 
    static xAODTrackParticleAuxContainerCnv_v2 converter; 
    std::unique_ptr< xAOD::TrackParticleAuxContainer_v2 > 
      old( poolReadObject< xAOD::TrackParticleAuxContainer_v2 >() ); 

    return converter.createTransient( old.get(), msg() );
  } else if( compareClassGuid( v1_guid ) ) {
    
    // The v1 converter: 
    static xAODTrackParticleAuxContainerCnv_v1 converter; 
 
    // Read in the v1 version: 
    std::unique_ptr< xAOD::TrackParticleAuxContainer_v1 > 
      old( poolReadObject< xAOD::TrackParticleAuxContainer_v1 >() ); 
 
    // Return the converted object: 
    return converter.createTransient( old.get(), msg() );     
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
    "xAOD::TrackParticleAuxContainer found" );
  return 0;
}
