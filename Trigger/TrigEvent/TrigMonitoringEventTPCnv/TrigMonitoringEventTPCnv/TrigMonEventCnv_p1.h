/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_EVENTCNV_P1_H
#define TRIGMON_EVENTCNV_P1_H

// Framework
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Transient data
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Persistent data and converters
#include "TrigMonitoringEventTPCnv/TrigMonEvent_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonROBCnv_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonRoiCnv_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonSeqCnv_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonTECnv_p1.h"

class MsgStream;

class TrigMonEventCnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigMonEvent, TrigMonEvent_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigMonEventCnv_p1() {}
  virtual ~TrigMonEventCnv_p1() {}

  virtual
  void persToTrans(const TrigMonEvent_p1* persObj,
		   TrigMonEvent* transObj,
		   MsgStream &log) const override;
  
  virtual
  void transToPers(const TrigMonEvent* transObj,
		   TrigMonEvent_p1* persObj,
		   MsgStream &log) const override;

 private:

  T_AthenaPoolTPCnvStdVectorConst<std::vector<TrigMonROB>,
                                  std::vector<TrigMonROB_p1>, TrigMonROBCnv_p1> m_robCnv;
  T_AthenaPoolTPCnvStdVectorConst<std::vector<TrigMonRoi>,
                                  std::vector<TrigMonRoi_p1>, TrigMonRoiCnv_p1> m_roiCnv;
  T_AthenaPoolTPCnvStdVectorConst<std::vector<TrigMonSeq>,
                                  std::vector<TrigMonSeq_p1>, TrigMonSeqCnv_p1> m_seqCnv;
  T_AthenaPoolTPCnvStdVectorConst<std::vector<TrigMonTE>,
                                  std::vector<TrigMonTE_p1>, TrigMonTECnv_p1>   m_teCnv;
};

#endif

