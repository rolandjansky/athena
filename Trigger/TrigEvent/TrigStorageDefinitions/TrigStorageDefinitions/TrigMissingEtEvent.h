/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGMISSINGETEVENT
#define TRIGSTORAGEDEF_TRIGMISSINGETEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

//take this out once we have the xAODFwd package
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigMissingET/TrigMissingETAuxContainer.h"

HLT_BEGIN_TYPE_REGISTRATION
HLT_REGISTER_TYPE(class TrigMissingET, class TrigMissingET, class TrigMissingETContainer )
HLT_REGISTER_TYPE(class TrigPileUpInfo, class TrigPileUpInfo, class TrigPileUpInfoContainer )
HLT_REGISTER_TYPE(xAOD::TrigMissingET, xAOD::TrigMissingETContainer, xAOD::TrigMissingETContainer, xAOD::TrigMissingETAuxContainer)
HLT_REGISTER_TYPE(xAOD::TrigMissingET, xAOD::TrigMissingET, xAOD::TrigMissingETContainer, xAOD::TrigMissingETAuxContainer)
HLT_END_TYPE_REGISTRATION(TrigMissingEtEvent)


#endif
