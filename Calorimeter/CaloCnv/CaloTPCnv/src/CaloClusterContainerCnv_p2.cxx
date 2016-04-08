/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterContainerCnv_p2.h" 
#include "CaloUtils/CaloClusterSignalState.h"

#include "DataModel/DataPool.h"
#include "AthenaKernel/errorcheck.h"

// void CaloClusterContainerCnv_p2::persToTrans(const CaloClusterContainer_p2* pers, 
// 					     CaloClusterContainer* trans,
// 					     unsigned long long ievt) 
// {
    
//     // Check whether event number has increased, if so we need to reset data pools
//     static unsigned long long lastEvent = 0;
//     if (lastEvent != ievt) {
// 	m_resetDataPool = true;
// 	lastEvent = ievt;
//     }
//     MsgStream log(0, "CaloClusterContainer_p2" );
//     persToTrans(pers, trans, log);
// }

    

void CaloClusterContainerCnv_p2::persToTrans(const CaloClusterContainer_p2* pers, 
					     CaloClusterContainer* trans, MsgStream &log) {
  // reset element link converters, and provide container name lookup table
  m_showerElementLinkCnv.resetForCnv(pers->m_linkNames);
  m_cellElementLinkCnv.resetForCnv(pers->m_linkNames);

  // Use data pool for clusters to avoid calling constructor for each event
  DataPool<CaloCluster> clusters;

  trans->clear (SG::VIEW_ELEMENTS);
  trans->reserve(pers->m_vec.size());
  if ( ! m_momentContainerCnv.setIterator(&pers->m_momentContainer,
                                          pers->m_vec.size()) ||
       ! m_samplingDataContainerCnv.setIterator(&pers->m_samplingDataContainer,
                                                pers->m_vec.size()))
  {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, "CaloClusterContainerCnv_p2")
      << "Not converting CaloClusterContainer.";
    return;
  }

  CaloClusterContainer_p2::const_iterator itp=pers->m_vec.begin();
  CaloClusterContainer_p2::const_iterator itp_e=pers->m_vec.end();
  for(;itp!=itp_e;++itp) {

    // Get next ptr for next cluster
    CaloCluster* transCluster = clusters.nextElementPtr();

    //The var-type pattern is stored in CaloSamplingDataContainer_p1 (once for the entire cluster container)
    //transCluster->m_dataStore.m_varTypePattern=pers->m_samplingDataContainer.m_varTypePattern;
    //... not any more... 

    //Convert Cluster-quantities
    persToTrans(&(*itp),transCluster,log);
    //Convert sampling data store
    m_samplingDataContainerCnv.persToTrans(&(pers->m_samplingDataContainer),&(transCluster->m_dataStore));
    //Convert moment store
    m_momentContainerCnv.persToTrans(&(pers->m_momentContainer),&(transCluster->m_momentStore));
    trans->push_back(transCluster);
  }
  //Convert TowerSegment
  CaloTowerSeg seg;
  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&seg);
  trans->setTowerSeg (seg);
}


void CaloClusterContainerCnv_p2::transToPers(const CaloClusterContainer* trans, 
					     CaloClusterContainer_p2* pers, MsgStream &log) {

  // reset element link converters, and provide container name lookup table
  m_showerElementLinkCnv.resetForCnv(pers->m_linkNames);
  m_cellElementLinkCnv.resetForCnv(pers->m_linkNames);

  pers->m_vec.resize(trans->size());
  CaloClusterContainer_p2::iterator itp=pers->m_vec.begin();
  CaloClusterContainer::const_iterator it=trans->begin();
  CaloClusterContainer::const_iterator it_e=trans->end();
  if (it!=it_e) {
    //The var-type pattern is stored in CaloSamplingDataContainer_p1 (once for the entire container)
    //pers->m_samplingDataContainer.m_varTypePattern=(*it)->m_dataStore.m_varTypePattern;
    //... not any more...
    
    //The number of moments is also the same for the entire container
    pers->m_momentContainer.m_nMoments=(*it)->m_momentStore.size();
  }
  for(;it!=it_e;++it,++itp) {
    //CaloClusterChangeSignalState statechange (*it, P4SignalState::CALIBRATED);
    P4SignalState::State oldstate=(*it)->signalState();
    (*it)->setSignalState(P4SignalState::CALIBRATED);
    transToPers(*it,&(*itp),log);
    m_samplingDataContainerCnv.transToPers(&((*it)->m_dataStore),&(pers->m_samplingDataContainer),log);
    m_momentContainerCnv.transToPers(&((*it)->m_momentStore),&(pers->m_momentContainer));
    (*it)->setSignalState(oldstate); //Change back to org signal state
  }
  m_caloTowerSegCnv.transToPers(&trans->getTowerSeg(),&(pers->m_towerSeg));
}



void CaloClusterContainerCnv_p2::persToTrans(const CaloClusterContainer_p2::CaloCluster_p* pers, 
					     CaloCluster* trans, MsgStream& log) {
  trans->setDefaultSignalState (P4SignalState::CALIBRATED);
  trans->setBasicEnergy (pers->m_basicSignal);
  trans->setTime (pers->m_time);
  trans->m_samplingPattern=pers->m_samplingPattern; 
  trans->m_barrel=pers->m_barrel;  
  trans->m_endcap=pers->m_endcap;
  trans->m_eta0=pers->m_eta0;
  trans->m_phi0=pers->m_phi0;   
  trans->m_status = CaloRecoStatus(pers->m_caloRecoStatus);
  trans->setClusterSize (pers->m_clusterSize);

  //trans->m_barrel=pers->m_samplingPattern & 0x1ff00f; //That's the OR of all barrel-bits
  //trans->m_endcap=pers->m_samplingPattern & 0xe00ff0; //That's the OR of all endcap-bits

  //Convert base class and element links
  m_P4EEtaPhiMCnv.persToTrans(&pers->m_P4EEtaPhiM,(P4EEtaPhiM*)trans,log);
  m_showerElementLinkCnv.persToTrans(&pers->m_dataLink,&trans->m_dataLink,log);
  m_cellElementLinkCnv.persToTrans(&pers->m_cellLink,&trans->m_cellLink,log);
  trans->setAthenaBarCode (IAthenaBarCode::UNDEFINEDBARCODE);
 
}




void CaloClusterContainerCnv_p2::transToPers(const CaloCluster* trans, 
					     CaloClusterContainer_p2::CaloCluster_p* pers, MsgStream& log) {

  pers->m_basicSignal=trans->getBasicEnergy();
  pers->m_time=trans->getTime();
  pers->m_samplingPattern=trans->m_samplingPattern; 
  pers->m_barrel=trans->m_barrel;  
  pers->m_endcap=trans->m_endcap;
  pers->m_eta0=trans->eta0();
  pers->m_phi0=trans->phi0(); 
  pers->m_caloRecoStatus=trans->m_status.getStatusWord();
  pers->m_clusterSize=trans->getClusterSize();
 
  //Convert base class and element links
  m_P4EEtaPhiMCnv.transToPers((P4EEtaPhiM*)trans,&pers->m_P4EEtaPhiM,log);
  m_showerElementLinkCnv.transToPers(&trans->m_dataLink,&pers->m_dataLink,log);
  m_cellElementLinkCnv.transToPers(&trans->m_cellLink,&pers->m_cellLink,log);
}


void CaloClusterContainerCnv_p2::persToTransUntyped(const void* pers,
                                                    void* trans,
                                                    MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloClusterContainer_p2*>(pers),
               reinterpret_cast<CaloClusterContainer*>(trans),
               log);
}


void CaloClusterContainerCnv_p2::transToPersUntyped(const void* trans,
                                                    void* pers,
                                                    MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloClusterContainer*>(trans),
               reinterpret_cast<CaloClusterContainer_p2*>(pers),
               log);
}


const std::type_info& CaloClusterContainerCnv_p2::transientTInfo() const
{
  return typeid (CaloClusterContainer);
}

  /** return C++ type id of the persistent class this converter is for
      @return std::type_info&
  */
const std::type_info& CaloClusterContainerCnv_p2::persistentTInfo() const
{
  return typeid (CaloClusterContainer_p2);
}


