/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TightCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TightCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class
#include <sstream>
#include <cmath>

TightCleaner::TightCleaner(float fSampMaxTightThreshold,
                           float etaTightThreshold,
                           float emfLowTightThreshold,
                           float emfHighTightThreshold,
                           float hecfTightThreshold):
  m_fSampMaxTightThreshold(fSampMaxTightThreshold),
  m_etaTightThreshold(etaTightThreshold),
  m_emfLowTightThreshold(emfLowTightThreshold),
  m_emfHighTightThreshold(emfHighTightThreshold),
  m_hecfTightThreshold(hecfTightThreshold){
  }
  
bool TightCleaner::operator()(const pHypoJet& jet) const {
  /* make cuts on jet attributes to select clean jets */
  
  bool isClean{true};

  float fsmJet;
  if(jet -> getAttribute("FracSamplingMax", fsmJet)){
    if(fsmJet > m_fSampMaxTightThreshold && std::abs(jet->eta()) < m_etaTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve FracSamplingMax");
  }

  float emfJet;
  if(jet -> getAttribute("EMFrac", emfJet)){
    if(emfJet < m_emfLowTightThreshold || emfJet > m_emfHighTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve EMFrac");
  }

  float hecfJet;
  if(jet -> getAttribute("HECFrac", hecfJet)){
    if(hecfJet > m_hecfTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve HECFrac");
  }
    
  return isClean;

}

std::string TightCleaner::toString() const noexcept {
  std::stringstream ss;
  ss << getName() 
     <<  ":"
     << " fSampMaxTightThreshold " << m_fSampMaxTightThreshold
     << " etaTightThreshold " << m_etaTightThreshold
     << " emfLowTightThreshold " << m_emfLowTightThreshold
     << " emfHighTightThreshold " << m_emfHighTightThreshold
     << " hecfTightThreshold " << m_hecfTightThreshold
     << '\n';
  return ss.str();
}


std::string TightCleaner::getName() const noexcept {return "TightCleaner";}
