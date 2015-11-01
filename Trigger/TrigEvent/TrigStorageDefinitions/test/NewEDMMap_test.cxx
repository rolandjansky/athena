/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TrigStorageDefinitions/EDM_TypeInfo.h"
#include "TrigStorageDefinitions/EDM_TypeInfoMethods.h"
#include <string>

struct value_printer{
  template <class element>
  void do_it() const {std::cout << "["<< element::value << "] ";}
};

class ElectronMuonTopoInfoContainer{
};

struct simpletest{
};

int main(){
  std::cout << "we need a new EDM combination.. join makes it too long" << std::endl;

  typedef Object2Container<ElectronMuonTopoInfo>::type type;
  
  std::cout << "the result of the query overall was" << std::endl;
  std::cout << typeid(type).name() << std::endl;

  
  // typedef IsKnownFeature<simpletest>::search_result unknowntype;
  // std::cout << "following should be unkown feature" << std::endl;
  // std::cout << typeid(unknowntype).name() << std::endl;
  // std::cout << "known? " << IsKnownFeature<simpletest>::value << std::endl;

  typedef IsKnownFeature<ElectronMuonTopoInfo>::search_result knowntype;

  std::cout << "following should be known feature" << std::endl;
  std::cout << typeid(knowntype).name() << std::endl;
  std::cout << "known? " << IsKnownFeature<ElectronMuonTopoInfo>::value << std::endl;

  typedef IsKnownFeature<xAOD::TrigElectronContainer>::search_result line;
  std::cout << typeid(line).name() << std::endl;
  std::cout << "known? " << IsKnownFeature<xAOD::TrigElectronContainer>::value << std::endl;

  return 0;
}
