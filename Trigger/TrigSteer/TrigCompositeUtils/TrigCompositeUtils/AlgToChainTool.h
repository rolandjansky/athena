/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigCompositeUtils_AlgToChainTool_h
#define TrigCompositeUtils_AlgToChainTool_h

#ifndef XAOD_STANDALONE

#include <string>
#include <set>
#include <vector>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigConfData/HLTMenu.h"


namespace TrigCompositeUtils {

  /** @class AlgToChainTool
   *  @brief Provide the reverse mapping: algorithm name to set of chain names
   **/

  class AlgToChainTool : public AthAlgTool {
  public:
     // Helper struct to save chain info
    struct ChainInfo {
        TrigCompositeUtils::DecisionID id;
        std::string name;
        std::vector<std::string> groups;
        bool isPassRaw;
      };

      AlgToChainTool(const std::string& type, const std::string& name, const IInterface* parent);

      virtual ~AlgToChainTool();

      virtual StatusCode initialize() override;

      virtual StatusCode start() override;

      /// Request set of chains' names for given algorithm - static lookup
      std::set<std::string> getChainsNamesForAlg(const std::string& algorithmName) const;

      /// Request set of chains for given algorithm - static lookup
      std::vector<TrigConf::Chain> getChainsForAlg(const std::string& algorithmName) const;

      /// Request set of active chains for given algorithm - dynamic lookup
      std::set<std::string> getActiveChainsForAlg(const std::string& algorithmName, const EventContext& context) const;

      /// Request set of chains for all algorithms in the menu
      std::map<std::string, std::vector<TrigConf::Chain>> getChainsForAllAlgs(const EventContext& context) const;

      /// Request set of chains from given navigation collection
      std::set<TrigCompositeUtils::DecisionID> retrieveActiveChains(const EventContext& context, const std::string& collectionName = "") const;

      /// Request all chains
      StatusCode getAllChains(std::vector<TrigConf::Chain>&) const;

      /// Retrieve chain information for gived chain id
      StatusCode getChainInfo(const EventContext& context, TrigCompositeUtils::DecisionID id, ChainInfo& info) const;

  private:
      SG::ReadHandle<TrigCompositeUtils::DecisionContainer> getDecisionFromStore(SmartIF<SGImplSvc>& eventStore, const std::string& key) const;

      SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{ this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu" };

      std::map<std::string, std::vector<TrigConf::Chain>> m_sequencerToChainMap;
      std::map<std::string, std::vector<std::string>> m_algToSequencersMap;
  };
}

#endif // XAOD_STANDALONE

#endif // TrigCompositeUtils_AlgToChainTool_h