/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEHITCNV_P1_H
#define TILEHITCNV_P1_H

/*
Transient/Persistent converter for TileHit class
Author: Davide Costanzo
*/

#include "TileSimEvent/TileHit.h"
#include "TileSimEventTPCnv/TileHit_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;


class TileHitCnv_p1  : public T_AthenaPoolTPCnvConstBase<TileHit, TileHit_p1>
{
public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TileHitCnv_p1() {}

  virtual void          persToTrans(const TileHit_p1* persObj, TileHit* transObj, MsgStream &log) const override;
  virtual void          transToPers(const TileHit* transObj, TileHit_p1* persObj, MsgStream &log) const override;
};


#endif

