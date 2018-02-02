/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigEMCluster.h"

#include "TrigCaloEventTPCnv/TrigEMCluster_p2.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p2.h"
#include "AthLinks/ElementLink.h"
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"

//static ElementLinkCnv_p1< ElementLink<RingerRingsContainer> > ELinkRingerRingsCnv;

void TrigEMClusterCnv_p2::transToPers(const TrigEMCluster* trans, 
                                     TrigEMCluster_p2* pers, MsgStream &log )
{

  log << MSG::DEBUG << "TrigEMClusterCnv_p2::tranToPers" << endmsg;

  if ( (!trans) || (!pers) ) {
    log << MSG::ERROR << "TrigEMClusterCnv_p2::trasnToPers with trans or pers == 0" << endmsg;
    return;
  }

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

  m_ELinkRingerRingsCnv.transToPers(&trans->m_rings, &pers->m_rings, log);
  pers->m_trigCaloCluster = baseToPersistent( &m_trigCaloClusterCnv, trans, log );
}

void TrigEMClusterCnv_p2::persToTrans(const TrigEMCluster_p2* pers, 
                                     TrigEMCluster* trans, MsgStream &log )
{

  log << MSG::DEBUG << "TrigEMClusterCnv_p2::persToTrans" << endmsg;

  if ( (!trans) || (!pers) ) {
    log << MSG::ERROR << "RingerRingsCnv_p2::persToTrans with trans or pers == 0" << endmsg;
    return;
  }
  
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

  m_ELinkRingerRingsCnv.persToTrans(&pers->m_rings, &trans->m_rings, log);
  fillTransFromPStore( &m_trigCaloClusterCnv, pers->m_trigCaloCluster, trans, log );

}
