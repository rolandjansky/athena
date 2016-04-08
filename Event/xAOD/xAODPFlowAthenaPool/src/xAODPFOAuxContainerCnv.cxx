/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODPFOAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODPFOAuxContainerCnv::xAODPFOAuxContainerCnv(ISvcLocator* svcLoc) : xAODPFOAuxContainerCnvBase(svcLoc) {}

xAOD::PFOAuxContainer* xAODPFOAuxContainerCnv::createPersistent(xAOD::PFOAuxContainer* trans){

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());

}

xAOD::PFOAuxContainer* xAODPFOAuxContainerCnv::createTransient(){

  static pool::Guid v1_guid( "F691F845-4A3D-466A-9187-FED7837D9372" );

  if( compareClassGuid( v1_guid ) ) {
    xAOD::PFOAuxContainer* trans_v1 = poolReadObject< xAOD::PFOAuxContainer >();
    return trans_v1;
  }
  else throw std::runtime_error("Unsupported persistent version of xAOD::PFOAuxContainer");

  return NULL;

}


