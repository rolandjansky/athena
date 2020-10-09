// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGDECISIONTOOL_DECISIONUNPACKERSTANDALONE_H
#define TRIGDECISIONTOOL_DECISIONUNPACKERSTANDALONE_H

// ASG include(s):
#include "AsgMessaging/AsgMessaging.h"

// Trigger configuration include(s):
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfL1Data/CTPConfig.h"

// Local include(s):
#include "TrigDecisionTool/IDecisionUnpacker.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/EventPtrDef.h"

// xAOD include(s):
#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigNavigation.h"

#include "AsgDataHandles/ReadHandleKey.h"

// Forward declaration(s):
namespace HLT {
  class TrigNavStructure;
}


namespace TrigConf {
  class ITrigConfigTool;
}

namespace Trig {

  // Forward declaration(s):
  class DecisionObjectHandleStandalone;

  /// Helper class for unpacking the xAOD trigger decision/navigation
  class DecisionUnpackerStandalone : public IDecisionUnpacker, public Logger {

  public:
    /// Constructor with arguments
    DecisionUnpackerStandalone(SG::ReadHandleKey<xAOD::TrigDecision>*,
				SG::ReadHandleKey<xAOD::TrigNavigation>* navikey);
    /// Destructor
    virtual ~DecisionUnpackerStandalone();

    DecisionUnpackerStandalone( const DecisionUnpackerStandalone& ) = delete;
    DecisionUnpackerStandalone& operator= ( const DecisionUnpackerStandalone& ) = delete;

    /// Function unpacking the payload of the trigger decision
    virtual StatusCode
    unpackDecision( std::unordered_map< std::string,
		    const LVL1CTP::Lvl1Item* >& itemsByName,
		    std::map< CTPID, LVL1CTP::Lvl1Item* >& itemsCache,
		    std::unordered_map< std::string,
		    const HLT::Chain* >& l2chainsByName,
		    std::map< CHAIN_COUNTER, HLT::Chain* >& l1chainsCache,
		    std::unordered_map< std::string,
		    const HLT::Chain* >& efchainsByName,
		    std::map< CHAIN_COUNTER, HLT::Chain* >& efchainsCache,
		    char& bgCode,
		    bool unpackHLT );
    /// Function unpacking the payload of the trigger navigation

    virtual StatusCode unpackNavigation( HLT::TrigNavStructure* nav );

    /// @name Functions checking/changing the state of the object
    /// @{
    
    virtual bool assert_handle();
    virtual void validate_handle();
    virtual void invalidate_handle();

    /// @}

  private:
    /// Function unpacking the decision of the LVL1 items
    StatusCode unpackItems( std::map< CTPID, LVL1CTP::Lvl1Item* >& itemsCache,
			    std::unordered_map< std::string,
			    const LVL1CTP::Lvl1Item*>& itemsByName );
    /// Function unpacking the decision of the HLT chains
    StatusCode unpackChains( std::map< unsigned, HLT::Chain* >& cache,
			     const std::vector< uint32_t >& raw,
			     const std::vector< uint32_t >& passedThrough,
			     const std::vector< uint32_t >& prescaled,
			     const std::vector< uint32_t >& resurrected,
			     std::unordered_map< std::string,
			     const HLT::Chain* >& output );

    /// Helper object for retrieving the event information
    DecisionObjectHandleStandalone* m_handle;
  }; // class DecisionUnpackerStandalone

} // namespace Trig

#endif // TRIGDECISIONTOOL_DECISIONUNPACKERSTANDALONE_H
