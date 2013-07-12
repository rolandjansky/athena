/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PmbCxxUtils/CustomBenchmark.h"

namespace PMonUtils {

  CustomBenchmark::CustomBenchmark(unsigned nmax)
    : m_nmax(nmax),
      m_data(new Data[nmax]),
      m_data_current(0)
  {
    Data * itE = &(m_data[m_nmax]);
    for (Data * it = m_data; it!=itE; ++it)
      it->init();
  }

  CustomBenchmark::~CustomBenchmark()
  {
    delete[] m_data;
  }

}
