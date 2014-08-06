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
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"

// LV1 and HLT configuration
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PrescaleSet.h"

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
  :AlgTool(name, type, parent),
   m_log(0),
   m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_storage(0),
   m_hltFrame(0),
   m_config_db(0),
   m_config_sv(0),
   m_countConfig_db(0),
   m_countConfig_sv(0),
   m_run(0),
   m_dbOrConfSvcPass(0)
{
  declareInterface<Trig::ITrigNtTool>(this);

  declareProperty("configSvc",        m_configSvc);
  declareProperty("printConfig",      m_printConfig       = false);
  declareProperty("useDB",            m_useDB             = true);
  declareProperty("useConfSvc",       m_useConfSvc        = true);

  declareProperty("connectionCool",   m_connectionCool    = "COOLONL_TRIGGER/COMP200");
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
  m_log = new MsgStream(msgSvc(), name());
  
  if(m_useConfSvc) {
    if(m_configSvc.retrieve().isFailure()) {
      log() << MSG::ERROR << "Could not retrieve TrigConfigSvc!" << endreq;
      return StatusCode::FAILURE;
    }
    else {
      log() << MSG::INFO << "Retrieved TrigConfigSvc " << m_configSvc << endreq;
    }
  }

  log() << MSG::INFO 
	<< "printConfig  = " << m_printConfig  << endreq
	<< "useDB        = " << m_useDB        << endreq
  << "useConfSvc   = " << m_useConfSvc   << endreq;

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------------------
StatusCode Trig::TrigNtConfTool::finalize()
{
  //
  // Clean up
  //
  log() << MSG::DEBUG << "finalize()" << endreq; 

  delete m_log; m_log = 0;
  
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
    log() << MSG::WARNING << "Null TrigMonConfig pointer" << endreq;
    return false;
  }

  if(outputLevel() <= MSG::DEBUG) {
    log() << MSG::DEBUG << "Filling Trigger Configuration using Option: " << m_dbOrConfSvcPass << ". (1=ConfSvc, 2=DB)." << endreq;
  }

  // Require each call of "Fill" to act on only one of DB or ConfSvc
  if(m_dbOrConfSvcPass < 1 && m_dbOrConfSvcPass > 2) {
    log() << MSG::WARNING << "Set one of SetOption(1) for ConfSvc OR SetOption(2) for DB to true before calling Fill" << endreq;
    return false;
  }

  if (m_dbOrConfSvcPass == 2) { //2 for DB
    m_dbOrConfSvcPass = 0;

    if(m_useDB == false) {
      log() << MSG::WARNING << "Trying to run a Database pass (SetOption(2)), but m_useDB is false. Aborting"  << endreq;
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
      log() << MSG::WARNING << "Trying to run a Conf Service pass (SetOption(1)), but m_useConfSvc is false. Aborting"  << endreq;
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

//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::ReadFromSv(TrigMonConfig &confg)
{
  //
  // Fill trigger configuration from config service
  //

  if(!m_configSvc) { 
    log() << MSG::WARNING << "Invalid TrigConfigSvc handle" << endreq;
    return false;
  }
  else {
    log() << MSG::INFO << "Filling TrigMonConfig using: " << m_configSvc << endreq;
  }

  const TrigConf::CTPConfig *ctp_confg = m_configSvc->ctpConfig();
  if(!ctp_confg) {
    log() << MSG::WARNING << "Failed to get CTPConfig or Menu" << endreq;
    return false;
  }

  const TrigConf::HLTSequenceList *seq_confg = m_configSvc->sequenceList();
  if(!seq_confg) {
    log() << MSG::WARNING << "Failed to get HLTSequenceList" << endreq;
    return false;
  }

  const TrigConf::HLTChainList *chn_confg = m_configSvc->chainList();
  if(!chn_confg) {
    log() << MSG::WARNING << "Failed to get HLTChainList" << endreq;
    return false;
  }

  log() << MSG::INFO << "Filling Keyset : " << m_configSvc->masterKey() << ","
    << m_configSvc->lvl1PrescaleKey() << "," << m_configSvc->hltPrescaleKey() << endreq;

  //
  // Just update HLT prescales
  //
  if(m_countConfig_sv != 0 && m_configSvc->masterKey() == confg.getMasterKey()) {
    Trig::FillConf conf;

    if(m_configSvc->hltPrescaleKey()  != confg.getHLTPrescaleKey()) {
      conf.UpdateHLT(confg, *chn_confg);
    }
    if(m_configSvc->lvl1PrescaleKey() != confg.getLV1PrescaleKey()) {
      conf.UpdateLV1(confg, *ctp_confg);
    }

    if(outputLevel() <= MSG::DEBUG) {
      log() << MSG::DEBUG << "SMK has not changed: " << confg.getMasterKey() << ", just reading in updates prescales." << endreq;
    }
    
    confg.setTriggerKeys(m_configSvc->masterKey(), 
			 m_configSvc->lvl1PrescaleKey(), 
			 m_configSvc->hltPrescaleKey());

    if(m_printConfig) {
      log() << MSG::INFO << "Print TrigMonConfig filled from TrigConfigSvc" << endreq;
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

  if(!conf.error().empty()) {
    log() << MSG::INFO  << "FillConf error stream:" << endreq 
	  << "-----------------------------------------------------------" << endreq
	  << conf.error() 
	  << "-----------------------------------------------------------" << endreq;
  }

  if(!conf.debug().empty()) {
    log() << MSG::VERBOSE << "FillConf debug stream:" << endreq 
	  << "-----------------------------------------------------------" << endreq
	  << conf.debug() 
	  << "-----------------------------------------------------------" << endreq;
  }

  confg.setTriggerKeys(m_configSvc->masterKey(), 
		       m_configSvc->lvl1PrescaleKey(), 
		       m_configSvc->hltPrescaleKey());
  
  confg.addValue("SOURCE", "CONFIG_SVC");

  if(m_printConfig) {
    log() << MSG::INFO << "Print TrigMonConfig filled from TrigConfigSvc" << endreq;
    Trig::Print(confg, std::cout);
  }

  m_countConfig_sv++;
  return true;
}

//---------------------------------------------------------------------------------------
bool Trig::TrigNtConfTool::ReadFromDB(TrigMonConfig &confg, unsigned run, unsigned lumi)
{
  //
  // Read configuration keys from COOL
  // 

  if(m_run != run) {    
    if(ReadKeysDB(run)) {
      log() << MSG::INFO << "Read keys for new run: " << run << endreq;      
    }
    else {
      log() << MSG::WARNING << "No keys for new run: " << run << endreq;
      return false;
    }
  }
  
  const std::map<unsigned, ConfigKeys>::const_iterator kit = m_keys.find(lumi);

  if(kit == m_keys.end()) {
    log() << MSG::INFO << "No COOL trigger keys for run, lumi: " << run << ", " << lumi << endreq;
    return false;
  }

  const Trig::ConfigKeys &key = kit->second;

  if(!key.isValid()) {
    log() << MSG::WARNING << "Invalid COOL keys for run, lumi: " << run << ", " << lumi << endreq;
    return false;
  }

  if(key == m_currentKey) {
    log() << MSG::DEBUG << "No configuration change for current lumi block: " << lumi << endreq;
    return false;
  }

  m_currentKey = key;
  confg.setTriggerKeys(key.getSMK(), key.getLV1_PS(), key.getHLT_PS());

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

    log() << MSG::DEBUG << "Using earlier cached result: " << keyStr.str() << endreq;
    return true;
  } 

  log() << MSG::INFO
	<< "Reading new trigger configuration from DB: " << m_connectionTrig << endreq
	<< "  run    = " << run             << endreq
	<< "  lumi   = " << lumi            << endreq
	<< "  SMK    = " << key.getSMK()    << endreq
	<< "  BGK    = " << key.getBGK()    << endreq
	<< "  LV1 PS = " << key.getLV1_PS() << endreq
	<< "  HLT PS = " << key.getHLT_PS() << endreq;

  //
  // Read configuration from trigger database
  //
  TrigConf::CTPConfig    ctp_conf;
  
  log() << MSG::DEBUG << "Opening connection to TriggerDB..." << endreq;
  if(!m_storage) {
    m_storage = new TrigConf::StorageMgr(m_connectionTrig,
					 m_username,
					 m_password);
  }

  //
  // Load L1 configuration
  //
  log() << MSG::INFO << "Retrieving Lvl1 CTP configuration from TriggerDB..." << endreq;

  ctp_conf.setSuperMasterTableId(key.getSMK());
  ctp_conf.setPrescaleSetId(key.getLV1_PS());
  ctp_conf.setBunchGroupSetId(key.getBGK());

  m_storage->masterTableLoader().load(ctp_conf);    

  //
  // Load HLT configuration
  //
  log() << MSG::INFO << "Loading HLT menu configuration from TriggerDB..." << endreq;

  //TrigConf::HLTFrame *hltFrame = TrigConf::HLTFrame::instance();
  
  if(!m_hltFrame) {
    m_hltFrame = new TrigConf::HLTFrame();
  }

  m_hltFrame->setSMK(key.getSMK());
  m_hltFrame->thePrescaleSetCollection().clear(); // TimM - need to clear here when calling this multiple times in a run.
  m_hltFrame->thePrescaleSetCollection().set_prescale_key_to_load(key.getHLT_PS());

  m_storage->hltFrameLoader().load(*m_hltFrame);

  log() << MSG::INFO << "Filling TrigMonConfig from DB..." << endreq;

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
    log() << MSG::INFO  << "FillConf error stream:" << endreq 
	  << "-----------------------------------------------------------" << endreq
	  << conf.error() 
	  << "-----------------------------------------------------------" << endreq;
  }
  if(!conf.debug().empty()) {
    log() << MSG::VERBOSE << "FillConf debug stream:" << endreq 
	  << "-----------------------------------------------------------" << endreq
	  << conf.debug() 
	  << "-----------------------------------------------------------" << endreq;
  }

  if(m_printConfig) {
    log() << MSG::INFO << "Print TrigMonConfig filled from DB" << endreq;
    Trig::Print(confg, std::cout);
  }

  confg.addValue("SOURCE", "TRIGGER_DB");
  m_countConfig_db++;

  //
  // Cache this configuration for future use
  //
  if(!m_configMap.insert(ConfigMap::value_type(m_currentKey, confg)).second) {
    log() << MSG::WARNING << "Failed to cache current configuration: " << keyStr.str() << endreq;
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
    log() << MSG::WARNING << "ReadKeysDB - skip invalid run: " << run << endreq;
    return false;
  }
  
  Trig::ReadConf read;
  
  read.setConnectionCOOL(m_connectionCool);
  read.updateConfigKeys(run);

  m_run  = run;
  m_keys = read.getAllKeys();
  
  return true;
}
