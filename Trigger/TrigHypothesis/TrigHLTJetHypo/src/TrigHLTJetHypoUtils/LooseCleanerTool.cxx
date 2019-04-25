/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LooseCleanerTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LooseCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class

#include <cmath>
#include <sstream>


LooseCleanerTool::LooseCleanerTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent) :
  base_class(type, name, parent){ 
}

bool LooseCleanerTool::select(const pHypoJet& jet) const {
  /* make cuts on jet attributes to select clean jets */
  
  bool isClean{true};

  float fsmJet;
  if(jet -> getAttribute("FracSamplingMax", fsmJet)){
    if(fsmJet > m_fSampMaxLooseThreshold && std::abs(jet->eta()) < m_etaLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve FracSamplingMax");
  }

  float emfJet;
  if(jet -> getAttribute("EMFrac", emfJet)){
    if(emfJet < m_emfLowLooseThreshold || emfJet > m_emfHighLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve EMFrac");
  }

  float hecfJet;
  if(jet -> getAttribute("HECFrac", hecfJet)){
    if(hecfJet > m_hecfLooseThreshold){isClean = false;}
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve HECFrac");
  }
    
  return isClean;

}

std::string LooseCleanerTool::toString() const noexcept {
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

std::string LooseCleanerTool::getName() const noexcept {
  return "LooseCleanerTool";}
