/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/HistogramFiller/HistogramFillerProfile.h"

using namespace Monitored;

unsigned HistogramFillerProfile::fill() {
  if (m_monVariables.size() != 2) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
  auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
  std::lock_guard<std::mutex> lock(*(this->m_mutex));

  if (valuesVector1.size() != valuesVector2.size()) {
    if (valuesVector1.size() == 1) {
      // first variable is scalar -- loop over second
      for (auto value2 : valuesVector2) {
        hist->Fill(valuesVector1[0], value2);
        ++i;
      }
    } else if (valuesVector2.size() == 1)  {
      // second variable is scalar -- loop over first
      for (auto value1 : valuesVector1) {
        hist->Fill(value1, valuesVector2[0]); 
        ++i;
      } 
    }
  } else {
    for (i = 0; i < valuesVector1.size(); ++i) {
      hist->Fill(valuesVector1[i], valuesVector2[i]);
    }
  }
  
  return i;
}