/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MultiComponentState.h
 * @date   Friday 31st December 2004
 * @author atkinson, Anthony Morley, Christos Anastopoulos
 * @brief   Basic definitions for a track state described by more
 * than one set of Track Parameters. The resulting state is
 * a mixture of components.
 * Each component is described by a ComponentParameters
 * object which is of the type std::pair<
 * std::unique_ptr<TrackParameters>, double>.
 * The double describes the weighting of
 * the component - or its relative importance in the mixture.
 */

#ifndef TrkMultiComponentState
#define TrkMultiComponentState

#include "TrkGaussianSumFilterUtils/ComponentParameters.h"
#include <vector>

class MsgStream;
namespace Trk {
/**
 * MultiComponentState is just a typedef
 */
typedef std::vector<ComponentParameters> MultiComponentState;

namespace MultiComponentStateHelpers {

/** Clone TrackParameters method */
MultiComponentState
clone(const MultiComponentState& in);

/** Scale the  covariance matrix components  by
   individual factors.
*/
MultiComponentState
WithScaledError(MultiComponentState&& in,
                double errorScaleLocX,
                double errorScaleLocY,
                double errorScalePhi,
                double errorScaleTheta,
                double errorScaleQoverP);

/** Check to see if all components in the state have measured track parameters
 */
bool
isMeasured(const MultiComponentState& in);

/** Performing renormalisation of total state weighting to one */
void
renormaliseState(MultiComponentState&, double norm = 1);

} // end of MultiComponentStateHelpers
} // end Trk namespace

#endif
