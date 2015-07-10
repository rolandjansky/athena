/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArNoisyROSummaryCnv_h
#define LArNoisyROSummaryCnv_h

#include "LArRecEvent/LArNoisyROSummary.h"
#include "LArTPCnv/LArNoisyROSummary_p3.h"
#include "LArTPCnv/LArNoisyROSummaryCnv_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"


typedef LArNoisyROSummary_p3 LArNoisyROSummary_PERSISTENT;
typedef T_AthenaPoolCustomCnv<LArNoisyROSummary,LArNoisyROSummary_PERSISTENT> LArNoisyROSummaryCnvBase;
 
class LArNoisyROSummaryCnv : public LArNoisyROSummaryCnvBase 
{
  friend class CnvFactory<LArNoisyROSummaryCnv>;
 protected:
  LArNoisyROSummaryCnv(ISvcLocator*);
  virtual LArNoisyROSummary* createTransient();
  virtual LArNoisyROSummary_PERSISTENT* createPersistent(LArNoisyROSummary*);
  private:
 
  LArNoisyROSummaryCnv_p3 m_converter;
 
};
 

#endif
