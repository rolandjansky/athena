/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigHLTJet_SMCTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "GaudiKernel/StatusCode.h"
#include "TrigHLTJetHypo_SMCTool.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/conditionsFactory2.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsSorter.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"


TrigHLTJetHypo_SMCTool::TrigHLTJetHypo_SMCTool(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent):
  TrigHLTJetHypoTool(type, name, parent) {

    declareProperty("EtThresholds",   m_EtThresholds );
    declareProperty("eta_mins",   m_etaMins);
    declareProperty("eta_maxs",   m_etaMaxs);
    declareProperty("asymmetricEtas",   m_asymmetricEtas);
    declareProperty("smc_mins",   m_JetMassMin );
    declareProperty("smc_maxs",   m_JetMassMax);
}


TrigHLTJetHypo_SMCTool::~TrigHLTJetHypo_SMCTool(){
}


Conditions TrigHLTJetHypo_SMCTool::getConditions() const {

    std::vector<double> JetMassMin_d = getStringBoundaries(m_JetMassMin);
    std::vector<double> JetMassMax_d = getStringBoundaries(m_JetMassMax);

    auto conditions = conditionsFactorysinglemass(m_etaMins,
            m_etaMaxs,
            m_EtThresholds,
            JetMassMin_d,
            JetMassMax_d);
    std::sort(conditions.begin(), conditions.end(), ConditionsSorter());

    return conditions;
}

std::vector<double> TrigHLTJetHypo_SMCTool::getStringBoundaries (const std::vector<std::string>& stv) const {

    std::vector<double> JetMassLimit;

    for (auto st : stv){

        if (st.find("-INF") != std::string::npos) {JetMassLimit.push_back(std::numeric_limits<double>::lowest());}

        else if (st.find("INF") != std::string::npos) {JetMassLimit.push_back(std::numeric_limits<double>::max());}

        else {JetMassLimit.push_back(std::stod(st)*s_GeV);}

    }

    return JetMassLimit;

}


 
std::unique_ptr<IJetGrouper> TrigHLTJetHypo_SMCTool::getJetGrouper() const {
  return std::make_unique<SingleJetGrouper>();
}

StatusCode TrigHLTJetHypo_SMCTool::checkVals() const {

    if (m_EtThresholds.size() != m_etaMins.size() or
            m_EtThresholds.size() != m_etaMaxs.size() or
            m_JetMassMin.size() != m_EtThresholds.size() or
            m_JetMassMax.size() != m_EtThresholds.size()) {

        ATH_MSG_ERROR(name()
                << ": mismatch between number of thresholds, "
                << " eta_min, eta_max, JetMassMin, JetMassMax boundaries: "
                << m_EtThresholds.size() << " "
                << m_etaMins.size() << " "
                << m_etaMaxs.size() << " "
                << m_JetMassMin.size() << " "
                << m_JetMassMin.size() << " "
                );
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}
    
std::vector<std::shared_ptr<ICleaner>> TrigHLTJetHypo_SMCTool::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
} 
