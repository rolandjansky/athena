/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexFitterInterfaces_IKinematicConstraint_h
#define TrkVertexFitterInterfaces_IKinematicConstraint_h

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

/**
 * @class IKinematicConstraint
 *
 * Kinematic constraints are implemented as a set of equation that depend on the
 * cartesian coordinates of the kinematic particles. Constraint-equations used
 * by the KinematicVertexFitter are implemented in a general form which
 * contains: a vector with the current values of each constraint-equation for a
 * given set of KinematicParticle and a matrix with the derivatives of the
 * constraint-equations vs the cartesian coordinates of the particles
 *
 * @author Maaike.Limper@cern.ch
 * @author Kirill.Prokofiev@cern.ch
 * March 2008
 */

namespace Trk {

// class KinematicParticle;

class IKinematicConstraint
{

public:
  /*** Virtual destructor */
  virtual ~IKinematicConstraint() = default;

  /*** Actual access methods: */
  virtual Amg::VectorX vectorOfValues(
    std::vector<Amg::VectorX>& cart_coordList,
    std::vector<int>& charges,
    Amg::Vector3D refPoint,
    double b_fieldTesla) const = 0;

  virtual Amg::MatrixX matrixOfDerivatives(
    std::vector<Amg::VectorX>& cart_coordList,
    std::vector<int>& charges,
    Amg::Vector3D refPoint,
    double b_fieldTesla) const = 0;

  virtual int numberOfEquations() const = 0;

}; // end of class definitions
} // end of namespace definitions

#endif // TrkVertexKinematicFitInterfaces_IKinematicConstraint_h
