/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia_i/Pymssm.h"
#include <iostream>

Pymssm::PYMSSM* Pymssm::_pymssm = 0;

Pymssm::Pymssm() 
  : 
  _dummy(-999)
  , _realdummy(-999.0) 
{
  init();
}

Pymssm::~Pymssm()
{
}

int& Pymssm::imss(int n) {
  if (n<0 || n>=_lenImss) {
    std::cout
      << "Pymssm: attempt to read or write IMSS out of bounds" << std::endl;
    _dummy=-999;
    return _dummy;
  } else {
    return _pymssm->imss[n]; // note the lack of a ``-1''. This is intentional.
  }
}

double& Pymssm::rmss(int n) {
  if (n<0 || n>=_lenRmss) {
    std::cout
      << "Pymssm: attempt to read or write RMSS out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pymssm->rmss[n]; // note the lack of a ``-1''. This is intentional.
  }
}
