/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Author: David Cote, September 2008. <david.cote@cern.ch>
#ifndef SKIMDECISIONCNV_P1_H
#define SKIMDECISIONCNV_P1_H

#include "EventBookkeeperMetaData/SkimDecision.h"
#include "EventBookkeeperTPCnv/SkimDecision_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class SkimDecisionCnv_p1  : public T_AthenaPoolTPCnvBase<SkimDecision, SkimDecision_p1> {
 public:
  SkimDecisionCnv_p1() {}
  virtual void   persToTrans(const SkimDecision_p1* persObj, SkimDecision* transObj, MsgStream &log);
  virtual void   transToPers(const SkimDecision* transObj, SkimDecision_p1* persObj, MsgStream &log);
};

template<>
class T_TPCnv<SkimDecision, SkimDecision_p1>
  : public SkimDecisionCnv_p1
{
public:
};

#endif // SKIMDECISIONCNV_P1_H
