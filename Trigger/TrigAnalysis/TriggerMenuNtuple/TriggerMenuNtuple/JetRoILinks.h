/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __JetRoILinks_h__
#define __JetRoILinks_h__
/*
  JetRoILinks.h
*/
#include "TriggerMenuNtuple/FeatureIndex.h"

class JetRoILinks {
public:
  JetRoILinks();
  ~JetRoILinks();

protected:
  FeatureIndex mTrigJetIndex;

};

#endif // __JetRoILinks_h__
