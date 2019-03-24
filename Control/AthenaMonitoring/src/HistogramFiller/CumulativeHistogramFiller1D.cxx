/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/HistogramFiller/CumulativeHistogramFiller1D.h"

using namespace Monitored;

unsigned CumulativeHistogramFiller1D::fill() {
  if (m_monVariables.size() != 1) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
  std::lock_guard<std::mutex> lock(*(this->m_mutex));

  for (auto value : valuesVector) {
    unsigned bin = hist->FindBin(value);

    for (unsigned j = bin; j > 0; --j) {
      hist->AddBinContent(j);
    }

    ++i;
  }

  return i;  
}