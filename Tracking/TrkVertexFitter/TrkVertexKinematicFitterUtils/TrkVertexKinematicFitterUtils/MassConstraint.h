/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexKinematicFitterUtils_MassConstraint_h
#define TrkVertexKinematicFitterUtils_MassConstraint_h

// This object defines a MASS constraint to be used with the constrained vertex
// fitter

#include "TrkVertexFitterInterfaces/IKinematicConstraint.h"

namespace Trk {
/** @class MassConstraint
    @brief Concrete implementation of Trk::IKinematicConstraint to formulate
           a mass constraint in kinematic vertex fits.

    The constraint is constructed with a mass value (in MeV), to which the
    invariant mass of the particles emerging from a vertex will be constrained.
    It calculates the values and derivatives for a list of particles,
    given by their parameters and charges.

    @author Maaike Limper
*/

class MassConstraint : public IKinematicConstraint
{
public:
  //! empty constructor
  MassConstraint() = default;

  /** standard constructor
      @param[in] mass Invariant mass in MeV to which particles shall be
     constrained
  */
  MassConstraint(double mass);

  virtual ~MassConstraint() = default;

  /** method returning the vector of parameters values. Method defined through
      the \<nop\>IKinematicConstraint base class
      @param[in] cart_coordList vector of particles represented in cartesian
     frame, that is through the parameters (px, py, pz, E, x, y, z).
      @param[in] charges charge information for the vector of particles
      @param[in] refPoint reference point for cartesian coordinates
      @param[in] b_fieldTesla local solenoidal approximation of field
      @param[out] vector of values to enter the constrained-fit equation
  */
  virtual Amg::VectorX vectorOfValues(
    std::vector<Amg::VectorX>& cart_coordList,
    std::vector<int>& charges,
    Amg::Vector3D refPoint,
    double b_fieldTesla) const override final;

  /** method returning the matrix of derivatives. Method defined through the
      \<nop\>IKinematicConstraint base class
      @param[in] cart_coordList vector of particles represented in cartesian
     frame, that is through the parameters (px, py, pz, E, x, y, z).
      @param[in] charges charge information for the vector of particles
      @param[in] refPoint reference point for cartesian coordinates
      @param[in] b_fieldTesla local solenoidal approximation of field
      @param[out] matrix of derivatives to enter the constrained-fit equation
  */
  virtual Amg::MatrixX matrixOfDerivatives(
    std::vector<Amg::VectorX>& cart_coordList,
    std::vector<int>& charges,
    Amg::Vector3D refPoint,
    double b_fieldTesla) const override final;

  //! tells the number of equations needed for this type of constraint, that is
  //! 1
  virtual int numberOfEquations() const override final { return s_eqno; }

  //! tells the mass value used for the constraint
  double mass() { return m_mass; }

private:
  //! value of mass to be constrained in MeV
  double m_mass = 0.;
  //! number of constraint-equations used by this type of constraint
  static constexpr int s_eqno = 1;
};
}

#endif // TrkVertexKinematicFitterUtils_MassConstraint_h
