/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialInteraction.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_MATERIALINTERACTION_H
#define TRKGEOMETRY_MATERIALINTERACTION_H

#include "EventPrimitives/EventPrimitives.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkGeometry/Material.h"

namespace Trk {

/** @class MaterialInteraction

 Collection of parametrizations used in the Tracking realm

 @author sarka.todorova@cern.ch
*/

struct MaterialInteraction
{

  /** dE/dl ionization energy loss per path unit */
  double dEdl_ionization(double p,
                         const Material* mat,
                         ParticleHypothesis particle,
                         double& sigma,
                         double& kazL) const;
  /** ionization energy loss from PDG */
  double PDG_energyLoss_ionization(double p,
                                   const Trk::Material* mat,
                                   Trk::ParticleHypothesis particle,
                                   double& sigma,
                                   double& kazL,
                                   double path) const;

  /** dE/dl radiation energy loss per path unit */
  double dEdl_radiation(double p,
                        const Material* mat,
                        ParticleHypothesis particle,
                        double& sigma) const;

  /** multiple scattering as function of dInX0 */
  double sigmaMS(double dInX0, double p, double beta) const;
};
}

#endif
