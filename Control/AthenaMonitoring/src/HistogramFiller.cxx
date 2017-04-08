/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaMonitoring/HistogramFiller.h"

using namespace std;

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

unsigned VecHistogramFiller1D::fill() {
  if (m_monVariables.size() != 1) {
    return 0;
  }

  unsigned i(0);
  auto hist = histogram();
  auto valuesVector = m_monVariables[0].get().getVectorRepresentation();
  std::lock_guard<std::mutex> lock(*(this->m_mutex));

  for (auto value : valuesVector) {
    hist->AddBinContent(i+1, value);
    hist->SetEntries(hist->GetEntries() + value);

    ++i;
  }

  return i;
}

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
      // second varaible is scalar -- loop over first
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

unsigned HistogramFiller2D::fill() {
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
      // second varaible is scalar -- loop over first
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