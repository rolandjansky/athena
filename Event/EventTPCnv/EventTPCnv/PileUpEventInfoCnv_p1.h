/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P1_H
#define EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P1_H

#include "EventInfo/PileUpEventInfo.h"
#include "EventTPCnv/PileUpEventInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PileUpEventInfoCnv_p1  : public T_AthenaPoolTPCnvBase<PileUpEventInfo, PileUpEventInfo_p1>  {
public:
  PileUpEventInfoCnv_p1() {}
  virtual void   persToTrans(const PileUpEventInfo_p1* persObj, PileUpEventInfo* transObj, MsgStream &log) ;
  virtual void   transToPers(const PileUpEventInfo* transObj, PileUpEventInfo_p1* persObj, MsgStream &log) ;
};

template<>
class T_TPCnv<PileUpEventInfo, PileUpEventInfo_p1>
  : public PileUpEventInfoCnv_p1
{
public:
};

#endif
