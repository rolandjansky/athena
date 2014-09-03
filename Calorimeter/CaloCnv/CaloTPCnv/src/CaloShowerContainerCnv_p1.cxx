/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloShowerContainerCnv_p1.h"

#include "CaloTPCnv/CaloShowerContainer_p1.h"
#define private public
#include "CaloEvent/CaloShowerContainer.h"
#undef private

CaloShowerContainerCnv_p1::CaloShowerContainerCnv_p1()
{
}

void CaloShowerContainerCnv_p1::persToTrans(const CaloShowerContainer_p1* pers,
                                            CaloShowerContainer* trans,
                                            MsgStream& /*log*/)
{
  const std::vector<CaloShowerContainer_p1::CaloShower>& persCont=pers->m_container;
  trans->resize(persCont.size());
  CaloShowerContainer_p1::const_iterator it=persCont.begin();
  CaloShowerContainer_p1::const_iterator it_e=persCont.end();
  CaloShowerContainer::size_type i=0;
  for (;it!=it_e;++it,++i) {
    CaloShower* shower=new CaloShower();
    trans->at(i)=shower;
    m_momentStoreCnv.persToTrans(&it->m_momentStore,&(shower->m_momentStore));
    m_samplingDataCnv.persToTrans(&it->m_samplingStore,&(shower->m_samplingStore));
  }
}

void CaloShowerContainerCnv_p1::transToPers(const CaloShowerContainer* trans,
                                            CaloShowerContainer_p1* pers,
                                            MsgStream& /*log*/)
{
  std::vector<CaloShowerContainer_p1::CaloShower>& persCont=pers->m_container;
  persCont.resize(trans->size());
  CaloShowerContainer::const_iterator it=trans->begin();
  CaloShowerContainer::const_iterator it_e=trans->end();
  std::vector<CaloShowerContainer_p1::CaloShower>::size_type i=0;
  for(;it!=it_e;++it,++i) {
    m_momentStoreCnv.transToPers(&((*it)->m_momentStore),&(persCont[i].m_momentStore));
    m_samplingDataCnv.transToPers(&((*it)->m_samplingStore),&(persCont[i].m_samplingStore));
  }
}
