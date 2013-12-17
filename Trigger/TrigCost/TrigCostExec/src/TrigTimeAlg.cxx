/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>

// ROOT
#include "TCanvas.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
#include "TImageDump.h"

// Local
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostExec/RobService.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostExec/TrigTimeAlg.h"

using namespace std;

//--------------------------------------------------------------------------------------      
// Start Event sub class
//
Anp::TrigTimeAlg::Event::Event()
  :n_event(0),
   n_alg_calls(0),
   n_alg_cache(0),
   n_alg_timeout(0),
   timer_1st(0.0),
   timer_2nd(0.0),
   timer_sum(0.0),
   timer_sum2(0.0)
{
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Event::AddEvent(const Event &rhs)
{
  n_event       += rhs.n_event;
  n_alg_calls   += rhs.n_alg_calls;
  n_alg_cache   += rhs.n_alg_cache;
  n_alg_timeout += rhs.n_alg_timeout;
  
  timer_sum     += rhs.timer_sum;
  timer_sum2    += rhs.timer_sum2;

  rob_reques.AddOneROB(rhs.rob_reques);
  rob_active.AddOneROB(rhs.rob_active);
  rob_cached.AddOneROB(rhs.rob_cached);
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Event::Clear()
{  
  // Clear variables
  n_event       = 0;
  n_alg_calls   = 0;
  n_alg_cache   = 0;
  n_alg_timeout = 0;
  timer_1st     = 0.0;
  timer_2nd     = 0.0;
  timer_sum     = 0.0;
  timer_sum2    = 0.0;

  rob_reques.Clear();
  rob_active.Clear();
  rob_cached.Clear();
  rois.clear();
}

//--------------------------------------------------------------------------------------      
// Start Hists sub class
//
Anp::TrigTimeAlg::Hists::Hists()
  :isConfig(false),
   hAlgTime(0),
   hCpuTime(0),
   hROBTime(0),
   hROBNreq(0),
   hROBSize(0),
   hROBSizeRetrv(0),
   hROBSizeCache(0),
   hROBNids(0),
   hROBNidsRetrv(0),
   hROBNidsCache(0),
   hAlgTeta(0),
   hAlgTphi(0),
   hROBRatioCache(0),
   hROBRatioRetrv(0),
   hNROBRatioCache(0),
   hNROBRatioRetrv(0),
   hAlgTimeDyn(0),
   hCpuTimeDyn(0),
   hROBTimeDyn(0)
{
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Hists::Set(TDirectory *dir, const Registry &reg)
{
  if(!isConfig) {
    isConfig = true;
  }
  else {
    cerr << "TrigTimeAlg::Hists::Set - error: executing 2nd time" << endl;
    return;
  }

  std::string prefix, opts;
  bool save_dir = true;
  reg.Get("TrigTimeAlg::Hists::Prefix", prefix);
  reg.Get("TrigTimeAlg::Hists::Opts",   opts);
  reg.Get("TrigTimeAlg::Hists::Save",   save_dir);

  hAlgTime = Anp::HistMan::Instance().CreateTH1(prefix+"_alg_time", "time_alg");
  hCpuTime = Anp::HistMan::Instance().CreateTH1(prefix+"_cpu_time", "time_alg");
  hROBTime = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_time", "time_alg");
  hROBNreq = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_nreq", "time_alg");
  hROBNids = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_nids", "time_alg");
  hROBSize = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_size", "time_alg");

  hROBNidsRetrv = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_nids", "time_alg");
  hROBNidsCache = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_nids", "time_alg");

  hROBSizeRetrv = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_size", "time_alg");
  hROBSizeCache = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_size", "time_alg");

  hAlgTeta = Anp::HistMan::Instance().CreateTH1(prefix+"_alg_time_vs_eta",  "time_algprofile");
  hAlgTphi = Anp::HistMan::Instance().CreateTH1(prefix+"_alg_time_vs_phi",  "time_algprofile");

  hROBRatioCache  = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_ratio_cache", "time_alg");
  hROBRatioRetrv  = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_ratio_retrv", "time_alg");

  hNROBRatioCache = Anp::HistMan::Instance().CreateTH1(prefix+"_nrob_ratio_cache", "time_alg");
  hNROBRatioRetrv = Anp::HistMan::Instance().CreateTH1(prefix+"_nrob_ratio_retrv", "time_alg");

  //
  // Rename some histograms
  //
  if(hROBNidsRetrv) hROBNidsRetrv->SetName((string(hROBNidsRetrv->GetName())+"_retrv").c_str());
  if(hROBNidsCache) hROBNidsCache->SetName((string(hROBNidsCache->GetName())+"_cache").c_str());

  if(hROBSizeRetrv) hROBSizeRetrv->SetName((string(hROBSizeRetrv->GetName())+"_retrv").c_str());
  if(hROBSizeCache) hROBSizeCache->SetName((string(hROBSizeCache->GetName())+"_cache").c_str());

  //
  // Create histogram with dynamic X axis binning
  //
  Registry hreg;
  hreg.Set("TimeHist::NBins",        100);
  hreg.Set("TimeHist::NWait",        1000);
  hreg.Set("TimeHist::PadUpperFrac", 2.0);
  hreg.Set("TimeHist::IgnUpperFrac", 0.05);
  hreg.Set("TimeHist::LowerEdge",    0.0);
  hreg.Set("TimeHist::UpperEdge",    10.0);

  hAlgTimeDyn = new TimeHist(prefix+"_alg_time");
  hCpuTimeDyn = new TimeHist(prefix+"_cpu_time");
  hROBTimeDyn = new TimeHist(prefix+"_rob_time");

  hAlgTimeDyn -> Config(hreg);
  hCpuTimeDyn -> Config(hreg);
  hROBTimeDyn -> Config(hreg);
  
  if(opts.find("rebin") != string::npos) {
    if(hAlgTime) hAlgTime->SetBit(TH1::kCanRebin);
    if(hCpuTime) hCpuTime->SetBit(TH1::kCanRebin);
    if(hROBTime) hROBTime->SetBit(TH1::kCanRebin);
  }

  if(save_dir) {
    Anp::SetDir(hAlgTime, dir);
    Anp::SetDir(hCpuTime, dir);
    Anp::SetDir(hROBTime, dir);
    Anp::SetDir(hROBNreq, dir);
    Anp::SetDir(hROBNids, dir);
    Anp::SetDir(hROBSize, dir);
    Anp::SetDir(hAlgTeta, dir);
    Anp::SetDir(hAlgTphi, dir);

    Anp::SetDir(hROBNidsRetrv, dir);
    Anp::SetDir(hROBNidsCache, dir);

    Anp::SetDir(hROBSizeRetrv, dir);
    Anp::SetDir(hROBSizeCache, dir);
    
    Anp::SetDir(hROBRatioCache, dir);
    Anp::SetDir(hROBRatioRetrv, dir);

    Anp::SetDir(hNROBRatioCache,  dir);
    Anp::SetDir(hNROBRatioRetrv, dir);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Hists::FillAllHist(const ROBEntry &rob,
					  unsigned /*run*/,
					  unsigned /*lumi*/,
					  double alg_time,
					  float eta, 
					  float phi)
{
  //
  // Fill event information
  // 
  double cpu_time = alg_time-rob.GetTime();
  if(cpu_time < 0.0) {
    cpu_time = 0.0;
  }

  if(hAlgTime) hAlgTime->Fill(alg_time);
  if(hCpuTime) hCpuTime->Fill(cpu_time);
  if(hROBTime) hROBTime->Fill(rob.GetTime());
  if(hROBNreq) hROBNreq->Fill(rob.GetNReq());
  
  const double size_total = rob.GetSize();
  const double size_retrv = rob.GetSize(TrigMonROBData::kRETRIEVED);
  const double size_cache = rob.GetSize(TrigMonROBData::kCACHED);

  if(hROBSize)      hROBSize     ->Fill(size_total);
  if(hROBSizeRetrv) hROBSizeRetrv->Fill(size_retrv);
  if(hROBSizeCache) hROBSizeCache->Fill(size_cache);


  const double nrob_total = rob.GetNROB();
  const double nrob_retrv = rob.GetNROB(TrigMonROBData::kRETRIEVED);
  const double nrob_cache = rob.GetNROB(TrigMonROBData::kCACHED);

  if(hROBNids)      hROBNids     ->Fill(nrob_total);
  if(hROBNidsRetrv) hROBNidsRetrv->Fill(nrob_retrv);
  if(hROBNidsCache) hROBNidsCache->Fill(nrob_cache);

  if(std::fabs(eta) < 10.0) {
    if(hAlgTeta) hAlgTeta->Fill(eta, alg_time);
  }

  if(std::fabs(phi) < 10.0) {
    if(hAlgTphi) hAlgTphi->Fill(phi, alg_time);
  }

  if(hAlgTimeDyn) hAlgTimeDyn->Fill(alg_time);
  if(hCpuTimeDyn) hCpuTimeDyn->Fill(cpu_time);
  if(hROBTimeDyn) hROBTimeDyn->Fill(rob.GetTime());

  //
  // Fill ROB ratio histograms
  //
  if(hROBRatioRetrv && size_total > 0.0) hROBRatioRetrv -> Fill(size_retrv/size_total);
  if(hROBRatioCache && size_total > 0.0) hROBRatioCache -> Fill(size_cache/size_total);

  if(hNROBRatioRetrv && nrob_total > 0.0) hNROBRatioRetrv -> Fill(nrob_retrv/nrob_total);
  if(hNROBRatioCache && nrob_total > 0.0) hNROBRatioCache -> Fill(nrob_cache/nrob_total);
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Hists::FillROBHist(const ROBEntry &)
{  
  //
  // Fill one ROB request/call: fill ratio histograms
  //
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Hists::Finalize()
{
  if(hAlgTimeDyn) hAlgTimeDyn -> Finalize();
  if(hCpuTimeDyn) hCpuTimeDyn -> Finalize();
  if(hROBTimeDyn) hROBTimeDyn -> Finalize();
}

//--------------------------------------------------------------------------------------      
TH1 *Anp::TrigTimeAlg::Hists::GetTH1(Type type) const
{
  if(type == AlgTimeDyn && hAlgTimeDyn) return hAlgTimeDyn->GetTH1();
  if(type == CpuTimeDyn && hCpuTimeDyn) return hCpuTimeDyn->GetTH1();
  if(type == ROBTimeDyn && hROBTimeDyn) return hROBTimeDyn->GetTH1();

  return 0;
}

//--------------------------------------------------------------------------------------      
// Start TrigTimeAlg class
//
Anp::TrigTimeAlg::TrigTimeAlg()
  :fDir(0),
   fDebug(true),
   fCanRebin(true),
   fMakeHists(true),
   fUseType(false),
   fTimeout(0.0),
   fIsConfig(false),
   fFillROBIds(false),
   fLevel(0),
   hEvtFract(0),
   hCacheROBNreq(0),
   hCacheROBSize(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::TrigTimeAlg::~TrigTimeAlg()
{
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Config(const Registry &reg)
{

  if(!fIsConfig) {
    fIsConfig = true;
  }
  else {
    cout << "TrigTimeAlg::Config - error: executing second time" << endl;    
  }

  reg.Get("TrigTimeAlg", "Debug",      fDebug);
  reg.Get("TrigTimeAlg", "CanvasType", fCanvasType);

  unsigned SlowNEvent = 10;
  reg.Get("TrigTimeAlg::HistOpts",     fHistOpts);
  reg.Get("TrigTimeAlg::CanRebin",     fCanRebin);
  reg.Get("TrigTimeAlg::MakeHists",    fMakeHists);
  reg.Get("TrigTimeAlg::UseType",      fUseType);
  reg.Get("TrigTimeAlg::Timeout",      fTimeout);
  reg.Get("TrigTimeAlg::Prefix",       fPrefix);
  reg.Get("TrigTimeAlg::SlowNEvent",   SlowNEvent);

  fSlow.SetNEvent(SlowNEvent);

  //
  // Create histograms
  //
  if(fMakeHists) {
    hEvtFract     = HistMan::Instance().CreateTH1("alg_over_evt",   "time_alg");
    hCacheROBNreq = HistMan::Instance().CreateTH1("cache_rob_nreq", "time_alg");
    hCacheROBSize = HistMan::Instance().CreateTH1("cache_rob_size", "time_alg");
    
    Registry hreg(reg);
    hreg.Set("TrigTimeAlg::Hists::Opts", fHistOpts);

    hreg.Set("TrigTimeAlg::Hists::Prefix", "evt");
    fHistEvent.Set(fDir, hreg);

    hreg.Set("TrigTimeAlg::Hists::Prefix", "roi");
    fHistCalls.Set(fDir, hreg);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Config(const TrigConfAlg &alg)
{
  //
  // Configure self
  //
  if(!fAlgVec.empty() && GetTypeId() != alg.getTypeId()) {
    cerr << "TrigTimeAlg::Config - error! Wrong algorithm type: " << alg.getType() << endl;
    return;
  }

  fAlgVec.push_back(alg);
  fIds.insert(alg.getNameId());
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::AddAlgEntry(const TrigMonSeq &seq,
				   const TrigMonAlg &alg,
				   const TrigConfSeq &conf_seq,
				   const TrigConfAlg &conf_alg,
				   const float eta, const float phi)
{
  //
  // Add single algorithm measurement for matching algorithm
  //
  if(fIds.count(conf_alg.getNameId()) != 1) {
    cerr << "TrigTimeAlg::AddAlgEntry input ALG error!" << endl;
    return;
  }

  if(fDebug && !fUseType) {
    cout << "ALG:" << GetName() << " called: " << alg.isCalled()
	 << ": eta, phi " << eta << ", " << phi << endl;
  }

  fEvent.n_alg_calls += alg.isCalled();
  fEvent.n_alg_cache += alg.isCached();

  if(alg.isCalled() && alg.getTimer() > fTimeout) ++fEvent.n_alg_timeout;

  //
  // Save times for 1st and 2nd algorithm calls
  //
  if     (fEvent.n_alg_calls == 1) fEvent.timer_1st = alg.getTimer();
  else if(fEvent.n_alg_calls == 2) fEvent.timer_2nd = alg.getTimer();
    
  //
  // Add timers and count algorithms
  //
  fEvent.timer_sum  += alg.getTimer();
  fEvent.timer_sum2 += alg.getTimer()*alg.getTimer();
  
  Count &count_chn = fCountChn[seq.getChnEncodedId()];
  Count &count_seq = fCountSeq[conf_seq.getId()];

  count_chn.ncalls += alg.isCalled();
  count_chn.ncache += alg.isCached();

  count_seq.ncalls += alg.isCalled();
  count_seq.ncache += alg.isCached();
    
  if(alg.isCalled()) {
    fEvent.rois.push_back(Anp::AlgEntry(alg, eta, phi));
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::AddRobEntry(const TrigMonROB &rob)
{
  //
  // Fill data for one rob request (single call to DataCollector)
  //
  if(fIds.count(rob.getRequestorId()) != 1) {
    cerr << "TrigTimeAlg::AddRobEntry - input ROB error!" << endl;
    return;
  }

  if(fEvent.n_alg_calls < 1) {
    cerr << "TrigTimeAlg::AddRobEntry - missing alg data: " << GetName() << endl;
  }

  const std::vector<TrigMonROBData> &rvec = rob.getData();
  const std::vector<TrigMonROBSum>   svec = rob.getSum();

  std::set<TrigMonROBData::History> hists;

  const RobService &rob_svc = RobService::Instance();
  
  //
  // Fill data from individual ROB ids
  //
  set<uint32_t> det_set, det_set_ret;
  set<uint32_t> ros_set, ros_set_ret;

  for(unsigned int i = 0; i < rvec.size(); ++i) {
    const TrigMonROBData &data = rvec.at(i);
    const uint32_t subdet = rob_svc.GetSubDetId(data.getROBId());
    const int      roshid = rob_svc.GetRosId   (data.getROBId());

    hists.insert(data.getHistory());    

    CountDetMap::iterator sit = 
      fSubDet.insert(CountDetMap::value_type(subdet, CountDet())).first;

    CountROSMap::iterator rit = 
      sit->second.rosmap.insert(CountROSMap::value_type(roshid, CountROB())).first;
    
    if(data.getHistory() == TrigMonROBData::kRETRIEVED) {
      sit->second.subdet.size_ret += data.getROBSize();
      rit->second.size_ret        += data.getROBSize();

      if(det_set_ret.insert(subdet).second) {
	sit->second.subdet.nreq_ret++;
      }
      if(ros_set_ret.insert(roshid).second) {
	rit->second.nreq_ret++;
      }
    }
    else if(data.getHistory() == TrigMonROBData::kCACHED) {
      sit->second.subdet.size_chd += data.getROBSize();
      rit->second.size_chd        += data.getROBSize();
    }

    if(det_set.insert(subdet).second) {
      sit->second.subdet.nreq++;
    }
    if(ros_set.insert(roshid).second) {
      rit->second.nreq++;
    }
  }

  //
  // Fill data from ROB summaries: can not fill ROS data - requires full ROB id
  //
  for(unsigned int i = 0; i < svec.size(); ++i) {
    const TrigMonROBSum &data = svec.at(i);
    const uint32_t subdet = data.getSubDet();

    hists.insert(data.getHistory());

    CountDetMap::iterator sit = 
      fSubDet.insert(CountDetMap::value_type(subdet, CountDet())).first;

    if(data.getHistory() == TrigMonROBData::kRETRIEVED) {
      sit->second.subdet.size_ret += data.getSize();
    }
    else if(data.getHistory() == TrigMonROBData::kCACHED) {
      sit->second.subdet.size_chd += data.getSize();
    }
  }

  //
  // Count all ROB requests
  //
  fEvent.rob_reques.AddMonROB(rob);

  if(hists.count(TrigMonROBData::kCACHED) == 1) {
    //
    // This ROB request was fully cached
    //
    fEvent.rob_cached.AddMonROB(rob);
  }
  else {
    //
    // This ROB request contains non-cached ROB ids
    //
    fEvent.rob_active.AddMonROB(rob);
  }

  unsigned match_roi = 0;
  for(unsigned i = 0; i < fEvent.rois.size(); ++i) {
    if(fEvent.rois[i].FillRoiROB(rob)) ++match_roi;
  }
  
  if(match_roi != 1) {
    cerr << "TrigTimeAlg::AddRobEntry - bad number of matching RoI: " 
	 << match_roi << " for algorithm: " << GetName() << endl;
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::ResetEvent(const EventTime &event, unsigned level)
{
  //
  // Reset event - inputs: event number, vector of timers arranged by event numbers
  //  
  if(!fUseType && fEvent.n_alg_calls == 0 && fEvent.n_alg_cache > 0) {
    cerr << "TrigTimeAlg::ResetEvent - cached but not called " << GetName() << endl;
    return;
  }
  
  // Algorithm is never called - reset event number and ignore this event
  if(fEvent.n_alg_calls == 0 && fEvent.n_alg_cache == 0) {
    return;
  }

  // Increase event count
  ++fEvent.n_event;

  //
  // Fill histograms
  //
  if(hEvtFract && event.event_timer > 0.0) {
    hEvtFract -> Fill(fEvent.timer_sum/event.event_timer);
  }
    
  for(unsigned i = 0; i < fEvent.rois.size(); ++i) {
    const AlgEntry &roi = fEvent.rois.at(i);
    fHistCalls.FillAllHist(roi.GetROB(), event.run, event.lumi, roi.GetAlgTime(), roi.eta, roi.phi);
  }
  
  fHistEvent.FillAllHist(fEvent.rob_reques, event.run, event.lumi, fEvent.timer_sum);
  
  if(fEvent.rob_cached.GetNReq() > 0) {
    if(hCacheROBNreq) { 
      hCacheROBNreq->Fill(fEvent.rob_cached.GetNReq());
      if(fDebug) cout << "hCacheROBNreq->Fill(fEvent.rob_cached.GetNReq())" << endl;
    }
    if(hCacheROBSize) { 
      hCacheROBSize->Fill(fEvent.rob_cached.GetSize());
      if(fDebug) cout << "hCacheROBSize->Fill(fEvent.rob_cached.GetSize());" << endl;
    }
  }

  //
  // Save N slowest events
  //
  fSlow.AddSlowEvent(event, fEvent.timer_sum);

  //
  // Add current event and RESET event data
  //
  fTotal.AddEvent(fEvent);
  fEvent.Clear();

  // Check level
  if(fLevel == 0) {
    fLevel = level;
  }
  else if(fLevel != level) {
    cerr << "TrigTimeAlg::ResetEvent - wrong level: " << level << ", " << GetName() << endl;
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeAlg::Finalize()
{
  //
  // Compute and save variables
  //
  if(fTotal.n_event < 1 || fTotal.n_alg_calls < 1) {
    return;
  }

  fVarMap["timer_sum"]          = fTotal.timer_sum;
  fVarMap["timer_call_mean"]    = fTotal.timer_sum/double(fTotal.n_alg_calls);
  fVarMap["timer_call_rms"]     = std::sqrt(fTotal.timer_sum2/double(fTotal.n_alg_calls));
  fVarMap["timer_event_mean"]   = fTotal.timer_sum/double(fTotal.n_event);
  fVarMap["timeout_limit"]      = fTimeout;
  fVarMap["ncalls"]             = fTotal.n_alg_calls;
  fVarMap["ncache"]             = fTotal.n_alg_cache;
  fVarMap["ntimeout"]           = fTotal.n_alg_timeout;
  fVarMap["nevent"]             = fTotal.n_event;
  fVarMap["ncalls_per_event"]   = fTotal.n_alg_calls/double(fTotal.n_event);
  fVarMap["ncache_per_event"]   = fTotal.n_alg_cache/double(fTotal.n_event);

  fVarMap["rob_time"]           = fTotal.rob_reques.GetTime();
  fVarMap["rob_nreq"]           = fTotal.rob_reques.GetNReq();
  fVarMap["rob_nreq_active"]    = fTotal.rob_active.GetNReq();
  fVarMap["rob_nreq_cached"]    = fTotal.rob_cached.GetNReq();

  fVarMap["rob_nrob"]           = fTotal.rob_reques.GetNROB();
  fVarMap["rob_nrob_retrv"]     = fTotal.rob_reques.GetNROB(TrigMonROBData::kRETRIEVED);
  fVarMap["rob_nrob_cache"]     = fTotal.rob_reques.GetNROB(TrigMonROBData::kCACHED);
  fVarMap["rob_nrob_other"]     = fTotal.rob_reques.GetNROBOther();

  fVarMap["rob_size"]           = fTotal.rob_reques.GetSize();
  fVarMap["rob_size_retrv"]     = fTotal.rob_reques.GetSize(TrigMonROBData::kRETRIEVED);
  fVarMap["rob_size_cache"]     = fTotal.rob_reques.GetSize(TrigMonROBData::kCACHED);
  fVarMap["rob_size_other"]     = fTotal.rob_reques.GetSizeOther();

  fVarMap["rob_time_per_event"] = fTotal.rob_reques.GetTime()/double(fTotal.n_event);
  fVarMap["rob_nreq_per_event"] = fTotal.rob_reques.GetNReq()/double(fTotal.n_event);
  fVarMap["rob_size_per_event"] = fTotal.rob_reques.GetSize()/double(fTotal.n_event);

  uint64_t ros_nreq = 0, ros_nret = 0;
  GetROSCount(ros_nreq, ros_nret);

  fVarMap["ros_nreq"]           = ros_nreq;
  fVarMap["ros_nreq_ret"]       = ros_nret;

  fHistEvent.Finalize();
  fHistCalls.Finalize();
}

//--------------------------------------------------------------------------------------      
unsigned Anp::TrigTimeAlg::GetNameId() const
{
  //
  // Get algorithm name hash id
  //
  if(!fAlgVec.empty()) return fAlgVec.front().getNameId();
  return 0;
}

//--------------------------------------------------------------------------------------      
unsigned Anp::TrigTimeAlg::GetTypeId() const
{
  //
  // Get algorithm name hash id
  //
  if(!fAlgVec.empty()) return fAlgVec.front().getTypeId();
  return 0;
}

//--------------------------------------------------------------------------------------      
const std::string Anp::TrigTimeAlg::GetName() const
{
  //
  // Get algorithm name hash id
  //
  if(!fAlgVec.empty()) { 
    if(fUseType) return fAlgVec.front().getType();
    else         return fAlgVec.front().getName();
  }

  return "UNKNOWN";
}

//---------------------------------------------------------------------------------------
double Anp::TrigTimeAlg::GetVar(const std::string &key) const
{
  //
  // Find variable
  //

  VarMap::const_iterator vit = fVarMap.find(key);
  if(vit != fVarMap.end()) {
    return vit -> second;
  } 

  return 0.0;
}

//---------------------------------------------------------------------------------------
std::string Anp::TrigTimeAlg::GetVarTitle(const std::string &key) const
{
  //
  // Find variable name
  //
  if(key == "ncalls")           return "Number of algorithm calls";
  if(key == "ncalls_per_event") return "Number of algorithm calls per event";
  if(key == "nevent")           return "Number of events";
  if(key == "timer_call_mean")  return "Mean time per call (ms)";
  if(key == "timer_event_mean") return "Mean time per event (ms)";
  if(key == "timer_sum")        return "Total time (ms)";
  
  return string();
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeAlg::PrintVar(std::ostream &os,
				const std::string &key,
				const std::string &prf,
				const std::string &suf) const			
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
void Anp::TrigTimeAlg::WriteROSCount(HtmlNode &node, 
				     const CountDetMap &dmap, 
				     const string &) const
{
  //
  // Count individual roses
  //
  set<uint32_t> roses;
    
  for(CountDetMap::const_iterator dit = dmap.begin(); dit != dmap.end(); ++dit) {
    const CountROSMap &rmap = dit->second.rosmap;

    for(CountROSMap::const_iterator rit = rmap.begin(); rit != rmap.end(); ++rit) {
      if(!roses.insert(rit->first).second) {
	cout << "TrigTimeAlg::WriteROSCount - duplicate ROS id: " << rit->first << endl;
      }
    }
  }

  node.GetCont() 
    << GetName() << " requested data from " << roses.size() << " ROS(es): " << endl << endl;

  PrintVar(node.GetCont(), "nevent",   "  Processed events:              ", "");
  PrintVar(node.GetCont(), "ncalls",   "  Active algorithm calls:        ", "");
  PrintVar(node.GetCont(), "ncache",   "  Cached algorithm calls:        ", "");
  PrintVar(node.GetCont(), "rob_nreq", "  Total ROB DataCollector calls: ", "");

  //
  // Write html table
  //
  node.GetCont()
    << "<style type=\"text/css\">"<< endl
    << "table,  th {border:1px solid black;}"<< endl
    << "th { padding:10px; background-color:white; color:black; }"<< endl
    << "td {padding-left:15px;padding-right:15px;}"<< endl
    << "</style>"<< endl
    << "<script src=\"sorttable.js\"></script>" << endl
    << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
    << "<tr>"
    << "<th> ROS </th>"
    << "<th> Sub-detector </th>"
    << "<th> Data requests </th>"
    << "<th> Data requests/sub-detector requests</th>"
    << "<th> Requested data (kB) </th>"
    << "<th> Retrieved data (kB) </th>"
    << "<th> Cached data (kB) </th>"
    << "</tr>" 
    << endl;

  const RobService &rob_svc = RobService::Instance();
 
  double   total_size_req = 0.0, total_size_ret = 0.0, total_size_chd = 0.0;
  uint64_t total_nreq = 0;

  for(CountDetMap::const_iterator dit = dmap.begin(); dit != dmap.end(); ++dit) {
    //
    // Sub-detector counts
    //
    const string   det_name = rob_svc.GetSubDetFromId(dit->first);
    const uint64_t det_nreq = dit->second.subdet.nreq;
    
    const CountROSMap &rmap = dit->second.rosmap;

    for(CountROSMap::const_iterator rit = rmap.begin(); rit != rmap.end(); ++rit) {
      //
      // ROS counts
      //
      const string ros_name = rob_svc.GetRosFromId(rit->first);

      const uint64_t ros_size_ret = rit->second.size_ret;
      const uint64_t ros_size_chd = rit->second.size_chd;
      const uint64_t ros_size_req = ros_size_ret + ros_size_chd;
      const uint64_t ros_nreq     = rit->second.nreq;


      double rat_nreq = 0.0;
      if(det_nreq > 0) {
	rat_nreq = double(ros_nreq)/double(det_nreq);
      }

      node.GetCont() 
	<< "<tr>" 
	<< "<td align=\"left\">"  << ros_name << "</td>"
	<< "<td align=\"left\">"  << det_name << "</td>"
	<< "<td align=\"right\">" << ros_nreq << "</td>"
	<< "<td align=\"right\">" << rat_nreq << "</td>"
	<< "<td align=\"right\">" << Anp::Round2Pair(ConvertWord2kB(ros_size_req), 1.0).first << "</td>"
	<< "<td align=\"right\">" << Anp::Round2Pair(ConvertWord2kB(ros_size_ret), 1.0).first << "</td>"
	<< "<td align=\"right\">" << Anp::Round2Pair(ConvertWord2kB(ros_size_chd), 1.0).first << "</td>"
	<< endl;

      total_nreq     += ros_nreq;
      total_size_req += ConvertWord2kB(ros_size_req);
      total_size_ret += ConvertWord2kB(ros_size_ret);
      total_size_chd += ConvertWord2kB(ros_size_chd);
    }
  }

  node.GetCont() 
      << "<tr>" 
      << "<td align=\"left\">Total</td>"
      << "<td align=\"left\"></td>"
      << "<td align=\"right\">" << total_nreq << "</td>"
      << "<td align=\"right\"></td>"
      << "<td align=\"right\">" << Anp::Round2Pair(total_size_req, 1.0).first << "</td>"
      << "<td align=\"right\">" << Anp::Round2Pair(total_size_ret, 1.0).first << "</td>"
      << "<td align=\"right\">" << Anp::Round2Pair(total_size_chd, 1.0).first << "</td>"
      << endl;
  
  node.GetCont() << "</table>" << endl;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeAlg::WriteDetCount(HtmlNode &node, 
				     const CountDetMap &cmap, 
				     const string &) const
{
  //
  // Write ROB size counts
  //
  node.GetCont() 
    << GetName() << " requested data from " << cmap.size() << " sub-detector(s): " << endl << endl;

  PrintVar(node.GetCont(), "nevent",   "  Processed events:              ", "");
  PrintVar(node.GetCont(), "ncalls",   "  Active algorithm calls:        ", "");
  PrintVar(node.GetCont(), "ncache",   "  Cached algorithm calls:        ", "");
  PrintVar(node.GetCont(), "rob_nreq", "  Total ROB DataCollector calls: ", "");

  //
  // Write html table
  //
  node.GetCont()
    << "<style type=\"text/css\">"<< endl
    << "table,  th {border:1px solid black;}"<< endl
    << "th { padding:10px; background-color:white; color:black; }"<< endl
    << "td {padding-left:15px;padding-right:15px;}"<< endl
    << "</style>"<< endl
    << "<script src=\"sorttable.js\"></script>" << endl
    << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
    << "<tr>"
    << "<th> Sub-detector </th>"
    << "<th> Requested data (kB) </th>"
    << "<th> Retrieved data (kB) </th>"
    << "<th> Cached data (kB) </th>"
    << "</tr>" 
    << endl;

  const RobService &rob_svc = RobService::Instance();   
  double total_req = 0.0, total_ret = 0.0, total_chd = 0.0;

  for(CountDetMap::const_iterator it = cmap.begin(); it != cmap.end(); ++it) {
    //
    // Convert size to kB and round to 0.1 kB
    //
    const uint64_t size_ret = it->second.subdet.size_ret;
    const uint64_t size_chd = it->second.subdet.size_chd;
    const uint64_t size_req = size_ret+size_chd;

    const string str_req = Anp::Round2Pair(ConvertWord2kB(size_req), 1.0).first;
    const string str_ret = Anp::Round2Pair(ConvertWord2kB(size_ret), 1.0).first;
    const string str_chd = Anp::Round2Pair(ConvertWord2kB(size_chd), 1.0).first;
    
    const string name = rob_svc.GetSubDetFromId(it->first);

    node.GetCont() 
      << "<tr>" 
      << "<td align=\"left\">"  << name    << "</td>"
      << "<td align=\"right\">" << str_req << "</td>"
      << "<td align=\"right\">" << str_ret << "</td>"
      << "<td align=\"right\">" << str_chd << "</td>"
      << endl; 

    total_req += ConvertWord2kB(size_req);
    total_ret += ConvertWord2kB(size_ret);
    total_chd += ConvertWord2kB(size_chd);
  }

  const string str_total_req = Anp::Round2Pair(total_req, 1.0).first;
  const string str_total_ret = Anp::Round2Pair(total_ret, 1.0).first;
  const string str_total_chd = Anp::Round2Pair(total_chd, 1.0).first;

  node.GetCont() 
      << "<tr>" 
      << "<td align=\"left\">Total</td>"
      << "<td align=\"right\">" << str_total_req << "</td>"
      << "<td align=\"right\">" << str_total_ret << "</td>"
      << "<td align=\"right\">" << str_total_chd << "</td>"
      << endl;
  
  node.GetCont() << "</table>" << endl;
}

//---------------------------------------------------------------------------------------
double Anp::TrigTimeAlg::ConvertWord2kB(unsigned nwords) const
{
  return 4.0*nwords/1024.0;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeAlg::WriteHtml(Handle<HtmlNode> node, TCanvas *canvas) //const
{
  //
  // Write html summary
  //
  if(!node.valid()) { 
    return;
  }
  
  ostream &os = node -> GetCont();

  if(node->GetBase().find("ALG_TYPE_3927684433") != string::npos) {
    cout << "TrigTimeAlg::WriteHtml - name: " << GetName() 
	 << " name id: " << GetNameId()
	 << " type id: " << GetTypeId()
	 << endl;
  }

  os << "Totals for all processed events: " << endl;

  PrintVar(os, "timer_sum",         "  Processing time:                ", " ms");
  PrintVar(os, "nevent",            "  Processed events:               ", "");
  PrintVar(os, "ncalls",            "  Active algorithm calls:         ", "");
  PrintVar(os, "ncache",            "  Cached algorithm calls:         ", "");
  
  if(fLevel == 2) {
    PrintVar(os, "rob_nreq",          "  Total ROB DataCollector calls:  ", "");
    PrintVar(os, "rob_nreq_active",   "    - active:                     ", "");
    PrintVar(os, "rob_nreq_cached",   "    - cached:                     ", "");
    PrintVar(os, "rob_size",          "  Size of requested ROBs:         ", " kBytes");
    PrintVar(os, "rob_size_retrv",    "    - retrieved:                  ", " kBytes");
    PrintVar(os, "rob_size_cache",    "    - cached:                     ", " kBytes");
    PrintVar(os, "rob_size_other",    "    - other:                      ", " kBytes (ignored, disabled or unclassified)");
  }
  os << endl;

  //
  // Print size of data requests by subdetector and ROS
  //
  bool write_new_endl = false;

  if(!fSubDet.empty()) {
    //
    // Write sub-detector counts
    //
    Handle<HtmlNode> nodeSD(new HtmlNode(node->GetBase()+"_subdet.html", HtmlNode::kHTML));
    node -> AddChild(nodeSD);
    nodeSD -> GetCont() << "<pre>" << endl;

    WriteDetCount(nodeSD.ref(), fSubDet);

    nodeSD -> GetCont() << "</pre>" << endl;
    os << nodeSD -> GetLink("Data requests per sub-detector") << endl;    
    write_new_endl = true;

    //
    // Write ROS counts
    //
    Handle<HtmlNode> nodeRS(new HtmlNode(node->GetBase()+"_rossum.html", HtmlNode::kHTML));
    node -> AddChild(nodeRS);
    nodeRS -> GetCont() << "<pre>" << endl;

    WriteROSCount(nodeRS.ref(), fSubDet);

    nodeRS -> GetCont() << "</pre>" << endl;
    os << nodeRS -> GetLink("Data requests per ROS") << endl;
    write_new_endl = true;
  }

  //
  // Write list of slowest events
  //
  if(fSlow.NSlowEvents() > 0) {
    Handle<HtmlNode> nodeSE(new HtmlNode(node->GetBase()+"_slwevt.html", HtmlNode::kHTML));
    node -> AddChild(nodeSE);
    
    stringstream slowLink;
    slowLink << "List of " << fSlow.NSlowEvents() << " slowest event(s)";
    os << nodeSE -> GetLink(slowLink.str()) << endl;
    
    fSlow.WriteSlow(nodeSE, fSlow.GetSlowEvents(), "events");
    write_new_endl = true;
  }

  if(write_new_endl) os << endl;

  os << "Mean quantities per event: " << endl;
  PrintVar(os, "timer_event_mean",   "  Processing time:         ", " ms");
  
  if(fLevel == 2) {
    PrintVar(os, "rob_nreq_per_event", "  ROB DataCollector calls: ", "");
    PrintVar(os, "rob_size_per_event", "  Requested ROB size:      ", " kBytes");
  }

  os << endl;

  //
  // Make histograms
  //
  if(fPrefix.empty() || (fCanvasType == "png" && fCanvasType == "eps")) {
    return;
  }
 
  os << "Histograms filled per RoI: " << endl;
  
  const string &prf = fPrefix;
  const string &can = fCanvasType;

  if(fHistOpts.find("dyn") != string::npos) {
    PrintHist(node.ref(), fHistCalls.GetTH1(Hists::AlgTimeDyn), canvas, prf+"_call_alg_time_dyn."+can,
	      "  Total processing time", fHistOpts);
    
    PrintHist(node.ref(), fHistCalls.GetTH1(Hists::CpuTimeDyn), canvas, prf+"_call_cpu_time_dyn."+can,
	      "  Algorithm CPU time", fHistOpts);
  }
  else {
    PrintHist(node.ref(), fHistCalls.hAlgTime, canvas, prf+"_call_alg_time."+can,
	      "  Total processing time", fHistOpts);

    PrintHist(node.ref(), fHistCalls.hCpuTime, canvas, prf+"_call_cpu_time."+can,
	      "  Algorithm CPU time", fHistOpts);
  }
  
  if(fLevel == 2) {
    if(fHistOpts.find("dyn") != string::npos) {
      PrintHist(node.ref(), fHistCalls.GetTH1(Hists::ROBTimeDyn), canvas, prf+"_call_rob_time_dyn."+can,
		"  ROB retrieval time", fHistOpts);
    }
    else {
      PrintHist(node.ref(), fHistCalls.hROBTime, canvas, prf+"_call_rob_time."+can,
		"  ROB retrieval time", fHistOpts);
    }
    
    PrintHist(node.ref(), fHistCalls.hROBNreq, canvas, prf+"_call_rob_nreq."+can,
	      "  Number of ROB DataCollector calls", fHistOpts);

    PrintHist(node.ref(), fHistCalls.hROBSize, canvas, prf+"_call_rob_size."+can,
	      "  Requested ROB size", fHistOpts);
    
    PrintHist(node.ref(), fHistCalls.hROBSizeRetrv, canvas, prf+"_call_rob_size_retrv."+can,
	      "  Retrieved ROB size", fHistOpts);

    PrintHist(node.ref(), fHistCalls.hROBSizeCache, canvas, prf+"_call_rob_size_cache."+can,
	      "  Cached    ROB size", fHistOpts);
  }
  os << endl;
  
  os << "Histograms filled per event: " << endl;
  
  if(fHistOpts.find("dyn") != string::npos) {
    PrintHist(node.ref(), fHistEvent.GetTH1(Hists::AlgTimeDyn), canvas, prf+"_evt_alg_time_dyn."+can,
	      "  Total processing time", fHistOpts);
    
    PrintHist(node.ref(), fHistEvent.GetTH1(Hists::CpuTimeDyn), canvas, prf+"_evt_cpu_time_dyn."+can,
	      "  Algorithm CPU time", fHistOpts);
  }
  else {
    PrintHist(node.ref(), fHistEvent.hAlgTime, canvas, prf+"_evt_alg_time."+can,
	      "  Total processing time", fHistOpts);
    
    PrintHist(node.ref(), fHistEvent.hCpuTime, canvas, prf+"_evt_cpu_time."+can,
	      "  Algorithm CPU time", fHistOpts);    
  }
  
  if(fLevel == 2) {
    if(fHistOpts.find("dyn") != string::npos) {
      PrintHist(node.ref(), fHistEvent.GetTH1(Hists::ROBTimeDyn), canvas, prf+"_evt_rob_time_dyn."+can,
		"  ROB retrieval time", fHistOpts);
    }
    else {
      PrintHist(node.ref(), fHistEvent.hROBTime, canvas, prf+"_evt_rob_time."+can,
		"  ROB retrieval time", fHistOpts);      
    }
    
    PrintHist(node.ref(), hEvtFract, canvas, prf+"_evt_time_frac."+can,
	      "  Processing time/Total event time");

    PrintHist(node.ref(), fHistEvent.hROBNreq, canvas, prf+"_evt_rob_nreq."+can,
	      "  Number of ROB DataCollector calls", fHistOpts);
    
    PrintHist(node.ref(), fHistEvent.hROBSize, canvas, prf+"evt_rob_size."+can,
	      "  Requested ROB size", fHistOpts);

    PrintHist(node.ref(), fHistEvent.hROBSizeRetrv, canvas, prf+"evt_rob_size_retrv."+can,
	      "  Retrieved ROB size", fHistOpts);

    PrintHist(node.ref(), fHistEvent.hROBSizeCache, canvas, prf+"evt_rob_size_cache."+can,
	      "  Cached    ROB size", fHistOpts);

    PrintHist(node.ref(), fHistEvent.hROBNids, canvas, prf+"evt_rob_nids."+can,
	      "  Requested number of ROBs", fHistOpts);

    PrintHist(node.ref(), fHistEvent.hROBNidsRetrv, canvas, prf+"evt_rob_nids_retrv."+can,
	      "  Retrieved number of ROBs", fHistOpts);

    PrintHist(node.ref(), fHistEvent.hROBNidsCache, canvas, prf+"evt_rob_nids_cache."+can,
	      "  Cached    number of ROBs", fHistOpts);
       
    
    PrintHist(node.ref(), fHistEvent.hROBRatioRetrv, canvas, prf+"_evt_rob_retrv_ratio_size."+can,
	      "  Size of retrieved ROBs/Size of requested ROBs", fHistOpts);
    
    PrintHist(node.ref(), fHistEvent.hROBRatioCache, canvas, prf+"_evt_rob_cache_ratio_size."+can,
	      "  Size of cached    ROBs/Size of requested ROBs");

    PrintHist(node.ref(), fHistEvent.hNROBRatioRetrv, canvas, prf+"_evt_nrob_retrv_ratio_size."+can,
	     "  Number of retrieved ROBs/Number of requested ROBs", fHistOpts);

    PrintHist(node.ref(), fHistEvent.hNROBRatioCache, canvas, prf+"_evt_nrob_cache_ratio_size."+can,
	     "  Number of    cached ROBs/Number of requested ROBs");
  }
  else {
    PrintHist(node.ref(), hEvtFract, canvas, prf+"_evt_time_frac."+can,
	      "  Processing time/Total event time");
  }
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeAlg::GetROSCount(uint64_t &nreq, uint64_t &nret) const
{
  //
  // Count individual roses
  //
  for(CountDetMap::const_iterator dit = fSubDet.begin(); dit != fSubDet.end(); ++dit) {
    //
    // Sub-detector counts
    //
    const CountROSMap &rmap = dit->second.rosmap;

    for(CountROSMap::const_iterator rit = rmap.begin(); rit != rmap.end(); ++rit) {
      //
      // ROS counts
      //
      nreq += rit->second.nreq;
      nret += rit->second.nreq_ret;
    }
  }
}

//---------------------------------------------------------------------------------------
bool Anp::TimeAlgSort::operator()(const Anp::Handle<Anp::TrigTimeAlg> &lhs,
				  const Anp::Handle<Anp::TrigTimeAlg> &rhs)
{
  //
  // Compare two objects based on key
  //
  if(!lhs.valid() || !rhs.valid()) { 
    cerr << "TimeAlgSort::operator() - lhs.ptr=" << lhs.get() << ", " << "rhs.ptr=" << rhs.get() << endl;
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
      //cout << "operator() - missing key " << fKey << " for: " 
      //<< lhs -> GetName() << " and " << rhs -> GetName() << endl;
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
