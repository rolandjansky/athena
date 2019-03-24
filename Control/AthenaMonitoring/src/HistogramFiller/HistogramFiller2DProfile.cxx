/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/HistogramFiller/HistogramFiller2DProfile.h"

using namespace Monitored;

unsigned HistogramFiller2DProfile::fill() {
  if (m_monVariables.size() != 3) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector1 = m_monVariables[0].get().getVectorRepresentation();
  auto valuesVector2 = m_monVariables[1].get().getVectorRepresentation();
  auto valuesVector3 = m_monVariables[2].get().getVectorRepresentation();
  std::lock_guard<std::mutex> lock(*(this->m_mutex));
  /*HERE NEED TO INCLUDE CASE IN WHICH SOME VARIABLES ARE SCALAR AND SOME VARIABLES ARE VECTORS
  unsigned i(0);
  if (m_variable1->size() != m_variable2->size() || m_variable1->size() != m_variable3->size() || m_variable2->size() != m_variable3->size() ) {

  }*/

  //For now lets just consider the case in which all variables are of the same length
  for (i = 0; i < valuesVector1.size(); ++i) {
    hist->Fill(valuesVector1[i], valuesVector2[i], valuesVector3[i]);
  }
  
  return i;
}