/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigEMCluster.h"

#include "TrigCaloEventTPCnv/TrigEMCluster_p1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h"


void TrigEMClusterCnv_p1::transToPers(const TrigEMCluster*, 
                                     TrigEMCluster_p1*, MsgStream &log )
{

  log << MSG::DEBUG << "TrigEMClusterCnv_p1::tranToPers" << endreq;
  log << MSG::WARNING << "TrigEMClusterCnv_p1::tranToPers : Trying to persist obsolete TrigEMClusterCnv_p1! I won't write anything!" << endreq;
/*
  pers->m_Energy     = trans->m_Energy;
  pers->m_Et         = trans->m_Et;
  for(int i=0; i<MAXSIZE; i++){ 
    pers->m_EnergyS[i]    = trans->m_EnergyS[i];
  }
  pers->m_Eta        = trans->m_Eta;
  pers->m_Phi        = trans->m_Phi;
  pers->m_e237       = trans->m_e237;
  pers->m_e277       = trans->m_e277;
  pers->m_fracs1     = trans->m_fracs1;
  pers->m_weta2      = trans->m_weta2;
  pers->m_ehad1      = trans->m_ehad1;
  pers->m_Eta1       = trans->m_Eta1;
  pers->m_emaxs1     = trans->m_emaxs1;
  pers->m_e2tsts1    = trans->m_e2tsts1;

  pers->m_trigCaloCluster = baseToPersistent( &m_trigCaloClusterCnv, trans, log );
*/
}

void TrigEMClusterCnv_p1::persToTrans(const TrigEMCluster_p1* pers, 
                                     TrigEMCluster* trans, MsgStream &log )
{

  log << MSG::DEBUG << "TrigEMClusterCnv_p1::persToTrans" << endreq;

  trans->m_Energy     = pers->m_Energy;
  trans->m_Et         = pers->m_Et;
  for(int i=0; i<MAXSIZE; i++){   
    trans->m_EnergyS[i]    = pers->m_EnergyS[i];
  }
  trans->m_Eta        = pers->m_Eta;
  trans->m_Phi        = pers->m_Phi;
  trans->m_e237       = pers->m_e237;
  trans->m_e277       = pers->m_e277;
  trans->m_fracs1     = pers->m_fracs1;
  trans->m_weta2      = pers->m_weta2;
  trans->m_ehad1      = pers->m_ehad1;
  trans->m_Eta1       = pers->m_Eta1;
  trans->m_emaxs1     = pers->m_emaxs1;
  trans->m_e2tsts1    = pers->m_e2tsts1;

  fillTransFromPStore( &m_trigCaloClusterCnv, pers->m_trigCaloCluster, trans, log );



}
