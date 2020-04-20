/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUEVENTTPCNV_TAUEVENTDUMMYCNV_PX_H
#define TAUEVENTTPCNV_TAUEVENTDUMMYCNV_PX_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class TauEventDummy {};

class TauEventDummyCnv_px : public T_AthenaPoolTPCnvBase<TauEventDummy, int>
{
public:
  virtual void persToTrans( const int*, TauEventDummy *, MsgStream & ) {}
  virtual void transToPers( const TauEventDummy*,  int*, MsgStream & ) {}
};
#endif
