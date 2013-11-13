/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef P4JacobianPxPyPzM2PxPyPzE_H
#define P4JacobianPxPyPzM2PxPyPzE_H

#include "CLHEP/Matrix/Matrix.h"

class P4JacobianPxPyPzM2PxPyPzE : public CLHEP::HepMatrix {
public:

  P4JacobianPxPyPzM2PxPyPzE( double px, double py, double pz, double M);

};

#endif
