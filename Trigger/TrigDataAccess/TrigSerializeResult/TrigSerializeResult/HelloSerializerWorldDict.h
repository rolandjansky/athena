/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_ATHEXHELLOWORLDDICT_H
#define ATHEXHELLOWORLD_ATHEXHELLOWORLDDICT_H
#include "TrigSerializeResult/TestClass.h"
//#include "StoreGate/DataVector.h"
//#include "StoreGate/ElementLink.h"
#include <vector>
#include <iterator>

namespace  {
  std::vector<long>  dvl;
  std::vector<int>   dvi;
  std::vector<float> dvf;
  //
  std::vector<int>::iterator idvi;
  std::vector<float>::iterator idvf;
  //std::char_traits<char>  chchc;
  std::allocator<char>    alll;
  std::basic_string<char, std::char_traits<char>,std::allocator<char> > strstrs;
  //std::vector<TrigCaloCluster>  trp;
  //dummyT<int> dmt;
  //DataVector<TrigInDetTrack>   injdjd;
  //ElementLink<DataVector<TrigInDetTrack> >   bbbdbd;
}

namespace {
  struct _STLAddRflx_Vector_Instances {
    std::vector<bool>               v1;
    std::vector<unsigned char>      v2;
    std::vector<signed char>        v3;
    std::vector<char>               v4;
    std::vector<std::string>        v5;
    std::vector<float>              v12;
    std::vector<double>             v13;
    std::vector<unsigned short int> v6;
    std::vector<signed short int>   v7;
//icc already includes std::vector<unsigned> when instantiating std::vector<bool>
#if not defined (__ICC)
    std::vector<unsigned int>       v8;
#endif
    std::vector<signed int>         v9;
    std::vector<unsigned long int>  v10;
    std::vector<signed long int>    v11;
  };
}



#endif
