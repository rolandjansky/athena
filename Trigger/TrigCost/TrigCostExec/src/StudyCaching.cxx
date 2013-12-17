/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

// Boost
#include <boost/foreach.hpp>

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TImageDump.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostExec/RobService.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostExec/StudyCaching.h"

REGISTER_ANP_OBJECT(AlgEvent,StudyCaching)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
// Begin StudyCaching code
//
Anp::StudyCaching::StudyCaching()
  :fDir      (0),
   fDirName  ("time"),
   fRootName (""),
   fDebug    (false),
   fShowNAlg (100),
   fLevel    (0),
   fIsConfig (false),
   fTimeEvent(0.0),
   fTimeSteer(0.0),
   fCountL2  (0),
   fCountEF  (0),
   fNEvent   (0),
   fTotalAlgNCache(0),
   fTotalAlgNCalls(0)
{
}

//-----------------------------------------------------------------------------
Anp::StudyCaching::~StudyCaching()
{
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::Config(const Registry &reg)
{
  fReg = reg;

  reg.Get("StudyCaching", "Debug",        fDebug);
  reg.Get("StudyCaching::HistOpts",       fHistOpts);
  reg.Get("StudyCaching::DirName",        fDirName);
  reg.Get("StudyCaching::ShowNAlg",       fShowNAlg);
  reg.Get("StudyCaching::Level",          fLevel);
  reg.Get("StudyCaching::MinLB",          fMinLB = 0);
  reg.Get("StudyCaching::MaxLB",          fMaxLB = 0);

  vector<int> lbs;
  
  if(reg.Get("StudyCaching::LBs", lbs)) {
    BOOST_FOREACH(const int lb, lbs) {
      fLBs.insert(static_cast<unsigned>(lb));
    }
  }

  string epath;
  
  if(reg.Get("StudyCaching::EventFile", epath)) {
    ReadEvents(epath, fTimes);
  }

  //
  // Initiliaze input data pointers
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
    
  //
  // Add self to global Factory list - used by CostMaker and others
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
    
  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "StudyCaching::ConfAlg", fConf, this);
    
  log() << "Config " << endl
	<< "   Debug: " << fDebug     << endl
	<< "   Level: " << fLevel     << endl;

  log() << "Config - select following lumi blocks: " << endl;

  BOOST_FOREACH(const unsigned lb, fLBs) {
    cout << "   " << lb << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //
  if(signal != "TRIG_CONFIG" && signal != "TRIG_CONFIG_DB") { 
    return;
  }

  log() << "Signal - process signal: " << signal << endl;

  if(fConf.valid()) { 
    Set(fConf->GetConfig());
  }
  else {
    log() << "Signal - invalid configuration input" << endl;   
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::Save(TDirectory *dir)
{
  fDir = Anp::GetDir(dir, fDirName);
}

//-----------------------------------------------------------------------------
bool Anp::StudyCaching::Init()
{  
  //
  // Read configuration from init file and release pointer
  //
  Anp::SetROOT_defaults();

  hTimeAlgs      = GetTH1("time_algs");
  hTimeCost      = GetTH1("time_cost");  
  hTimeEvent     = GetTH1("time_event");
  hTimeTexec     = GetTH1("time_texec");
  hTimeTproc     = GetTH1("time_tproc");

  hAlgNExec      = GetTH1("alg_nexec");
  hAlgNCache     = GetTH1("alg_ncache");
  hAlgFCache     = GetTH1("alg_fcache");
  hAlgNTotal     = GetTH1("alg_ntotal");

  hSeqNExec      = GetTH1("seq_nexec");
  hSeqNTotal     = GetTH1("seq_ntotal");

  hChnPass       = GetTH1("chn_pass");
  hEventLB       = GetTH1("event_lb");

  return true;
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::Exec()
{
  //
  // Process data for one event
  // 
  if(fEntry.valid() && 
     fEntry.GetState() == Ntuple::kREAD && 
     fEntry.GetEvent() == Ntuple::kPASS) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::Done()
{
  log() << "Done - summary: " << endl
	<< "   Events:         " << fNEvent         << endl
	<< "   CountL2:        " << fCountL2        << endl
	<< "   CountEF:        " << fCountEF        << endl
	<< "   TotalAlgNCache: " << fTotalAlgNCache << endl
	<< "   TotalAlgNCalls: " << fTotalAlgNCalls << endl;
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::Set(const TrigMonConfig &config)
{
  //
  // Copy configuration and prepare algorithm, chain and sequence objects
  //
  fIsConfig = true;
  fConfig   = config;
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::Run(const TrigMonEvent &event)
{
  //
  // Collect chain, sequence and algorithm timing entries
  //
  if(!fIsConfig) {
    log() << "Run - missing valid trigger configuration" << endl;
    return;
  }
  
  const unsigned lb = event.getLumi();

  if(fMinLB > 0 && fMinLB < fMaxLB) {
    if(!(fMinLB <= lb && lb <= fMaxLB)) {
      return;
    }
  }

  if(!fLBs.empty() && fLBs.count(lb) == 0) {
    return;
  }

  if(!PassEvent(event)) {
    return;
  }
  
  if(!AddEvent(event.getSec(), event.getNanoSec(), fEvents)) {
    log() << "Run - failed to add: " << event.getSec() << "," << event.getNanoSec() << endl;
    return;
  }

  ++fNEvent;

  bool has_L2 = false;
  bool has_EF = false;

  unsigned alg_nexec  = 0;
  unsigned alg_ncache = 0;
  unsigned alg_ntotal = 0;

  unsigned seq_nexec  = 0;
  unsigned seq_ntotal = 0;

  double   time_algs  = 0.0;

  for(unsigned iseq = 0; iseq < event.size<TrigMonSeq>(); ++iseq) {
    const TrigMonSeq &seq = event.at<TrigMonSeq>(iseq);

    if(seq.getLevel() == 2) has_L2 = true;
    if(seq.getLevel() == 3) has_EF = true;

    if(fLevel != 0 && seq.getLevel() != fLevel) { 
      //continue;   
    }

    seq_nexec  += seq.isExecuted();
    seq_ntotal += 1.0;

    const std::vector<TrigMonAlg> &alg_entry = seq.getAlg();

    for(unsigned j = 0; j < alg_entry.size(); ++j) {
      const TrigMonAlg &alg = alg_entry.at(j);

      alg_nexec  += alg.isCalled();
      alg_ncache += alg.isCached();
      alg_ntotal += 1.0;
      
      time_algs  += alg.getTimer();
    }
  }

  if(fLevel == 0 && has_L2 && has_EF) {
    log() << "Run - found L2 and EF records... this alg does not expect this" << endl;
  }

  fTotalAlgNCache += alg_ncache;
  fTotalAlgNCalls += alg_ntotal;

  FillHist(hAlgNExec,  alg_nexec);
  FillHist(hAlgNCache, alg_ncache);
  FillHist(hAlgNTotal, alg_ntotal);

  if(alg_ntotal > 0) {
    FillHist(hAlgFCache, double(alg_ncache)/double(alg_ntotal));
  }

  FillHist(hSeqNExec,  seq_nexec);
  FillHist(hSeqNTotal, seq_ntotal);

  //
  // Count events
  //
  fCountL2 += has_L2;
  fCountEF += has_EF;
  
  //
  // Read event timers: steering Tproc and internal cost timer
  //
  float icost_timer = 0.0;
  float tproc_timer = 0.0;
  float texec_timer = 0.0;
  const float event_timer = event.getTimer();

  if(fLevel == 0 || fLevel == 2) { 
    event.getVar(100, icost_timer);
    event.getVar(101, texec_timer);
    event.getVar(102, tproc_timer);
  }  
  if(fLevel == 0 || fLevel == 3) { 
    event.getVar(110, icost_timer);
    event.getVar(111, texec_timer);
    event.getVar(112, tproc_timer);
  }
    
  fTimeSteer += tproc_timer;
  fTimeEvent += event_timer;

  FillHist(hTimeAlgs,  time_algs);
  FillHist(hTimeCost,  icost_timer);
  FillHist(hTimeEvent, event_timer);
  FillHist(hTimeTexec, texec_timer);
  FillHist(hTimeTproc, tproc_timer);

  //
  // Process trigger decisions
  //
  BOOST_FOREACH(const TrigMonChain &chain, event.getChains()) {

    if(fLevel != 0 && chain.getLevel() != fLevel) { 
      continue;   
    }

    if(chain.isPassed(TrigMonChain::kPassed)) {
      FillHist(hChnPass, chain.getCounter());
    }
  }

  //
  // Fill number of events
  //
  FillHist(hEventLB, event.getLumi());
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::Write(HtmlNode &, HtmlMenu &)
{
  //
  // Write html summary
  //
}

//-----------------------------------------------------------------------------
string Anp::StudyCaching::level() const
{  
  if     (fLevel == 0) return "AL";
  if     (fLevel == 2) return "L2";
  else if(fLevel == 3) return "EF";
  
  return "UN";
}

//---------------------------------------------------------------------------------------
TH1* Anp::StudyCaching::GetTH1(const std::string &key) const
{
  //
  // Create histograms - owned by TDirectory
  //
  TH1 *h = Anp::HistMan::Instance().CreateTH1(key, "study_caching_" + level());

  if(!h) {
    log()<< "GetTH1 - missing histogram: " << key << endl;
  }
  else {
    Anp::SetDir(h, fDir);  
  }

  return h;
}

//---------------------------------------------------------------------------------------
TH2* Anp::StudyCaching::GetTH2(const std::string &key) const
{
  //
  // Create histograms - owned by TDirectory
  //
  TH2 *h = Anp::HistMan::Instance().CreateTH2(key, "study_caching_" + level());

  if(!h) {
    log()<< "GetTH1 - missing histogram: " << key << endl;
  }
  else {
    Anp::SetDir(h, fDir);  
  }

  return h;
}

//---------------------------------------------------------------------------------------
void Anp::StudyCaching::FillHist(TH1 *h, double v)
{
  if(h) h->Fill(v);
}

//-----------------------------------------------------------------------------
void Anp::StudyCaching::ReadEvents(const std::string &fpath, SecMap &smap)
{
  //
  // Read text file with event LB, sec, nanosec
  //
  ifstream myfile(fpath);  
  string line;

  if(!myfile.is_open()) {
    log() << "ReadEvents - failed to open: " << fpath << endl;
    return;
  }

  log() << "ReadEvents - opened: " << fpath << endl;

  vector<string> parts;

  unsigned nevent = 0;

  while ( getline (myfile,line) ) {
    parts.clear();
    Anp::StringTok(parts, line, " ");
    
    if(parts.size() == 3) {
      stringstream s1, s2;
      
      s1 << parts[1];
      s2 << parts[2];
      
      unsigned sec = 0;
      unsigned ns  = 0;
      
      if((s1 >> sec) && (s2 >> ns)) {

	if(AddEvent(sec, ns, smap)) {
	  ++nevent;
	}
	else {
	  log() << "ReadEvents - failed to add: " << sec << ", " << ns << endl;
	}
      }
    }
  }

  log() << "ReadEvents - finished reading file" << endl
	<< "   Seconds: " << smap.size() << endl
	<< "   Events:  " << nevent      << endl;
  
  myfile.close();
}


//-----------------------------------------------------------------------------
bool Anp::StudyCaching::AddEvent(unsigned sec, unsigned ns, SecMap &smap) const
{
  SecMap::iterator sit = smap.find(sec);
  
  if(sit == smap.end()) {
    sit = smap.insert(SecMap::value_type(sec, set<unsigned>())).first;
  }
  
  return sit->second.insert(ns).second;  
}

//-----------------------------------------------------------------------------
bool Anp::StudyCaching::PassEvent(const TrigMonEvent &event) const
{
  if(fTimes.empty()) {
    return true;
  }

  const SecMap::const_iterator sit = fTimes.find(event.getSec());
  
  if(sit == fTimes.end()) {
    return false;
  }

  return sit->second.count(event.getNanoSec()) > 0;
}
