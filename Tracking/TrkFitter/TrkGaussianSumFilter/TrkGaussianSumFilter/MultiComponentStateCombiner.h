/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************************************
      MultiComponentStateCombiner.h  -  description
      -----------------------------------------------
begin                : Monday 20th December 2004
authors               : atkinson,morley,anastopoulos
description          : These methods take a multi-component state and collapses
                       all components returning a single set of track
                       parameters with single mean and covariance matrix.
*******************************************************************************/

#ifndef MultiComponentStateCombiner_H
#define MultiComponentStateCombiner_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

namespace Trk {

namespace MultiComponentStateCombiner {

/** Calculate combined state of many components */
std::unique_ptr<Trk::TrackParameters>
combine(const MultiComponentState&, const bool useMode = false, const double fractionPDFused = 1.0);

/** Combined/merge a component to another one */
void
combineWithWeight(Trk::ComponentParameters& mergeTo, const Trk::ComponentParameters& addThis);

/** Calculate combined state and weight of many components */
std::unique_ptr<Trk::ComponentParameters>
combineWithWeight(const MultiComponentState&, const bool useMode = false, const double fractionPDFused = 1.0);

std::unique_ptr<Trk::ComponentParameters>
compute(const MultiComponentState*, const bool useMode = false, const double fractionPDFused = 1.0);
}
} // end Trk namespace

#endif
