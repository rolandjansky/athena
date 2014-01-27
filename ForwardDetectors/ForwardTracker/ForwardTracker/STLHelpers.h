/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDTRACKER_STLHELPERS_H
#define FORWARDTRACKER_STLHELPERS_H

#include <cmath>
#include <stdexcept>
#include <iostream>

namespace ForwardTracker {

  template <class PtrType> bool absZGreater(const PtrType rhs, const PtrType lhs) { return std::fabs(lhs->position().z()) > std::fabs(rhs->position().z()); }
  
  template <class streamptr> 
    inline void openFile(const std::string& fname, streamptr& strm) {
    
    strm->open(fname.c_str());
    
    if (strm->good()) std::cout << "Open file: " << fname << std::endl;
    else {
      
      std::string msg = "Error opening file " + fname;
      throw std::runtime_error(msg);
    }
  }
}

#endif
