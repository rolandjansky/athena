/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//  StringUtils.h
//  L1TopoCommon
//  Created by Christian Ohm

#ifndef L1TOPOCOMMON_STRINGUTILS_H
#define L1TOPOCOMMON_STRINGUTILS_H

// this file contains the following helper functions

/*
 1) binary representation of data
    * get_binrep
    * get_binrep_vector
    * get_binrep_boolvec

 2) join(vec<str>,char) similar to the python str.join function 

*/

#include <sstream>
#include <bitset>
#include <algorithm>
#include <vector>
#include <limits.h>

namespace TCS {
  
   template <class T>
   inline std::string to_string (const T& t, int precision = -999, int width = -999, bool scientific = false) {
      std::stringstream ss;
      if (precision != -999) {
         ss.precision(precision);
      }
      if (width != -999) {
         ss.width(width);
      }
      if (scientific) {
         ss.setf(std::ios::scientific);
      }
      else if (precision != -999)
         ss.setf(std::ios::fixed);
      ss << t;
      return ss.str();
   }
  
   template<typename T>
   inline std::string get_binrep(const T& a) {
      std::stringstream ss;
      const char* beg = reinterpret_cast<const char*>(&a);
      const char* end = beg + sizeof(a);
      while(beg != end)
         ss << std::bitset<CHAR_BIT>(*beg++);
      return ss.str();
   }
  
   template<typename T>
   inline std::string get_binrep_vector(const T& a) {
      std::stringstream ss;
      for (unsigned int i = 0; i < a.size(); ++i) {
         ss << get_binrep(a.at(i));
      }
      return ss.str();
   }
  
   // for vector<bool> specifically (don't want to convert to 1 char per bit)...
   inline std::string get_binrep_boolvec(const std::vector<bool>& a) {
      std::stringstream ss;
      for (std::vector<bool>::const_iterator it = a.begin(); it != a.end(); ++it)
         ss << ((*it) ? 1 : 0);
      return ss.str();
   }

   std::string join(const std::vector<std::string>& v, const char c=',');
  
} // end of namespace TCS

#endif // not L1TOPOCOMMON_STRINGUTILS_H
