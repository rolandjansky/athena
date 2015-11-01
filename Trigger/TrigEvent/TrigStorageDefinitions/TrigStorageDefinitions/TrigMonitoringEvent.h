/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGEDEF_TRIGMONITORINGEVENT
#define STORAGEDEF_TRIGMONITORINGEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(class TrigMonConfig, class TrigMonConfig, class TrigMonConfigCollection)
  HLT_REGISTER_TYPE(class TrigMonEvent, class TrigMonEvent,class TrigMonEventCollection)
HLT_END_TYPE_REGISTRATION(TrigMonitoringEvent)

#endif
