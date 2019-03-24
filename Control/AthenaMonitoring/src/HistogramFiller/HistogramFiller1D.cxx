/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/HistogramFiller/HistogramFiller1D.h"

using namespace Monitored;

unsigned HistogramFiller1D::fill() {
  if (m_monVariables.size() != 1) {
    return 0;
  }

  unsigned i(0);
  auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
  std::lock_guard<std::mutex> lock(*(this->m_mutex));

  for (auto value : valuesVector) {
    histogram()->Fill(value);
    ++i;
  }

  return i;
} 