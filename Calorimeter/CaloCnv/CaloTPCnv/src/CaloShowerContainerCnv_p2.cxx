/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloShowerContainerCnv_p2.h"

#include "CaloTPCnv/CaloShowerContainer_p2.h"
#define private public
#include "CaloEvent/CaloShowerContainer.h"
#undef private
#include "AthenaKernel/errorcheck.h"

CaloShowerContainerCnv_p2::CaloShowerContainerCnv_p2()
{
}

void CaloShowerContainerCnv_p2::persToTrans(const CaloShowerContainer_p2* pers, CaloShowerContainer* trans,
					    MsgStream&)
{
  if (! m_momentContainerCnv.setIterator(&pers->m_momentContainer,
                                         pers->m_nClusters) ||
      ! m_samplingContainerCnv.setIterator(&pers->m_samplingDataContainer,
                                           pers->m_nClusters))
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
    m_momentContainerCnv.persToTrans(&(pers->m_momentContainer),&(shower->m_momentStore));
    m_samplingContainerCnv.persToTrans(&(pers->m_samplingDataContainer),&(shower->m_samplingStore));
    trans->push_back(shower);
  }
}

void CaloShowerContainerCnv_p2::transToPers(const CaloShowerContainer* trans, CaloShowerContainer_p2* pers, MsgStream& log) { 
					    
  pers->m_nClusters=trans->size();
  CaloShowerContainer::const_iterator it=trans->begin();
  CaloShowerContainer::const_iterator it_e=trans->end();
  if (it!=it_e) {
    //The var-type pattern and the sampling pattern are stored in CaloSamplingDataContainer_p1
    //(once for the entire container)
    //pers->m_samplingDataContainer.m_varTypePattern=(*it)->m_samplingStore.m_varTypePattern;
    //pers->m_samplingDataContainer.m_samplingPattern=(*it)->m_samplingStore.m_samplingPattern; *** TO BE SOLVED !!!!!
    pers->m_momentContainer.m_nMoments=(*it)->m_momentStore.size();
    
  }
  for(;it!=it_e;++it) {
    m_momentContainerCnv.transToPers(&((*it)->m_momentStore),&(pers->m_momentContainer));
    m_samplingContainerCnv.transToPers(&((*it)->m_samplingStore),&(pers->m_samplingDataContainer),log);
  }
}


/** Convert persistent object representation to transient
    @param pers [IN] void* pointer to the persistent object
    @param trans [OUT] void* pointer to the empty transient object
    @param log [IN] output message stream
*/
void CaloShowerContainerCnv_p2::persToTransUntyped(const void* pers,
                                                   void* trans,
                                                   MsgStream& log)
{
  persToTrans (reinterpret_cast<const CaloShowerContainer_p2*> (pers),
               reinterpret_cast<CaloShowerContainer*> (trans),
               log);
}


/** Convert transient object representation to persistent
    @param trans [IN] void* pointer to the transient object
    @param pers [OUT] void* pointer to the empty persistent object
    @param log [IN] output message stream
*/  
void CaloShowerContainerCnv_p2::transToPersUntyped(const void* trans,
                                                   void* pers,
                                                   MsgStream& log)
{
  transToPers (reinterpret_cast<const CaloShowerContainer*> (trans),
               reinterpret_cast<CaloShowerContainer_p2*> (pers),
               log);
}


/** return C++ type id of the transient class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloShowerContainerCnv_p2::transientTInfo() const
{
  return typeid (CaloShowerContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloShowerContainerCnv_p2::persistentTInfo() const
{
  return typeid (CaloShowerContainer_p2);
}

