/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PythiaExo_i/Pytcsm.h"
#include <iostream>

Pytcsm::PYTCSM* Pytcsm::s_pytcsm = 0;

Pytcsm::Pytcsm() 
  : 
  m_dummy(-999)
  , m_realdummy(-999.0) 
{
  init();
}

Pytcsm::~Pytcsm()
{
}

int& Pytcsm::itcm(int n) {
  if (n<0 || n>=s_lenItcm) {
    std::cout
      << "Pytcsm: attempt to read or write ITCM out of bounds" << std::endl;
    m_dummy=-999;
    return m_dummy;
  } else {
    return s_pytcsm->itcm[n]; // note the lack of a ``-1''. This is intentional.
  }
}

double& Pytcsm::rtcm(int n) {
  if (n<0 || n>=s_lenRtcm) {
    std::cout
      << "Pytcsm: attempt to read or write RTCM out of bounds" << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pytcsm->rtcm[n]; // note the lack of a ``-1''. This is intentional.
  }
}
