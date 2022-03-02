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

#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/HLTPrescalesSet.h"
#include "TrigConfData/L1PrescalesSet.h"
#include "TrigConfData/L1BunchGroupSet.h"

#include "TrigConfData/HLTChain.h"
#include "TrigConfData/L1Item.h"
#include <type_traits>
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
               outTEs.reserve(outputTEs[ sig ].size());
               for( size_t outTEcounter = 0;
                    outTEcounter< outputTEs[ sig ].size(); ++outTEcounter ) {
                  HLTTriggerElement* element =
                     new HLTTriggerElement( outputTEs[ sig ][ outTEcounter ] );
                  outTEs.push_back( element );
               }
               HLTSignature* signature =
                  new HLTSignature( counters[ sig ], logics[ sig ], std::move(outTEs) );
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
                                         std::move(signatures) );
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
      msg << MSG::INFO << "Loaded xAOD::TriggerMenu configuration:" << endmsg;
      msg << MSG::INFO << "  SMK = " << menu->smk()
          << ", L1PSK = " << menu->l1psk()
          << ", HLTPSK = " << menu->hltpsk() << endmsg;

      // Return gracefully:
      return StatusCode::SUCCESS;
   }


   /// Helper function ptree key->[] to std::vector<T>
   template<typename T>
   std::vector<T> ToVector(const TrigConf::DataStructure& ds, const std::string& child){
      using ptree = boost::property_tree::ptree;
      std::vector<T> return_vector;
      for( const ptree::value_type& entry : ds.data().get_child(child) ) {
         return_vector.push_back( entry.second.get_value<T>() );
      }
      return return_vector;
   }

   /// Helper function ptree key->[[]] to std::vector<std::vector<T>>
   template<typename T>
   std::vector<std::vector<T>> ToVectorVector(const TrigConf::DataStructure& ds, const std::string& child){
      using ptree = boost::property_tree::ptree;
      std::vector<std::vector<T>> return_vector;
      for( const ptree::value_type& outer : ds.data().get_child(child) ) {
         return_vector.push_back(std::vector<T>());
         for (const ptree::value_type& inner : outer.second) {
            return_vector.back().push_back( inner.second.get_value<T>() );
         }
      }
      return return_vector;
   }

   /// Load JSON derived data into legacy structures to maintain 
   /// compatiblity with existing code.
   ///
   /// @param loadedHlt The incoming HLT trigger menu object to translate
   /// @param loadedL1 The incoming L1 trigger menu object to translate
   /// @param loadedHltps The incoming HLT prescales menu object to translate
   /// @param loadedL1ps The incoming L1 prescales menu object to translate
   /// @param loadedBg The incoming bunchgroup object to translate
   /// @param ctpConfig The LVL1 configuration object to fill
   /// @param chainList The HLT configuration object to fill
   /// @param bgSet The bunch structure configuration object to fill
   /// @param msg MsgStream to print messages to
   /// @returns <code>StatusCode::SUCCESS</code> if successful,
   ///          <code>StatusCode::FAILURE</code> if not
   ///
   StatusCode prepareTriggerMenu(const HLTMenu& loadedHlt,
                                 const L1Menu& loadedL1,
                                 const HLTPrescalesSet& loadedHltps,
                                 const L1PrescalesSet& loadedL1ps,
                                 const L1BunchGroupSet& loadedBgSet,
                                 CTPConfig& ctpConfig,
                                 HLTChainList& chainList,
                                 HLTSequenceList& sequenceList,
                                 BunchGroupSet& bgSet,
                                 MsgStream& msg ) {

      // Clear the current LVL1 configuration:
      ctpConfig.menu().clear();
      ctpConfig.clearPrescaleSets();
      ctpConfig.prescaleSet().resize( 512 );

      if (loadedL1.isInitialized()) {
         // Fill the LVL1 configuration:
         for (const L1Item& loadedItem : loadedL1) {
            TriggerItem* item = new TriggerItem();
            item->setName( loadedItem.name() );
            item->setCtpId( loadedItem.ctpId() );
            ctpConfig.menu().addTriggerItem( item );
 
            float ps = -1.0;
            if (loadedL1ps.isInitialized()) {
               const L1PrescalesSet::L1Prescale& loadedPrescale = loadedL1ps.prescale( loadedItem.name() );
               ps = static_cast< float >( loadedPrescale.prescale );
            }
            ctpConfig.prescaleSet().setPrescale( loadedItem.ctpId(), ps );
            
            if( msg.level() <= MSG::VERBOSE ) {
               msg << MSG::VERBOSE << "L1 item " << loadedItem.name()
                  << " has ctpid " << loadedItem.ctpId()
                  << " and prescale " << ps 
                  << endmsg;
            }
         }
      }

      // Clear the current HLT configuration:
      chainList.clear();
      sequenceList.clear();

      // Fill the HLT configuration:
      if (loadedHlt.isInitialized()) {
         for (const Chain& loadedChain : loadedHlt) {
            // Figure out which level this chain is from:
            std::string level = "";
            if( loadedChain.name().find( "L2_" ) == 0 ) {
               level = "L2";
            } else if( loadedChain.name().find( "EF_" ) == 0 ) {
               level = "EF";
            } else if( loadedChain.name().find( "HLT_" ) == 0 ) {
               level = "HLT";
            } else {
               msg << MSG::WARNING << "prepareTriggerMenu(...): "
                   << "Couldn't figure out 'level' for chain: "
                   << loadedChain.name() << endmsg;
            }

            // An empty signature list for the chain:
            std::vector< HLTSignature* > signatures;

            // Optional Run2 payload
            std::vector<uint32_t> counters;
            std::vector<int> logics;
            std::vector<std::vector<std::string>> outputTEs;
            if (loadedChain.hasChild("signature")) {
               counters = ToVector<uint32_t>(loadedChain, "signature.counters");
               logics = ToVector<int>(loadedChain, "signature.logics");
               outputTEs = ToVectorVector<std::string>(loadedChain, "signature.outputTEs");
            }

            if( msg.level() <= MSG::VERBOSE ) {
               msg << MSG::VERBOSE << "chain " << loadedChain.name()
                   << " has counter " << loadedChain.counter()
                   << " and " << counters.size() << " signatures (runs 1,2 only)" << endmsg;
            }

            for( size_t sig = 0; sig < counters.size(); ++sig ) {
               std::vector< HLTTriggerElement* > outTEs;
               outTEs.reserve(outputTEs[ sig ].size());
               for( size_t outTEcounter = 0; outTEcounter< outputTEs[ sig ].size(); ++outTEcounter ) {
                  HLTTriggerElement* element = new HLTTriggerElement( outputTEs[ sig ][ outTEcounter ] );
                  outTEs.push_back( element );
               }
               HLTSignature* signature = new HLTSignature( counters[ sig ], logics[ sig ], std::move(outTEs) );
               signatures.push_back( signature );
               if( msg.level() <= MSG::VERBOSE ) {
                  msg << MSG::VERBOSE << "prepared signature: " << *( signatures.back() ) << endmsg;
               }
            }
 
            // Create the chain object:
            HLTChain* chain = new HLTChain( loadedChain.name(),
                                            loadedChain.counter(),
                                            1, // Chain version not important
                                            level,
                                            loadedChain.l1item(), // L1 seeds (string)
                                            -1, // Lower chain ID not important
                                            std::move(signatures) ); // Empty for R3 JSONs
 
            chain->set_rerun_prescale( -1.0 ); // Not used in R3
            chain->set_pass_through( -1.0 );  // Not used in R3
            chain->set_leg_multiplicities( loadedChain.legMultiplicities() );

            for (const std::string& group : loadedChain.groups()){
               chain->addGroup(group);
            }

            if (loadedHltps.isInitialized()) {
               const HLTPrescalesSet::HLTPrescale& loadedPrescale = loadedHltps.prescale( loadedChain.name() );
               chain->set_prescale( loadedPrescale.prescale );
            } else {
               chain->set_prescale( 0 );
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
      }

      // Add sequence information if it's available (R1 or R2 menu):
      if( loadedHlt.hasChild("sequence_run2") ) {
         std::vector<std::string> menu_outputTEs = ToVector<std::string>(loadedHlt, "sequence_run2.outputTEs");
         std::vector<std::vector<std::string>> menu_inputTEs = ToVectorVector<std::string>(loadedHlt, "sequence_run2.inputTEs");
         std::vector<std::vector<std::string>> menu_algorithms = ToVectorVector<std::string>(loadedHlt, "sequence_run2.algorithms");

         for( size_t i = 0; i< menu_outputTEs.size(); ++i ) {
            HLTTriggerElement* outputTE = new HLTTriggerElement( menu_outputTEs[ i ] );
            std::vector< HLTTriggerElement* > inputTEs;
            for( size_t j = 0; j < menu_inputTEs[ i ].size(); ++j ) {
               HLTTriggerElement* te = new HLTTriggerElement( menu_inputTEs[ i ][ j ] );
               inputTEs.push_back( te );
            }
            HLTSequence* sequence = new HLTSequence( inputTEs, outputTE, menu_algorithms[ i ] );
            sequenceList.addHLTSequence( sequence );
            // This throws a runtime_error if it fails, which we don't need to
            // handle, since this is a FATAL error anyways.
         }
      }

      // Bunchgroup data is TODO
      // Create a new BunchGroupSet object, since an existing one can't be
      // modified... :-/
      BunchGroupSet bgSetNew;

      // Empty structure if data are missing
      if (!loadedBgSet) {

         for( size_t i = 0; i < 16; ++i ) {
            BunchGroup bg;
            bg.setInternalNumber(i);
            bgSetNew.addBunchGroup(bg);
         }

      } else {

         // Fill it with info:
         for( size_t i = 0; i < loadedBgSet.size(); ++i ) {

            const std::shared_ptr<L1BunchGroup> loadedBg = loadedBgSet.getBunchGroup(i);

            // Create a BunchGroup object:
            BunchGroup bg;
            bg.setInternalNumber(i);

            // Not the most efficient of copies, but one format has 16 bit ints whereas the other has 32 bit ints.
            for (const uint16_t b : loadedBg->bunches()) {
               bg.addBunch( static_cast<int>(b) );
            }

            // Add it to the set:
            bgSetNew.addBunchGroup(bg);
         }

      }

      static_assert(std::is_move_assignable<BunchGroupSet>::value);
      // Replace the current bunch-group set with the new one:
      bgSet = std::move(bgSetNew);

      // Return gracefully:
      return StatusCode::SUCCESS;
   }


} // namespace TrigConf
