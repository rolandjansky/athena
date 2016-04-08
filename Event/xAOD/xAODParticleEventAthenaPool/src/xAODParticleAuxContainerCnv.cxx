/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODParticleAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODParticleAuxContainerCnv::xAODParticleAuxContainerCnv(ISvcLocator* svcLoc)
  : xAODParticleAuxContainerCnvBase(svcLoc)
{ }


xAOD::ParticleAuxContainer*
xAODParticleAuxContainerCnv::createPersistent(xAOD::ParticleAuxContainer* trans) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}


xAOD::ParticleAuxContainer* xAODParticleAuxContainerCnv::createTransient(){

  static pool::Guid v1_guid( "B53E64D2-C5EA-4B93-9B3C-F4506C823708" );

  if( compareClassGuid( v1_guid ) ) {
    xAOD::ParticleAuxContainer* trans_v1 = poolReadObject< xAOD::ParticleAuxContainer >();
    return trans_v1;
  }
  else throw std::runtime_error("Unsupported persistent version of xAOD::ParticleAuxContainer");

  return NULL;
}
