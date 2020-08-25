/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianThetaPToCotThetaPt.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANTHETAPTOCOTTHETAPT_H
#define TRKEVENTPRIMITIVES_JACOBIANTHETAPTOCOTTHETAPT_H

#include "EventPrimitives/EventPrimitives.h"

namespace Trk {
  
  /** @class JacobianThetaPToCotThetaPt
   
    This is the 5x5 jacobian for the transformation of
    track parameters and errors having the new standard momentum representation
    of @f$ (\phi, \theta, q/p) @f$ to the representation
    of @f$ (\phi, cot\theta, q/p_{T}) @f$, while the first two variables that express
    the local position representation keep unchanged.
   
    Mathematical motivation: <br>
    assuming @f$ u_{\mu} = (d_{0}', z_{0}', \phi_{0}', cot\theta, q/p_{T}) @f$
    and @f$ v_{\mu} = (d_{0}, z_{0}, \phi_{0}, \theta, q/p)@f$, the set of @f$ v_{\mu}@f$ parameters
    can be expressed by the set of @f$ u_{\mu} @f$ parameters by following functions:<br>
    - @f$ u_{1} = d_{0}' = d_{0} @f$
    - @f$ u_{2} = z_{0}' = z_{0} @f$
    - @f$ u_{3} = \phi_{0}' = \phi_{0} @f$
    - @f$ u_{4} = cot\theta = cot(\theta) @f$
    - @f$ u_{5} = q/p_{T} = \frac{q}{p \cdot sin(\theta)} @f$
   <br>
    this results in three non-trivial terms of the corresponding jacobian:
    - @f$ \frac{\partial cot\theta}{\partial \theta} = - \frac{1}{sin^{2}\theta} @f$
    - @f$ \frac{\partial (q/p_{T})}{\partial \theta} = - (q/p) \frac{cos(\theta)}{sin^2(\theta)} @f$
    - @f$ \frac{\partial (q/p_{T})}{\partial (q/p)} = 1./sin\theta @f$
   
    @author Andreas.Salzburger@cern.ch
   */
  class JacobianThetaPToCotThetaPt : public AmgMatrix(5,5) {
    public:
      JacobianThetaPToCotThetaPt(const double theta, const double qp);
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANTHETAPTOCOTTHETAPT_H


