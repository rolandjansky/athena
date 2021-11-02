/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfIO/TrigDBL1BunchGroupSetLoader.h"
#include "TrigConfIO/TrigDBMenuLoader.h"
#include "TrigConfInterfaces/IJobOptionsSvc.h"

#include "LVL1ConfigSvc.h"

#include <memory>

TrigConf::LVL1ConfigSvc::LVL1ConfigSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator)
{}

StatusCode TrigConf::LVL1ConfigSvc::loadRun3StyleMenu()
{
  auto l1menu = std::make_unique<TrigConf::L1Menu>();
  TrigConf::L1BunchGroupSet* l1bgset = nullptr;

  if (m_inputType == "DB") {
    // load l1menu
    TrigConf::TrigDBMenuLoader dbmenuloader(m_dbConnection);
    dbmenuloader.setLevel(TrigConf::MSGTC::WARNING);
    ATH_CHECK( dbmenuloader.loadL1Menu(m_smk, *l1menu) );

    // Online/athenaHLT does not use a bunch group key. For all other
    // use-cases load it here (will be replaced by CondAlg, ATR-24394)
    if (m_bgsk > 0) {
      l1bgset = new TrigConf::L1BunchGroupSet();
      TrigConf::TrigDBL1BunchGroupSetLoader dbbgsloader(m_dbConnection);
      dbbgsloader.setLevel(TrigConf::MSGTC::WARNING);

      ATH_CHECK( dbbgsloader.loadBunchGroupSet(m_bgsk, *l1bgset) );
      l1bgset->setBGSK(m_bgsk);
    }
  }
  else if (m_inputType == "FILE") {
    // json file menu loader
    TrigConf::JsonFileLoader fileLoader;
    fileLoader.setLevel(TrigConf::MSGTC::WARNING);

    ATH_CHECK( fileLoader.loadFile(m_l1FileName, *l1menu) );
    l1menu->setSMK(m_smk);  // allow assigning a dummy SMK when running from FILE

    if (!m_bgsFileName.empty()) {
      l1bgset = new TrigConf::L1BunchGroupSet();
      ATH_CHECK( fileLoader.loadFile(m_bgsFileName, *l1bgset) );
    }
  }
  else {
    ATH_MSG_ERROR("Unknown input type '" << m_inputType
                  << "'. Allowed values: " << m_inputType.documentation());
    return StatusCode::FAILURE;
  }

  ServiceHandle<StoreGateSvc> detStore("StoreGateSvc/DetectorStore", name());
  ATH_CHECK(detStore.retrieve());
  if (detStore->record(std::move(l1menu), "L1TriggerMenu").isSuccess()) {
    ATH_MSG_INFO("Recorded L1 menu as 'L1TriggerMenu' in detector store");
  }
  if (l1bgset && detStore->record(l1bgset, "L1BunchGroup").isSuccess()) {
    ATH_MSG_INFO("Recorded L1 bunchgroup as 'L1BunchGroup' in detector store");
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigConf::LVL1ConfigSvc::initialize()
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

  ATH_MSG_INFO("Loading L1 trigger menu from:");
  ATH_MSG_INFO(m_inputType);
  if (m_inputType == "FILE") {
    ATH_MSG_INFO(m_l1FileName);
    ATH_MSG_INFO(m_bgsFileName);
  }
  else if (m_inputType == "DB") {
    ATH_MSG_INFO(m_dbConnection);
    ATH_MSG_INFO(m_smk);
    ATH_MSG_INFO(m_bgsk);
  }

  ATH_CHECK(loadRun3StyleMenu());

  return StatusCode::SUCCESS;
}
