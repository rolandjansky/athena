/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : TrigMuonEFInfoContainerCnv
 *
 * @brief transient persistent converter for TrigMuonEFInfoContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <f.bucci@cern.ch>          - U. Geneva
 * @author Sergio Grancagnolo  <Sergio.Grancagnolo@le.infn.it> - U.Salento/INFN Le
 *
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_TRIGMUONEFINFOCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGMUONEFINFOCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h" 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

class TrigMuonEFInfoContainer_tlp2;
class TrigMuonEFInfoContainerCnv_tlp1;
class TrigMuonEFInfoContainerCnv_tlp2;
 
typedef TrigMuonEFInfoContainer_tlp2   TrigMuonEFInfoContainer_PERS;
typedef T_AthenaPoolCustomCnv<TrigMuonEFInfoContainer, TrigMuonEFInfoContainer_PERS> TrigMuonEFInfoContainerCnvBase;
 
 
class TrigMuonEFInfoContainerCnv : public TrigMuonEFInfoContainerCnvBase{
  
  friend class CnvFactory<TrigMuonEFInfoContainerCnv>;
  
 protected:
  
  TrigMuonEFInfoContainerCnv( ISvcLocator *svcloc );
  ~TrigMuonEFInfoContainerCnv();
  
  virtual TrigMuonEFInfoContainer_PERS *createPersistent( TrigMuonEFInfoContainer *transObj);
  virtual TrigMuonEFInfoContainer      *createTransient();
  
 
 private:
  
  IMessageSvc                   *m_msgSvc;
  MsgStream                     m_log;
  
  TrigMuonEFInfoContainerCnv_tlp1*   m_TPConverter1;
  TrigMuonEFInfoContainerCnv_tlp2*   m_TPConverter2;
  
};


#endif 