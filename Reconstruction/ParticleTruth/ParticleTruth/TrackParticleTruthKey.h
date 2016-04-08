/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PARTICLETRUTH_TRACKPARTICLETRUTHKEY_H
#define PARTICLETRUTH_TRACKPARTICLETRUTHKEY_H

#include "Particle/TrackParticleContainer.h"
#include "DataModel/ElementLink.h"

namespace Rec {

    // This class identifies a track in TrackParticleTruthCollection.
    // It uses less memory than ElementLink<TrackParticleContainer> because it does not carry a
    // string-the TrackParticleContainer key.  Instead this is a wrapper around
    // a single integer - the index of the track in a TrackParticleContainer.
    //
    // taken from TrackTruthKey by andrei.gaponenko@cern.ch, 2007

    class TrackParticleTruthKey {
    public:
        // Is it enough to use a 32 bit unsigned instead?
        typedef TrackParticleContainer::size_type size_type;

        // Contructor from EL
        TrackParticleTruthKey(const ElementLink<TrackParticleContainer>& el) : m_EL(el) {}

        // Reflex is clueless about handling types that do not have a default constructor
        TrackParticleTruthKey() {}

        // Check whether ElementLink is valid or has been thinned away
        bool      isValid() const { return (m_EL.isValid()); }

        // Return index of EL
        size_type index() const { return (m_EL.index()); }  

        const ElementLink<TrackParticleContainer>& link() const { return m_EL; }


    private:
        ElementLink<TrackParticleContainer>  m_EL;
    };

    inline bool operator<(const TrackParticleTruthKey& a, const TrackParticleTruthKey& b) {
        return a.index() < b.index();
    }
  
}

#endif /* PARTICLETRUTH_TRACKPARTICLETRUTHKEY_H */
