/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/InDetCandidate.h"

#include <iostream>

namespace MuonCombined {
  
  InDetCandidate::InDetCandidate(const xAOD::TrackParticle& idTrack) : m_idTrackParticle(&idTrack), m_siAssociated(false) {}

  InDetCandidate::InDetCandidate( const ElementLink<xAOD::TrackParticleContainer>& idTrackLink ) : m_idTrackParticleLink(idTrackLink), m_idTrackParticle(0),m_siAssociated(false) {}

  InDetCandidate::~InDetCandidate() {
    for( auto x : m_tags ) delete x;
  }

  std::string InDetCandidate::toString() const {
    std::ostringstream sout;
    sout << " pt " << indetTrackParticle().pt() << " eta " << indetTrackParticle().eta() << " phi " << indetTrackParticle().phi() 
	 << " tags ";
    for( auto x : combinedDataTags() ) sout << " " << x->name();
    if( !combinedDataTags().empty() ) sout << std::endl << " primary tag " << combinedDataTags().front()->toString();
    if (isSiliconAssociated()) sout<<std::endl << " is SiliconAssociated.";
    return sout.str();
  }
}

