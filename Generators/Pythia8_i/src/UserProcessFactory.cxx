/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia8_i/UserProcessFactory.h"

#include <stdexcept>

namespace Pythia8_UserProcess{
 
  Sigma2Process *UserProcessFactory::create(const string &name){
    map<string, const ICreator*>::const_iterator it = s_creators().find(name);
    if(it == s_creators().end()){
      //eek!
      throw std::runtime_error("Pythia 8 UserProcessFactory: cannot create user process " + name);
    }
    
    return it->second->create();
  }
  
  ///static function to instantiate map of string name Vs. creator object on first use
  map<string, const UserProcessFactory::ICreator*> &UserProcessFactory::s_creators(){
    static map<string, const UserProcessFactory::ICreator*> creators;
    return creators;
  }
  
}

