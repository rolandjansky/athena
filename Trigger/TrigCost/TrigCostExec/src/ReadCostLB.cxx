/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/Factory.h"

// Local
#include "TrigCostExec/ReadCostLB.h"

REGISTER_ANP_OBJECT(AlgEvent,ReadCostLB)

using namespace std;

//-----------------------------------------------------------------------------
Anp::ReadCostLB::ReadCostLB()
  :fDebug(false),
   fBegLB(0),
   fEndLB(0),
   fAcceptFile(false),
   fCountPass(0),
   fCountFail(0)
{
}

//-----------------------------------------------------------------------------
Anp::ReadCostLB::~ReadCostLB()
{
}

//-----------------------------------------------------------------------------
void Anp::ReadCostLB::Config(const Registry &reg)
{
  //
  // Read configuration and setup ntuple branches
  //
  reg.Get("ReadCostLB", "Debug",   fDebug);
  reg.Get("ReadCostLB::BegLB",     fBegLB);
  reg.Get("ReadCostLB::EndLB",     fEndLB);
  
  //
  // This is needed by StudyTime for ROBID<-> SUBDET map is read from ntuple
  //
  fConfg = NtupleSvc::Instance("config").Branch<TrigMonConfig>("conf");
  fEntry = NtupleSvc::Instance("event") .Branch<TrigMonEvent> ("data");
}

//-----------------------------------------------------------------------------
void Anp::ReadCostLB::Signal(const Registry &reg, const std::string &signal)
{
  //
  // Get current file name
  //
  if(signal == "NEW_FILE") {

    string file_path;
    if(reg.Get("SIGNAL::FilePath", file_path)) {
      fCurrFile = file_path;
      
      if(fDebug) {
	log() << "Signal - read FilePath=" << file_path << endl;
      }
    }
    else {
      log() << "Signal - missing FilePath" << endl;
    }
  }
}

//-----------------------------------------------------------------------------
bool Anp::ReadCostLB::Init()
{  
  //
  // Print summary of callback requests
  //  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::ReadCostLB::Exec()
{
  //
  // Process one event from ntuple
  //
  if(fConfg.valid() && fConfg.GetState() == Ntuple::kREAD) {
    Run(fConfg.GetRef());
  }

  //
  // Make new event entry
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {
    
    if(Run(fEntry.GetRef())) {
      ++fCountPass;
    }
    else {
      ++fCountFail;
      fEntry.SetEvent(Ntuple::kFAIL);
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::ReadCostLB::Done()
{
  //
  // Print configuration summary
  //
  const unsigned ntotal = fCountPass+fCountFail;
  
  if(ntotal > 0) {
    log() << "Done - " << endl
	  << "   total  events: " << ntotal     << endl
	  << "   passed events: " << fCountPass << endl
	  << "   failed events: " << fCountFail << endl;
  }
}


//-----------------------------------------------------------------------------
bool Anp::ReadCostLB::Run(const TrigMonEvent &event)
{
  //
  // Accept selected LBs
  //
  if(fBegLB == 0) {
    return true;
  }

  if(!fAcceptFile || !AcceptLB(event.getLumi())) {
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
void Anp::ReadCostLB::Run(const TrigMonConfig &config)
{
  //
  // Make copy of configuration object
  //
  if(fCurrFile.empty()) {
    log() << "Run - missing current file name" << endl;
    return;
  }

  if(std::find(fFiles.begin(), fFiles.end(), fCurrFile) != fFiles.end()) {
    return;
  }

  //
  // New file
  //
  fFiles.push_back(fCurrFile);
  fAcceptFile = false;

  const std::vector<string>   &vars = config.getVarName();
  const std::vector<uint32_t> &keys = config.getVarId();

  if(vars.size() == keys.size()) {
    for(unsigned i = 0; i < vars.size(); ++i) {
      if(vars.at(i) == "LB") {
	const unsigned lb = keys.at(i);
	
	if(fDebug) {
	  cout << "   " << fCurrFile << ": lb=" << lb << endl;
	}

	if(AcceptLB(lb)) {
	  fAcceptFile = true;
	}
      }
    }
  }  
}

//-----------------------------------------------------------------------------
bool Anp::ReadCostLB::AcceptLB(unsigned lb) const
{
  //
  //
  //
  if(fBegLB == 0) {
    return true;
  }

  if(fBegLB <= lb && lb <= fEndLB) {
    return true;
  }  

  return false;
}
