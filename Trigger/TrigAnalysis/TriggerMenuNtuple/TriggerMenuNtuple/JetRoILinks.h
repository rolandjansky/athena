/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUNTUPLE_JETROILINKS_H
#define TRIGGERMENUNTUPLE_JETROILINKS_H
/*
  JetRoILinks.h
*/
#include "TriggerMenuNtuple/FeatureIndex.h"

class JetRoILinks {
public:
  JetRoILinks();
  ~JetRoILinks();

protected:
  FeatureIndex m_trigJetIndex;

};

#endif // TRIGGERMENUNTUPLE_JETROILINKS_H
