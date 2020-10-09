/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserResonanceFactory.h"


#include <vector>
#include <stdexcept>

namespace Pythia8_UserResonance{
  
  ResonanceWidths *UserResonanceFactory::create(const std::string &name, int pdgid){

    std::map<std::string, const ICreator*>::const_iterator it = s_creators().find(name);
    if(it == s_creators().end()){
      //eek!
      throw std::runtime_error("Pythia 8 UserResonanceFactory: cannot create user resonance " + name);
    }
    return it->second->create(pdgid);
  }
  
  ///static function to instantiate map of string name Vs. creator object on first use
  std::map<std::string, const UserResonanceFactory::ICreator*> &UserResonanceFactory::s_creators(){
    static std::map<std::string, const UserResonanceFactory::ICreator*> creators;
    return creators;
  }
  
}