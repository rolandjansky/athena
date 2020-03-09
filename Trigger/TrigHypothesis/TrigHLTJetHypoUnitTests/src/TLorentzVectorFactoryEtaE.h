/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_LORENTZVECTORFACTORYETAE_H
#define TRIGHLTJETHYPOUNITTESTS_LORENTZVECTORFACTORYETAE_H

#include "./ITLorentzVectorFactory.h"
#include <TLorentzVector.h>
#include <cmath>
#include <stdexcept>

class TLorentzVectorFactoryEtaE: public ITLorentzVectorFactory{
 public:
  virtual TLorentzVector make(double eta, double e) const override{
    if(e <= 0.) {throw std::out_of_range("E < 0");}

    double ttOn2 = std::exp(-eta);  //tan(theta/2)
    double tt = 2.*ttOn2/(1.- ttOn2*ttOn2);  // tan(theta)
    double cost = sqrt(1./(1+tt*tt)); // 1+ tan^2(theta) = 1/cos^2(theta)
    double sint = tt*cost;
    TLorentzVector tl;
    tl.SetXYZM(0., e*sint, e*cost, 0.);
    
    return tl;
  }
};
#endif
