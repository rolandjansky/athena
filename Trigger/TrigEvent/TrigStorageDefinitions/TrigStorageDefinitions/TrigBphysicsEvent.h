/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGBPHYSICSEVENT
#define TRIGSTORAGEDEF_TRIGBPHYSICSEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"
#include "Navigation/NavigationDefaults.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"


HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(xAOD::TrigBphys, xAOD::TrigBphysContainer, xAOD::TrigBphysContainer, xAOD::TrigBphysAuxContainer)

HLT_END_TYPE_REGISTRATION(TrigBphysicsEvent)


#endif
