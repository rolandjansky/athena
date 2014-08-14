/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigTrackCountsCollectionCnv
 *
 * @brief transient persistent converter for TrigTrackCountsCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTrackCountsCollectionCnv.h,v 1.4 2009-02-23 18:59:20 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGTRACKCOUNTSCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGTRACKCOUNTSCOLLECTION_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigInDetEvent/TrigTrackCountsCollection.h"
class TrigTrackCountsCollection_tlp1;
 
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigTrackCountsCollection_tlp1   TrigTrackCountsCollection_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigTrackCountsCollection, TrigTrackCountsCollection_PERS> TrigTrackCountsCollectionCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigTrackCountsCollection object
//-----------------------------------------------------------------------------
class TrigTrackCountsCollectionCnv_tlp1;
class TrigTrackCountsCollectionCnv : public TrigTrackCountsCollectionCnvBase
{
  
  friend class CnvFactory<TrigTrackCountsCollectionCnv>;
  
 protected:
  
  TrigTrackCountsCollectionCnv( ISvcLocator *svcloc );
  ~TrigTrackCountsCollectionCnv();
  
  virtual TrigTrackCountsCollection_PERS *createPersistent( TrigTrackCountsCollection *transObj);
  virtual TrigTrackCountsCollection      *createTransient();
  
 
 private:
  
  IMessageSvc                   *m_msgSvc;
  MsgStream                     m_log;
  
  TrigTrackCountsCollectionCnv_tlp1*   m_TPConverter;
  
};//end of class definitions


#endif //TRIGTRACKCOUNTSCOLLECTION_CNV_H
