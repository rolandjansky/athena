/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowEvent/eflowObject.h"
#include "eflowEventTPCnv/eflowObjectCnv_p1.h"
#include "Navigation/NavigableVector.h"

void eflowObjectCnv_p1::persToTrans( const eflowObject_p1 *persObj, eflowObject *transObj, MsgStream &eflowStream ){


  //data

  transObj->set_d0 (persObj->m_d0);
  transObj->set_z0 (persObj->m_z0);
  transObj->set_type (persObj->m_eflowType);
  transObj->set_charge (persObj->m_charge);
  transObj->set_valid (persObj->m_valid);
  //transObj->m_nTrack = persObj->m_nTrack;
  //transObj->m_nClus = persObj->m_nClus;

  transObj->set_eta(persObj->m_eta);
  transObj->set_phi(persObj->m_phi);
  transObj->set_energy(persObj->m_e);
  transObj->set_m(persObj->m_m);

  //element links data

  ElementLink<VxContainer> convElementLink;
  m_elVxCnv.persToTrans(&persObj->m_convElementLink, &convElementLink, eflowStream);
  transObj->addConversion (convElementLink);

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


void eflowObjectCnv_p1::transToPers( const eflowObject *transObj,eflowObject_p1 *persObj, MsgStream &eflowStream ){

  //data

  persObj->m_d0 = transObj->d0();
  persObj->m_z0 = transObj->z0();
  persObj->m_eflowType = transObj->eflowType();
  persObj->m_charge = transObj->charge();
  persObj->m_valid = transObj->isValid();
  //persObj->m_nTrack = transObj->m_nTrack;
  //persObj->m_nClus = transObj->m_nClus;

  persObj->m_eta = transObj->eta();
  persObj->m_phi = transObj->phi();
  persObj->m_e = transObj->energy();
  persObj->m_m = transObj->m();

  //element links data

  m_elVxCnv.transToPers(&transObj->conversionLink(), &persObj->m_convElementLink, eflowStream);

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
