/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARNOISYROSUMMARYCNV_P3_H
#define LARTPCNV_LARNOISYROSUMMARYCNV_P3_H

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummary_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class MsgStream;

class LArNoisyROSummaryCnv_p3: public T_AthenaPoolTPCnvBase<LArNoisyROSummary,LArNoisyROSummary_p3>
{
 public:
  LArNoisyROSummaryCnv_p3() {};

  virtual void   persToTrans(const LArNoisyROSummary_p3* pers, LArNoisyROSummary* trans, MsgStream &log) ;
  virtual void   transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p3* pers, MsgStream &log) ;
  
};


#endif
