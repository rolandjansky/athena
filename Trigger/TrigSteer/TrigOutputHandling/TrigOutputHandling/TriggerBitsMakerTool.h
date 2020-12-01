/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGOUTPUTHANDLING_TRIGGERBITSMAKERTOOL_H
#define TRIGOUTPUTHANDLING_TRIGGERBITSMAKERTOOL_H

#include <string>
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigOutputHandling/HLTResultMTMakerTool.h"
#include "TrigOutputHandling/ITriggerBitsMakerTool.h"
#include "TrigConfData/HLTMenu.h"

/**
 * @class TriggerBitsMakerTool
 * @brief Obtains trigger bits from Navigation summary via getBits and (online) fills trigger bits in the HLTResultMT object
 **/
class TriggerBitsMakerTool : public extends<AthAlgTool, HLTResultMTMakerTool, ITriggerBitsMakerTool> {
public:
  TriggerBitsMakerTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TriggerBitsMakerTool() override = default;

  virtual StatusCode fill( HLT::HLTResultMT& resultToFill, const EventContext& ctx ) const override;

  virtual StatusCode getBits(boost::dynamic_bitset<uint32_t>& passRaw,
    boost::dynamic_bitset<uint32_t>& prescaled,
    boost::dynamic_bitset<uint32_t>& rerun,
    const EventContext& ctx) const override;
  
  virtual StatusCode initialize() override;
  virtual StatusCode start() override;

private:

  /**
   * @brief Set to 1 the bit correspinding to 'chain' in 'resultToFill'
   **/
  StatusCode setBit(const TrigCompositeUtils::DecisionID chain, boost::dynamic_bitset<uint32_t>& resultToFill) const;

  /**
   * @brief Check that a chain's hash in the menu JSON (via python) agrees with the C++ implementation
   **/
  StatusCode hashConsistencyCheck(const std::string& chain, const size_t hash) const;

  /**
   * @brief Check that no existing key maps to a given value and that the string is not empty
   **/
  StatusCode preInsertCheck(const std::string& chain, const uint32_t bit) const;

  SG::ReadHandleKey<TrigCompositeUtils::DecisionContainer> m_finalChainDecisions { this, "ChainDecisions", "HLTNav_Summary",
    "Container with final chain decisions"  };

  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu"};

  Gaudi::Property<std::map<std::string, uint32_t>> m_extraChainToBit { this, "ExtraChainToBit", {},
    "Special case and testing purposes hard-coded chain-to-bit mappings to use in addition to those from the HLT menu."};

  using ChainToBitMap = std::map< TrigCompositeUtils::DecisionID, uint32_t>;
  ChainToBitMap m_mapping; //!< Mapping of each chain's hash ID to its chain counter

  uint32_t m_largestBit{0}; //!< Largest chain counter hence largest bit needed to be stored in result bitmap
};

#endif // TRIGOUTPUTHANDLING_TRIGGERBITSMAKERTOOL_H
