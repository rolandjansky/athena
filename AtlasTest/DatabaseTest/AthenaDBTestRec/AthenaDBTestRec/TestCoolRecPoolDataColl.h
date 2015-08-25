/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENADBTESTREC_TESTCOOLRECPOOLDATACOLL_H
#define ATHENADBTESTREC_TESTCOOLRECPOOLDATACOLL_H
// collection of TestCoolRecPoolData objects for storing in a 
// COOL multichannel folder
// Richard Hawkings, started 16/8/06
#include "AthenaPoolUtilities/CondMultChanCollection.h"
#include "AthenaDBTestRec/TestCoolRecPoolData.h"
#include "CLIDSvc/CLASS_DEF.h"

typedef CondMultChanCollection<TestCoolRecPoolData> TestCoolRecPoolDataColl;

CLASS_DEF( TestCoolRecPoolDataColl , 117237436 , 1 )

#endif //  ATHENADBTESTREC_TESTCOOLRECPOOLDATACOLL_H
