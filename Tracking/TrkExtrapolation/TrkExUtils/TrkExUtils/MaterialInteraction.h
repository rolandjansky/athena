/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  static double dEdl_ionization(double p,
                                const Material* mat,
                                ParticleHypothesis particle,
                                double& sigma,
                                double& kazL);
  /** ionization energy loss from PDG */
  static double PDG_energyLoss_ionization(double p,
                                          const Trk::Material* mat,
                                          Trk::ParticleHypothesis particle,
                                          double& sigma,
                                          double& kazL,
                                          double path);

  /** dE/dl radiation energy loss per path unit */
  static double dEdl_radiation(double p,
                               const Material* mat,
                               ParticleHypothesis particle,
                               double& sigma);

  /** multiple scattering as function of dInX0 */
  static double sigmaMS(double dInX0, double p, double beta);
};
} // namespace Trk

#endif
