/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_EIGENP5TOP4JACOBIANPHITHETAEM2PXPYPZE_H
#define FOURMOM_EIGENP5TOP4JACOBIANPHITHETAEM2PXPYPZE_H

#include "EventPrimitives/EventPrimitives.h"

class EigenP5toP4JacobianPhiThetaEM2PxPyPzE : public AmgMatrix(4,5) {
    public:
    EigenP5toP4JacobianPhiThetaEM2PxPyPzE( const double phi0, const double theta, const double E, const double mass = 105.6583692  );
    ~EigenP5toP4JacobianPhiThetaEM2PxPyPzE(){}
};


#endif 

