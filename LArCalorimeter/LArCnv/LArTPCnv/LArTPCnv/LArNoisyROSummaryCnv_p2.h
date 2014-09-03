/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTPCNV_LARNOISYROSUMMARYCNV_P2_H
#define LARTPCNV_LARNOISYROSUMMARYCNV_P2_H

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummary_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class MsgStream;

class LArNoisyROSummaryCnv_p2: public T_AthenaPoolTPCnvBase<LArNoisyROSummary,LArNoisyROSummary_p2>
{
 public:
  LArNoisyROSummaryCnv_p2() {};

  virtual void   persToTrans(const LArNoisyROSummary_p2* pers, LArNoisyROSummary* trans, MsgStream &log) ;
  virtual void   transToPers(const LArNoisyROSummary* trans, LArNoisyROSummary_p2* pers, MsgStream &log) ;
  
};


#endif
