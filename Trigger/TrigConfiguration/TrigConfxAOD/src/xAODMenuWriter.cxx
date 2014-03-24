/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMenuWriter.cxx 589118 2014-03-24 16:20:19Z krasznaa $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Trigger configuration include(s):
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTChain.h"

// EDM include(s):
#include "xAODTrigger/TrigConfKeys.h"
#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"

// Local include(s):
#include "xAODMenuWriter.h"

namespace {
   /// Helper operator for printing the contents of vectors
   template< typename T >
   MsgStream& operator<< ( MsgStream& out,
                           const std::vector< T >& vec ) {

      // A little prefix:
      out << "[";
      // Print the contents:
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << vec[ i ];
         if( i < vec.size() - 1 ) {
            out << ", ";
         }
      }
      // A little postfix:
      out << "]";
      // Return the stream:
      return out;
   }
}

namespace TrigConf {

   xAODMenuWriter::xAODMenuWriter( const std::string& name,
                                   ISvcLocator* svcLoc )
      : AthAlgorithm( name, svcLoc ),
        m_trigConf( "TrigConfigSvc", name ),
        m_metaStore( "MetaDataStore", name ),
        m_tmc( 0 ) {

      declareProperty( "EventObjectName", m_eventName = "TrigConfKeys" );
      declareProperty( "MetaObjectName", m_metaName = "TriggerMenu" );
      declareProperty( "OverwriteEventObj", m_overwriteEventObj = false );

      declareProperty( "TrigConfigSvc", m_trigConf );
      declareProperty( "MetaDataStore", m_metaStore );
   }

   StatusCode xAODMenuWriter::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "EventObjectName   = " << m_eventName );
      ATH_MSG_DEBUG( "MetaObjectName    = " << m_metaName );
      ATH_MSG_DEBUG( "OverwriteEventObj = "
                     << ( m_overwriteEventObj ? "Yes" : "No" ) );
      ATH_MSG_VERBOSE( "TrigConfigSvc = " << m_trigConf );
      ATH_MSG_VERBOSE( "MetaDataStore = " << m_metaStore );

      // Retrieve the necessary service(s):
      CHECK( m_trigConf.retrieve() );
      CHECK( m_metaStore.retrieve() );

      // Clear the internal cache variable:
      m_convertedKeys.clear();

      // Create an empty trigger menu container:
      xAOD::TriggerMenuAuxContainer* aux = new xAOD::TriggerMenuAuxContainer();
      m_tmc = new xAOD::TriggerMenuContainer();
      m_tmc->setStore( aux );

      // Record the trigger configuration metadata into it:
      CHECK( m_metaStore->record( aux, m_metaName + "Aux." ) );
      CHECK( m_metaStore->record( m_tmc, m_metaName ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODMenuWriter::execute() {

      // Check if the event-level object exists already:
      if( ( ! evtStore()->contains< xAOD::TrigConfKeys >( m_eventName ) ) ||
          m_overwriteEventObj ) {

         // Create the trigger configuration keys for the event data:
         xAOD::TrigConfKeys* keys =
            new xAOD::TrigConfKeys( m_trigConf->masterKey(),
                                    m_trigConf->lvl1PrescaleKey(),
                                    m_trigConf->hltPrescaleKey() );

         // Now record it:
         if( m_overwriteEventObj ) {
            CHECK( evtStore()->overwrite( keys, m_eventName, false ) );
         } else {
            CHECK( evtStore()->record( keys, m_eventName ) );
         }
      }

      // Create the keys in the "internal format":
      TrigKey_t ckeys =
         std::make_pair( m_trigConf->masterKey(),
                         std::make_pair( m_trigConf->lvl1PrescaleKey(),
                                         m_trigConf->hltPrescaleKey() ) );

      // Check if we converted this configuration already:
      if( ! m_convertedKeys.insert( ckeys ).second ) {
         ATH_MSG_VERBOSE( "Configuration with keys SMK: "
                          << ckeys.first << ", L1PSK: " << ckeys.second.first
                          << ", HLTPSK: " << ckeys.second.second
                          << " already translated" );
         return StatusCode::SUCCESS;
      }

      // Tell the user what's happening:
      ATH_MSG_INFO( "Converting configuration with keys SMK: "
                    << ckeys.first << ", L1PSK: " << ckeys.second.first
                    << ", HLTPSK: " << ckeys.second.second );

      // Apparently not, so let's make a new object:
      xAOD::TriggerMenu* menu = new xAOD::TriggerMenu();
      m_tmc->push_back( menu );

      //
      // Set its keys:
      //
      menu->setSMK( m_trigConf->masterKey() );
      menu->setL1psk( m_trigConf->lvl1PrescaleKey() );
      menu->setHLTpsk( m_trigConf->hltPrescaleKey() );

      //
      // Set its LVL1 information:
      //
      ATH_MSG_DEBUG( "Filling LVL1 information" );
      std::vector< uint16_t > ctpIds;
      std::vector< std::string > itemNames;
      std::vector< float > itemPrescales;
      TrigConf::ItemContainer::const_iterator item_itr =
         m_trigConf->ctpConfig()->menu().items().begin();
      TrigConf::ItemContainer::const_iterator item_end =
         m_trigConf->ctpConfig()->menu().items().end();
      std::vector< int > prescales =
         m_trigConf->ctpConfig()->prescaleSet().prescales();
      for( ; item_itr != item_end; ++item_itr ) {

         // Extract the information:
         ctpIds.push_back( ( *item_itr )->ctpId() );
         itemNames.push_back( ( *item_itr )->name() );
         itemPrescales.push_back( prescales[ ( *item_itr )->ctpId() ] );

         // Some verbose information:
         ATH_MSG_VERBOSE( "  \"" << itemNames.back() << "\" CTP Id = "
                          << ctpIds.back() << ", prescale = "
                          << itemPrescales.back() );
      }
      menu->setItemCtpIds( ctpIds );
      menu->setItemNames( itemNames );
      menu->setItemPrescales( itemPrescales );

      //
      // Set its HLT information:
      //
      ATH_MSG_DEBUG( "Filling HLT information" );
      std::vector< uint16_t > chainIds;
      std::vector< std::string > chainNames, chainParentNames;
      std::vector< float > chainPrescales, chainRerunPrescales,
         chainPassthroughPrescales;
      TrigConf::HLTChainList::const_iterator chain_itr =
         m_trigConf->chains().begin();
      TrigConf::HLTChainList::const_iterator chain_end =
         m_trigConf->chains().end();
      for( ; chain_itr != chain_end; ++chain_itr ) {

         // Extract the information:
         chainIds.push_back( ( *chain_itr )->chain_counter() );
         chainNames.push_back( ( *chain_itr )->chain_name() );
         chainParentNames.push_back( ( *chain_itr )->lower_chain_name() );
         chainPrescales.push_back( ( *chain_itr )->prescale() );
         chainRerunPrescales.push_back(
              ( *chain_itr )->prescales().getRerunPrescale("").second );
         chainPassthroughPrescales.push_back( ( *chain_itr )->pass_through() );

         // Some verbose information:
         ATH_MSG_VERBOSE( "  \"" << chainNames.back() << "\" Chain Id = "
                          << chainIds.back() << ", parent name = \""
                          << chainParentNames.back() << "\", prescale = "
                          << chainPrescales.back() << ", re-run prescale = "
                          << chainRerunPrescales.back()
                          << ", pass-through presclale = "
                          << chainPassthroughPrescales.back() );
      }
      menu->setChainIds( chainIds );
      menu->setChainNames( chainNames );
      menu->setChainParentNames( chainParentNames );
      menu->setChainPrescales( chainPrescales );
      menu->setChainRerunPrescales( chainRerunPrescales );
      menu->setChainPassthroughPrescales( chainPassthroughPrescales );

      //
      // Set its bunch-group information:
      //
      ATH_MSG_DEBUG( "Filling bunch-group information" );
      std::vector< std::vector< uint16_t > > bgs;
      std::vector< BunchGroup >::const_iterator bg_itr =
         m_trigConf->bunchGroupSet()->bunchGroups().begin();
      std::vector< BunchGroup >::const_iterator bg_end =
         m_trigConf->bunchGroupSet()->bunchGroups().end();
      for( int i = 0; bg_itr != bg_end; ++bg_itr, ++i ) {

         // Extract the information. Unfortunately we need to make
         // and explicit conversion by hand.
         const std::vector< uint16_t > bunches( bg_itr->bunches().begin(),
                                                bg_itr->bunches().end() );
         bgs.push_back( bunches );

         // Some verbose information:
         ATH_MSG_VERBOSE( "  Bunch group " << i << " bunches: "
                          << bgs.back() );
      }
      menu->setBunchGroupBunches( bgs );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace TrigConf
