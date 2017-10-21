/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterContainerCnv_p3.h" 

#include "AthAllocators/DataPool.h"
    
#include "AthenaPoolCnvSvc/Compressor.h"
#include "AthenaKernel/errorcheck.h"

void CaloClusterContainerCnv_p3::persToTrans(const CaloClusterContainer_p3* pers, 
					     CaloClusterContainer* trans, MsgStream &log) {
  if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG << "Reading CaloClusterContainerCNV_p3" << endmsg;
					     
  // reset element link converters, and provide container name lookup table
  m_showerElementLinkCnv.resetForCnv(pers->m_linkNames);
  m_cellElementLinkCnv.resetForCnv(pers->m_linkNames);

  // Use data pool for clusters to avoid calling constructor for each event
  DataPool<CaloCluster> clusters;

  trans->clear (SG::VIEW_ELEMENTS);
  trans->reserve(pers->m_vec.size());
	  
  if ( ! m_samplingDataContainerCnv.setIterator(&pers->m_samplingDataContainer,
                                                pers->m_vec.size()))
  {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, "CaloClusterContainerCnv_p3")
      << "Not converting CaloClusterContainer.";
    return;
  }

  CaloClusterContainer_p3::const_iterator itp=pers->m_vec.begin();
  CaloClusterContainer_p3::const_iterator itp_e=pers->m_vec.end();
  
   std::vector<float> temp_Moments;
   Compressor A;
   A.expandToFloat( pers->m_momentContainer.m_Mvalue, temp_Moments); // unpacks moments
   
   std::vector<float>::const_iterator i_mom = temp_Moments.begin();
   
   const std::vector<unsigned short>& keys = pers->m_momentContainer.m_Mkey;
   unsigned int nkeys = keys.size();
  
  for(;itp!=itp_e;++itp) {
    // Get next ptr for next cluster
    CaloCluster* transCluster = clusters.nextElementPtr();
    //Convert Cluster-quantities
    persToTrans(&(*itp),transCluster,log); 
    //Convert sampling data store
    m_samplingDataContainerCnv.persToTrans(&(pers->m_samplingDataContainer),&(transCluster->m_dataStore));
    
    //Convert moment store
    CaloClusterMomentStore::moment_store transStore;
    for (unsigned short i=0;i<nkeys;++i) {
           transStore.insert (transStore.end(),  CaloClusterMomentStore::moment_store::value_type( keys[i], (*i_mom) ) );
	    //std::cout<<"imom: "<<(*i_key)<<"   "<<(*i_mom)<<"\t";
	    ++i_mom;
	    }
    transCluster->m_momentStore.setMomentStore (std::move (transStore));
    //std::cout<<std::endl; 
    trans->push_back(transCluster);
  }
  //Convert TowerSegment
  CaloTowerSeg seg;
  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&seg);
  trans->setTowerSeg (seg);
}



void CaloClusterContainerCnv_p3::transToPers(const CaloClusterContainer* trans, 
					     CaloClusterContainer_p3* pers, MsgStream &log) {
  if (log.level() <= MSG::DEBUG) log<< MSG::DEBUG << "Writing CaloClusterContainerCNV_p3" << endmsg;

  // reset element link converters, and provide container name lookup table
  m_showerElementLinkCnv.resetForCnv(pers->m_linkNames);
  m_cellElementLinkCnv.resetForCnv(pers->m_linkNames);

  pers->m_vec.resize(trans->size()); 
  CaloClusterContainer_p3::iterator itp=pers->m_vec.begin();
  CaloClusterContainer::const_iterator it=trans->begin(); 
  CaloClusterContainer::const_iterator it_e=trans->end();  
 
  
  // moment keys are done here - once per container - we store keys and number of them
  pers->m_momentContainer.m_Mkey.clear();
  if (it!=it_e) {    
    const CaloClusterMomentStore* ms=&((*it)->m_momentStore);
    CaloClusterMomentStore::moment_iterator mit=((*it)->m_momentStore).begin();
    int nMom=ms->size();
    pers->m_momentContainer.m_nMoments = nMom;
    for (int w=0; w<nMom; w++){  // here stores moment keys per container not cluster
	   // std::cout<<"key: "<<mit.getMomentType()<<"\t";
	    pers->m_momentContainer.m_Mkey.push_back(mit.getMomentType());  
	    mit++; 
	    }
	   // std::cout<<std::endl;
  } 
  else
    pers->m_momentContainer.m_nMoments = 0;

  std::vector<float> temp_Moments;
  
  for(;it!=it_e;++it,++itp) { 
    //CaloClusterChangeSignalState statechange (*it, P4SignalState::CALIBRATED);
    P4SignalState::State oldstate=(*it)->signalState();
    (*it)->setSignalState(P4SignalState::CALIBRATED);
    transToPers(*it,&(*itp),log);
    m_samplingDataContainerCnv.transToPers(&((*it)->m_dataStore),&(pers->m_samplingDataContainer),log);
    
    CaloClusterMomentStore::moment_iterator itm=(*it)->m_momentStore.begin(); 
    CaloClusterMomentStore::moment_iterator itm_e=(*it)->m_momentStore.end();
  
    for(;itm!=itm_e;itm++) {
	temp_Moments.push_back(itm.getMoment().getValue());
	//std::cout<<"mom: "<<itm.getMoment().getValue()<<"\t";
	} 
    	//std::cout<<std::endl;
    (*it)->setSignalState(oldstate);
  }
   
  Compressor A;
  A.setNrBits(16);
  // A.setIgnoreSign();
  A.reduce(temp_Moments,pers->m_momentContainer.m_Mvalue); // packs moments
   
  m_caloTowerSegCnv.transToPers(&trans->getTowerSeg(),&(pers->m_towerSeg));

} 



void CaloClusterContainerCnv_p3::persToTrans(const CaloClusterContainer_p3::CaloCluster_p* pers, 
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




void CaloClusterContainerCnv_p3::transToPers(const CaloCluster* trans, 
					     CaloClusterContainer_p3::CaloCluster_p* pers, MsgStream& log) {

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


void CaloClusterContainerCnv_p3::persToTransUntyped(const void* pers,
                                                    void* trans,
                                                    MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloClusterContainer_p3*>(pers),
               reinterpret_cast<CaloClusterContainer*>(trans),
               log);
}


void CaloClusterContainerCnv_p3::transToPersUntyped(const void* trans,
                                                    void* pers,
                                                    MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloClusterContainer*>(trans),
               reinterpret_cast<CaloClusterContainer_p3*>(pers),
               log);
}


const std::type_info& CaloClusterContainerCnv_p3::transientTInfo() const
{
  return typeid (CaloClusterContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloClusterContainerCnv_p3::persistentTInfo() const
{
  return typeid (CaloClusterContainer_p3);
}


