/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Trigger include(s):
#include "TrigNavStructure/TrigNavStructure.h"

#include "TrigSteeringEvent/Lvl1Item.h"
#include "TrigSteeringEvent/Chain.h"
#include "TrigConfHLTData/HLTChainList.h"

// Local include(s):
#include "TrigDecisionTool/DecisionUnpackerStandalone.h"
#include "TrigDecisionTool/DecisionObjectHandleStandalone.h"

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
#include "TrigNavigation/NavigationCore.h"
#endif

namespace {

   /// Helper function for extracting the decision of an item/chain from the
   /// packed payload
   bool get32BitDecision( unsigned int index,
                          const std::vector< uint32_t >& vec ) {

      if( index >= vec.size() * 32 ) {
         return 0;
      }
      uint32_t word = vec[ index / 32 ];
      return ( ( ( word >> ( index % 32 ) ) & 0x1 ) != 0 );
   }
  
} // private namespace

namespace Trig {

  DecisionUnpackerStandalone::DecisionUnpackerStandalone( SG::ReadHandleKey<xAOD::TrigDecision>* deckey,
                                                          SG::ReadHandleKey<xAOD::TrigNavigation>* navikey)
    : m_handle( std::make_unique<DecisionObjectHandleStandalone>( deckey, navikey ) )
  {
  }
  
  DecisionUnpackerStandalone::~DecisionUnpackerStandalone() {
  }
  
  StatusCode
  DecisionUnpackerStandalone::
  unpackDecision( std::unordered_map< std::string,const LVL1CTP::Lvl1Item* >& itemsByName,
                  std::map< CTPID, LVL1CTP::Lvl1Item >& itemsCache,
                  std::unordered_map< std::string, const HLT::Chain* >& l2chainsByName,
                  std::map< CHAIN_COUNTER, HLT::Chain >& l2chainsCache,
                  std::unordered_map< std::string, const HLT::Chain* >& efchainsByName,
                  std::map< CHAIN_COUNTER, HLT::Chain >& efchainsCache,
                  char& bgCode,
                  bool unpackHLT ) {

      // Grab the trigger decision:
      const xAOD::TrigDecision* xaoddec = m_handle->getDecision();
      if( ! xaoddec ){
         ATH_MSG_ERROR( "xAOD decision is null" );
         return StatusCode::FAILURE;
      }

      // Read the bunch group code:
      bgCode = xaoddec->bgCode();

      // L1 items
      itemsByName.clear();
      ATH_MSG_DEBUG( "Unpacking of L1 items" );

      if( unpackItems( itemsCache,itemsByName ).isFailure() ) {
         ATH_MSG_WARNING( "Unpacking  of L1 items failed" );
      }

      // Protect from unpacking in case HLT was not run
      // (i.e. configuration chains are 0)
      if( ! unpackHLT ) {
         return StatusCode::SUCCESS;
      }

      // L2 chains
      l2chainsByName.clear();
      ATH_MSG_DEBUG( "Unpacking of L2 chains" );

      if( unpackChains( l2chainsCache, xaoddec->lvl2PassedRaw(),
                        xaoddec->lvl2PassedThrough(), xaoddec->lvl2Prescaled(),
                        xaoddec->lvl2Resurrected(),
                        l2chainsByName ).isFailure() ) {
         ATH_MSG_WARNING( "Unpacking  of L2 chains failed" );
      }

      // EF chains
      efchainsByName.clear();
      ATH_MSG_DEBUG( "Unpacking of EF/HLT chains" );
  
      if ( unpackChains( efchainsCache, xaoddec->efPassedRaw(),
                         xaoddec->efPassedThrough(), xaoddec->efPrescaled(),
                         xaoddec->efResurrected(),
                         efchainsByName ).isFailure() ) {
         ATH_MSG_WARNING( "Unpacking  of EF/HLT chains failed" );
      }

      this->unpacked_decision(true);

      return StatusCode::SUCCESS;
   }

   StatusCode
   DecisionUnpackerStandalone::unpackNavigation( HLT::TrigNavStructure* nav ) {
      // A security check:
      if( ! nav ) {
         ATH_MSG_VERBOSE( "Null HLT::TrigNavStructure pointer received" );
         return StatusCode::SUCCESS;
      }

      ATH_MSG_DEBUG( "Unpacking Navigation" );

      const xAOD::TrigNavigation* serializedNav = m_handle->getNavigation();
      if( ! serializedNav ) {
         [[maybe_unused]] static std::atomic<bool> warningPrinted =
            [&]() { ATH_MSG_WARNING( "Serialized navigation not available" );
                    return true; }();
         return StatusCode::FAILURE;
      }

      // try to get navigation from EF/HLT
      ATH_MSG_DEBUG( "Trying to unpack Navigation of size: "
                     << serializedNav->serialized().size() );

      
      bool navi_nonempty = !(serializedNav->serialized().empty());

#if !defined(XAOD_STANDALONE) && !defined(XAOD_ANALYSIS)
      HLT::NavigationCore* fullNav = dynamic_cast<HLT::NavigationCore*>(nav);
      
      if(!fullNav){
        ATH_MSG_ERROR("downcast failed");
        return StatusCode::FAILURE;
      }
      
      fullNav->reset();
      bool unpacking_status = navi_nonempty && fullNav->deserialize( serializedNav->serialized() ) ;
#else
      nav->reset();
      bool unpacking_status = navi_nonempty && nav->deserialize( serializedNav->serialized() ) ;
#endif

      if ( ! unpacking_status ) {
         ATH_MSG_WARNING( "Navigation unpacking failed" );
      } else {
         ATH_MSG_DEBUG( "Unpacked Navigation" );
      }

      this->unpacked_navigation(true);

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   bool DecisionUnpackerStandalone::assert_handle() {

      if( ! m_handle ) {
         ATH_MSG_ERROR("Logic ERROR, no handle for TrigDecisionTool ");
         return false;
      }
      if( m_handle->getDecision() == 0 ) {
         ATH_MSG_ERROR( "No TrigDecision object is accessible" );
         return false;
      }
      if( m_handle->valid() ) {
         // On this case we already unpacked.. so we don't want to mess further
         return false;
      }
      return true;
   }

   void DecisionUnpackerStandalone::validate_handle(){
      m_handle->validate();
      return;
   }

   void DecisionUnpackerStandalone::invalidate_handle(){
      ATH_MSG_VERBOSE("invalidating handle");
      m_handle->reset();
      this->unpacked_decision(false);
      this->unpacked_navigation(false);
      return;
   }

   StatusCode
   DecisionUnpackerStandalone::
   unpackItems( std::map< unsigned, LVL1CTP::Lvl1Item >& itemsCache,
                std::unordered_map< std::string, const LVL1CTP::Lvl1Item* >& itemsByName ) {
      itemsByName.reserve( itemsByName.size() + itemsCache.size() );
      for( auto& [ctpid, item] : itemsCache ) {
         ATH_MSG_VERBOSE( "Unpacking bits for item: " << ctpid << " "
                          << item.name() );
         bool passBP = get32BitDecision( ctpid,
                                         m_handle->getDecision()->tbp() );
         bool passAP = get32BitDecision( ctpid,
                                         m_handle->getDecision()->tap() );
         bool passAV = get32BitDecision( ctpid,
                                         m_handle->getDecision()->tav() );
         ATH_MSG_VERBOSE( "     --- bits are: bp: " << passBP
                          << " ap: " << passAP << " av: "
                          << passAV );

         LVL1CTP::Lvl1Item itemNew (item.name(), item.hashId(),
                                    passBP, passAP, passAV,
                                    item.prescaleFactor());
         item = std::move (itemNew);
         itemsByName[ item.name() ] = &item;
      }

      return StatusCode::SUCCESS;
   }
   
   
   StatusCode
   DecisionUnpackerStandalone::
   unpackChains( std::map< unsigned, HLT::Chain >& cache,
                 const std::vector< uint32_t >& raw,
                 const std::vector< uint32_t >& passedthrough,
                 const std::vector< uint32_t >& prescaled,
                 const std::vector< uint32_t >& resurrected,
                 std::unordered_map< std::string, const HLT::Chain* >& output ) {
      output.reserve( output.size() + cache.size() );

      for( auto& [cntr, chain] : cache ) {

         chain.reset();
         ATH_MSG_VERBOSE( "raw dec for " << cntr << " is "
                          << get32BitDecision( cntr, raw ) );
         chain.setDecisions( get32BitDecision( cntr, raw ),
                             get32BitDecision( cntr,
                                               passedthrough ),
                             get32BitDecision( cntr, prescaled),
                             get32BitDecision( cntr, resurrected ) );
         output[ chain.getChainName() ] = &chain;
         ATH_MSG_VERBOSE( "Updated chain in this event : " << chain );
      }

      return StatusCode::SUCCESS;
   }

} // namespace Trig
