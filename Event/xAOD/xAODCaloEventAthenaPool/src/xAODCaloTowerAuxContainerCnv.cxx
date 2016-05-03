/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloTowerAuxContainerCnv.cxx 700671 2015-10-15 08:33:38Z wlampl $

// System include(s):
#include <exception>
//#include <memory>

// Local include(s):
#include "xAODCaloTowerAuxContainerCnv.h"
//#include "xAODCaloTowerAuxContainerCnv_v1.h"

xAODCaloTowerAuxContainerCnv::xAODCaloTowerAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODCaloTowerAuxContainerCnvBase( svcLoc ) {

}

xAOD::CaloTowerAuxContainer*
xAODCaloTowerAuxContainerCnv::
createPersistent( xAOD::CaloTowerAuxContainer* trans ) {
   // Create a copy of the container:
  return new  xAOD::CaloTowerAuxContainer(*trans);
}

xAOD::CaloTowerAuxContainer* xAODCaloTowerAuxContainerCnv::createTransient() {

   // The known ID(s) of this container:
  static const pool::Guid v1_guid( "CE74E4D1-D2F4-4CED-8191-EC26D8836575" );

  // Check which version of the container we're reading:

  if( compareClassGuid( v1_guid ) ) {
    // It's the latest version, read it directly:
    return poolReadObject< xAOD::CaloTowerAuxContainer >();
  }
  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
			    "xAOD::CaloTowerAuxContainer found" );
  return nullptr;
}
