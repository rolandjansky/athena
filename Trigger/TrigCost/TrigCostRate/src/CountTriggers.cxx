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
#include "TrigCostRate/CountTriggers.h"
#include "TrigCostBase/HtmlABC.h"

REGISTER_ANP_OBJECT(AlgEvent,CountTriggers)

using namespace std;

//-----------------------------------------------------------------------------
Anp::CountTriggers::TData::TData()
  :name(),
   level(0),
   counter(0),
   ps(0),
   pt(0),
   count_tav(0),
   count_tbp(0),
   count_tap(0),
   count_raw(0),
   count_phy(0)
{
}

//-----------------------------------------------------------------------------
float Anp::CountTriggers::TData::GetEff() const
{
  if(count_tav !=0){
    return static_cast<float>(count_tav)/static_cast<float>(count_raw);
  }

  return 0.0;
}

//-----------------------------------------------------------------------------
Anp::CountTriggers::CountTriggers()
  :fDebug(false),
   fLumiBeg(0),
   fLumiEnd(0),
   fCountRaw(0),
   fCountPass(0),
   fCountFail(0),
   fCountBuff(0),
   fCheckForL1Failure(false)
{
}

//-----------------------------------------------------------------------------
Anp::CountTriggers::~CountTriggers()
{
}

//-----------------------------------------------------------------------------
void Anp::CountTriggers::Config(const Registry &reg)
{
  //
  // Configure self and register input data
  //
  reg.Get("CountTriggers", "Debug", fDebug);
  reg.Get("CountTriggers::LumiBeg", fLumiBeg);
  reg.Get("CountTriggers::LumiEnd", fLumiEnd);
  reg.Get("CountTriggers::CheckL1", fCheckForL1Failure);
  
  //
  // Register input data
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");

  //
  // Register input configuration when requested
  //
  fConfig = NtupleSvc::Instance("config").Branch<TrigMonConfig>("conf").GetRef();

  if(fDebug){
    for(unsigned i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
      const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);
      cout << "Chain name: " << chain.getName() << endl;
    }
  }
  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "CountTriggers::ConfAlg", fConf, this);

  //
  // Add self for a callback from html algorithm
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
}

//-----------------------------------------------------------------------------
void Anp::CountTriggers::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //

  if(signal != "TRIG_CONFIG" && signal != "TRIG_CONFIG_DB") return;

  if(fConf.valid()) { 
    log() << "Signal - configuration algorithm: " << fConf->GetName() << endl;
    fConfig = fConf->GetConfig(); 
  }
  else {
    log() << "Signal - invalid configuration algorithm" << endl;
  }

  if(fDebug){
    for(unsigned i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
      const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);
      cout << "Chain name: " << chain.getName() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
bool Anp::CountTriggers::Init()
{  
  return true;
}

//-----------------------------------------------------------------------------
void Anp::CountTriggers::Exec()
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
void Anp::CountTriggers::Done()
{
  //
  // Print number of passed and failed events
  // 
  const unsigned ntotal = fCountPass+fCountFail;

  log() << "Done - " << endl
	<< "   total  events: " << ntotal     << endl
	<< "   raw  events:   " << fCountRaw  << endl
	<< "   passed events: " << fCountPass << endl
	<< "   failed events: " << fCountFail << endl;

  if(fDebug) Write(std::cout);
}

//-----------------------------------------------------------------------------
bool Anp::CountTriggers::Run(const TrigMonEvent &event)
{
  //
  // Process one event and require that event passes LB and trigger selection(s)
  //
  if(fLumiBeg > 0 && fLumiBeg <= fLumiEnd) {
    if(!(fLumiBeg <= event.getLumi() && event.getLumi() <= fLumiEnd)) {
      return false;
    }
  }

  ++fCountRaw;

  const std::vector<uint32_t> &event_hlt = event.getChain();
  const std::vector<uint16_t> &event_lv1 = event.getL1Item();

  for(unsigned i = 0; i < event_hlt.size(); ++i) {
    const TrigMonChain chain(event_hlt.at(i));

    TData &data = FindTData(chain.getEncodedId());

    if(chain.isPassed(TrigMonChain::kPassed))           data.count_phy++;
    if(chain.isPassed(TrigMonChain::kPassedRaw))        data.count_raw++;
    if(chain.isPassed(TrigMonChain::kL1AfterVeto))      data.count_tav++;
    if(chain.isPassed(TrigMonChain::kL1AfterPrescale))  data.count_tap++;
    if(chain.isPassed(TrigMonChain::kL1BeforePrescale)) data.count_tbp++;
  }
  bool passL1 = false;

  for(unsigned i = 0; i < event_lv1.size(); ++i) {
    const TrigMonL1Item item(event_lv1.at(i));

    TData &data = FindTData(item.getEncodedId());

    data.count_raw++;
    if(item.isPassed(TrigMonL1Item::kL1AfterVeto)){
      data.count_tav++;
      passL1=true;
    }
    if(item.isPassed(TrigMonL1Item::kL1AfterPrescale))  data.count_tap++;
    if(item.isPassed(TrigMonL1Item::kL1BeforePrescale)) data.count_tbp++;    
  }

  if(event_lv1.size()==0) cout << "ERROR - Not L1 Items accepted!" << endl;
  if(!passL1 && fCheckForL1Failure) cout << "ERROR - FailedL1!!!!!!!" << endl; 
    
  return true;
}

//-----------------------------------------------------------------------------
Anp::CountTriggers::TData& Anp::CountTriggers::FindTData(uint32_t id)
{
  //
  // Find the Trigger Info
  //
  TrigMap::iterator chn = fTrigMap.find(id);

  if(chn == fTrigMap.end()) {
    //
    // Find matching trigger configuration
    //
    for(unsigned i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
      const TrigConfChain &chain = fConfig.at<TrigConfChain>(i);
      
      if(chain.getEncodedId() == id) {
	
	TData data;
	data.name    = chain.getName();
	data.counter = chain.getCounter();
	data.level   = chain.getLevelId();
	data.ps      = chain.getPS();
	data.pt      = chain.getPT();

	chn = fTrigMap.insert(TrigMap::value_type(id, data)).first;
	break;
      }
    }

    if(chn == fTrigMap.end()) {
      log() << "FindTData - failed to match id: " << id << endl;
      chn = fTrigMap.insert(TrigMap::value_type(id, TData())).first;
    }
  }

  return chn->second;
}

//-----------------------------------------------------------------------------
void Anp::CountTriggers::Write(HtmlNode &inode, HtmlMenu &)
{
  //
  // Printing trigger counts from cost data
  //
  cout << "CountTriggers::Write - writing table for Chain Counts!" << endl;

  //
  // Create index page for this algorithm
  //
  Handle<HtmlNode> rnode(new HtmlNode(GetName()+".html", HtmlNode::kHTML));
  rnode->SetBase(GetName());

  inode.AddChild(rnode);
  inode.GetCont() << rnode->GetLink("Rates summary "+GetName()) << endl;

  Write(rnode->GetCont());
}

//-----------------------------------------------------------------------------
void Anp::CountTriggers::Write(std::ostream &os) const
{
  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Chain </th>"
     << "<th> Counter </th>"
     << "<th> PS </th>"
     << "<th> PT </th>"
     << "<th> Input Counts </th>"
     << "<th> Accept Counts </th>"
     << "<th> Accept Fraction </th>"
     << "</tr>" << endl;

  for(TrigMap::const_iterator trig = fTrigMap.begin(); trig != fTrigMap.end(); ++trig){
    const TData &data = trig->second;

    os << "<tr>" 
       << "<td align=\"left\"> "   << data.name                             << " </td>"
       << "<td align=\"center\"> " << data.counter                          << " </td>"
       << "<td align=\"center\"> " << data.ps                               << " </td>"
       << "<td align=\"center\"> " << data.pt                               << " </td>"
       << "<td align=\"center\"> " << data.count_raw                        << " </td>"
       << "<td align=\"center\"> " << data.count_tav                        << " </td>"
       << "<td align=\"center\"> " << data.GetEff()                         << " </td>"
       << "</tr>" << endl;
  }
  
  os << "</table>" << endl;
}
