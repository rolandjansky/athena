/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LlpCleanerTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LlpCleanerTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class

#include <sstream>
#include <cmath>

LlpCleanerTool::LlpCleanerTool(const std::string& type,
                              const std::string& name,
                              const IInterface* parent) :
  base_class(type, name, parent){ 
}


bool LlpCleanerTool::select(const pHypoJet& jet) const {
  /* make cuts on jet attributes to select jets from long-lived particles */
  
  bool isClean{true};

  float fsmJet;
  float neJet;
  if(jet -> getAttribute("FracSamplingMax", fsmJet)){
    if(jet -> getAttribute("NegativeE", neJet)){
      if(fsmJet > m_fSampMaxLlpThreshold && 
         std::abs(neJet) > m_negELlpThreshold){isClean = false;}
    } else {
      throw UncleanableJet("CleanerTool Cannot retrieve NegativeE");
    }
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve FracSamplingMax");
  }


  float hecfJet;
  float hecqJet;
  float alqfJet;
  if(jet -> getAttribute("HECFrac", hecfJet)){
    if(jet -> getAttribute("HECQuality", hecqJet)){
      if(jet -> getAttribute("AverageLArQF", alqfJet)){
        if(hecfJet > m_hecfLlpThreshold && std::abs(hecqJet) > m_hecqLlpThreshold &&
           alqfJet > m_avLarQFLlpThreshold){isClean = false;}
      } else {
        throw UncleanableJet("CleanerTool Cannot retrieve AverageLArQF");
      }
    } else {
      throw UncleanableJet("CleanerTool Cannot retrieve HECQuality");
    }
  } else {
    throw UncleanableJet("CleanerTool Cannot retrieve HECFrac");
  }
    
  return isClean;

}

std::string LlpCleanerTool::toString() const noexcept {
  std::stringstream ss;
  ss << getName() 
     <<  ":"
     << " fSampMaxLlpThreshold " << m_fSampMaxLlpThreshold
     << " negELlpThreshold " << m_negELlpThreshold
     << " hecfLlpThreshold " << m_hecfLlpThreshold
     << " hecqLlpThreshold " << m_hecqLlpThreshold
     << " avLarQFLlpThreshold " << m_avLarQFLlpThreshold
     << '\n';
  return ss.str();
}

std::string LlpCleanerTool::getName() const noexcept {return "LlpCleanerTool";}
