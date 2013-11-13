/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef P4JacobianEEtaPhiM2PxPyPzE_H
#define P4JacobianEEtaPhiM2PxPyPzE_H

#include "CLHEP/Matrix/Matrix.h"

class P4JacobianEEtaPhiM2PxPyPzE : public CLHEP::HepMatrix {
public:

  P4JacobianEEtaPhiM2PxPyPzE( double E, double eta, double phi, double M);

};

#endif
