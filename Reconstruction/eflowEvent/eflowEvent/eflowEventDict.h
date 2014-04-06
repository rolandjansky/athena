/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EFLOWEVENTDICT_H
#define EFLOWEVENTDICT_H

#include "eflowEvent/eflowObject.h"
#include "eflowEvent/eflowObjectContainer.h"



namespace eflowEventDicty{
  struct tmp {
    std::vector<eflowObject*> m_eflowDummyVec;
    NavigableVector<Rec::TrackParticleContainer> m_dummy2;
    NavigableVector<CaloClusterContainer>        m_dummy3;
  };
}
#endif

