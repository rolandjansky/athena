/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigHLTJetHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *********************************************************************/

/* WORK IN PROGRESS _ DO NOT USE ! */


#ifndef TRIGHLTJETHYPO_H
#define TRIGHLTJETHYPO_H

#include <memory>
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

using JetVector = std::vector<const xAOD::Jet*>;
using JetIter = JetVector::iterator;
using JetCIter = JetVector::const_iterator;

class ICleanerMatcher{
 public:
  virtual void process(JetIter, JetIter) = 0;
  virtual bool pass() const = 0;
  virtual std::pair<JetCIter, JetCIter> passedJetIters() const = 0;
  virtual std::pair<JetCIter, JetCIter> failedJetIters() const = 0;
};

// Define a class to hold etaMin, etaMax and threshold (Et in MeV)
// that jets will be matched against
class Condition{
 public:
  Condition(double etaMin, double etaMax, double threshold): 
    m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){}

    bool etaOK(const xAOD::IParticle* ip) const {
      if (std::abs(ip->eta()) < m_etaMin){return false;}
      if (std::abs(ip->eta()) >= m_etaMax){return false;}
      return true;
    }

    bool etOK(const xAOD::IParticle* ip) const {
    if ((ip->p4()).Et() < threshold()){return false;}
      return true;
    }

    bool allOK(const xAOD::IParticle* ip) const {
      return etaOK(ip) && etOK(ip);
    }

  double etaMin() const {return m_etaMin;}
  double etaMax() const {return m_etaMax;}
  double threshold() const {return m_threshold;}

 private:
  double m_etaMin;
  double m_etaMax;
  double m_threshold;
};

std::ostream& operator<< (std::ostream& os, const Condition& c){
  return os << "Condition: etaMin "
            << c.etaMin() 
            << " etaMax " 
            << c.etaMax() 
            << " threshold: " 
            << c.threshold()
            <<'\n';
}

using Conditions = std::vector<Condition>;
using ConditionsIter = std::vector<Condition>::iterator;

struct ConditionsSorter{
  bool operator()(const Condition& l, const Condition& r){
    return l.threshold() > r.threshold();
  }
};



class TrigHLTJetHypo : public HLT::HypoAlgo {

 public:

  TrigHLTJetHypo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TrigHLTJetHypo();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

 private:

  HLT::ErrorCode checkJets(const xAOD::JetContainer*);
  void publishResult(const std::unique_ptr<ICleanerMatcher> &);
  void bumpCounters(bool, int);
  void monitorLeadingJet(const xAOD::Jet* jet);
  void writeDebug(bool, int,
                  const std::pair<JetCIter, JetCIter>&,
                  const std::pair<JetCIter, JetCIter>& ) const;
  std::unique_ptr<ICleanerMatcher> getCleanerMatcher() const;
  HLT::ErrorCode 
    markAndStorePassingJets(const std::unique_ptr<ICleanerMatcher>&,
                            const xAOD::JetContainer*,
                            const HLT::TriggerElement*);
  void resetCounters();

  double     m_EtCut;
  double     m_etaMinCut;
  double     m_etaMaxCut;
  unsigned int m_multiplicitycut;

  /// Et thresholds for assymmetric jet thresholds
  /// NB: for backwards compatability, retain the m_EtCut, 
  ///     so if multiple thresholds are not set, it will 
  ///     use the old, m_EtCut for each jet,
  ///     if however multiple thresholds are set, the 
  ///     old m_EtCut will be ignored.
  ///     use an extra vector for the actual property, 
  ///     so it can be modified, without changing what 
  ///     is actually passed in from the configurable 
  std::vector<double> m_EtThresholds;
  std::vector<double> m_EtThresholdsInput;
  std::vector<double> m_etaMins;
  std::vector<double> m_etaMaxs;
  Conditions m_conditions;

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
  
  bool m_doBasicCleaning;
  bool m_triggerOnBad;
  
  float m_n90Threshold;
  float m_presamplerThreshold;
  float m_hecFThreshold;
  float m_hecQThreshold;
  float m_qmeanThreshold;
  float m_negativeEThreshold;
  float m_emFThreshold;
  float m_larQThreshold;
  

  // path for the histograms:

  //std::string m_path;

  // Histogram Service:
  //IHistogramSvc* m_histsvc;

  StoreGateSvc*                           m_storeGate;

  // Timing:

  ITrigTimerSvc*            m_timersvc;
  std::vector<TrigTimer*>   m_timers;

};
#endif

