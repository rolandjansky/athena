/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRIGGERBITSMAKERTOOL_H
#define TRIGOUTPUTHANDLING_TRIGGERBITSMAKERTOOL_H

#include <string>
#include "DecisionHandling/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigOutputHandling/HLTResultMTMakerTool.h"


/**
 * @class TriggerBitsMakerTool
 * @brief fills trigger bits in the HLTResultMT object
 **/
class TriggerBitsMakerTool : public extends<AthAlgTool, HLTResultMTMakerTool> {
public:
  TriggerBitsMakerTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TriggerBitsMakerTool() override;

  virtual StatusCode fill( HLT::HLTResultMT& resultToFill ) const override;
  StatusCode fill( std::vector<uint32_t>& place ) const;
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

private:
  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalChainDecisions { this, "ChainDecisions", "HLTSummary", "Container with final chain decisions"  }; 

  Gaudi::Property<std::map<std::string, int>> m_chainToStreamProperty { this, "ChainToBit", {}, "Mapping from the chain name to bit position in trigger bits array"};

  typedef std::map< TrigCompositeUtils::DecisionID, int> ChainToBitMap;
  ChainToBitMap m_mapping;


};

#endif // TRIGOUTPUTHANDLING_TRIGGERBITSMAKERTOOL_H
