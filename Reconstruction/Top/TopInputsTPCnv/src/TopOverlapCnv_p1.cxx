/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "TopInputsTPCnv/TopOverlapCnv_p1.h"


void TopOverlapCnv_p1::transToPers( const TopRec::TopOverlap* transObj,
				    TopOverlap_p1* persObj,
				    MsgStream &msg ) {

  msg << MSG::DEBUG << typeid(*transObj).name() << " called" << endreq;

  //persObj->m_flag=transObj->flag();

  persObj->m_index=transObj->index();

  persObj->m_use=transObj->use();

  ElementLinkVector<Analysis::MuonContainer> overlapMuons;
  overlapMuons = transObj->overlapMuonsVec();
  ElementLinkIntVector_p1 overlapMuons_p1;
  m_muon_cnv.transToPers(&overlapMuons,&overlapMuons_p1,msg);
  persObj->m_overlapMuons_p1=overlapMuons_p1;

  ElementLinkVector<ElectronContainer> overlapElectrons;
  overlapElectrons = transObj->overlapElectronsVec();
  ElementLinkIntVector_p1 overlapElectrons_p1;
  m_electron_cnv.transToPers(&overlapElectrons,&overlapElectrons_p1,msg);
  persObj->m_overlapElectrons_p1=overlapElectrons_p1;

  ElementLinkVector<PhotonContainer> overlapPhotons;
  overlapPhotons = transObj->overlapPhotonsVec();
  ElementLinkIntVector_p1 overlapPhotons_p1;
  m_photon_cnv.transToPers(&overlapPhotons,&overlapPhotons_p1,msg);
  persObj->m_overlapPhotons_p1=overlapPhotons_p1;

  ElementLinkVector<Analysis::TauJetContainer> overlapTauJets;
  overlapTauJets = transObj->overlapTauJetsVec();
  ElementLinkIntVector_p1 overlapTauJets_p1;
  m_tauJet_cnv.transToPers(&overlapTauJets,&overlapTauJets_p1,msg);
  persObj->m_overlapTauJets_p1=overlapTauJets_p1;

  ElementLinkVector<JetCollection> overlapJets;
  overlapJets = transObj->overlapJetsVec();
  ElementLinkIntVector_p1 overlapJets_p1;
  m_jet_cnv.transToPers(&overlapJets,&overlapJets_p1,msg);
  persObj->m_overlapJets_p1=overlapJets_p1;

  ElementLinkVector<Rec::TrackParticleContainer> overlapTracks;
  overlapTracks = transObj->overlapTracksVec();
  ElementLinkIntVector_p1 overlapTracks_p1;
  m_track_cnv.transToPers(&overlapTracks,&overlapTracks_p1,msg);
  persObj->m_overlapTracks_p1=overlapTracks_p1;

  msg << MSG::DEBUG << "Created persistent state of " << typeid(*transObj).name() 
      << endreq;
 
}

void TopOverlapCnv_p1::persToTrans( const TopOverlap_p1* persObj,
				    TopRec::TopOverlap* transObj,
				    MsgStream &msg ) {

  msg << MSG::DEBUG << "Creating transient state of " << typeid(*persObj).name()
      << endreq;

  //transObj->setFlag(persObj->m_flag);
  
  transObj->setIndex(persObj->m_index);

  transObj->setUse(persObj->m_use);
 
  ElementLinkVector<Analysis::MuonContainer> overlapMuons;
  ElementLinkIntVector_p1 overlapMuons_p1 = persObj->m_overlapMuons_p1;
  m_muon_cnv.persToTrans(&overlapMuons_p1,&overlapMuons,msg);
  transObj->setOverlapMuons(overlapMuons);

  ElementLinkVector<ElectronContainer> overlapElectrons;
  ElementLinkIntVector_p1 overlapElectrons_p1 = persObj->m_overlapElectrons_p1;
  m_electron_cnv.persToTrans(&overlapElectrons_p1,&overlapElectrons,msg);
  transObj->setOverlapElectrons(overlapElectrons);

  ElementLinkVector<PhotonContainer> overlapPhotons;
  ElementLinkIntVector_p1 overlapPhotons_p1 = persObj->m_overlapPhotons_p1;
  m_photon_cnv.persToTrans(&overlapPhotons_p1,&overlapPhotons,msg);
  transObj->setOverlapPhotons(overlapPhotons);

  ElementLinkVector<Analysis::TauJetContainer> overlapTauJets;
  ElementLinkIntVector_p1 overlapTauJets_p1 = persObj->m_overlapTauJets_p1;
  m_tauJet_cnv.persToTrans(&overlapTauJets_p1,&overlapTauJets,msg);
  transObj->setOverlapTauJets(overlapTauJets);

  ElementLinkVector<JetCollection> overlapJets;
  ElementLinkIntVector_p1 overlapJets_p1 = persObj->m_overlapJets_p1;
  m_jet_cnv.persToTrans(&overlapJets_p1,&overlapJets,msg);
  transObj->setOverlapJets(overlapJets);

  ElementLinkVector<Rec::TrackParticleContainer> overlapTracks;
  ElementLinkIntVector_p1 overlapTracks_p1 = persObj->m_overlapTracks_p1;
  m_track_cnv.persToTrans(&overlapTracks_p1,&overlapTracks,msg);
  transObj->setOverlapTracks(overlapTracks);

  msg << MSG::DEBUG << "Loading " << typeid(*transObj).name() << endreq;
}

