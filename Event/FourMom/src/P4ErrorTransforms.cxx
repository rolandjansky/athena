/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/P4ErrorTransforms.h"
#include "FourMom/ErrorMatrixPxPyPzE.h"
#include "FourMom/ErrorMatrixEEtaPhiM.h"
#include "FourMom/P4JacobianPxPyPzE2EEtaPhiM.h"
#include "FourMom/P4JacobianEEtaPhiM2PxPyPzE.h"

namespace P4ErrorTransforms {

  ErrorMatrixEEtaPhiM* toEEtaPhiM( const ErrorMatrixPxPyPzE& em,
				   double px, double py, double pz, double E)
  {
    P4JacobianPxPyPzE2EEtaPhiM J( px, py, pz, E);
    CLHEP::HepSymMatrix res(4);
    res = em.hsm().similarity(J);
    return new ErrorMatrixEEtaPhiM(res);
  }

  ErrorMatrixPxPyPzE* toPxPyPzE( const ErrorMatrixEEtaPhiM& em,
				 double E, double eta, double phi, double M)
  {
    P4JacobianEEtaPhiM2PxPyPzE J( E, eta, phi, M);
    CLHEP::HepSymMatrix res(4);
    res = em.hsm().similarity(J);
    return new ErrorMatrixPxPyPzE(res);
  }


}
