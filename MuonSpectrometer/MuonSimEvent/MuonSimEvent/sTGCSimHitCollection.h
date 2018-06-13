/*
  2018-05-29, Jing
*/

#ifndef sTGCSimHitCollection_H
#define sTGCSimHitCollection_H

#include "MuonSimEvent/sTGCSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<sTGCSimHit> OldsTGCSimHitCollection;

typedef AtlasHitsVector<sTGCSimHit> sTGCSimHitCollection;
typedef AtlasHitsVector<sTGCSimHit>::iterator sTGCSimHitIterator;
typedef AtlasHitsVector<sTGCSimHit>::const_iterator sTGCSimHitConstIterator;

#ifndef __CINT__
CLASS_DEF( sTGCSimHitCollection , 20180530 , 1 )
CLASS_DEF( OldsTGCSimHitCollection , 20180529 , 1 )
#endif

#endif

