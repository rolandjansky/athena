/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_TRACKTRUTHLOOKUP_H
#define INDETPHYSVALMONITORING_TRACKTRUTHLOOKUP_H 1

/**
 * @file   TrackTruthLookup.h
 * @author Andreas Sogaard <andreas.sogaard@cern.ch>
 * @date   5 November 2016
 **/

// STL include(s)
#include <unordered_map> /* std::unordered_map */
#include <vector> /* std::vector */
#include <stdexcept> /* std::out_of_range */

// xAOD include(s)
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace IDPVM {
/**
 * Class for providing fast lookup of linked tracks and truth particles.
 *
 * Intended to be used to avoid nested loops when matching tracks and truth
 * particles, e.g. when computing efficiencies.
 *
 * The links are stored in two unordered maps (single caching call per event,
 * with O(N) complexity), emulating a bidirectional[1] map with O(1) look-up
 * complexity. Tracks in the provided container are guaranteed to exist in the
 * corresponding map, but may map to a nullptr if no link to a truth particle
 * exists. Conversely, every truth particle is guaranteed to exist in the
 * corresponding map, but may map to an empty vector if no link to a track
 * exists.
 *
 * Use, e.g. in the InDetPhysValLargeD0Tool, like:
 *
 *     [InDetPhysValMonitoring/InDetPhysValLargeD0Tool.h]
 *         #include "src/TrackTruthLookup.h"
 *         ...
 *         private:
 *             std::unique_ptr<IDPVM::TrackTruthLookup> m_lookup;
 *         ...
 *
 *     [src/InDetPhysValLargeD0Tool.cxx]
 *         InDetPhysValLargeD0Tool::initialize() {
 *             ...
 *             m_lookup = std::move(std::unique_ptr<IDPVM::TrackTruthLookup>(new IDPVM::TrackTruthLookup()));
 *             ...
 *         }
 *         ...
 *         InDetPhysValLargeD0Tool::fillHistograms() {
 *             ...
 *             m_lookup->cache(ptracks, truthParticles);
 *             ...
 *             for (const auto &thisTruth : *truthParticles) {
 *                 for (const auto& track :  m_lookup->getTracks(thisTruth)) {
 *                     // Only looping explicitly associated tracks, hence
 *                     // O(N) complexity rather than O(N^2).
 *                 }
 *             }
 *             ...
 *         }
 *
 * [1] It is not *exactly* bidirectional, since the mapping track <-> truth is
 *     not exactly bijective: tracks are injective (one-to-one) on the set of
 *     truth particles but one truth particle may (although rarely) be
 *     associated to more than one track.
 */
  class TrackTruthLookup {
  public:
    /// Constructor(s).
    TrackTruthLookup() {
    }

    TrackTruthLookup(const xAOD::TrackParticleContainer* trackParticles,
                     const xAOD::TruthParticleContainer* truthParticles) {
      cache(trackParticles, truthParticles);
    }

    TrackTruthLookup(const xAOD::TrackParticleContainer* trackParticles,
                     const std::vector<const xAOD::TruthParticle*>* truthParticlesVec) {
      cache(trackParticles, truthParticlesVec);
    }

    /// Destructor.
    ~TrackTruthLookup() {
    }

    /** @name CacheMethods
     *  Methods for initial cache generation of links between associated
     *  tracks and truth particles in the provided containers.
     */
    // @{
    /** Cache using a vector of TruthParticles, for compatibility with datatype returned
     *  from the xAOD::TruthEvent::truthParticleLinks()
     */
    void cache(const xAOD::TrackParticleContainer* trackParticles,
               const std::vector<const xAOD::TruthParticle*>* truthParticlesVec);

    /// Generate cache from usual evtStore() retrieved container pointers.
    void cache(const xAOD::TrackParticleContainer* trackParticles,
               const xAOD::TruthParticleContainer* truthParticles);
    // @}

    /** Accessor to get the vector of xAOD::TrackParticles associated with 'truth',
     *  possibly empty if none is associated.
     *  Throws out_of_range exception if truth particle does not exist in cache.
     */
    std::vector<const xAOD::TrackParticle*> getTracks(const xAOD::TruthParticle* truth) const;

    /** Accessor to get the unique xAOD::TruthParticle associated with 'track', or
     *  a nullptr is none is associated.
     * Throws out_of_range exception if track does not exist in cache.
     */
    const xAOD::TruthParticle* getTruth(const xAOD::TrackParticle* track) const;

    /// Returns true if the Lookup contains the pointer 'truth'.
    inline bool
    contains(const xAOD::TruthParticle* truth) const {
      return m_mapTruth.find(truth) != m_mapTruth.end();
    }

    /// Returns true if the Lookup contains the pointer 'track'.
    inline bool
    contains(const xAOD::TrackParticle* track) const {
      return m_mapTrack.find(track) != m_mapTrack.end();
    }

    /// Clears the contents of the unordered map data members.
    inline void
    clear() {
      m_mapTrack.clear();
      m_mapTruth.clear();
      return;
    }

  private:
    void cacheTracks(const xAOD::TrackParticleContainer* trackParticles);
    void cacheTruth(const xAOD::TruthParticleContainer* truthParticles);
    void cacheTruth(const std::vector<const xAOD::TruthParticle*>* truthParticlesVec);
    /// Data member(s).
    // Mapping of xAOD::TrackParticle to their unique associated
    // xAOD::TruthParticle. nullptr if none exist.
    std::unordered_map<const xAOD::TrackParticle*,
                       const xAOD::TruthParticle*> m_mapTrack;

    // Mapping of xAOD::TruthParticle to vector of their (possibly multiple
    // associated xAOD::TrackParticles. Empty if none exist.
    std::unordered_map<const xAOD::TruthParticle*,
                       std::vector<const xAOD::TrackParticle*> > m_mapTruth;
  };
} // namespace

#endif // > !INDETPHYSVALMONITORING_TRACKTRUTHLOOKUP_H
