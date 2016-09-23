/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LlpCleaner.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   N Anjos
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LlpCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/UncleanableJet.h"  // exception class

#include <sstream>
#include <cmath>

LlpCleaner::LlpCleaner(float fSampMaxLlpThreshold,
                       float negELlpThreshold,
                       float hecfLlpThreshold,
                       float hecqLlpThreshold,
                       float avLarQFLlpThreshold):
  m_fSampMaxLlpThreshold(fSampMaxLlpThreshold),
  m_negELlpThreshold(negELlpThreshold),
  m_hecfLlpThreshold(hecfLlpThreshold),
  m_hecqLlpThreshold(hecqLlpThreshold),
  m_avLarQFLlpThreshold(avLarQFLlpThreshold){
  }
  
bool LlpCleaner::operator()(const pHypoJet& jet) const {
  /* make cuts on jet attributes to select jets from long-lived particles */
  
  bool isClean{true};

  float fsmJet;
  float neJet;
  if(jet -> getAttribute("FracSamplingMax", fsmJet)){
    if(jet -> getAttribute("NegativeE", neJet)){
      if(fsmJet > m_fSampMaxLlpThreshold && 
         std::abs(neJet) > m_negELlpThreshold){isClean = false;}
    } else {
      throw UncleanableJet("Cleaner Cannot retrieve NegativeE");
    }
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve FracSamplingMax");
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
        throw UncleanableJet("Cleaner Cannot retrieve AverageLArQF");
      }
    } else {
      throw UncleanableJet("Cleaner Cannot retrieve HECQuality");
    }
  } else {
    throw UncleanableJet("Cleaner Cannot retrieve HECFrac");
  }
    
  return isClean;

}

std::string LlpCleaner::toString() const noexcept {
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

std::string LlpCleaner::getName() const noexcept {return "LlpCleaner";}
