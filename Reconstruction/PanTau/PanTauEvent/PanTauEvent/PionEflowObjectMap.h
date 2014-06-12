/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUEVENT_PIONEFLOWOBJECTMAP_H
#define PANTAUEVENT_PIONEFLOWOBJECTMAP_H

#include <map>
//#include "TauTrackEvent/TruthTau.h"
//#include "eflowEvent/eflowObject.h"

class eflowObject;
class TruthParticle;

namespace PanTau {
  typedef std::multimap< const eflowObject*, const TruthParticle* > PionEflowObjectMap;
}


#endif //PANTAUEVENT_PIONEFLOWOBJECTMAP_H
