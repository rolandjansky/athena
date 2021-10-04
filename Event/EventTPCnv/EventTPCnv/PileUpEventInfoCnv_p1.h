/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P1_H
#define EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P1_H

#include "EventInfo/PileUpEventInfo.h"
#include "EventTPCnv/PileUpEventInfo_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PileUpEventInfoCnv_p1  : public T_AthenaPoolTPCnvConstBase<PileUpEventInfo, PileUpEventInfo_p1>  {
public:
  PileUpEventInfoCnv_p1() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const PileUpEventInfo_p1* persObj, PileUpEventInfo* transObj, MsgStream &log) const override;
  virtual void   transToPers(const PileUpEventInfo* transObj, PileUpEventInfo_p1* persObj, MsgStream &log) const override;
};

template<>
class T_TPCnv<PileUpEventInfo, PileUpEventInfo_p1>
  : public PileUpEventInfoCnv_p1
{
public:
};

#endif
