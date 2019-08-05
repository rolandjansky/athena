/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
      MultiStateMaterialEffectsAdapter.h  -  description
      --------------------------------------------------
begin                : Tuesday 22nd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Converter class for single component material effects to
                       multi-component material effects
************************************************************************************/

#ifndef TrkMultiStateMaterialEffectsAdapter_H
#define TrkMultiStateMaterialEffectsAdapter_H

#include "GaudiKernel/ToolHandle.h"
#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"

// class ISvcLocator;

namespace Trk {

class IMaterialEffectsUpdator;

namespace MultiStateMaterialEffectsAdapter {

void
compute(IMultiStateMaterialEffects::Cache&,
        const ToolHandle<IMaterialEffectsUpdator>&,
        const ComponentParameters&,
        const MaterialProperties&,
        double,
        PropDirection = anyDirection,
        ParticleHypothesis = nonInteracting);

double
extractDeltaP(const TrackParameters& updatedParameters, const TrackParameters& originalParameters);

const AmgSymMatrix(5) *
  extractDeltaCovariance(const TrackParameters& updatedParameters, const TrackParameters& originalParameters);

} // end namespace MultiStateMaterialEffectsAdapter

} // end namespace Trk

#endif
