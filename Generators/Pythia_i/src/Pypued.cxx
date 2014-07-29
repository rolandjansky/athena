/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia_i/Pypued.h"
#include <iostream>

Pypued::PYPUED* Pypued::_pypued = 0;

Pypued::Pypued() 
  : 
  _dummy(-999)
  , _realdummy(-999.0) 
{
  init();
}

Pypued::~Pypued()
{
}

int& Pypued::iued(int n) {
  if (n<0 || n>=_lenIued) {
    std::cout
      << "Pypued: attempt to read or write IUED out of bounds" << std::endl;
    _dummy=-999;
    return _dummy;
  } else {
    return _pypued->iued[n]; // note the lack of a ``-1''. This is intentional.
  }
}

double& Pypued::rued(int n) {
  if (n<0 || n>=_lenRued) {
    std::cout
      << "Pypued: attempt to read or write RUED out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pypued->rued[n]; // note the lack of a ``-1''. This is intentional.
  }
}
