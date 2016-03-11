/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PythiaExo_i/Pyssmt.h"
#include <iostream>
Pyssmt::PYSSMT const * Pyssmt::s_pyssmt = 0;

const double& Pyssmt::zmix(int i, int j) const {
  if (i<0 || i>s_sizeZmix || j<0 || j>s_sizeZmix) {
    std::cout 
      << "Pyssmt: attempt to read or write ZMIX out of bounds" << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->zmix[j-1][i-1];
  }
}

const double& Pyssmt::umix(int i, int j) const {
  if (i<0 || i>s_sizeUmix || j<0 || j>s_sizeUmix) {
    std::cout
      << "Pyssmt: attempt to read or write UMIX out of bounds" << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->umix[j-1][i-1];
  }
}

const double& Pyssmt::vmix(int i, int j) const {
  if (i<0 || i>s_sizeVmix || j<0 || j>s_sizeVmix) {
    std::cout
      << "Pyssmt: attempt to read or write VMIX out of bounds" << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->vmix[j-1][i-1];
  }
}

const double& Pyssmt::smz(int i) const {
  if (i<0 || i>s_lenSmz) {
    std::cout
      << "Pyssmt: attempt to read or write SMZ out of bounds" << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->smz[i-1];
  }
}

const double& Pyssmt::smw(int i) const {
  if (i<0 || i>s_lenSmw) {
    std::cout
      << "Pyssmt: attempt to read or write SMW out of bounds" << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pyssmt->smw[i-1];
  }
}


