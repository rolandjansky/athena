/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPOUNITTESTS_LORENTZVECTORFACTORY_H
#define TRIGHLTJETHYPOUNITTESTS_LORENTZVECTORFACTORY_H

#include <TLorentzVector.h>
#include <cmath>
#include <iostream>
#include <stdexcept>

class TLorentzVectorFactory{
 public:
  TLorentzVector make(double eta, double et){
    if(et <= 0.) {throw std::out_of_range("Et < 0");}

    double ttOn2 = std::exp(-eta);
    double tt = 2.*ttOn2/(1.- ttOn2*ttOn2);
    double ez = et/tt;
    TLorentzVector tl;
    tl.SetXYZM(et, 0., ez, 0.);
    // std::cout << "inputs: eta " << eta << " Et " << et << '\n';
    // std::cout << "outputs: eta " << tl.Eta() << " Et " << tl.Et() << '\n';
    
    return tl;
  }
};
#endif
