/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopInputs/TopOverlap.h"

/////////////////////////////////////////////////////////////////////////////////////
TopRec::TopOverlap::TopOverlap() 
  : m_overlapMuons(0), m_overlapElectrons(0), m_overlapPhotons(0), m_overlapTauJets(0), m_overlapJets(0)
{
  m_use=true;
  m_index=0;
  //m_flag=0;
}

/////////////////////////////////////////////////////////////////////////////////////
bool TopRec::TopOverlap::hasOverlap() const
{
  bool overlap = overlapsWithMuon();
  overlap = overlap || overlapsWithElectron();
  overlap = overlap || overlapsWithPhoton();
  overlap = overlap || overlapsWithTauJet();
  overlap = overlap || overlapsWithJet();
  overlap = overlap || overlapsWithTrack();
  return overlap;
}

/////////////////////////////////////////////////////////////////////////////////////
const MuonLinks TopRec::TopOverlap::overlapMuons() const {
  MuonLinks muonLinks;
  ElementLinkVector<Analysis::MuonContainer>::const_iterator muonItr  = m_overlapMuons.begin();
  ElementLinkVector<Analysis::MuonContainer>::const_iterator muonItrE = m_overlapMuons.end();
  for(; muonItr!=muonItrE; ++muonItr) {
    ElementLink<Analysis::MuonContainer> muon_el = (*muonItr);
    const Analysis::Muon* muon = *muon_el; // FIXME: have to dereference ElementLink before filling or ATHENA crashes
    if(muon)
      muonLinks.push_back(muon_el);
  }
  return muonLinks;
}  

const ElectronLinks TopRec::TopOverlap::overlapElectrons() const {
  ElectronLinks electronLinks;
  ElementLinkVector<ElectronContainer>::const_iterator electronItr  = m_overlapElectrons.begin();
  ElementLinkVector<ElectronContainer>::const_iterator electronItrE = m_overlapElectrons.end();
  for(; electronItr!=electronItrE; ++electronItr) {
    ElementLink<ElectronContainer> electron_el = (*electronItr);
    const Analysis::Electron* electron = *electron_el;
    if(electron) 
      electronLinks.push_back(electron_el);
  }
  return electronLinks;
}  

const PhotonLinks TopRec::TopOverlap::overlapPhotons() const {
  PhotonLinks photonLinks;
  ElementLinkVector<PhotonContainer> ::const_iterator photonItr  = m_overlapPhotons.begin();
  ElementLinkVector<PhotonContainer> ::const_iterator photonItrE = m_overlapPhotons.end();
  for(; photonItr!=photonItrE; ++photonItr) {
    ElementLink<PhotonContainer> photon_el = (*photonItr);
    const Analysis::Photon* photon = *photon_el;
    if(photon)
      photonLinks.push_back(photon_el);
  }
  return photonLinks;
}  

const TauJetLinks TopRec::TopOverlap::overlapTauJets() const {
  TauJetLinks tauJetLinks;
  ElementLinkVector<Analysis::TauJetContainer> ::const_iterator tauJetItr  = m_overlapTauJets.begin();
  ElementLinkVector<Analysis::TauJetContainer> ::const_iterator tauJetItrE = m_overlapTauJets.end();
  for(; tauJetItr!=tauJetItrE; ++tauJetItr) {
    ElementLink<Analysis::TauJetContainer> tau_el = (*tauJetItr);
    const Analysis::TauJet* tau = *tau_el;
    if(tau)
      tauJetLinks.push_back(tau_el);
  }
  return tauJetLinks;
}  

const JetLinks TopRec::TopOverlap::overlapJets() const {
  JetLinks jetLinks;
  ElementLinkVector<JetCollection> ::const_iterator jetItr  = m_overlapJets.begin();
  ElementLinkVector<JetCollection> ::const_iterator jetItrE = m_overlapJets.end();
  for(; jetItr!=jetItrE; ++jetItr) {
    ElementLink<JetCollection> jet_el = (*jetItr);
    const Jet* jet = *jet_el;
    if(jet)
      jetLinks.push_back(jet_el);
  }
  return jetLinks;
}  

const TrackLinks TopRec::TopOverlap::overlapTracks() const {
  TrackLinks trackLinks;
  ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trackItr  = m_overlapTracks.begin();
  ElementLinkVector<Rec::TrackParticleContainer>::const_iterator trackItrE = m_overlapTracks.end();
  for(; trackItr!=trackItrE; ++trackItr) {
    ElementLink<Rec::TrackParticleContainer> track_el = (*trackItr);
    const Rec::TrackParticle* track = *track_el;
    if(track) {
      trackLinks.push_back(track_el);
    }
  }
  return trackLinks;
}  

/////////////////////////////////////////////////////////////////////////////////////
void TopRec::TopOverlap::insertOverlapMuon(const MuonLink& overlapMuon) {
  m_overlapMuons.push_back(overlapMuon);
}

void TopRec::TopOverlap::insertOverlapElectron(const ElectronLink& overlapElectron) {
  m_overlapElectrons.push_back(overlapElectron); 
}

void TopRec::TopOverlap::insertOverlapPhoton(const PhotonLink& overlapPhoton) {
  m_overlapPhotons.push_back(overlapPhoton); 
}

void TopRec::TopOverlap::insertOverlapTauJet(const TauJetLink& overlapTauJet) {
  m_overlapTauJets.push_back(overlapTauJet); 
}

void TopRec::TopOverlap::insertOverlapJet(const JetLink& overlapJet) {
  m_overlapJets.push_back(overlapJet); 
}
 
void TopRec::TopOverlap::insertOverlapTrack(const TrackLink& overlapTrack) {
  m_overlapTracks.push_back(overlapTrack); 
}
 
/////////////////////////////////////////////////////////////////////////////////////
void TopRec::TopOverlap::insertLink(const ElementLink<Analysis::MuonContainer>& overlapMuon) {
  this->insertOverlapMuon(overlapMuon);
}

void TopRec::TopOverlap::insertLink(const ElementLink<ElectronContainer>& overlapElectron) {
  this->insertOverlapElectron(overlapElectron);
}

void TopRec::TopOverlap::insertLink(const ElementLink<PhotonContainer>& overlapPhoton) {
  this->insertOverlapPhoton(overlapPhoton);
}

void TopRec::TopOverlap::insertLink(const ElementLink<Analysis::TauJetContainer>& overlapTauJet) {
  this->insertOverlapTauJet(overlapTauJet);
}

void TopRec::TopOverlap::insertLink(const ElementLink<JetCollection>& overlapJet) {
  this->insertOverlapJet(overlapJet);
}
 
void TopRec::TopOverlap::insertLink(const ElementLink<Rec::TrackParticleContainer>& overlapTrack) {
  this->insertOverlapTrack(overlapTrack);
}
 
/////////////////////////////////////////////////////////////////////////////////////
void TopRec::TopOverlap::setOverlapMuons(const ElementLinkVector<Analysis::MuonContainer>& overlapMuons) { 
  if(overlapMuons.size() > 0) {
    m_overlapMuons = overlapMuons; 
  }
}

void TopRec::TopOverlap::setOverlapElectrons(const ElementLinkVector<ElectronContainer>& overlapElectrons) { 
  if(overlapElectrons.size() > 0) {
    m_overlapElectrons = overlapElectrons; 
  }
}

void TopRec::TopOverlap::setOverlapPhotons(const ElementLinkVector<PhotonContainer>& overlapPhotons) {
  if(overlapPhotons.size() > 0) {
    m_overlapPhotons = overlapPhotons; 
  }
}

void TopRec::TopOverlap::setOverlapTauJets(const ElementLinkVector<Analysis::TauJetContainer>& overlapTauJets) {
  if(overlapTauJets.size() > 0) {
    m_overlapTauJets = overlapTauJets; 
  }
}

void TopRec::TopOverlap::setOverlapJets(const ElementLinkVector<JetCollection>& overlapJets) {
  if(overlapJets.size() > 0) {
    m_overlapJets = overlapJets; 
  }
}

void TopRec::TopOverlap::setOverlapTracks(const ElementLinkVector<Rec::TrackParticleContainer>& overlapTracks) {
  if(overlapTracks.size() > 0) {
    m_overlapTracks = overlapTracks; 
  }
}

/////////////////////////////////////////////////////////////////////////////////////
void TopRec::TopOverlap::setOverlapMuons(const MuonLinks& overlapMuons) {
  if(overlapMuons.size()>0) {
    MuonLinks::const_iterator muonItr  = overlapMuons.begin();
    MuonLinks::const_iterator muonItrE = overlapMuons.end();
    for(; muonItr!=muonItrE; ++muonItr)
      m_overlapMuons.push_back(*muonItr);
  }
}

void TopRec::TopOverlap::setOverlapElectrons(const ElectronLinks& overlapElectrons) {
  if(overlapElectrons.size()>0) {
    ElectronLinks::const_iterator electronItr  = overlapElectrons.begin();
    ElectronLinks::const_iterator electronItrE = overlapElectrons.end();
    for(; electronItr!=electronItrE; ++electronItr)
      m_overlapElectrons.push_back(*electronItr);
  }
}

void TopRec::TopOverlap::setOverlapPhotons(const PhotonLinks& overlapPhotons) {
  if(overlapPhotons.size()>0) {
    PhotonLinks::const_iterator photonItr  = overlapPhotons.begin();
    PhotonLinks::const_iterator photonItrE = overlapPhotons.end();
    for(; photonItr!=photonItrE; ++photonItr)
      m_overlapPhotons.push_back(*photonItr);
  }
}

void TopRec::TopOverlap::setOverlapTauJets(const TauJetLinks& overlapTauJets) {
  if(overlapTauJets.size()>0) {
    TauJetLinks::const_iterator tauJetItr  = overlapTauJets.begin();
    TauJetLinks::const_iterator tauJetItrE = overlapTauJets.end();
    for(; tauJetItr!=tauJetItrE; ++tauJetItr)
      m_overlapTauJets.push_back(*tauJetItr);
  }
}

void TopRec::TopOverlap::setOverlapJets(const JetLinks& overlapJets) {
  if(overlapJets.size()>0) {
    JetLinks::const_iterator jetItr  = overlapJets.begin();
    JetLinks::const_iterator jetItrE = overlapJets.end();
    for(; jetItr!=jetItrE; ++jetItr)
      m_overlapJets.push_back(*jetItr);
  }
}

void TopRec::TopOverlap::setOverlapTracks(const TrackLinks& overlapTracks) {
  if(overlapTracks.size()>0) {
    TrackLinks::const_iterator trackItr  = overlapTracks.begin();
    TrackLinks::const_iterator trackItrE = overlapTracks.end();
    for(; trackItr!=trackItrE; ++trackItr)
      m_overlapTracks.push_back(*trackItr);
  }
}
