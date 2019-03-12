/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
 * @file
 *
 * Overlaying of Identifiable Containers.  Functions common to the
 * single-hit-per-readout-channel-in-an-event case and the
 * possible-multiple-hits-on-a-channel case are declared in this file.
 *
 * @author Tadej Novak
 * @author Andrei Gaponenko <agaponenko@lbl.gov>, 2009
 */

#ifndef IDC_OVERLAYCOMMON_H
#define IDC_OVERLAYCOMMON_H

#include <memory>

#include <Identifier/IdentifierHash.h>

namespace Overlay
{

template <class Collection>
std::unique_ptr<Collection> copyCollection(const IdentifierHash &hashId,
                                           const Collection *collection);

template<class Datum, class Alg>
void mergeChannelData(Datum &baseDatum,
                      const Datum &additionalDatum,
                      Alg *algorithm);

template <class Collection, class Alg>
void mergeCollections(Collection *bkgCollection,
                      Collection *signalCollection,
                      Collection *outputCollection,
                      Alg *algorithm);

} // namespace Overlay

#include "IDC_OverlayBase/IDC_OverlayCommon.icc"

#endif
