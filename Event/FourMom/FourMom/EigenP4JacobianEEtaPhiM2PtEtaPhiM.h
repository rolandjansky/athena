/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EigenP4JacobianEEtaPhiM2PtEtaPhiM_H
#define EigenP4JacobianEEtaPhiM2PtEtaPhiM_H

#include "EventPrimitives/EventPrimitives.h"

class EigenP4JacobianEEtaPhiM2PtEtaPhiM : public AmgMatrix(4,4) {
public:

  EigenP4JacobianEEtaPhiM2PtEtaPhiM( double E, double eta, double M);
  ~EigenP4JacobianEEtaPhiM2PtEtaPhiM(){}
};

#endif
