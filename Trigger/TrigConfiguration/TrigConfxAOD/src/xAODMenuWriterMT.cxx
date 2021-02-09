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
#include "TrigConfHLTData/HLTPrescale.h"

#include "TrigConfData/L1BunchGroupSet.h"

// EDM include(s):
#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"
#include "xAODTrigger/TriggerMenuJson.h"
#include "xAODTrigger/TriggerMenuJsonAuxContainer.h"

// Local include(s):
#include "xAODMenuWriterMT.h"
#include "PrintVectorHelper.h"
#include "TrigConfxAOD/KeyWriterTool.h"

namespace TrigConf
{

   xAODMenuWriterMT::xAODMenuWriterMT(const std::string &name,
                                      ISvcLocator *svcLoc)
       : AthReentrantAlgorithm(name, svcLoc),
         m_metaStore("MetaDataStore", name),
         m_tmc(nullptr)
   {
   }

   xAODMenuWriterMT::~xAODMenuWriterMT()
   {
   }

   StatusCode xAODMenuWriterMT::initialize()
   {

      // Greet the user:
      ATH_MSG_INFO("Initialising - Package version: " << PACKAGE_VERSION);
      ATH_MSG_DEBUG(" EventObjectName   = " << m_eventName);
      ATH_MSG_INFO("LVL1ConfigSvc = " << m_l1TrigConf);
      ATH_MSG_INFO("HLTConfigSvc = " << m_hltTrigConf);
      ATH_MSG_VERBOSE("MetaDataStore = " << m_metaStore);

      // Retrieve the necessary service(s):
      ATH_CHECK(m_l1TrigConf.retrieve());
      ATH_CHECK(m_hltTrigConf.retrieve());
      ATH_CHECK(m_metaStore.retrieve());

      const bool writeKeysWithTool = (m_isHLTJSONConfig && m_isL1JSONConfig);
      if (writeKeysWithTool)
      {
         ATH_CHECK(m_keyWriterTool.retrieve());
      }
      ATH_CHECK(m_eventName.initialize(!writeKeysWithTool)); // WriteHandleKey

      CHECK(m_HLTMenuKey.initialize(m_isHLTJSONConfig));             // ReadHandleKey
      CHECK(m_HLTPrescaleSetInputKey.initialize(m_isHLTJSONConfig)); // ReadCondHandleKey

      CHECK(m_L1MenuKey.initialize(m_isL1JSONConfig));             // ReadHandleKey
      CHECK(m_L1PrescaleSetInputKey.initialize(m_isL1JSONConfig)); // ReadCondHandleKey

      // Clear the internal cache variable:
      m_convertedKeys.clear();

      // Create an empty trigger menu container:
      if (m_writexAODTriggerMenu)
      {

         ATH_MSG_DEBUG("xAOD::TriggerMenuAuxContainer MetaObjectName    = " << m_metaName);

         xAOD::TriggerMenuAuxContainer *aux = new xAOD::TriggerMenuAuxContainer();
         m_tmc = new xAOD::TriggerMenuContainer();
         m_tmc->setStore(aux);

         // Record the trigger configuration metadata into it:
         CHECK(m_metaStore->record(aux, m_metaName + "Aux."));
         CHECK(m_metaStore->record(m_tmc, m_metaName));
      }

      if (m_writexAODTriggerMenuJson)
      {

         if (!m_isHLTJSONConfig)
         {
            ATH_MSG_ERROR("Require IsHLTJSONConfig to be TRUE when WritexAODTriggerMenuJson is TRUE");
            return StatusCode::FAILURE;
         }

         if (!m_isL1JSONConfig)
         {
            ATH_MSG_ERROR("Require IsL1JSONConfig to be TRUE when WritexAODTriggerMenuJson is TRUE");
            return StatusCode::FAILURE;
         }

         // HLT JSON object - contains HLT menus
         xAOD::TriggerMenuJsonAuxContainer *aux_hlt = new xAOD::TriggerMenuJsonAuxContainer();
         m_menuJSON_hlt = new xAOD::TriggerMenuJsonContainer();
         m_menuJSON_hlt->setStore(aux_hlt);

         CHECK(m_metaStore->record(aux_hlt, m_metaNameJSON_hlt + "Aux."));
         CHECK(m_metaStore->record(m_menuJSON_hlt, m_metaNameJSON_hlt));

         // L1 JSON object - contains L1 menus
         xAOD::TriggerMenuJsonAuxContainer *aux_l1 = new xAOD::TriggerMenuJsonAuxContainer();
         m_menuJSON_l1 = new xAOD::TriggerMenuJsonContainer();
         m_menuJSON_l1->setStore(aux_l1);

         CHECK(m_metaStore->record(aux_l1, m_metaNameJSON_l1 + "Aux."));
         CHECK(m_metaStore->record(m_menuJSON_l1, m_metaNameJSON_l1));

         // HLT PS JSON object - contains prescales sets for HLT menus
         xAOD::TriggerMenuJsonAuxContainer *aux_hltps = new xAOD::TriggerMenuJsonAuxContainer();
         m_menuJSON_hltps = new xAOD::TriggerMenuJsonContainer();
         m_menuJSON_hltps->setStore(aux_hltps);

         CHECK(m_metaStore->record(aux_hltps, m_metaNameJSON_hltps + "Aux."));
         CHECK(m_metaStore->record(m_menuJSON_hltps, m_metaNameJSON_hltps));

         // L1 PS JSON object - contains prescales sets for L1 menus
         xAOD::TriggerMenuJsonAuxContainer *aux_l1ps = new xAOD::TriggerMenuJsonAuxContainer();
         m_menuJSON_l1ps = new xAOD::TriggerMenuJsonContainer();
         m_menuJSON_l1ps->setStore(aux_l1ps);

         CHECK(m_metaStore->record(aux_l1ps, m_metaNameJSON_l1ps + "Aux."));
         CHECK(m_metaStore->record(m_menuJSON_l1ps, m_metaNameJSON_l1ps));

         // Bunchgroup JSON object - contains bungchgroup configuration
         // TODO
         // xAOD::TriggerMenuJsonAuxContainer* aux_bg = new xAOD::TriggerMenuJsonAuxContainer();
         // m_menuJSON_bg = new xAOD::TriggerMenuJsonContainer();
         // m_menuJSON_bg->setStore( aux_bg );

         // CHECK( m_metaStore->record( aux_bg, m_metaNameJSON_bg + "Aux." ) );
         // CHECK( m_metaStore->record( m_menuJSON_bg, m_metaNameJSON_bg ) );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODMenuWriterMT::execute(const EventContext &ctx) const
   {

      // Create the keys in the "internal format":
      TrigKey_t ckeys;

      if (m_writexAODTriggerMenuJson)
      {
         // Write to SG via writer tool.
         // Get keys back via pass-by-reference
         ATH_CHECK(m_keyWriterTool->writeKeys(ctx, /*SMK*/ ckeys.first, /*L1PSK*/ ckeys.second.first, /*HLTPSK*/ ckeys.second.second));
      }
      else
      { // Fall back to R2 TrigConfSvc. Expect this to stop working in the near future (2021+)
         ckeys.first = m_hltTrigConf->masterKey();
         ckeys.second.first = m_l1TrigConf->lvl1PrescaleKey();
         ckeys.second.second = m_hltTrigConf->hltPrescaleKey();
         // Write the keys here, do not use the keyWriterTool (this is R3 only)
         std::unique_ptr<xAOD::TrigConfKeys> keys = std::make_unique<xAOD::TrigConfKeys>(ckeys.first, ckeys.second.first, ckeys.second.second);
         SG::WriteHandle<xAOD::TrigConfKeys> trigConfKeysWrite(m_eventName, ctx);
         ATH_CHECK(trigConfKeysWrite.record(std::move(keys)));
      }

      // The following code must only run on one event at a time
      std::lock_guard<std::mutex> lock(m_mutex);

      // TODO - Deprecate this in the future. Only keep m_writexAODTriggerMenuJson mode
      if (m_writexAODTriggerMenu)
      {

         // Check if we converted this configuration already:
         if (!m_convertedKeys.insert(ckeys).second)
         {
            ATH_MSG_VERBOSE("Configuration with keys SMK: "
                            << ckeys.first << ", L1PSK: " << ckeys.second.first
                            << ", HLTPSK: " << ckeys.second.second
                            << " already translated");
         }
         else
         {

            // Tell the user what's happening:
            ATH_MSG_INFO("Converting configuration with keys SMK: "
                         << ckeys.first << ", L1PSK: " << ckeys.second.first
                         << ", HLTPSK: " << ckeys.second.second);

            // Apparently not, so let's make a new object:
            xAOD::TriggerMenu *menu = new xAOD::TriggerMenu();
            m_tmc->push_back(menu); // Now owned by MetaDataStore

            //
            // Set its keys:
            //
            menu->setSMK(ckeys.first);
            menu->setL1psk(ckeys.second.first);
            menu->setHLTpsk(ckeys.second.second);

            if (m_isL1JSONConfig)
            {
               CHECK(populateL1FromJSON(menu, ctx));
            }
            else
            {
               CHECK(populateL1FromTrigConf(menu));
            }

            if (m_isHLTJSONConfig)
            {
               CHECK(populateHLTFromJSON(menu, ctx));
            }
            else
            {
               CHECK(populateHLTFromTrigConf(menu));
            }

            CHECK(populateBunchGroup(menu));
         } // m_convertedKeys
      }    // m_writexAODTriggerMenu

      if (m_writexAODTriggerMenuJson)
      {

         if (!m_converted_smk.insert(ckeys.first).second)
         {
            ATH_MSG_VERBOSE("Already converted SMK: " << ckeys.first);
         }
         else
         {
            ATH_MSG_DEBUG("Filling HLT information for SMK:" << ckeys.first);
            SG::ReadHandle<TrigConf::HLTMenu> hltMenuHandle(m_HLTMenuKey, ctx);
            ATH_CHECK(hltMenuHandle.isValid());
            std::stringstream hltTriggerMenuJson;
            hltMenuHandle->printRaw(hltTriggerMenuJson);
            xAOD::TriggerMenuJson *hlt = new xAOD::TriggerMenuJson();
            m_menuJSON_hlt->push_back(hlt); // Now owned by MetaDataStore
            hlt->setKey(ckeys.first);
            hlt->setName(hltMenuHandle->name());
            hlt->setPayload(hltTriggerMenuJson.str());
            //////////////////////////////////////////////////////////////////////////////
            ATH_MSG_DEBUG("Filling L1 information for SMK:" << ckeys.first);
            SG::ReadHandle<TrigConf::L1Menu> l1MenuHandle = SG::makeHandle(m_L1MenuKey, ctx);
            ATH_CHECK(l1MenuHandle.isValid());
            std::stringstream l1TriggerMenuJson;
            l1MenuHandle->printRaw(l1TriggerMenuJson);
            xAOD::TriggerMenuJson *l1 = new xAOD::TriggerMenuJson();
            m_menuJSON_l1->push_back(l1); // Now owned by MetaDataStore
            l1->setKey(ckeys.first);
            l1->setName(l1MenuHandle->name());
            l1->setPayload(l1TriggerMenuJson.str());
         }

         if (!m_converted_hltpsk.insert(ckeys.second.second).second)
         {
            ATH_MSG_VERBOSE("Already converted HLTPSK: " << ckeys.second.second);
         }
         else
         {
            ATH_MSG_DEBUG("Filling prescale information for HLTPSK:" << ckeys.second.second);
            SG::ReadCondHandle<TrigConf::HLTPrescalesSet> hltPSHandle(m_HLTPrescaleSetInputKey, ctx);
            ATH_CHECK(hltPSHandle.isValid());
            std::stringstream hltPSJSON;
            hltPSHandle->printRaw(hltPSJSON);
            xAOD::TriggerMenuJson *hltps = new xAOD::TriggerMenuJson();
            m_menuJSON_hltps->push_back(hltps); // Now owned by MetaDataStore
            hltps->setKey(ckeys.second.second);
            hltps->setName(hltPSHandle->name());
            hltps->setPayload(hltPSJSON.str());
         }

         if (!m_converted_l1psk.insert(ckeys.second.first).second)
         {
            ATH_MSG_VERBOSE("Already converted LVL1PSK: " << ckeys.second.first);
         }
         else
         {
            ATH_MSG_DEBUG("Filling prescale information for LVL1PSK:" << ckeys.second.first);
            SG::ReadCondHandle<TrigConf::L1PrescalesSet> l1PSHandle(m_L1PrescaleSetInputKey, ctx);
            ATH_CHECK(l1PSHandle.isValid());
            std::stringstream l1PSJSON;
            l1PSHandle->printRaw(l1PSJSON);
            xAOD::TriggerMenuJson *l1ps = new xAOD::TriggerMenuJson();
            m_menuJSON_l1ps->push_back(l1ps); // Now owned by MetaDataStore
            l1ps->setKey(ckeys.second.first);
            l1ps->setName(l1PSHandle->name());
            l1ps->setPayload(l1PSJSON.str());
         }

         //
         // TODO: bg
         //
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODMenuWriterMT::populateL1FromJSON(xAOD::TriggerMenu *menu, const EventContext &ctx) const
   {
      //
      // Set its LVL1 information:
      //
      ATH_MSG_DEBUG("Filling LVL1 information");
      SG::ReadHandle<TrigConf::L1Menu> l1MenuHandle(m_L1MenuKey, ctx);
      ATH_CHECK(l1MenuHandle.isValid());
      const size_t nL1Items = l1MenuHandle->size();
      ATH_MSG_DEBUG("Configuring from " << m_L1MenuKey << " with " << nL1Items << " L1 items");

      SG::ReadCondHandle<TrigConf::L1PrescalesSet> l1PSHandle(m_L1PrescaleSetInputKey, ctx);
      ATH_CHECK(l1PSHandle.isValid());
      const size_t nL1ItemsPS = l1PSHandle->size();
      ATH_MSG_DEBUG("Prescales from " << m_L1PrescaleSetInputKey << " with " << nL1ItemsPS << " L1 items");

      if (nL1Items != nL1ItemsPS)
      {
         ATH_MSG_ERROR("Inconsistent number of configured L1 items (" << nL1Items << ") and L1 item prescales (" << nL1ItemsPS << ")");
         return StatusCode::FAILURE;
      }

      std::vector<uint16_t> ctpIds;
      std::vector<std::string> itemNames;
      std::vector<float> itemPrescales;

      ctpIds.reserve(nL1Items);
      itemNames.reserve(nL1Items);
      itemPrescales.reserve(nL1Items);

      for (const TrigConf::L1Item &l1 : *l1MenuHandle)
      {
         // Extract the information:
         ctpIds.push_back(l1.ctpId());
         itemNames.push_back(l1.name());
         const TrigConf::L1PrescalesSet::L1Prescale l1ps = l1PSHandle->prescale(l1.name());
         itemPrescales.push_back(l1ps.prescale);

         // Some verbose information:
         ATH_MSG_VERBOSE("  \"" << itemNames.back() << "\" CTP Id = "
                                << ctpIds.back() << ", prescale = "
                                << itemPrescales.back());
      }

      menu->setItemCtpIds(ctpIds);
      menu->setItemNames(itemNames);
      menu->setItemPrescales(itemPrescales);

      return StatusCode::SUCCESS;
   }

   StatusCode xAODMenuWriterMT::populateHLTFromJSON(xAOD::TriggerMenu *menu, const EventContext &ctx) const
   {
      //
      // Set its HLT information:
      //
      ATH_MSG_DEBUG("Filling HLT information");
      SG::ReadHandle<TrigConf::HLTMenu> hltMenuHandle(m_HLTMenuKey, ctx);
      ATH_CHECK(hltMenuHandle.isValid());
      const size_t nChains = hltMenuHandle->size();
      ATH_MSG_DEBUG("Configuring from " << m_HLTMenuKey << " with " << nChains << " chains");

      SG::ReadCondHandle<TrigConf::HLTPrescalesSet> hltPSHandle(m_HLTPrescaleSetInputKey, ctx);
      ATH_CHECK(hltPSHandle.isValid());
      const size_t nChainsPS = hltPSHandle->size();
      ATH_MSG_DEBUG("Prescales from " << m_HLTPrescaleSetInputKey << " with " << nChains << " chains");

      if (nChains != nChainsPS)
      {
         ATH_MSG_ERROR("Inconsistent number of configured HLT chains (" << nChains << ") and HLT chain prescales (" << nChainsPS << ")");
         return StatusCode::FAILURE;
      }

      std::vector<uint16_t> chainIds;
      std::vector<std::string> chainNames, chainParentNames;
      std::vector<float> chainPrescales, chainRerunPrescales,
          chainPassthroughPrescales;

      std::vector<std::vector<uint32_t>> chainSignatureCounters;
      std::vector<std::vector<int>> chainSignatureLogics;
      std::vector<std::vector<std::vector<std::string>>> chainSignatureOutputTEs;
      std::vector<std::vector<std::string>> chainSignatureLabels;

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

      for (const TrigConf::Chain &ch : *hltMenuHandle)
      {
         // Extract the information:
         chainIds.push_back(ch.counter());
         chainNames.push_back(ch.name());
         chainParentNames.push_back(ch.l1item());
         const TrigConf::HLTPrescalesSet::HLTPrescale hltps = hltPSHandle->prescale(ch.name());
         chainPrescales.push_back(hltps.prescale);
         chainRerunPrescales.push_back(-1.0);       // Unused in Run3
         chainPassthroughPrescales.push_back(-1.0); // Unused in Run3

         std::vector<uint32_t> counters;
         std::vector<int> logics;
         std::vector<std::vector<std::string>> outputTEs;
         std::vector<std::string> labels;

         // Per-step algorithms is TODO, these are blank for now.

         chainSignatureCounters.push_back(counters);
         chainSignatureLogics.push_back(logics);
         chainSignatureOutputTEs.push_back(outputTEs);
         chainSignatureLabels.push_back(labels);

         // Some verbose information:
         ATH_MSG_VERBOSE("  \"" << chainNames.back() << "\" Chain Id = "
                                << chainIds.back() << ", parent name = \""
                                << chainParentNames.back() << "\", prescale = "
                                << chainPrescales.back() << ", re-run prescale = "
                                << chainRerunPrescales.back()
                                << ", pass-through presclale = "
                                << chainPassthroughPrescales.back());
      }

      menu->setChainIds(chainIds);
      menu->setChainNames(chainNames);
      menu->setChainParentNames(chainParentNames);
      menu->setChainPrescales(chainPrescales);
      menu->setChainRerunPrescales(chainRerunPrescales);
      menu->setChainPassthroughPrescales(chainPassthroughPrescales);

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

   StatusCode xAODMenuWriterMT::populateL1FromTrigConf(xAOD::TriggerMenu *menu) const
   {
      //
      // Set its LVL1 information:
      //
      ATH_MSG_DEBUG("Filling LVL1 information");
      std::vector<uint16_t> ctpIds;
      std::vector<std::string> itemNames;
      std::vector<float> itemPrescales;
      TrigConf::ItemContainer::const_iterator item_itr =
          m_l1TrigConf->ctpConfig()->menu().items().begin();
      TrigConf::ItemContainer::const_iterator item_end =
          m_l1TrigConf->ctpConfig()->menu().items().end();
      std::vector<float> prescales =
          m_l1TrigConf->ctpConfig()->prescaleSet().prescales_float();
      for (; item_itr != item_end; ++item_itr)
      {

         // Extract the information:
         ctpIds.push_back((*item_itr)->ctpId());
         itemNames.push_back((*item_itr)->name());
         itemPrescales.push_back(prescales[(*item_itr)->ctpId()]);

         // Some verbose information:
         ATH_MSG_VERBOSE("  \"" << itemNames.back() << "\" CTP Id = "
                                << ctpIds.back() << ", prescale = "
                                << itemPrescales.back());
      }
      menu->setItemCtpIds(ctpIds);
      menu->setItemNames(itemNames);
      menu->setItemPrescales(itemPrescales);

      return StatusCode::SUCCESS;
   }

   StatusCode xAODMenuWriterMT::populateHLTFromTrigConf(xAOD::TriggerMenu *menu) const
   {
      //
      // Set its HLT information:
      //
      ATH_MSG_DEBUG("Filling HLT information");
      std::vector<uint16_t> chainIds;
      std::vector<std::string> chainNames, chainParentNames;
      std::vector<float> chainPrescales, chainRerunPrescales,
          chainPassthroughPrescales;

      std::vector<std::vector<uint32_t>> chainSignatureCounters;
      std::vector<std::vector<int>> chainSignatureLogics;
      std::vector<std::vector<std::vector<std::string>>> chainSignatureOutputTEs;
      std::vector<std::vector<std::string>> chainSignatureLabels;

      TrigConf::HLTChainList::const_iterator chain_itr =
          m_hltTrigConf->chains().begin();
      TrigConf::HLTChainList::const_iterator chain_end =
          m_hltTrigConf->chains().end();
      for (; chain_itr != chain_end; ++chain_itr)
      {

         // Extract the information:
         chainIds.push_back((*chain_itr)->chain_counter());
         chainNames.push_back((*chain_itr)->chain_name());
         chainParentNames.push_back((*chain_itr)->lower_chain_name());
         chainPrescales.push_back((*chain_itr)->prescale());
         chainRerunPrescales.push_back(
             (*chain_itr)->prescales().getRerunPrescale("").second);
         chainPassthroughPrescales.push_back((*chain_itr)->pass_through());

         std::vector<uint32_t> counters;
         std::vector<int> logics;
         std::vector<std::vector<std::string>> outputTEs;
         std::vector<std::string> labels;

         ATH_MSG_VERBOSE((*chain_itr)->chain_name() << " has " << (*chain_itr)->signatureList().size() << " signatures");
         for (auto &signature : (*chain_itr)->signatureList())
         {
            uint32_t cntr = signature->signature_counter();
            counters.push_back(cntr);
            logics.push_back(signature->logic());
            labels.push_back(signature->label());
            std::vector<std::string> outputTEids;
            for (auto &outputTE : signature->outputTEs())
            {
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
         ATH_MSG_VERBOSE("  \"" << chainNames.back() << "\" Chain Id = "
                                << chainIds.back() << ", parent name = \""
                                << chainParentNames.back() << "\", prescale = "
                                << chainPrescales.back() << ", re-run prescale = "
                                << chainRerunPrescales.back()
                                << ", pass-through presclale = "
                                << chainPassthroughPrescales.back());
      }
      menu->setChainIds(chainIds);
      menu->setChainNames(chainNames);
      menu->setChainParentNames(chainParentNames);
      menu->setChainPrescales(chainPrescales);
      menu->setChainRerunPrescales(chainRerunPrescales);
      menu->setChainPassthroughPrescales(chainPassthroughPrescales);

      menu->setChainSignatureCounters(chainSignatureCounters);
      menu->setChainSignatureLogics(chainSignatureLogics);
      menu->setChainSignatureOutputTEs(chainSignatureOutputTEs);
      menu->setChainSignatureLabels(chainSignatureLabels);

      //
      // Set its sequence information:
      //
      ATH_MSG_DEBUG("Filling sequence information");
      auto &sequenceList = m_hltTrigConf->sequences();
      std::vector<std::vector<std::string>> sequenceInputTEs;
      std::vector<std::string> sequenceOutputTE;
      std::vector<std::vector<std::string>> sequenceAlgorithms;

      for (auto &seq : sequenceList)
      {
         std::vector<std::string> inputTEs;
         for (auto &input : seq->inputTEs())
            inputTEs.push_back(input->name());
         sequenceInputTEs.push_back(inputTEs);
         sequenceAlgorithms.push_back(seq->algorithms());
         sequenceOutputTE.push_back(seq->outputTE()->name());

         ATH_MSG_VERBOSE("original sequence: \n"
                         << *seq);

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
} // namespace TrigConf

StatusCode
TrigConf::xAODMenuWriterMT::populateBunchGroup(xAOD::TriggerMenu *menu) const
{
   //
   // Set its bunch-group information:
   //
   ATH_MSG_DEBUG("Filling bunch-group information");
   std::vector<std::vector<uint16_t>> bgs;
   if (m_isL1JSONConfig)
   {
      const TrigConf::L1BunchGroupSet *l1bgs = nullptr;
      detStore()->retrieve(l1bgs).ignore();
      if (l1bgs)
      {
         for (size_t i = 0; i < l1bgs->maxNBunchGroups(); ++i)
         {
            auto bg = l1bgs->getBunchGroup(i);
            bgs.push_back(bg->bunches());
         }
      }
      else
      {
         ATH_MSG_ERROR("Did not find L1BunchGroupSet in DetectorStore");
      }
   }
   else
   {
      // vector of 16 bunchgroups, each containing the vector of BCIDs
      for (auto &bunchGroup : m_l1TrigConf->bunchGroupSet()->bunchGroups())
      {
         const std::vector<uint16_t> bunches(bunchGroup.bunches().begin(),
                                             bunchGroup.bunches().end());
         bgs.push_back(bunches);
      }
   }
   menu->setBunchGroupBunches(bgs);
   return StatusCode::SUCCESS;
}
