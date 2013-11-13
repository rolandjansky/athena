/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P5JACOBIAND0Z0PHITHETAQOVERP2D0Z0PHIETAP_H
#define FOURMOM_P5JACOBIAND0Z0PHITHETAQOVERP2D0Z0PHIETAP_H
/********************************************************************
NAME:     P5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP.h
PACKAGE:  offline/Event/FourMom
AUTHORS:  T. Cuhadar Donszelmann
CREATED:  May 2009
PURPOSE:  Jacobian to transfrom trk perigee parameters (d0,z0,phi,theta,E) to (d0,z0,phi,eta,p)
UPDATED:
********************************************************************/
#include "CLHEP/Matrix/Matrix.h"

class P5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP : public CLHEP::HepMatrix {
    public:
    P5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP(const double phi, const int  charge, const double momentum);
    ~P5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP(){}
};


#endif 

