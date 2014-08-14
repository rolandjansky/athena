/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigVertexCollectionCnv
 *
 * @brief transient persistent converter for TrigVertexCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigVertexCollectionCnv.h,v 1.2 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGVERTEXCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGVERTEXCOLLECTION_CNV_H
 
 
class MsgStream;
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
class TrigVertexCollection_tlp2;
class TrigVertexCollectionCnv_tlp2;
 
//-----------------------------------------------------------------------------
// typedefs
//-----------------------------------------------------------------------------
typedef TrigVertexCollection_tlp2   TrigVertexCollection_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigVertexCollection, TrigVertexCollection_PERS> TrigVertexCollectionCnvBase;

class MsgStream;

//-----------------------------------------------------------------------------
// Converter for TrigVertexCollection object
//-----------------------------------------------------------------------------

class TrigVertexCollectionCnv : public TrigVertexCollectionCnvBase
 {
  
  friend class CnvFactory<TrigVertexCollectionCnv>;
 
  protected:
   
  TrigVertexCollectionCnv( ISvcLocator *svcloc );
  ~TrigVertexCollectionCnv();

   virtual TrigVertexCollection_PERS *createPersistent( TrigVertexCollection *transObj);
   virtual TrigVertexCollection      *createTransient();
 
  private:
 
   IMessageSvc                   *m_msgSvc;
   MsgStream                     m_log;
   
   TrigVertexCollectionCnv_tlp2*  m_TPConverter;
 
  };//end of class definitions
  
 
 #endif //TRIGVERTEXCOLLECTION_CNV_H
