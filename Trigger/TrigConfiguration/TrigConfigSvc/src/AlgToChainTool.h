/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_ALGTOCHAINTOOL_H
#define TRIGCONFDATA_ALGTOCHAINTOOL_H

#include <string>
#include <set>
#include <vector>
#include <map>

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigConfData/HLTMenu.h"


namespace TrigConf {

  /** @class AlgToChainTool
   *  @brief Provide the reverse mapping: algorithms to set fo chain names
   **/

  class AlgToChainTool : public AthAlgTool {
  public:
      AlgToChainTool(const std::string& type, const std::string& name, const IInterface* parent);
      virtual ~AlgToChainTool();

      virtual StatusCode initialize() override;

      virtual StatusCode start() override;

      /// Request set of chains for given algorithm
      std::set<std::string> getChainsForAlg(const std::string& algorithmName) const;

  private:
      SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{ this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu" };
      std::map<std::string, std::vector<std::string>> m_sequencerToChainMap;
      std::map<std::string, std::vector<std::string>> m_algToSequencersMap;
  };
}

#endif // TRIGCONFDATA_ALGTOCHAINTOOL_H
