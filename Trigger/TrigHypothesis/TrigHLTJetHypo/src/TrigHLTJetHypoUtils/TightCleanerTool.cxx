
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TightCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TightCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class
#include <sstream>
#include <cmath>

TightCleanerTool::TightCleanerTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent) :
  base_class(type, name, parent){ 
}
  
  
bool TightCleanerTool::select(const pHypoJet& jet) const {
  /* make cuts on jet attributes to select clean jets */
  
  bool isClean{true};

  float fsmJet;
  if(jet -> getAttribute("FracSamplingMax", fsmJet)){
    if(fsmJet > m_fSampMaxTightThreshold && std::abs(jet->eta()) < m_etaTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve FracSamplingMax");
  }

  float emfJet;
  if(jet -> getAttribute("EMFrac", emfJet)){
    if(emfJet < m_emfLowTightThreshold || emfJet > m_emfHighTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve EMFrac");
  }

  float hecfJet;
  if(jet -> getAttribute("HECFrac", hecfJet)){
    if(hecfJet > m_hecfTightThreshold){isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve HECFrac");
  }
    
  return isClean;

}

std::string TightCleanerTool::toString() const noexcept {
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


std::string TightCleanerTool::getName() const noexcept {
  return "TightCleanerTool";}
