/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserProcessFactory.h"

#include <stdexcept>

namespace Pythia8_UserProcess{
 
  Sigma2Process *UserProcessFactory::create(const std::string &name){
    std::map<std::string, const ICreator*>::const_iterator it = s_creators().find(name);
    if(it == s_creators().end()){
      //eek!
      throw std::runtime_error("Pythia 8 UserProcessFactory: cannot create user process " + name);
    }
    
    return it->second->create();
  }
  
  ///static function to instantiate map of string name Vs. creator object on first use
  std::map<std::string, const UserProcessFactory::ICreator*> &UserProcessFactory::s_creators(){
    static std::map<std::string, const UserProcessFactory::ICreator*> creators;
    return creators;
  }
  
}

