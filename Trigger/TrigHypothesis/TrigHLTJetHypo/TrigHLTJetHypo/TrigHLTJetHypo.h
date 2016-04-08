/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_H
#define TRIGHLTJETHYPO_H
/********************************************************************
 *
 * NAME:     TrigHLTJetHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *********************************************************************/

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerMatcherFactory.h"

class TriggerElement;
class CleanerMatcher;

class TrigHLTJetHypo : public HLT::HypoAlgo {

 public:

  TrigHLTJetHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  HLT::ErrorCode checkJets(const xAOD::JetContainer*);
  void publishResult(const CleanerMatcher &, bool,
                     const xAOD::JetContainer*&);
  void bumpCounters(bool, int);
  void monitorLeadingJet(const xAOD::Jet* jet);
  void writeDebug(bool,
                  const HypoJetVector&,
                  const HypoJetVector&,
		  const CleanerMatcher&) const;

  // CleanerMatcher getCleanerMatcher() const;
  
  HLT::ErrorCode 
    markAndStorePassingJets(const CleanerMatcher&,
                            const xAOD::JetContainer*,
                            const HLT::TriggerElement*);
  void resetCounters();

  // vectors with Et thresholds, eta nins and eta maxs
  // (thresh, eta min, eta max) triplets will bbe converted to Conditon objs.
  std::vector<double> m_EtThresholds;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;

  // vector of indices find ofssets into the jet vector,
  // and other Condition variables used for TLA style hypos.
  std::vector<unsigned int> m_jetvec_indices;
  std::vector<double> m_ystarMins;
  std::vector<double> m_ystarMaxs;
  std::vector<double> m_massMins;
  std::vector<double> m_massMaxs;

  // Conditions m_conditions;
  std::shared_ptr<CleanerMatcherFactory> m_cleanerMatcherFactory;

  int m_accepted;
  int m_rejected;
  int m_errors;

  // Switch to accept all the events.

  bool m_acceptAll;

  // Switch on Monitoring:
  
  bool m_doMonitoring;

  int m_cutCounter;
  double m_njet;
  double m_et;
  double m_eta;
  double m_phi;
  
  
  // switch on cleaning
  
  std::string m_cleaningAlg;  // determines cleaner obj
  std::string m_matchingAlg;  // determines matcher obj;

  //basic cleaning
  float m_n90Threshold;
  float m_presamplerThreshold;
  float m_hecFThreshold;
  float m_hecQThreshold;
  float m_qmeanThreshold;
  float m_negativeEThreshold;
  float m_emFThreshold;
  float m_larQThreshold;
  //loose cleaning
  float m_fSampMaxLooseThreshold;
  float m_etaLooseThreshold;
  float m_emfLowLooseThreshold;
  float m_emfHighLooseThreshold;
  float m_hecfLooseThreshold;
  //Tight cleaning
  float m_fSampMaxTightThreshold;
  float m_etaTightThreshold;
  float m_emfLowTightThreshold;
  float m_emfHighTightThreshold;
  float m_hecfTightThreshold;
  //Long-lived particle cleaning
  float m_fSampMaxLlpThreshold;
  float m_negELlpThreshold;
  float m_hecfLlpThreshold;
  float m_hecqLlpThreshold;
  float m_avLarQFLlpThreshold;

  // std::shared_ptr<ICleaner> m_cleaner;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

