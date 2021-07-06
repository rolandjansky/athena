/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/InDetCandidate.h"

#include <iostream>

namespace MuonCombined {
  
  InDetCandidate::InDetCandidate(const xAOD::TrackParticle& idTrack) : m_idTrackParticle(&idTrack), m_siAssociated(false), m_extension(nullptr) {}

  InDetCandidate::InDetCandidate( const ElementLink<xAOD::TrackParticleContainer>& idTrackLink ) : m_idTrackParticleLink(idTrackLink), m_idTrackParticle(nullptr),m_siAssociated(false), m_extension(nullptr) {}

  InDetCandidate::~InDetCandidate() {
    delete m_extension;
  }

  std::string InDetCandidate::toString() const {
    std::ostringstream sout;
    sout << " pt " << indetTrackParticle().pt() << " eta " << indetTrackParticle().eta() << " phi " << indetTrackParticle().phi() ;
    if (isSiliconAssociated()) sout<<std::endl << " is SiliconAssociated.";
    return sout.str();
  }
}

