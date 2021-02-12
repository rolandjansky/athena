/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1DECODER_ROISUNPACKINGTOOLBASE_H
#define L1DECODER_ROISUNPACKINGTOOLBASE_H

#include "IRoIsUnpackingTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfData/HLTMenu.h"
#include "xAODTrigger/TrigComposite.h"

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

  
  virtual StatusCode updateConfiguration( ) override { return StatusCode::SUCCESS; }
  
  virtual StatusCode unpack(const EventContext& /*ctx*/,
                            const ROIB::RoIBResult& /*roib*/,
                            const HLT::IDSet& /*activeChains*/) const override { return StatusCode::SUCCESS; }

  virtual StatusCode unpack(const EventContext& /*ctx*/,
                            const xAOD::TrigComposite& /*l1TriggerResult*/,
                            const HLT::IDSet& /*activeChains*/) const override { return StatusCode::SUCCESS; }


protected:
  
  ///@{ @name Properties
  SG::WriteHandleKey<TrigCompositeUtils::DecisionContainer> m_decisionsKey{
    this, "Decisions", "RoIDecisions", "Decisions for each RoI"};

  SG::ReadHandleKey<TrigConf::HLTMenu> m_HLTMenuKey{
    this, "HLTTriggerMenu", "DetectorStore+HLTTriggerMenu", "HLT Menu"};

  ToolHandle<GenericMonitoringTool> m_monTool{
    this, "MonTool", "", "Monitoring tool"};
  ///@}

  std::map<HLT::Identifier, HLT::IDVec> m_thresholdToChainMapping;
  std::map<HLT::Identifier, HLT::Identifier> m_legToChainMapping;
  /**
   * Fills mapping from L1 threshold -> to HLT chain
   * @arg filter is a function that, using the threshold name defines if this decoder instance should take care of this threshold
   **/

  StatusCode decodeMapping( std::function< bool(const std::string&)> filter ) ;

  void addChainsToDecision( HLT::Identifier thresholdId,
                            TrigCompositeUtils::Decision* d,
                            const HLT::IDSet& activeChains ) const;

  //!< copies thresholds from src to dest filtering nulls and making verbose messages
  StatusCode copyThresholds( const std::vector<TrigConf::TriggerThreshold*>& src, 
			     std::vector<TrigConf::TriggerThreshold*>& dest ) const;
  
}; 


#endif
