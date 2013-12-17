/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <iostream>
#include <sstream>

// ROOT
#include "TH1.h"
#include "TH2.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/PlotEvent.h"

REGISTER_ANP_OBJECT(AlgEvent,PlotEvent)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
Anp::PlotEvent::PlotEvent()
  :fDir(0),
   fDirName("event"),
   fDirHist(),
   fDebug(false)
{
}

//-----------------------------------------------------------------------------
Anp::PlotEvent::~PlotEvent()
{
}

//-----------------------------------------------------------------------------
void Anp::PlotEvent::Config(const Registry &reg)
{
  reg.Get("PlotEvent", "Debug", fDebug);
  reg.Get("PlotEvent::DirName", fDirName);
  reg.Get("PlotEvent::DirHist", fDirHist);

  //
  // Book tree branches before Init()
  //
  fEntry = NtupleSvc::Instance("event") .Branch<TrigMonEvent> ("data");
}

//-----------------------------------------------------------------------------
void Anp::PlotEvent::Save(TDirectory *dir)
{
  fDir = Anp::GetDir(dir, fDirName);

  if(fDir) cout << "PlotEvent::Save - created TDirectory "  << fDirName << endl;
  else     cout << "PlotEvent::Save - null TDirectory ptr " << fDirName << endl;
}

//-----------------------------------------------------------------------------
void Anp::PlotEvent::Exec()
{
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
void Anp::PlotEvent::Run(const TrigMonEvent &event)
{
  //
  // Iterate over all variables
  //
  const std::vector<TrigMonVar> vec = event.getVar();
  
  if(fDebug) {
    cout << "PlotEvent::Run - TrigMonVar vector size: " << vec.size() << endl
	 << "  key size " << event.getVarKey().size() << endl
	 << "  var size " << event.getVarVal().size() << endl;
  }
  
  for(unsigned int i = 0; i < vec.size(); ++i) {    
    const TrigMonVar &var = vec.at(i);
    TH1 *h = FindTH1(var.getKey());
    if(h) {
      h -> Fill(var.getData());
    }
    else {
      if(fDebug) cout << "PlotEvent::Run - missing key: " << var.getKey() << endl;
    }

    if(fDebug) {
      cout << "PlotEvent::Run - " << vec[i].getKey() << ": " << vec[i].getData() << endl;
    }
  }

  // compute time for L2 and EF
  double time_seq_L2 = 0., time_seq_EF = 0.;
  double time_alg_L2 = 0., time_alg_EF = 0.;

  for(vector<TrigMonSeq>::const_iterator it = event.begin<TrigMonSeq>();
      it != event.end<TrigMonSeq>(); ++it) {
    if(it->getLevel() == 2) {
      time_seq_L2 += it->getSeqTimer();
      time_alg_L2 += it->getAlgTimer();
    }
    if(it->getLevel() == 3) {
      time_seq_EF += it->getSeqTimer();
      time_alg_EF += it->getAlgTimer();
    }
  }

  //
  // Fill L2 times
  //
  TH1 *hseq_L2 = FindTH1("time_L2_seq"); 
  TH1 *halg_L2 = FindTH1("time_L2_alg"); 

  if(hseq_L2) hseq_L2 -> Fill(time_seq_L2);
  if(halg_L2) halg_L2 -> Fill(time_alg_L2);

  //
  // Fill EF times
  //
  TH1 *hseq_EF = FindTH1("time_EF_seq");
  TH1 *halg_EF = FindTH1("time_EF_alg"); 

  if(hseq_EF) hseq_EF -> Fill(time_seq_EF);
  if(halg_EF) halg_EF -> Fill(time_alg_EF);

  //
  // Fill number of TrigMonChain and TrigMonL1Item objects
  //
  TH1 *hmon_HLT = FindTH1("count_mon_chains");
  TH1 *hmon_LV1 = FindTH1("count_mon_items");
  
  if(hmon_HLT) hmon_HLT -> Fill(event.getChain().size());
  if(hmon_LV1) hmon_LV1 -> Fill(event.getL1Item().size());
}

//-----------------------------------------------------------------------------
void Anp::PlotEvent::Plot(const unsigned int key, const float var)
{
  TH1 *h = FindTH1(key);
  if(h) {
    h -> Fill(var);
  }
}

//-----------------------------------------------------------------------------
void Anp::PlotEvent::Plot(const std::string &key, const float var, const std::string &hname)
{
  TH1 *h = FindTH1(key, hname);
  if(h) {
    h -> Fill(var);
  }
}

//-----------------------------------------------------------------------------
TH1* Anp::PlotEvent::GetKeyHist(uint32_t key)
{
  KeyMap::iterator kit = fKeyHist.find(key);
  if(kit != fKeyHist.end()) {
    return kit->second;
  }
  
  return 0;
}

//-----------------------------------------------------------------------------
TH1* Anp::PlotEvent::GetVarHist(const std::string &key)
{
  VarMap::iterator kit = fVarHist.find(key);
  if(kit != fVarHist.end()) {
    return kit->second;
  }
  
  return 0;
}

//-----------------------------------------------------------------------------
TH1* Anp::PlotEvent::FindTH1(const unsigned int hid)
{
  // Search for histogram
  KeyMap::iterator hit = fKeyHist.find(hid);
  if(hit != fKeyHist.end()) {
    return hit -> second;
  }

  TH1 *h = HistMan::Instance().CreateTH1(hid, fDirHist);
  SetDir(h, fDir);
  
  if(fDebug) {
    if(h) cout << "PlotEvent::FindTH1 - created  " << hid << endl;
    else  cout << "PlotEvent::FindTH1 - null ptr " << hid << endl;
  }

  hit = fKeyHist.insert(KeyMap::value_type(hid, h)).first;
  return hit -> second;
}

//-----------------------------------------------------------------------------
TH1* Anp::PlotEvent::FindTH1(const std::string &key, const std::string &hname)
{
  // Search for histogram
  VarMap::iterator hit = fVarHist.find(key);
  if(hit != fVarHist.end()) {
    return hit -> second;
  }

  TH1 *h = 0;
  if(hname.empty()) h = HistMan::Instance().CreateTH1(key,   fDirHist);
  else              h = HistMan::Instance().CreateTH1(hname, fDirHist);

  SetDir(h, fDir, key);  

  hit = fVarHist.insert(VarMap::value_type(key, h)).first;
  return hit -> second;
}
