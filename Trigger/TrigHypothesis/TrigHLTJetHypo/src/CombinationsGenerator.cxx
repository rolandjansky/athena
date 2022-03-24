/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "./CombinationsGenerator.h"

std::ostream& operator << (std::ostream& os, const CombinationsGenerator& cg){
  os << "CombinationsGenerator m_invalid " <<std::boolalpha << cg.m_invalid
     << " bitmask len " << cg.m_bitmask.size() 
       << " m_bitmask: ";
  for (const auto& c : cg.m_bitmask) {
    if (c == 0) {
      os << 0 << " ";
    } else if (c == 1) {
      os << 1 << " ";
    } else {
      os << '?' << " ";
    }
  }
  os << " m_N " << cg.m_N << " m_K " << cg.m_K;

  os << '\n';
  return os;
}
