/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TRIGHLTJETHYPO2_H
#define TRIGHLTJETHYPO_TRIGHLTJETHYPO2_H
/********************************************************************
 *
 * NAME:     Trighltjethypo2.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *********************************************************************/

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include <memory>
#include <chrono>

class TriggerElement;
class TrigHLTJetHypoHelper;
enum class HypoStrategy;

class TrigHLTJetHypo2 : public HLT::HypoAlgo {

 public:

  TrigHLTJetHypo2(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHypo2();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:
  
  bool checkStrategy(HypoStrategy);
  bool checkEtaEtStrategy();
  bool checkTLAStrategy();
  bool checkDijetMassDEtaStrategy();
  bool checkHTStrategy();
  bool checksinglemassStrategy();

  void setCleaner();

  bool setConditions(HypoStrategy);
  bool setEtaEtConditions();
  bool setTLAConditions();
  bool setDijetMassDEtaConditions();
  bool setHTConditions();
  bool setsinglemassConditions();

  bool setJetGrouper(HypoStrategy);

  std::vector<double> getStringBoundaries (const std::vector<std::string>& stv);

  HLT::ErrorCode checkJets(const xAOD::JetContainer*);
  void publishResult(const TrigHLTJetHypoHelper&, bool,
                     const xAOD::JetContainer*&);
  void bumpCounters(bool, int);
  void monitorLeadingJet(const xAOD::Jet* jet);
  void writeDebug(bool,
                  const HypoJetVector&,
                  const HypoJetVector&) const;

  HLT::ErrorCode 
    markAndStorePassingJets(const TrigHLTJetHypoHelper&,
                            const xAOD::JetContainer*,
                            const HLT::TriggerElement*);
  void resetCounters();

  void accumulateTime(std::chrono::nanoseconds) noexcept;

  std::string m_chainName;  // used for configuration of dimass chains
  // vectors with Et thresholds, eta nins and eta maxs
  // (thresh, eta min, eta max) triplets will bbe converted to Conditon objs.
  std::vector<double> m_EtThresholds;
  std::vector<std::string> m_JetMassMin;
  std::vector<std::string> m_JetMassMax;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;
  std::vector<int> m_asymmetricEtas;

  // vector of indices find ofssets into the jet vector,
  // and other Condition variables used for TLA style hypos.
  std::vector<double> m_ystarMins;
  std::vector<double> m_ystarMaxs;
  std::vector<double> m_massMins;
  std::vector<double> m_massMaxs;

  // delta eta cut (DimassDeta strategy)
  std::vector<double> m_dEtaMins;
  std::vector<double> m_dEtaMaxs;

  //HT
  double m_htMin;

  // Dijets
  /*double m_invm;
    double m_deta;*/

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
  std::string m_hypoStrategy; // determines Conditions, jetGrouper


  // Jet Grouper parameters:
  std::vector<unsigned int> m_jetvec_indices; //indexed jets
  unsigned int m_combinationsSize;  // jet combinations

  // Cleaning parameters
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


  std::vector<CleanerBridge> m_cleaners;
  std::shared_ptr<IJetGrouper> m_grouper;
  Conditions m_conditions;
    
  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

 // local code fragment timing

  double m_chainTimeAv{0.}; //std::chrono
  // double m_chainTimeSquareAv{0.}; //std::chrono
  unsigned int m_nCalls{0};

  bool m_dumpJets{false};
};
#endif
