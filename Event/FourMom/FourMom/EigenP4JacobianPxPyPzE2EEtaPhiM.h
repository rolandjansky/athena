/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EigenP4JacobianPxPyPzE2EEtaPhiM_H
#define EigenP4JacobianPxPyPzE2EEtaPhiM_H

#include "EventPrimitives/EventPrimitives.h"

class EigenP4JacobianPxPyPzE2EEtaPhiM : public AmgMatrix(4,4) {
public:

  EigenP4JacobianPxPyPzE2EEtaPhiM( double px, double py, double pz, double E);
  EigenP4JacobianPxPyPzE2EEtaPhiM() {}

};

#endif
