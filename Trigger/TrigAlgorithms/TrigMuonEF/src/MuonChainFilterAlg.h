/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEF_MUONCHAINFILTERALG_H
#define TRIGMUONEF_MUONCHAINFILTERALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

/**
 * @class MuonChainFilterAlg filter to (de)select chains for muon trigger algorithms
 * initially designed to pass if at least one active chain is NOT in the list to be filtered (NotGate = false)
 * the filter will also pass if NotGate = true and at least one active chain is FOUND in the list
 **/
class MuonChainFilterAlg : public ::AthAlgorithm {
 public:
  MuonChainFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode finalize() { return StatusCode::SUCCESS; }
  StatusCode execute();

 private:
    StatusCode createDummyMuonContainers();

    Gaudi::Property<std::vector<std::string>> m_filterChains {this, "ChainsToFilter", {}, "Vector of chains to filter out"};
    SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_inputDecisionKeys{this, "InputDecisions", {}, "Inputs to the filter"};

    Gaudi::Property<bool> m_writeL2muComb {this, "WriteMuComb", false, "Flag to record muComb muons"};
    SG::WriteHandleKey<xAOD::L2CombinedMuonContainer> m_muCombKey {this, "L2MuCombContainer", "MuonL2CBInfo", "Output container for muComb"};

    Gaudi::Property<bool> m_writeL2muFast {this, "WriteMuFast", false, "Flag to record muFast muons"};
    SG::WriteHandleKey<xAOD::L2StandAloneMuonContainer> m_muFastKey {this, "L2MuFastContainer", "MuonL2SAInfo", "Output container for muFast"};

    Gaudi::Property<bool> m_notGate {this, "NotGate", false, "filtering if not contained in ChainsToFilter"};

    TrigCompositeUtils::DecisionIDContainer m_filterChainIDs;
};

#endif
