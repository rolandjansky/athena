/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloRingsAuxContainerCnv.cxx 659544 2015-04-08 17:39:38Z wsfreund $
// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#include "xAODCaloRings/versions/CaloRingsAuxContainer_v1.h"

// Local include(s):
#include "xAODCaloRingsAuxContainerCnv.h"
//#include "xAODCaloRingsAuxContainerCnv_v0.h"

xAODCaloRingsAuxContainerCnv::xAODCaloRingsAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCaloRingsAuxContainerCnvBase( svcLoc ) {

}

xAOD::CaloRingsAuxContainer*
xAODCaloRingsAuxContainerCnv::
createPersistent( xAOD::CaloRingsAuxContainer* trans ) {

  // Create a copy of the container:
  xAOD::CaloRingsAuxContainer* result =
     new xAOD::CaloRingsAuxContainer( *trans );

  return result;
}

xAOD::CaloRingsAuxContainer* xAODCaloRingsAuxContainerCnv::createTransient() {

  // The known ID(s) of this container:
  static const pool::Guid v1_guid( "581CAE9D-55D9-445A-9D64-2013091FE019" );

  // Check which version of the container we're reading:

  if( compareClassGuid( v1_guid ) ) {
     // It's the latest version, read it directly:
     return poolReadObject< xAOD::CaloRingsAuxContainer >();

  } //else if( compareClassGuid( v0_guid ) ) { // Kept as reference if needed.

    // The v0 converter:
    //static xAODCaloRingsAuxContainerCnv_v0 converter;
    
    // Read in the v0 version:
    //std::unique_ptr< xAOD::CaloRingsAuxContainer_v0 >
      //old( poolReadObject< xAOD::CaloRingsAuxContainer_v0 >() );
    
    // Return the converted object:
    //return converter.createTransient( old.get(), msg() );

  //} 

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
                            "xAOD::CaloRingsAuxContainer found" );
  return 0;
}
