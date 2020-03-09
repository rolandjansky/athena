/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloTowerContainerCnv_p1.h"

#include "CaloTPCnv/CaloTowerContainer_p1.h"
#include "CaloEvent/CaloTowerContainer.h"


void CaloTowerContainerCnv_p1::persToTrans (const CaloTowerContainer_p1* pers,
                                            CaloTowerContainer* trans,
                                            MsgStream& /*log*/) const
{
  m_caloTowerSegCnv.persToTrans(&(pers->m_towerSeg),&(trans->m_towerSeg));
  trans->m_caloRegions.resize(pers->m_caloRegions.size());
  std::copy(pers->m_caloRegions.begin(),pers->m_caloRegions.end(),trans->m_caloRegions.begin());
}


void CaloTowerContainerCnv_p1::transToPers (const CaloTowerContainer* trans,
                                            CaloTowerContainer_p1* pers,
                                            MsgStream& /*log*/) const
{
  m_caloTowerSegCnv.transToPers(&(trans->m_towerSeg),&(pers->m_towerSeg));
  pers->m_caloRegions.resize(trans->m_caloRegions.size());
  std::copy(trans->m_caloRegions.begin(),trans->m_caloRegions.end(),pers->m_caloRegions.begin());
}
