/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloClusterMomentStoreCnv_p1.h" 

#include "CaloEvent/CaloClusterMomentStore.h"
#include "CaloTPCnv/CaloClusterMomentStore_p1.h"


CaloClusterMomentStoreCnv_p1::CaloClusterMomentStoreCnv_p1()
{
}


// CaloClusterMomentStore* CaloClusterMomentStoreCnv_p1::createTrans(const CaloClusterMomentStore_p1* pers /*,MsgStream&*/) {
//   CaloClusterMomentStore* trans=new CaloClusterMomentStore();
//   persToTrans(pers,trans);
//   return trans;
// }

void CaloClusterMomentStoreCnv_p1::persToTrans(const CaloClusterMomentStore_p1* pers, CaloClusterMomentStore* trans /*,MsgStream&*/)  {
  std::vector<CaloClusterMomentStore_p1::ClusterMoment>::const_iterator it=pers->m_store.begin();
  std::vector<CaloClusterMomentStore_p1::ClusterMoment>::const_iterator it_e=pers->m_store.end();
  for (;it!=it_e;it++) 
    trans->insert((CaloClusterMoment::MomentType)it->key,CaloClusterMoment(it->value));
}



// CaloClusterMomentStore_p1* CaloClusterMomentStoreCnv_p1::createPers(const CaloClusterMomentStore* trans /*,MsgStream& */) {
//   CaloClusterMomentStore_p1* pers=new CaloClusterMomentStore_p1();
//   transToPers(trans,pers);
//   return pers;
// }


void CaloClusterMomentStoreCnv_p1::transToPers(const CaloClusterMomentStore* trans, CaloClusterMomentStore_p1* pers/*,MsgStream&*/) {
  CaloClusterMomentStore::moment_iterator it=trans->begin();
  CaloClusterMomentStore::moment_iterator it_e=trans->end();

  CaloClusterMomentStore_p1::ClusterMoment mom;
  for(;it!=it_e;it++) {
    mom.key=it.getMomentType();
    mom.value=it.getMoment().getValue();
    pers->m_store.push_back(mom);
  }
}
