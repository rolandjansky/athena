/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilHack.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostBase/WriteTrigConfig.h"
#include "TrigCostBase/ConfigAlg.h"

REGISTER_ANP_OBJECT(AlgEvent,ConfigAlg)

using namespace std;

//-----------------------------------------------------------------------------
Anp::ConfigAlg::ConfigAlg()
  :fIsConfig(false),
   fIsFrozen(false),
   fConfigCount(0)
{
}

//-----------------------------------------------------------------------------
Anp::ConfigAlg::~ConfigAlg()
{
}

//-----------------------------------------------------------------------------
void Anp::ConfigAlg::Config(const Registry &reg)
{
  //
  // Read configuration and setup ntuple branches
  //
  reg.Get("ConfigAlg", "Debug",   fDebug);
  
  //
  // This is needed by StudyTime for ROBID<-> SUBDET map is read from ntuple
  //
  fConfg = NtupleSvc::Instance("config").Branch<TrigMonConfig>("conf");
  fEntry = NtupleSvc::Instance("event") .Branch<TrigMonEvent> ("data");
  
  //
  // Add self to global list - used by clients: MakeRates
  //
  Factory<ConfigABC>::Instance().Hold(AlgEvent::GetName(), Handle<ConfigABC>(this));

  //
  // Configure base class
  //
  ConfigABC::ReadConfig(reg);
}

//-----------------------------------------------------------------------------
bool Anp::ConfigAlg::Init()
{  
  //
  // Print summary of callback requests
  //
  log() << "Init - registered " << fCallback.size() << " callback(s)" << endl;

  for(unsigned i = 0; i < fCallback.size(); ++i) {
    cout << "   callback algorithm: " << fCallback.at(i)->GetName() << endl;
  }

  fIsFrozen = true; // no more new callback algorithms

  //
  // Read configuration from init file and release pointer
  //
  if(fConfg.valid() && fConfg.GetState() == Ntuple::kREAD) { 
    //
    // Only process first TrigMonConfig ntuple entry
    //
    if(!fIsConfig) {
      Set(fConfg.GetRef());
    }
  }
  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::ConfigAlg::Exec()
{
  //
  // Process one event from ntuple
  //
  if(fConfg.valid() && fConfg.GetState() == Ntuple::kREAD) {
    if(!fIsConfig) {
      Set(fConfg.GetRef());
    }
  }

  //
  // Make new event entry
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::ConfigAlg::Done()
{
  //
  // Print configuration summary
  //
  log() << "Done - config count: " << fConfigCount << endl;
}

//-----------------------------------------------------------------------------
void Anp::ConfigAlg::RegisterCallback(Callback *alg)
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
const TrigMonConfig& Anp::ConfigAlg::GetConfig(const std::string &key) const
{
  //
  // Find requested configuration
  //
  ConfigMap::const_iterator cit = fConfig.find(key);
  if(cit == fConfig.end()) {
    log() << "GetConfig - unknown key: " << key << endl;
    return fDefault;
  }
  
  return cit->second;
}

//-----------------------------------------------------------------------------
TrigMonConfig& Anp::ConfigAlg::GetConfigModifiable(const std::string modifyingAlgName,
						   const std::string &key)				   
{
  //
  // Find requested configuration
  //  
  log() << "Configuration is being modified by " << modifyingAlgName << endl;

  ConfigMap::iterator cit = fConfig.find(key);
  if(cit == fConfig.end()) {
    log() << "GetConfig - unknown key: " << key << endl;
    return fDefault;
  }
  
  return cit->second;
}

//-----------------------------------------------------------------------------
void Anp::ConfigAlg::Run(const TrigMonEvent &)
{
  //
  // No action for different events 
  //   - configuration is always read from 1st entry of "config" TTree
  //   - time dependent configuration is read by ConfigDB from COOL/TriggerDB
  //
}

//-----------------------------------------------------------------------------
void Anp::ConfigAlg::Set(const TrigMonConfig &conf)
{
  //
  // Make copy of configuration object
  //
  TrigMonConfig config = conf;

  //
  // Set defaults, update PS/PT/express PS and read group definitions
  // 
  ConfigABC::UpdateConfig(config);

  //
  // Save configuration
  //
  fIsConfig = true;
  fConfigCount++;
  fConfig["CURRENT"] = config;

  if(fConfigCount == 1) fDefault = config;

  //
  // Write out XML configuration
  //  


  //
  // Inform clients about new configuration
  //
  Registry reg;
  reg.Set("TRIG_CONFIG","INIT");

  for(unsigned i = 0; i < fCallback.size(); ++i) {
    fCallback.at(i)->Signal(reg, "TRIG_CONFIG");
  }
}
