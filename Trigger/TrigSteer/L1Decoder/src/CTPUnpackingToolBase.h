/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1DECODER_CTPUNPACKINGTOOLBASE_H
#define L1DECODER_CTPUNPACKINGTOOLBASE_H 1

#include "ICTPUnpackingTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"


namespace ROIB {
  class RoIBResult;
}

/**
 * Base class for CTP unpacking tools
 *
 * Provides common features needed by all CPT unpacker.
 */
class CTPUnpackingToolBase : public extends<AthAlgTool, ICTPUnpackingTool> { 
public:
  
  CTPUnpackingToolBase(const std::string& type,
                       const std::string& name, 
                       const IInterface* parent);

  virtual StatusCode decode(const ROIB::RoIBResult& /*roib*/, 
                            HLT::IDVec& /*enabledChains*/) const override
  { return StatusCode::SUCCESS; }
  
  virtual StatusCode initialize() override;
   
protected:

  StatusCode decodeCTPToChainMapping();
  typedef std::map<size_t, HLT::IDVec> IndexToIdentifiers;
  IndexToIdentifiers       m_ctpToChain;

  ///@{ @name Properties
  Gaudi::Property<std::vector<std::string>> m_ctpToChainProperty{
    this, "CTPToChainMapping", {}, "Mapping of the form: '34:HLT_x', '35:HLT_y', ..., both "
                                   "CTP ID and chain may appear many times"};

  Gaudi::Property<bool> m_forceEnable{
    this, "ForceEnableAllChains", false, "Enables all chains in each event, testing mode"};

  ToolHandle<GenericMonitoringTool> m_monTool{this, "MonTool", "", "Monitoring tool"};
  ///@}
};

#endif
