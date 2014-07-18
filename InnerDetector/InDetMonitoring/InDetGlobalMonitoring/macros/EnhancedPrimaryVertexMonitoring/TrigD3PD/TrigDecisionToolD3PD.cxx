/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigDecisionToolD3PD.cxx 318255 2010-08-27 11:46:34Z krasznaa $

// STL include(s):
#include <algorithm>

// Local include(s):
#include "TrigDecisionToolD3PD.h"

ClassImp( D3PD::TrigDecisionToolD3PD )

namespace D3PD {

   /**
    * To read any encoded trigger information from a D3PD, the file has to contain
    * a metadata tree called "TrigConfTree". This is located in the file in a
    * subdirectory called "<d3pd tree name>Meta". For instance in the egammaD3PDs
    * the tree is under "egammaMeta/TrigConfTree". If you don't have such a tree
    * in your file, the tool will not be usable.
    *
    * The tool of course also needs to access the event-level TTree holding the
    * event-wise trigger information.
    *
    * @param eventTree Pointer to the event-wise D3PD TTree
    * @param confTree  Pointer to "TrigConfTree"
    * @param prefix    Prefix used by the trigger tools. "trig_" by default.
    */
   TrigDecisionToolD3PD::TrigDecisionToolD3PD( TTree* eventTree, TTree* confTree,
                                               const TString& prefix )
      : TNamed( "TrigDecisionToolD3PD", "Trigger decision tool" ),
        Trig::DataAccess( eventTree, prefix ),
        Trig::ConfigAccess( confTree ),
        Trig::ChainGroupHandling() {

   }

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
   Bool_t TrigDecisionToolD3PD::IsPassed( const std::string& name,
                                          TrigDefs::DecisionTypes type ) {

      // Load the appropriate configuration:
      GetConfigSvc().Load( GetSMK(), GetL1PSK(), GetHLTPSK() );

      Int_t id = 0; // Helper variable

      //
      // If we're asking about a LVL1 item:
      //
      if( name.find( "L1_" ) != name.npos ) {

         // Find out if the LVL1 item exists in the current configuration:
         if( ( id = GetConfigSvc().GetCTPId( name ) ) < 0 ) {
            Warning( "IsPassed", TString::Format( "LVL1 item not known: %s",
                                                  name.c_str() ) );
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
               return ( ( ( * GetL1Result( Trig::DataAccess::TAV ) )[ word ] >> bit ) & 0x1 );
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         case TrigDefs::L1_TBP:
            if( GetDetailLevel() >= 1 ) {
               return ( ( ( * GetL1Result( Trig::DataAccess::TBP ) )[ word ] >> bit ) & 0x1 );
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         case TrigDefs::L1_TAP:
            if( GetDetailLevel() >= 1 ) {
               return ( ( ( * GetL1Result( Trig::DataAccess::TAP ) )[ word ] >> bit ) & 0x1 );
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         default:
            Warning( "IsPassed", "Decision type not recognised. Returning physics decision." );
            if( GetDetailLevel() >= 0 ) {
               return ( ( ( * GetL1Result( Trig::DataAccess::TAV ) )[ word ] >> bit ) & 0x1 );
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         }
      }

      // Else we have to be talking about an HLT chain:
      if( ( id = GetConfigSvc().GetChainId( name ) ) < 0 ) {
         Warning( "IsPassed", TString::Format( "HLT chain not known: %s",
                                               name.c_str() ) );
         return kFALSE;
      }

      //
      // If this is a LVL2 chain:
      //
      if( name.find( "L2_" ) != name.npos ) {

         //
         // Decide on which kind of result the user wants:
         //
         std::vector< short >::const_iterator begin, end;
         switch( type ) {

         case TrigDefs::Physics:
            if( GetDetailLevel() >= 0 ) {
               begin = GetL2Result( Trig::DataAccess::Physics )->begin();
               end   = GetL2Result( Trig::DataAccess::Physics )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         case TrigDefs::requireDecision:
            if( GetDetailLevel() >= 1 ) {
               begin = GetL2Result( Trig::DataAccess::Raw )->begin();
               end   = GetL2Result( Trig::DataAccess::Raw )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         case TrigDefs::allowResurrected:
            if( GetDetailLevel() >= 2 ) {
               begin = GetL2Result( Trig::DataAccess::Resurrected )->begin();
               end   = GetL2Result( Trig::DataAccess::Resurrected )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         case TrigDefs::passedThrough:
            if( GetDetailLevel() >= 2 ) {
               begin = GetL2Result( Trig::DataAccess::PassedThrough )->begin();
               end   = GetL2Result( Trig::DataAccess::PassedThrough )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         default:
            Warning( "IsPassed", "Decision type not recognised. Returning physics decision." );
            if( GetDetailLevel() >= 0 ) {
               begin = GetL2Result( Trig::DataAccess::Physics )->begin();
               end   = GetL2Result( Trig::DataAccess::Physics )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
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
      if( name.find( "EF_" ) != name.npos ) {

         //
         // Decide on which kind of result the user wants:
         //
         std::vector< short >::const_iterator begin, end;
         switch( type ) {

         case TrigDefs::Physics:
            if( GetDetailLevel() >= 0 ) {
               begin = GetEFResult( Trig::DataAccess::Physics )->begin();
               end   = GetEFResult( Trig::DataAccess::Physics )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         case TrigDefs::requireDecision:
            if( GetDetailLevel() >= 1 ) {
               begin = GetEFResult( Trig::DataAccess::Raw )->begin();
               end   = GetEFResult( Trig::DataAccess::Raw )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         case TrigDefs::allowResurrected:
            if( GetDetailLevel() >= 2 ) {
               begin = GetEFResult( Trig::DataAccess::Resurrected )->begin();
               end   = GetEFResult( Trig::DataAccess::Resurrected )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         case TrigDefs::passedThrough:
            if( GetDetailLevel() >= 2 ) {
               begin = GetEFResult( Trig::DataAccess::PassedThrough )->begin();
               end   = GetEFResult( Trig::DataAccess::PassedThrough )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
               return kFALSE;
            }
            break;
         default:
            Warning( "IsPassed", "Decision type not recognised. Returning physics decision." );
            if( GetDetailLevel() >= 0 ) {
               begin = GetEFResult( Trig::DataAccess::Physics )->begin();
               end   = GetEFResult( Trig::DataAccess::Physics )->end();
            } else {
               Warning( "IsPassed",
                        "The requested information is not available in the input" );
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
      Error( "IsPassed", "We've arrived at a serious programming bug!" );
      return kFALSE;
   }

} // namespace D3PD
