/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODCutBookkeeperAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODCutBookkeeperAuxContainerCnv::xAODCutBookkeeperAuxContainerCnv(ISvcLocator* svcLoc) : xAODCutBookkeeperAuxContainerCnvBase(svcLoc) {}

xAOD::CutBookkeeperAuxContainer* xAODCutBookkeeperAuxContainerCnv::createPersistent(xAOD::CutBookkeeperAuxContainer* trans){

  // Create a copy of the container:
  return SG::copyThinned (*trans, IThinningSvc::instance());
}


xAOD::CutBookkeeperAuxContainer* xAODCutBookkeeperAuxContainerCnv::createTransient(){

  static pool::Guid v1_guid( "AF612BAA-20B8-40A3-A418-894A9FB8A61B" );

  if( compareClassGuid( v1_guid ) ) {
    xAOD::CutBookkeeperAuxContainer* trans_v1 = poolReadObject< xAOD::CutBookkeeperAuxContainer >();
    return trans_v1;
  }
  else throw std::runtime_error("Unsupported persistent version of xAOD::CutBookkeeperAuxContainer");

  return NULL;
}
