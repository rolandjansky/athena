/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserHooksFactory.h"

#include <stdexcept>

namespace Pythia8_UserHooks{
 
  UserHooks *UserHooksFactory::create(const string &name){
    std::map<string, const ICreator*>::const_iterator it = s_creators().find(name);
    if(it == s_creators().end()){
      //eek!
      throw std::runtime_error("Pythia 8 UserHooksFactory: cannot create user hook " + name);
    }
    return it->second->create();
  }
 
  ///static function to instantiate map of string name Vs. creator object on first use
  std::map<string, const UserHooksFactory::ICreator*> &UserHooksFactory::s_creators(){
    static std::map<string, const UserHooksFactory::ICreator*> creators;
    return creators;
  }
  
}

