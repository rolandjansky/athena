/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEHITVECTORCNV
#define TILEHITVECTORCNV

#include "TileSimEvent/TileHitVector.h"
#include "TileSimEventTPCnv/TileHitVector_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef TileHitVector_p1  TileHitVector_PERS;

class TileHitVectorCnv  : public T_AthenaPoolCustomCnv<TileHitVector, TileHitVector_PERS > {

  friend class CnvFactory<TileHitVectorCnv>;

 protected:
   TileHitVectorCnv(ISvcLocator* svcloc): T_AthenaPoolCustomCnv<TileHitVector, TileHitVector_PERS >( svcloc) {}

  TileHitVector_PERS*  createPersistent(TileHitVector* transCont);
  TileHitVector*       createTransient ();
};


#endif
