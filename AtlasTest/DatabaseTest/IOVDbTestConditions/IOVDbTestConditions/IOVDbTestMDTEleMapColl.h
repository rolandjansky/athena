/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBTESTCONDITIONS_IOVDBTESTMDTELEMAPCOLL_H
#define IOVDBTESTCONDITIONS_IOVDBTESTMDTELEMAPCOLL_H

#include "AthenaPoolUtilities/CondMultChanCollection.h"
#include "IOVDbTestConditions/IOVDbTestMDTEleMap.h"
#include "CLIDSvc/CLASS_DEF.h"

/**This typedef represents a collection of IOVDbTestMDTEleMap objects. 
It is a DataVector. It can be saved
to storegate and persistified using POOL*/

typedef CondMultChanCollection<IOVDbTestMDTEleMap> IOVDbTestMDTEleMapColl;

CLASS_DEF(IOVDbTestMDTEleMapColl, 155887251 , 1 )

#endif
