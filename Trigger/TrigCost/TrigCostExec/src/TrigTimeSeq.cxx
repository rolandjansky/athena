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
#include "TrigCostExec/TrigTimeSeq.h"

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::TrigTimeSeq::Event::Event()
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
void Anp::TrigTimeSeq::Event::AddEvent(const Event &rhs)
{
  n_event     += rhs.n_event;
  n_alg_calls += rhs.n_alg_calls;
  n_alg_cache += rhs.n_alg_cache;
  n_seq_execs += rhs.n_seq_execs;
  timer_sum   += rhs.timer_sum;
  timer_sum2  += rhs.timer_sum2;
}

//--------------------------------------------------------------------------------------      
Anp::TrigTimeSeq::TrigTimeSeq()
  :fDir(0),
   fDebug(false),
   fMakeHists(true),
   fLevel(0),
   hSeqTimer(0),
   hEvtFract(0),
   hSeqNExecs(0),
   hAlgNCalls(0),
   hAlgNCache(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::TrigTimeSeq::~TrigTimeSeq()
{
  //
  // Cleanup histograms
  //
  for(HistMap::iterator hit = fHist.begin(); hit != fHist.end(); ++hit) {
    TH1 *h = hit -> second;
    if(h && !(h->GetDirectory())) { 
      if(fDebug) {
	cout << "TrigTimeSeq dtor - deleting TH1: " << h->GetName() << endl;
      }
      delete h;
    }
  }

  fHist.clear();
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeSeq::Event::Clear()
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
void Anp::TrigTimeSeq::Config(const Registry &reg, const TrigConfSeq &seq)
{
  //
  // Configure self
  //
  fSeq = seq;

  reg.Get("TrigTimeSeq", "Debug",   fDebug);
  reg.Get("TrigTimeSeq::MakeHists", fMakeHists);

  //
  // Make my path
  //
  stringstream pathA;
  pathA << "SEQ_" << setfill('0') << setw(10) << seq.getId();

  fName = seq.getName();
  fPath = pathA.str();

  if(seq.getId() == 0) {
    cout << "TrigTimeSeq::Config - id=0: " << seq.getName() << " / " << fPath << endl;
  }

  //
  // Fill lookup table for algorithms with zeros
  //
  TrigTimeAlg *alg = 0;
  fAlgName.insert(fAlgName.end(), seq.getAlg().size(), alg);
  fAlgType.insert(fAlgType.end(), seq.getAlg().size(), alg);

  //
  // Create histograms which are filled every event
  //
  if(fMakeHists) {
    hSeqTimer    = FindTH1("seq_timer");
    hEvtFract    = FindTH1("seq_over_evt");
    hSeqNExecs   = FindTH1("seq_nexecs");
    hAlgNCalls   = FindTH1("seq_alg_ncalls");
    hAlgNCache   = FindTH1("seq_alg_ncache");
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeSeq::AddEntry(const TrigMonSeq &entry)
{
  //
  // Add single algorithm measurement for matching algorithm
  //
  if(entry.getSeqIndex() != fSeq.getIndex()) { 
    cerr << "TrigTimeSeq::AddEntry - wrong seq index: " << entry.getSeqIndex() << endl;
    return;
  }

  if(fDebug) {
    cout << "SEQ:" << fSeq.getName() << " state: " << Trig::SeqStateAsString(entry) << endl;
  }

  //
  // Count of algorithm calls by this sequence
  //
  fEvent.timer_sum += entry.getSeqTimer(); 
  fEvent.n_seq_execs++;

  //
  // Check level
  //
  if(fLevel == 0) { 
    fLevel = entry.getLevel();
  }
  else if(fLevel != entry.getLevel()) {
    cerr << "StudyTime::Run - bad seq level: " << fLevel << "!=" << entry.getLevel() << endl;
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeSeq::ResetEvent(const EventTime &event)
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
  if(hSeqNExecs) hSeqNExecs -> Fill(fEvent.n_seq_execs);
  if(hSeqTimer)  hSeqTimer  -> Fill(fEvent.timer_sum);
  if(hAlgNCalls) hAlgNCalls -> Fill(fEvent.n_alg_calls);
  if(hAlgNCache) hAlgNCache -> Fill(fEvent.n_alg_cache);
  
  if(hEvtFract && event.event_timer > 0.0) {
    hEvtFract  -> Fill(fEvent.timer_sum/event.event_timer);
  }

  // Add current event and reset event data
  fTotal.AddEvent(fEvent);
  fEvent.Clear();

  //
  // Reset algorithm pointers
  //    
  for(vector<TrigTimeAlg *>::iterator it = fAlgName.begin(); it != fAlgName.end(); ++it) {  
    if(*it) {
      (*it) -> ResetEvent(event, fLevel);
    }
    else {
      cerr << "TrigTimeSeq::AddEvent - failed to get TrigTimeAlg" << endl;
    }
  }
  for(vector<TrigTimeAlg *>::iterator it = fAlgType.begin(); it != fAlgType.end(); ++it) {
    if(*it) {
      (*it) -> ResetEvent(event, fLevel);
    }
    else {
      cerr << "TrigTimeSeq::AddEvent - failed to get TrigTimeAlg" << endl;
    }
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeSeq::Finalize()
{
  //
  // Compute and save variables
  //
  if(fTotal.n_event < 1 || fTotal.n_seq_execs < 1) {
    return;
  }

  fVarMap["timer_sum"]        = fTotal.timer_sum;
  fVarMap["timer_exec_mean"]  = fTotal.timer_sum/double(fTotal.n_seq_execs);
  fVarMap["timer_exec_rms"]   = std::sqrt(fTotal.timer_sum2/double(fTotal.n_seq_execs));
  fVarMap["timer_event_mean"] = fTotal.timer_sum/double(fTotal.n_event);
  fVarMap["ncalls"]           = fTotal.n_alg_calls;
  fVarMap["ncache"]           = fTotal.n_alg_cache;
  fVarMap["nexecs"]           = fTotal.n_seq_execs;
  fVarMap["nevent"]           = fTotal.n_event;
}

//---------------------------------------------------------------------------------------
double Anp::TrigTimeSeq::GetVar(const std::string &key) const
{
  //
  // Find variable
  //

  VarMap::const_iterator vit = fVarMap.find(key);
  if(vit != fVarMap.end()) {
    return vit -> second;
  } 

  //cerr << "TrigTimeSeq::FindVar - " << fName << " missing var " << key << endl;
  return 0.0;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeSeq::PrintVar(const std::string &key,
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
void Anp::TrigTimeSeq::WriteHtml(Handle<HtmlNode> node, const HtmlMenu &menu) const
{
  //
  // Write summary into html node
  //
  if(!node.valid()) { 
    return;
  }

  node -> GetCont() << "Sequence: " << fSeq.getName() << endl << endl;

  //
  // Print algorithms
  //
  node -> GetCont() << "Sequence uses " << fSeq.getAlg().size() << " algorithm(s):" << endl;

  for(vector<TrigConfAlg>::const_iterator ialg = fSeq.algBeg(); ialg != fSeq.algEnd(); ++ialg) {
    const TrigConfAlg &alg = *ialg;
    
    //
    // Try to get link for algorithm
    //
    Handle<HtmlNode> nodeA = menu.FindAlgName(alg.getName());
    if(nodeA.valid()) {
      node -> GetCont() << nodeA->GetLink(alg.getName()) << endl;
    }
    else {
      node -> GetCont() << alg.getName() << endl;
    }
  }
  
  node -> GetCont() << endl;

  //
  // Print summary
  //
  PrintVar("nevent",           "Number of events:              ", "",    node -> GetCont());
  PrintVar("nexecs",           "Number of sequence executions: ", "",    node -> GetCont());
  PrintVar("timer_sum",        "Total time:                    ", " ms", node -> GetCont());
  PrintVar("timer_event_mean", "Mean time per event:           ", " ms", node -> GetCont());

  node -> GetCont() << endl;
}

//---------------------------------------------------------------------------------------
TH1* Anp::TrigTimeSeq::FindTH1(const std::string &key)
{
  //
  // Find/make histogram
  //
  HistMap::iterator hit = fHist.find(key);
  if(hit != fHist.end()) {
    return hit -> second;
  }

  TH1 *h = Anp::HistMan::Instance().CreateTH1(key, "time_seq");
  Anp::SetDir(h, fDir);
   
  hit = fHist.insert(HistMap::value_type(key, h)).first;

  if(!h) {
    cerr << "TrigTimeSeq::FindTH1 - no HistMan histogram for: " << key << endl;
  } 

  return h;
}

//---------------------------------------------------------------------------------------
Anp::TrigTimeAlg* Anp::TrigTimeSeq::GetAlgName(unsigned index)
{
  //
  // Set pointer to algorithm instance class: index is position within sequence
  //
  if(index < fAlgName.size()) return fAlgName.at(index);
  
  cerr << "TrigTimeSeq::GetAlgName - missing index: " << index << endl;
  return 0;
}

//---------------------------------------------------------------------------------------
Anp::TrigTimeAlg* Anp::TrigTimeSeq::GetAlgType(unsigned index)
{
  //
  // Set pointer to algorithm class class: index is position within sequence
  //
  if(index < fAlgType.size()) return fAlgType.at(index);

  cerr << "TrigTimeSeq::GetAlgType - missing index: " << index << endl;
  return 0;
}

//---------------------------------------------------------------------------------------
bool Anp::TrigTimeSeq::SetAlgType(unsigned index, TrigTimeAlg *alg)
{
  //
  // Set pointer to algorithm instance: index is position within sequence
  //
  if(!alg) {
    cerr << "TrigTimeSeq::SetAlgName - null alg pointer" << endl;
    return false;
  }
  
  if(index >= fAlgName.size()) {
    cerr << "TrigTimeSeq::SetAlgName - index is out of range" << endl;    
    return false;
  }

  if(fAlgName.at(index)) { 
    cerr << "TrigTimeSeq::SetAlgName - alg pointer is already set" << endl;    
    return false;
  }
  
  fAlgName.at(index) = alg;
  return true;
}

//---------------------------------------------------------------------------------------
bool Anp::TrigTimeSeq::SetAlgName(unsigned index, TrigTimeAlg *alg)
{
  //
  // Set pointer to algorithm bass class: index is position within sequence
  //
  if(!alg) {
    cerr << "TrigTimeSeq::SetAlgType - null alg pointer" << endl;
    return false;
  }
  
  if(index >= fAlgType.size()) {
    cerr << "TrigTimeSeq::SetAlgType - index is out of range" << endl;    
    return false;
  }

  if(fAlgType.at(index)) { 
    cerr << "TrigTimeSeq::SetAlgType - alg pointer is already set" << endl;    
    return false;
  }
  
  fAlgType.at(index) = alg;
  return true;
}

//-----------------------------------------------------------------------------
bool Anp::operator==(const Handle<TrigTimeSeq> &lhs, const Handle<TrigTimeSeq> &rhs)
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
bool Anp::operator==(const Handle<TrigTimeSeq> &seq, const TrigConfSeq &key)
{
  //
  // Compare handle and config
  //
  if(!seq.valid()) return false;

  return seq->GetName() == key.getName();  
}

//-----------------------------------------------------------------------------
bool Anp::operator==(const TrigConfSeq &key, const Handle<TrigTimeSeq> &seq)
{
  //
  // Compare handle and config
  //
  return seq == key;
}

//-----------------------------------------------------------------------------
bool Anp::operator<(const Handle<TrigTimeSeq> &lhs, const Handle<TrigTimeSeq> &rhs)
{
  //
  // Compare handles
  //
  if(!lhs.valid() || !rhs.valid()) {
    return false;
  }

  return lhs ->GetId() < rhs -> GetId();
}

//-----------------------------------------------------------------------------
bool Anp::operator<(const Handle<TrigTimeSeq> &seq, unsigned int key)
{
  //
  // Compare handle and config
  //
  if(!seq.valid()) return false;

  return seq->GetId() < key;
}

//-----------------------------------------------------------------------------
bool Anp::operator<(unsigned int key, const Handle<TrigTimeSeq> &seq)
{
  //
  // Compare handle and config
  //
  if(!seq.valid()) return false;

  return key < seq->GetId();
}

//---------------------------------------------------------------------------------------
bool Anp::TimeSeqSort::operator()(const Anp::Handle<Anp::TrigTimeSeq> &lhs,
				  const Anp::Handle<Anp::TrigTimeSeq> &rhs)
{
  //
  // Compare two objects based on key
  //
  if(!lhs.valid() || !rhs.valid()) { 
    cerr << "TimeSeqSort::operator() - lhs.ptr=" << lhs.get() << ", " << "rhs.ptr=" << rhs.get() << endl;
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
