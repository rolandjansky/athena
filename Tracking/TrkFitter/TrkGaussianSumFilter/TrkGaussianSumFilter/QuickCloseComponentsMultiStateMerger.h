/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      QuickCloseComponentsMultiStateMerger.h  -  description
      -------------------------------------------------
begin                : Wednesday 3rd September 2008
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Class for merging components of a multi-state based on
                       combination of those which are "close" together as
                       defined by some metric ... but faster
*********************************************************************************/

#ifndef TrkQuickCloseComponentsMultiStateMerger_H
#define TrkQuickCloseComponentsMultiStateMerger_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"

namespace Trk {
class IMultiComponentStateCombiner;
class TrackStateOnSurface;
namespace QuickCloseComponentsMultiStateMerger {
/** Method for merging components - ownership of objects is passed */
MultiComponentState
merge(Trk::MultiComponentState&&, const unsigned int maximumNumberOfComponents);

MultiComponentState
mergeFullDistArray(MultiComponentStateAssembler::Cache& cache,
                   Trk::MultiComponentState&,
                   const unsigned int maximumNumberOfComponents);
}

}

#endif
