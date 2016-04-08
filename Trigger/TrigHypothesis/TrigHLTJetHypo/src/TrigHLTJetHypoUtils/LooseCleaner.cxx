/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LooseCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LooseCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class

#include <cmath>
#include <sstream>

LooseCleaner::LooseCleaner(float fSampMaxLooseThreshold,
                           float etaLooseThreshold,
                           float emfLowLooseThreshold,
                           float emfHighLooseThreshold,
                           float hecfLooseThreshold):
  m_fSampMaxLooseThreshold(fSampMaxLooseThreshold),
  m_etaLooseThreshold(etaLooseThreshold),
  m_emfLowLooseThreshold(emfLowLooseThreshold),
  m_emfHighLooseThreshold(emfHighLooseThreshold),
  m_hecfLooseThreshold(hecfLooseThreshold){
  }
  
bool LooseCleaner::operator()(const pHypoJet& jet) const {
  /* make cuts on jet attributes to select clean jets */
  
  bool isClean{true};

  float fsmJet;
  if(jet -> getAttribute("FracSamplingMax", fsmJet)){
    if(fsmJet > m_fSampMaxLooseThreshold && std::abs(jet->eta()) < m_etaLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve FracSamplingMax");
  }

  float emfJet;
  if(jet -> getAttribute("EMFrac", emfJet)){
    if(emfJet < m_emfLowLooseThreshold || emfJet > m_emfHighLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve EMFrac");
  }

  float hecfJet;
  if(jet -> getAttribute("HECFrac", hecfJet)){
    if(hecfJet > m_hecfLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve HECFrac");
  }
    
  return isClean;

}

std::string LooseCleaner::toString() const noexcept {
  std::stringstream ss;
  ss << getName() 
     <<  ":"
     << " fSampMaxLooseThreshold " << m_fSampMaxLooseThreshold
     << " etaLooseThreshold " << m_etaLooseThreshold
     << " emfLowLooseThreshold " << m_emfLowLooseThreshold
     << " emfHighLooseThreshold " << m_emfHighLooseThreshold
     << " hecfLooseThreshold " << m_hecfLooseThreshold
     << '\n';
  return ss.str();
}

std::string LooseCleaner::getName() const noexcept {return "LooseCleaner";}
