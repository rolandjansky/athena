/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for SkimDecisionCnv
//  Author: David Cote, September 2008. <david.cote@cern.ch>
//  This class uses TP separation
///////////////////////////////////////////////////////////////////

#ifndef SKIMDECISIONCNV__H
#define SKIMDECISIONCNV__H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "EventBookkeeperTPCnv/SkimDecision_p1.h"
#include "EventBookkeeperMetaData/SkimDecision.h"
#include "EventBookkeeperTPCnv/SkimDecisionCnv_p1.h"

// the latest persistent representation type of SkimDecision
typedef  SkimDecision_p1  SkimDecision_PERS;
typedef  T_AthenaPoolCustomCnv<SkimDecision, SkimDecision_PERS >  SkimDecisionCnvBase;

class SkimDecisionCnv : public SkimDecisionCnvBase 
{
  friend class CnvFactory<SkimDecisionCnv >;
 protected:
  SkimDecisionCnv (ISvcLocator* svcloc) : SkimDecisionCnvBase(svcloc) {}
  virtual SkimDecision_PERS*  createPersistent (SkimDecision* transCont);
  virtual SkimDecision*     createTransient ();

 private:
  SkimDecisionCnv_p1   m_TPConverter;
};

#endif  // SKIMDECISIONCNV__H

