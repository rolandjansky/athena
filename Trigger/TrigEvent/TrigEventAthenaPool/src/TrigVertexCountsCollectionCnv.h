/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigVertexCountsCollectionCnv
 *
 * @brief transient persistent converter for TrigVertexCountsCollection
 *
 * @author W. H. Bell  <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGVERTEXCOUNTSCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGVERTEXCOUNTSCOLLECTION_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEvent/TrigVertexCountsCollection.h"
class TrigVertexCountsCollection_tlp1;
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigVertexCountsCollection_tlp1   TrigVertexCountsCollection_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigVertexCountsCollection, TrigVertexCountsCollection_PERS> TrigVertexCountsCollectionCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigVertexCountsCollection object
//-----------------------------------------------------------------------------
class TrigVertexCountsCollectionCnv_tlp1;
class TrigVertexCountsCollectionCnv : public TrigVertexCountsCollectionCnvBase
{
  
  friend class CnvFactory<TrigVertexCountsCollectionCnv>;
 
  protected:
   
  TrigVertexCountsCollectionCnv( ISvcLocator *svcloc );
  ~TrigVertexCountsCollectionCnv();
   
   virtual TrigVertexCountsCollection_PERS *createPersistent( TrigVertexCountsCollection *transObj);
   virtual TrigVertexCountsCollection      *createTransient();
  
 private:
   
   TrigVertexCountsCollectionCnv_tlp1* m_TPConverter;

 };//end of class definitions
  
 
#endif //TRIGVERTEXCOUNTSCOLLECTION_CNV_H
