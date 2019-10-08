/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_ROISUNPACKINGTOOLBASE_H
#define L1DECODER_ROISUNPACKINGTOOLBASE_H

#include "IRoIsUnpackingTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfData/HLTMenu.h"

namespace ROIB {
  class RoIBResult;
}


/**
 * Base class for RoI unpackers
 *
 * Provides common features needed by all unpackers. By itself it does not
 * do anything useful.
 */
class RoIsUnpackingToolBase : public extends<AthAlgTool, IRoIsUnpackingTool> { 
public: 
  RoIsUnpackingToolBase(const std::string& type,
                        const std::string& name, 
                        const IInterface* parent);

  typedef HLT::IDtoIDVecMap ThresholdToIdentifiers;
  
  virtual StatusCode initialize() override;

  
  virtual StatusCode updateConfiguration( const IRoIsUnpackingTool::SeedingMap& /*seeding*/ ) override { return StatusCode::SUCCESS; }
  
  virtual StatusCode unpack(const EventContext& /*ctx*/,
                            const ROIB::RoIBResult& /*roib*/,
                            const HLT::IDSet& /*activeChains*/) const override { return StatusCode::SUCCESS; }


protected:
  
  ///@{ @name Properties
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{
    this, "Decisions", "RoIDecisions", "Decisions for each RoI"};

  Gaudi::Property<std::vector<std::string>> m_thresholdToChainProperty{
    this, "ThresholdToChainMapping", {}, "Mapping from the threshold name to chain in the form: "
                                         "'EM3 : HLT_e5', 'EM3 : HLT_e5tight', ..., ( note spaces ), if absent, the L1 item -> HLT chain seeding relation is used to find the threshold"};

  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu",
      "HLT Menu"};

  ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
  ///@}

  std::map<HLT::Identifier, HLT::IDVec> m_thresholdToChainMapping;
  /**
   * decodes mapping from L1 item -> to HLT chain  to find out which threshold seeds which chain
   * Threshold have to pass the selection of the filter (i.e. the filter should return true for the threshold to seed a chain)
   **/
  StatusCode decodeMapping( std::function< bool(const TrigConf::TriggerThreshold*)> filter, const TrigConf::ItemContainer& l1Items, const IRoIsUnpackingTool::SeedingMap& seeding );

  StatusCode decodeMapping( std::function< bool(const std::string&)> filter ) ;

  void addChainsToDecision( HLT::Identifier thresholdId,
                            TrigCompositeUtils::Decision* d,
                            const HLT::IDSet& activeChains ) const;

  //!< copies thresholds from src to dest filtering nulls and making verbose messages
  StatusCode copyThresholds( const std::vector<TrigConf::TriggerThreshold*>& src, 
			     std::vector<TrigConf::TriggerThreshold*>& dest ) const;
  
}; 


#endif
