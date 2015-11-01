/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGTOPOEVENT
#define TRIGSTORAGEDEF_TRIGTOPOEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(class ElectronMuonTopoInfo, class ElectronMuonTopoInfo,class  ElectronMuonTopoInfoContainer)
  HLT_REGISTER_TYPE(class ElectronMuonTopoInfo, class  ElectronMuonTopoInfoContainer, class ElectronMuonTopoInfoContainer)
HLT_END_TYPE_REGISTRATION(TrigTopoEvent)

#endif
