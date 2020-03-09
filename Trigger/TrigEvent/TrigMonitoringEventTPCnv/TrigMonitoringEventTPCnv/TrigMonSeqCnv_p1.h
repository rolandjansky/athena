/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMON_SEQCNV_P1_H
#define TRIGMON_SEQCNV_P1_H

// Framework
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// Transient data
#include "TrigMonitoringEvent/TrigMonSeq.h"

// Persistent data and converters
#include "TrigMonitoringEventTPCnv/TrigMonSeq_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonAlgCnv_p1.h"

class MsgStream;

class TrigMonSeqCnv_p1 : public T_AthenaPoolTPCnvConstBase<TrigMonSeq, TrigMonSeq_p1>
{
 public:
  using base_class::transToPers;
  using base_class::persToTrans;


  TrigMonSeqCnv_p1() {}
  virtual ~TrigMonSeqCnv_p1() {}

  virtual
  void persToTrans(const TrigMonSeq_p1* persObj,
		   TrigMonSeq* transObj,
		   MsgStream &log) const override;
  
  virtual
  void transToPers(const TrigMonSeq* transObj,
		   TrigMonSeq_p1* persObj,
		   MsgStream &log) const override;

 private:

  T_AthenaPoolTPCnvStdVectorConst<std::vector<TrigMonAlg>,
                                  std::vector<TrigMonAlg_p1>, TrigMonAlgCnv_p1> m_algCnv;
};

#endif

