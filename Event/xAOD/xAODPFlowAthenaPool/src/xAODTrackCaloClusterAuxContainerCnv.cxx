// $Id: xAODTrackCaloClusterAuxContainerCnv.cxx 791368 2017-01-05 08:55:07Z hrivnac $

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
  
  // Check which version of the container we're reading:
  if ( compareClassGuid( v1_guid ) )
    return poolReadObject< xAOD::TrackCaloClusterAuxContainer >();
  

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
    "xAOD::TrackCaloClusterAuxContainer found" );
  return 0;
}