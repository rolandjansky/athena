/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterContainerCnv_p1.h" 

#define private public
#define protected public
#include "CaloEvent/CaloClusterContainer.h"
#undef private
#undef protected 
#include "CaloUtils/CaloClusterSignalState.h"

void CaloClusterContainerCnv_p1::persToTrans(const CaloClusterContainer_p1* pers, CaloClusterContainer* trans, MsgStream &log) {
  CaloClusterContainer_p1::const_iterator it=pers->m_vec.begin();
  CaloClusterContainer_p1::const_iterator it_e=pers->m_vec.end();
  for(;it!=it_e;it++) {
    CaloCluster* transCluster=new CaloCluster();
    persToTrans(&(*it),transCluster,log);
    trans->push_back(transCluster);
  }

  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&(trans->m_towerSeg));
}


void CaloClusterContainerCnv_p1::transToPers(const CaloClusterContainer* trans, CaloClusterContainer_p1* pers, MsgStream &log)  {
  pers->m_vec.resize(trans->size());
  CaloClusterContainer_p1::iterator itp=pers->m_vec.begin();

  CaloClusterContainer::const_iterator it=trans->begin();
  CaloClusterContainer::const_iterator it_e=trans->end();
  for(;it!=it_e;it++,itp++) {
    //CaloClusterChangeSignalState statechange (*it, P4SignalState::CALIBRATED);
     P4SignalState::State oldstate=(*it)->signalState();
    (*it)->setSignalState(P4SignalState::CALIBRATED);
    transToPers(*it,&(*itp),log);
    (*it)->setSignalState(oldstate); //Change back to org signal state
   }
   m_caloTowerSegCnv.transToPers(&(trans->m_towerSeg),&(pers->m_towerSeg));
}



void CaloClusterContainerCnv_p1::persToTrans(const CaloClusterContainer_p1::CaloCluster_p* pers, 
					     CaloCluster* trans, MsgStream& log) {
  trans->setDefaultSignalState (P4SignalState::CALIBRATED);
  trans->m_ownDataStore=pers->m_ownDataStore;
  trans->m_basicSignal=pers->m_basicSignal;
  trans->m_time=pers->m_time;
  trans->m_samplingPattern=pers->m_samplingPattern; 
  trans->m_barrel=pers->m_barrel;  
  trans->m_endcap=pers->m_endcap;
  trans->m_eta0=pers->m_eta0;
  trans->m_phi0=pers->m_phi0;   

  //Convert base class
  m_P4EEtaPhiMCnv.persToTrans(&pers->m_P4EEtaPhiM,(P4EEtaPhiM*)trans,log);
  m_caloSamplingDataCnv.persToTrans(&pers->m_dataStore,&trans->m_dataStore);
  m_caloMomentStoreCnv.persToTrans(&pers->m_momentStore,&trans->m_momentStore);
  m_showerElementLinkCnv.persToTrans(&pers->m_dataLink,&trans->m_dataLink,log);
  m_cellElementLinkCnv.persToTrans(&pers->m_cellLink,&trans->m_cellLink,log);

  trans->m_status.m_status=pers->m_caloRecoStatus;
  trans->setAthenaBarCode (IAthenaBarCode::UNDEFINEDBARCODE);
}




void CaloClusterContainerCnv_p1::transToPers(const CaloCluster* trans, 
					     CaloClusterContainer_p1::CaloCluster_p* pers, MsgStream& log) {
  pers->m_ownDataStore=trans->m_ownDataStore;
  pers->m_basicSignal=trans->m_basicSignal;
  pers->m_time=trans->m_time;
  pers->m_samplingPattern=trans->m_samplingPattern; 
  pers->m_barrel=trans->m_barrel;  
  pers->m_endcap=trans->m_endcap;
  pers->m_eta0=trans->m_eta0;
  pers->m_phi0=trans->m_phi0; 

  //Convert base class
  m_P4EEtaPhiMCnv.transToPers((P4EEtaPhiM*)trans,&pers->m_P4EEtaPhiM,log);
  m_caloSamplingDataCnv.transToPers(&trans->m_dataStore,&pers->m_dataStore);
  m_caloMomentStoreCnv.transToPers(&trans->m_momentStore,&pers->m_momentStore);
  m_showerElementLinkCnv.transToPers(&trans->m_dataLink,&pers->m_dataLink,log);
  m_cellElementLinkCnv.transToPers(&trans->m_cellLink,&pers->m_cellLink,log);

  pers->m_caloRecoStatus=trans->m_status.m_status;

}

