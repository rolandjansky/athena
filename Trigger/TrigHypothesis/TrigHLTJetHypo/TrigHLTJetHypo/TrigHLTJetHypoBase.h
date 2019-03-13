/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TRIGHLTJETHYPOBASE_H
#define TRIGHLTJETHYPO_TRIGHLTJETHYPOBASE_H
/********************************************************************
 *
 * NAME:     TrigHLTJetHypoBase.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  February 13, 2017
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

class TrigHLTJetHypoBase : public HLT::HypoAlgo {

 public:

  TrigHLTJetHypoBase(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHypoBase();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 protected:

  // set values provided by sub classes
  void setConditions();
  void setJetGrouper();

  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const = 0;  // sub class provides                                                                                                       
  virtual std::shared_ptr<IJetGrouper> getJetGrouper() const = 0 ; // subclass provides
  virtual Conditions getConditions() const = 0;  // sub class provides
  virtual bool checkVals()  const = 0;  // sub class provides

  void addCleaner(const CleanerBridge&);

 private:
  
  void setCleaners();

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

std::vector<double> getEtThresholds(const std::vector<double>& dEtas,
                                    const std::vector<double>& etThresholds);
#endif
