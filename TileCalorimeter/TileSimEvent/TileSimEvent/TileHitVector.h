/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Collection definition for TileHit 
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
// October 17th, 2003.
// 20/02/04 - Name changed to TileHitVector
//
//************************************************************
 
#ifndef TileHitVector_H
#define TileHitVector_H

#include <iostream>
#include <vector>
#include "TileSimEvent/TileHit.h"
#include "TileSimEvent/TileSimHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "HitManagement/AthenaHitsVector.h"

#ifndef __CINT__
  #include "CLIDSvc/CLASS_DEF.h"
#endif

typedef AthenaHitsVector<TileHit> TileOldHitVector;

typedef AtlasHitsVector<TileHit> TileHitVector;
typedef AtlasHitsVector<TileHit>::iterator TileHitVecIterator;
typedef AtlasHitsVector<TileHit>::const_iterator TileHitVecConstIterator;

#ifndef __CINT__
  CLASS_DEF(TileHitVector, 2960, 1 ) 
  CLASS_DEF(TileOldHitVector, 2961, 1)
#endif

#endif 
