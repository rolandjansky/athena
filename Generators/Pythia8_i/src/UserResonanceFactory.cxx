/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserResonanceFactory.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <vector>
#include <stdexcept>

namespace Pythia8_UserResonance{

  using std::vector;
  
  ResonanceWidths *UserResonanceFactory::create(const string &name, int pdgid){

    map<string, const ICreator*>::const_iterator it = s_creators().find(name);
    if(it == s_creators().end()){
      //eek!
      throw std::runtime_error("Pythia 8 UserResonanceFactory: cannot create user resonance " + name);
    }
    return it->second->create(pdgid);
  }
  
  ///static function to instantiate map of string name Vs. creator object on first use
  map<string, const UserResonanceFactory::ICreator*> &UserResonanceFactory::s_creators(){
    static map<string, const UserResonanceFactory::ICreator*> creators;
    return creators;
  }
  
}
