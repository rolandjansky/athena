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
  enum BitCategory{ HLTPassRawCategory, HLTPrescaledCategory, HLTRerunCategory };

  StatusCode setBit(const TrigCompositeUtils::DecisionID chain, const BitCategory category, HLT::HLTResultMT& resultToFill) const;

  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalChainDecisions { this, "ChainDecisions", "HLTNav_Summary", "Container with final chain decisions"  };

  Gaudi::Property<std::string> m_menuJSON {this, "HLTmenuFile", "UNSET", "Filename of just-generated HLT Menu JSON used to configure the TriggerBitsMakerTool"};

  typedef std::map< TrigCompositeUtils::DecisionID, uint32_t> ChainToBitMap;
  ChainToBitMap m_mapping; //!< Mapping of each chain's hash ID to its chain counter

  uint32_t m_largestBit; //!< Largest chain counter hence largest bit needed to be stored in result bitmap
};

#endif // TRIGOUTPUTHANDLING_TRIGGERBITSMAKERTOOL_H
