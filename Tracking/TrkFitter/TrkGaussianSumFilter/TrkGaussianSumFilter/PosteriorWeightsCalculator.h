/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   PosteriorWeightsCalculator.h
 * @date   Friday 25th February 2005
 * @author Tom Atkinson, Anthony Morley, Christos Anastopoulos
 * 
 * Helper to calculate the weighting of state components
 * after an update from a measurement. The information for
 * the measurement is provided through the RIO_OnTrack
 * object.
 */

#ifndef PosteriorWeightsCalculator_H
#define PosteriorWeightsCalculator_H

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"

namespace Trk {

namespace PosteriorWeightsCalculator{
  std::vector<Trk::ComponentParameters> weights(MultiComponentState&&,
                                                const MeasurementBase&);
}

} // end Trk namespace

#endif
