/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>

// Local
#include "TrigCostBase/RunAlgs.h"
#include "TrigCostBase/Factory.h"

REGISTER_ANP_OBJECT(AlgEvent,RunAlgs)

using namespace std;

//-----------------------------------------------------------------------------
Anp::RunAlgs::RunAlgs()
  :fDebug(false)
{
}

//-----------------------------------------------------------------------------
Anp::RunAlgs::~RunAlgs()
{
}

//-----------------------------------------------------------------------------
void Anp::RunAlgs::Config(const Anp::Registry &reg)
{
  //
  // Initialize self and children
  //
      
  reg.Get("RunAlgs", "Debug", fDebug);
  
  //
  // Create and configure algorithms
  //
  vector<string> alist;
  unsigned awidth = 0;
  reg.Get("RunAlgs::AlgList", alist);

  log() << "Config - attempt to create " << alist.size() << " algorithm(s)..." << endl;
  for(unsigned i = 0; i < alist.size(); ++i) {
    //
    // algorithm name
    //
    const string &aname = alist.at(i);
    string atype = aname;
    
    //
    // Check for individual algorithm configuration
    //
    
    Registry nreg;
    if(!reg.Get(aname, nreg)) { 
      if(fDebug) {
	log() << "Config - failed to get Registry for: " << aname << endl;
      }
      
      reg.Get("AlgEventType", atype);
    }
    else {
      if(!nreg.Get("AlgType", atype)) {
	nreg.Get("AlgEventType", atype);	
      }
    }

    if(atype.empty()) continue;

    Handle<AlgEvent> alg = Factory<AlgEvent>::Instance().Create(atype);

    if(alg.valid()) {
      alg -> SetName(aname);
      alg -> SetType(atype);
      fAlg.push_back(alg);
    }
    else{
      cerr << "   error: failed to create: " << aname << "/" << atype << endl;
    }

    awidth = std::max<unsigned>(awidth, aname.size());
  }

  log() << "Config - created " << fAlg.size() << " algorithm(s):" << endl;
  for(unsigned i = 0; i < fAlg.size(); ++i) {
    cout << "   " << std::setw(awidth) << std::left << fAlg.at(i)->GetName() << " - "
	 << fAlg.at(i)->GetType() << endl;
  }

  //
  // Configure algorithms
  //
  for(unsigned i = 0; i < fAlg.size(); ++i) {
    Handle<AlgEvent> alg = fAlg.at(i);

    Registry nreg;
    if(reg.Get(alg->GetName(), nreg)) {
      alg -> Config(nreg);

      if(fDebug) {
	log() << "Config - configuring algorithm using key registry: " << alg->GetName() << endl;
      }
    }
    else {      
      alg -> Config(reg);

      if(fDebug) {
	log() << "Config - configuring algorithm using top registry" << endl;
      }
    }

    if(fDebug) {
      log() << "Config - finished configuring algorithm: " << alg->GetName() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::RunAlgs::Signal(const Anp::Registry &reg, const string &signal)
{
  //
  // Initialize algorithms
  //
  for(unsigned int i = 0; i < fAlg.size(); ++i) {
    Handle<AlgEvent> alg = fAlg.at(i);

    if(fDebug) {
      log() << "Signal - try algorithm: " << alg->GetName() << endl;
    }

    alg -> Signal(reg, signal);

    if(fDebug) {
      log() << "Signal - finished with algorithm: " << alg->GetName() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::RunAlgs::Save(TDirectory *dir)
{
  //
  // Initialize algorithms
  //
  for(unsigned int i = 0; i < fAlg.size(); ++i) {
    Handle<AlgEvent> alg = fAlg.at(i);

    if(fDebug) {
      log() << "Save - try algorithm: " << alg->GetName() << endl;
    }

    alg -> Save(dir);

    if(fDebug) {
      log() << "Save - finished with algorithm: " << alg->GetName() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
bool Anp::RunAlgs::Init()
{
  //
  // Initialize algorithms
  //

  AlgVec::iterator ialg = fAlg.begin();

  while(ialg != fAlg.end()) {    
    if(fDebug) {
      log() << "Init - initializing algorithm: " << (*ialg)->GetName() << endl;
    }

    const bool astat = (*ialg) -> Init();
    
    if(fDebug) {
      log() << "Init - finished initializing algorithm: " << (*ialg)->GetName() << endl;
    }

    if(astat) {
      ialg++;
    }
    else {
      log() << "Init - algorithm failed Init(), remove " << (*ialg)->GetName() << endl;
      ialg = fAlg.erase(ialg);
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
void Anp::RunAlgs::Exec()
{
  //
  // Find all ROOT files in directory matching all my keys
  //
  
  for(AlgVec::iterator ialg = fAlg.begin(); ialg != fAlg.end(); ++ialg) {
    AlgEvent *alg = (*ialg).get();
    const std::string &name = alg->GetName();

    alg -> PassEvent();

    try {
      alg -> Exec();
    }
    catch(std::exception &e) {
      log() << "Exec - caught std::exception in " << name << ":" << endl
	    << "   \"" << e.what() << "\"" << endl;
      exit(1);
    }
    catch(...) {
      log() << "Exec - caught unknown exception in " << name << endl;
      exit(1);
    }

    if(!(alg -> IsPassedEvent())) { 
      break; 
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::RunAlgs::Done()
{
  //
  // Call end for algorithms
  //
  for(AlgVec::iterator ialg = fAlg.begin(); ialg != fAlg.end(); ++ialg) {
    (*ialg) -> Done();
  }

  //
  // Erase algorithms (delete them if nobody else holds handle)
  //
  log() << "Done - releasing " << fAlg.size() << " algorithm(s)" << endl;

  for(AlgVec::iterator ialg = fAlg.begin(); ialg != fAlg.end(); ++ialg) {
    cout << "   erasing " << (*ialg)->GetName() << endl;
    (*ialg).release();
  }

  fAlg.clear(); 

  log() << "Done - finished." << endl;
}
