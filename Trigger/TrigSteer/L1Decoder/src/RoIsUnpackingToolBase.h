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

  virtual StatusCode updateConfiguration() override { return StatusCode::SUCCESS; }
  
  virtual StatusCode unpack(const EventContext& /*ctx*/,
                            const ROIB::RoIBResult& /*roib*/,
                            const HLT::IDSet& /*activeChains*/) const override { return StatusCode::SUCCESS; }
  
protected:
  
  ///@{ @name Properties
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{
    this, "Decisions", "RoIDecisions", "Decisions for each RoI"};

  Gaudi::Property<std::vector<std::string>> m_thresholdToChainProperty{
    this, "ThresholdToChainMapping", {}, "Mapping from the threshold name to chain in the form: "
                                         "'EM3 : HLT_e5', 'EM3 : HLT_e5tight', ..., ( note spaces )"};

  ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
  ///@}

  std::map<HLT::Identifier, HLT::IDVec> m_thresholdToChainMapping;

  StatusCode decodeMapping();
  void addChainsToDecision( HLT::Identifier thresholdId,
                            TrigCompositeUtils::Decision* d,
                            const HLT::IDSet& activeChains ) const;
}; 


#endif
