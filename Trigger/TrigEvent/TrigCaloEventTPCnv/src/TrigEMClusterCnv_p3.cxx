/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrigCaloEvent/TrigEMCluster.h"
#undef private

#include "TrigCaloEventTPCnv/TrigEMCluster_p3.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p3.h"
#include "DataModel/ElementLink.h"
#include "TrigCaloEvent/RingerRingsContainer.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h"

static ElementLinkCnv_p3< ElementLink<RingerRingsContainer> > ELinkRingerRingsCnv;
static TrigCaloClusterCnv_p2 trigCaloClusterCnv;

void TrigEMClusterCnv_p3::transToPers(const TrigEMCluster* trans, 
                                     TrigEMCluster_p3* pers, MsgStream &log )
{

  log << MSG::DEBUG << "TrigEMClusterCnv_p3::tranToPers" << endreq;

  if ( (!trans) || (!pers) ) {
    log << MSG::ERROR << "TrigEMClusterCnv_p3::trasnToPers with trans or pers == 0" << endreq;
  }

  pers->m_allFloats[0]     = trans->m_Energy;
  pers->m_allFloats[1]     = trans->m_Et;
  pers->m_allFloats[2]     = trans->m_Eta;
  pers->m_allFloats[3]     = trans->m_Phi;
  pers->m_allFloats[4]     = trans->m_e237;
  pers->m_allFloats[5]     = trans->m_e277;
  pers->m_allFloats[6]     = trans->m_fracs1;
  pers->m_allFloats[7]     = trans->m_weta2;
  pers->m_allFloats[8]     = trans->m_ehad1;
  pers->m_allFloats[9]     = trans->m_Eta1;
  pers->m_allFloats[10]    = trans->m_emaxs1;
  pers->m_allFloats[11]    = trans->m_e2tsts1;
  for(int i=0; i<MAXSIZE; i++){ 
    pers->m_allFloats[12+i]    = trans->m_EnergyS[i];
  }

  ELinkRingerRingsCnv.transToPers(&trans->m_rings, &pers->m_rings, log);
  trigCaloClusterCnv.transToPers(trans, &pers->m_trigCaloCluster, log);
//  pers->m_trigCaloCluster = baseToPersistent( &m_trigCaloClusterCnv, trans, log );
}

void TrigEMClusterCnv_p3::persToTrans(const TrigEMCluster_p3* pers, 
                                     TrigEMCluster* trans, MsgStream &log )
{

  log << MSG::DEBUG << "TrigEMClusterCnv_p3::persToTrans" << endreq;

  if ( (!trans) || (!pers) ) {
    log << MSG::ERROR << "TrigEMClusterCnv_p3::persToTrans with trans or pers == 0" << endreq;
  }

  // See that the order of the stored object is not the same as previously
  trans->m_Energy     = pers->m_allFloats[0];
  trans->m_Et         = pers->m_allFloats[1];
  trans->m_Eta        = pers->m_allFloats[2];
  trans->m_Phi        = pers->m_allFloats[3];
  trans->m_e237       = pers->m_allFloats[4];
  trans->m_e277       = pers->m_allFloats[5];
  trans->m_fracs1     = pers->m_allFloats[6];
  trans->m_weta2      = pers->m_allFloats[7];
  trans->m_ehad1      = pers->m_allFloats[8];
  trans->m_Eta1       = pers->m_allFloats[9];
  trans->m_emaxs1     = pers->m_allFloats[10];
  trans->m_e2tsts1    = pers->m_allFloats[11];
  for(int i=0; i<MAXSIZE; i++){   
    trans->m_EnergyS[i]    = pers->m_allFloats[12+i];
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

  ELinkRingerRingsCnv.persToTrans(&pers->m_rings, &trans->m_rings, log);
  trigCaloClusterCnv.persToTrans(&pers->m_trigCaloCluster, trans, log);
//  fillTransFromPStore( &m_trigCaloClusterCnv, pers->m_trigCaloCluster, trans, log );

}
