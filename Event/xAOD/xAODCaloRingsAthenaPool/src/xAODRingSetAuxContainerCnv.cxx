/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingSetAuxContainerCnv.cxx 659544 2015-04-08 17:39:38Z wsfreund $
// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#include "xAODCaloRings/versions/RingSetAuxContainer_v1.h"

// Local include(s):
#include "xAODRingSetAuxContainerCnv.h"
//#include "xAODRingSetAuxContainerCnv_v0.h"

xAODRingSetAuxContainerCnv::xAODRingSetAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODRingSetAuxContainerCnvBase( svcLoc ) {

}

xAOD::RingSetAuxContainer* xAODRingSetAuxContainerCnv::createPersistent( 
    xAOD::RingSetAuxContainer* trans ) 
{

  // Create a copy of the container:
  xAOD::RingSetAuxContainer* result =
     new xAOD::RingSetAuxContainer( *trans );

  return result;
}

xAOD::RingSetAuxContainer* xAODRingSetAuxContainerCnv::createTransient() {

  // The known ID(s) of this container:
  static const pool::Guid v1_guid( "18C4774A-EE60-4E78-843D-02B6235FBCF4" );

  // Check which version of the container we're reading:

  if( compareClassGuid( v1_guid ) ) {
     // It's the latest version, read it directly:
     return poolReadObject< xAOD::RingSetAuxContainer >();

  } //else if( compareClassGuid( v0_guid ) ) { // Kept as reference if needed.

    // The v0 converter:
    //static xAODRingSetAuxContainerCnv_v0 converter;
    
    // Read in the v0 version:
    //std::unique_ptr< xAOD::RingSetAuxContainer_v0 >
      //old( poolReadObject< xAOD::RingSetAuxContainer_v0 >() );
    
    // Return the converted object:
    //return converter.createTransient( old.get(), msg() );

  //} 

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
                            "xAOD::RingSetAuxContainer found" );
  return 0;
}
