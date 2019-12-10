/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloShowerContainerCnv_p2.h"

#include "CaloTPCnv/CaloShowerContainer_p2.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "AthenaKernel/errorcheck.h"

void CaloShowerContainerCnv_p2::persToTrans (const CaloShowerContainer_p2* pers,
                                             CaloShowerContainer* trans,
                                             MsgStream&) const
{
  CaloClusterMomentContainer_p1::const_iterator momentStoreIterator;
  CaloSamplingDataContainerCnv_p1::State samplingState;
  if (! m_momentContainerCnv.setIterator(&pers->m_momentContainer,
                                         pers->m_nClusters,
                                         momentStoreIterator) ||
      ! m_samplingContainerCnv.setState(&pers->m_samplingDataContainer,
                                        pers->m_nClusters,
                                        samplingState))
  {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, "CaloShowerContainerCnv_p2")
      << "Not converting CaloShowerContainer.";
    return;
  }
  
  trans->clear();
  trans->reserve(pers->m_nClusters);
  for (unsigned i=0;i<pers->m_nClusters;++i) {
    CaloShower* shower=new CaloShower();
    //trans->at(i)=shower;
    m_momentContainerCnv.persToTrans(&(pers->m_momentContainer),&(shower->getMomentStore()),
                                     momentStoreIterator);
    m_samplingContainerCnv.persToTrans(&(pers->m_samplingDataContainer),&(shower->getSamplingStore()),
                                       samplingState);
    trans->push_back(shower);
  }
}


void CaloShowerContainerCnv_p2::transToPers (const CaloShowerContainer* trans,
                                             CaloShowerContainer_p2* pers,
                                             MsgStream& log) const
{ 
  pers->m_nClusters=trans->size();
  CaloShowerContainer::const_iterator it=trans->begin();
  CaloShowerContainer::const_iterator it_e=trans->end();
  if (it!=it_e) {
    //The var-type pattern and the sampling pattern are stored in CaloSamplingDataContainer_p1
    //(once for the entire container)
    //pers->m_samplingDataContainer.m_varTypePattern=(*it)->m_samplingStore.m_varTypePattern;
    //pers->m_samplingDataContainer.m_samplingPattern=(*it)->m_samplingStore.m_samplingPattern; *** TO BE SOLVED !!!!!
    pers->m_momentContainer.m_nMoments=(*it)->getMomentStore().size();
    
  }
  for(;it!=it_e;++it) {
    m_momentContainerCnv.transToPers(&((*it)->getMomentStore()),&(pers->m_momentContainer));
    m_samplingContainerCnv.transToPers(&((*it)->getSamplingStore()),&(pers->m_samplingDataContainer),log);
  }
}
