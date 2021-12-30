/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDPVM_CachedGetAssocTruth_h
#define IDPVM_CachedGetAssocTruth_h

/**
 * @file CachedGetAssocTruth.h
 * @author shaun roe
 * @date 11 October 2016
 **/

#include <unordered_map>
#include <string>
#include "xAODTruth/TruthParticle.h"
#include "xAODTracking/TrackParticle.h"

///Class to retrieve associated truth from a track, implementing a cached response
namespace IDPVM {
  class CachedGetAssocTruth {
  public:
    CachedGetAssocTruth() = default;
    CachedGetAssocTruth& operator = (CachedGetAssocTruth&&) = default;
    CachedGetAssocTruth(CachedGetAssocTruth&&) = default;
    ~CachedGetAssocTruth() = default;
    ///copy and assignment are not possible
    CachedGetAssocTruth& operator = (const CachedGetAssocTruth&) = delete;
    CachedGetAssocTruth(const CachedGetAssocTruth&) = delete;
    ///clear the cache
    void clear();
    ///Get the associated truth particle, given a track particle
    const xAOD::TruthParticle* operator () (const xAOD::TrackParticle* trackParticle);
    const xAOD::TruthParticle* getTruth(const xAOD::TrackParticle* const trackParticle);
  private:
    ///private cache container; map or unordered_map could be used
    std::unordered_map<const xAOD::TrackParticle*, const xAOD::TruthParticle*> m_cache;
  };
}// end of namespace
#endif
