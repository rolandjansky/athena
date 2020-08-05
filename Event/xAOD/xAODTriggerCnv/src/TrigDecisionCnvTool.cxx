/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionCnvTool.cxx 688265 2015-08-08 16:31:45Z stelzer $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigConfHLTData/HLTChainList.h"

// Local include(s):
#include "TrigDecisionCnvTool.h"

namespace xAODMaker {

   //
   // This is a copy of the contents of TrigDecisionTool/Conditions.h until
   // that header is moved to TrigDecisionInterface.
   ///
   namespace TrigDefs {
      // Conditions Mask elements
      // -- we should do this with enum but there are issues with python
      //    so we'll code it as int for now.
 
      const static unsigned int requireDecision          = 0x1;
      // ... otherwise ignore the decision
      //const static unsigned int passedThrough            = 0x1 << 1;
      // ... otherwise ignore the passedThrough requirement
      //const static unsigned int allowResurrectedDecision = 0x1 << 2;
      // ... otherwise require that it wasn't resurrected
      //
      const static unsigned int enforceLogicalFlow       = 0x1 << 5;
      // ... otherwise we only care about this trigger level
      //const static unsigned int eventAccepted            = 0x1 << 6;
      // ... an event was written out (useful for trigger rates)
      //
      //const static unsigned int ignoreIOV = 0x1 << 15;
      //const static unsigned int alsoDeactivateTEs = 0x1 << 16;
      //const static unsigned int ignoreFDR = 0x1 << 17;
      //
      const static unsigned int Physics = requireDecision | enforceLogicalFlow;
      //const static unsigned int fullChain = enforceLogicalFlow;
      // Bit location placeholders for isPassedBit()
      const static unsigned int EF_passedRaw              =0x1;
      const static unsigned int EF_passThrough            =0x1 << 1;
      const static unsigned int EF_prescaled              =0x1 << 2;
      const static unsigned int EF_resurrected            =0x1 << 3;
      const static unsigned int L2_passedRaw              =0x1 << 8;
      const static unsigned int L2_passThrough            =0x1 << 9;
      const static unsigned int L2_prescaled              =0x1 << 10;
      const static unsigned int L2_resurrected            =0x1 << 11;
      //const static unsigned int L1_isPassedAfterPrescale  = 0x1 << 16;
      //const static unsigned int L1_isPassedBeforePrescale = 0x1 << 17;
      //const static unsigned int L1_isPassedAfterVeto      = 0x1 << 18;
      //const static unsigned int EF_error                  = 0x1 << 19;
      //const static unsigned int L2_error                  = 0x1 << 20;
   }

   /// The range in which we copy the trigger decision for the chains
   /**
    * Note that the "physics chains" by convention get a chain counter
    * smaller than this value.
    */
   static const uint32_t MAXIMUM_CHAIN_ID = 8192;
   /// Size of the bitset vectors
   static const uint32_t BITSET_SIZE = MAXIMUM_CHAIN_ID / 32;

   TrigDecisionCnvTool::TrigDecisionCnvTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_tdt( "Trig::TrigDecisionTool/TrigDecisionTool" ),
        m_trigconf( "TrigConf::TrigConfigSvc/TrigConfigSvc", name ) {

      // Declare the interface(s) provided by the tool:
      declareInterface< ITrigDecisionCnvTool >( this );

      declareProperty ("TrigDecisionTool", m_tdt);
   }

   
   TrigDecisionCnvTool::~TrigDecisionCnvTool() {
   }


   StatusCode TrigDecisionCnvTool::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

      // Retrieve the needed component(s):
      CHECK( m_tdt.retrieve() );
      CHECK( m_trigconf.retrieve() );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   /**
    * This is the important function of the tool. It takes an existing
    * TrigDec::TrigDecision object, and copies its contents into a new
    * xAOD::TrigDecision object.
    *
    * @param aod The AOD object to take the information from
    * @param xaod The xAOD object to fill the information into
    * @returns <code>StatusCode::SUCCESS</code> if all went fine,
    *          something else if not
    */
   StatusCode TrigDecisionCnvTool::convert( const TrigDec::TrigDecision* aod,
                                            xAOD::TrigDecision* xaod, const TriggerInfo * triggerInfo) const {

      ATH_MSG_DEBUG( "converting TrigDecision to xAOD::TrigDecision" );

      // Copy the trigger configuration key:
      xaod->setSMK( aod->masterKey() );

      // Copy the LVL1 information:
      xaod->setBGCode( aod->BGCode() );
      ATH_MSG_DEBUG( "converting BGCode " << std::hex << (unsigned int) aod->BGCode() << "  (size " << sizeof(aod->BGCode()) << "|" << sizeof(xaod->bgCode()) << ")" );

      if( aod->getL1Result().isConfigured() ) {
         ATH_MSG_DEBUG( "setting L1 info from old TrigDecision");
         xaod->setTAV( aod->getL1Result().itemsAfterVeto() );
         xaod->setTAP( aod->getL1Result().itemsAfterPrescale() );
         xaod->setTBP( aod->getL1Result().itemsBeforePrescale() );
      } else if( triggerInfo != nullptr ){
         ATH_MSG_DEBUG( "setting L1 info from TriggerInfo (l1 size " << triggerInfo->level1TriggerInfo().size() << ")" );
         
         uint32_t L1SIZE = triggerInfo->level1TriggerInfo().size()/3;
         std::vector<uint32_t>::const_iterator begin, end;
    
         begin = triggerInfo->level1TriggerInfo().begin();
         end = begin;
         std::advance(end,L1SIZE);
         std::vector<uint32_t> tbp(begin,end);
    
         begin = end;
         std::advance(end,L1SIZE);
         std::vector<uint32_t> tap(begin,end);
    
         begin = end;
         std::advance(end,L1SIZE);
         std::vector<uint32_t> tav(begin,end);

         xaod->setTAV( tav );
         xaod->setTAP( tap );
         xaod->setTBP( tbp );

         ATH_MSG_DEBUG( "TBP size = " << xaod->tbp().size() );
         ATH_MSG_DEBUG( "TAP size = " << xaod->tap().size() );
         ATH_MSG_DEBUG( "TAV size = " << xaod->tav().size() );
         
      } else {

         ATH_MSG_DEBUG( "can not set L1 info as the old TrigDecision has no L1Result and there is no TriggerInfo available");

      }

      // Copy the basic HLT information:
      xaod->setLVL2ErrorBits( aod->getL2Result().error_bits() );
      xaod->setEFErrorBits  ( aod->getEFResult().error_bits() );
      xaod->setLVL2Truncated( aod->getL2Result().isHLTResultTruncated() );
      xaod->setEFTruncated  ( aod->getEFResult().isHLTResultTruncated() );

      //
      // Construct the HLT decision bits using the TDT.
      //

      // The LVL2 bitsets:
      std::vector< uint32_t > l2PassedPhysics( BITSET_SIZE, 0 );
      std::vector< uint32_t > l2PassedRaw    ( BITSET_SIZE, 0 );
      std::vector< uint32_t > l2PassedThrough( BITSET_SIZE, 0 );
      std::vector< uint32_t > l2Prescaled    ( BITSET_SIZE, 0 );
      std::vector< uint32_t > l2Resurrected  ( BITSET_SIZE, 0 );

      // The EF bitsets:
      std::vector< uint32_t > efPassedPhysics( BITSET_SIZE, 0 );
      std::vector< uint32_t > efPassedRaw    ( BITSET_SIZE, 0 );
      std::vector< uint32_t > efPassedThrough( BITSET_SIZE, 0 );
      std::vector< uint32_t > efPrescaled    ( BITSET_SIZE, 0 );
      std::vector< uint32_t > efResurrected  ( BITSET_SIZE, 0 );

      // Get a list of all the HLT chains:
      const TrigConf::HLTChainList& chains = m_trigconf->chains();

      // Loop over all the chains:
      TrigConf::HLTChainList::const_iterator chain_itr = chains.begin();
      TrigConf::HLTChainList::const_iterator chain_end = chains.end();
      for( ; chain_itr != chain_end; ++chain_itr ) {

         // Decide whether to care about the chain or not:
         if( static_cast< uint32_t >( ( *chain_itr )->chain_counter() )
             >= MAXIMUM_CHAIN_ID ) {
            continue;
         }

         // Get the decision for the chain:
         const uint32_t bits =
            m_tdt->isPassedBits( ( *chain_itr )->chain_name() );

         // Decide which level the chain is from:
         if( ( *chain_itr )->level_enum() == TrigConf::L2 ) {
            // Set the different decisions:
            CHECK( setBit( l2PassedPhysics, ( *chain_itr )->chain_counter(),
                           m_tdt->isPassed( ( *chain_itr )->chain_name(),
                                            TrigDefs::Physics ) ) );
            CHECK( setBit( l2PassedRaw, ( *chain_itr )->chain_counter(),
                           ( bits & TrigDefs::L2_passedRaw ) ) );
            CHECK( setBit( l2PassedThrough, ( *chain_itr )->chain_counter(),
                           ( bits & TrigDefs::L2_passThrough ) ) );
            CHECK( setBit( l2Prescaled, ( *chain_itr )->chain_counter(),
                           ( bits & TrigDefs::L2_prescaled ) ) );
            CHECK( setBit( l2Resurrected, ( *chain_itr )->chain_counter(),
                           ( bits & TrigDefs::L2_resurrected ) ) );
         }
         // EF and HLT results are treated the same:
         else {
            // Set the different decisions:
            CHECK( setBit( efPassedPhysics, ( *chain_itr )->chain_counter(),
                           m_tdt->isPassed( ( *chain_itr )->chain_name(),
                                            TrigDefs::Physics ) ) );
            CHECK( setBit( efPassedRaw, ( *chain_itr )->chain_counter(),
                           ( bits & TrigDefs::EF_passedRaw ) ) );
            CHECK( setBit( efPassedThrough, ( *chain_itr )->chain_counter(),
                           ( bits & TrigDefs::EF_passThrough ) ) );
            CHECK( setBit( efPrescaled, ( *chain_itr )->chain_counter(),
                           ( bits & TrigDefs::EF_prescaled ) ) );
            CHECK( setBit( efResurrected, ( *chain_itr )->chain_counter(),
                           ( bits & TrigDefs::EF_resurrected ) ) );
         }
      }

      // Put the bit-patterns into the output object:
      xaod->setLVL2PassedPhysics( l2PassedPhysics );
      xaod->setLVL2PassedRaw    ( l2PassedRaw );
      xaod->setLVL2PassedThrough( l2PassedThrough );
      xaod->setLVL2Prescaled    ( l2Prescaled );
      xaod->setLVL2Resurrected  ( l2Resurrected );
      xaod->setEFPassedPhysics( efPassedPhysics );
      xaod->setEFPassedRaw    ( efPassedRaw );
      xaod->setEFPassedThrough( efPassedThrough );
      xaod->setEFPrescaled    ( efPrescaled );
      xaod->setEFResurrected  ( efResurrected );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode TrigDecisionCnvTool::setBit( std::vector< uint32_t >& bitset,
                                           uint32_t chainId, bool value ) {

      // Check if the operation will be successful:
      if( ( chainId >= MAXIMUM_CHAIN_ID ) ||
          ( bitset.size() < BITSET_SIZE ) ) {
         return StatusCode::FAILURE;
      }

      // Decide which bit to modify exactly:
      const size_t word = chainId / 32;
      const size_t bit  = chainId % 32;

      if( value ) {
         // Turn the bit on:
         bitset[ word ] = bitset[ word ] | ( 0x1 << bit );
      } else {
         // Turn the bit off:
         bitset[ word ] = bitset[ word ] & ( ~ ( 0x1 << bit ) );
      }

      // Everything went fine:
      return StatusCode::SUCCESS;
   }

} // namespace xAODMaker
