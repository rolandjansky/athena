/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P5_H
#define EVENTATHENAPOOL_PILEUPEVENTINFOCNV_P5_H

#include "EventTPCnv/PileUpEventInfo_p5.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class PileUpEventInfoCnv_p5  : public T_AthenaPoolTPCnvBase<PileUpEventInfo, PileUpEventInfo_p5>  {
public:
  PileUpEventInfoCnv_p5() {}
  virtual void   persToTrans(const PileUpEventInfo_p5* persObj, PileUpEventInfo* transObj, MsgStream &log) ;
  virtual void   transToPers(const PileUpEventInfo* transObj, PileUpEventInfo_p5* persObj, MsgStream &log) ;
};

template<>
class T_TPCnv<PileUpEventInfo, PileUpEventInfo_p5>
  : public PileUpEventInfoCnv_p5
{
public:
};

#endif
