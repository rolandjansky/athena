/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_ENHANCEDBIASWEIGHTCOMPALG_H
#define TRIG_ENHANCEDBIASWEIGHTCOMPALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include <vector>
#include <string>

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_unordered_map.h>

/**
 *  @class EnhancedBiasWeightCompAlg
 *  @brief Compute Enhanced Bias weight
 **/

class EnhancedBiasWeightCompAlg : public AthReentrantAlgorithm {
  public:

    EnhancedBiasWeightCompAlg(const std::string& name, ISvcLocator* svcLoc);

    virtual StatusCode initialize() override;
    virtual StatusCode start() override;
    virtual StatusCode execute(const EventContext& context) const override;
    virtual StatusCode stop() override;

  private:
    /**
     *  @class EBChainInfo
     *  @brief Store necessary information for weight computing for Enhanced Bias chain 
     **/

    class EBChainInfo {
      public:
        EBChainInfo(const HLT::Identifier & id) : m_id(id), m_totalPrescale(1.0) {}

        std::string getName() const {return m_id.name();}
        TrigCompositeUtils::DecisionID getId() const {return m_id;}
        float getPrescale() const {return m_totalPrescale;}
        bool getIsRandom() const {return (getName().find("HLT_noalg_eb_L1RD") != std::string::npos);}
        bool getIsNoPS() const {return (getName().find("noPS") != std::string::npos);}

        void setPrescale(const float& prescale) {m_totalPrescale = prescale;}

      private:
        /// HLTID of the chain
        HLT::Identifier m_id;

        /// Value of total prescale L1 * HLT
        float m_totalPrescale;
    };

    /**
     *  @class EBResult
     *  @brief Store result of EB weight calculation
     **/
    struct EBResult {
      float weight {1.0};
      uint8_t isUnbiased {};
    };

    SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu"};
    ToolHandle<Trig::TrigDecisionTool> m_tdt{this, "TrigDecisionTool", ""};
    SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalDecisionKey{ this, "FinalDecisionKey", "", "Final stage of all decisions" };
    SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_EBWeightKey {this, "EBWeight", "HLT_EBWeight", "Computed EB Weight"};

    SG::ReadCondHandleKey<TrigConf::HLTPrescalesSet> m_HLTPrescaleSetInputKey {this, "HLTPrescales", "HLTPrescales", "HLT prescales set"};
    SG::ReadCondHandleKey<TrigConf::L1PrescalesSet> m_L1PrescaleSetInputKey {this, "L1Prescales", "L1Prescales", "L1 prescales set"};

    /// Calculate EB result based on total prescales of chains
    EBResult calculateEBWeight(const std::vector<EBChainInfo>& EBChains) const;

    /// Retrieve total prescales (L1 * HLT) for chains into map
    StatusCode getPrescales(const EventContext& context, std::vector<EBChainInfo>& EBChains) const;

    /// Get list of Enhanced Bias chains that passed
    std::vector<EBChainInfo> getPassedEBChains(const TrigCompositeUtils::Decision& decisionObject) const;

    /// Check if any of random chains passed
    bool checkIfTriggeredByRandomChain(const std::vector<EBChainInfo>& EBChain) const;

    /// Parse list of l1 items
    std::vector<std::string> parseItems(const std::string& itemStr);

    /// Available EB chains' IDs from HLT Menu
    std::vector<TrigCompositeUtils::DecisionID> m_EBChainIds;

    /// L1 items for EB chains from HLT Menu
    std::map<TrigCompositeUtils::DecisionID, std::vector<std::string>> m_EBChainIdToItem;

    /// Name of xml output file
    mutable std::string m_outputFilename ATLAS_THREAD_SAFE;

    /// List of calculated weight + unbiased flag pairs to be saved in xml
    mutable tbb::concurrent_vector<std::pair<int, bool>> m_ebWeights ATLAS_THREAD_SAFE;

    /// Map of event number to index in m_ebWeights vector to be saved in xml
    mutable tbb::concurrent_unordered_map<int, int> m_eventToWeight ATLAS_THREAD_SAFE;

};

#endif //TRIG_ENHANCEDBIASWEIGHTCOMPALG_H