/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedEvent/InDetCandidate.h"

#include <iostream>

namespace MuonCombined {
  
  InDetCandidate::InDetCandidate(const xAOD::TrackParticle& idTrack) : m_idTrackParticle(&idTrack), m_parameters(0) {}

  InDetCandidate::InDetCandidate( const ElementLink<xAOD::TrackParticleContainer>& idTrackLink ) : m_idTrackParticleLink(idTrackLink), m_idTrackParticle(0), m_parameters(0) {}

  InDetCandidate::~InDetCandidate() {
    for( auto x : m_tags ) delete x;

    if (m_parameters) {
      //std::cout << "m_parameters size=" << m_parameters->size() << std::endl;
      //unsigned ctr=0;
      Rec::ParticleExtrapolationVector::const_iterator it=m_parameters->begin();
      Rec::ParticleExtrapolationVector::const_iterator it_e=m_parameters->end();
      // C++11  for (const std::pair<const Trk::TrackParameters*, RecIntersect::LayerType>&  y : *m_parameters) {
      for (;it!=it_e; ++it) {
	//std::cout << "  Deleting element " << ctr++ << ", ptr=" <<  it->first << std::endl;
	delete it->first;
      }
      delete m_parameters;
    }
  }

  std::string InDetCandidate::toString() const {
    std::ostringstream sout;
    sout << " pt " << indetTrackParticle().pt() << " eta " << indetTrackParticle().eta() << " phi " << indetTrackParticle().phi() 
	 << " tags ";
    for( auto x : combinedDataTags() ) sout << " " << x->name();
    if( !combinedDataTags().empty() ) sout << std::endl << " primary tag " << combinedDataTags().front()->toString();
    return sout.str();
  }
}

