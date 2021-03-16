/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger include(s):
#include "xAODTrigger/TrigDecision.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

// Local include(s):
#include "TrigDecisionFillerTool.h"

namespace D3PD {

   TrigDecisionFillerTool::TrigDecisionFillerTool( const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent )
      : BlockFillerTool< void >( type, name, parent ),
        m_trigDec( "Trig::TrigDecisionTool/TrigDecisionTool" ),
        m_trigConf( "TrigConf::TrigConfigSvc/TrigConfigSvc", name )
   {
      // Avoid coverity warnings.
      m_saveLVL1Raw = true;
      m_saveLVL1Physics = true;
      m_saveHLTRaw = true;
      m_saveHLTPhysics = true;
      m_saveHLTResurrected = true;
      m_saveHLTPassThrough = true;
      m_saveHLTChainState = true;
      m_saveBGCode = true;
      m_saveTruncated = true;
      book().ignore();

      declareProperty( "SaveLVL1Raw",        m_saveLVL1Raw = false );
      declareProperty( "SaveLVL1Physics",    m_saveLVL1Physics = false );
      declareProperty( "SaveHLTRaw",         m_saveHLTRaw = false );
      declareProperty( "SaveHLTPhysics",     m_saveHLTPhysics = false );
      declareProperty( "SaveHLTResurrected", m_saveHLTResurrected = false );
      declareProperty( "SaveHLTPassThrough", m_saveHLTPassThrough = false );
      declareProperty( "SaveHLTChainState",  m_saveHLTChainState = false );
      declareProperty( "SaveBGCode",         m_saveBGCode = false );
      declareProperty( "SaveTruncated",      m_saveTruncated = false );
   }

   StatusCode TrigDecisionFillerTool::initialize() {

      // Retrieve the TrigDecisionTool:
      CHECK( m_trigDec.retrieve() );

      // Retrieve the trigger configuration service:
      CHECK( m_trigConf.retrieve() );

      return StatusCode::SUCCESS;
   }

   StatusCode TrigDecisionFillerTool::book() {

      if( m_saveLVL1Raw ) {
         CHECK( addVariable( "L1_TBP", m_tbp,
                             "LVL1 Trigger Before Prescale bits" ) );
         CHECK( addVariable( "L1_TAP", m_tap,
                             "LVL1 Trigger After Prescale bits" ) );
      }

      if( m_saveLVL1Physics ) {
         CHECK( addVariable( "L1_TAV", m_tav,
                             "LVL1 Trigger After Veto bits" ) );
      }

      if( m_saveHLTRaw ) {
         CHECK( addVariable( "L2_passedRaw", m_L2_passedRAW,
                             "LVL2 decision with condition \"TrigDefs::requireDecision\"" ) );
         CHECK( addVariable( "EF_passedRaw", m_EF_passedRAW,
                             "EF decision with condition \"TrigDefs::requireDecision\"" ) );
         CHECK( addVariable( "HLT_passedRaw", m_HLT_passedRAW,
                             "HLT decision with condition \"TrigDefs::requireDecision\"" ) );
      }

      if( m_saveHLTPhysics ) {
         CHECK( addVariable( "L2_passedPhysics", m_L2_passedPhysics,
                             "LVL2 decision with condition \"TrigDefs::Physics\"" ) );
         CHECK( addVariable( "EF_passedPhysics", m_EF_passedPhysics,
                             "EF decision with condition \"TrigDefs::Physics\"" ) );
         CHECK( addVariable( "HLT_passedPhysics", m_HLT_passedPhysics,
                             "HLT decision with condition \"TrigDefs::Physics\"" ) );

      }

      if( m_saveHLTResurrected ) {
         CHECK( addVariable( "L2_resurrected", m_L2_resurrected,
                             "LVL2 decision with condition "
                             "\"TrigDefs::allowResurrectedDecision|"
                             "TrigDefs::requireDecision|"
                             "TrigDefs::enforceLogicalFlow\"" ) );
         CHECK( addVariable( "EF_resurrected", m_EF_resurrected,\
                             "EF decision with condition "
                             "\"TrigDefs::allowResurrectedDecision|"
                             "TrigDefs::requireDecision|"
                             "TrigDefs::enforceLogicalFlow\"" ) );
         CHECK( addVariable( "HLT_resurrected", m_HLT_resurrected,\
                             "HLT decision with condition "
                             "\"TrigDefs::allowResurrectedDecision|"
                             "TrigDefs::requireDecision|"
                             "TrigDefs::enforceLogicalFlow\"" ) );
      }

      if( m_saveHLTPassThrough ) {
         CHECK( addVariable( "L2_passedThrough", m_L2_passedThrough,
                             "LVL2 decision with condition \"TrigDefs::passedThrough\"" ) );
         CHECK( addVariable( "EF_passedThrough", m_EF_passedThrough,
                             "EF decision with condition \"TrigDefs::passedThrough\"" ) );
         CHECK( addVariable( "HLT_passedThrough", m_HLT_passedThrough,
                             "HLT decision with condition \"TrigDefs::passedThrough\"" ) );
      }

      if( m_saveHLTChainState ) {
         CHECK( addVariable( "L2_wasPrescaled", m_L2_wasPrescaled,
                             "LVL2 chains that got prescaled in the current event" ) );
         CHECK( addVariable( "L2_wasResurrected", m_L2_wasResurrected,
                             "LVL2 chains that got resurrected in the current event" ) );
         CHECK( addVariable( "EF_wasPrescaled", m_EF_wasPrescaled,
                             "EF chains that got prescaled in the current event" ) );
         CHECK( addVariable( "EF_wasResurrected", m_EF_wasResurrected,
                             "EF chains that got resurrected in the current event" ) );
         CHECK( addVariable( "HLT_wasPrescaled", m_HLT_wasPrescaled,
                             "HLT chains that got prescaled in the current event" ) );
         CHECK( addVariable( "HLT_wasResurrected", m_HLT_wasResurrected,
                             "HLT chains that got resurrected in the current event" ) );
      }

      if( m_saveBGCode ) {
         CHECK( addVariable( "bgCode", m_bgCode,
                             "8-bit LVL1 bunch group code" ) );
      }

      if( m_saveTruncated ) {
         CHECK( addVariable( "L2_truncated", m_l2Truncated,
                             "True if the LVL2 HLT result got truncated by the DAQ" ) );
         CHECK( addVariable( "EF_truncated", m_efTruncated,
                             "True if the EF HLT result got truncated by the DAQ" ) );
         CHECK( addVariable( "HLT_truncated", m_HLTTruncated,
                             "True if the merged HLT result got truncated by the DAQ" ) );
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigDecisionFillerTool::fill() {

      //
      // Handle the LVL1 data:
      //
      if( m_saveLVL1Raw || m_saveLVL1Physics || m_saveBGCode ) {

         //
         // The LVL1 data can be retrieved most easily from the TrigDecision object itself.
         //
         const xAOD::TrigDecision* decision = nullptr;
         CHECK( evtStore()->retrieve( decision ) );

         if( m_saveLVL1Raw ) {
            *m_tbp = decision->tbp();
            *m_tap = decision->tap();
            ATH_MSG_DEBUG( "Saved the RAW LVL1 information" );
         }
         if( m_saveLVL1Physics ) {
           *m_tav = decision->tav();
            ATH_MSG_DEBUG( "Saved the Physics LVL1 information" );
         }

         if( m_saveBGCode ) {
            *m_bgCode = decision->bgCode();
            ATH_MSG_DEBUG( "Saved the BGCode information" );
         }

         if( m_saveTruncated ) {
            *m_l2Truncated = decision->lvl2Truncated();
            *m_efTruncated = decision->efTruncated();
            *m_HLTTruncated = decision->efTruncated();//using EFResult
            ATH_MSG_DEBUG( "Saved HLTResult truncation information" );
         }
      }

      // If the HLT information is not needed, return right away:
      if( ! ( m_saveHLTRaw || m_saveHLTPhysics || m_saveHLTResurrected ||
              m_saveHLTPassThrough ) ) {
         return StatusCode::SUCCESS;
      }

      //
      // Clear the variables:
      //
      if( m_saveHLTRaw ) {
         m_L2_passedRAW->clear();
         m_EF_passedRAW->clear();
         m_HLT_passedRAW->clear();
      }
      if( m_saveHLTPhysics ) {
         m_L2_passedPhysics->clear();
         m_EF_passedPhysics->clear();
         m_HLT_passedPhysics->clear();
      }
      if( m_saveHLTResurrected ) {
         m_L2_resurrected->clear();
         m_EF_resurrected->clear();
         m_HLT_resurrected->clear();
      }
      if( m_saveHLTPassThrough ) {
         m_L2_passedThrough->clear();
         m_EF_passedThrough->clear();
         m_HLT_passedThrough->clear();
      }
      if( m_saveHLTChainState ) {
         m_L2_wasPrescaled->clear();
         m_L2_wasResurrected->clear();
         m_EF_wasPrescaled->clear();
         m_EF_wasResurrected->clear();
         m_HLT_wasPrescaled->clear();
         m_HLT_wasResurrected->clear();
      }

      //
      // Loop over the configuration chains:
      //
      for( const TrigConf::HLTChain* chain : m_trigConf->chains() ) {

        // Is this a LVL2, or an EF chain?
        const bool isLVL2 = ( chain->level() == "L2" );

        // Save the LVL2 raw decision:
        if( isLVL2 && m_saveHLTRaw &&
            m_trigDec->isPassed( chain->chain_name(),
                                 TrigDefs::requireDecision ) ) {
          m_L2_passedRAW->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the LVL2 physics decision:
        if( isLVL2 && m_saveHLTPhysics &&
            m_trigDec->isPassed( chain->chain_name(),
                                 TrigDefs::Physics ) ) {
          m_L2_passedPhysics->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the LVL2 resurrected decision:
        if( isLVL2 && m_saveHLTResurrected &&
            m_trigDec->isPassed( chain->chain_name(),
                                 TrigDefs::allowResurrectedDecision |
                                 TrigDefs::requireDecision |
                                 TrigDefs::enforceLogicalFlow ) ) {
          m_L2_resurrected->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the LVL2 passThrough state:
        if( isLVL2 && m_saveHLTPassThrough &&
            m_trigDec->isPassed( chain->chain_name(),
                                 TrigDefs::passedThrough ) ) {
          m_L2_passedThrough->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }

        const bool isEF = ( chain->level() == "EF" );
        // Save the EF raw decision:
        if( ( isEF ) && m_saveHLTRaw &&
            m_trigDec->isPassed( chain->chain_name(), TrigDefs::requireDecision ) ) {
          m_EF_passedRAW->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the EF physics decision:
        if( ( isEF) && m_saveHLTPhysics &&
            m_trigDec->isPassed( chain->chain_name(), TrigDefs::Physics ) ) {
          m_EF_passedPhysics->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the EF resurrected decision:
        if( ( isEF ) && m_saveHLTResurrected &&
            m_trigDec->isPassed( chain->chain_name(),
                                 TrigDefs::allowResurrectedDecision |
                                 TrigDefs::requireDecision |
                                 TrigDefs::enforceLogicalFlow ) ) {
          m_EF_resurrected->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the EF passThrough state:
        if( ( isEF ) && m_saveHLTPassThrough &&
            m_trigDec->isPassed( chain->chain_name(),
                                 TrigDefs::passedThrough ) ) {
          m_EF_passedThrough->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }

        const bool isHLT = ( chain->level() == "HLT" );
        // Save the merged HLT raw decision:
        if( ( isHLT ) && m_saveHLTRaw &&
            m_trigDec->isPassed( chain->chain_name(), TrigDefs::requireDecision ) ) {
          m_HLT_passedRAW->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the HLT physics decision:
        if( ( isHLT) && m_saveHLTPhysics &&
            m_trigDec->isPassed( chain->chain_name(), TrigDefs::Physics ) ) {
          m_HLT_passedPhysics->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the HLT resurrected decision:
        if( ( isHLT ) && m_saveHLTResurrected &&
            m_trigDec->isPassed( chain->chain_name(),
                                 TrigDefs::allowResurrectedDecision |
                                 TrigDefs::requireDecision |
                                 TrigDefs::enforceLogicalFlow ) ) {
          m_HLT_resurrected->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }
        // Save the HLT passThrough state:
        if( ( isHLT ) && m_saveHLTPassThrough &&
            m_trigDec->isPassed( chain->chain_name(),
                                 TrigDefs::passedThrough ) ) {
          m_HLT_passedThrough->push_back( static_cast< chainId_t >( chain->chain_counter() ) );
        }

        // Save the chain state:
        if( m_saveHLTChainState ) {
          const unsigned int bits = m_trigDec->isPassedBits( chain->chain_name() );
          // Save the LVL2 prescale state:
          if( isLVL2 && ( bits & TrigDefs::L2_prescaled ) ) {
            m_L2_wasPrescaled->push_back( chain->chain_counter() );
          }
          // Save the LVL2 resurrected state:
          if( isLVL2 && ( bits & TrigDefs::L2_resurrected ) ) {
            m_L2_wasResurrected->push_back( chain->chain_counter() );
          }
          // Save the EF prescale state:
          if( ( isEF ) && ( bits & TrigDefs::EF_prescaled ) ) {
            m_EF_wasPrescaled->push_back( chain->chain_counter() );
          }
          // Save the EF resurrected state:
          if( ( isEF ) && ( bits & TrigDefs::EF_resurrected ) ) {
            m_EF_wasResurrected->push_back( chain->chain_counter() );
          }
          // Save the HLT prescale state:
          if( ( isHLT ) && ( bits & TrigDefs::EF_prescaled ) ) {//merged L2EF use same EF bits (see ChainGroup)
            m_HLT_wasPrescaled->push_back( chain->chain_counter() );
          }
          // Save the EF resurrected state:
          if( ( isHLT ) && ( bits & TrigDefs::EF_resurrected ) ) {//merged L2EF use same EF bits(see ChainGroup)
            m_HLT_wasResurrected->push_back( chain->chain_counter() );
          }

        }
      }

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
