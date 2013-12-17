/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>

// ROOT
#include "TDirectory.h"
#include "TH1.h"

// Local
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostExec/TrigTimeChn.h"

using namespace std;

//--------------------------------------------------------------------------------------      
// Code for helper classes
//
Anp::TrigTimeChn::Event::Event()
  :n_event(0),
   n_alg_calls(0),
   n_alg_cache(0),
   n_seq_execs(0),
   timer_1st(0.0),
   timer_2nd(0.0),
   timer_sum(0.0),
   timer_sum2(0.0)
{
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeChn::Event::AddEvent(const Event &rhs)
{
  n_event     += rhs.n_event;
  n_alg_calls += rhs.n_alg_calls;
  n_alg_cache += rhs.n_alg_cache;
  n_seq_execs += rhs.n_seq_execs;
  timer_sum   += rhs.timer_sum;
  timer_sum2  += rhs.timer_sum2;
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeChn::Event::Clear()
{  
  // Clear variables
  n_event     = 0;
  n_alg_calls = 0;
  n_alg_cache = 0;
  n_seq_execs = 0;
  timer_1st   = 0.0;
  timer_2nd   = 0.0;
  timer_sum   = 0.0;
  timer_sum2  = 0.0;
}

//--------------------------------------------------------------------------------------      
// Code for main class
//
Anp::TrigTimeChn::TrigTimeChn()
  :fDir(0),
   fId(0),
   fDebug(false),
   fMakeHists(true),
   fMaxTimer(0),
   hChnTimer(0),
   hEvtFract(0),
   hAlgNCalls(0),
   hAlgNCache(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::TrigTimeChn::~TrigTimeChn()
{
  //
  // Cleanup histograms
  //
  for(HistMap::iterator hit = fHist.begin(); hit != fHist.end(); ++hit) {
    TH1 *h = hit -> second;
    if(h && !(h->GetDirectory())) delete h;
  }
  fHist.clear();
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeChn::Config(const Registry &reg, const TrigConfChain &chn)
{
  //
  // Configure self
  //
  fChn = chn;

  reg.Get("TrigTimeChn", "Debug",      fDebug);
  reg.Get("TrigTimeChn", "MakeHists",  fMakeHists);
  reg.Get("TrigTimeChn", "CanvasType", fCanvasType);

  //
  // Make my path
  //
  stringstream pathA;
  pathA << "CHN_" << setfill('0') << setw(10) << chn.getId();

  fId   = chn.getId();
  fName = chn.getName();
  fPath = pathA.str();

  if(fId == 0) {
    cout << "TrigTimeChn::Config - id=0: " << chn.getName() << " / " << fPath << endl;
  }

  //
  // Create histograms which are filled every event
  //
  if(fMakeHists) {
    hChnTimer    = FindTH1("chn_timer");
    hEvtFract    = FindTH1("chn_over_evt");
    hAlgNCalls   = FindTH1("chn_alg_ncalls");
    hAlgNCache   = FindTH1("chn_alg_ncache");
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeChn::AddEntry(const TrigMonSeq &entry)
{
  //
  // Add single algorithm measurement for matching algorithm
  //
  if(entry.getChnEncodedId() != fChn.getEncodedId()) { 
    cerr << "TrigTimeChn::AddEntry - wrong chain id: " << entry.getChnEncodedId() << endl;
    return;
  }

  //
  // Count of sequence executions by this chain
  //
  if(entry.getSeqTimer() > 0.0) {
    fEvent.timer_sum += entry.getSeqTimer(); 
  }
  else {
    fEvent.timer_sum += entry.getAlgTimer();     
  }

  fEvent.n_seq_execs++;

  //
  // Count of algorithm executions by this chain
  //
  const std::vector<TrigMonAlg>  &alg_entry = entry.getAlg();
  
  for(unsigned int i = 0; i < alg_entry.size(); ++i) {
    if(alg_entry.at(i).isCalled()) { 
      fEvent.n_alg_calls++;
    }
    else { 
      fEvent.n_alg_cache++;
    }
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeChn::AddEvent(double event_time)
{
  //
  // Add event to total and reset current event measurements
  //
  if(fEvent.n_seq_execs == 0) {
    return;
  }
  
  // Increase event count
  ++fEvent.n_event;

  //
  // Fill histograms if they are created
  //
  if(hChnTimer)  hChnTimer  -> Fill(fEvent.timer_sum);
  if(hAlgNCalls) hAlgNCalls -> Fill(fEvent.n_alg_calls);
  if(hAlgNCache) hAlgNCache -> Fill(fEvent.n_alg_cache);

  if(hEvtFract && event_time > 0.0) {
    hEvtFract  -> Fill(fEvent.timer_sum/event_time);
  }

  fMaxTimer = std::max(fMaxTimer, fEvent.timer_sum);

  // Add current event and reset event data
  fTotal.AddEvent(fEvent);
  fEvent.Clear();
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeChn::Finalize()
{
  //
  // Compute and save variables
  //
  if(fTotal.n_event < 1 || fTotal.n_seq_execs < 1) {
    return;
  }

  fVarMap["timer_sum"]        = fTotal.timer_sum;
  fVarMap["timer_max"]        = fMaxTimer;
  fVarMap["timer_exec_mean"]  = fTotal.timer_sum/double(fTotal.n_seq_execs);
  fVarMap["timer_exec_rms"]   = std::sqrt(fTotal.timer_sum2/double(fTotal.n_seq_execs));
  fVarMap["timer_event_mean"] = fTotal.timer_sum/double(fTotal.n_event);
  fVarMap["ncalls"]           = fTotal.n_alg_calls;
  fVarMap["ncache"]           = fTotal.n_alg_cache;
  fVarMap["nexecs"]           = fTotal.n_seq_execs;
  fVarMap["nevent"]           = fTotal.n_event;
}

//---------------------------------------------------------------------------------------
double Anp::TrigTimeChn::GetVar(const std::string &key) const
{
  //
  // Find variable
  //

  VarMap::const_iterator vit = fVarMap.find(key);
  if(vit != fVarMap.end()) {
    return vit -> second;
  } 

  //cerr << "TrigTimeChn::FindVar - " << fName << " missing var " << key << endl;
  return 0.0;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeChn::PrintVar(const std::string &key,
				const std::string &prf,
				const std::string &suf,
				std::ostream &os) const
{
  //
  // Print variable if it exists
  //
  os << prf;

  VarMap::const_iterator vit = fVarMap.find(key);
  if(vit != fVarMap.end()) {
    os << vit -> second;
  }

  os << suf << endl;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeChn::WriteHtml(Handle<HtmlNode> node, TCanvas *canvas) const
{
  //
  // Write summary into html node
  //
  if(!node.valid()) return;

  stringstream &os = node -> GetCont();
  os << "<pre>" << endl;

  //
  // Print summary
  //
  os << "----------------------------------------------------" << endl
     << "Timing information for a subset of processed events:" << endl << endl;

  PrintVar("nevent",           "Number of active events:    ", "",    os);
  PrintVar("timer_sum",        "Total time:                 ", " ms", os);
  PrintVar("timer_max",        "Longest event time:         ", " ms", os);
  PrintVar("timer_event_mean", "Mean time per event:        ", " ms", os);
  PrintVar("ncalls",           "Number of algorithm calls:  ", "",    os);
  PrintVar("ncache",           "Number of algorithm caches: ", "",    os);
  
  os << endl;
  
  Anp::PrintHist(node.ref(), hChnTimer, canvas, node->GetBase()+"_chn_timer."+fCanvasType, 
		 "Chain event time");

  Anp::PrintHist(node.ref(), hEvtFract, canvas, node->GetBase()+"_evt_fract."+fCanvasType,
		 "Chain time/Event time");

  Anp::PrintHist(node.ref(), hAlgNCalls, canvas, node->GetBase()+"_alg_ncalls."+fCanvasType,
		 "Number of executed algorithm calls per event");

  Anp::PrintHist(node.ref(), hAlgNCache, canvas, node->GetBase()+"_alg_ncache."+fCanvasType,
		 "Number of cached algorithm calls per event");

  os << "</pre>" << endl;
}

//---------------------------------------------------------------------------------------
TH1* Anp::TrigTimeChn::FindTH1(const std::string &key)
{
  //
  // Find/make histogram
  //
  HistMap::iterator hit = fHist.find(key);
  if(hit != fHist.end()) {
    return hit -> second;
  }

  TH1 *h = Anp::HistMan::Instance().CreateTH1(key, "time_chn");
  Anp::SetDir(h, fDir);
   
  hit = fHist.insert(HistMap::value_type(key, h)).first;

  if(!h) {
    cerr << "TrigTimeChn::FindTH1 - no HistMan histogram for: " << key << endl;
  }

  return h;
}

//-----------------------------------------------------------------------------
bool Anp::operator==(const Handle<TrigTimeChn> &lhs, const Handle<TrigTimeChn> &rhs)
{
  //
  // Compare handles
  //
  if(!lhs.valid() || !rhs.valid()) {
    return false;
  }

  return lhs->GetName() == rhs->GetName();
}

//-----------------------------------------------------------------------------
bool Anp::operator==(const Handle<TrigTimeChn> &chn, const TrigConfChain &key)
{
  //
  // Compare handle and config
  //
  if(!chn.valid()) return false;

  return chn->GetName() == key.getName();  
}

//-----------------------------------------------------------------------------
bool Anp::operator==(const TrigConfChain &key, const Handle<TrigTimeChn> &chn)
{
  //
  // Compare handle and config
  //
  if(!chn.valid()) return false;

  return chn->GetName() == key.getName();
}

//-----------------------------------------------------------------------------
bool Anp::operator<(const Handle<TrigTimeChn> &lhs, const Handle<TrigTimeChn> &rhs)
{
  //
  // Compare handles
  //
  if(!lhs.valid() || !rhs.valid()) {
    return false;
  }

  return lhs->GetTrigConfChain().getEncodedId() < rhs->GetTrigConfChain().getEncodedId();
}

//-----------------------------------------------------------------------------
bool Anp::operator<(const Handle<TrigTimeChn> &chn, unsigned int key)
{
  //
  // Compare handle and config
  //
  if(!chn.valid()) return false;

  return chn->GetTrigConfChain().getEncodedId() < key;
}

//-----------------------------------------------------------------------------
bool Anp::operator<(unsigned int key, const Handle<TrigTimeChn> &chn)
{
  //
  // Compare handle and config
  //
  if(!chn.valid()) return false;

  return key < chn->GetTrigConfChain().getEncodedId();
}

//---------------------------------------------------------------------------------------
bool Anp::TimeChnSort::operator()(const Anp::Handle<Anp::TrigTimeChn> &lhs,
				  const Anp::Handle<Anp::TrigTimeChn> &rhs)
{
  //
  // Compare two objects based on key
  //
  if(!lhs.valid() || !rhs.valid()) { 
    cerr << "TimeChnSort::operator() - lhs.ptr=" << lhs.get() << ", " << "rhs.ptr=" << rhs.get() << endl;
    return false;
  }

  if(!fKey.empty() && fKey != "name") {
    //
    // Compare by variables
    //
    const std::map<std::string, double> &lmap = lhs -> GetVarMap();
    const std::map<std::string, double> &rmap = rhs -> GetVarMap();
    
    const map<string, double>::const_iterator lit = lmap.find(fKey);
    const map<string, double>::const_iterator rit = rmap.find(fKey);

    //
    // Need these to preserve properties of < operator
    //
    if(lit == lmap.end() && rit == rmap.end()) {
      cout << "operator() - missing key " << fKey << " for: " 
	   << lhs -> GetName() << " and " << rhs -> GetName() << endl;
      return false;
    }
    else if(lit == lmap.end()) {
      return false;
    }
    else if(rit == rmap.end()) {
      return true;
    }
    else {
      return lit -> second > rit -> second;
    }
  }

  //
  // Compare by name or type
  //
  return lhs->GetName() < rhs->GetName();
}
