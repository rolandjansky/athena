/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_ROICNV_P1_H
#define TRIGMON_ROICNV_P1_H

#include "TrigMonitoringEvent/TrigMonRoi.h"
#include "TrigMonitoringEventTPCnv/TrigMonRoi_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TrigMonRoiCnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigMonRoi, TrigMonRoi_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigMonRoiCnv_p1() {}
  virtual ~TrigMonRoiCnv_p1() {}

  virtual
  void persToTrans(const TrigMonRoi_p1* persObj,
		   TrigMonRoi* transObj,
		   MsgStream &log) const override;
  
  virtual
  void transToPers(const TrigMonRoi* transObj,
		   TrigMonRoi_p1* persObj,
		   MsgStream &log) const override;
};

#endif

