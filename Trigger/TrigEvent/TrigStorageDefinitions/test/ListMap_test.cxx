/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigStorageDefinitions/TypeInformation.h"
#include <boost/type_traits.hpp>
#include <iostream>
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

#define LISTSIZE(name)\
std::cout << #name << " size: " << TypeInfo_##name::last_index + 1 << std::endl;


HLT_BEGIN_TYPE_REGISTRATION
     HLT_REGISTER_TYPE(struct ObjectA,struct ObjectA,struct ContainerA)
     HLT_REGISTER_TYPE(struct ObjectB,struct ObjectB,struct ContainerB,struct AuxContainerB)
HLT_END_TYPE_REGISTRATION(Test)


#define TYPEMAPCLASS(name)\
struct class_##name{\
  typedef TypeInfo_##name map;\
  static const std::string package_name;\
};\

TYPEMAPCLASS(Test)

//preemt definitions
#define TRIGSTORAGEDEF_MERGEPACKS

struct TypeInfo_EDM {
typedef  HLT::TypeInformation::newlist
::add<class_Test>  ::go
::done map;
};

#include "TrigStorageDefinitions/EDM_TypeInfo.h"


struct AuxContainerB{};





int main(){
  std::cout << "ListMap_test" << std::endl;
      

  std::cout << typeid(Container2Aux<ContainerB,TypeInfo_EDM>::type).name() << std::endl;
  std::cout << typeid(Container2Aux<ContainerA,TypeInfo_EDM>::type).name() << std::endl;

  

  return 0;
}

//run unit test with cmt make check
