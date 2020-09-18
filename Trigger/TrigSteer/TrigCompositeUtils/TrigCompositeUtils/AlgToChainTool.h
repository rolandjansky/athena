/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
      AlgToChainTool(const std::string& type, const std::string& name, const IInterface* parent);
      virtual ~AlgToChainTool();

      virtual StatusCode initialize() override;

      virtual StatusCode start() override;

      /// Request set of chains for given algorithm - static lookup
      std::set<std::string> getChainsForAlg(const std::string& algorithmName) const;

      /// Request set of active chains for given algorithm - dynamic lookup
      std::set<std::string> getActiveChainsForAlg(const std::string& algorithmName, const EventContext& context) const;

  private:
      std::set<std::string> retrieveActiveChains(const EventContext& context) const;

      SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{ this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu" };

      std::map<std::string, std::vector<std::string>> m_sequencerToChainMap;
      std::map<std::string, std::vector<std::string>> m_algToSequencersMap;
  };
}

#endif // XAOD_STANDALONE

#endif // TrigCompositeUtils_AlgToChainTool_h