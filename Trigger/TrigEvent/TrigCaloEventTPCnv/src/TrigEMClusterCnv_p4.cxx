/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigCaloEventTPCnv/TrigEMCluster_p4.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p4.h"
#include "AthLinks/ElementLink.h"
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"

static ElementLinkCnv_p3< ElementLink<RingerRingsContainer> > ELinkRingerRingsCnv;
static TrigCaloClusterCnv_p2 trigCaloClusterCnv;

void TrigEMClusterCnv_p4::transToPers(const TrigEMCluster* trans, 
                                     TrigEMCluster_p4* pers, MsgStream &log )
{

  log << MSG::DEBUG << "TrigEMClusterCnv_p4::tranToPers" << endmsg;

  if ( (!trans) || (!pers) ) {
    log << MSG::ERROR << "TrigEMClusterCnv_p4::trasnToPers with trans or pers == 0" << endmsg;
    return;
  }

  pers->m_allFloats[0]     = trans->energy();
  pers->m_allFloats[1]     = trans->et();
  pers->m_allFloats[2]     = trans->eta();
  pers->m_allFloats[3]     = trans->phi();
  pers->m_allFloats[4]     = trans->e237();
  pers->m_allFloats[5]     = trans->e277();
  pers->m_allFloats[6]     = trans->fracs1();
  pers->m_allFloats[7]     = trans->weta2();
  pers->m_allFloats[8]     = trans->ehad1();
  pers->m_allFloats[9]     = trans->Eta1();
  pers->m_allFloats[10]    = trans->emaxs1();
  pers->m_allFloats[11]    = trans->e2tsts1();
  pers->m_allFloats[12]     = trans->e233();
  pers->m_allFloats[13]     = trans->wstot();
  for(int i=0; i<MAXSIZE; i++){ 
    pers->m_allFloats[14+i]    = trans->energy((CaloSampling::CaloSample)i);
  }

  ELinkRingerRingsCnv.transToPers(&trans->ringsLink(), &pers->m_rings, log);
  trigCaloClusterCnv.transToPers(trans, &pers->m_trigCaloCluster, log);
//  pers->m_trigCaloCluster = baseToPersistent( &m_trigCaloClusterCnv, trans, log );
}

void TrigEMClusterCnv_p4::persToTrans(const TrigEMCluster_p4* pers, 
                                     TrigEMCluster* trans, MsgStream &log )
{

  log << MSG::DEBUG << "TrigEMClusterCnv_p4::persToTrans" << endmsg;

  if ( (!trans) || (!pers) ) {
    log << MSG::ERROR << "TrigEMClusterCnv_p4::persToTrans with trans or pers == 0" << endmsg;
    return;
  }

  // See that the order of the stored object is not the same as previously
  trans->setEnergy     (pers->m_allFloats[0]);
  trans->setEt         (pers->m_allFloats[1]);
  trans->setEta        (pers->m_allFloats[2]);
  trans->setPhi        (pers->m_allFloats[3]);
  trans->set_e237      (pers->m_allFloats[4]);
  trans->set_e277      (pers->m_allFloats[5]);
  trans->set_fracs1    (pers->m_allFloats[6]);
  trans->set_weta2     (pers->m_allFloats[7]);
  trans->set_ehad1     (pers->m_allFloats[8]);
  trans->set_Eta1      (pers->m_allFloats[9]);
  trans->set_emaxs1    (pers->m_allFloats[10]);
  trans->set_e2tsts1   (pers->m_allFloats[11]);
  trans->set_e233      (pers->m_allFloats[12]);
  trans->set_wstot     (pers->m_allFloats[13]);
  for(int i=0; i<MAXSIZE; i++){   
    trans->setEnergy ((CaloSampling::CaloSample)i, pers->m_allFloats[14+i]);
  }
  
/*  trans->m_Energy     = pers->m_Energy;
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
  trans->m_e2tsts1    = pers->m_e2tsts1; */

  ElementLink<RingerRingsContainer> ringsLink;
  ELinkRingerRingsCnv.persToTrans(&pers->m_rings, &ringsLink, log);
  trans->setRings (ringsLink);
  trigCaloClusterCnv.persToTrans(&pers->m_trigCaloCluster, trans, log);
//  fillTransFromPStore( &m_trigCaloClusterCnv, pers->m_trigCaloCluster, trans, log );

}
