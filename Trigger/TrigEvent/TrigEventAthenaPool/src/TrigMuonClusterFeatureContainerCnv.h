/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigMuonClusterFeatureContainerCnv
 *
 * @brief transient persistent converter for TrigMuonClusterFatureContainer
 *
 * @author Stefano Giagu  <Stefano.Giagu@cern.ch>  - U. of Rome
 * @author Antonio Policicchio <Antonio.Policicchio@cern.ch> - U. of Washington
 *
 * File and Version Information:
 * $Id: TrigMuonClusterFeatureContainerCnv.h,v 1.2 2009-02-23 18:59:19 ssnyder Exp $
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGMUONCLUSTERFEATURECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGMUONCLUSTERFEATURECONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
 
#include "TrigMuonEvent/TrigMuonClusterFeatureContainer.h"
class TrigMuonClusterFeatureContainer_tlp1;
 
 
//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef TrigMuonClusterFeatureContainer_tlp1   TrigMuonClusterFeatureContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigMuonClusterFeatureContainer, TrigMuonClusterFeatureContainer_PERS> TrigMuonClusterFeatureContainerCnvBase;
 
//-----------------------------------------------------------------------------
// Converter for TrigMuonClusterFeatureCollection object
//-----------------------------------------------------------------------------
class TrigMuonClusterFeatureContainerCnv_tlp1;
class TrigMuonClusterFeatureContainerCnv : public TrigMuonClusterFeatureContainerCnvBase
{
  
  friend class CnvFactory<TrigMuonClusterFeatureContainerCnv>;
  
 protected:
  
  TrigMuonClusterFeatureContainerCnv( ISvcLocator *svcloc );
  ~TrigMuonClusterFeatureContainerCnv();
  
  virtual TrigMuonClusterFeatureContainer_PERS *createPersistent( TrigMuonClusterFeatureContainer *transObj);
  virtual TrigMuonClusterFeatureContainer      *createTransient();
  
 
 private:
  
  IMessageSvc                   *m_msgSvc;
  MsgStream                     m_log;
  
  TrigMuonClusterFeatureContainerCnv_tlp1*   m_TPConverter;
  
};//end of class definitions


#endif //TRIGMUONCLUSTERFEATURECONTAINER_CNV_H
