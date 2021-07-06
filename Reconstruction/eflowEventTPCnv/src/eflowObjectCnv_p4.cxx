/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowEvent/eflowObject.h"
#include "eflowEventTPCnv/eflowObjectCnv_p4.h"
#include "Navigation/NavigableVector.h"

void eflowObjectCnv_p4::persToTrans( const eflowObject_p4 *persObj, eflowObject *transObj, MsgStream &eflowStream ){


  //data

  transObj->set_type (persObj->m_eflowType);
  transObj->set_charge (persObj->m_charge);
  transObj->setCaloRecoStatus (CaloRecoStatus(persObj->m_recoStatus));

  transObj->set_eta(persObj->m_eta);
  transObj->set_phi(persObj->m_phi);
  transObj->set_energy(persObj->m_e);
  transObj->set_m(persObj->m_m);

  //element links data

  //m_elMuCnv.persToTrans(&persObj->m_muonElementLink, &transObj->m_muonElementLink, eflowStream);
  //m_elVxCnv.persToTrans(&persObj->m_convElementLink, &transObj->m_convElementLink, eflowStream);

  //navigable vectors data
  //convert persistent elementlinkvector to transient first and then copy links to navigable vectors

  ElementLinkVector<Rec::TrackParticleContainer> dummyTrackVector;
  m_navTrackCnv.persToTrans(&persObj->m_eflowTrack,&dummyTrackVector,eflowStream);
  for (const ElementLink<Rec::TrackParticleContainer> track : dummyTrackVector)
    transObj->addTrack(track);

  ElementLinkVector<CaloClusterContainer> dummyClusterVector;
  m_navClusCnv.persToTrans(&persObj->m_eflowClus,&dummyClusterVector,eflowStream);
  for (const ElementLink<CaloClusterContainer> clus : dummyClusterVector)
    transObj->addClus (clus);
}


void eflowObjectCnv_p4::transToPers( const eflowObject *transObj,eflowObject_p4 *persObj, MsgStream &eflowStream ){

  //data

  persObj->m_eflowType = transObj->eflowType();
  persObj->m_charge = transObj->charge();
  persObj->m_recoStatus = transObj->getCaloRecoStatus().getStatusWord();

  persObj->m_eta = transObj->eta();
  persObj->m_phi = transObj->phi();
  persObj->m_e = transObj->energy();
  persObj->m_m = transObj->m();

  //element links data


  //if (transObj->m_muonElementLink.isValid()) m_elMuCnv.transToPers(&transObj->m_muonElementLink, &persObj->m_muonElementLink, eflowStream);
  //if (transObj->m_convElementLink.isValid()) m_elVxCnv.transToPers(&transObj->m_convElementLink, &persObj->m_convElementLink, eflowStream);

  //navigable vectors
  //there are no converters so we have to make an ElementLinkVector first for which converters exist

  ElementLinkVector<Rec::TrackParticleContainer> dummyTrackVector;
  for (int i = 0; i < transObj->numTrack();i++)
    dummyTrackVector.push_back(transObj->trackLink(i));
  m_navTrackCnv.transToPers(&dummyTrackVector, &persObj->m_eflowTrack, eflowStream);

  ElementLinkVector<CaloClusterContainer> dummyClusterVector;
  for (int i = 0; i < transObj->numClus(); i++)
    dummyClusterVector.push_back(transObj->clusLink(i));
  m_navClusCnv.transToPers(&dummyClusterVector, &persObj->m_eflowClus, eflowStream);
}
