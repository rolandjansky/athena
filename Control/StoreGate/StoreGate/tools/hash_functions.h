/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H
#ifndef _CPP_FUNCTIONAL
 #include <functional>
#endif
#ifndef _CPP_STRING
 #include <string>
#endif
/* #include <typeinfo> NO MORE */
/* #include "GaudiKernel/System.h" */

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

  //templated helper to generate class ID
  //  template <typename T>
  //  inline int simpleStringHash(int maxInt=0xFFFF) {
  //    return simpleStringHash(System::typeinfoName(typeid(T)),maxInt);  
  //  }

  /** @class SimpleStringHashFunctor Unary function for SGI hashed containers */
  struct SimpleStringHashFunctor : public std::unary_function<std::string, size_t>
  {
    size_t operator()(const std::string& str) const {
      return (size_t)simpleStringHash(str); 
    }
  };
}

#ifdef OS_SOLARIS_2_5
/* #include <rw/stdex/hashmap.h>
 * #define SG_HASH_MAP_TYPE rw_hashmap */ 
#  include <map>
#  define SG_HASH_MAP_TYPE(A,B) std::map<A,B>

#else
#if defined __GNUC__
#   if __GNUC__ > 2
#ifndef __SGI_STL_INTERNAL_HASH_MAP_H
 #     include <ext/hash_map>
#endif
#  define SG_HASH_MAP_TYPE(A,B) __gnu_cxx::hash_map<A,B,SG::SimpleStringHashFunctor>
#   else
#     include <hash_map>
#  define SG_HASH_MAP_TYPE(A,B) std::hash_map<A,B,SG::SimpleStringHashFunctor>
#   endif    /*version*/
#endif /*GNUC*/
#endif /*platform*/




#endif



