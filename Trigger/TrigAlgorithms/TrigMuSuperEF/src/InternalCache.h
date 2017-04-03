/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigMuSuperEF_InternalCache_H__
#define TrigMuSuperEF_InternalCache_H__

#include <vector>
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

class TrigMuonEFInfo;
class InternalCache  {
  
 public:
  /// Default Constructor
  InternalCache();
  
  /// Destructor
  ~InternalCache();
  
  
  void SetMuonCandidates(MuonCandidateCollection *muonCand);
  void SetCombinedTracks(xAOD::TrackParticleContainer *combtrk);
  void SetSATracks(xAOD::TrackParticleContainer *satrk);
  void SetExtrapolatedTracks(TrackCollection *extrtrk);
  void SetCombinedTrackColl(TrackCollection *combtrkcoll);
  void SetSATracksAux(xAOD::TrackParticleAuxContainer satrkaux);
  void SetMuonContainer(xAOD::MuonContainer*  muonContainerOwn);
  void SetSegmentTaggedMuonCandidates(MuonCandidateCollection*  muonCand);
  MuonCandidateCollection *MuonCandidates() { return m_STMuonCandidates; }
  std::pair<xAOD::TrackParticleContainer*,xAOD::TrackParticleAuxContainer*> CombinedTracks() {return m_combinedtracks;}
  std::pair<xAOD::TrackParticleContainer*, xAOD::TrackParticleAuxContainer*> SATracks() {return m_satracks;}
  TrackCollection *ExtrapolatedTracks() {   return m_extrapolatedtracks;}
  TrackCollection *CombinedTrackColl() {return m_combtrkcoll;}
  std::pair<xAOD::MuonContainer*, xAOD::MuonAuxContainer*> MuonContainer() {return m_muoncont;}
  MuonCandidateCollection *MuonSTCandidates() { return m_STMuonCandidates; }

 private:
  
  MuonCandidateCollection *m_muoncandidates;
  MuonCandidateCollection *m_STMuonCandidates;
  std::pair<xAOD::TrackParticleContainer*,xAOD::TrackParticleAuxContainer*> m_combinedtracks;
  std::pair<xAOD::TrackParticleContainer*,xAOD::TrackParticleAuxContainer*> m_satracks;
  TrackCollection *m_extrapolatedtracks;
  TrackCollection *m_combtrkcoll;
  std::pair<xAOD::MuonContainer*, xAOD::MuonAuxContainer*> m_muoncont;
};

#endif
