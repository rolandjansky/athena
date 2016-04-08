/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetConfAuxContainerCnv.cxx 659544 2015-04-08 17:39:38Z wsfreund $
// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#include "xAODCaloRings/versions/RingSetConfAuxContainer_v1.h"

// Local include(s):
#include "xAODRingSetConfAuxContainerCnv.h"
//#include "xAODRingSetConfAuxContainerCnv_v0.h"

xAODRingSetConfAuxContainerCnv::xAODRingSetConfAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODRingSetConfAuxContainerCnvBase( svcLoc ) {

}

xAOD::RingSetConfAuxContainer* xAODRingSetConfAuxContainerCnv::createPersistent( 
    xAOD::RingSetConfAuxContainer* trans ) 
{

  // Create a copy of the container:
  xAOD::RingSetConfAuxContainer* result =
     new xAOD::RingSetConfAuxContainer( *trans );

  return result;
}

xAOD::RingSetConfAuxContainer* xAODRingSetConfAuxContainerCnv::createTransient() {

  // The known ID(s) of this container:
  static const pool::Guid v1_guid( "B080CC3F-D375-49BE-B400-F4B3115417FD" );

  // Check which version of the container we're reading:

  if( compareClassGuid( v1_guid ) ) {
     // It's the latest version, read it directly:
     return poolReadObject< xAOD::RingSetConfAuxContainer >();

  } //else if( compareClassGuid( v0_guid ) ) { // Kept as reference if needed.

    // The v0 converter:
    //static xAODRingSetConfAuxContainerCnv_v0 converter;
    
    // Read in the v0 version:
    //std::unique_ptr< xAOD::RingSetConfAuxContainer_v0 >
      //old( poolReadObject< xAOD::RingSetConfAuxContainer_v0 >() );
    
    // Return the converted object:
    //return converter.createTransient( old.get(), msg() );

  //} 

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
                            "xAOD::RingSetConfAuxContainer found" );
  return 0;
}
