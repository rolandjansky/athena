/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EigenP4JacobianEEtaPhiM2PxPyPzE_H
#define EigenP4JacobianEEtaPhiM2PxPyPzE_H

#include "EventPrimitives/EventPrimitives.h"

class EigenP4JacobianEEtaPhiM2PxPyPzE : public AmgMatrix(4,4) {
public:
  EigenP4JacobianEEtaPhiM2PxPyPzE( double E, double eta, double phi, double M);
  ~EigenP4JacobianEEtaPhiM2PxPyPzE(){}
};

#endif
