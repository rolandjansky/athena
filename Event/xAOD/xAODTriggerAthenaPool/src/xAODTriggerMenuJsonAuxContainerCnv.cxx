/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODTriggerMenuJsonAuxContainerCnv.h"

xAODTriggerMenuJsonAuxContainerCnv::
xAODTriggerMenuJsonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTriggerMenuJsonAuxContainerCnvBase( svcLoc ) {
}

xAOD::TriggerMenuJsonAuxContainer*
xAODTriggerMenuJsonAuxContainerCnv::createTransientWithKey (const std::string& key) {

  std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer_v1> obj
    { xAODTriggerMenuJsonAuxContainerCnvBase::createTransientWithKey (key) };

  return obj.release();
}
