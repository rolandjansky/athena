/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrkVertexKinematicFitterUtils_CollinearityConstraint_h 
#define TrkVertexKinematicFitterUtils_CollinearityConstraint_h 

// This object defines a Collinearity constraint to be used with the constrained kinematic vertex fitter
// It minimizes the angles between the tracks (phi in the x-y plane and theta in the z-plane) using scalar products

#include "TrkVertexFitterInterfaces/IKinematicConstraint.h"


namespace Trk
{
  class CollinearityConstraint : public IKinematicConstraint
  {
  public:

    CollinearityConstraint() ;
    
    ~CollinearityConstraint();

   virtual const Amg::VectorX vectorOfValues(std::vector<Amg::VectorX> & cart_coordList, std::vector<int> &charges, Amg::Vector3D refPoint, double b_fieldTesla ) const;
   
   virtual const Amg::MatrixX matrixOfDerivatives(std::vector<Amg::VectorX> & cart_coordList, std::vector<int> &charges, Amg::Vector3D refPoint, double b_fieldTesla ) const;
   
   virtual int numberOfEquations() const { return m_eqno; } 

  private:
    
   int m_eqno; // number of constraint-equations used
   };
}

#endif //TrkVertexKinematicFitterUtils_CollinearityConstraint_h 
