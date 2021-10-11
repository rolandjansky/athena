/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARNOISYROSUMMARYCNV_P4_H
#define LARTPCNV_LARNOISYROSUMMARYCNV_P4_H

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummary_p4.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class MsgStream;

class LArNoisyROSummaryCnv_p4: public T_AthenaPoolTPCnvConstBase<LArNoisyROSummary,LArNoisyROSummary_p4>
{
 public:
  LArNoisyROSummaryCnv_p4() {};
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const LArNoisyROSummary_p4* pers, LArNoisyROSummary* trans, MsgStream &log) const override;
  virtual void   transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p4* pers, MsgStream &log) const override;
  
};


#endif
