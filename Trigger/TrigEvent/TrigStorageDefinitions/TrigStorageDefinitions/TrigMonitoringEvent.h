/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGEDEF_TRIGMONITORINGEVENT
#define STORAGEDEF_TRIGMONITORINGEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(struct TrigMonConfig, struct TrigMonConfig, struct TrigMonConfigCollection)
  HLT_REGISTER_TYPE(struct TrigMonEvent, struct TrigMonEvent,struct TrigMonEventCollection)
HLT_END_TYPE_REGISTRATION(TrigMonitoringEvent)

#endif
