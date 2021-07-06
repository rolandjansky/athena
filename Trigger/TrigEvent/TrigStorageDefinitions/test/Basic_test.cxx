/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigStorageDefinitions/TypeInformation.h"
#include <iostream>
#include <boost/type_traits.hpp>

using namespace HLT::TypeInformation;

struct testX{};
struct testY{};

int main(){
  std::cout << "Basic_test" << std::endl;

  typedef newlist::
    add<testX>::go::
    add<testY>::go::
    done mylist;
  std::cout << "with newlist" << std::endl;
  std::cout << typeid(at<mylist,0>::type).name() << std::endl;
  std::cout << typeid(at<mylist,1>::type).name() << std::endl;
  std::cout << typeid(mylist::get<0>::type).name() << std::endl;
  std::cout << typeid(mylist::get<1>::type).name() << std::endl;

  typedef list<testX,list<testY,nil> > manual_list;
  std::cout << "manual list" << std::endl;
  std::cout << typeid(at<manual_list,0>::type).name() << std::endl;
  std::cout << typeid(at<manual_list,1>::type).name() << std::endl;
  std::cout << typeid(manual_list::get<0>::type).name() << std::endl;
  std::cout << typeid(manual_list::get<1>::type).name() << std::endl;

  typedef list<testY,nil> single_entry_list;
  std::cout << "single entry" << std::endl;
  std::cout << typeid(single_entry_list::get<0>::type).name() << std::endl;

  return 0;
}

//run unit test with cmt make check
