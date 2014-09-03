/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloTPCnv/CaloTowerSegCnv_p1.h"
#include "CaloTPCnv/CaloTowerSeg_p1.h"

#define private public
#define protected public
#include "CaloEvent/CaloTowerSeg.h"
#undef private
#undef protected

void CaloTowerSegCnv_p1::persToTrans(const CaloTowerSeg_p1* pers, CaloTowerSeg* trans){
 trans->m_neta=pers->m_neta;    
 trans->m_nphi=pers->m_nphi; 
 trans->m_etamin=pers->m_etamin; 
 trans->m_etamax=pers->m_etamax;
 trans->m_phimin=pers->m_phimin;
 trans->m_phimax=pers->m_phimax;
 trans->m_deta=pers->m_deta;  
 trans->m_dphi=pers->m_dphi;

}

void CaloTowerSegCnv_p1::transToPers(const CaloTowerSeg* trans, CaloTowerSeg_p1* pers) {
 pers->m_neta=trans->m_neta;    
 pers->m_nphi=trans->m_nphi; 
 pers->m_etamin=trans->m_etamin; 
 pers->m_etamax=trans->m_etamax;
 pers->m_phimin=trans->m_phimin;
 pers->m_phimax=trans->m_phimax;
 pers->m_deta=trans->m_deta;  
 pers->m_dphi=trans->m_dphi;

}
