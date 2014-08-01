/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PythiaExo_i/Pyssmt.h"
#include <iostream>
Pyssmt::PYSSMT const * Pyssmt::_pyssmt = 0;

const double& Pyssmt::zmix(int i, int j) const {
  if (i<0 || i>_sizeZmix || j<0 || j>_sizeZmix) {
    std::cout 
      << "Pyssmt: attempt to read or write ZMIX out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pyssmt->zmix[j-1][i-1];
  }
}

const double& Pyssmt::umix(int i, int j) const {
  if (i<0 || i>_sizeUmix || j<0 || j>_sizeUmix) {
    std::cout
      << "Pyssmt: attempt to read or write UMIX out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pyssmt->umix[j-1][i-1];
  }
}

const double& Pyssmt::vmix(int i, int j) const {
  if (i<0 || i>_sizeVmix || j<0 || j>_sizeVmix) {
    std::cout
      << "Pyssmt: attempt to read or write VMIX out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pyssmt->vmix[j-1][i-1];
  }
}

const double& Pyssmt::smz(int i) const {
  if (i<0 || i>_lenSmz) {
    std::cout
      << "Pyssmt: attempt to read or write SMZ out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pyssmt->smz[i-1];
  }
}

const double& Pyssmt::smw(int i) const {
  if (i<0 || i>_lenSmw) {
    std::cout
      << "Pyssmt: attempt to read or write SMW out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pyssmt->smw[i-1];
  }
}


