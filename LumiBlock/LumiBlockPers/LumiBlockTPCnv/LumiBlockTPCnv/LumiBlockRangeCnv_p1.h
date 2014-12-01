/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKRANGECNV_P1_H
#define LUMIBLOCKRANGECNV_P1_H

#include "LumiBlockTPCnv/LumiBlockRange_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LumiBlockData/LB_IOVRange.h"

class MsgStream;
class LumiBlockRangeCnv_p1  : public T_AthenaPoolTPCnvBase<LB_IOVRange, LumiBlockRange_p1> {
 public:
  LumiBlockRangeCnv_p1() {}
  virtual void   persToTrans(const LumiBlockRange_p1* persObj, LB_IOVRange* transObj, MsgStream &log);
  virtual void   transToPers(const LB_IOVRange* transObj, LumiBlockRange_p1* persObj, MsgStream &log);
};

template<>
class T_TPCnv<LB_IOVRange, LumiBlockRange_p1>
  : public LumiBlockRangeCnv_p1
{
public:
};

#endif // LUMIBLOCKRANGECNV_P1_H
