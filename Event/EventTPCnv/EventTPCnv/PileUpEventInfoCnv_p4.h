/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P4_H
#define EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P4_H

#include "EventTPCnv/PileUpEventInfo_p4.h"
#include "EventInfo/PileUpEventInfo.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PileUpEventInfoCnv_p4  : public T_AthenaPoolTPCnvConstBase<PileUpEventInfo, PileUpEventInfo_p4>  {
public:
  PileUpEventInfoCnv_p4() {}
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const PileUpEventInfo_p4* persObj, PileUpEventInfo* transObj, MsgStream &log) const override;
  virtual void   transToPers(const PileUpEventInfo* transObj, PileUpEventInfo_p4* persObj, MsgStream &log) const override;
};

template<>
class T_TPCnv<PileUpEventInfo, PileUpEventInfo_p4>
  : public PileUpEventInfoCnv_p4
{
public:
};

#endif
