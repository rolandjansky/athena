/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfData/HLTMenu.h"
#include "TrigConfData/HLTMonitoring.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "HLTConfigSvc.h"
#include "TrigConfMD5.h"

#include <memory>

TrigConf::HLTConfigSvc::HLTConfigSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator)
{}

StatusCode TrigConf::HLTConfigSvc::writeConfigToDetectorStore()
{
  std::unique_ptr<TrigConf::HLTMenu> hltmenu = std::make_unique<TrigConf::HLTMenu>();
  std::unique_ptr<TrigConf::HLTMonitoring> monitoring;

  if (m_inputType == "DB") {
    // db menu loader
    TrigConf::TrigDBMenuLoader dbloader(m_dbConnection);
    dbloader.setLevel(TrigConf::MSGTC::WARNING);

    ATH_CHECK( dbloader.loadHLTMenu(m_smk, *hltmenu) );
  }
  else if (m_inputType == "FILE") {
    // load the json file into TrigConf::HLTMenu
    TrigConf::JsonFileLoader fileLoader;
    fileLoader.setLevel(TrigConf::MSGTC::WARNING);

    ATH_CHECK( fileLoader.loadFile(m_hltFileName, *hltmenu) );

    uint32_t smk = m_smk.value();
    if (!m_l1FileName.empty() && smk == 0u) {
      auto l1menu = std::make_unique<TrigConf::L1Menu>();
      const bool status = fileLoader.loadFile(m_l1FileName, *l1menu);
      if (status) {
        smk = TrigConf::truncatedHash(*l1menu, *hltmenu);
      } else {
        ATH_MSG_DEBUG("No L1 menu created, cannot compute a MC-SMK in this job");  
      }
    }
    ATH_MSG_INFO("Setting file-loaded HLT Menu SMK to:" << smk);
    hltmenu->setSMK(smk);  // allow assigning a specified or hashed SMK when running from FILE

    if (!m_monitoringFileName.empty()) {
      monitoring.reset( new TrigConf::HLTMonitoring() );
      const bool success = fileLoader.loadFile(m_monitoringFileName, *monitoring);
      if (not success) {
        if (m_monitoringOptional) {
          ATH_MSG_INFO("Unable to read " << m_monitoringFileName 
            << ", but the monitoring collection is flagged as optional. Continuing.");
          monitoring.reset(); // This smartpointer goes back from having Some to having None.
        } else {
          ATH_MSG_ERROR("Unable to read " << m_monitoringFileName 
            << ", the monitoring collection flagged as non-optional in this job.");
          return StatusCode::FAILURE;
        }
      } else { // success
        monitoring->setSMK(smk);
      }
    }
  }
  else {
    ATH_MSG_ERROR("Unknown input type '" << m_inputType
                  << "'. Allowed values: " << m_inputType.documentation());
    return StatusCode::FAILURE;
  }

  ServiceHandle<StoreGateSvc> detStore("StoreGateSvc/DetectorStore", name());
  ATH_CHECK( detStore.retrieve() );
  if (detStore->record(std::move(hltmenu), "HLTTriggerMenu").isSuccess()) {
    ATH_MSG_INFO("Recorded HLT menu as 'HLTTriggerMenu' in detector store");
  }
  if (monitoring and detStore->record(std::move(monitoring), "HLTMonitoringMenu").isSuccess()) {
    ATH_MSG_INFO("Recorded HLT monitoring menu as 'HLTMonitoringMenu' in detector store");
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigConf::HLTConfigSvc::initialize()
{
  // Handle to JobOptionsSvc to retrieve configuration keys
  if (auto joSvc = serviceLocator()->service<TrigConf::IJobOptionsSvc>("JobOptionsSvc")) {
    if (joSvc->superMasterKey() > 0) {
      m_inputType = "DB";
      m_smk = joSvc->superMasterKey();
      m_dbConnection = joSvc->server();
    }
  }
  else {
    ATH_MSG_DEBUG("Did not locate TrigConf::JobOptionsSvc, not running athenaHLT");
  }

  ATH_MSG_INFO("Loading HLT trigger menu from:");
  ATH_MSG_INFO(m_inputType);
  if (m_inputType == "FILE") {
    ATH_MSG_INFO(m_hltFileName);
    ATH_MSG_INFO(m_monitoringFileName);
  }
  else if (m_inputType == "DB") {
    ATH_MSG_INFO(m_dbConnection);
    ATH_MSG_INFO(m_smk);
  }
  ATH_CHECK( writeConfigToDetectorStore() );

  return StatusCode::SUCCESS;
}
