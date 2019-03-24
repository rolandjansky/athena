/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/HistogramFiller/HistogramFillerEfficiency.h"

using namespace Monitored;

unsigned HistogramFillerEfficiency::fill() {
  if (m_monVariables.size() != 2) {
    return 0;
  }

  unsigned i(0);
  auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
  auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
  std::lock_guard<std::mutex> lock(*(this->m_mutex));

  for (i = 0; i < valuesVector1.size(); ++i) {
    m_eff->Fill(valuesVector1[i],valuesVector2[i]);
  }
  
  return i;
}