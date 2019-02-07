/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "dRMatchingTool.h"

// InDetPhysValMonitoring include(s)
#include "../src/TrackTruthSelectionTool.h" /* to perform dynamic_cast */

namespace { // Placing utility functions in anonymous namespace.
// Utility definitions.
  const float pi = 3.14159265359;
  const float twopi = 6.28318530718;

// Accessor utility function, for getting the best available value of pT.
  template<class U>
  float
  pt(const U* p) {
    return p->pt();
  }

// Accessor utility function, for getting the best available value of phi.
// Need to explicitly state that 'isAvailable' and 'auxdata' are templated
// functions.
  template<class U>
  float
  phi(const U* p) {
    return(p->template isAvailable<float>("phi") ?
           p->template auxdata<float>("phi") : p->phi());
  }

// Accessor utility function, for getting the best available value of eta.
// Need to explicitly state that 'isAvailable' and 'auxdata' are templated
// functions.
  template<class U>
  float
  eta(const U* p) {
    return(p->template isAvailable<float>("eta") ?
           p->template auxdata<float>("eta") : p->eta());
  }

// Function to compute dPhi-separation using best available parameter values.
  template<class U, class V>
  float
  comp_deltaPhi(const U* p1, const V* p2) {
    // Ensures that $\Delta\phi \in [-pi, pi)$, and takes absolute value.
    float dphi = phi(p1) - phi(p2);

    while (dphi >= pi) {
      dphi -= twopi;
    }
    while (dphi < -pi) {
      dphi += twopi;
    }
    return std::fabs(dphi);
  }

// Function to compute dEta-separation using best available parameter values.
  template<class U, class V>
  float
  comp_deltaEta(const U* p1, const V* p2) {
    return eta(p1) - eta(p2);
  }

// Function to compute dR-separation using best available parameter values.
  template<class U, class V>
  float
  comp_deltaR(const U* p1, const V* p2) {
    return sqrt(pow(comp_deltaPhi(p1, p2), 2.) + pow(comp_deltaEta(p1, p2), 2.));
  }

// Function for sorting vector of xAOD particles by increasing pT.
  template<class U>
  bool
  sort_pt(const U* p1, const U* p2) {
    return pt(p1) < pt(p2);
  }

// Function for sorting vector of xAOD particles by increasing eta.
  template<class U>
  bool
  sort_eta(const U* p1, const U* p2) {
    return eta(p1) < eta(p2);
  }

// Function for sorting vector of xAOD particles by increasing phi.
  template<class U>
  bool
  sort_phi(const U* p1, const U* p2) {
    return phi(p1) < phi(p2);
  }
} // namespace


dRMatchingTool::dRMatchingTool(const std::string& name) :
  asg::AsgTool(name),
  m_accept("dRMatching"),
  m_numProcessed(0),
  m_numPassed(0) {
  declareInterface<IAsgSelectionTool>(this);

  // Decleare cut properties, for access in job option.
  declareProperty("dRmax", m_dRmax = -1);
  declareProperty("pTResMax", m_pTResMax = -1);
}

dRMatchingTool::~dRMatchingTool() {
}

StatusCode
dRMatchingTool::initialize() {
  if (AlgTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Initializing " << name() << "...");

  // Clear cuts container.
  m_cuts.clear();

  // Define cut names and descriptions.
  if (m_dRmax > -1) {
    m_cuts.push_back(std::make_pair("dRmax", "Cut on maximal dR between track and truth particle."));
  }
  if (m_pTResMax > -1) {
    m_cuts.push_back(std::make_pair("pTResMax",
                                    "Cut on maximal, relativ pT difference between track and truth particle."));
  }

  // Add cuts to the AcceptOmfp.
  for (const auto& cut : m_cuts) {
    if (m_accept.addCut(cut.first, cut.second) < 0) {
      ATH_MSG_ERROR("Failed to add cut " << cut.first << " because the AcceptInfo object is full.");
      return StatusCode::FAILURE;
    }
  }

  // Initialise counters.
  m_numPassedCuts.resize(m_accept.getNCuts(), 0);

  /**
   * If no cuts are enabled, force to have length one, such that it will always
   * return 'false'. This is because we want the dRMatchingTool to be
   * ineffecitive in this case, and not select _all_ tracks.
   */
  if (m_accept.getNCuts() == 0) {
    m_accept.addCut("nop", "Forcing to have length 1.");
  }

  return StatusCode::SUCCESS;
}

const asg::AcceptInfo&
dRMatchingTool::getAcceptInfo( ) const {
  return m_accept;
}

asg::AcceptData
dRMatchingTool::accept(const xAOD::IParticle* /*p*/) const {

  ATH_MSG_ERROR(
    "accept(...) function called without needed Truth- or TrackParticleContainer. Please use one of the dRMatchingTool-specific accept methods.");

  return asg::AcceptData (&m_accept);
}

template<class T, class U>
void
dRMatchingTool::sortVectors(const T* container,
                            std::vector< const U* >& vec_pt,
                            std::vector< const U* >& vec_eta,
                            std::vector< const U* >& vec_phi,
                            bool (* selectionTool)(const U*)) const {
  // Look all particles in container.
  for (const U* p : *container) {
    // Ignore particles not passing the selection, if applicable.
    if (selectionTool and !(*selectionTool)(p)) {
      continue;
    }

    // Append passing particles to cached vectors.
    vec_pt.push_back(p);
    vec_eta.push_back(p);
    vec_phi.push_back(p);
  }

  // Sort vectors.
  std::sort(vec_pt.begin(), vec_pt.end(), sort_pt <U>);
  std::sort(vec_eta.begin(), vec_eta.end(), sort_eta<U>);
  std::sort(vec_phi.begin(), vec_phi.end(), sort_phi<U>);

  return;
}

void
dRMatchingTool::checkCacheTrackParticles(const xAOD::TrackParticleContainer* trackParticles,
                                         bool (* trackSelectionTool)(const xAOD::TrackParticle*)) const {
  // Check whether to cache.
  if (*trackParticles == m_baseTrackContainer) {
    return;
  }

  // Clear existing cache.
  clearTrackParticles();

  // Cache track particles.
  sortVectors<xAOD::TrackParticleContainer,
              xAOD::TrackParticle>(trackParticles,
                                   m_trackParticlesSortedPt,
                                   m_trackParticlesSortedEta,
                                   m_trackParticlesSortedPhi,
                                   trackSelectionTool);

  // Store copy of base track container.
  m_baseTrackContainer = *trackParticles;

  return;
}

void
dRMatchingTool::checkCacheTruthParticles(const xAOD::TruthParticleContainer* truthParticles,
                                         bool (* truthSelectionTool)(const xAOD::TruthParticle*)) const {
  // Check whether to cache.
  if (*truthParticles == m_baseTruthContainer) {
    return;
  }

  // Clear existing cache.
  clearTruthParticles();

  // Cache truth particles.
  sortVectors<xAOD::TruthParticleContainer,
              xAOD::TruthParticle>(truthParticles,
                                   m_truthParticlesSortedPt,
                                   m_truthParticlesSortedEta,
                                   m_truthParticlesSortedPhi,
                                   truthSelectionTool);

  // Store copy of base truth container.
  m_baseTruthContainer = *truthParticles;

  return;
}

template<class U, class V>
bool
dRMatchingTool::sortedMatch(const U* p,
                            std::vector< const V* >& vec_pt,
                            std::vector< const V* >& vec_eta,
                            std::vector< const V* >& vec_phi) const {
  // (Re-)set variables.
  m_dRmin = 9999.;

  // Perform search in cached vectors.
  auto it_pt_lower = m_pTResMax < 0 ? vec_pt.begin() :
                     std::lower_bound(vec_pt.begin(), vec_pt.end(),
                                      pt(p) * (1. - m_pTResMax),
                                      [](const V* o, const float& val) -> bool {
    return pt(o) < val;
  });

  auto it_pt_upper = m_pTResMax < 0 ? vec_pt.end() :
                     std::upper_bound(vec_pt.begin(), vec_pt.end(),
                                      pt(p) * (1. + m_pTResMax),
                                      [](const float& val, const V* o) -> bool {
    return val < pt(o);
  });

  auto it_eta_lower = m_dRmax < 0 ? vec_eta.begin() :
                      std::lower_bound(vec_eta.begin(), vec_eta.end(),
                                       eta(p) - m_dRmax,
                                       [](const V* o, const float& val) -> bool {
    return eta(o) < val;
  });

  auto it_eta_upper = m_dRmax < 0 ? vec_eta.end() :
                      std::upper_bound(vec_eta.begin(), vec_eta.end(),
                                       eta(p) + m_dRmax,
                                       [](const float& val, const V* o) -> bool {
    return val < eta(o);
  });

  // Dealing with cyclic nature of phi: Determining whether phi range wraps
  // around +-pi.
  bool wrapLow = phi(p) - m_dRmax < -pi;
  bool wrapHigh = phi(p) + m_dRmax > pi;
  bool wrap = wrapLow or wrapHigh;

  auto it_phi_lower = m_dRmax < 0 ? vec_phi.begin() :
                      std::lower_bound(vec_phi.begin(), vec_phi.end(),
                                       phi(p) - m_dRmax + (wrapLow ? twopi : 0),
                                       [](const V* o, const float& val) -> bool {
    return phi(o) < val;
  });

  auto it_phi_upper = m_dRmax < 0 ? vec_phi.end() :
                      std::upper_bound(vec_phi.begin(), vec_phi.end(),
                                       phi(p) + m_dRmax + (wrapHigh ? -twopi : 0),
                                       [](const float& val, const V* o) -> bool {
    return val < phi(o);
  });

  // Break early if no truth particles passed selection.
  if (m_pTResMax > 0 and it_pt_upper < it_pt_lower) {
    return false;
  } else if (m_dRmax > 0 and it_eta_upper < it_eta_lower) {
    return false;
  } else if (m_dRmax > 0 and((!wrap and it_phi_upper < it_phi_lower)or
                               (wrap and it_phi_upper > it_phi_lower))) {
    return false;
  }

  // Initialise base set.
  std::vector< const V* > set(vec_pt);

  // -- Sort, pointer-based; necessary for set_intersection.
  std::sort(set.begin(), set.end());

  // Compute subset of selected truth particles.
  std::vector< const V* > subset_pt(it_pt_lower, it_pt_upper);
  std::vector< const V* > subset_eta(it_eta_lower, it_eta_upper);
  std::vector< const V* > subset_phi;
  if (!wrap) {
    subset_phi = std::vector< const V* >(it_phi_lower, it_phi_upper);
  } else {
    subset_phi = std::vector< const V* >(vec_phi.begin(), it_phi_upper);
    subset_phi.insert(subset_phi.end(), it_phi_lower, vec_phi.end());
  }

  // Add subsets according to specified cut values.
  std::vector< std::vector< const V* > > subsets;
  if (m_pTResMax > 0) {
    subsets.push_back(subset_pt);
  }
  if (m_dRmax > 0) {
    subsets.push_back(subset_eta);
    subsets.push_back(subset_phi);
  }

  // Compute successive intersections between base set and subset.
  for (std::vector< const V* > subset : subsets) {
    // -- Sort, pointer-based; necessary for set::set_intersection.
    std::sort(subset.begin(), subset.end());

    // -- Set intersection.
    std::vector< const V* > intersection;
    std::set_intersection(set.begin(), set.end(),
                          subset.begin(), subset.end(),
                          std::back_inserter(intersection));

    // -- Break early if intersection is empty.
    if (intersection.size() == 0) {
      return false;
    }

    set = intersection;
  }

  // If only pT-matching, we're done.
  if (m_dRmax < 0) {
    return set.size() > 0;
  }

  // Otherwise, compute dR for all remaining particles.
  bool passes = false;
  for (const V* other : set) {
    float dR = comp_deltaR(p, other);
    m_dRmin = (dR < m_dRmin ? dR : m_dRmin);
    passes |= m_dRmin < m_dRmax;
  }

  return passes;
}

asg::AcceptData
dRMatchingTool::accept(const xAOD::TrackParticle* track,
                       const xAOD::TruthParticleContainer* truthParticles,
                       bool (* truthSelectionTool)(const xAOD::TruthParticle*)) const {
  asg::AcceptData acceptData (&m_accept);

  // Determine whether to cache current truth particle container
  checkCacheTruthParticles(truthParticles, truthSelectionTool);

  bool passes = sortedMatch<xAOD::TrackParticle,
                            xAOD::TruthParticle>(track,
                                                 m_truthParticlesSortedPt,
                                                 m_truthParticlesSortedEta,
                                                 m_truthParticlesSortedPhi);

  // Set cut values.
  if (m_dRmax > -1) {
    acceptData.setCutResult("dRmax", passes);
  }
  if (m_pTResMax > -1) {
    acceptData.setCutResult("pTResMax", passes);
  }

  // Book keep cuts
  for (const auto& cut : m_cuts) {
    unsigned int pos = acceptData.getCutPosition(cut.first);
    if (acceptData.getCutResult(pos)) {
      m_numPassedCuts[pos]++;
    }
  }

  m_numProcessed++;
  if (acceptData) {
    m_numPassed++;
  }

  return acceptData;
}

asg::AcceptData
dRMatchingTool::accept(const xAOD::TruthParticle* truth,
                       const xAOD::TrackParticleContainer* trackParticles,
                       bool (* trackSelectionTool)(const xAOD::TrackParticle*)) const {
  asg::AcceptData acceptData (&m_accept);

  // Determine whether to cache current track particle container
  checkCacheTrackParticles(trackParticles, trackSelectionTool);

  bool passes = sortedMatch<xAOD::TruthParticle,
                            xAOD::TrackParticle>(truth,
                                                 m_trackParticlesSortedPt,
                                                 m_trackParticlesSortedEta,
                                                 m_trackParticlesSortedPhi);

  // Set cut values.
  if (m_dRmax > -1) {
    acceptData.setCutResult("dRmax", passes);
  }
  if (m_pTResMax > -1) {
    acceptData.setCutResult("pTResMax", passes);
  }

  // Book keep cuts
  for (const auto& cut : m_cuts) {
    unsigned int pos = acceptData.getCutPosition(cut.first);
    if (acceptData.getCutResult(pos)) {
      m_numPassedCuts[pos]++;
    }
  }

  m_numProcessed++;
  if (acceptData) {
    m_numPassed++;
  }

  return acceptData;
}

asg::AcceptData
dRMatchingTool::acceptLegacy(const xAOD::TrackParticle* p,
                             const xAOD::TruthParticleContainer* truthParticles,
                             bool (* truthSelectionTool)(const xAOD::TruthParticle*)) const {
  asg::AcceptData acceptData (&m_accept);
  m_dRmin = 9999.;

  // Define variables.
  const unsigned int Ncuts(m_cuts.size());
  bool passes(false), passesThis(false);

  // Loop all truth particles.
  for (const xAOD::TruthParticle* truth : *truthParticles) {
    // Ignore all truth particles failing the selection.
    if (truthSelectionTool and !(*truthSelectionTool)(truth)) {
      continue;
    }

    // Compute cut variable values.
    float dR = comp_deltaR(p, truth);
    float pTRes = std::fabs(pt(truth) / pt(p) - 1.);

    // Initialise cut monitoring objects.
    std::vector<bool> vecPassesThis(Ncuts, false);

    // Check whether each individual cut passed.
    unsigned int icut = 0;
    if (m_dRmax > -1) {
      vecPassesThis[icut++] = dR < m_dRmax;
    }
    if (m_pTResMax > -1) {
      vecPassesThis[icut++] = pTRes < m_pTResMax;
    }

    // Check whether all cuts passed.
    passesThis = std::all_of(vecPassesThis.begin(),
                             vecPassesThis.end(),
                             [](const bool& v) {
      return v;
    });
    passes |= passesThis;

    // If the current truth particle was matched, check minimal dR.
    if (passesThis) {
      m_dRmin = (dR < m_dRmin ? dR : m_dRmin);
    }
  }

  // Set cut values.
  if (m_dRmax > -1) {
    acceptData.setCutResult("dRmax", passes);
  }
  if (m_pTResMax > -1) {
    acceptData.setCutResult("pTResMax", passes);
  }

  // Book keep cuts
  for (const auto& cut : m_cuts) {
    unsigned int pos = acceptData.getCutPosition(cut.first);
    if (acceptData.getCutResult(pos)) {
      m_numPassedCuts[pos]++;
    }
  }

  m_numProcessed++;
  if (acceptData) {
    m_numPassed++;
  }

  return acceptData;
}

asg::AcceptData
dRMatchingTool::acceptLegacy(const xAOD::TruthParticle* p,
                             const xAOD::TrackParticleContainer* trackParticles,
                             bool (* trackSelectionTool)(const xAOD::TrackParticle*)) const {
  // Reset the results.
  asg::AcceptData acceptData (&m_accept);
  m_dRmin = 9999.;

  // Define variables.
  const unsigned int Ncuts(m_cuts.size());
  bool passes(false), passesThis(false);

  // Loop all track particles.
  for (const xAOD::TrackParticle* track : *trackParticles) {
    // Ignore all tracks failing the selection.
    if (trackSelectionTool and !(*trackSelectionTool)(track)) {
      continue;
    }

    // Compute cut variable values.
    float dR = comp_deltaR(p, track);
    float pTRes = std::fabs(pt(track) / pt(p) - 1.);

    // Initialise cut monitoring objects.
    std::vector<bool> vecPassesThis(Ncuts, false);

    // Check whether each individual cut passed.
    unsigned int icut = 0;
    if (m_dRmax > -1) {
      vecPassesThis[icut++] = dR < m_dRmax;
    }
    if (m_pTResMax > -1) {
      vecPassesThis[icut++] = pTRes < m_pTResMax;
    }

    // Check whether all cuts passed.
    passesThis = std::all_of(vecPassesThis.begin(),
                             vecPassesThis.end(),
                             [](const bool& v) {
      return v;
    });
    passes |= passesThis;

    // If the current track particle was matched, check minimal dR.
    if (passesThis) {
      m_dRmin = (dR < m_dRmin ? dR : m_dRmin);
    }
  }

  // Set cut values.
  if (m_dRmax > -1) {
    acceptData.setCutResult("dRmax", passes);
  }
  if (m_pTResMax > -1) {
    acceptData.setCutResult("pTResMax", passes);
  }

  // Book keep cuts
  for (const auto& cut : m_cuts) {
    unsigned int pos = acceptData.getCutPosition(cut.first);
    if (acceptData.getCutResult(pos)) {
      m_numPassedCuts[pos]++;
    }
  }

  m_numProcessed++;
  if (acceptData) {
    m_numPassed++;
  }

  return acceptData;
}

StatusCode
dRMatchingTool::finalize() {
  ATH_MSG_INFO("Finalizing " << name() << "...");

  if (m_numProcessed == 0) {
    ATH_MSG_INFO("No tracks processed in selection tool.");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_INFO(m_numPassed << " / " << m_numProcessed << " = "
                           << m_numPassed * 100. / m_numProcessed
                           << "% passed all cuts.");
  for (const auto& cut : m_cuts) {
    ULong64_t numPassed = m_numPassedCuts.at(m_accept.getCutPosition(cut.first));
    ATH_MSG_INFO(numPassed << " = " << numPassed * 100. / m_numProcessed
                           << "% passed " << cut.first << " cut.");
  }

  return StatusCode::SUCCESS;
}

float
dRMatchingTool::dRmin() const {
  return m_dRmin;
}
