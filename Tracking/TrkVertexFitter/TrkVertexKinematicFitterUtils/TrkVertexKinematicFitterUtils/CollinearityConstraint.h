/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexKinematicFitterUtils_CollinearityConstraint_h 
#define TrkVertexKinematicFitterUtils_CollinearityConstraint_h 

// This object defines a Collinearity constraint to be used with the constrained kinematic vertex fitter
// It minimizes the angles between the tracks (phi in the x-y plane and theta in the z-plane) using scalar products

#include "TrkVertexFitterInterfaces/IKinematicConstraint.h"

namespace Trk {
class CollinearityConstraint : public IKinematicConstraint
{
public:


  CollinearityConstraint() = default;
  virtual ~CollinearityConstraint() = default;

  virtual Amg::VectorX vectorOfValues(
    std::vector<Amg::VectorX>& cart_coordList,
    std::vector<int>& charges,
    Amg::Vector3D refPoint,
    double b_fieldTesla) const override final;

  virtual Amg::MatrixX matrixOfDerivatives(
    std::vector<Amg::VectorX>& cart_coordList,
    std::vector<int>& charges,
    Amg::Vector3D refPoint,
    double b_fieldTesla) const override final;

  virtual int numberOfEquations() const override final { return s_eqno; }

private:
  static constexpr int s_eqno = 2; // number of constraint-equations used
};
}

#endif //TrkVertexKinematicFitterUtils_CollinearityConstraint_h 
