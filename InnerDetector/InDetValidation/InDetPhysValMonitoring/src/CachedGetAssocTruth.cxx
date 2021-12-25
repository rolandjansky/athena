/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CachedGetAssocTruth.cxx
 * @author shaun roe
 * @date 11 October 2016
 **/

 #include "CachedGetAssocTruth.h"
 #include "xAODTruth/TruthParticleContainer.h"

namespace IDPVM {
  CachedGetAssocTruth::CachedGetAssocTruth() : m_cache{} {
  #ifndef NDEBUG
    m_nCalls = 0;
    m_nCacheHits = 0;
  #endif
    // nop
  }

  void
  CachedGetAssocTruth::clear() {
    #ifndef NDEBUG
    m_nCalls = 0;
    m_nCacheHits = 0;
    #endif
    m_cache.clear();
  }

  const xAOD::TruthParticle*
  CachedGetAssocTruth::getTruth(const xAOD::TrackParticle* trackParticle) {
    #ifndef NDEBUG
    m_nCalls++;
    #endif
    if (not trackParticle) {
      return nullptr;
    }
    auto pCache = m_cache.find(trackParticle);
    if (pCache != m_cache.end()) {
      #ifndef NDEBUG
      m_nCacheHits++;
      #endif
      return pCache->second;
    }
    using ElementTruthLink_t = ElementLink<xAOD::TruthParticleContainer>;
    const xAOD::TruthParticle* result(nullptr);
    // 0. is there any truth?
    if (trackParticle->isAvailable<ElementTruthLink_t>("truthParticleLink")) {
      // 1. ..then get link
      const ElementTruthLink_t ptruthContainer = trackParticle->auxdata<ElementTruthLink_t>("truthParticleLink");
      if (ptruthContainer.isValid()) {
        result = *ptruthContainer;
      }
    }
    m_cache[trackParticle] = result;
    return result;
  }

  const xAOD::TruthParticle*
  CachedGetAssocTruth::operator () (const xAOD::TrackParticle* trackParticle) {
    return getTruth(trackParticle);
  }

  std::string
  CachedGetAssocTruth::report() {
    std::string op("No cache report from 'CachedGetAssocTruth' is available in OPT builds.");
    #ifndef NDEBUG
    op = "\nCache report\nNum. calls = " + std::to_string(m_nCalls);
    op += "\nNum. cache hits = " + std::to_string(m_nCacheHits);
    op += "\nCache size = " + std::to_string(m_cache.size());
    #endif
    return op;
  }
}
