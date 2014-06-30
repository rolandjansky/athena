/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArFebErrorSummaryCnv_H
#define LArFebErrorSummaryCnv_H

#include "LArRawEvent/LArFebErrorSummary.h"
#include "LArTPCnv/LArFebErrorSummary_p1.h"
#include "LArTPCnv/LArFebErrorSummaryCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

typedef LArFebErrorSummary_p1 LArFebErrorSummary_PERSISTENT;

typedef T_AthenaPoolCustomCnv<LArFebErrorSummary,LArFebErrorSummary_PERSISTENT> LArFebErrorSummaryCnvBase;

class LArFebErrorSummaryCnv : public LArFebErrorSummaryCnvBase 
{
  friend class CnvFactory<LArFebErrorSummaryCnv>;
 protected:
  LArFebErrorSummaryCnv(ISvcLocator*);
  virtual LArFebErrorSummary* createTransient();
  virtual LArFebErrorSummary_PERSISTENT* createPersistent(LArFebErrorSummary*);
 private:

  LArFebErrorSummaryCnv_p1 m_converter;

};

#endif
