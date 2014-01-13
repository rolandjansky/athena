/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <exception>

// LV1 and HLT configuration
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PrescaleSet.h"

#include "TrigConfJobOptData/JobOption.h"
#include "TrigConfJobOptData/JobOptionTable.h"

// Loader classes
#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfStorage/IHLTFrameLoader.h"
#include "TrigConfStorage/IHLTPrescaleSetLoader.h"

// Trigger
#include "TrigCostMonitor/FillConf.h"
#include "TrigCostMonitor/ReadConf.h"
#include "TrigCostMonitor/UtilCost.h"

// PhysicsNtuple
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostRate/ConfigDB.h"

REGISTER_ANP_OBJECT(AlgEvent,ConfigDB)

using namespace std;

//-----------------------------------------------------------------------------
Anp::ConfigDB::ConfigDB()
  :fDebug(false),
   fUseKeys(false),
   fBGK(0),
   fSMK(0),
   fLV1_PS(0),
   fHLT_PS(0),
   fConfigRun(0),
   fConfigLB(0),
   fCoolId("COOLONL_TRIGGER/COMP200"),
   fTrigId("TRIGGERDB"),
   fUsername(),
   fPassword(),
   fIsConfig(false),
   fIsFrozen(false),
   fValidConf(false),
   fConfigCount(0),
   fRun(0),
   fLumi(0),
   fRunList(0)
{
}

//-----------------------------------------------------------------------------
Anp::ConfigDB::~ConfigDB()
{
}

//-----------------------------------------------------------------------------
void Anp::ConfigDB::Config(const Registry &reg)
{
  //
  // Read configuration and setup ntuple branches
  //
  reg.Get("ConfigDB", "Debug",      fDebug);
  reg.Get("ConfigDB", "COOLTRIGID", fCoolId);
  reg.Get("ConfigDB::UseKeys",      fUseKeys);
  reg.Get("ConfigDB::COOLID",       fCoolId);
  reg.Get("ConfigDB::SMK",          fSMK);
  reg.Get("ConfigDB::BGK",          fBGK);
  reg.Get("ConfigDB::LV1_PS",       fLV1_PS);
  reg.Get("ConfigDB::HLT_PS",       fHLT_PS);
  reg.Get("ConfigDB::ConfigRun",    fConfigRun);
  reg.Get("ConfigDB::ConfigLB",     fConfigLB);

  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
  
  //
  // Add self to global list - used by clients: MakeRates, PredRates, etc
  //
  Factory<ConfigABC>::Instance().Hold(AlgEvent::GetName(), Handle<ConfigABC>(this));

  //
  // Configure base class
  //
  ConfigABC::ReadConfig(reg);
}

//-----------------------------------------------------------------------------
bool Anp::ConfigDB::Init()
{  
  //
  // Read configuration once if keys are set
  //
  fIsFrozen = true; // no more new callback algorithms
  
  log() << "Init - registered " << fCallback.size() << " callback(s)" << endl;

  for(unsigned i = 0; i < fCallback.size(); ++i) {
    cout << "   callback algorithm: " << fCallback.at(i)->GetName() << endl;
  }

  if(fUseKeys) {
    fKey.setSMK(fSMK);
    fKey.setBGK(fBGK);
    fKey.setLV1_PS(fLV1_PS);
    fKey.setHLT_PS(fHLT_PS);
    
    log() << "Init - reading configuration using fixed keys:\n   " << endl;
    fKey.print(std::cout);

    fData.clear();
    Load(fData.config, fData.bgmap, fKey);

    //
    // Read list of L1 thresholds per L1 item
    //
    ConfigABC::ReadItemTh(fData.config);

    //
    // Inform clients about new configuration
    //
    Registry sreg;
    sreg.Set("TRIG_CONFIG_SOURCE", AlgEvent::GetName());
    
    for(unsigned i = 0; i < fCallback.size(); ++i) {
      fCallback.at(i)->Signal(sreg, "TRIG_CONFIG_DB");
    }

    //
    // Release handle to TrigMonEvent - configuration is already read
    //
    fEntry.release();
  }
  else if(fConfigRun > 0 && fConfigLB > 0) {
    log() << "Init - reading configuration using predefined run and LB:" << endl
	  << "   ConfigRun = " << fConfigRun << endl
	  << "   ConfigLB  = " << fConfigLB  << endl;

    Run(fConfigRun, fConfigLB);

    //
    // Release handle to TrigMonEvent - configuration is already read
    //
    fEntry.release();
  }

  return true;
}

//-----------------------------------------------------------------------------
void Anp::ConfigDB::Exec()
{
  //
  // Read event entry
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {

    if(fDebug) {
      log() << "Exec - reading TrigMonEvent..." << endl;
    }

    Run(fEntry.GetRef().getRun(), fEntry.GetRef().getLumi());
  }
}

//-----------------------------------------------------------------------------
void Anp::ConfigDB::Done()
{
  //
  // Print configuration summary
  //
  log() << "Done - config count: " << fConfigCount << endl;
}

//-----------------------------------------------------------------------------
void Anp::ConfigDB::RegisterCallback(Callback *alg)
{
  //
  // Register algorithm for trigger configuration callbacks
  //
  if(fIsFrozen) {
    log() << "RegisterCallback - callbacks are not allowed after configuration step" << endl;
    return;
  }

  if(alg) {
    fCallback.push_back(alg);
  }
}

//-----------------------------------------------------------------------------
const TrigMonConfig& Anp::ConfigDB::GetConfig(const std::string &) const
{
  //
  // Find requested configuration
  //  
  return fData.config;
}

//-----------------------------------------------------------------------------
TrigMonConfig& Anp::ConfigDB::GetConfigModifiable(const std::string modifyingAlgName,
						  const std::string &key)
{
  //
  // Find requested configuration
  //  
  log() << "Configuration is being modified by " << modifyingAlgName << " with key=" << key<< endl;

  return fData.config;
}

//-----------------------------------------------------------------------------
std::map<std::string, Trig::BunchGroup> Anp::ConfigDB::GetBG() const
{
  //
  // Find requested configuration
  //  
  return fData.bgmap;
}

//-----------------------------------------------------------------------------
void Anp::ConfigDB::Run(unsigned run, unsigned lumi)
{
  //
  // Nothing to do if configuration keys are set
  //
  if(fUseKeys) {
    return;
  }

  //
  // Read configuration keys for new run
  //
  if(fRun == 0 || std::find(fRunList.begin(), fRunList.end(), run) == fRunList.end()) {
    Read(run);  
    fRunList.push_back(run); 
  }

  //
  // Check if keys have changed for this lumi block
  //
  if(fLumi == lumi) {
    return;
  }
    
  fLumi = lumi;
  KeyMap::const_iterator kit = fKeys.find(fLumi);

  //
  // Check that the SMK, L1 and HLT have all been read e.g. we have a valid configuration
  //   -- this can cause an error if COOL is written at the same time that the trigger configuration is read because on of the following can be missed
  //
  fValidConf=true;

  if(kit != fKeys.end()){
    if(!(kit->second.isValid())) fValidConf=false; 
  }

  if((kit == fKeys.end()) || !fValidConf) {
    log() << "Run - missing keys for lumi: " << fLumi << " - try to read again..." << endl;
    
    //
    // Try to read keys again - in case COOL has changed since last time
    //
    Read(run);

    kit = fKeys.find(fLumi);

    if(kit->second.isValid()) fValidConf=true; 

    if((kit == fKeys.end()) || !fValidConf) {
      log() << "Run - missing keys for lumi: " << fLumi << " - failed 2nd time." << endl;
      return;
    }
  }
  
  if(fKey.equalKeys(kit->second)) {
    return;
  }

  //
  // Process new configuration keys
  //
  fKey = kit->second;

  std::vector<unsigned> conf_key;
  conf_key.push_back(fKey.getBGK());
  conf_key.push_back(fKey.getSMK());
  conf_key.push_back(fKey.getLV1_PS());
  conf_key.push_back(fKey.getHLT_PS());

  ConfMap::iterator iconf = fConf.find(conf_key);
  if(iconf == fConf.end()) {
    //
    // Read new confguration set
    //
    fData.clear();
    fData.config.setTriggerKeys(fKey.getSMK(), fKey.getLV1_PS(), fKey.getHLT_PS());
    fData.config.setEventID(0, fLumi, fRun, 0, 0);

    Load(fData.config, fData.bgmap, fKey);
    
    //
    // Read list of L1 thresholds per L1 item
    //
    ConfigABC::ReadItemTh(fData.config);

    fConf[conf_key] = fData;
  }
  else {
    //
    // Copy previous result
    //
    fData = iconf->second;
  }  

  //
  // Inform clients about new configuration
  //
  Registry sreg;
  sreg.Set("TRIG_CONFIG_SOURCE", AlgEvent::GetName());

  for(unsigned i = 0; i < fCallback.size(); ++i) {
    fCallback.at(i)->Signal(sreg, "TRIG_CONFIG_DB");
  }
}

//-----------------------------------------------------------------------------
void Anp::ConfigDB::Read(unsigned run)
{
  //
  // Read trigger keys for one run
  //
  fConfigCount++;

  Trig::ReadConf conf;
  std::stringstream pstr;
  
  conf.setConnectionCOOL(fCoolId);
  conf.updateConfigKeys(run);
  conf.print(pstr);
  
  fRun  = run;
  fLumi = 0;    
  fKeys = conf.getAllKeys();

  log() << "Read - run #" << fRun << endl << pstr.str();  

  log() << "------------------------------------------------------" << endl;
  cout << conf.infos();
  log() << "------------------------------------------------------" << endl;

  if(fDebug) {
    log() << "------------------------------------------------------" << endl;
    cout << conf.debug();
    log() << "------------------------------------------------------" << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::ConfigDB::Load(TrigMonConfig &config, 
			 BGMap &bmap,
			 const Trig::ConfigKeys &key) const
{
  //
  // Process run, lumi with new configuration keys
  //
  log() << "Load - loading new configuration keys: " << endl
	<< "   run = " << config.getRun()
	<< " lumi = "  << setw(4) << config.getLumi()
	<< " BGK = "   << key.getBGK()
	<< " SMK  = "  << key.getSMK()
	<< " LV1PS = " << key.getLV1_PS()
	<< " HLTPS = " << key.getHLT_PS() << endl;

  config.setTriggerKeys(key.getSMK(), key.getLV1_PS(), key.getHLT_PS());

  //
  // Read configuration from trigger database
  //
  TrigConf::CTPConfig    ctp_conf;
  TrigConf::PrescaleSet  psk_conf;
  std::stringstream      log_conf;

  std::auto_ptr<TrigConf::IStorageMgr> sm = 
    std::auto_ptr<TrigConf::IStorageMgr>(new TrigConf::StorageMgr(fTrigId,
								  fUsername,
								  fPassword,
								  std::cout));

  //
  // Load L1 configuration
  //
  log() << "Load - retrieving Lvl1 CTP configuration from TriggerDB..." << endl;

  ctp_conf.setSuperMasterTableId(key.getSMK());
  ctp_conf.setPrescaleSetId(key.getLV1_PS());
  ctp_conf.setBunchGroupSetId(key.getBGK());

  sm->masterTableLoader().load(ctp_conf);    

  //
  // Load HLT configuration
  //
  log() << "Load - retrieving HLT menu configuration from TriggerDB..." << endl;

  TrigConf::HLTFrame *hltFrame = new TrigConf::HLTFrame();

  hltFrame->setSMK(key.getSMK());
  hltFrame->thePrescaleSetCollection().set_prescale_key_to_load(key.getHLT_PS());
  
  sm->hltFrameLoader().load(*hltFrame);
  
  // PrescaleSet is now loaded by the frameloader
  //   TrigConf::HLTPrescaleSet hltpss;
  //   hltpss.set_psk(key.getHLT_PS());

//   TrigConf::StorageMgr &s = dynamic_cast<TrigConf::StorageMgr &>(*sm);
//   s.hltPrescaleSetLoader().load(hltpss);

  // load the job properties
  TrigConf::JobOptionTable jot;
  log() << "Load - retrieving L2 JO from the TriggerDB ..." << endl;
  jot.setSuperMasterTableId(key.getSMK());
  jot.setTriggerLevel(0); // 0 = L2, 1 = EF
  sm->jobOptionTableLoader().load( jot );
  
  //
  // Fill L1 and HLT config: ORDER MATTERS!
  //
  Trig::FillConf conf;
  
  conf.FillLV1(config, ctp_conf);
  conf.FillSeq(config, hltFrame->getHLTSequenceList());
  conf.FillHLT(config, hltFrame->getHLTChainList());
  // conf.FillStreams(config, hltpss);
  conf.FillStreams(config, *hltFrame->thePrescaleSet() ); // Joerg
  // load the L1 items for the EB chains 
  conf.FillEBHypo(config, jot);
  
  delete hltFrame;

  if(!conf.error().empty()) {
    log() << "Load - FillConf error stream:" << endl 
	  << "-----------------------------------------------------------" << endl
	  << conf.error() 
	  << "-----------------------------------------------------------" << endl;
  }
  if(!conf.debug().empty() && fDebug) {
    log() << "Load - FillConf debug stream:" << endl 
	  << "-----------------------------------------------------------" << endl
	  << conf.debug() 
	  << "-----------------------------------------------------------" << endl;
  }

  if(fDebug) {
    log() << "Load - print TrigMonConfig filled from DB" << endl;
    Trig::Print(config, std::cout);
  }

  //
  // Set defaults, update PS/PT/express PS and read group definitions using XML
  // 
  ConfigABC::UpdateConfig(config);

  //
  // Print bunch group info
  //
  const TrigConf::BunchGroupSet &b = ctp_conf.bunchGroupSet();
  const std::vector<TrigConf::BunchGroup> &bgs = b.bunchGroups();

  // store the BCIDs and the name for this BG Key
  std::vector<string> bunchkeys;
  bunchkeys.push_back("BGRP0"); bunchkeys.push_back("BGRP1"); bunchkeys.push_back("BGRP2"); bunchkeys.push_back("BGRP3");
  bunchkeys.push_back("BGRP4"); bunchkeys.push_back("BGRP5"); bunchkeys.push_back("BGRP6"); bunchkeys.push_back("BGRP7");

  for(unsigned i = 0; i < bgs.size(); ++i) {
    const TrigConf::BunchGroup &bg = bgs.at(i);    
    Trig::BunchGroup bgdata;
    
    std::vector<int> bunches_conf =  bg.bunches();
    std::copy(bunches_conf.begin(), bunches_conf.end(), std::inserter(bgdata.bcids, bgdata.bcids.end()));

    bgdata.name   = bg.name();
    bgdata.key    = bunchkeys.at(bg.internalNumber());
    bgdata.keynum = bg.internalNumber();
    bgdata.size   = bgdata.bcids.size();
    
    bgdata.PrintBunchGroup();

    bmap[bgdata.key] = bgdata;

    if(fDebug) {
      bg.print("   ");
    }
  }
}
