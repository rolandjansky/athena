// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaKernel/ThinningInfo.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2020
 * @brief Hold thinning/slimming information for a given object.
 */


#ifndef ATHENAKERNEL_THINNINGINFO_H
#define ATHENAKERNEL_THINNINGINFO_H


#include "CxxUtils/ConcurrentBitset.h"

#include <map>

namespace SG {


class ThinningDecisionBase;


/**
 * @brief Hold thinning/slimming information for a given object.
 *
 * Thinning/slimming information for objects in the event store
 * can come from several sources.  At present, this includes the TrigDecision
 * objects recorded in the event store by thinning algorithms
 * and the list of suppressed aux variables constructed by AthenaOutputStream.
 *
 * ThinningCacheTool will assemble them in one place for ease of access
 * during output.  ThinningCache maps object keys to ThinningInfo objects,
 * when then hold the information for single objects.
 */
class ThinningInfo
{
public:
  /// Thinning for the object.
  const ThinningDecisionBase* m_decision;

  /// Set of auxids not to be written for this object.
  /// (In principle, we should use SG::auxid_set_t here, but there
  /// are dependency issues with that.)
  CxxUtils::ConcurrentBitset m_vetoed;

  /// Test if a variable has been vetoed.
  bool vetoed (size_t id) const
  {
    return m_vetoed.test (id);
  }

  /// This map holds a set of auxids to be lossy compressed
  /// for each compression level (nmantissa).
  /// Same comment above for the SG::auxid_set_t applies here.
  /// Keeping compression level as the key allows us to use
  /// the set as the value, which eases the look up.
  /// Maybe it would be more natural to have auxid => compression level
  /// but that might have complicated things a bit.
  /// If an auxid exists in multiple compression levels,
  /// always pick the most strict one (lowest) for space savings.
  typedef std::map<unsigned int,
                   CxxUtils::ConcurrentBitset> compression_map_t;
  compression_map_t m_compression;

  /// Test if a variable is asked to be compressed.
  /// If it is, return the compression level (nmantissa).
  /// 0 means no compression is asked for.
  unsigned int compression (size_t id) const
  {
    for (const auto &entry : m_compression) {
      if (entry.second.test(id)) return entry.first;
    }
    return 0;
  }
};


} // namespace SG


#endif // not ATHENAKERNEL_THINNINGINFO_H
