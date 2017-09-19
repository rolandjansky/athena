/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     cleanerFactory.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   N Anjos, P Sherwood
//
// ********************************************************************
//


#include <stdexcept>
#include <memory>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LooseCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AntiLooseCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TightCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AntiTightCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/LlpCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AntiLlpCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/NullCleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCleaner.h"

using pIC = std::shared_ptr<ICleaner>;

std::shared_ptr<ICleaner> makeEtaEtCleaner(double etaMin,
                                           double etaMax,
                                           double etMin,
                                           double etMax){

   return std::shared_ptr<ICleaner> (new EtaEtCleaner(etaMin,
                                                      etaMax,
                                                      etMin,
                                                      etMax));
}


CleanerFactory::CleanerFactory(//basic cleaning
			       double /*n90Threshold*/,
			       double /*presamplerThreshold*/,
			       double /*negativeEThreshold*/,
			       //loose cleaning
			       double fSampMaxLooseThreshold,
			       double etaLooseThreshold,
			       double emfLowLooseThreshold,
			       double emfHighLooseThreshold,
			       double hecfLooseThreshold,
			       //tight cleaning
			       double fSampMaxTightThreshold,
			       double etaTightThreshold,
			       double emfLowTightThreshold,
			       double emfHighTightThreshold,
			       double hecfTightThreshold,
			       //long-lived particle cleaning
			       double fSampMaxLlpThreshold,
			       double negELlpThreshold,
			       double hecfLlpThreshold,
			       double hecqLlpThreshold,
			       double avLarQFLlpThreshold,
			       const std::string& key):
  //m_n90Threshold(n90Threshold),
  //m_presamplerThreshold(presamplerThreshold),
  //m_negativeEThreshold(negativeEThreshold),
  m_fSampMaxLooseThreshold(fSampMaxLooseThreshold),
  m_etaLooseThreshold(etaLooseThreshold),
  m_emfLowLooseThreshold(emfLowLooseThreshold),
  m_emfHighLooseThreshold(emfHighLooseThreshold),
  m_hecfLooseThreshold(hecfLooseThreshold),
  m_fSampMaxTightThreshold(fSampMaxTightThreshold),
  m_etaTightThreshold(etaTightThreshold),
  m_emfLowTightThreshold(emfLowTightThreshold),
  m_emfHighTightThreshold(emfHighTightThreshold),
  m_hecfTightThreshold(hecfTightThreshold),
  m_fSampMaxLlpThreshold(fSampMaxLlpThreshold),
  m_negELlpThreshold(negELlpThreshold),
  m_hecfLlpThreshold(hecfLlpThreshold),
  m_hecqLlpThreshold(hecqLlpThreshold),
  m_avLarQFLlpThreshold(avLarQFLlpThreshold),
  m_name(key){
  //cleaners have no state, so can be cached.
  make_();
  }


pIC CleanerFactory::make() const noexcept{
  return m_cleaner;
}


void CleanerFactory::make_() {

  /*
  if (m_name == "basicCleaning"){
    m_cleaner =  pIC (new BasicCleaner(m_n90Threshold, 
				       m_presamplerThreshold,
				       m_negativeEThreshold));
    
  }
  else if (m_name == "antiBasicCleaning"){
    m_cleaner =  pIC (new AntiBasicCleaner(m_n90Threshold, 
					   m_presamplerThreshold,
					   m_negativeEThreshold));
  } 
  */ 

  if (m_name == "looseCleaning"){
    m_cleaner =  pIC (new LooseCleaner(m_fSampMaxLooseThreshold,
				       m_etaLooseThreshold,
				       m_emfLowLooseThreshold,
				       m_emfHighLooseThreshold,
				       m_hecfLooseThreshold));
  } else if (m_name == "antiLooseCleaning"){
    m_cleaner =  pIC (new AntiLooseCleaner(m_fSampMaxLooseThreshold,
					   m_etaLooseThreshold,
					   m_emfLowLooseThreshold,
					   m_emfHighLooseThreshold,
					   m_hecfLooseThreshold));
  } else if (m_name == "tightCleaning"){
    m_cleaner =  pIC (new TightCleaner(m_fSampMaxTightThreshold,
				       m_etaTightThreshold,
				       m_emfLowTightThreshold,
				       m_emfHighTightThreshold,
				       m_hecfTightThreshold));
  } else if (m_name == "antiTightCleaning"){
    m_cleaner =  pIC (new AntiTightCleaner(m_fSampMaxTightThreshold,
					   m_etaTightThreshold,
					   m_emfLowTightThreshold,
					   m_emfHighTightThreshold,
					   m_hecfTightThreshold));
  } else if (m_name == "llpCleaning"){
    m_cleaner =  pIC (new LlpCleaner(m_fSampMaxLlpThreshold,
				     m_negELlpThreshold,
				     m_hecfLlpThreshold,
				     m_hecqLlpThreshold,
				     m_avLarQFLlpThreshold));
  } else if (m_name == "antiLlpCleaning"){
    m_cleaner =  pIC (new AntiLlpCleaner(m_fSampMaxLlpThreshold,
					 m_negELlpThreshold,
					 m_hecfLlpThreshold,
					 m_hecqLlpThreshold,
					 m_avLarQFLlpThreshold));
    } else if (m_name == "noCleaning"){
    m_cleaner =  pIC (new NullCleaner);
    
  } else {
    throw std::invalid_argument{"Unknown cleaner specified :" + m_name};
    
  }
}
  



