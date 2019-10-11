/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_STREAMTAGMAKERTOOL_H
#define TRIGOUTPUTHANDLING_STREAMTAGMAKERTOOL_H

// Trigger includes
#include "TrigConfData/HLTMenu.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigOutputHandling/HLTResultMTMakerTool.h"
#include "TrigPartialEventBuilding/PEBInfoWriterToolBase.h" // Defines PEBInfo and keys to retrieve it

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Gaudi includes
#include "Gaudi/Parsers/Factory.h" // Needed to declare less common Property types

// System includes
#include <string>
#include <map>
#include <unordered_map>
#include <tuple>

/**
 * @class StreamTagMakerTool
 * @brief makes stream tags out of chain decisions
 **/
class StreamTagMakerTool : public extends<AthAlgTool, HLTResultMTMakerTool> {
public:
  StreamTagMakerTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~StreamTagMakerTool() override;

  virtual StatusCode fill( HLT::HLTResultMT& resultToFill ) const override;

  virtual StatusCode initialize() override;

  virtual StatusCode finalize() override;

  /// Type describing StreamTag information needed by the tool: {name, type, obeysLumiBlock, forceFullEventBuilding}
  typedef std::tuple<std::string, std::string, bool, bool> StreamTagInfo;

private:

  SG::ReadHandleKey<TrigConf::HLTMenu> m_hltMenuKey{"DetectorStore+HLTTriggerMenu"};

  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalChainDecisions {this, "ChainDecisions", "HLTNav_Summary",
    "Container with final chain decisions" };

  SG::ReadHandleKeyArray<TrigCompositeUtils::DecisionContainer> m_pebDecisionKeys {this, "PEBDecisionKeys", {},
    "Decisions including PEBInfo" };

  Gaudi::Property<bool> m_allowRerunChains {this, "AllowRerunChains", false,
    "Normally false, but if set to true this will allow resurrected chains which ran in the second pass to also add stream tags"};

  Gaudi::Property<std::string> m_menuJSON {this, "HLTmenuFile", "UNSET",
    "Filename of just-generated HLT Menu JSON used to configure the TriggerBitsMakerTool"};

  /// Chain to streams map filled from the HLT Menu JSON
  std::unordered_map<TrigCompositeUtils::DecisionID, std::vector<StreamTagInfo> > m_mapping;

  /// Helper method to fill the chainID->PEBInfo map
  StatusCode fillPEBInfoMap(std::unordered_map<TrigCompositeUtils::DecisionID, PEBInfoWriterToolBase::PEBInfo>& map) const;
};

/// operator<< for StreamTagInfo
std::ostream& operator<< (std::ostream& str, const StreamTagMakerTool::StreamTagInfo& info) {
  str << "[" << std::get<0>(info) << ", " << std::get<1>(info) << ", " << std::get<2>(info) << ", " << std::get<3>(info) << "]";
  return str;
}

#endif // TRIGOUTPUTHANDLING_STREAMTAGMAKERTOOL_H
