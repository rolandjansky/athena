/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGTRTHITCOUNTSCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGTRTHITCOUNTSCOLLECTION_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEvent/TrigTrtHitCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p2.h"

typedef TrigTrtHitCountsCollection_p2   TrigTrtHitCountsCollection_PERS;

typedef T_AthenaPoolCustomCnv<TrigTrtHitCountsCollection, TrigTrtHitCountsCollection_PERS> TrigTrtHitCountsCollectionCnvBase;
 
class TrigTrtHitCountsCollectionCnv : public TrigTrtHitCountsCollectionCnvBase {
  
  friend class CnvFactory<TrigTrtHitCountsCollectionCnv>;
 
  protected:
   
  TrigTrtHitCountsCollectionCnv( ISvcLocator *svcloc ): TrigTrtHitCountsCollectionCnvBase(svcloc) {}
   
   virtual TrigTrtHitCountsCollection_PERS *createPersistent( TrigTrtHitCountsCollection *transObj);
   virtual TrigTrtHitCountsCollection      *createTransient();
  
  };//end of class definitions
  
 
#endif //TRIGTRTHITCOUNTSCOLLECTION_CNV_H
