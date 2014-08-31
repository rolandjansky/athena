/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// \file HLTConversions
/// \author Gianluca Comune <gianluca.comune@cern.ch>
///

#ifndef HLTCONVERSIONS_H
#define HLTCONVERSIONS_H
#include <vector>
///
/// \class HLTConversions
/// \brief Class from which HLT algorithms can import some static functions to do basic convertion
///

#include <iostream>
#include <stdint.h>

class HLTConversions {
  
public:
  HLTConversions();
  ~HLTConversions();

  static uint32_t f2i(float fltValue){
    uint32_t* i=(uint32_t*) &fltValue;
    return (*i);
  };

  static float i2f(uint32_t intValue){
    float* f =(float*) &intValue;
    return (*f);
  };

  static std::vector<uint32_t> d2i(double& dblValue){
    uint32_t* i=(uint32_t*) &dblValue;
    std::vector<uint32_t> vec;
    vec.push_back(*i);
    i++;
    vec.push_back(*i); 
    return vec;
  }       

  static double i2d(uint32_t int1Value, uint32_t int2Value){
    unsigned long long int msblongValue=int2Value;
    unsigned long long int totlongValue=int1Value+(msblongValue<<32);
    double* d=(double*) &totlongValue;
    return (*d);
  }       

};

#endif 
