/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECAY3BODY_H
#define DECAY3BODY_H
#include "CLHEP/Vector/LorentzVector.h"

class Decay3Body {
public:
  Decay3Body();
  ~Decay3Body();

  void doDecay(const CLHEP::HepLorentzVector & mother,
               CLHEP::HepLorentzVector & daughter1,
               CLHEP::HepLorentzVector & daughter2,
               CLHEP::HepLorentzVector & daughter3);

private:
  inline double sqr(double a);
};

#endif
