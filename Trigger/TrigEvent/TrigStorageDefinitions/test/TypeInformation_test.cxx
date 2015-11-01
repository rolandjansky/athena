/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//template<class T>
//struct DataVector{};
class TrigInDetTrack{};

#include "TrigStorageDefinitions/TypeInformation.h"
#include "TrigStorageDefinitions/EDM_TypeInformation.h"
#include "TrigStorageDefinitions/EDM_TypeInfoMethods.h"
#include "TrigStorageDefinitions/EDM_TypeInfo.h"
#include <boost/type_traits.hpp>
#include <iostream>

class TrigInDetTrackCollection : public DataVector<TrigInDetTrack>{};

int main(){
  std::cout << "TypeInformation_test" << std::endl;


  //typedef Object2Container<TrigInDetTrack,TypeInfo_EDM>::type container;

  std::cout << typeid(Object2Container<TrigInDetTrack,TypeInfo_EDM>::type).name() << std::endl;
  std::cout << typeid(Container2Aux<TrigInDetTrackCollection,TypeInfo_EDM>::type).name() << std::endl;

  return 0;
}

//run unit test with cmt make check
