/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// JacobianCotThetaPtToThetaP.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTPRIMITIVES_JACOBIANCOTTHETAPTTOTHETAP_H
#define TRKEVENTPRIMITIVES_JACOBIANCOTTHETAPTTOTHETAP_H

// Amg
#include "EventPrimitives/EventPrimitives.h"

namespace Trk {

/** @class JacobianCotThetaPtToThetaP
   
    This is the 5x5 jacobian for the transformation of
    track parameters and errors having a momentum representation
    of @f$ (\phi, cot\theta, q/p_{T}) @f$ to the new standard representation
    of @f$ (\phi, \theta, q/p) @f$, while the first two variables that express
    the local position representation keep unchanged.
   
    Mathematical motivation: <br>
    assuming @f$ u_{\mu} = ( d_{0}', z_{0}', \phi_{0}', cot\theta, q/p_{T} ) @f$
    and @f$ v_{\mu} = (d_{0}, z_{0}, \phi_{0}, \theta, q/p)@f$, the set of @f$ v_{\mu}@f$ parameters
    can be expressed by the set of @f$ u_{\mu} @f$ parameters by following functions:<br>
    - @f$ v_{1} = d_{0} = d_{0}' @f$
    - @f$ v_{2} = z_{0} = z_{0}' @f$
    - @f$ v_{3} = \phi_{0} = \phi_{0}' @f$
    - @f$ v_{4} = \theta = atan(\frac{1}{cot\theta}) @f$
    - @f$ v_{5} = q/p = \frac{q}{p_{T}} \cdot sin(atan(\frac{1}{cot\theta})) @f$
   <br>
    this results into three non-trivial terms of the corresponding jacobian:
    - @f$ \frac{\partial \theta}{\partial cot\theta} = - \frac{1}{1+cot^{2}\theta} @f$
    - @f$ \frac{\partial (q/p)}{\partial cot\theta} = - \frac{cot\theta}{(1+cot^{2}\theta)^{3/2}} \frac{q}{p_{T}}@f$
    - @f$ \frac{\partial (q/p)}{\partial (q/p_{T})} = \frac{1}{sqrt{1+cot^2\theta}} @f$
    
    @author Andreas.Salzburger@cern.ch
   */
  class JacobianCotThetaPtToThetaP : public AmgMatrix(5,5) {

    public:
      JacobianCotThetaPtToThetaP(const double cotTheta, const double qpT);
  };

} // end of namespace

#endif // TRKEVENTPRIMITIVES_JACOBIANCOTTHETAPTTOTHETAP_H


