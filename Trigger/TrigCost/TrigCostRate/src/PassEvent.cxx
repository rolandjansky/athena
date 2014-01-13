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
#include "TrigCostBase/EventSvc.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostRate/PassEvent.h"

REGISTER_ANP_OBJECT(AlgEvent,PassEvent)

using namespace std;

//-----------------------------------------------------------------------------
Anp::PassEvent::PassEvent()
  :fDebug(false),
   fLogic("OR"),
   fMaxEvts(0),
   fLumiBeg(0),
   fLumiEnd(0),
   fLogicKey(0),
   fCountPass(0),
   fCountFail(0),
   fCountBuff(0),
   fDecisionBitLV1(TrigMonL1Item::kL1AfterVeto),
   fDecisionBitHLT(TrigMonChain::kPassed)
{
}

//-----------------------------------------------------------------------------
Anp::PassEvent::~PassEvent()
{
}

//-----------------------------------------------------------------------------
void Anp::PassEvent::Config(const Registry &reg)
{
  //
  // Configure self and register input data
  //
  reg.Get("PassEvent", "Debug", fDebug);
  reg.Get("PassEvent::Logic",   fLogic);
  reg.Get("PassEvent::Accept",  fAccept);
  reg.Get("PassEvent::Veto",    fVeto);

  reg.Get("PassEvent::LBLimit", fMaxEvts);
  reg.Get("PassEvent::LumiBeg", fLumiBeg);
  reg.Get("PassEvent::LumiEnd", fLumiEnd);
  
  //
  // Register input data
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");

  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "PassEvent::ConfAlg", fConf, this);

  //
  // Read decision bit for passing events
  //
  string decisionKeyLV1, decisionKeyHLT;

  if(reg.Get("PassEvent", "DecisionKeyLV1", decisionKeyLV1)) {
    fDecisionBitLV1 = Anp::AsTriggerBitLV1(decisionKeyLV1);
  }
  if(reg.Get("PassEvent", "DecisionKeyHLT", decisionKeyHLT)) {
    fDecisionBitHLT = Anp::AsTriggerBitHLT(decisionKeyHLT);
  }

  //
  // Set logic key
  //
  if     (fLogic == "AND") fLogicKey = 1;
  else if(fLogic == "OR")  fLogicKey = 2;
  else {
    log() << "Config - unknown logic value: " << fLogic << endl;
  }

  if(fLumiBeg > 0 && fLumiBeg <= fLumiEnd) {
    log() << "Config" << endl
	  << "   LumiBeg=" << fLumiBeg << endl
	  << "   LumiEnd=" << fLumiEnd << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::PassEvent::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //
  if(signal != "TRIG_CONFIG") return;

  if(fConf.valid()) { 
    Set(fConf->GetConfig()); 
  }
  else {
    log() << "Signal - invalid configuration algorithm" << endl;
  }
}

//-----------------------------------------------------------------------------
bool Anp::PassEvent::Init()
{  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::PassEvent::Exec()
{
  //
  // Make new event entry
  //
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD &&
     fEntry.GetEvent() == Ntuple::kPASS) {    
    //
    // Check that event passed trigger(s) and not vetoed
    //
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
void Anp::PassEvent::Done()
{
  //
  // Print number of passed and failed events
  // 
  const unsigned ntotal = fCountPass+fCountFail;

  if(ntotal > 0) {
    log() << "Done - " << endl
	  << "   total  events: " << ntotal     << endl
	  << "   passed events: " << fCountPass << endl
	  << "   failed events: " << fCountFail << endl
	  << "   nevents T0   = " << ntotal     << endl;
  }
}

//-----------------------------------------------------------------------------
bool Anp::PassEvent::Run(const TrigMonEvent &event)
{
  //
  // Process one event and require that event passes LB and trigger selection(s)
  //
  if(fLumiBeg > 0 && fLumiBeg <= fLumiEnd) {
    if(!(fLumiBeg <= event.getLumi() && event.getLumi() <= fLumiEnd)) {
      return false;
    }
  }

  //
  // Count events per lb and reject if we get too many
  //
  if(!EventLimit(event)) return false;

  //
  // Count selected chains for this event
  //
  if(fAcceptIds.empty()) return true;

  const std::vector<uint32_t> &event_hlt = event.getChain();
  const std::vector<uint16_t> &event_lv1 = event.getL1Item();

  unsigned scount = 0;
  unsigned vcount = 0;
  
  for(unsigned i = 0; i < event_hlt.size(); ++i) {
    const TrigMonChain chain(event_hlt.at(i));

    if(chain.isPassed(fDecisionBitHLT)) {
      scount += fAcceptIds.count(chain.getEncodedId());
    }
    if(chain.isPassed(fDecisionBitHLT)) {
      vcount += fVetoIds.count(chain.getEncodedId());
    }
  }
  
  for(unsigned i = 0; i < event_lv1.size(); ++i) {
    const TrigMonL1Item item(event_lv1.at(i));
    
    if(item.isPassed(fDecisionBitLV1)) {
      scount += fAcceptIds.count(item.getEncodedId());
    }
    if(item.isPassed(fDecisionBitLV1)) {
      vcount += fVetoIds.count(item.getEncodedId());
    }
  }

  if(vcount > 0) {
    return false;
  }

  if(fLogicKey == 1) {
    if(scount != fAcceptIds.size()) {
      return false;
    }
  }
  else if(fLogicKey == 2) {
    if(scount == 0) {
      return false;
    }
  }

  //
  // Count the number of SFO Files
  //
  float event_buffer_size = 0.0;
  if(event.getVar(9,event_buffer_size)) {
    ++fCountBuff;
  }
    
  return true;
}

//-----------------------------------------------------------------------------
void Anp::PassEvent::Set(const TrigMonConfig &config)
{
  //
  // Save counters for accept triggers
  //
  for(unsigned i = 0; i < config.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chain = config.at<TrigConfChain>(i);

    //
    // Collect accept triggers
    //
    if(std::find(fAccept.begin(), fAccept.end(), chain.getName()) != fAccept.end()) {
      fAcceptIds.insert(chain.getEncodedId());
      log() << "Set - add pass trigger: " << chain.getName() << endl;
    }
    
    //
    // Collect veto triggers
    //
    if(std::find(fVeto.begin(), fVeto.end(), chain.getName()) != fVeto.end()) {
      fVetoIds.insert(chain.getEncodedId());
      log() << "Set - add veto trigger: " << chain.getName() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
bool Anp::PassEvent::EventLimit(const TrigMonEvent &event)
{
  //
  // Add Event to vector for lumi block
  //
  const unsigned lb = event.getLumi();

  if(lb>=fEvents.size()){
    vector<unsigned>::iterator iter = fEvents.end();
    fEvents.insert(iter,lb-fEvents.size()+1,0);
  }
  ++fEvents.at(lb);

  if(fMaxEvts==0) return true;

  return fEvents.at(lb)<fMaxEvts;

}
