/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_DRMATCHINGTOOL_H
#define INDETPHYSVALMONITORING_DRMATCHINGTOOL_H 1

/**
 * @file   dRMatchingTool.h
 * @author Andreas Sogaard <andreas.sogaard@cern.ch>
 * @date   3 November 2016
 * @brief  Tool to perform dR-based matching of tracks and truth particles.
 */

// STL include(s)
#include <cmath> /* std::fabs, sqrt, pow */
#include <algorithm> /* std::all_of (C++11), std::sort, std::lower_bound, std::upper_bound, std::set_intersection */
#include <iterator> /* std::back_inserter */

// ATLAS software include(s)
#include "AthenaBaseComps/AthAlgTool.h"
#include "PATCore/IAsgSelectionTool.h"
#include "AsgTools/AsgTool.h"

// xAOD include(s)
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"

// Local include(s)
#include "../src/TrackTruthSelectionTool.h"

/**
 * @brief Tool to perform dR-based matching of tracks and truth particles.
 *
 * The tool can match both a single track to a set of truth particles, and vice
 * versa. As the tool derives from the general IAsgSelectionTool interface, the
 * actual 'accept' methods has to be invoked using (dynamic) type casting, e.g.
 *
 *   bool dRMatched = (bool) dynamic_cast<dRMatchingTool&>(*m_dRMatchingTool) \
 *     .accept(track, truthParticles, &truthSelectionTool);
 *
 * to match a track to a subset of truth particle, or
 *
 *   bool dRMatched = (bool) dynamic_cast<dRMatchingTool&>(*m_dRMatchingTool) \
 *     .accept(truth, trackParticles);
 *
 * to match a truth particle to all tracks. The standard accept(const
 * xAOD::IParticle*) method is disabled.
 *
 * Optional track- and truth particle selection functions are allowed, to
 * perform the matching to only a subset of the respective containers. If these
 * functions are omitted as arguments, the matching is performed to the all
 * particles in the specified container.
 *
 * The tool is able to perform matching wrt. dR as well relative pT-resolution,
 * i.e.
 *   \Delta p_{T}/p_{T} = \frac{|p_{T,this} - p_{T,other}|}{p_{T,this}}
 * The tool is configured such that a successful match means that all set cuts
 * were passed. That means that if only the dR-cut is set, no pT-resoution cut
 * is imposed, and conversely. In particular, if neither cut is set, no
 * particles pass the matching. The cut values are set in the job option as e.g.
 *
 *   from InDetPhysValMonitoring.InDetPhysValMonitoringConf import \
 *     dRMatchingTool
 *   dRMatcher = dRMatchingTool()
 *   dRMatcher.dRmax    = 0.1 # Disabled: -1
 *   dRMatcher.pTResMax = 0.1 # Disabled: -1
 *   ToolSvc += dRMatcher
 *
 * The main 'accept' methods are implemented using caching and sorting to avoid
 * O(N^2) complexity from what is essentially nested loops. This speeds up
 * performance drastically, at the cost of transparency. To this end, and for
 * validation purposes, 'acceptLegacy' methods are kept in, which are
 * conceptually simpler and more intuitive, but much slower, than the main
 * methods. However, these legacy methods ought not to be used for actual, even
 * just moderately large runs.
 */
class dRMatchingTool:
  public virtual ::IAsgSelectionTool,
  public asg::AsgTool  {
  ASG_TOOL_CLASS1(dRMatchingTool, IAsgSelectionTool);
public:
  /// Constructor(s).
  dRMatchingTool(const std::string& name);

  /// Destructor.
  virtual
  ~dRMatchingTool();

  /// SelectionTool method(s).
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual const asg::AcceptInfo& getAcceptInfo( ) const override;
  virtual asg::AcceptData accept(const xAOD::IParticle* p) const override;

  /// dR-matching specific method(s).
  // For matching single track to set of truth particles.
  virtual asg::AcceptData
  acceptLegacy(const xAOD::TrackParticle* p,
               const xAOD::TruthParticleContainer* truthParticles,
               bool (* truthSelectionTool)(const xAOD::TruthParticle*) = nullptr) const;

  asg::AcceptData
  accept(const xAOD::TrackParticle* p,
         const xAOD::TruthParticleContainer* truthParticles,
         bool (* truthSelectionTool)(const xAOD::TruthParticle*) = nullptr) const;

  // For matching single truth particle to set of tracks.
  virtual asg::AcceptData
  acceptLegacy(const xAOD::TruthParticle* p,
               const xAOD::TrackParticleContainer* trackParticles,
               bool (* trackSelectionTool)(const xAOD::TrackParticle*) = nullptr) const;

  asg::AcceptData
  accept(const xAOD::TruthParticle* p,
         const xAOD::TrackParticleContainer* trackParticles,
         bool (* trackSelectionTool)(const xAOD::TrackParticle*) = nullptr) const;

  // Accessor for the minimal dR from the latest matching of either type.
  float dRmin() const;
protected:
  /// Internal method(s).
  // Cache track particles.
  void checkCacheTrackParticles(const xAOD::TrackParticleContainer* trackParticles,
                                bool (* trackSelectionTool)(const xAOD::TrackParticle*) = nullptr) const;
  // Cache truth particles.
  void checkCacheTruthParticles(const xAOD::TruthParticleContainer* truthParticles,
                                bool (* truthSelectionTool)(const xAOD::TruthParticle*) = nullptr) const;

  // Clear cached track particle arrays.
  inline void
  clearTrackParticles() const {
    m_trackParticlesSortedPt.clear();
    m_trackParticlesSortedEta.clear();
    m_trackParticlesSortedPhi.clear();
    return;
  }

  // Clear cached truth particle arrays.
  inline void
  clearTruthParticles() const {
    m_truthParticlesSortedPt.clear();
    m_truthParticlesSortedEta.clear();
    m_truthParticlesSortedPhi.clear();
    return;
  }

  // Sort and cache all particles of type U in container into the three vectors,
  // subject to their passing the selectionTool.
  template<class T, class U>
  void sortVectors(const T* container,
                   std::vector< const U* >& vec_pt,
                   std::vector< const U* >& vec_eta,
                   std::vector< const U* >& vec_phi,
                   bool (* selectionTool)(const U*)) const;

  // Match the particle p by dR and/or pT to the particles in the three cached
  // vectors.
  template<class U, class V>
  bool sortedMatch(const U* p,
                   std::vector< const V* >& vec_pt,
                   std::vector< const V* >& vec_eta,
                   std::vector< const V* >& vec_phi) const;
private:
  /// Data member(s).
  // AcceptInfo object.
  asg::AcceptInfo m_accept;
  // Vector of stored cut names and descriptions.
  std::vector<std::pair<std::string, std::string> > m_cuts;

  // Counters.
  mutable ULong64_t m_numProcessed; // !< a counter of the number of tracks proccessed
  mutable ULong64_t m_numPassed; // !< a counter of the number of tracks that passed all cuts
  mutable std::vector<ULong64_t> m_numPassedCuts; // !< tracks the number of tracks that passed each cut family

  /// Cut vales.
  // The maximal dR-distance allowed for successful matching.
  float m_dRmax;
  // The maximal pT-resolution allowed for successful matching.
  float m_pTResMax;
  // The minimal dR for latest successful matching of either type.
  mutable float m_dRmin = 9999.;

  // Copy of the xAOD::TruthParticleContainer used to perform the caching. A
  // copy, rather than a pointer or reference, is necessary to check when a new
  // container is encountered (i.e. when a new event is processed).
  mutable xAOD::TruthParticleContainer m_baseTruthContainer;
  // Copy of the xAOD::TrackParticleContainer used to perform the caching.
  mutable xAOD::TrackParticleContainer m_baseTrackContainer;

  // Cached vectors of the xAOD::TruthParticles in m_baseTruthContainer, sorted
  // in ascending order accoring to their pT, eta, and phi, resp.
  mutable std::vector< const xAOD::TruthParticle* > m_truthParticlesSortedPt;
  mutable std::vector< const xAOD::TruthParticle* > m_truthParticlesSortedEta;
  mutable std::vector< const xAOD::TruthParticle* > m_truthParticlesSortedPhi;
  // Cached vectors of the xAOD::TrackParticles in m_baseTrackContainer, sorted
  // in ascending order accoring to their pT, eta, and phi, resp.
  mutable std::vector< const xAOD::TrackParticle* > m_trackParticlesSortedPt;
  mutable std::vector< const xAOD::TrackParticle* > m_trackParticlesSortedEta;
  mutable std::vector< const xAOD::TrackParticle* > m_trackParticlesSortedPhi;
};

#endif // > !INDETPHYSVALMONITORING_DRMATCHINGTOOL_H
