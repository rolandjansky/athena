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

      /// Request set of chains for given algorithm - static lookup (menu)
      std::vector<std::string> getChainsForAlg(const std::string& algorithmName) const;

      /// Request set of active chains for given algorithm - dynamic lookup (navigation)
      std::set<std::string> getActiveChainsForAlg(const std::string& algorithmName, const EventContext& context) const;

      /// Request set of chains for all algorithms in the menu
      StatusCode getChainsForAllAlgs(const EventContext& context, std::map<std::string, std::set<std::string>>& algToChain) const;

      /// Read the cached or non cached SG kays and request set of chains for given navigation collection
      std::set<TrigCompositeUtils::DecisionID> retrieveActiveChains(const EventContext& context, const std::string& collectionName = "") const;

      /// Request all chains from the menu
      StatusCode getAllChains(std::vector<TrigConf::Chain>&) const;

      /// Retrieve chain information for gived chain id
      StatusCode getChainInfo(const EventContext& context, TrigCompositeUtils::DecisionID id, ChainInfo& info) const;

      /// Retrieve algorithms and their active sequences
      StatusCode getAllActiveSequences(const EventContext& context, std::map<std::string, std::string>& algToSeq);

      /// Cache the StoreGate keys
      void cacheSGKeys(const EventContext& context);

  private:
      SG::ReadHandle<TrigCompositeUtils::DecisionContainer> getDecisionFromStore(SmartIF<SGImplSvc>& eventStore, const std::string& key) const;

      SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{ this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu" };

      std::map<std::string, std::vector<TrigConf::Chain>> m_sequencerToChainMap;
      std::map<std::string, std::vector<std::string>> m_algToSequencersMap;

      std::vector<std::string> m_cachedEventStoreKeys;
      uint64_t  m_cachedEventID;

      // Based on the sequence name create the first part of the name of collection from the decision store
      std::string createCollectionName(const std::string& sequenceName) const;

      /// Read the keys from StoreGate
      std::vector<std::string> readSGKeys(const EventContext& context) const;

      /// Request set of chains for given navigation collection and list of StoreGate keys
      std::set<TrigCompositeUtils::DecisionID> retrieveActiveChainsForKeys(const EventContext& context, const std::string& collectionName, const std::vector<std::string>& keys) const;
  };
}

#endif // XAOD_STANDALONE

#endif // TrigCompositeUtils_AlgToChainTool_h