/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file
 *
 * Common base class and generic overlaying code for boolean-like hits.
 * Multi hit overlay n1 hits + n hits -> m hits
 *
 * @author Tadej Novak
 * @author Ketevi A. Assamagan, 2009
 */

#ifndef IDC_MULTIHITOVERLAYCOMMON_H
#define IDC_MULTIHITOVERLAYCOMMON_H

#include <vector>

namespace Overlay
{

/**
 *  Merging of hits on the same channel.  Modifies the first argument by adding
 data from the second..
 *  Implementations can assume that the arguments have the same Id.  (That is,
 correspond to the same readout channel.)
 *
 *  A generic implementation of this template is provided in this package,
 *  but subdetectors can provide specializations suitable for their RDO type.
 *
 *  As a Collection is usually based on the DataVector elements are not const.
 */
template <class Datum>
void mergeMultiHits(const std::vector<Datum *> &bkgHits,
                    const std::vector<Datum *> &signalHits,
                    std::vector<Datum *> &outputHits);

template <class Collection>
void mergeMultiHitCollections(Collection *bkgCollection,
                              Collection *signalCollection,
                              Collection *outputCollection);
} // namespace Overlay

#include "MuonOverlayBase/IDC_MultiHitOverlayCommon.icc"

#endif
