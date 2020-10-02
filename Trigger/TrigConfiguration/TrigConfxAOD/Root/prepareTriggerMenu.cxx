/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: prepareTriggerMenu.cxx 792850 2017-01-18 18:58:03Z ssnyder $

// Infrastructure include(s):
#include "AsgMessaging/MsgStream.h"

// Trigger configuration include(s):
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTSignature.h"

// Local include(s):
#include "TrigConfxAOD/tools/prepareTriggerMenu.h"

namespace TrigConf {

   /// In order to avoid duplicating code between how the dual-use and the
   /// Athena-only code translates the xAOD metadata into the transient
   /// trigger configuration information, this function is used by both
   /// to achieve this.
   ///
   /// @param menu The persistent trigger menu object to translate
   /// @param ctpConfig The LVL1 configuration object to fill
   /// @param chainList The HLT configuration object to fill
   /// @param bgSet The bunch structure configuration object to fill
   /// @param msg MsgStream to print messages to
   /// @returns <code>StatusCode::SUCCESS</code> if successful,
   ///          <code>StatusCode::FAILURE</code> if not
   ///
   StatusCode prepareTriggerMenu( const xAOD::TriggerMenu* menu,
                                  CTPConfig& ctpConfig,
                                  HLTChainList& chainList,
                                  HLTSequenceList& sequenceList,
                                  BunchGroupSet& bgSet,
                                  MsgStream& msg ) {

      // A little sanity check:
      if( ( ! menu->hasStore() ) && ( ( ! menu->container() ) ||
                                      ( ! menu->container()->hasStore() ) ) ) {
         msg << MSG::FATAL << "prepareTriggerMenu(...) Received "
             << "xAOD::TriggerMenu object is not connected to an auxiliary "
             << "store" << endmsg;
         return StatusCode::FAILURE;
      }

      // Clear the current LVL1 configuration:
      ctpConfig.menu().clear();
      ctpConfig.clearPrescaleSets();
      ctpConfig.prescaleSet().resize( 512 );

      // Fill the LVL1 configuration:
      for( size_t i = 0; i < menu->itemCtpIds().size(); ++i ) {
         TriggerItem* item = new TriggerItem();
         item->setName( menu->itemNames()[ i ] );
         item->setCtpId( menu->itemCtpIds()[ i ] );
         ctpConfig.menu().addTriggerItem( item );
         if( menu->itemPrescalesAvailable() ) {
            ctpConfig.prescaleSet().setPrescale( menu->itemCtpIds()[ i ],
						 static_cast< float >( menu->itemPrescales()[ i ] ) );
         }
         if( msg.level() <= MSG::VERBOSE ) {
            msg << MSG::VERBOSE << "L1 item " << menu->itemNames()[ i ]
                << " has ctpid " << menu->itemCtpIds()[ i ] << endmsg;
         }
      }

      // Clear the current HLT configuration:
      chainList.clear();
      sequenceList.clear();

      // A helper variable:
      bool signatureWarningPrinted = false;

      // Fill the HLT configuration:
      for( size_t i = 0; i < menu->chainIds().size(); ++i ) {

         // Figure out which level this chain is from:
         std::string level = "";
         if( menu->chainNames()[ i ].find( "L2_" ) == 0 ) {
            level = "L2";
         } else if( menu->chainNames()[ i ].find( "EF_" ) == 0 ) {
            level = "EF";
         } else if( menu->chainNames()[ i ].find( "HLT_" ) == 0 ) {
            level = "HLT";
         } else {
            msg << MSG::WARNING << "prepareTriggerMenu(...): "
                << "Couldn't figure out 'level' for chain: "
                << menu->chainNames()[ i ] << endmsg;
         }
         // An empty signature list for the chain:
         std::vector< HLTSignature* > signatures;

         // If signature information is available, read it in:
         if( menu->chainSignatureCountersAvailable() &&
             menu->chainSignatureCounters().size() &&
             menu->chainSignatureLogicsAvailable() &&
             menu->chainSignatureLogics().size() &&
             menu->chainSignatureOutputTEsAvailable() &&
             menu->chainSignatureOutputTEs().size() /*&&
             menu->chainSignatureLabelsAvailable() &&
             menu->chainSignatureLabels().size() */) {

            const std::vector< uint32_t >& counters =
               menu->chainSignatureCounters()[ i ];
            const std::vector< int >& logics =
               menu->chainSignatureLogics()[ i ];
            const std::vector< std::vector< std::string > >& outputTEs =
               menu->chainSignatureOutputTEs()[ i ];
            /*
            const std::vector< std::string >& labels =
               menu->chainSignatureLabels()[ i ];
             */

            if( msg.level() <= MSG::VERBOSE ) {
               msg << MSG::VERBOSE << "chain " << menu->chainNames()[ i ]
                   << " has counter " << menu->chainIds()[ i ]
                   << " and " << counters.size() << " signatures" << endmsg;
            }
            for( size_t sig = 0; sig < counters.size(); ++sig ) {
               std::vector< HLTTriggerElement* > outTEs;
               for( size_t outTEcounter = 0;
                    outTEcounter< outputTEs[ sig ].size(); ++outTEcounter ) {
                  HLTTriggerElement* element =
                     new HLTTriggerElement( outputTEs[ sig ][ outTEcounter ] );
                  outTEs.push_back( element );
               }
               HLTSignature* signature =
                  new HLTSignature( counters[ sig ], logics[ sig ], outTEs );
               signatures.push_back( signature );
               if( msg.level() <= MSG::VERBOSE ) {
                  msg << MSG::VERBOSE << "prepared signature: "
                      << *( signatures.back() ) << endmsg;
               }
            }
         } else if( ! signatureWarningPrinted ) {
            msg << MSG::WARNING << "prepareTriggerMenu(...): "
                << "HLT Signature information not available on the input"
                << endmsg;
            signatureWarningPrinted = true;
         }

         // Create the chain object:
         HLTChain* chain = new HLTChain( menu->chainNames()[ i ],
                                         menu->chainIds()[ i ],
                                         1, // Chain version not important
                                         level,
                                         menu->chainParentNames()[ i ],
                                         -1, // Lower chain ID not important
                                         signatures );
         if( menu->chainRerunPrescalesAvailable() ) {
            chain->set_rerun_prescale( menu->chainRerunPrescales()[ i ] );
         }
         if( menu->chainPassthroughPrescalesAvailable() ) {
            chain->set_pass_through( menu->chainPassthroughPrescales()[ i ] );
         }
         if ( menu->chainPrescalesAvailable() ) {
             chain->set_prescale( menu->chainPrescales()[ i ]);
         }

         // Add it to the list of chains:
         if( ! chainList.addHLTChain( chain ) ) {
            msg << MSG::FATAL << "prepareTriggerMenu(...): "
                << "Couldn't add chain \"" << chain->name()
                << "\"" << endmsg;
            delete chain;
            return StatusCode::FAILURE;
         }
      }

      // Add sequence information if it's available:
      if( menu->sequenceInputTEsAvailable() &&
          menu->sequenceOutputTEsAvailable() &&
          menu->sequenceAlgorithmsAvailable() ) {

         for( size_t i = 0; i< menu->sequenceOutputTEs().size(); ++i ) {
            HLTTriggerElement* outputTE =
               new HLTTriggerElement( menu->sequenceOutputTEs()[ i ] );
            std::vector< HLTTriggerElement* > inputTEs;
            for( size_t j = 0; j < menu->sequenceInputTEs()[ i ].size(); ++j ) {
               HLTTriggerElement* te =
                  new HLTTriggerElement( menu->sequenceInputTEs()[ i ][ j ] );
               inputTEs.push_back( te );
            }
            HLTSequence* sequence =
               new HLTSequence( inputTEs, outputTE,
                                menu->sequenceAlgorithms()[ i ] );
            sequenceList.addHLTSequence( sequence );
            // This throws a runtime_error if it fails, which we don't need to
            // handle, since this is a FATAL error anyways.
         }
      } else {
         msg << MSG::WARNING << "prepareTriggerMenu(...): "
             << "HLT Sequence information not available on the input" << endmsg;
      }

      // Check if bunch-groups are available:
      if( menu->bunchGroupBunchesAvailable() ) {

         // Create a new BunchGroupSet object, since an existing one can't be
         // modified... :-/
         BunchGroupSet bgSetNew;

         // Fill it with info:
         for( size_t i = 0; i < menu->bunchGroupBunches().size(); ++i ) {

            // Create a BunchGroup object:
            BunchGroup bg;
            bg.setInternalNumber( i );
            std::vector< uint16_t >::const_iterator b_itr =
               menu->bunchGroupBunches()[ i ].begin();
            std::vector< uint16_t >::const_iterator b_end =
               menu->bunchGroupBunches()[ i ].end();
            for( ; b_itr != b_end; ++b_itr ) {
               bg.addBunch( *b_itr );
            }

            // Add it to the set:
            bgSetNew.addBunchGroup( bg );
         }

         // Replace the current bunch-group set with the new one:
         bgSet = bgSetNew;

      } else {
         msg << MSG::WARNING << "prepareTriggerMenu(...): "
             << "Bunch-group information not available on the "
             << "input" << endmsg;
      }

      // Let the user know what happened:
      msg << MSG::INFO << "Loaded configuration:" << endmsg;
      msg << MSG::INFO << "  SMK = " << menu->smk()
          << ", L1PSK = " << menu->l1psk()
          << ", HLTPSK = " << menu->hltpsk() << endmsg;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace TrigConf
