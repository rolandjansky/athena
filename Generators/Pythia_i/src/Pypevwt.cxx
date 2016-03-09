/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia_i/Pypevwt.h"
#include <iostream>

Pypevwt::PYPEVWT* Pypevwt::s_pypevwt = 0;

Pypevwt::Pypevwt() 
  : 
  m_dummy(-999)
  , m_realdummy(-999.0) 
{
  init();
}

Pypevwt::~Pypevwt()
{
}

int& Pypevwt::ievwt(int n) {
  if (n<1 || n>=s_lenIevwt+1) {
    std::cout
      << "Pypevwt: attempt to read or write IEVWT out of bounds" << std::endl;
    m_dummy=-999;
    return m_dummy;
  } else {
    return s_pypevwt->ievwt[n-1];
  }
}

double& Pypevwt::revwt(int n) {
  if (n<1 || n>=s_lenRevwt+1) {
    std::cout
      << "Pypevwt: attempt to read or write REVWT out of bounds" << std::endl;
    m_realdummy=-999.0;
    return m_realdummy;
  } else {
    return s_pypevwt->revwt[n-1];
  }
}
