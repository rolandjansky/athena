/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGSPACEPOINTCOUNTSCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGSPACEPOINTCOUNTSCOLLECTION_CNV_H
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p4.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollectionCnv_p4.h"

typedef TrigSpacePointCountsCollection_p4   TrigSpacePointCountsCollection_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigSpacePointCountsCollection, TrigSpacePointCountsCollection_PERS> TrigSpacePointCountsCollectionCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigSpacePointCountsCollection object
//-----------------------------------------------------------------------------

class TrigSpacePointCountsCollectionCnv : public TrigSpacePointCountsCollectionCnvBase
{
  
  friend class CnvFactory<TrigSpacePointCountsCollectionCnv>;
  
 protected:
  
public:
  TrigSpacePointCountsCollectionCnv( ISvcLocator *svcloc ) : TrigSpacePointCountsCollectionCnvBase(svcloc) {}
protected:
  
  virtual TrigSpacePointCountsCollection_PERS *createPersistent( TrigSpacePointCountsCollection *transObj);
  virtual TrigSpacePointCountsCollection      *createTransient();

private:
  TrigSpacePointCountsCollectionCnv_tlp1 m_converter_tlp1;
  TrigSpacePointCountsCollectionCnv_p4   m_converter;

};//end of class definitions


#endif //TRIGSPACEPOINTCOUNTSCOLLECTION_CNV_H
