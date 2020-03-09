/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEHITVECTORCNV_P1_H
#define TILEHITVECTORCNV_P1_H

#include "TileSimEventTPCnv/TileHitVector_p1.h"
#include "TileSimEvent/TileHitVector.h"
#include "TileSimEventTPCnv/TileHitCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class TileHitVectorCnv_p1 : public T_AtlasHitsVectorCnv< TileHitVector, TileHitVector_p1, TileHitCnv_p1 > {

 public:
  
  TileHitVectorCnv_p1()  {};
  virtual TileHitVector* createTransient(const TileHitVector_p1* persObj, MsgStream &log);
  
};

#endif
