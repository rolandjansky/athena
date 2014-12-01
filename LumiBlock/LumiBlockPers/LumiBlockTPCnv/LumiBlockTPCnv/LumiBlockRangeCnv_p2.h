/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKRANGECNV_P2_H
#define LUMIBLOCKRANGECNV_P2_H

#include "LumiBlockTPCnv/LumiBlockRange_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LumiBlockData/LB_IOVRange.h"

class MsgStream;
class LumiBlockRangeCnv_p2  : public T_AthenaPoolTPCnvBase<LB_IOVRange, LumiBlockRange_p2> {
 public:
  LumiBlockRangeCnv_p2() {}
  virtual void   persToTrans(const LumiBlockRange_p2* persObj, LB_IOVRange* transObj, MsgStream &log);
  virtual void   transToPers(const LB_IOVRange* transObj, LumiBlockRange_p2* persObj, MsgStream &log);
};

template<>
class T_TPCnv<LB_IOVRange, LumiBlockRange_p2>
  : public LumiBlockRangeCnv_p2
{
public:
};

#endif // LUMIBLOCKRANGECNV_P2_H
