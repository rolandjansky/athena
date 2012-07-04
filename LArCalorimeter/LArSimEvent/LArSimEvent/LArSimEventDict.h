/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSIMEVENT_LARSIMEVENT_H
#define LARSIMEVENT_LARSIMEVENT_H

#include "LArSimEvent/LArHit.h"
#include "LArSimEvent/LArHitContainer.h"
#include "StoreGate/StoreGateSvc.h" 

namespace LArSimEvent
{
  struct tmp
  {
    PyGate<LArHitContainer>     m_ip;
    LArHitContainer::const_iterator   m_it_ip;
  };
}

#endif
