/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//  Header file for SkimDecisionCollectionCnv
//  Author: David Cote, September 2008. <david.cote@cern.ch>
//  This class uses TP separation
///////////////////////////////////////////////////////////////////

#ifndef SKIMDECISIONCOLLECTIONCNV__H
#define SKIMDECISIONCOLLECTIONCNV__H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "EventBookkeeperTPCnv/SkimDecisionCollection_p1.h"
#include "EventBookkeeperMetaData/SkimDecisionCollection.h"
#include "EventBookkeeperTPCnv/SkimDecisionCollectionCnv_p1.h"

// the latest persistent representation type of SkimDecisionCollection
typedef  SkimDecisionCollection_p1  SkimDecisionCollection_PERS;
typedef  T_AthenaPoolCustomCnv<SkimDecisionCollection, SkimDecisionCollection_PERS >  SkimDecisionCollectionCnvBase;

class SkimDecisionCollectionCnv : public SkimDecisionCollectionCnvBase 
{
  friend class CnvFactory<SkimDecisionCollectionCnv >;
 protected:
  SkimDecisionCollectionCnv (ISvcLocator* svcloc) : SkimDecisionCollectionCnvBase(svcloc) {}
  virtual SkimDecisionCollection_PERS*  createPersistent (SkimDecisionCollection* transCont);
  virtual SkimDecisionCollection*     createTransient ();

 private:
  SkimDecisionCollectionCnv_p1   m_TPConverter;
};

#endif  // SKIMDECISIONCOLLECTIONCNV__H

