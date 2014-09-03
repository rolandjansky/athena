/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloTowerContainerCnv_p1.h"

#include "CaloTPCnv/CaloTowerContainer_p1.h"
#define private public
#include "CaloEvent/CaloTowerContainer.h"
#undef private


void CaloTowerContainerCnv_p1::persToTrans(const CaloTowerContainer_p1* pers, CaloTowerContainer* trans) {
  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&(trans->m_towerSeg));
  trans->m_caloRegions.resize(pers->m_caloRegions.size());
  std::copy(pers->m_caloRegions.begin(),pers->m_caloRegions.end(),trans->m_caloRegions.begin());
}


void CaloTowerContainerCnv_p1::transToPers(const CaloTowerContainer* trans, CaloTowerContainer_p1* pers)  {
  m_caloTowerSegCnv.transToPers(&(trans->m_towerSeg),&(pers->m_towerSeg));
  pers->m_caloRegions.resize(trans->m_caloRegions.size());
  std::copy(trans->m_caloRegions.begin(),trans->m_caloRegions.end(),pers->m_caloRegions.begin());
}


/** Convert persistent object representation to transient
    @param pers [IN] void* pointer to the persistent object
    @param trans [OUT] void* pointer to the empty transient object
    @param log [IN] output message stream
*/
void CaloTowerContainerCnv_p1::persToTransUntyped(const void* pers,
                                                  void* trans,
                                                  MsgStream& /*log*/)
{
  persToTrans (reinterpret_cast<const CaloTowerContainer_p1*> (pers),
               reinterpret_cast<CaloTowerContainer*> (trans));
}


/** Convert transient object representation to persistent
    @param trans [IN] void* pointer to the transient object
    @param pers [OUT] void* pointer to the empty persistent object
    @param log [IN] output message stream
*/  
void CaloTowerContainerCnv_p1::transToPersUntyped(const void* trans,
                                                  void* pers,
                                                  MsgStream& /*log*/)
{
  transToPers (reinterpret_cast<const CaloTowerContainer*> (trans),
               reinterpret_cast<CaloTowerContainer_p1*> (pers));
}


/** return C++ type id of the transient class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloTowerContainerCnv_p1::transientTInfo() const
{
  return typeid (CaloTowerContainer);
}

/** return C++ type id of the persistent class this converter is for
    @return std::type_info&
*/
const std::type_info& CaloTowerContainerCnv_p1::persistentTInfo() const
{
  return typeid (CaloTowerContainer_p1);
}

