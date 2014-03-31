/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEHITVECTORCNV_P1_H
#define TILEHITVECTORCNV_P1_H

#define private public
#include "TileTPCnv/TileHitVector_p1.h"
#undef private 
#include "TileSimEvent/TileHitVector.h"
#include "TileTPCnv/TileHitCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class TileHitVectorCnv_p1 : public T_AtlasHitsVectorCnv< TileHitVector, TileHitVector_p1, TileHitCnv_p1 >
{
 public:
  
  TileHitVectorCnv_p1()  {};
  virtual TileHitVector* createTransient(const TileHitVector_p1* persObj, MsgStream &log);
  
};

#endif
