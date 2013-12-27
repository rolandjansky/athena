/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKRANGECNV_P1_H
#define LUMIBLOCKRANGECNV_P1_H

#include "LumiBlockTPCnv/LumiBlockRange_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AthenaKernel/IOVRange.h"

class MsgStream;
class LumiBlockRangeCnv_p1  : public T_AthenaPoolTPCnvBase<IOVRange, LumiBlockRange_p1> {
 public:
  LumiBlockRangeCnv_p1() {}
  virtual void   persToTrans(const LumiBlockRange_p1* persObj, IOVRange* transObj, MsgStream &log);
  virtual void   transToPers(const IOVRange* transObj, LumiBlockRange_p1* persObj, MsgStream &log);
};

template<>
class T_TPCnv<IOVRange, LumiBlockRange_p1>
  : public LumiBlockRangeCnv_p1
{
public:
};

#endif // LUMIBLOCKRANGECNV_P1_H
