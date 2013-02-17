/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CombinedIDHalfTracksObjectColl_h__
#define __CombinedIDHalfTracksObjectColl_h__
 
#include "TrigCosmicEvent/CombinedIDHalfTracksObject.h"
#include "SGTools/BaseInfo.h"
 
class CombinedIDHalfTracksObjectCollection : public DataVector<CombinedIDHalfTracksObject> {
};
 
// typedef DataVector<CombinedIDHalfTracksObject> CombinedIDHalfTracksObjectCollection;
//CLASS_DEF( DataVector<CombinedIDHalfTracksObject> , 43166145 , 1 )
CLASS_DEF( CombinedIDHalfTracksObjectCollection ,1333450068, 1)
//1333450068 CombinedIDHalfTracksObjectCollection ( 796579156 259708244 4f7ad554 )
 
SG_BASE(CombinedIDHalfTracksObjectCollection, DataVector<CombinedIDHalfTracksObject>); 
#endif


