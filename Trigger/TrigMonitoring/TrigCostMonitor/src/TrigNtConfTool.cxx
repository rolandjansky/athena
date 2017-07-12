/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/types.h>

// Framework
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/errorcheck.h"

// LV1 and HLT configuration
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigT1Result/CTP_RDO.h"
#include "CTPfragment/CTPfragment.h"
#include "CTPfragment/CTPExtraWordsFormat.h"
#include "CTPfragment/Issue.h"

// Loader classes
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/IHLTPrescaleSetLoader.h"

// Local
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostMonitor/FillConf.h"
#include "TrigCostMonitor/ReadConf.h"
#include "TrigCostMonitor/UtilCost.h"
#include "TrigCostMonitor/TrigNtConfTool.h"

//---------------------------------------------------------------------------------------
Trig::TrigNtConfTool::TrigNtConfTool(const std::string &name,
             const std::string &type,
             const IInterface  *parent)
  :AthAlgTool(name, type, parent),
   m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_storage(0),
   m_hltFrame(0),
   m_config_db(0),
   m_config_sv(0),
   m_countConfig_db(0),
   m_countConfig_sv(0),
   m_run(0),
   m_dbOrConfSvcPass(0),
   m_currentL1PSK(0)
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("configSvc",        m_configSvc);
  declareProperty("printConfig",      m_printConfig       = false);
  declareProperty("useDB",            m_useDB             = true);
  declareProperty("useConfSvc",       m_useConfSvc        = true);

  declareProperty("connectionCool",   m_connectionCool    = "COOLONL_TRIGGER/CONDBR2");
  declareProperty("connectionTrig",   m_connectionTrig    = "TRIGGERDB");
  declareProperty("username",         m_username          = "");
  declareProperty("password",         m_password          = "");

  declareProperty("triggerMenuSetup", m_triggerMenuSetup  = "");
  declareProperty("L1PrescaleSet",    m_L1PrescaleSet     = "");
  declareProperty("HLTPrescaleSet",   m_HLTPrescaleSet    = "");
}

//---------------------------------------------------------------------------------------
Trig::TrigNtConfTool::~TrigNtConfTool()
{
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtConfTool::initialize()
{    
  
  if(m_useConfSvc) {
    CHECK(m_configSvc.retrieve());
    ATH_MSG_INFO("Retrieved TrigConfigSvc " << m_configSvc);
  }

  ATH_MSG_INFO("printConfig  = " << m_printConfig);
  ATH_MSG_INFO("useDB        = " << m_useDB      );
  ATH_MSG_INFO("useConfSvc   = " << m_useConfSvc );

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtConfTool::finalize()
{
  //
  // Clean up
  //
  ATH_MSG_DEBUG("finalize()"); 
  
  if(m_storage) {
    delete m_storage;
  }
  if(m_hltFrame) {
    delete m_hltFrame;
  }

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::Fill(TrigMonConfig *confg)
{
  //
  // Fill current trigger configuration
  //
  if(!confg) {
    ATH_MSG_WARNING("Null TrigMonConfig pointer" );
    return false;
  }

  ATH_MSG_DEBUG("Filling Trigger Configuration using Option: " << m_dbOrConfSvcPass << ". (1=ConfSvc, 2=DB)." );

  // Require each call of "Fill" to act on only one of DB or ConfSvc
  if(m_dbOrConfSvcPass != 1 && m_dbOrConfSvcPass != 2) {
    ATH_MSG_WARNING("Set one of SetOption(1) for ConfSvc OR SetOption(2) for DB to true before calling Fill" );
    return false;
  }

  if (m_dbOrConfSvcPass == 2) { //2 for DB
    m_dbOrConfSvcPass = 0;

    if(m_useDB == false) {
      ATH_MSG_WARNING("Trying to run a Database pass (SetOption(2)), but m_useDB is false. Aborting"  );
      return false;
    }

    //
    // Save first configuration pointer
    //
    //if(!m_config_db) { // TIMM removed this conditional
    m_config_db = confg;
    //}
    return ReadFromDB(*m_config_db, m_config_db->getRun(), m_config_db->getLumi());
  }

  if (m_dbOrConfSvcPass == 1) { //1 for ConfSercue
    m_dbOrConfSvcPass = 0;

    if(m_useConfSvc == false) {
      ATH_MSG_WARNING("Trying to run a Conf Service pass (SetOption(1)), but m_useConfSvc is false. Aborting"  );
      return false;
    }

    //
    // Save first configuration pointer
    //
    //if(!m_config_sv) { // TIMM removed this conditional
    m_config_sv = confg;
    //}
    return ReadFromSv(*m_config_sv);
  }

  return false;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::Fill(TrigMonEvent &event)
{
  //
  // Check if wee need to update configuration
  //
  if(!m_config_db || !m_useDB) {
    return true;
  }
  
  //
  // Read trigger chain and sequence configuration from DB or use cached result
  //
  if(ReadFromDB(*m_config_db, event.getRun(), event.getLumi())) {
    
    m_config_db->setEventID(event.getEvent(),
       event.getLumi(),
       event.getRun(),
       event.getSec(),
       event.getNanoSec());
  }

  return true;
}

uint32_t Trig::TrigNtConfTool::GetL1PSK(bool updateValue) {

  if (updateValue == true) {
    ATH_MSG_DEBUG("Updating L1 PSK value from CTP fragment.");

    const CTP_RDO* ctpRDO = nullptr; 
    ctpRDO = evtStore()->tryConstRetrieve<CTP_RDO>();
    if (!ctpRDO) {
      ATH_MSG_WARNING("Could not retrieve CTP_RDO (converted from CTP DAQ ROB). Cannot update L1 prescale key in HLT Cost Monitoring.");
      return m_currentL1PSK;
    }

    // Expect at least 2 initial words, plus our payload, plus extra folders (optional)
    std::vector<uint32_t> l1ExtraPayload = ctpRDO->getEXTRAWords();
    if (l1ExtraPayload.size() < 3) {
      ATH_MSG_WARNING("Extra data in CTP_RDO was size < 3. Cannot update L1 prescale key in HLT Cost Monitoring.");
      return m_currentL1PSK;
    }

    // Remove first two entries (time since previous crossing and turn counter)
    l1ExtraPayload.erase(l1ExtraPayload.begin(), l1ExtraPayload.begin()+2);

    // Extra check - we don't want CTPfragment to trip up decoding this
    if (l1ExtraPayload.size() > 1 && l1ExtraPayload.at(1) > l1ExtraPayload.size() - 2) {
      ATH_MSG_WARNING("Mal-formatted extra payload data. Cannot update L1 prescale key in HLT Cost Monitoring.");
      return m_currentL1PSK;
    }

    CTPfragment::ExtraPayload ctp_payload(l1ExtraPayload);
    m_currentL1PSK = ctp_payload.getL1PSK();
    ATH_MSG_DEBUG("L1 PSK is now " << m_currentL1PSK);
 
  }

  return m_currentL1PSK;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::GetKeysChangedFromSv(TrigMonConfig &confg) {

  // First call
  if (m_countConfig_sv == 0) {
     ATH_MSG_DEBUG("First call to TrigNtConfTool for the ConfSvc - need to do initial read of menu." );
     return true;
  }

  if ( m_configSvc->masterKey() != confg.getMasterKey() ) {
    ATH_MSG_DEBUG("Master key has changed from " << confg.getMasterKey() << " to " << m_configSvc->masterKey() << " - (re)fill " );
    return true;
  } else if ( (int)GetL1PSK() != (int)confg.getLV1PrescaleKey() ) {
    ATH_MSG_DEBUG("L1 key has changed from " << confg.getLV1PrescaleKey() << " to " << GetL1PSK() << " - (re)fill " );
    return true;
  } else  if ( m_configSvc->hltPrescaleKey() != confg.getHLTPrescaleKey() ) {
    ATH_MSG_DEBUG("HLT key has changed from " << confg.getHLTPrescaleKey() << " to " << m_configSvc->hltPrescaleKey() << " - (re)fill " );
    return true;
  }
  ATH_MSG_DEBUG("No kyes have changed from ConfigSvc - no need to fetch config again " );
  return false;

}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::ReadFromSv(TrigMonConfig &confg)
{
  //
  // Fill trigger configuration from config service
  //
  GetL1PSK(true); // Update cached value for L1 PSK

  // Do we need to update?
  if (GetKeysChangedFromSv(confg) == false) {
    return false; // Nothing to do
  }

  if(!m_configSvc) { 
    ATH_MSG_ERROR("Invalid TrigConfigSvc handle" );
    return false;
  }
  else {
    ATH_MSG_DEBUG("Filling TrigMonConfig using: " << m_configSvc );
  }

  const TrigConf::CTPConfig *ctp_confg = m_configSvc->ctpConfig();
  if(!ctp_confg) {
    ATH_MSG_ERROR("Failed to get CTPConfig or Menu" );
    return false;
  }

  const TrigConf::HLTSequenceList *seq_confg = m_configSvc->sequenceList();
  if(!seq_confg) {
    ATH_MSG_ERROR("Failed to get HLTSequenceList" );
    return false;
  }

  const TrigConf::HLTChainList *chn_confg = m_configSvc->chainList();
  if(!chn_confg) {
    ATH_MSG_ERROR("Failed to get HLTChainList" );
    return false;
  }

  ATH_MSG_INFO("Exporting a new TrigConf from ConfigSvc for : SMK,L1,HLT=" << m_configSvc->masterKey() << "," << GetL1PSK() << "," << m_configSvc->hltPrescaleKey() );

  std::stringstream ss1, ss2, ss3;
  ss1 << m_configSvc->masterKey();
  ss1 >> m_triggerMenuSetup;
  ss2 << GetL1PSK();
  ss2 >> m_L1PrescaleSet;
  ss3 << m_configSvc->hltPrescaleKey();
  ss3 >> m_HLTPrescaleSet;

  //
  // Just update HLT prescales
  //
  if(m_countConfig_sv != 0 && m_configSvc->masterKey() == confg.getMasterKey()) {
    Trig::FillConf conf;

    if(m_configSvc->hltPrescaleKey()  != confg.getHLTPrescaleKey()) {
      conf.UpdateHLT(confg, *chn_confg);
    }
    if((unsigned)GetL1PSK() != confg.getLV1PrescaleKey()) {
      conf.UpdateLV1(confg, *ctp_confg);
    }

    ATH_MSG_DEBUG("SMK has not changed: " << confg.getMasterKey() << ", just reading in updates prescales." );
    
    confg.setTriggerKeys(m_configSvc->masterKey(), 
       GetL1PSK(), 
       m_configSvc->hltPrescaleKey());

    conf.FillVar(confg, m_triggerMenuSetup, m_L1PrescaleSet, m_HLTPrescaleSet);

    if(m_printConfig) {
      ATH_MSG_INFO("Print TrigMonConfig filled from TrigConfigSvc" );
      Trig::Print(confg, std::cout);
    }

    return true;
  }

  //
  // Clear only trigger configuration from TrigMonConfig
  //
  confg.getVec<TrigConfChain>().clear();
  confg.getVec<TrigConfSeq>().clear();

  //
  // Fill L1 and HLT config: ORDER MATTERS!
  // NOTE! The stream prescales and names are not correctly set here (and never were)
  // FillStreams() needs to be added here and modified for this to work correctly.
  //
  Trig::FillConf conf;

  conf.FillLV1(confg, *ctp_confg);
  conf.FillSeq(confg, *seq_confg);
  conf.FillHLT(confg, *chn_confg);
  conf.FillVar(confg, m_triggerMenuSetup, m_L1PrescaleSet, m_HLTPrescaleSet);

  // Fill config information from the CTP, starting with the BG info
  const std::vector<TrigConf::BunchGroup> bunchGroups = ctp_confg->bunchGroupSet().bunchGroups();
  for (unsigned bg = 0; bg < bunchGroups.size(); ++bg) {
    ATH_MSG_DEBUG(" TrigConf::CTPConfig BunchGroup " << bunchGroups.at(bg).name() << " has size " << bunchGroups.at(bg).bunches().size() );
    std::stringstream ssNameKey, ssSizeKey, ssSizeVal;
    ssNameKey << "CTPConfig:NAME:BGRP" << bg;
    ssSizeKey << "CTPConfig:SIZE:BGRP" << bg;
    ssSizeVal << bunchGroups.at(bg).bunches().size();
    confg.addValue(ssNameKey.str(), bunchGroups.at(bg).name());
    confg.addValue(ssSizeKey.str(), ssSizeVal.str());
  }
  std::stringstream bgSet, ctpVersion, l1Version;
  bgSet << ctp_confg->bunchGroupSetId();
  ctpVersion << ctp_confg->ctpVersion();
  l1Version << ctp_confg->l1Version();
  confg.addValue("BunchGroupSet", bgSet.str() );
  confg.addValue("CTPVersion", ctpVersion.str() );
  confg.addValue("LV1Version", l1Version.str() );  

  if(!conf.error().empty()) {
    ATH_MSG_INFO("FillConf error stream:" );
    ATH_MSG_INFO("-----------------------------------------------------------" );
    ATH_MSG_INFO(conf.error() );
    ATH_MSG_INFO("-----------------------------------------------------------" );
  }

  if(!conf.debug().empty()) {
    ATH_MSG_DEBUG("FillConf debug stream:");
    ATH_MSG_DEBUG("-----------------------------------------------------------");
    ATH_MSG_DEBUG(conf.debug() );
    ATH_MSG_DEBUG("-----------------------------------------------------------" );
  }


  confg.setTriggerKeys(m_configSvc->masterKey(), 
           GetL1PSK(), 
           m_configSvc->hltPrescaleKey());
  
  confg.addValue("SOURCE", "CONFIG_SVC");

  if(m_printConfig) {
    ATH_MSG_INFO("Print TrigMonConfig filled from TrigConfigSvc" );
    Trig::Print(confg, std::cout);
  }

  m_countConfig_sv++;
  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::GetKeysChangedFromDB(unsigned run, unsigned lumi) {

  //
  // Read configuration keys from COOL
  // 

  if(m_run != run) {    
    if(ReadKeysDB(run)) {
      ATH_MSG_INFO("Read keys for new run: " << run );      
    } else {
      ATH_MSG_WARNING("No keys for new run: " << run );
      return false;
    }
  }
  
  const std::map<unsigned, ConfigKeys>::const_iterator kit = m_keys.find(lumi);

  if(kit == m_keys.end()) {
    ATH_MSG_INFO("No COOL trigger keys for run, lumi: " << run << ", " << lumi );
    return false;
  }

  const Trig::ConfigKeys &key = kit->second;

  if(!key.isValid()) {
    ATH_MSG_WARNING("Invalid COOL keys for run, lumi: " << run << ", " << lumi );
    return false;
  }

  if(key == m_currentKey) {
    ATH_MSG_DEBUG("No DB configuration change for current lumi block: " << lumi );
    return false;
  }

  ATH_MSG_DEBUG("There are DB changes for the LB - refill " << lumi );
  m_currentKey = key;
  return true;
}


//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::ReadFromDB(TrigMonConfig &confg, unsigned run, unsigned lumi)
{
  
  // Do we need to update? (note that if we do, this also sets m_currentKey)
  if (GetKeysChangedFromDB(run, lumi) == false) {
    return false; // Nothing to do
  }

  confg.setTriggerKeys(m_currentKey.getSMK(), m_currentKey.getLV1_PS(), m_currentKey.getHLT_PS());

  std::stringstream ss1, ss2, ss3;
  ss1 << m_currentKey.getSMK();
  ss1 >> m_triggerMenuSetup;
  ss2 << m_currentKey.getLV1_PS();
  ss2 >> m_L1PrescaleSet;
  ss3 << m_currentKey.getHLT_PS();
  ss3 >> m_HLTPrescaleSet;

  std::stringstream keyStr;
  m_currentKey.print(keyStr);

  //
  // Lookup cached configuration: already retrieved from trigger DB
  //
  ConfigMap::iterator iconf = m_configMap.find(m_currentKey);
  if(iconf != m_configMap.end()) {

    confg.getVec<TrigConfChain>().clear();
    confg.getVec<TrigConfSeq>().clear();
    
    confg.getVec<TrigConfChain>() = iconf->second.getVec<TrigConfChain>();
    confg.getVec<TrigConfSeq>()   = iconf->second.getVec<TrigConfSeq>();

    ATH_MSG_DEBUG("Using earlier cached result: " << keyStr.str() );
    return true;
  } 

  ATH_MSG_INFO("Reading new trigger configuration from DB: " << m_connectionTrig );
  ATH_MSG_INFO("  run    = " << run             );
  ATH_MSG_INFO("  lumi   = " << lumi            );
  ATH_MSG_INFO("  SMK    = " << m_currentKey.getSMK()    );
  ATH_MSG_INFO("  BGK    = " << m_currentKey.getBGK()    );
  ATH_MSG_INFO("  LV1 PS = " << m_currentKey.getLV1_PS() );
  ATH_MSG_INFO("  HLT PS = " << m_currentKey.getHLT_PS() );

  //
  // Read configuration from trigger database
  //
  TrigConf::CTPConfig    ctp_conf;
  
  ATH_MSG_DEBUG("Opening connection to TriggerDB..." );
  if(!m_storage) {
    m_storage = new TrigConf::StorageMgr(m_connectionTrig, m_username, m_password);
  }

  //
  // Load L1 configuration
  //
  ATH_MSG_INFO("Retrieving Lvl1 CTP configuration from TriggerDB..." );

  ctp_conf.setSuperMasterTableId(m_currentKey.getSMK());
  ctp_conf.setPrescaleSetId(m_currentKey.getLV1_PS());
  ctp_conf.setBunchGroupSetId(m_currentKey.getBGK());

  m_storage->masterTableLoader().load(ctp_conf); 

  ATH_MSG_INFO("Got " << ctp_conf.menu().items().size() << " L1 Items to fill" );

  //
  // Load HLT configuration
  //
  ATH_MSG_INFO("Loading HLT menu configuration from TriggerDB..." );

  //TrigConf::HLTFrame *hltFrame = TrigConf::HLTFrame::instance();
  
  if(!m_hltFrame) {
    m_hltFrame = new TrigConf::HLTFrame();
  }

  m_hltFrame->setSMK(m_currentKey.getSMK());
  m_hltFrame->thePrescaleSetCollection().clear(); // TimM - need to clear here when calling this multiple times in a run.
  m_hltFrame->thePrescaleSetCollection().set_prescale_key_to_load(m_currentKey.getHLT_PS());

  m_storage->hltFrameLoader().load(*m_hltFrame);

  ATH_MSG_INFO("Filling TrigMonConfig from DB..." );

  //
  // Clear TrigMonConfig
  //
  confg.getVec<TrigConfChain>().clear();
  confg.getVec<TrigConfSeq>().clear();

  //
  // Fill L1 and HLT config: ORDER MATTERS!
  // NOTE! The stream prescales and names are not correctly set here (and never were)
  // FillStreams() needs to be added here for this to work correctly.
  //
  Trig::FillConf conf;
  
  conf.FillLV1(confg, ctp_conf);
  conf.FillSeq(confg, m_hltFrame->getHLTSequenceList());
  conf.FillHLT(confg, m_hltFrame->getHLTChainList());
  conf.FillVar(confg, m_triggerMenuSetup, m_L1PrescaleSet, m_HLTPrescaleSet);
  

  if(!conf.error().empty()) {
    ATH_MSG_INFO("FillConf error stream:" );
    ATH_MSG_INFO("-----------------------------------------------------------" );
    ATH_MSG_INFO(conf.error() );
    ATH_MSG_INFO("-----------------------------------------------------------" );
  }
  if(!conf.debug().empty()) {
    ATH_MSG_VERBOSE("FillConf debug stream:" ); 
    ATH_MSG_VERBOSE("-----------------------------------------------------------" );
    ATH_MSG_VERBOSE(conf.debug() );
    ATH_MSG_VERBOSE("-----------------------------------------------------------" );
  }


  if(m_printConfig) {
    ATH_MSG_INFO("Print TrigMonConfig filled from DB" );
    Trig::Print(confg, std::cout);
  }

  confg.addValue("SOURCE", "TRIGGER_DB");
  m_countConfig_db++;

  //
  // Cache this configuration for future use
  //
  if(!m_configMap.insert(ConfigMap::value_type(m_currentKey, confg)).second) {
    ATH_MSG_WARNING("Failed to cache current configuration: " << keyStr.str() );
  }

  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::ReadKeysDB(unsigned run)
{
  //
  // Read configuration keys from COOL
  // 
  if(run < 100000) {
    ATH_MSG_WARNING("ReadKeysDB - skip invalid run: " << run );
    return false;
  }
  
  Trig::ReadConf read;

  if (run < 239708) { // Run 1
    m_connectionCool = "COOLONL_TRIGGER/COMP200";
  }
  
  read.setConnectionCOOL(m_connectionCool);
  read.updateConfigKeys(run);

  m_run  = run;
  m_keys = read.getAllKeys();
  
  return true;
}
