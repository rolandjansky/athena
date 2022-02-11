// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDECISIONTOOL_DECISIONUNPACKERSTANDALONE_H
#define TRIGDECISIONTOOL_DECISIONUNPACKERSTANDALONE_H

// Trigger configuration include(s):
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigSteeringEvent/Lvl1Item.h"

// Local include(s):
#include "TrigDecisionTool/IDecisionUnpacker.h"
#include "TrigDecisionTool/Logger.h"

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
    unpackDecision( const EventContext& ctx,
                    std::unordered_map< std::string, const LVL1CTP::Lvl1Item* >& itemsByName,
                    std::map< CTPID, LVL1CTP::Lvl1Item >& itemsCache,
                    std::unordered_map< std::string, const HLT::Chain* >& l2chainsByName,
                    std::map< CHAIN_COUNTER, HLT::Chain >& l2chainsCache,
                    std::unordered_map< std::string, const HLT::Chain* >& efchainsByName,
                    std::map< CHAIN_COUNTER, HLT::Chain >& efchainsCache,
                    char& bgCode,
                    bool unpackHLT ) const override;
    /// Function unpacking the payload of the trigger navigation
    virtual StatusCode unpackNavigation( const EventContext& ctx,
                                         HLT::TrigNavStructure* nav ) const override;

  private:
    /// Function unpacking the decision of the LVL1 items
    StatusCode unpackItems( const xAOD::TrigDecision& trigDec,
                            std::map< CTPID, LVL1CTP::Lvl1Item >& itemsCache,
                            std::unordered_map< std::string,
                            const LVL1CTP::Lvl1Item*>& itemsByName ) const;

    /// Function unpacking the decision of the HLT chains
    StatusCode unpackChains( std::map< unsigned, HLT::Chain >& cache,
                             const std::vector< uint32_t >& raw,
                             const std::vector< uint32_t >& passedThrough,
                             const std::vector< uint32_t >& prescaled,
                             const std::vector< uint32_t >& resurrected,
                             std::unordered_map< std::string,
                             const HLT::Chain* >& output ) const;

    /// Key of the trigger decision object in the event
    SG::ReadHandleKey<xAOD::TrigDecision>* m_deckey{nullptr};
    /// Key of the trigger navigation object in the event
    SG::ReadHandleKey<xAOD::TrigNavigation>* m_navikey{nullptr};

  }; // class DecisionUnpackerStandalone

} // namespace Trig

#endif // TRIGDECISIONTOOL_DECISIONUNPACKERSTANDALONE_H
