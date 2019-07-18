/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_ITLORENTZVECTORFACTORY_H
#define TRIGHLTJETHYPOUNITTESTS_ITLORENTZVECTORFACTORY_H

#include <TLorentzVector.h>

class ITLorentzVectorFactory{
 public:
  virtual TLorentzVector make(double, double) const = 0;
};
#endif
