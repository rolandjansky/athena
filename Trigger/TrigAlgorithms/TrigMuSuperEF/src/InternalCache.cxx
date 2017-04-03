/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InternalCache.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
InternalCache::InternalCache(){
  m_muoncandidates=0;
  m_STMuonCandidates=0;
  m_extrapolatedtracks =0;
  m_combinedtracks=std::make_pair(new xAOD::TrackParticleContainer(),new xAOD::TrackParticleAuxContainer());
  m_satracks=std::make_pair(new xAOD::TrackParticleContainer(), new xAOD::TrackParticleAuxContainer());
  m_combtrkcoll=0;
  m_muoncont=std::make_pair(new xAOD::MuonContainer, new xAOD::MuonAuxContainer());
 }


/**
 * Destructor.
 * We do not delete the pointers, since these are not owned by this class.
 */
InternalCache::~InternalCache(){
  if(m_combinedtracks.first) delete m_combinedtracks.first;  
  if(m_combinedtracks.second) delete m_combinedtracks.second; 
  if(m_satracks.first) delete m_satracks.first;  
  if(m_satracks.second) delete m_satracks.second; 
  if(m_muoncont.first) delete m_muoncont.first;  
  if(m_muoncont.second) delete m_muoncont.second; 
}


void InternalCache::SetMuonCandidates(MuonCandidateCollection *muonCand){
  m_muoncandidates = muonCand;
}

void InternalCache::SetSegmentTaggedMuonCandidates(MuonCandidateCollection *muonCand){
  m_STMuonCandidates = muonCand;
}

void InternalCache::SetCombinedTracks(xAOD::TrackParticleContainer* combtrk){
  xAOD::TrackParticleContainer::iterator trk_itr = combtrk->begin();
  xAOD::TrackParticleContainer::iterator trk_end = combtrk->end();
  m_combinedtracks.first->setStore(m_combinedtracks.second);
  m_combinedtracks.first->reserve(combtrk->size());
  for(; trk_itr !=trk_end; ++trk_itr){
    xAOD::TrackParticle *trk1 = new xAOD::TrackParticle();
    trk1->makePrivateStore(**trk_itr);
    m_combinedtracks.first->push_back(trk1);
  }
}

void InternalCache::SetSATracks(xAOD::TrackParticleContainer* satrk){
  xAOD::TrackParticleContainer::iterator trk_itr = satrk->begin();
  xAOD::TrackParticleContainer::iterator trk_end = satrk->end();

  m_satracks.first->setStore(m_satracks.second);
  m_satracks.first->reserve(satrk->size());
  for(; trk_itr !=trk_end; ++trk_itr){
    xAOD::TrackParticle *trk1 = new xAOD::TrackParticle();
    trk1->makePrivateStore(**trk_itr);
    m_satracks.first->push_back(trk1);
  }
}

void InternalCache::SetExtrapolatedTracks(TrackCollection *extrtrk){
  m_extrapolatedtracks = extrtrk;
}

void InternalCache::SetCombinedTrackColl(TrackCollection *combtrkcoll){
  m_combtrkcoll=combtrkcoll;
}

void InternalCache::SetMuonContainer(xAOD::MuonContainer* muonContainer){

  m_muoncont.first->setStore(m_muoncont.second);
  xAOD::MuonContainer::iterator mu_itr = muonContainer->begin();
  xAOD::MuonContainer::iterator mu_end = muonContainer->end();
  m_muoncont.first->reserve(muonContainer->size());
  for(; mu_itr != mu_end; ++mu_itr){
    xAOD::Muon *mu1 = new xAOD::Muon();
    mu1->makePrivateStore(**mu_itr);
    m_muoncont.first->push_back(mu1);
  }
}
