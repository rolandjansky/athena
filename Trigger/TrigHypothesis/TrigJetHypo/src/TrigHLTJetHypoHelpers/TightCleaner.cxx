/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TightCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "./TightCleaner.h"
#include "./UncleanableJet.h"  // exception class

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
  
bool TightCleaner::operator()(const xAOD::Jet* jet) const {
  /* make cuts on jet attributes to select clean jets */
  
  bool isClean{true};

  float fsmJet;
  if(jet -> getAttribute<float>("FracSamplingMax", fsmJet)){
    if(fsmJet > m_fSampMaxTightThreshold && std::abs(jet->eta()) < m_etaTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve FracSamplingMax");
  }

  float emfJet;
  if(jet -> getAttribute<float>("EMFrac", emfJet)){
    if(emfJet < m_emfLowTightThreshold || emfJet > m_emfHighTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve EMFrac");
  }

  float hecfJet;
  if(jet -> getAttribute<float>("HECFrac", hecfJet)){
    if(hecfJet > m_hecfTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve HECFrac");
  }
    
  return isClean;

}
