/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/HistogramFiller/VecHistogramFiller1DWithOverflows.h"

using namespace Monitored;

unsigned VecHistogramFiller1DWithOverflows::fill() {
  if (m_monVariables.size() != 1) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
  std::lock_guard<std::mutex> lock(*(this->m_mutex));

  for (auto value : valuesVector) {
    hist->AddBinContent(i, value);
    hist->SetEntries(hist->GetEntries() + value);

    ++i;
  }

  return i;
}