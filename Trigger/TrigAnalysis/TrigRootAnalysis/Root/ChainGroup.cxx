/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ChainGroup.cxx 726661 2016-02-28 01:18:32Z ssnyder $

// STL include(s):
#include <algorithm>
#include <cmath>

// ROOT include(s):
#include <TRegexp.h>
#include <TString.h>

// Local include(s):
#include "TrigRootAnalysis/ChainGroup.h"
#include "TrigRootAnalysis/IDataAccess.h"
#include "TrigRootAnalysis/IConfigAccess.h"
#include "TrigRootAnalysis/IITrigConfigSvcD3PD.h"

ClassImp( D3PD::ChainGroup )

namespace D3PD {

   /**
    * Protected constructor, to be only used by the code itself.
    *
    * @param triggerNames Patterns given by the user
    * @param parent Interface to enable access to the trigger data
    * @param svc The trigger configuration service
    */
   ChainGroup::ChainGroup( const std::vector< std::string >& triggerNames,
                           Trig::IDataAccess& parent,
                           Trig::IConfigAccess& svc )
      : TNamed( "ChainGroup", "A particular chain group" ),
        m_parent( parent ), m_configSvc( svc ), m_triggerNames( triggerNames ),
        m_smk( -9999 ), m_existingTriggers(), m_existingIDs() {

   }

   /**
    * This function implements an "OR" of the triggers that match the selection.
    * The function returns <code>kTRUE</code> if any of the matching triggers pass
    * the selection.
    *
    * If you want to mix LVL1 and HLT triggers, you can only use TrigDefs::Physics as
    * the decision type. The function will issue warnings if you request special LVL1
    * or HLT decision types for a trigger that doesn't belong to the correct group.
    *
    * @param type The trigger decision type you're interested in
    * @returns <code>kTRUE</code> if any of the selected triggers passed the selection,
    *          <code>kFALSE</code> otherwise
    */
   Bool_t ChainGroup::IsPassed( TrigDefs::DecisionTypes type ) {

      //
      // Make sure the object is up to date:
      //
      if( ! Update() ) {
         Error( "IsPassed", "Trigger(s) can't be evaluated" );
         return kFALSE;
      }

      //
      // Loop over the matching triggers:
      //
      for( size_t i = 0; i < m_existingTriggers.size(); ++i ) {

         // If this is a LVL1 item:
         if( m_existingTriggers[ i ].find( "L1_" ) == 0 ) {

            // Variables helping in looking up the correct bit in the result:
            const Int_t word = m_existingIDs[ i ] / 32;
            const Int_t bit  = m_existingIDs[ i ] % 32;

            switch( type ) {

            case TrigDefs::Physics:
               if( m_parent.GetDetailLevel() >= 0 ) {
                  // Return right away if the trigger fired
                  if( ( ( * m_parent.GetL1Result( Trig::IDataAccess::TAV ) )[ word ]
                        >> bit ) & 0x1 ) {
                     return kTRUE;
                  }
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::L1_TBP:
               if( m_parent.GetDetailLevel() >= 1 ) {
                  // Return right away if the trigger fired
                  if( ( ( * m_parent.GetL1Result( Trig::IDataAccess::TBP ) )[ word ]
                        >> bit ) & 0x1 ) {
                     return kTRUE;
                  }
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::L1_TAP:
               if( m_parent.GetDetailLevel() >= 1 ) {
                  // Return right away if the trigger fired
                  if( ( ( * m_parent.GetL1Result( Trig::IDataAccess::TAP ) )[ word ]
                        >> bit ) & 0x1 ) {
                     return kTRUE;
                  }
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            default:
               Warning( "IsPassed", "Decision type not recognised for LVL1 items!" );
               break;
            }

         }
         // If this is a LVL2 chain:
         else if( m_existingTriggers[ i ].find( "L2_" ) == 0 ) {

            std::vector< short >::const_iterator begin, end;
            switch( type ) {

            case TrigDefs::Physics:
               if( m_parent.GetDetailLevel() >= 0 ) {
                  begin =
                     m_parent.GetL2Result( Trig::IDataAccess::Physics )->begin();
                  end   =
                     m_parent.GetL2Result( Trig::IDataAccess::Physics )->end();
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::requireDecision:
               if( m_parent.GetDetailLevel() >= 1 ) {
                  begin =
                     m_parent.GetL2Result( Trig::IDataAccess::Raw )->begin();
                  end   =
                     m_parent.GetL2Result( Trig::IDataAccess::Raw )->end();
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::allowResurrected:
               if( m_parent.GetDetailLevel() >= 2 ) {
                  begin =
                     m_parent.GetL2Result( Trig::IDataAccess::Resurrected )->begin();
                  end   =
                     m_parent.GetL2Result( Trig::IDataAccess::Resurrected )->end();
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::passedThrough:
               if( m_parent.GetDetailLevel() >= 2 ) {
                  begin =
                     m_parent.GetL2Result( Trig::IDataAccess::PassedThrough )->begin();
                  end   =
                     m_parent.GetL2Result( Trig::IDataAccess::PassedThrough )->end();
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            default:
               Warning( "IsPassed", "Decision type not recognised for HLT chains!" );
               break;
            }

            if( std::find( begin, end, m_existingIDs[ i ] ) != end ) {
               return kTRUE; // Return right away if the trigger fired
            }

         }
         // If this is an EF chain:
         else if( m_existingTriggers[ i ].find( "EF_" ) == 0 ) {

            std::vector< short >::const_iterator begin, end;
            switch( type ) {

            case TrigDefs::Physics:
               if( m_parent.GetDetailLevel() >= 0 ) {
                  begin =
                     m_parent.GetEFResult( Trig::IDataAccess::Physics )->begin();
                  end   =
                     m_parent.GetEFResult( Trig::IDataAccess::Physics )->end();
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::requireDecision:
               if( m_parent.GetDetailLevel() >= 1 ) {
                  begin =
                     m_parent.GetEFResult( Trig::IDataAccess::Raw )->begin();
                  end   =
                     m_parent.GetEFResult( Trig::IDataAccess::Raw )->end();
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::allowResurrected:
               if( m_parent.GetDetailLevel() >= 2 ) {
                  begin =
                     m_parent.GetEFResult( Trig::IDataAccess::Resurrected )->begin();
                  end   =
                     m_parent.GetEFResult( Trig::IDataAccess::Resurrected )->end();
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::passedThrough:
               if( m_parent.GetDetailLevel() >= 2 ) {
                  begin =
                     m_parent.GetEFResult( Trig::IDataAccess::PassedThrough )->begin();
                  end   =
                     m_parent.GetEFResult( Trig::IDataAccess::PassedThrough )->end();
               } else {
                  Warning( "IsPassed",
                           "The requested information is not available in the input" );
                  return kFALSE;
               }
               break;
            default:
               Warning( "IsPassed", "Decision type not recognised for HLT chains!" );
               break;
            }

            if( std::find( begin, end, m_existingIDs[ i ] ) != end ) {
               return kTRUE; // Return right away if the trigger fired
            }

         } else {
            Fatal( "IsPassed", "Something seems to be wrong with this function!" );
            return kFALSE;
         }

      }

      return kFALSE; // If none of the matching triggers fired in the event
   }

   /**
    * This function is implemented in the same way as the offline version of
    * ChainGroup::getPrescale(...). If the chain group only holds one chain/item,
    * then the function returns the cumulative prescale of this chain/item.
    * (e.g. EF prescale * LVL2 prescale * LVL1 prescale)
    *
    * In case there are multiple triggers defined, there are two cases. If one
    * of the defined triggers is unprescaled (prescale = 1.0) then the chain group
    * by definition has a prescale of 1.0. But if all of the chains/items are
    * prescaled, we can't provide a single prescale for them. In this case the
    * function returns 0.0.
    *
    * @returns The cumulative prescale of the triggers
    */
   Float_t ChainGroup::GetPrescale() {

      //
      // Make sure the object is up to date:
      //
      if( ! Update() ) {
         Error( "IsPassed", "Trigger(s) can't be evaluated" );
         return 0.0;
      }

      // We handle the case when there's only one trigger in the chain group
      // in a distinct way:
      const Bool_t singleTrigger = ( m_existingTriggers.size() == 1 );

      //
      // Loop over all the matching triggers:
      //
      std::vector< std::string >::const_iterator name_itr =
         m_existingTriggers.begin();
      std::vector< std::string >::const_iterator name_end =
         m_existingTriggers.end();
      for( ; name_itr != name_end; ++name_itr ) {

         //
         // Calculate the overall prescale of the chain, or just request the
         // prescale of LVL1 items:
         //
         Float_t chainPrescale = 1.0;
         if( name_itr->find( "EF_" ) != name_itr->npos ) {

            chainPrescale = m_configSvc.GetConfigSvc().GetPrescale( *name_itr );
            const std::string& l2name =
               m_configSvc.GetConfigSvc().GetLowerChainName( *name_itr );
            chainPrescale *= m_configSvc.GetConfigSvc().GetPrescale( l2name );
            const std::string& l1name =
               m_configSvc.GetConfigSvc().GetLowerChainName( l2name );
            chainPrescale *= m_configSvc.GetConfigSvc().GetPrescale( l1name );

         } else if( name_itr->find( "L2_" ) != name_itr->npos ) {

            chainPrescale = m_configSvc.GetConfigSvc().GetPrescale( *name_itr );
            const std::string& l1name =
               m_configSvc.GetConfigSvc().GetLowerChainName( *name_itr );
            chainPrescale *= m_configSvc.GetConfigSvc().GetPrescale( l1name );

         } else if( name_itr->find( "L1_" ) != name_itr->npos ) {

            chainPrescale = m_configSvc.GetConfigSvc().GetPrescale( *name_itr );

         } else {
            Fatal( "GetPrescale",
                   "Something seems to be wrong with this function!" );
            return 0.0;
         }

         // If we just have this one trigger in the group, just return now:
         if( singleTrigger ) return chainPrescale;

         // If we have multiple triggers, but one of them is unprescaled, then the
         // whole group is labeled as unprescaled as well:
         const Bool_t unprescaled = ( std::abs( chainPrescale - 1.0 ) <
                                      1e-5 );
         if( unprescaled ) return 1.0;

      }

      // If we have multiple triggers, and all are prescaled:
      return 0.0;
   }

   /**
    * This function returns the list of triggers that match a given selection.
    * For instance to get the list of all LVL2 chains, one has to request a
    * chain group with the pattern "L2.*".
    *
    * @returns The list of triggers matching the selection
    */
   const std::vector< std::string >& ChainGroup::GetListOfTriggers() {

      //
      // Make sure the object is up to date:
      //
      if( ! Update() ) {
         Error( "IsPassed", "Trigger(s) can't be evaluated" );
         m_existingTriggers.clear();
      }

      return m_existingTriggers;
   }

   /**
    * This is a very slow function. It uses the <code>GetPassedL1Triggers(...)</code>,
    * <code>GetPassedL2Triggers(...)</code> and <code>GetPassedEFTriggers(...)</code>
    * functions, and merges their results.
    *
    * So if you know what kind of triggers you're looking for, you're better off
    * using one of the specific functions.
    *
    * @param type The trigger decision type you're interested in
    * @returns The names of all the triggers from this chain group that fired
    *          in the current event
    */
   std::vector< std::string >
   ChainGroup::GetPassedTriggers( TrigDefs::DecisionTypes type ) {

      std::vector< std::string > result = GetPassedL1Triggers( type );
      const std::vector< std::string > l2 = GetPassedL2Triggers( type );
      const std::vector< std::string > ef = GetPassedEFTriggers( type );
      result.insert( result.end(), l2.begin(), l2.end() );
      result.insert( result.end(), ef.begin(), ef.end() );

      return result;
   }

   /**
    * The function loops over all the LVL1 items of this chain group, and checks
    * which one of them passed in the current event. It then returns all the ones
    * that fired.
    *
    * The function is very slow, so it should really just be used in interactive
    * analysis, or in a very last step of an analysis. (When running on relatively
    * few events.)
    *
    * @param type The trigger decision type you're interested in
    * @returns The names of the LVL1 items from this chain group that fired
    *          in the current event
    */
   std::vector< std::string >
   ChainGroup::GetPassedL1Triggers( TrigDefs::DecisionTypes type ) {

      // The result object:
      std::vector< std::string > result;

      //
      // Make sure the object is up to date:
      //
      if( ! Update() ) {
         Error( "GetPassedL1Triggers", "Trigger(s) can't be evaluated" );
         return result;
      }

      //
      // Loop over the matching triggers:
      //
      for( size_t i = 0; i < m_existingTriggers.size(); ++i ) {

         // Only look at LVL1 items here:
         if( m_existingTriggers[ i ].find( "L1_" ) != 0 ) continue;

         // Variables helping in looking up the correct bit in the result:
         const Int_t word = m_existingIDs[ i ] / 32;
         const Int_t bit  = m_existingIDs[ i ] % 32;

         switch( type ) {

         case TrigDefs::Physics:
            if( m_parent.GetDetailLevel() >= 0 ) {
               // If this trigger fired, add it to the list:
               if( ( ( * m_parent.GetL1Result( Trig::IDataAccess::TAV ) )[ word ]
                     >> bit ) & 0x1 ) {
                  result.push_back( m_existingTriggers[ i ] );
               }
            } else {
               Warning( "GetPassedL1Triggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         case TrigDefs::L1_TBP:
            if( m_parent.GetDetailLevel() >= 1 ) {
               // Return right away if the trigger fired
               if( ( ( * m_parent.GetL1Result( Trig::IDataAccess::TBP ) )[ word ]
                     >> bit ) & 0x1 ) {
                  result.push_back( m_existingTriggers[ i ] );
               }
            } else {
               Warning( "GetPassedL1Triggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         case TrigDefs::L1_TAP:
            if( m_parent.GetDetailLevel() >= 1 ) {
               // Return right away if the trigger fired
               if( ( ( * m_parent.GetL1Result( Trig::IDataAccess::TAP ) )[ word ]
                     >> bit ) & 0x1 ) {
                  result.push_back( m_existingTriggers[ i ] );
               }
            } else {
               Warning( "GetPassedL1Triggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         default:
            Error( "GetPassedL1Triggers",
                   "Decision type not recognised for LVL1 items!" );
            return result;
            break;
         }
      }

      return result;
   }

   /**
    * The function loops over all the LVL2 chains of this chain group, and checks
    * which one of them passed in the current event. It then returns all the ones
    * that fired.
    *
    * The function is very slow, so it should really just be used in interactive
    * analysis, or in a very last step of an analysis. (When running on relatively
    * few events.)
    *
    * @param type The trigger decision type you're interested in
    * @returns The names of the LVL2 chains from this chain group that fired
    *          in the current event
    */
   std::vector< std::string >
   ChainGroup::GetPassedL2Triggers( TrigDefs::DecisionTypes type ) {

      // The result object:
      std::vector< std::string > result;

      //
      // Make sure the object is up to date:
      //
      if( ! Update() ) {
         Error( "GetPassedL2Triggers", "Trigger(s) can't be evaluated" );
         return result;
      }

      //
      // Loop over the matching triggers:
      //
      for( size_t i = 0; i < m_existingTriggers.size(); ++i ) {

         // Only look at LVL2 chains here:
         if( m_existingTriggers[ i ].find( "L2_" ) != 0 ) continue;

         // Decide which list to search in:
         std::vector< short >::const_iterator begin, end;
         switch( type ) {

         case TrigDefs::Physics:
            if( m_parent.GetDetailLevel() >= 0 ) {
               begin =
                  m_parent.GetL2Result( Trig::IDataAccess::Physics )->begin();
               end   =
                  m_parent.GetL2Result( Trig::IDataAccess::Physics )->end();
            } else {
               Warning( "GetPassedL2Triggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         case TrigDefs::requireDecision:
            if( m_parent.GetDetailLevel() >= 1 ) {
               begin =
                  m_parent.GetL2Result( Trig::IDataAccess::Raw )->begin();
               end   =
                  m_parent.GetL2Result( Trig::IDataAccess::Raw )->end();
            } else {
               Warning( "GetPassedL2Triggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         case TrigDefs::allowResurrected:
            if( m_parent.GetDetailLevel() >= 2 ) {
               begin =
                  m_parent.GetL2Result( Trig::IDataAccess::Resurrected )->begin();
               end   =
                  m_parent.GetL2Result( Trig::IDataAccess::Resurrected )->end();
            } else {
               Warning( "GetPassedL2Triggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         case TrigDefs::passedThrough:
            if( m_parent.GetDetailLevel() >= 2 ) {
               begin =
                  m_parent.GetL2Result( Trig::IDataAccess::PassedThrough )->begin();
               end   =
                  m_parent.GetL2Result( Trig::IDataAccess::PassedThrough )->end();
            } else {
               Warning( "GetPassedL2Triggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         default:
            Error( "GetPassedL2Triggers",
                   "Decision type not recognised for HLT chains!" );
            return result;
            break;
         }

         // Search for the current ID:
         if( std::find( begin, end, m_existingIDs[ i ] ) != end ) {
            result.push_back( m_existingTriggers[ i ] );
         }
      }

      return result;
   }

   /**
    * The function loops over all the EF chains of this chain group, and checks
    * which one of them passed in the current event. It then returns all the ones
    * that fired.
    *
    * The function is very slow, so it should really just be used in interactive
    * analysis, or in a very last step of an analysis. (When running on relatively
    * few events.)
    *
    * @param type The trigger decision type you're interested in
    * @returns The names of the EF chains from this chain group that fired
    *          in the current event
    */
   std::vector< std::string >
   ChainGroup::GetPassedEFTriggers( TrigDefs::DecisionTypes type ) {

      // The result object:
      std::vector< std::string > result;

      //
      // Make sure the object is up to date:
      //
      if( ! Update() ) {
         Error( "GetPassedEFTriggers", "Trigger(s) can't be evaluated" );
         return result;
      }

      //
      // Loop over the matching triggers:
      //
      for( size_t i = 0; i < m_existingTriggers.size(); ++i ) {

         // Only look at EF chains here:
         if( m_existingTriggers[ i ].find( "EF_" ) != 0 ) continue;

         // Decide which list to search in:
         std::vector< short >::const_iterator begin, end;
         switch( type ) {

         case TrigDefs::Physics:
            if( m_parent.GetDetailLevel() >= 0 ) {
               begin =
                  m_parent.GetEFResult( Trig::IDataAccess::Physics )->begin();
               end   =
                  m_parent.GetEFResult( Trig::IDataAccess::Physics )->end();
            } else {
               Warning( "GetPassedEFTriggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         case TrigDefs::requireDecision:
            if( m_parent.GetDetailLevel() >= 1 ) {
               begin =
                  m_parent.GetEFResult( Trig::IDataAccess::Raw )->begin();
               end   =
                  m_parent.GetEFResult( Trig::IDataAccess::Raw )->end();
            } else {
               Warning( "GetPassedEFTriggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         case TrigDefs::allowResurrected:
            if( m_parent.GetDetailLevel() >= 2 ) {
               begin =
                  m_parent.GetEFResult( Trig::IDataAccess::Resurrected )->begin();
               end   =
                  m_parent.GetEFResult( Trig::IDataAccess::Resurrected )->end();
            } else {
               Warning( "GetPassedEFTriggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         case TrigDefs::passedThrough:
            if( m_parent.GetDetailLevel() >= 2 ) {
               begin =
                  m_parent.GetEFResult( Trig::IDataAccess::PassedThrough )->begin();
               end   =
                  m_parent.GetEFResult( Trig::IDataAccess::PassedThrough )->end();
            } else {
               Warning( "GetPassedEFTriggers",
                        "The requested information is not available in the input" );
               return result;
            }
            break;
         default:
            Error( "GetPassedEFTriggers",
                   "Decision type not recognised for HLT chains!" );
            return result;
            break;
         }

         // Search for the current ID:
         if( std::find( begin, end, m_existingIDs[ i ] ) != end ) {
            result.push_back( m_existingTriggers[ i ] );
         }
      }

      return result;
   }

   /**
    * This function makes sure that the object is aware of the configuration that was
    * used in the current event, and is configured appropriately.
    *
    * @returns <code>kTRUE</code> if the operation was successful,
    *          <code>kFALSE</code> otherwise
    */
   Bool_t ChainGroup::Update() {

      // Return right away if no update is needed:
      if( m_smk == m_parent.GetSMK() ) {
         return kTRUE;
      }

      // Clear the cache variables:
      m_existingTriggers.clear();
      m_existingIDs.clear();

      // Loop over the provided trigger names, and expand them in case they
      // contain lists or wildcards:
      std::vector< std::string >::const_iterator name_itr =
         m_triggerNames.begin();
      std::vector< std::string >::const_iterator name_end =
         m_triggerNames.end();
      for( ; name_itr != name_end; ++name_itr ) {

         // It's also possible to specify triggers like "L1_MU6,L1_EM5". In
         // this case the code has to split up these strings into separate
         // trigger names.
         std::vector< std::string > split_names = ToVector( *name_itr );
         std::vector< std::string >::const_iterator splitname_itr =
            split_names.begin();
         std::vector< std::string >::const_iterator splitname_end =
            split_names.end();
         for( ; splitname_itr != splitname_end; ++splitname_itr ) {

            // Use a TRegexp object only if it's needed:
            TRegexp re( *splitname_itr );
            Ssiz_t len = 0;
            const Bool_t isWildcarded =
               ( ( splitname_itr->find( '^' ) != splitname_itr->npos ) ||
                 ( splitname_itr->find( '$' ) != splitname_itr->npos ) ||
                 ( splitname_itr->find( '.' ) != splitname_itr->npos ) ||
                 ( splitname_itr->find( '[' ) != splitname_itr->npos ) ||
                 ( splitname_itr->find( ']' ) != splitname_itr->npos ) ||
                 ( splitname_itr->find( '*' ) != splitname_itr->npos ) ||
                 ( splitname_itr->find( '+' ) != splitname_itr->npos ) ||
                 ( splitname_itr->find( '?' ) != splitname_itr->npos ) );

            // Find out which LVL1 items match the selection:
            std::vector< std::string >::const_iterator trigger_itr =
               m_configSvc.GetConfigSvc().GetLVL1Items().begin();
            std::vector< std::string >::const_iterator trigger_end =
               m_configSvc.GetConfigSvc().GetLVL1Items().end();
            for( ; trigger_itr != trigger_end; ++trigger_itr ) {
               if( isWildcarded ) {
                  if( re.Index( *trigger_itr, &len ) != kNPOS ) {
                     m_existingTriggers.push_back( *trigger_itr );
                     m_existingIDs.push_back( m_configSvc.GetConfigSvc().GetCTPId( *trigger_itr ) );
                  }
               } else {
                  if( *splitname_itr == *trigger_itr ) {
                     m_existingTriggers.push_back( *trigger_itr );
                     m_existingIDs.push_back( m_configSvc.GetConfigSvc().GetCTPId( *trigger_itr ) );
                  }
               }
            }

            // Find out which HLT chains match the selection:
            trigger_itr = m_configSvc.GetConfigSvc().GetHLTChains().begin();
            trigger_end = m_configSvc.GetConfigSvc().GetHLTChains().end();
            for( ; trigger_itr != trigger_end; ++trigger_itr ) {
               if( isWildcarded ) {
                  if( re.Index( *trigger_itr, &len ) != kNPOS ) {
                     m_existingTriggers.push_back( *trigger_itr );
                     m_existingIDs.push_back( m_configSvc.GetConfigSvc().GetChainId( *trigger_itr ) );
                  }
               } else {
                  if( *splitname_itr == *trigger_itr ) {
                     m_existingTriggers.push_back( *trigger_itr );
                     m_existingIDs.push_back( m_configSvc.GetConfigSvc().GetChainId( *trigger_itr ) );
                  }
               }
            }
         }
      }

      // Do a final sanity check:
      if( m_existingTriggers.size() != m_existingIDs.size() ) {
         Fatal( "Update", "There is a serious problem in the code!" );
         return kFALSE;
      }

      return kTRUE; // The update was successful
   }

   /**
    * This function is used to create properly formatted names from a comma separated
    * list of names. The list can even contain spaces.
    *
    * @param names Comma separated trigger names list
    * @returns The trigger names split into a vector
    */
   std::vector< std::string > ChainGroup::ToVector( const std::string& names ) {

      // First let's erase all whitespace:
      std::string local_names( names );
      std::string::size_type pos;
      while( ( pos = local_names.find_first_of( ' ' ) ) != local_names.npos ) {
         local_names.erase( pos, 1 );
      }

      // Now let's split it up:
      std::vector< std::string > result;
      std::string::size_type old_pos = 0;
      while( ( pos = local_names.find( ',', old_pos ) ) != local_names.npos ) {
         result.push_back( local_names.substr( old_pos, pos - old_pos ) );
         old_pos = pos + 1;
      }
      if( old_pos < local_names.size() ) {
         result.push_back( local_names.substr( old_pos, local_names.size() -
                                               old_pos ) );
      }

      return result;
   }

} // namespace D3PD
