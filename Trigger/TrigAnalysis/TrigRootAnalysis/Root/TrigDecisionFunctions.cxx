/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionFunctions.cxx 502443 2012-05-25 10:43:25Z krasznaa $

// ROOT include(s):
#include <TError.h>

// STL include(s):
#include <algorithm>
#include <utility>

// Local include(s):
#include "../TrigRootAnalysis/TrigDecisionFunctions.h"
#include "../TrigRootAnalysis/IITrigConfigSvcD3PD.h"

namespace D3PD {

   namespace Trig {

      /**
       * Perhaps the most important function that people need. It can tell the user
       * whether a given LVL1 item or HLT chain passed in the current event.
       *
       * By default the function returns the "physics decision", but it can be asked
       * to give more detailed results as well. The function tries to be smart, so if
       * the user asks for information that is not available in the given D3PD, it
       * should just print a warning. (And return kFALSE.)
       *
       * For a description of the types of trigger results, have a look at the
       * documentation of the TrigDefs::DecisionTypes enumeration.
       *
       * @param name The name of the trigger we're interested in
       * @param type The type of the trigger decision to look up
       * @returns <code>kTRUE</code> if the trigger was passed in the current event,
       *          <code>kFALSE</code> otherwise
       */
      Bool_t TrigDecisionFunctions::IsPassed( const std::string& name,
                                              TrigDefs::DecisionTypes type ) {

         // Check if this trigger is configured for this event:
         if( ! GetConfigSvc().IsConfigured( name ) ) {
            ::Warning( "TrigDecisionFunctions::IsPassed",
                       "Trigger not configured: %s",
                     name.c_str() );
            return kFALSE;
         }

         Int_t id = 0; // Helper variable

         //
         // If we're asking about a LVL1 item:
         //
         if( name.find( "L1_" ) == 0 ) {

            // Find out if the LVL1 item exists in the current configuration:
            if( ( id = GetConfigSvc().GetCTPId( name ) ) < 0 ) {
               ::Warning( "TrigDecisionFunctions::IsPassed",
                          "LVL1 item not known: %s",
                        name.c_str() );
               return kFALSE;
            }

            // Variables helping in looking up the correct bit in the result:
            const Int_t word = id / 32;
            const Int_t bit  = id % 32;

            //
            // Give the result based on a few conditions:
            //
            switch( type ) {

            case TrigDefs::Physics:
               if( GetDetailLevel() >= 0 ) {
                  return ( ( ( * GetL1Result( Trig::IDataAccess::TAV ) )[ word ]
                             >> bit ) & 0x1 );
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::L1_TBP:
               if( GetDetailLevel() >= 1 ) {
                  return ( ( ( * GetL1Result( Trig::IDataAccess::TBP ) )[ word ]
                             >> bit ) & 0x1 );
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::L1_TAP:
               if( GetDetailLevel() >= 1 ) {
                  return ( ( ( * GetL1Result( Trig::IDataAccess::TAP ) )[ word ]
                             >> bit ) & 0x1 );
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            default:
               ::Warning( "TrigDecisionFunctions::IsPassed",
                          "Decision type not recognised. Returning physics "
                          "decision." );
               if( GetDetailLevel() >= 0 ) {
                  return ( ( ( * GetL1Result( Trig::IDataAccess::TAV ) )[ word ]
                             >> bit ) & 0x1 );
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            }
         }

         // Else we have to be talking about an HLT chain:
         if( ( id = GetConfigSvc().GetChainId( name ) ) < 0 ) {
            ::Warning( "TrigDecisionFunctions::IsPassed",
                       "HLT chain not known: %s",
                       name.c_str() );
            return kFALSE;
         }

         //
         // If this is a LVL2 chain:
         //
         if( name.find( "L2_" ) == 0 ) {

            //
            // Decide on which kind of result the user wants:
            //
            std::vector< short >::const_iterator begin, end;
            switch( type ) {

            case TrigDefs::Physics:
               if( GetDetailLevel() >= 0 ) {
                  begin = GetL2Result( Trig::IDataAccess::Physics )->begin();
                  end   = GetL2Result( Trig::IDataAccess::Physics )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::requireDecision:
               if( GetDetailLevel() >= 1 ) {
                  begin = GetL2Result( Trig::IDataAccess::Raw )->begin();
                  end   = GetL2Result( Trig::IDataAccess::Raw )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::allowResurrected:
               if( GetDetailLevel() >= 2 ) {
                  begin =
                     GetL2Result( Trig::IDataAccess::Resurrected )->begin();
                  end   =
                     GetL2Result( Trig::IDataAccess::Resurrected )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::passedThrough:
               if( GetDetailLevel() >= 2 ) {
                  begin =
                     GetL2Result( Trig::IDataAccess::PassedThrough )->begin();
                  end   =
                     GetL2Result( Trig::IDataAccess::PassedThrough )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            default:
               ::Warning( "TrigDecisionFunctions::IsPassed",
                          "Decision type not recognised. Returning physics "
                          "decision." );
               if( GetDetailLevel() >= 0 ) {
                  begin = GetL2Result( Trig::IDataAccess::Physics )->begin();
                  end   = GetL2Result( Trig::IDataAccess::Physics )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            }

            //
            // See if the trigger chain met this condition: 
            //
            if( std::find( begin, end, id ) != end ) {
               return kTRUE;
            } else {
               return kFALSE;
            }
         }

         //
         // If this is an EF chain:
         //
         if( name.find( "EF_" ) == 0 ) {

            //
            // Decide on which kind of result the user wants:
            //
            std::vector< short >::const_iterator begin, end;
            switch( type ) {

            case TrigDefs::Physics:
               if( GetDetailLevel() >= 0 ) {
                  begin = GetEFResult( Trig::IDataAccess::Physics )->begin();
                  end   = GetEFResult( Trig::IDataAccess::Physics )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::requireDecision:
               if( GetDetailLevel() >= 1 ) {
                  begin = GetEFResult( Trig::IDataAccess::Raw )->begin();
                  end   = GetEFResult( Trig::IDataAccess::Raw )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::allowResurrected:
               if( GetDetailLevel() >= 2 ) {
                  begin =
                     GetEFResult( Trig::IDataAccess::Resurrected )->begin();
                  end   =
                     GetEFResult( Trig::IDataAccess::Resurrected )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            case TrigDefs::passedThrough:
               if( GetDetailLevel() >= 2 ) {
                  begin =
                     GetEFResult( Trig::IDataAccess::PassedThrough )->begin();
                  end   =
                     GetEFResult( Trig::IDataAccess::PassedThrough )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            default:
               ::Warning( "TrigDecisionFunctions::IsPassed",
                          "Decision type not recognised. Returning physics "
                          "decision." );
               if( GetDetailLevel() >= 0 ) {
                  begin = GetEFResult( Trig::IDataAccess::Physics )->begin();
                  end   = GetEFResult( Trig::IDataAccess::Physics )->end();
               } else {
                  ::Warning( "TrigDecisionFunctions::IsPassed",
                             "The requested information is not available in the "
                             "input" );
                  return kFALSE;
               }
               break;
            }

            //
            // See if the trigger chain met this condition: 
            //
            if( std::find( begin, end, id ) != end ) {
               return kTRUE;
            } else {
               return kFALSE;
            }
         }

         //
         // If everything went well, we should never reach this point. If we do, then
         // some of the assumptions about the trigger configuration have to be wrong.
         //
         ::Error( "TrigDecisionFunctions::IsPassed",
                  "We've arrived at a serious programming bug!" );
         return kFALSE;
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
       * @returns The names of all the triggers that fired in the current event
       */
      std::vector< std::string >
      TrigDecisionFunctions::GetPassedTriggers( TrigDefs::DecisionTypes type ) {

         // Get all the fired trigger from the 3 trigger levels:
         std::vector< std::string >   result( GetPassedL1Triggers( type ) );
         const std::vector< std::string > l2( GetPassedL2Triggers( type ) );
         const std::vector< std::string > ef( GetPassedEFTriggers( type ) );

         // Now merge them:
         result.insert( result.end(), l2.begin(), l2.end() );
         result.insert( result.end(), ef.begin(), ef.end() );

#ifdef __GXX_EXPERIMENTAL_CXX0X__
         return std::move( result );
#else
         return result;
#endif // C++11 rvalue availability
      }

      /**
       * Get a list of all the LVL1 triggers that passed in the current event.
       * Keep in mind that the function is quite slow. It has to do multiple lookups
       * in an std::map, that I don't see avoiding. It also returns the name list
       * by value, which can take a lot of CPU cycles. (Creating the object on the
       * heap and returning a pointer to it would be terribly prone to memory leaks.)
       *
       * Use this function only in interactive analysis, or just in the final steps
       * of an analysis. (On a "small" number of events.)
       *
       * @param type The trigger decision type you're interested in
       * @returns The names of all the LVL1 items that fired in the current event
       */
      std::vector< std::string >
      TrigDecisionFunctions::
      GetPassedL1Triggers( TrigDefs::DecisionTypes type ) {

         // The result object:
         std::vector< std::string > result;

         // Select the LVL1 result to investigate:
         const std::vector< unsigned int >* l1result = 0;
         switch( type ) {

         case TrigDefs::Physics:
            if( GetDetailLevel() >= 0 ) {
               l1result = GetL1Result( Trig::IDataAccess::TAV );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL1Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         case TrigDefs::L1_TBP:
            if( GetDetailLevel() >= 1 ) {
               l1result = GetL1Result( Trig::IDataAccess::TBP );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL1Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         case TrigDefs::L1_TAP:
            if( GetDetailLevel() >= 1 ) {
               l1result = GetL1Result( Trig::IDataAccess::TAP );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL1Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         default:
            ::Warning( "TrigDecisionFunctions::GetPassedL1Triggers",
                       "Decision type not recognised. Returning physics "
                       "decision." );
            if( GetDetailLevel() >= 0 ) {
               l1result = GetL1Result( Trig::IDataAccess::TAV );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL1Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         }

         // Loop over all bits of the LVL1 result, and decode which LVL1 items have passed:
         for( Int_t id = 0; id < 256; ++id ) {

            // Variables helping in looking up the correct bit in the result:
            const Int_t word = id / 32;
            const Int_t bit  = id % 32;

            // If this bit is on, add the corresponding trigger to the result list:
            if( ( ( *l1result )[ word ] >> bit ) & 0x1 ) {
               result.push_back( GetConfigSvc().GetNameFromCTPId( id ) );
            }
         }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
         return std::move( result );
#else
         return result;
#endif // C++11 rvalue availability
      }

      /**
       * Get a list of all the LVL2 triggers that passed in the current event.
       * Keep in mind that the function is quite slow. It has to do multiple lookups
       * in an std::map, that I don't see avoiding. It also returns the name list
       * by value, which can take a lot of CPU cycles. (Creating the object on the
       * heap and returning a pointer to it would be terribly prone to memory leaks.)
       *
       * Use this function only in interactive analysis, or just in the final steps
       * of an analysis. (On a "small" number of events.)
       *
       * @param type The trigger decision type you're interested in
       * @returns The names of all the LVL2 items that fired in the current event
       */
      std::vector< std::string >
      TrigDecisionFunctions::
      GetPassedL2Triggers( D3PD::TrigDefs::DecisionTypes type ) {

         // The result object:
         std::vector< std::string > result;

         // Select the LVL2 result to investigate:
         const std::vector< short >* l2result = 0;
         switch( type ) {

         case TrigDefs::Physics:
            if( GetDetailLevel() >= 0 ) {
               l2result = GetL2Result( Trig::IDataAccess::Physics );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL2Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         case TrigDefs::requireDecision:
            if( GetDetailLevel() >= 1 ) {
               l2result = GetL2Result( Trig::IDataAccess::Raw );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL2Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         case TrigDefs::allowResurrected:
            if( GetDetailLevel() >= 2 ) {
               l2result = GetL2Result( Trig::IDataAccess::Resurrected );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL2Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         case TrigDefs::passedThrough:
            if( GetDetailLevel() >= 2 ) {
               l2result = GetL2Result( Trig::IDataAccess::PassedThrough );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL2Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         default:
            ::Warning( "TrigDecisionFunctions::GetPassedL2Triggers",
                       "Decision type not recognised. Returning physics "
                       "decision." );
            if( GetDetailLevel() >= 0 ) {
               l2result = GetL2Result( Trig::IDataAccess::Physics );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedL2Triggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         }

         // Loop over all chain IDs and add all names to the result list:
         std::vector< short >::const_iterator itr = l2result->begin();
         std::vector< short >::const_iterator end = l2result->end();
         for( ; itr != end; ++itr ) {
            result.push_back( GetConfigSvc().GetL2NameFromChainId( *itr ) );
         }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
         return std::move( result );
#else
         return result;
#endif // C++11 rvalue availability
      }

      /**
       * Get a list of all the EF triggers that passed in the current event.
       * Keep in mind that the function is quite slow. It has to do multiple lookups
       * in an std::map, that I don't see avoiding. It also returns the name list
       * by value, which can take a lot of CPU cycles. (Creating the object on the
       * heap and returning a pointer to it would be terribly prone to memory leaks.)
       *
       * Use this function only in interactive analysis, or just in the final steps
       * of an analysis. (On a "small" number of events.)
       *
       * @param type The trigger decision type you're interested in
       * @returns The names of all the EF items that fired in the current event
       */
      std::vector< std::string >
      TrigDecisionFunctions::GetPassedEFTriggers( D3PD::TrigDefs::DecisionTypes type ) {

         // The result object:
         std::vector< std::string > result;

         // Select the EF result to investigate:
         const std::vector< short >* efresult = 0;
         switch( type ) {

         case TrigDefs::Physics:
            if( GetDetailLevel() >= 0 ) {
               efresult = GetEFResult( Trig::IDataAccess::Physics );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedEFTriggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         case TrigDefs::requireDecision:
            if( GetDetailLevel() >= 1 ) {
               efresult = GetEFResult( Trig::IDataAccess::Raw );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedEFTriggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         case TrigDefs::allowResurrected:
            if( GetDetailLevel() >= 2 ) {
               efresult = GetEFResult( Trig::IDataAccess::Resurrected );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedEFTriggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         case TrigDefs::passedThrough:
            if( GetDetailLevel() >= 2 ) {
               efresult = GetEFResult( Trig::IDataAccess::PassedThrough );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedEFTriggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         default:
            ::Warning( "TrigDecisionFunctions::GetPassedEFTriggers",
                       "Decision type not recognised. Returning physics "
                       "decision." );
            if( GetDetailLevel() >= 0 ) {
               efresult = GetEFResult( Trig::IDataAccess::Physics );
            } else {
               ::Warning( "TrigDecisionFunctions::GetPassedEFTriggers",
                          "The requested information is not available in the "
                          "input" );
               return result;
            }
            break;
         }

         // Loop over all chain IDs and add all names to the result list:
         std::vector< short >::const_iterator itr = efresult->begin();
         std::vector< short >::const_iterator end = efresult->end();
         for( ; itr != end; ++itr ) {
            result.push_back( GetConfigSvc().GetEFNameFromChainId( *itr ) );
         }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
         return std::move( result );
#else
         return result;
#endif // C++11 rvalue availability
      }

   } // namespace Trig

} // namespace D3PD
