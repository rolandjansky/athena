/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_TRIGHLTJETHYPOTOOL_H
#define TRIGHLTJETHYPO_TRIGHLTJETHYPOTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHLTJetHypo/ITrigHLTJetHypoTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper.h"

#include "xAODJet/Jet.h"

#include <string>
#include <chrono>

using namespace std::chrono;

class TrigHLTJetHypoTool : virtual public ITrigHLTJetHypoTool, 
// virtual public AthAlgTool {
  public AthAlgTool {
 public:
  TrigHLTJetHypoTool(const std::string&, 
                     const std::string&, 
                     const IInterface* );
  
  // to allow access to the IHelloTool interface
  StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );
  StatusCode initialize() override;
  StatusCode doesItPass(const xAOD::JetContainer*, bool&) override;
  
 protected:
  
  // set values provided by sub classes
  void setConditions();
  void setJetGrouper();
  
  // methods provided by subclasses:
  virtual std::vector<std::shared_ptr<ICleaner>> getCleaners() const = 0;
  virtual std::shared_ptr<IJetGrouper> getJetGrouper() const = 0;
  virtual Conditions getConditions() const = 0;
  virtual StatusCode checkVals() const = 0;

  void addCleaner(const CleanerBridge&);

 private:
  
  void setCleaners();

  void publishResult(const TrigHLTJetHypoHelper&, bool,
                     const xAOD::JetContainer*);
  void bumpCounters(bool, int);
  void monitorLeadingJet(const xAOD::Jet* jet);
  void writeDebug(bool,
                  const HypoJetVector&,
                  const HypoJetVector&) const;

  void resetCounters();

  void accumulateTime(nanoseconds) noexcept;

  std::string m_chainName;  // used for configuration of dimass chains

  int m_accepted{0};
  int m_rejected{0};
  int m_errors{0};

  // Switch to accept all the events.
  bool m_acceptAll{false};

  // Switch on Monitoring:
  bool m_doMonitoring{false};

  // monitored variables
  int m_njet{0};
  double m_et{0.};
  double m_eta{0.};
  double m_phi{0.};
  
  // switch on cleaning
  std::string m_cleaningAlg;  // determines cleaner obj
  std::string m_matchingAlg;  // determines matcher obj;


  // Cleaning parameters
  //basic cleaning
  float m_n90Threshold{0.};
  float m_presamplerThreshold{0.};
  float m_hecFThreshold{0.};
  float m_hecQThreshold{0.};
  float m_qmeanThreshold{0.};
  float m_negativeEThreshold{0.};
  float m_emFThreshold{0.};
  float m_larQThreshold{0.};
  //loose cleaning
  float m_fSampMaxLooseThreshold{0.};
  float m_etaLooseThreshold{0.};
  float m_emfLowLooseThreshold{0.};
  float m_emfHighLooseThreshold{0.};
  float m_hecfLooseThreshold{0.};
  //Tight cleaning
  float m_fSampMaxTightThreshold{0.};
  float m_etaTightThreshold{0.};
  float m_emfLowTightThreshold{0.};
  float m_emfHighTightThreshold{0.};
  float m_hecfTightThreshold{0.};
  //Long-lived particle cleaning
  float m_fSampMaxLlpThreshold{0.};
  float m_negELlpThreshold{0.};
  float m_hecfLlpThreshold{0.};
  float m_hecqLlpThreshold{0.};
  float m_avLarQFLlpThreshold{0.};

  std::vector<CleanerBridge> m_cleaners;
  std::shared_ptr<IJetGrouper> m_grouper;
  Conditions m_conditions;
    
  double m_chainTimeAv{0.}; //std::chrono
  // double m_chainTimeSquareAv{0.}; //std::chrono
  unsigned int m_nCalls{0};
  
  bool m_dumpJets{false};
};

std::vector<double> getEtThresholds(const std::vector<double>& dEtas,
                                    const std::vector<double>& etThresholds);


#endif
