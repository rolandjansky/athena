/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGSPACEPOINTCOUNTSCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGSPACEPOINTCOUNTSCOLLECTION_CNV_H
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p4.h"

typedef TrigSpacePointCountsCollection_p4   TrigSpacePointCountsCollection_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigSpacePointCountsCollection, TrigSpacePointCountsCollection_PERS> TrigSpacePointCountsCollectionCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigSpacePointCountsCollection object
//-----------------------------------------------------------------------------

class TrigSpacePointCountsCollectionCnv : public TrigSpacePointCountsCollectionCnvBase
{
  
  friend class CnvFactory<TrigSpacePointCountsCollectionCnv>;
  
 protected:
  
  TrigSpacePointCountsCollectionCnv( ISvcLocator *svcloc ) : TrigSpacePointCountsCollectionCnvBase(svcloc) {}
  
  virtual TrigSpacePointCountsCollection_PERS *createPersistent( TrigSpacePointCountsCollection *transObj);
  virtual TrigSpacePointCountsCollection      *createTransient();
  
};//end of class definitions


#endif //TRIGSPACEPOINTCOUNTSCOLLECTION_CNV_H
