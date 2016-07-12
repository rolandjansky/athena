/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_NtupleSafeCast_h
#define MuonCalib_NtupleSafeCast_h

#include "limits"
#include "cmath"

namespace MuonCalib {

  inline float NtupleSafeCast(const double &dval) {
    if(std::fabs(dval) < std::numeric_limits<float>::max())
      return static_cast<float>(dval);
    float ret=std::numeric_limits<float>::max();
    if(dval<0)
      ret*=-1;
    return ret;
  }
	
} //namespace MuonCalib

#endif
