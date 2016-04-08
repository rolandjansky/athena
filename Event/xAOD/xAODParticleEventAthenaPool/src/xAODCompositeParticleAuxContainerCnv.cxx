/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCompositeParticleAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODCompositeParticleAuxContainerCnv::xAODCompositeParticleAuxContainerCnv(ISvcLocator* svcLoc) : xAODCompositeParticleAuxContainerCnvBase(svcLoc) {}

xAOD::CompositeParticleAuxContainer* xAODCompositeParticleAuxContainerCnv::createPersistent(xAOD::CompositeParticleAuxContainer* trans){

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}


xAOD::CompositeParticleAuxContainer* xAODCompositeParticleAuxContainerCnv::createTransient(){

  static pool::Guid v1_guid( "E43BBAC2-214D-4AFC-927D-AA0EE2C9217C" );

  if( compareClassGuid( v1_guid ) ) {
    xAOD::CompositeParticleAuxContainer* trans_v1 = poolReadObject< xAOD::CompositeParticleAuxContainer >();
    return trans_v1;
  }
  else throw std::runtime_error("Unsupported persistent version of xAOD::CompositeParticleAuxContainer");

  return NULL;
}
