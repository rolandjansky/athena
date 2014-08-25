/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STORAGEDEF_TRIGCOMBINEDEVENT
#define STORAGEDEF_TRIGCOMBINEDEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(struct TrigComposite, struct TrigComposite, struct TrigCompositeContainer ) 
  HLT_REGISTER_TYPE(struct TrigComposite, struct TrigCompositeContainer, struct TrigCompositeContainer ) 
HLT_END_TYPE_REGISTRATION(TrigCombinedEvent) 

#endif
