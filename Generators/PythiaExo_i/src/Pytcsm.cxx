/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PythiaExo_i/Pytcsm.h"
#include <iostream>

Pytcsm::PYTCSM* Pytcsm::_pytcsm = 0;

Pytcsm::Pytcsm() 
  : 
  _dummy(-999)
  , _realdummy(-999.0) 
{
  init();
}

Pytcsm::~Pytcsm()
{
}

int& Pytcsm::itcm(int n) {
  if (n<0 || n>=_lenItcm) {
    std::cout
      << "Pytcsm: attempt to read or write ITCM out of bounds" << std::endl;
    _dummy=-999;
    return _dummy;
  } else {
    return _pytcsm->itcm[n]; // note the lack of a ``-1''. This is intentional.
  }
}

double& Pytcsm::rtcm(int n) {
  if (n<0 || n>=_lenRtcm) {
    std::cout
      << "Pytcsm: attempt to read or write RTCM out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pytcsm->rtcm[n]; // note the lack of a ``-1''. This is intentional.
  }
}
