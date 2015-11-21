/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     cleanerFactory.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include <stdexcept>
#include <memory>

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" // ICleaner
#include "./cleanerFactory.h"

#include "./BasicCleaner.h"
#include "./AntiBasicCleaner.h"
#include "./LooseCleaner.h"
#include "./AntiLooseCleaner.h"
#include "./TightCleaner.h"
#include "./AntiTightCleaner.h"
#include "./LlpCleaner.h"
#include "./AntiLlpCleaner.h"
#include "./NullCleaner.h"

std::shared_ptr<ICleaner>
cleanerFactory(//basic cleaning
	       double m_n90Threshold,
               double m_presamplerThreshold,
               double m_negativeEThreshold,
               //loose cleaning
               double m_fSampMaxLooseThreshold,
               double m_etaLooseThreshold,
               double m_emfLowLooseThreshold,
               double m_emfHighLooseThreshold,
               double m_hecfLooseThreshold,
               //tight cleaning
               double m_fSampMaxTightThreshold,
               double m_etaTightThreshold,
               double m_emfLowTightThreshold,
               double m_emfHighTightThreshold,
               double m_hecfTightThreshold,
               //long-lived particle cleaning
               double m_fSampMaxLlpThreshold,
               double m_negELlpThreshold,
               double m_hecfLlpThreshold,
               double m_hecqLlpThreshold,
               double m_avLarQFLlpThreshold,
               //cleaning mode
               const std::string& key){

  using pIC = std::shared_ptr<ICleaner>;

  if (key == "basicCleaning"){
    return pIC (new BasicCleaner(m_n90Threshold, 
                                 m_presamplerThreshold,
                                 m_negativeEThreshold));

  } else if (key == "antiBasicCleaning"){
    return pIC (new AntiBasicCleaner(m_n90Threshold, 
                                     m_presamplerThreshold,
                                     m_negativeEThreshold));
  } else if (key == "looseCleaning"){
    return pIC (new LooseCleaner(m_fSampMaxLooseThreshold,
                                 m_etaLooseThreshold,
                                 m_emfLowLooseThreshold,
                                 m_emfHighLooseThreshold,
                                 m_hecfLooseThreshold));
  } else if (key == "antiLooseCleaning"){
    return pIC (new AntiLooseCleaner(m_fSampMaxLooseThreshold,
                                     m_etaLooseThreshold,
                                     m_emfLowLooseThreshold,
                                     m_emfHighLooseThreshold,
                                     m_hecfLooseThreshold));
  } else if (key == "tightCleaning"){
    return pIC (new TightCleaner(m_fSampMaxTightThreshold,
                                 m_etaTightThreshold,
                                 m_emfLowTightThreshold,
                                 m_emfHighTightThreshold,
                                 m_hecfTightThreshold));
  } else if (key == "antiTightCleaning"){
    return pIC (new AntiTightCleaner(m_fSampMaxTightThreshold,
                                     m_etaTightThreshold,
                                     m_emfLowTightThreshold,
                                     m_emfHighTightThreshold,
                                     m_hecfTightThreshold));
  } else if (key == "llpCleaning"){
    return pIC (new LlpCleaner(m_fSampMaxLlpThreshold,
                               m_negELlpThreshold,
                               m_hecfLlpThreshold,
                               m_hecqLlpThreshold,
                               m_avLarQFLlpThreshold));
  } else if (key == "antiLlpCleaning"){
    return pIC (new AntiLlpCleaner(m_fSampMaxLlpThreshold,
                                   m_negELlpThreshold,
                                   m_hecfLlpThreshold,
                                   m_hecqLlpThreshold,
                                   m_avLarQFLlpThreshold));
  } else if (key == "noCleaning"){
    return pIC (new NullCleaner);

  } else {
    throw std::invalid_argument{"Unknown cleaner specified :" + key};

  }
}



