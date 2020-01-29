/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_ITLORENTZVECTORFACTORY_H
#define TRIGHLTJETHYPOUNITTESTS_ITLORENTZVECTORFACTORY_H

#include <TLorentzVector.h>

class ITLorentzVectorFactory{
 public:
  virtual ~ITLorentzVectorFactory() = default;
  virtual TLorentzVector make(double, double) const = 0;
};
#endif
