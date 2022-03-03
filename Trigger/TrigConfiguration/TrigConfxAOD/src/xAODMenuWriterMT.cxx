/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "KeyWriterTool.h"

namespace TrigConf
{

   xAODMenuWriterMT::xAODMenuWriterMT(const std::string &name,
                                      ISvcLocator *svcLoc)
       : AthReentrantAlgorithm(name, svcLoc),
         m_metaStore("MetaDataStore", name)
   {
   }

   xAODMenuWriterMT::~xAODMenuWriterMT()
   {
   }

   StatusCode xAODMenuWriterMT::initialize()
   {

      // Greet the user:
      ATH_MSG_INFO("Initialising");
      ATH_MSG_VERBOSE("MetaDataStore = " << m_metaStore);

      // Retrieve the necessary service(s):
      ATH_CHECK(m_metaStore.retrieve());

      ATH_CHECK(m_keyWriterTool.retrieve());

      ATH_CHECK(m_HLTMenuKey.initialize());             // ReadHandleKey, but DetStore (so renounce)
      renounce(m_HLTMenuKey);
      ATH_CHECK(m_HLTMonitoringKey.initialize());       // ReadHandleKey, but DetStore (so renounce)
      renounce(m_HLTMonitoringKey);
      ATH_CHECK(m_HLTPrescaleSetInputKey.initialize()); // ReadCondHandleKey

      ATH_CHECK(m_L1MenuKey.initialize());             // ReadHandleKey, but DetStore (so renounce)
      renounce(m_L1MenuKey);
      ATH_CHECK(m_L1PrescaleSetInputKey.initialize()); // ReadCondHandleKey

      ATH_CHECK(m_bgInputKey.initialize()); // ReadCondHandleKey

      // HLT JSON object - contains HLT menus
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> aux_hlt = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> hlt = std::make_unique<xAOD::TriggerMenuJsonContainer>();
      m_menuJSON_hlt = hlt.get(); // Keep a cached pointer from which we can add to the output metastore
      m_menuJSON_hlt->setStore(aux_hlt.get());

      ATH_CHECK(m_metaStore->record(std::move(aux_hlt), m_metaNameJSON_hlt + "Aux."));
      ATH_CHECK(m_metaStore->record(std::move(hlt), m_metaNameJSON_hlt));

      // HLT Monitoring JSON object - contains Monitoring groups for HLT menus
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> aux_hltmonitoring = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> hltmonitoring = std::make_unique<xAOD::TriggerMenuJsonContainer>();
      m_menuJSON_hltmonitoring = hltmonitoring.get(); // Keep a cached pointer from which we can add to the output metastore
      m_menuJSON_hltmonitoring->setStore(aux_hltmonitoring.get());

      ATH_CHECK(m_metaStore->record(std::move(aux_hltmonitoring), m_metaNameJSON_hltmonitoring + "Aux."));
      ATH_CHECK(m_metaStore->record(std::move(hltmonitoring), m_metaNameJSON_hltmonitoring));

      // L1 JSON object - contains L1 menus
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> aux_l1 = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> l1 = std::make_unique<xAOD::TriggerMenuJsonContainer>();
      m_menuJSON_l1 = l1.get(); // Keep a cached pointer from which we can add to the output metastore
      m_menuJSON_l1->setStore(aux_l1.get());

      ATH_CHECK(m_metaStore->record(std::move(aux_l1), m_metaNameJSON_l1 + "Aux."));
      ATH_CHECK(m_metaStore->record(std::move(l1), m_metaNameJSON_l1));

      // HLT PS JSON object - contains prescales sets for HLT menus
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> aux_hltps = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> hltps = std::make_unique<xAOD::TriggerMenuJsonContainer>();
      m_menuJSON_hltps = hltps.get(); // Keep a cached pointer from which we can add to the output metastore
      m_menuJSON_hltps->setStore(aux_hltps.get());

      ATH_CHECK(m_metaStore->record(std::move(aux_hltps), m_metaNameJSON_hltps + "Aux."));
      ATH_CHECK(m_metaStore->record(std::move(hltps), m_metaNameJSON_hltps));

      // L1 PS JSON object - contains prescales sets for L1 menus
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> aux_l1ps = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> l1ps = std::make_unique<xAOD::TriggerMenuJsonContainer>();
      m_menuJSON_l1ps = l1ps.get(); // Keep a cached pointer from which we can add to the output metastore
      m_menuJSON_l1ps->setStore(aux_l1ps.get());

      ATH_CHECK(m_metaStore->record(std::move(aux_l1ps), m_metaNameJSON_l1ps + "Aux."));
      ATH_CHECK(m_metaStore->record(std::move(l1ps), m_metaNameJSON_l1ps));

      // Bunchgroup JSON object - contains bungchgroup configuration
      std::unique_ptr<xAOD::TriggerMenuJsonAuxContainer> aux_bg = std::make_unique<xAOD::TriggerMenuJsonAuxContainer>();
      std::unique_ptr<xAOD::TriggerMenuJsonContainer> bg = std::make_unique<xAOD::TriggerMenuJsonContainer>();
      m_menuJSON_bg = bg.get(); // Keep a cached pointer from which we can add to the output metastore
      m_menuJSON_bg->setStore( aux_bg.get() );

      ATH_CHECK( m_metaStore->record(std::move(aux_bg), m_metaNameJSON_bg + "Aux." ) );
      ATH_CHECK( m_metaStore->record(std::move(bg), m_metaNameJSON_bg ) );

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode xAODMenuWriterMT::execute(const EventContext &ctx) const
   {

      // Create the keys in the "internal format":
      TrigKey_t ckeys;

      // Write to SG via writer tool.
      // Get keys back via pass-by-reference
      ATH_CHECK(m_keyWriterTool->writeKeys(ctx, /*SMK*/ ckeys.first, /*L1PSK*/ ckeys.second.first, /*HLTPSK*/ ckeys.second.second));

      uint32_t bunchgroupKey = 0;
      ATH_CHECK(m_keyWriterTool->writeBunchgroupKey(ctx, bunchgroupKey));

      // The following code must only run on one event at a time
      std::lock_guard<std::mutex> lock(m_mutex);

      if (!m_converted_smk.insert(ckeys.first).second) {
         ATH_MSG_VERBOSE("Already converted SMK: " << ckeys.first);
      } else {
         ATH_MSG_DEBUG("Filling HLT Menu information for SMK:" << ckeys.first);
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
         ATH_MSG_DEBUG("Filling HLT Monitoring information for SMK:" << ckeys.first);
         SG::ReadHandle<TrigConf::HLTMonitoring> hltMonitoringHandle(m_HLTMonitoringKey, ctx);
         if (hltMonitoringHandle.isValid()) {
            std::stringstream hltMonitoringJson;
            hltMonitoringHandle->printRaw(hltMonitoringJson);
            xAOD::TriggerMenuJson *hltmonitoring = new xAOD::TriggerMenuJson();
            m_menuJSON_hltmonitoring->push_back(hltmonitoring); // Now owned by MetaDataStore
            hltmonitoring->setKey(ckeys.first);
            hltmonitoring->setName(hltMonitoringHandle->name());
            hltmonitoring->setPayload(hltMonitoringJson.str());
         } else {
            ATH_MSG_DEBUG("No HLT Monitoring JSON available - skipping.");
         }
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

      if (!m_converted_hltpsk.insert(ckeys.second.second).second) {
         ATH_MSG_VERBOSE("Already converted HLTPSK: " << ckeys.second.second);
      } else {
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

      if (!m_converted_l1psk.insert(ckeys.second.first).second) {
         ATH_MSG_VERBOSE("Already converted LVL1PSK: " << ckeys.second.first);
      } else {
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

      if (!m_converted_bg.insert(bunchgroupKey).second) {
         ATH_MSG_VERBOSE("Already converted Bunchgroup Key: " << bunchgroupKey);
      } else {
         ATH_MSG_DEBUG("Filling prescale information for Bunchgroup Key:" << bunchgroupKey);
         SG::ReadCondHandle<TrigConf::L1BunchGroupSet> bunchgroupHandle(m_bgInputKey, ctx);
         ATH_CHECK(bunchgroupHandle.isValid());
         std::stringstream l1BunchgroupJSON;
         bunchgroupHandle->printRaw(l1BunchgroupJSON);
         xAOD::TriggerMenuJson *l1bg = new xAOD::TriggerMenuJson();
         m_menuJSON_bg->push_back(l1bg); // Now owned by MetaDataStore
         l1bg->setKey(bunchgroupKey);
         l1bg->setName(bunchgroupHandle->name());
         l1bg->setPayload(l1BunchgroupJSON.str());
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

}
