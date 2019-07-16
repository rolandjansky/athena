/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "xAODTrackCaloClusterAuxContainerCnv.h"
#include "xAODTrackCaloClusterAuxContainerCnv_v1.h"

// EDM include(s):
#include "xAODPFlow/TrackCaloClusterContainer.h"

xAODTrackCaloClusterAuxContainerCnv::
xAODTrackCaloClusterAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackCaloClusterAuxContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrackCaloClusterAuxContainerCnv::initialize() {

   // Call the base class's initialize:
   CHECK( xAODTrackCaloClusterAuxContainerCnvBase::initialize() );

   // Return gracefully:
   return StatusCode::SUCCESS;
}

xAOD::TrackCaloClusterAuxContainer*
xAODTrackCaloClusterAuxContainerCnv::
createPersistent( xAOD::TrackCaloClusterAuxContainer* trans ) {

   // Create a copy of the container:
  std::unique_ptr< xAOD::TrackCaloClusterAuxContainer > result(
                                                               SG::copyThinned( *trans, IThinningSvc::instance() ) );
  
   // Return the thinned object:
   return result.release();
}

xAOD::TrackCaloClusterAuxContainer*
xAODTrackCaloClusterAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "E3492C37-2469-4346-BCBA-7A18CACD46AC" );
  static const pool::Guid v2_guid( "2F39C8C4-D2F2-4C35-91BE-F81AC9054C9F" );
  
  // Check which version of the container we're reading:
  if ( compareClassGuid( v2_guid ) ){
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::TrackCaloClusterAuxContainer >();
  }
  if ( compareClassGuid( v1_guid ) ){
    // The v1 converter:
    static xAODTrackCaloClusterAuxContainerCnv_v1 converter;
    // Read in the v1 version:
    std::unique_ptr< xAOD::TrackCaloClusterAuxContainer_v1 >
      old( poolReadObject< xAOD::TrackCaloClusterAuxContainer_v1 >() );
    
    // Return the converted object:
    return converter.createTransient( old.get(), msg() );
  }
  

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
    "xAOD::TrackCaloClusterAuxContainer found" );
  return 0;
}
