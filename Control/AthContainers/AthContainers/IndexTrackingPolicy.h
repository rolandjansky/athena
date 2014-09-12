// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IndexTrackingPolicy.h 581165 2014-02-03 10:42:54Z krasznaa $
/**
 * @file IndexTrackingPolicy.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2013
 * @brief Describes the possible index tracking policies.
 *
 * If the payload of a container does not derive from @c SG::AuxElement,
 * then the container does not track indices (and thus cannot have
 * associated aux data).  If it does, whether a container will track
 * indices (and potentially have aux data) is governed by both
 * the ownership policy and the index tracking policy.  The default
 * value for the index tracking policy is DEFAULT_TRACK_INDICES.  In that
 * case, we track indices if this container owns its elements
 * (OWN_ELEMENTS) and we don't if the container does not down its
 * elements (VIEW_ELEMENTS).  This is the usual case.  However,
 * the index tracking policy may also be set to ALWAYS_TRACK_INDICES
 * or NEVER_TRACK_INDICES to override this.  (The major foreseen use case
 * for this is to allow index tracking for a view container populated
 * with objects allocated from a @c DataPool.)
 */


#ifndef ATHCONTAINERS_INDEXTRACKINGPOLICY_H
#define ATHCONTAINERS_INDEXTRACKINGPOLICY_H


namespace SG {


enum IndexTrackingPolicy {
  /// Default value.  Set index tracking based on the ownership policy.
  /// If this container owns its elements, then track indices; otherwise do not.
  DEFAULT_TRACK_INDICES,

  /// Always track indices, regardless of the setting of the ownership policy.
  ALWAYS_TRACK_INDICES,

  /// Never track indices, regardless of the setting of the ownership policy.
  NEVER_TRACK_INDICES
};


} // namespace SG


#endif // not ATHCONTAINERS_INDEXTRACKINGPOLICY_H
