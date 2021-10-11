/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/MuonCandidate.h"
#include "TrkTrack/Track.h"
#include "TrkParameters/TrackParameters.h"

namespace MuonCombined {
  
  MuonCandidate::MuonCandidate( const Trk::Track& msTrack ) :
    m_extrapolatedTrack(nullptr),
    m_muonSpectrometerTrack(&msTrack),
    m_ownsExtrapolatedTrack(false){}

  MuonCandidate::MuonCandidate( const Trk::Track& msTrack, const Trk::Track* extrapolatedTrack )  :
    m_extrapolatedTrack(extrapolatedTrack),
    m_muonSpectrometerTrack(&msTrack),
    m_ownsExtrapolatedTrack(true) {}

  MuonCandidate::MuonCandidate( const ElementLink<xAOD::TrackParticleContainer>& trackLink ) :
    m_muonSpectrometerTrackLink(trackLink),
    m_extrapolatedTrack(nullptr),
    m_muonSpectrometerTrack((*m_muonSpectrometerTrackLink)->track()),
    m_ownsExtrapolatedTrack(false){}

  MuonCandidate::MuonCandidate( const ElementLink<xAOD::TrackParticleContainer>& trackLink, const Trk::Track* extrapolatedTrack ) :
    m_muonSpectrometerTrackLink(trackLink),
    m_extrapolatedTrack(extrapolatedTrack),
    m_muonSpectrometerTrack((*m_muonSpectrometerTrackLink)->track()),
    m_ownsExtrapolatedTrack(true){}

  MuonCandidate::MuonCandidate(const ElementLink<xAOD::TrackParticleContainer>& trackLink, const ElementLink<TrackCollection>& saTrackLink ) :
    m_muonSpectrometerTrackLink(trackLink),
    m_extrapolatedTrackLink(saTrackLink),
    m_extrapolatedTrack(saTrackLink.getDataPtr()!=nullptr ? *saTrackLink : nullptr),
    m_muonSpectrometerTrack((*m_muonSpectrometerTrackLink)->track()),
    m_ownsExtrapolatedTrack(false) {}

  MuonCandidate::~MuonCandidate() {
    if(m_ownsExtrapolatedTrack) delete m_extrapolatedTrack;
  }
    
  std::string MuonCandidate::toString() const {
    const Trk::Track* track = m_extrapolatedTrack ? m_extrapolatedTrack : &muonSpectrometerTrack();
    const Trk::Perigee* perigee = track->perigeeParameters();
    std::ostringstream sout;
    if(!perigee)     
      sout << " Track has no perigee parameters!";
    else
      sout << " pt " << perigee->momentum().perp() << " eta " << perigee->momentum().eta() << " phi " << perigee->momentum().phi();
    sout << " ownsExtrapolatedTrack " << m_ownsExtrapolatedTrack;
    sout << " hasExtrapolatedTrack " << (m_extrapolatedTrack ? "yes" : "no") << std::endl;    
    return sout.str();
  }
}
