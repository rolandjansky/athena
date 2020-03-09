/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_LORENTZVECTORFACTORY_H
#define TRIGHLTJETHYPOUNITTESTS_LORENTZVECTORFACTORY_H

#include "./ITLorentzVectorFactory.h"
#include <TLorentzVector.h>
#include <cmath>
#include <stdexcept>

class TLorentzVectorFactory: public ITLorentzVectorFactory{
 public:
  virtual TLorentzVector make(double eta, double et) const override{
    if(et <= 0.) {throw std::out_of_range("Et < 0");}

    double ttOn2 = std::exp(-eta); //tan(theta/2)
    double tt = 2.*ttOn2/(1.- ttOn2*ttOn2);  // tan(theta)
    double ez = et/tt;
    TLorentzVector tl;
    tl.SetXYZM(et, 0., ez, 0.);
    
    return tl;
  }
};
#endif
