/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ServiceHandle.h"

#include "TrigConfData/HLTMenu.h"
#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "HLTConfigSvc.h"

#include <memory>

TrigConf::HLTConfigSvc::HLTConfigSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator)
{}

StatusCode TrigConf::HLTConfigSvc::writeConfigToDetectorStore()
{
  auto hltmenu = std::make_unique<TrigConf::HLTMenu>();

  if (m_inputType == "DB") {
    // db menu loader
    TrigConf::TrigDBMenuLoader dbloader(m_dbConnection);
    dbloader.setLevel(TrigConf::MSGTC::WARNING);

    if (dbloader.loadHLTMenu(m_smk, *hltmenu)) {
      ATH_MSG_INFO("Loaded HLT menu from DB " << m_dbConnection << " for SMK " << m_smk.value());
    }
    else {
      ATH_MSG_ERROR("Failed loading HLT menu from DB for SMK " << m_smk.value());
      return StatusCode::FAILURE;
    }
  }
  else if (m_inputType == "FILE") {
    // load the json file into TrigConf::HLTMenu
    TrigConf::JsonFileLoader fileLoader;
    fileLoader.setLevel(TrigConf::MSGTC::WARNING);

    if (fileLoader.loadFile(m_hltFileName, *hltmenu)) {
      hltmenu->setSMK(m_smk);
      ATH_MSG_INFO("Loaded HLT menu file " << m_hltFileName.value());
    }
    else {
      ATH_MSG_ERROR("Failed loading HLT menu file " << m_hltFileName.value());
      return StatusCode::FAILURE;
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
    ATH_MSG_INFO("Recorded HLT menu with key 'HLTTriggerMenu' in the detector store");
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
    ATH_MSG_INFO("Did not locate TrigConf::JobOptionsSvc, not running athenaHLT");
  }

  ATH_CHECK( writeConfigToDetectorStore() );

  return StatusCode::SUCCESS;
}
