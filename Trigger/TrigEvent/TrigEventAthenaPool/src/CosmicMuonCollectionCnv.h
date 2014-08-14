/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : CosmicMuonCollectionCnv
 *
 * @brief transient persistent converter for CosmicMuonCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: CosmicMuonCollectionCnv.h,v 1.2 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_COSMICMUONCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_COSMICMUONCOLLECTION_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigCosmicEvent/CosmicMuonCollection.h"
class  CosmicMuonCollectionCnv_tlp1;
class  CosmicMuonCollection_tlp1;
 
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef CosmicMuonCollection_tlp1   CosmicMuonCollection_PERS;
 
typedef T_AthenaPoolCustomCnv<CosmicMuonCollection, CosmicMuonCollection_PERS> CosmicMuonCollectionCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for CosmicMuonCollection object
//-----------------------------------------------------------------------------
class CosmicMuonCollectionCnv : public CosmicMuonCollectionCnvBase
{
  
  friend class CnvFactory<CosmicMuonCollectionCnv>;
  
 protected:
  
  CosmicMuonCollectionCnv( ISvcLocator *svcloc );
  ~CosmicMuonCollectionCnv();
  
  virtual CosmicMuonCollection_PERS *createPersistent( CosmicMuonCollection *transObj);
  virtual CosmicMuonCollection      *createTransient();
  
 
 private:
  
  IMessageSvc                   *m_msgSvc;
  MsgStream                     m_log;
  
  CosmicMuonCollectionCnv_tlp1*   m_TPConverter;
  
};//end of class definitions


#endif //COSMICMUONCOLLECTION_CNV_H
