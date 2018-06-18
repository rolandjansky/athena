/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPOTOOL_ETAET_H
#define TRIGJETHYPOTOOL_ETAET_H
/********************************************************************
 *
 * NAME:     TrigJetHypo_EtaEtTool.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 *           
 *
 *********************************************************************/

#include "AsgTools/AsgTool.h"
#include "TrigHLTJetHypo/ITrigJetHypoTool.h"
#include "TrigHLTJetHypoTool.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"
#include "DecisionHandling/HLTIdentifier.h"


class TrigJetHypoTool_EtaEt:
public asg::AsgTool, virtual public ITrigJetHypoTool{
ASG_TOOL_CLASS(TrigJetHypoTool_EtaEt, ITrigJetHypoTool)

public:

TrigJetHypoTool_EtaEt(const std::string& name);
virtual ~TrigJetHypoTool_EtaEt() override;
virtual StatusCode initialize() override;
virtual StatusCode finalize() override;

  // ITrigJetHypo interface
virtual StatusCode decide(const xAOD::JetContainer*, bool&) const override;
virtual TrigCompositeUtils::DecisionID decisionId() const override; 


 private:
// vectors with Et thresholds, eta mins and eta maxs
// (thresh, eta min, eta max) triplets will be converted to Conditon objs.
std::vector<double> m_EtThresholds;
std::vector<double> m_etaMins;
std::vector<double> m_etaMaxs;
std::vector<int> m_asymmetricEtas;
bool m_acceptAll;
std::string m_chainName;
std::string m_matchingAlg;  // determines matcher obj;

Conditions m_conditions;

bool m_dumpJets{false};

HLT::Identifier m_id;

StatusCode checkVals()  const;
std::vector<std::shared_ptr<ICleaner>> getCleaners() const;
std::unique_ptr<IJetGrouper> getJetGrouper() const;
Conditions getConditions() const;
void setCleaners();

void writeDebug(bool,
                  const HypoJetVector&,
                  const HypoJetVector&) const;



private:

std::vector<CleanerBridge> m_cleaners;
std::string m_cleaningAlg;

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

};
#endif
