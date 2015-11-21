/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LooseCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "./LooseCleaner.h"
#include "./UncleanableJet.h"  // exception class

#include <cmath>

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
  
bool LooseCleaner::operator()(const xAOD::Jet* jet) const {
  /* make cuts on jet attributes to select clean jets */
  
  bool isClean{true};

  float fsmJet;
  if(jet -> getAttribute<float>("FracSamplingMax", fsmJet)){
    if(fsmJet > m_fSampMaxLooseThreshold && std::abs(jet->eta()) < m_etaLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve FracSamplingMax");
  }

  float emfJet;
  if(jet -> getAttribute<float>("EMFrac", emfJet)){
    if(emfJet < m_emfLowLooseThreshold || emfJet > m_emfHighLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve EMFrac");
  }

  float hecfJet;
  if(jet -> getAttribute<float>("HECFrac", hecfJet)){
    if(hecfJet > m_hecfLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve HECFrac");
  }
    
  return isClean;

}
