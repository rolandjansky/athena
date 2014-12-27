/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHadronPythiaDecayer_H
#define RHadronPythiaDecayer_H

//Use this to override the G4DecayProducts class and use up to 2000 decay products
//#define USEG4DECAYPRODUCTS2

#ifdef USEG4DECAYPRODUCTS2
#include "RHadrons/G4DecayProducts2.h"
#else
#include "G4DecayProducts.hh"
#endif

#include "G4VExtDecayer.hh"
#include "G4Track.hh"
#include <string>

class RHadronPythiaDecayer: public G4VExtDecayer
{
  public:
   RHadronPythiaDecayer( const std::string s) : G4VExtDecayer(s) {}
   virtual G4DecayProducts* ImportDecayProducts(const G4Track&);
  private:
};

#endif
