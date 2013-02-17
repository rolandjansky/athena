/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombinedIDMuonObjectColl_h__
#define __CombinedIDMuonObjectColl_h__
 
#include "TrigCosmicEvent/CombinedIDMuonObject.h"
#include "SGTools/BaseInfo.h"
 
class CombinedIDMuonObjectCollection : public DataVector<CombinedIDMuonObject> {
};
 

CLASS_DEF( CombinedIDMuonObjectCollection , 1120216612 , 1 )

SG_BASE(CombinedIDMuonObjectCollection, DataVector<CombinedIDMuonObject>); 
 
#endif

