/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

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

    ATH_CHECK( dbloader.loadHLTMenu(m_smk, *hltmenu) );
  }
  else if (m_inputType == "FILE") {
    // load the json file into TrigConf::HLTMenu
    TrigConf::JsonFileLoader fileLoader;
    fileLoader.setLevel(TrigConf::MSGTC::WARNING);

    ATH_CHECK( fileLoader.loadFile(m_hltFileName, *hltmenu) );
    hltmenu->setSMK(m_smk);  // allow assigning a dummy SMK when running from FILE
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
  }
  else if (m_inputType == "DB") {
    ATH_MSG_INFO(m_dbConnection);
    ATH_MSG_INFO(m_smk);
  }
  ATH_CHECK( writeConfigToDetectorStore() );

  return StatusCode::SUCCESS;
}
