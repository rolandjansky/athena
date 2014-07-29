/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia_i/Pypevwt.h"
#include <iostream>

Pypevwt::PYPEVWT* Pypevwt::_pypevwt = 0;

Pypevwt::Pypevwt() 
  : 
  _dummy(-999)
  , _realdummy(-999.0) 
{
  init();
}

Pypevwt::~Pypevwt()
{
}

int& Pypevwt::ievwt(int n) {
  if (n<1 || n>=_lenIevwt+1) {
    std::cout
      << "Pypevwt: attempt to read or write IEVWT out of bounds" << std::endl;
    _dummy=-999;
    return _dummy;
  } else {
    return _pypevwt->ievwt[n-1];
  }
}

double& Pypevwt::revwt(int n) {
  if (n<1 || n>=_lenRevwt+1) {
    std::cout
      << "Pypevwt: attempt to read or write REVWT out of bounds" << std::endl;
    _realdummy=-999.0;
    return _realdummy;
  } else {
    return _pypevwt->revwt[n-1];
  }
}
