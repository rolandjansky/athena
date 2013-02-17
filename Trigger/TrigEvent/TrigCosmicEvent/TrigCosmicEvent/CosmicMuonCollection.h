/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CosmicMuonColl_h__
#define __CosmicMuonColl_h__

#include "TrigCosmicEvent/CosmicMuon.h"
#include "SGTools/BaseInfo.h"

class CosmicMuonCollection : public DataVector<CosmicMuon> {
};

// typedef DataVector<CosmicMuon> CosmicMuonCollection;
CLASS_DEF( DataVector<CosmicMuon> , 165794208 , 1 )
CLASS_DEF( CosmicMuonCollection , 1323329367 , 13)

SG_BASE(CosmicMuonCollection, DataVector<CosmicMuon>);

#endif
