/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Pythia_i/Pymsrv.h"
#include <iostream>

Pymsrv::PYMSRV* Pymsrv::s_pymsrv = 0;

Pymsrv::Pymsrv() 
  : 
  m_dummy(-999)
  , m_realdummy(-999.0) 
{
  init();
}

Pymsrv::~Pymsrv()
{
}

double& Pymsrv::rvlam(int n) {
  int i1(n/100);
  int i2((n-i1*100)/10);
  int i3(n-i1*100-i2*10);
  if (i1<1 || i1>3 || i2<1 || i2>3 || i3<1 || i3>3) {
    std::cout
      << "Pymsrv: attempt to read or write RVLAM out of bounds" << std::endl;
    m_dummy=-999;
    return m_dummy;
  } else {
    std::cout << "Pymsrv: lambda indices " << i1 << " " << i2 << " " << i3 << std::endl;
    return s_pymsrv->rvlam[i3-1][i2-1][i1-1]; // note ``-1''. CLA, this way the indices work as expected.
  }
}

double& Pymsrv::rvlamp(int n) {
  int i1(n/100);
  int i2((n-i1*100)/10);
  int i3(n-i1*100-i2*10);
  if (i1<1 || i1>3 || i2<1 || i2>3 || i3<1 || i3>3) {
    std::cout
      << "Pymsrv: attempt to read or write RVLAMP out of bounds" << std::endl;
    m_dummy=-999;
    return m_dummy;
  } else {
    std::cout << "Pymsrv: lambda^prime indices " << i1 << " " << i2 << " " << i3 << std::endl;
    return s_pymsrv->rvlamp[i3-1][i2-1][i1-1]; // note ``-1''. CLA, this way the indices work as expected.
  }
}

double& Pymsrv::rvlamb(int n) {
  int i1(n/100);
  int i2((n-i1*100)/10);
  int i3(n-i1*100-i2*10);
  if (i1<1 || i1>3 || i2<1 || i2>3 || i3<1 || i3>3) {
    std::cout
      << "Pymsrv: attempt to read or write RVLAMB out of bounds" << std::endl;
    m_dummy=-999;
    return m_dummy;
  } else {
    std::cout << "Pymsrv: lambda^doubelprime indices " << i1 << " " << i2 << " " << i3 << std::endl;
    return s_pymsrv->rvlamb[i3-1][i2-1][i1-1]; // note ``-1''. CLA, this way the indices work as expected.
  }
}
