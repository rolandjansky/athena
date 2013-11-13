/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EigenP4JacobianPxPyPzM2PxPyPzE_H
#define EigenP4JacobianPxPyPzM2PxPyPzE_H

#include "EventPrimitives/EventPrimitives.h"

class EigenP4JacobianPxPyPzM2PxPyPzE : public AmgMatrix(4,4) {
public:

  EigenP4JacobianPxPyPzM2PxPyPzE( double px, double py, double pz, double M);
  ~EigenP4JacobianPxPyPzM2PxPyPzE(){}
};

#endif
