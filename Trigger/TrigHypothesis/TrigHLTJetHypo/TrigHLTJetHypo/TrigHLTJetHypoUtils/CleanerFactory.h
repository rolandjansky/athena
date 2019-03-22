/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CLEANERFACTORY_H
#define TRIGHLTJETHYPO_CLEANERFACTORY_H


// ********************************************************************
//
// NAME:     cleanerFactory.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <memory>
#include <string>
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"

// Jet selector rather than traditional cleaner. Uses the Cleaner interface
std::shared_ptr<ICleaner> makeEtaEtCleaner(double etaMin,
                                           double etaMax,
                                           double etMin,
                                           double etMax);

class CleanerFactory{
 public:
  // This strange constructor allows the Algorithm, which 
  // itself is initialezed with all possible cleaning variables
  // to push the creation of any standard Cleaner into this Factory class.
  CleanerFactory (double, 
		  double, 
		  double,
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double, 
		  double,
		  const std::string& key);

  std::shared_ptr<ICleaner> make() const noexcept;
  
 private:

  //run 1 cleaning
  //double m_n90Threshold;
  //double m_presamplerThreshold;
  //double m_negativeEThreshold;
  
  //loose cleaning
  double m_fSampMaxLooseThreshold;
  double m_etaLooseThreshold;
  double m_emfLowLooseThreshold;
  double m_emfHighLooseThreshold;
  double m_hecfLooseThreshold;

  //tight cleaning
  double m_fSampMaxTightThreshold;
  double m_etaTightThreshold;
  double m_emfLowTightThreshold;
  double m_emfHighTightThreshold;
  double m_hecfTightThreshold;

  //long-lived particle cleaning
  double m_fSampMaxLlpThreshold;
  double m_negELlpThreshold;
  double m_hecfLlpThreshold;
  double m_hecqLlpThreshold;
  double m_avLarQFLlpThreshold;

  void make_();
  std::shared_ptr<ICleaner> m_cleaner;

  std::string m_name;
};
#endif
