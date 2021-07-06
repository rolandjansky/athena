/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterMomentContainerCnv_p1.h"
//#include "CaloTPCnv/CaloClusterMomentContainer_p1.h"
#include "CaloEvent/CaloClusterMomentStore.h"
#include "AthenaKernel/errorcheck.h"
     
CaloClusterMomentContainerCnv_p1::CaloClusterMomentContainerCnv_p1()
{
}

void CaloClusterMomentContainerCnv_p1::transToPers(const CaloClusterMomentStore* trans, 
						   CaloClusterMomentContainer_p1* pers) const
{
  CaloClusterMomentStore::moment_iterator it=trans->begin();
  CaloClusterMomentStore::moment_iterator it_e=trans->end();
  CaloClusterMomentContainer_p1::ClusterMoment_p mom;
  for(;it!=it_e;++it) {
    mom.key=it.getMomentType();
    mom.value=it.getMoment().getValue();
    pers->m_store.push_back(mom);
  }
}

void CaloClusterMomentContainerCnv_p1::persToTrans(const CaloClusterMomentContainer_p1* pers,
						   CaloClusterMomentStore* trans,
                                                   CaloClusterMomentContainer_p1::const_iterator& momentStoreIterator) const
{
  typedef CaloClusterMomentStore::moment_store moment_store;
  typedef moment_store::value_type value_type;
  moment_store transStore;

  for (unsigned short i=0;i<pers->m_nMoments;++i) {
    //trans->insert((CaloClusterMoment::MomentType)momentStoreIterator->key,
    //		  CaloClusterMoment(momentStoreIterator->value));
    //Insert with 'hint', should be faster
    transStore.insert(transStore.end(),    
		      value_type(momentStoreIterator->key,
 				 momentStoreIterator->value));
    ++momentStoreIterator;
  }
  trans->setMomentStore (std::move (transStore));
}

bool CaloClusterMomentContainerCnv_p1::setIterator(const CaloClusterMomentContainer_p1* pers,
                                                   unsigned int ncluster,
                                                   CaloClusterMomentContainer_p1::const_iterator& momentStoreIterator) const
{
  momentStoreIterator=pers->m_store.begin();

  if (pers->m_nMoments * ncluster != pers->m_store.size()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, "CaloClusterMomentContainerCnv_p1")
      << "Corrupted persistent data seen: "
      << ncluster << " clusters * "
      << pers->m_nMoments << " moments != "
      << pers->m_store.size() << " stored values.";
    return false;
  }

  return true;
}

  
