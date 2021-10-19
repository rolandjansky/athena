/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARNOISYROSUMMARYCNV_P5_H
#define LARTPCNV_LARNOISYROSUMMARYCNV_P5_H

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummary_p5.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class MsgStream;

class LArNoisyROSummaryCnv_p5: public T_AthenaPoolTPCnvConstBase<LArNoisyROSummary,LArNoisyROSummary_p5>
{
 public:
  LArNoisyROSummaryCnv_p5() {};
  using base_class::persToTrans;
  using base_class::transToPers;

  virtual void   persToTrans(const LArNoisyROSummary_p5* pers, LArNoisyROSummary* trans, MsgStream &log) const override;
  virtual void   transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p5* pers, MsgStream &log) const override;
  
};


#endif
