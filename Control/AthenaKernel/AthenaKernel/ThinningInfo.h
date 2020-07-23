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
};


} // namespace SG


#endif // not ATHENAKERNEL_THINNINGINFO_H
