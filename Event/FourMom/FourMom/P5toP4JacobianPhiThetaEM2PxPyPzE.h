/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P5TOP4JACOBIANPHITHETAEM2PXPYPZE_H
#define FOURMOM_P5TOP4JACOBIANPHITHETAEM2PXPYPZE_H
/**
 * @author Tulay Cuhadar Donszelmann
 * @tcuhadar@cern.ch
 * @date March 2009
 * @Package offline/Event/FourMom/
 * @class P5toP4JacobianPhiThetaEM2PxPyPzE
 * @brief Jacobian to transfrom trk perigee parameters (d0,z0,phi,theta,E) to (px,py,pz,E)
 */



#include "CLHEP/Matrix/Matrix.h"
//#include "FourMom/ParamDefs.h"

class P5toP4JacobianPhiThetaEM2PxPyPzE : public CLHEP::HepMatrix {
    public:
    P5toP4JacobianPhiThetaEM2PxPyPzE( const double phi0, const double theta, const double E, const double mass = 105.6583692  );
    ~P5toP4JacobianPhiThetaEM2PxPyPzE(){}
};


#endif 

