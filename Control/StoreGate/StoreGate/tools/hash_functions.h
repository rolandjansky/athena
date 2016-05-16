/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <functional>
#include <string>

namespace SG {
  ///simple hash function derived from Sedgewick Algorithms in C++ 3rd ed
  ///@param str    the string to be hashed
  ///@param maxInt the maximum value the function returns. 
  ///              Hash collision probability is 1/min(maxInt, 2**127)
  inline int simpleStringHash(std::string str, int maxInt=0xFFFF) {
    int h(0), a(127);
    for (std::string::size_type iC=0; iC<str.size(); iC++)
      h = (a*h + str[iC]) % maxInt;    
    return h;  
  }

}

#endif



