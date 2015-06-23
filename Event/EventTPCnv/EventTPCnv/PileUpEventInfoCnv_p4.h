/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P4_H
#define EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P4_H

#include "EventTPCnv/PileUpEventInfo_p4.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PileUpEventInfoCnv_p4  : public T_AthenaPoolTPCnvBase<PileUpEventInfo, PileUpEventInfo_p4>  {
public:
  PileUpEventInfoCnv_p4() {}
  virtual void   persToTrans(const PileUpEventInfo_p4* persObj, PileUpEventInfo* transObj, MsgStream &log) ;
  virtual void   transToPers(const PileUpEventInfo* transObj, PileUpEventInfo_p4* persObj, MsgStream &log) ;
};

template<>
class T_TPCnv<PileUpEventInfo, PileUpEventInfo_p4>
  : public PileUpEventInfoCnv_p4
{
public:
};

#endif
