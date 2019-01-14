/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_STREAMTAGMAKERTOOL_H
#define TRIGOUTPUTHANDLING_STREAMTAGMAKERTOOL_H


#include <string>
#include <map>
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigOutputHandling/HLTResultMTMakerTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "Gaudi/Parsers/Factory.h" // needed to declare less common Property types
#include "eformat/StreamTag.h"

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

private:
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalChainDecisions { this, "ChainDecisions", "HLTSummary", 
    "Container with final chain decisions"  }; 

  Gaudi::Property<bool> m_allowRerunChains { this, "AllowRerunChains", false, 
    "Normally false, but if set to true this will allow resurrected chains which ran in the second pass to also add stream tags"};

  Gaudi::Property<std::map<std::string, std::string>> m_chainToStreamProperty { this, "ChainToStream", {}, 
    "Mapping from the chain name to string name (temporary solution, will be replaced)"};

  Gaudi::Property<std::map<std::string, std::vector<uint32_t>>> m_streamSubDets { this, "StreamSubDets", {}, 
    "Mapping from the stream name to subdetector IDs (temporary solution, will be replaced)"};

  Gaudi::Property<std::map<std::string, std::vector<uint32_t>>> m_streamRobs { this, "StreamRobs", {}, 
    "Mapping from the stream name to ROB IDs (temporary solution, will be replaced)"};

  typedef std::map< TrigCompositeUtils::DecisionID, eformat::helper::StreamTag> ChainToStreamMap;
  ChainToStreamMap m_mapping;
};

#endif // TRIGOUTPUTHANDLING_STREAMTAGMAKERTOOL_H
