/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_EIGENP5JACOBIAND0Z0PHITHETAQOVERP2D0Z0PHIETAP_H
#define FOURMOM_EIGENP5JACOBIAND0Z0PHITHETAQOVERP2D0Z0PHIETAP_H

#include "EventPrimitives/EventPrimitives.h"

class EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP : public AmgMatrix(5,5) {
 public:
  EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP(const double phi, const int  charge, const double momentum);
  ~EigenP5Jacobiand0z0PhiThetaqOverP2d0z0PhiEtaP(){}
};


#endif 

