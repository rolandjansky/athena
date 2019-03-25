/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RHadronPythiaDecayer_H
#define RHadronPythiaDecayer_H

#include "G4VExtDecayer.hh"
#include "G4Track.hh"
#include <string>

class G4DecayProducts;

class RHadronPythiaDecayer: public G4VExtDecayer
{
  public:
   RHadronPythiaDecayer( const std::string s );
   virtual G4DecayProducts* ImportDecayProducts(const G4Track&);
};

#endif
