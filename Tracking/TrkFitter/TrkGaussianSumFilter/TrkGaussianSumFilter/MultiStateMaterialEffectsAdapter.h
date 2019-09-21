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
#include <memory>
// class ISvcLocator;

namespace Trk {

class IGSFMaterialEffects ;

namespace MultiStateMaterialEffectsAdapter {

void
compute(IMultiStateMaterialEffects::Cache&,
        const ToolHandle<IGSFMaterialEffects>&,
        const ComponentParameters&,
        const MaterialProperties&,
        double,
        PropDirection = anyDirection,
        ParticleHypothesis = nonInteracting);

double
extractDeltaP(const TrackParameters& updatedParameters, const TrackParameters& originalParameters);

std::unique_ptr<AmgSymMatrix(5)>
  extractDeltaCovariance(const TrackParameters& updatedParameters, const TrackParameters& originalParameters);

} // end namespace MultiStateMaterialEffectsAdapter

} // end namespace Trk

#endif
