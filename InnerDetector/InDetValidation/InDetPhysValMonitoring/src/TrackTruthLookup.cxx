/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackTruthLookup.h"

namespace IDPVM {
// Utility function to get the linked truth particle from a track.
// Taken directly from InDetPhysValLargeD0Tool.cxx
  const xAOD::TruthParticle*
  getTruthPointer(const xAOD::TrackParticle& track) {
    using ElementTruthLink_t = ElementLink<xAOD::TruthParticleContainer>;
    const xAOD::TruthParticle* result(nullptr);

    // Check whether truthParticleLink exists.
    if (track.isAvailable<ElementTruthLink_t>("truthParticleLink")) {
      // If so, get linked xAOD::TruthParticle.
      const ElementTruthLink_t ptruthContainer =
        track.auxdata<ElementTruthLink_t>("truthParticleLink");
      if (ptruthContainer.isValid()) {
        result = *ptruthContainer;
      }
    }
    return result;
  }

  std::vector<const xAOD::TrackParticle*>
  TrackTruthLookup::getTracks(const xAOD::TruthParticle* truth) const {
    // C++ try-catch blocks are zero-cost if no exception is thrown, so
    // performance should be unaffected by this check.
    try {
      return m_mapTruth.at(truth);
    } catch (const std::out_of_range& oor) {
      throw std::out_of_range(
              "Truth particle was not found in lookup map. Did you remember to call TrackTruthLookup::cache?");
    }
  }

  const xAOD::TruthParticle*
  TrackTruthLookup::getTruth(const xAOD::TrackParticle* track) const {
    // C++ try-catch blocks are zero-cost if no exception is thrown, so
    // performance should be unaffected by this check.
    try {
      return m_mapTrack.at(track);
    } catch (const std::out_of_range& oor) {
      throw std::out_of_range(
              "Track particle was not found in lookup map. Did you remember to call TrackTruthLookup::cache?");
    }
  }

  void
  TrackTruthLookup::cacheTracks(const xAOD::TrackParticleContainer* trackParticles) {
    // Cache all track particles.
    for (const xAOD::TrackParticle* track : *trackParticles) {
      const xAOD::TruthParticle* truth = getTruthPointer(*track);

      // Store pointer, even if null.
      m_mapTrack[track] = truth;

      // Store link in reverse direction as well, to avoid O(N^2) complexity.
      if (truth) {
        if (!contains(truth)) {
          // New truth particle; initialise vector.
          m_mapTruth[truth] = {
            track
          };
        } else {
          // Existing truth particle; append vector.
          m_mapTruth[truth].push_back(track);
        }
      }
    }
     }

  void
  TrackTruthLookup::cacheTruth(const xAOD::TruthParticleContainer* truthParticles) {
    // Cache remaining truth particles.
    for (const xAOD::TruthParticle* truth : *truthParticles) {
      if (!contains(truth)) {
        m_mapTruth[truth] = {};
      }
    }
     }

  void
  TrackTruthLookup::cacheTruth(const std::vector<const xAOD::TruthParticle*>* truthParticlesVec) {
    // Cache remaining truth particles.
    for (const xAOD::TruthParticle* truth : *truthParticlesVec) {
      if (!contains(truth)) {
        m_mapTruth[truth] = {};
      }
    }
     }

  void
  TrackTruthLookup::cache(const xAOD::TrackParticleContainer* trackParticles,
                          const xAOD::TruthParticleContainer* truthParticles) {
    // Clear existing cache.
    clear();
    cacheTracks(trackParticles);
    cacheTruth(truthParticles);
 }

  void
  TrackTruthLookup::cache(const xAOD::TrackParticleContainer* trackParticles,
                          const std::vector<const xAOD::TruthParticle*>* truthParticlesVec) {
    // Clear existing cache.
    clear();
    cacheTracks(trackParticles);
    cacheTruth(truthParticlesVec);
 }
} // namespace
