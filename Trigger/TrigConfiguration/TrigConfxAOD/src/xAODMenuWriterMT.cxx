/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

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
#include "TrigConfHLTData/HLTSignature.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTSequenceList.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"

// Local include(s):
#include "xAODMenuWriterMT.h"
#include "PrintVectorHelper.h"


namespace TrigConf {

   xAODMenuWriterMT::xAODMenuWriterMT( const std::string& name,
                                       ISvcLocator* svcLoc )
      : AthReentrantAlgorithm( name, svcLoc ),
        m_trigConf( "TrigConfigSvc", name ),
        m_metaStore( "MetaDataStore", name ),
        m_tmc( nullptr ) {
   }

   xAODMenuWriterMT::~xAODMenuWriterMT() {
   }

   StatusCode xAODMenuWriterMT::initialize() {

      // Greet the user:
      ATH_MSG_INFO( "Initialising - Package version: " << PACKAGE_VERSION );
      ATH_MSG_DEBUG( "EventObjectName   = " << m_eventName );
      ATH_MSG_DEBUG( "MetaObjectName    = " << m_metaName );
      ATH_MSG_VERBOSE( "TrigConfigSvc = " << m_trigConf );
      ATH_MSG_VERBOSE( "MetaDataStore = " << m_metaStore );

      // Retrieve the necessary service(s):
      CHECK( m_trigConf.retrieve() );
      CHECK( m_metaStore.retrieve() );

      CHECK( m_eventName.initialize() ); // WriteHandleKey

      CHECK( m_HLTMenuKey.initialize(m_isHLTJSONConfig) ); // ReadHandleKey
      CHECK( m_L1MenuKey.initialize(m_isL1JSONConfig) ); // ReadHandleKey
 
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

   StatusCode xAODMenuWriterMT::execute(const EventContext& ctx) const {

      std::unique_ptr<xAOD::TrigConfKeys> keys = std::make_unique<xAOD::TrigConfKeys>(
         m_trigConf->masterKey(),
         m_trigConf->lvl1PrescaleKey(),
         m_trigConf->hltPrescaleKey() );

      SG::WriteHandle<xAOD::TrigConfKeys> trigConfKeysWrite(m_eventName, ctx);
      ATH_CHECK( trigConfKeysWrite.record( std::move(keys) ) );

      // Create the keys in the "internal format":
      const TrigKey_t ckeys =
         std::make_pair( m_trigConf->masterKey(),
                         std::make_pair( m_trigConf->lvl1PrescaleKey(),
                                         m_trigConf->hltPrescaleKey() ) );

      // The followign code must only run on one event at a time
      std::lock_guard<std::mutex> lock(m_mutex);

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
      m_tmc->push_back( menu ); // Now owned by MetaDataStore

      //
      // Set its keys:
      //
      menu->setSMK( m_trigConf->masterKey() );
      menu->setL1psk( m_trigConf->lvl1PrescaleKey() );
      menu->setHLTpsk( m_trigConf->hltPrescaleKey() );

      if (m_isL1JSONConfig) {
         CHECK( populateL1FromJSON(menu, ctx) );
      } else {
         CHECK( populateL1FromTrigConf(menu) );
      }

      if (m_isHLTJSONConfig) {
         CHECK( populateHLTFromJSON(menu, ctx) );
      } else {
         CHECK( populateHLTFromTrigConf(menu) );
      }

      CHECK( populateBunchGroup(menu) ); 

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODMenuWriterMT::populateL1FromJSON(xAOD::TriggerMenu* menu, const EventContext& ctx) const {
      //
      // Set its LVL1 information:
      //
      ATH_MSG_DEBUG( "Filling LVL1 information" );
      SG::ReadHandle<TrigConf::L1Menu> l1MenuHandle = SG::makeHandle( m_L1MenuKey, ctx );
      ATH_CHECK( l1MenuHandle.isValid() );
      const size_t nL1Items = l1MenuHandle->size();
      ATH_MSG_DEBUG("Configuring from " << m_L1MenuKey << " with " << nL1Items << " L1 items");

      std::vector< uint16_t > ctpIds;
      std::vector< std::string > itemNames;
      std::vector< float > itemPrescales;

      ctpIds.reserve(nL1Items);
      itemNames.reserve(nL1Items);
      itemPrescales.reserve(nL1Items);

      for (const TrigConf::L1Item& l1 : *l1MenuHandle) {
         // Extract the information:
         ctpIds.push_back( l1.ctpId() );
         itemNames.push_back( l1.name() );
         itemPrescales.push_back( 1.0 ); // TODO

         // Some verbose information:
         ATH_MSG_VERBOSE( "  \"" << itemNames.back() << "\" CTP Id = "
                          << ctpIds.back() << ", prescale = "
                          << itemPrescales.back() );
      }

      menu->setItemCtpIds( ctpIds );
      menu->setItemNames( itemNames );
      menu->setItemPrescales( itemPrescales );

      return StatusCode::SUCCESS;
   }


   StatusCode xAODMenuWriterMT::populateHLTFromJSON(xAOD::TriggerMenu* menu, const EventContext& ctx) const {
       //
      // Set its HLT information:
      //
      ATH_MSG_DEBUG( "Filling HLT information" );
      SG::ReadHandle<TrigConf::HLTMenu> hltMenuHandle = SG::makeHandle( m_HLTMenuKey, ctx );
      ATH_CHECK( hltMenuHandle.isValid() );
      const size_t nChains = hltMenuHandle->size();
      ATH_MSG_DEBUG("Configuring from " << m_HLTMenuKey << " with " << nChains << " chains");

      std::vector< uint16_t > chainIds;
      std::vector< std::string > chainNames, chainParentNames;
      std::vector< float > chainPrescales, chainRerunPrescales,
         chainPassthroughPrescales;

      std::vector< std::vector< uint32_t > > chainSignatureCounters;
      std::vector< std::vector< int > > chainSignatureLogics;
      std::vector< std::vector< std::vector< std::string > > > chainSignatureOutputTEs;
      std::vector< std::vector< std::string > > chainSignatureLabels;

      chainIds.reserve(nChains);
      chainNames.reserve(nChains);
      chainParentNames.reserve(nChains);
      chainPrescales.reserve(nChains);
      chainRerunPrescales.reserve(nChains);
      chainPassthroughPrescales.reserve(nChains);

      chainSignatureCounters.reserve(nChains);
      chainSignatureLogics.reserve(nChains);
      chainSignatureOutputTEs.reserve(nChains);
      chainSignatureLabels.reserve(nChains);

      for (const TrigConf::Chain& ch : *hltMenuHandle) {
         // Extract the information:
         chainIds.push_back( ch.counter() );
         chainNames.push_back( ch.name() );
         chainParentNames.push_back( ch.l1item() );
         chainPrescales.push_back( 1.0 ); // TODO
         chainRerunPrescales.push_back( -1.0 ); // TODO
         chainPassthroughPrescales.push_back( 0.0 ); // Unused in Run3

         std::vector<uint32_t> counters;
         std::vector<int> logics;
         std::vector<std::vector<std::string> > outputTEs;
         std::vector<std::string> labels;

         // Per-step algorithms is TODO, these are blank for now.

         chainSignatureCounters.push_back(counters);
         chainSignatureLogics.push_back(logics);
         chainSignatureOutputTEs.push_back(outputTEs);
         chainSignatureLabels.push_back(labels);

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

      menu->setChainSignatureCounters(chainSignatureCounters);
      menu->setChainSignatureLogics(chainSignatureLogics);
      menu->setChainSignatureOutputTEs(chainSignatureOutputTEs);
      menu->setChainSignatureLabels(chainSignatureLabels);

      //
      // Set its sequence information:
      //
      // TODO

      return StatusCode::SUCCESS;
   }


   StatusCode xAODMenuWriterMT::populateL1FromTrigConf(xAOD::TriggerMenu* menu) const {
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
      std::vector< float > prescales =
         m_trigConf->ctpConfig()->prescaleSet().prescales_float();
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

      return StatusCode::SUCCESS;
   }


   StatusCode xAODMenuWriterMT::populateHLTFromTrigConf(xAOD::TriggerMenu* menu) const {
      //
      // Set its HLT information:
      //
      ATH_MSG_DEBUG( "Filling HLT information" );
      std::vector< uint16_t > chainIds;
      std::vector< std::string > chainNames, chainParentNames;
      std::vector< float > chainPrescales, chainRerunPrescales,
         chainPassthroughPrescales;

      std::vector< std::vector< uint32_t > > chainSignatureCounters;
      std::vector< std::vector< int > > chainSignatureLogics;
      std::vector< std::vector< std::vector< std::string > > > chainSignatureOutputTEs;
      std::vector< std::vector< std::string > > chainSignatureLabels;

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

         std::vector<uint32_t> counters;
         std::vector<int> logics;
         std::vector<std::vector<std::string> > outputTEs;
         std::vector<std::string> labels;

         ATH_MSG_VERBOSE((*chain_itr)->chain_name() << " has " << (*chain_itr)->signatureList().size() << " signatures");
         for(auto& signature : (*chain_itr)->signatureList() ){
            uint32_t cntr = signature->signature_counter();
            counters.push_back(cntr);
            logics.push_back(signature->logic());
            labels.push_back(signature->label());
            std::vector<std::string> outputTEids;
            for(auto& outputTE : signature->outputTEs()){
               outputTEids.push_back(outputTE->name());
            }
            outputTEs.push_back(outputTEids);
            ATH_MSG_VERBOSE("converted this signature: " << *signature);
         }
         chainSignatureCounters.push_back(counters);
         chainSignatureLogics.push_back(logics);
         chainSignatureOutputTEs.push_back(outputTEs);
         chainSignatureLabels.push_back(labels);

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

      menu->setChainSignatureCounters(chainSignatureCounters);
      menu->setChainSignatureLogics(chainSignatureLogics);
      menu->setChainSignatureOutputTEs(chainSignatureOutputTEs);
      menu->setChainSignatureLabels(chainSignatureLabels);

      //
      // Set its sequence information:
      //
      ATH_MSG_DEBUG( "Filling sequence information" );
      auto& sequenceList = m_trigConf->sequences();
      std::vector<std::vector<std::string> > sequenceInputTEs;
      std::vector<std::string> sequenceOutputTE;
      std::vector<std::vector<std::string> > sequenceAlgorithms;

      for(auto& seq : sequenceList){
         std::vector<std::string> inputTEs;
         for(auto& input : seq->inputTEs()) inputTEs.push_back(input->name());
         sequenceInputTEs.push_back(inputTEs);
         sequenceAlgorithms.push_back(seq->algorithms());
         sequenceOutputTE.push_back(seq->outputTE()->name());

         ATH_MSG_VERBOSE("original sequence: \n" << *seq);

         ATH_MSG_VERBOSE("added sequence with: ");
         ATH_MSG_VERBOSE("  inputTEs: " << sequenceInputTEs.back());
         ATH_MSG_VERBOSE("     algos: " << sequenceAlgorithms.back());
         ATH_MSG_VERBOSE("  outputTE: " << sequenceOutputTE.back());
      }

      menu->setSequenceInputTEs(sequenceInputTEs);
      menu->setSequenceOutputTEs(sequenceOutputTE);
      menu->setSequenceAlgorithms(sequenceAlgorithms);

      return StatusCode::SUCCESS;
   }


   StatusCode xAODMenuWriterMT::populateBunchGroup(xAOD::TriggerMenu* menu) const {
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

      return StatusCode::SUCCESS;
   }

} // namespace TrigConf
