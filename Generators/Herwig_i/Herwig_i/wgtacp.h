/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WGTACP_h
#define WGTACP_h
#include <vector>

extern "C" {
  void* wgtacp_address_();
}

/**
@class      Wgtacp.hh

@brief      Class definition for Wgtacp common block.

@author     Jan Kotanski
*/

class Wgtacp {
public:
  Wgtacp();
  ~Wgtacp();
  
  inline void init(); // inlined for speed of access (small function)
  
  inline double wgtacp(int i);
  inline int userwgts();

private: 
  struct WGTACP;
  friend struct WGTACP;
  
  enum {
    WgtacpSize = 28
  };

  struct WGTACP 
  {
    double wgtacp[WgtacpSize];
    int     userwgts;
 };

  static WGTACP* _wgtacp;
};

#include "Herwig_i/wgtacp.icc"

#endif

