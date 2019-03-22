/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOLMT_H
#define TRIGJETHYPOTOOLMT_H
/********************************************************************
 *
 * NAME:     TrigJetHypoToolMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 *
 *********************************************************************/


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

#include "ITrigJetHypoToolMT.h"
#include "ITrigJetHypoToolConfig.h"

class TrigJetHypoToolMT: public extends<AthAlgTool, ITrigJetHypoToolMT> {

 public:
  
  TrigJetHypoToolMT(const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~TrigJetHypoToolMT();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // ITrigJetHypoToolMT interface
  virtual StatusCode
    decide(const xAOD::JetContainer*, bool& pass) const override;
  virtual const HLT::Identifier& getId() const override; 
 private:

  // Identifier is used to keep track of which tool made which decision.
  // The information is stored in the event store.
  HLT::Identifier m_decisionId;  


  // Paraphanalia needed for the Jet Hypo Helper class:
  Conditions m_conditions;

  Gaudi::Property<bool> m_dumpJets{this, "dumpJets", {}, "verbose jet dump flag"};
  
  void setCleaners();
 
  void writeDebug(bool,
                  const HypoJetVector&,
                  const HypoJetVector&) const;

  

 private:

 std::vector<CleanerBridge> m_cleaners;

 // Used to generate helper objects foe TrigHLTJetHelper
 // from user supplied values
 ToolHandle<ITrigJetHypoToolConfig> m_config {
   this, "HypoConfigurer", {}, "Configurer to set up TrigHLTJetHypoHelper2"}; 


 // Cleaning parameters
 
 Gaudi::Property<std::string>
   m_cleaningAlg{this, "cleaningAlg", "noCleaning", "Cleaning Alg"};
 
 //basic cleaning
 Gaudi::Property<float>
   m_n90Threshold{this, "n90CleaningThreshold", 2., "" };
 
 Gaudi::Property<float>
   m_presamplerThreshold{this, "presamplerCleaningThreshold", 0.9, ""};
 
 Gaudi::Property<float>
   m_negativeEThreshold{this, "negativeECleaningThreshold", -60e3, ""};//60 GeV
 
 Gaudi::Property<float>
   m_qmeanThreshold {this, "qmeanCleaningThreshold", 0.8, ""};
 
 Gaudi::Property<float>
   m_hecQThreshold {this, "HECQCleaningThreshold", 0.5, ""};
 
 Gaudi::Property<float>
   m_hecFThreshold {this, "HECfCleaningThreshold", 0.5, ""};
 
 Gaudi::Property<float>
   m_larQThreshol {this, "LArQCleaningThreshold", 0.8, ""};
 
 Gaudi::Property<float>
   m_emFThreshold {this, "EMfCleaningThreshold", 0.95, ""};
 
 
 //loose cleaning
 Gaudi::Property<float>
   m_fSampMaxLooseThreshold {this, "fracSamplingMaxLooseThreshold", 0.8, ""};

 Gaudi::Property<float>
   m_etaLooseThreshold{this, "etaLooseThreshold", 2.0, ""};
 
 Gaudi::Property<float>
   m_emfLowLooseThreshold{this, "EMfLowLooseThreshold", 0.10, ""};

 Gaudi::Property<float>
   m_emfHighLooseThreshold{this, "EMfHighLooseThreshold", 0.99, ""};
 
 Gaudi::Property<float>
   m_hecfLooseThreshold{this, "HECfLooseThreshold", 0.85, ""};
 
 //tight cleaning
 Gaudi::Property<float>
   m_fSampMaxTightThreshold{this, "fracSamplingMaxTightThreshold", 0.8, ""};
 
 Gaudi::Property<float>
   m_etaTightThreshold {this, "etaTightThreshold",  2.0, ""};
 
 Gaudi::Property<float>
   m_emfLowTightThreshold{this, "EMfLowTightThreshold", 0.10, ""};
 
 Gaudi::Property<float>
   m_emfHighTightThreshold{this, "EMfHighTightThreshold", 0.99, ""};
 
 Gaudi::Property<float>
   m_hecfTightThreshold{this, "HECfTightThreshold", 0.85, ""};
 
 //long-lived particle cleaning
 Gaudi::Property<float>
   m_fSampMaxLlpThreshold {this, "fracSamplingMaxLlpThreshold", 0.85, ""};
 
 Gaudi::Property<float>
   m_negELlpThreshold{this, "negativeELlpThreshold", 10e3, ""}; // 10 GeV

 Gaudi::Property<float>
   m_hecfLlpThreshold{this, "HECfLlpThreshold", 0.5, ""};
 
 Gaudi::Property<float>
   m_hecqLlpThreshold{this, "HECQLlpThreshold", 0.5, ""};
 
 Gaudi::Property<float>
   m_avLarQFLlpThreshold{this, "AverageLArQFLlpThreshold", 0.8*65535, ""};

 Gaudi::Property<bool>
   m_acceptAll{this, "AcceptAll", false, "flag to turn of hypo"};
 

 
 // Monitored variables...
 /*
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
*/


};
#endif
