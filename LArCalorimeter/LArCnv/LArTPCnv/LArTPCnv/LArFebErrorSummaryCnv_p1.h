/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTPCNV_LARFEBERRORSUMMARYCNV_P1_H
#define LARTPCNV_LARFEBERRORSUMMARYCNV_P1_H

#include "LArRawEvent/LArFebErrorSummary.h"
#include "LArTPCnv/LArFebErrorSummary_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// LArFebErrorSummaryCnv_p1, used for T/P separation
// author G.Unal

class MsgStream;
class LArFebErrorSummaryCnv_p1  : public T_AthenaPoolTPCnvBase<LArFebErrorSummary, LArFebErrorSummary_p1>
{
 public:
  LArFebErrorSummaryCnv_p1() { };
  virtual void   persToTrans(const LArFebErrorSummary_p1* pers, LArFebErrorSummary* trans, MsgStream &log) ;
  virtual void   transToPers(const LArFebErrorSummary* trans, LArFebErrorSummary_p1* pers, MsgStream &log) ;

};


#endif
