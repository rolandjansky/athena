/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia_i/Pypued.h"
#include <iostream>

Pypued::PYPUED* Pypued::s_pypued = 0;

Pypued::Pypued() 
  : 
  m_dummy(-999)
  , m_realdummy(-999.0) 
{
  init();
}

Pypued::~Pypued()
{
}

int& Pypued::iued(int n) {
  if (n<0 || n>=s_lenIued) {
    std::cout
      << "Pypued: attempt to read or write IUED out of bounds" << std::endl;
    m_dummy=-999;
    return m_dummy;
  } else {
    return s_pypued->iued[n]; // note the lack of a ``-1''. This is intentional.
  }
}

double& Pypued::rued(int n) {
  if (n<0 || n>=s_lenRued) {
    std::cout
      << "Pypued: attempt to read or write RUED out of bounds" << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pypued->rued[n]; // note the lack of a ``-1''. This is intentional.
  }
}
