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
#include "TrigCostExec/TrigTimeROB.h"

using namespace std;

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeROB::SubDet::Clear()
{  
  // Clear variables
      time                   = 0.0;
      requestSize            = 0.0;
      requestSizeCached      = 0.0;
      retrievSize            = 0.0;
      nrobs                  =  0;
      nrobs_cached           =  0;
      nrobs_retrieved        =  0;
      nrobs_ignored          =  0;
      nrobs_disabled         =  0;
      nTrigMonRobRequests    =  0;
      nTrigMonROB_FullyCached=  0;
      nTrigMonROB_FullyRetriev= 0;
      alg_name               =  "UNKNOWN_ALG";
}
//--------------------------------------------------------------------------------------      
void Anp::TrigTimeROB::SubDet::AddSubDet(const SubDet &rhs)
{  
  //
  // Add the results from the Event
  //
  time                     += rhs.time;
  requestSize              += rhs.requestSize;
  requestSizeCached        += rhs.requestSizeCached; 
  retrievSize              += rhs.retrievSize;
  nrobs                    += rhs.nrobs;
  nrobs_cached             += rhs.nrobs_cached;
  nrobs_retrieved          += rhs.nrobs_retrieved;
  nrobs_ignored            += rhs.nrobs_ignored;
  nrobs_disabled           += rhs.nrobs_disabled;
  nTrigMonRobRequests      += rhs.nTrigMonRobRequests;
  nTrigMonROB_FullyCached  += rhs.nTrigMonROB_FullyCached;
  nTrigMonROB_FullyRetriev += rhs.nTrigMonROB_FullyRetriev;
  alg_name                  = rhs.alg_name;
}

//--------------------------------------------------------------------------------------      
// Start Hists sub class
//
Anp::TrigTimeROB::Hists::Hists()
  :isConfig(false),
   dir(0),
   hRobTime(0),
   hCachedSize(0),
   hRequestSize(0),
   hnumRobRequests(0)
{
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeROB::Hists::Set(TDirectory *dir, const Registry &reg)
{
  if(!isConfig) {
    isConfig = true;
  }
  else {
    cerr << "TrigTimeROB::Hists::Set - error: executing 2nd time" << endl;
    return;
  }

  std::string prefix, opts;
  bool save_dir = true;
  reg.Get("TrigTimeROB::Hists::Prefix", prefix);
  reg.Get("TrigTimeROB::Hists::Opts",   opts);
  reg.Get("TrigTimeROB::Hists::Save",   save_dir);

  hRobTime        = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_time",     "time_rob");
  hCachedSize     = Anp::HistMan::Instance().CreateTH1(prefix+"_cached_size",  "time_rob");
  hRequestSize    = Anp::HistMan::Instance().CreateTH1(prefix+"_request_size", "time_rob");
  hnumRobRequests = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_requests", "time_rob");

  //
  // Set histogram option for with dynamic X axis re-binning
  //
  if(opts.find("rebin") != string::npos) {
    if(hRobTime)        hRobTime       ->SetBit(TH1::kCanRebin);
    if(hCachedSize)     hCachedSize    ->SetBit(TH1::kCanRebin);
    if(hRequestSize)    hRequestSize   ->SetBit(TH1::kCanRebin);
    if(hnumRobRequests) hnumRobRequests->SetBit(TH1::kCanRebin);
    }

    if(save_dir) {
    Anp::SetDir(hRobTime,        dir);
    Anp::SetDir(hCachedSize,     dir);
    Anp::SetDir(hRequestSize,    dir);
    Anp::SetDir(hnumRobRequests, dir);
    }
}

//--------------------------------------------------------------------------------------      
TH1* Anp::TrigTimeROB::Hists::GetTH1(const std::string &hname)
{
  //
  // Create and configure TH1
  //
  TH1 *h = Anp::HistMan::Instance().CreateTH1(hname, "time_rob");
  if(!h) {
    cout << "TrigTimeROB::Hists::GetTH1 - no TH1 for: " << hname << endl;
    return 0;
  }
  
  h->SetDirectory(dir);
  h->SetBit(TH1::kCanRebin);

  return h;
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeROB::Hists::Fill(const SubDet &info)
{
  //
  // Fill event information
  // 
  if(hRobTime)        hRobTime       ->Fill(info.time);
  if(hCachedSize)     hCachedSize    ->Fill(4.0*info.requestSizeCached/1024.0);
  if(hRequestSize)    hRequestSize   ->Fill(4.0*info.requestSize/1024.0);
  if(hnumRobRequests) hnumRobRequests->Fill(info.nTrigMonRobRequests);
}

//--------------------------------------------------------------------------------------      
// Start TrigTimeROB class
//
Anp::TrigTimeROB::TrigTimeROB()
  :fDir(0),
   fDebug(true),
   fCanRebin(true),
   fMakeHists(true),
   fUseType(false),
   fTimeout(0.0),
   fIsConfig(false),
   fisGlobalROS(false),
   fReset(false)
{
}

//--------------------------------------------------------------------------------------      
Anp::TrigTimeROB::~TrigTimeROB()
{
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeROB::Config(const Registry &reg, const uint32_t subdetId)
{

  if(!fIsConfig) {
    fIsConfig = true;
  }
  else {
    cout << "TrigTimeROB::Config - error: executing second time" << endl;    
  }

  reg.Get("TrigTimeROB", "Debug",      fDebug);
  reg.Get("TrigTimeROB", "CanvasType", fCanvasType);
  reg.Get("TrigTimeROB::HistOpts",     fHistOpts);
  reg.Get("TrigTimeROB::CanRebin",     fCanRebin);
  reg.Get("TrigTimeROB::MakeHists",    fMakeHists);
  reg.Get("TrigTimeROB::UseType",      fUseType);
  reg.Get("TrigTimeROB::Timeout",      fTimeout);
  reg.Get("TrigTimeROB::Prefix",       fPrefix);
  reg.Get("TrigTimeROB::isGlobalROS",  fisGlobalROS);
  reg.Get("TrigTimeROB::CoolData",     fCoolLumiName);
  reg.Get("TrigTimeROB::CostData",     fCostLumiName);
  
  fSubdetId=subdetId;
  fSubDet.Clear();
  fSubDetTotal.Clear();
  fSubDetEvt.Clear();

  //
  // Create histograms
  //
  if(fMakeHists) {
    Registry hreg(reg);
    hreg.Set("TrigTimeROB::Hists::Opts", fHistOpts);

    hreg.Set("TrigTimeROB::Hists::Prefix", "evt");
    fHistEvent.Set(fDir, hreg);

    hreg.Set("TrigTimeROB::Hists::Prefix", "call");
    fHistCalls.Set(fDir, hreg);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeROB::AddROBEntry(const TrigMonROB &rob)
{

  if(fDebug) cout << "TrigTimeROB::AddRobEntry - The rob getRequestorId: " << rob.getRequestorId() << endl;

  fSubDet.Clear();

  const std::vector<TrigMonROBData> &rvec = rob.getData();

  std::set<TrigMonROBData::History> hists;

  //
  // Fill data from individual ROB ids
  //
  ++fSubDet.nTrigMonRobRequests;
  for(unsigned int i = 0; i < rvec.size(); ++i) {
    const TrigMonROBData &data = rvec.at(i);
    //
    // Filling subdet struct - Info about the TrigMonROB request
    //
    ++fSubDet.nrobs;
    hists.insert(data.getHistory());
    fSubDet.requestSize+=data.getROBSize();

    if(data.getHistory()==TrigMonROBData::kCACHED){ 
      ++fSubDet.nrobs_cached;
      fSubDet.requestSizeCached+=data.getROBSize();
    }
    else if(data.getHistory()==TrigMonROBData::kRETRIEVED) ++fSubDet.nrobs_retrieved;
    else if(data.getHistory()==TrigMonROBData::kDISABLED) ++fSubDet.nrobs_disabled;
    else if(data.getHistory()==TrigMonROBData::kIGNORED) ++fSubDet.nrobs_ignored;
  }

  if(fSubDet.nrobs_cached!=fSubDet.nrobs){
    //
    // This ROB request was NOT fully cached
    //
    fSubDet.time+=static_cast<float>(rob.getTimer());
    if(fDebug) cout << "TrigTimeROB::Finalize adding the caching time: " << rob.getTimer() << endl;
  }
  else if(fSubDet.nrobs_cached==fSubDet.nrobs){ 
    //
    // This ROB request was fully cached
    //
    ++fSubDet.nTrigMonROB_FullyCached;
    if(fDebug) cout << "TrigTimeROB::Finalize adding the caching time: " << rob.getTimer() << endl;
  }
  //write the histograms
  fHistCalls.Fill(fSubDet);
  //
  // Add current event and RESET event data
  //
  fSubDetTotal.AddSubDet(fSubDet);
  fSubDetEvt.AddSubDet(fSubDet);
  fSubDet.Clear();
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeROB::AddROSEntry(const TrigMonROB &rob, const std::string algName, const uint32_t rosId)
{

  if(fDebug) cout << "TrigTimeROB::AddROSEntry - The ros getRequestorId: " << rob.getRequestorId() << endl;

  fSubDet.Clear();
  fReset = true;
  fSubDet.alg_name = algName;
  if(fisGlobalROS) fSubdetId = rosId;
  
  const std::vector<TrigMonROBData> &rvec = rob.getData();

  std::set<TrigMonROBData::History> hists;

  //
  // Fill data from individual ROB ids
  //
  ++fSubDet.nTrigMonRobRequests;
  const RobService &rob_svc = RobService::Instance();
  for(unsigned int i = 0; i < rvec.size(); ++i) {
    const TrigMonROBData &data = rvec.at(i);
    if(rob_svc.GetRosId(data.getROBId())==fSubdetId){
      //
      // Filling subdet struct - Info about the TrigMonROB request
      //
      if(fDebug) cout << "TrigTimeROB::AddROSEntry - adding ros info: " << fSubdetId << " size: " << data.getROBSize() << endl;
      ++fSubDet.nrobs;
      hists.insert(data.getHistory());
      fSubDet.requestSize +=data.getROBSize();
      
      if(data.getHistory()==TrigMonROBData::kCACHED){ 
	++fSubDet.nrobs_cached;
	fSubDet.requestSizeCached+=data.getROBSize();
      }
      else if(data.getHistory()==TrigMonROBData::kRETRIEVED){
	++fSubDet.nrobs_retrieved;
	fSubDet.retrievSize += data.getROBSize();
      }
      else if(data.getHistory()==TrigMonROBData::kDISABLED) ++fSubDet.nrobs_disabled;
      else if(data.getHistory()==TrigMonROBData::kIGNORED) ++fSubDet.nrobs_ignored;
    }
  }

  //
  // Consistency check. make sure that there is only one request to each ros for a rob request from an alg
  //
  //if(fSubDet.nrobs>1){
  //  cout << "TrigTimeROB::AddROSEntry - LOGIC ERROR - There are " << fSubDet.nrobs << " ros requests for this one rob request...should only be 1" << endl; 
  //}

  if(fSubDet.nrobs_cached!=fSubDet.nrobs){
    //
    // This ROB request was NOT fully cached
    //
    fSubDet.time+=static_cast<float>(rob.getTimer());
    if(fDebug) cout << "TrigTimeROB::Finalize adding the caching time: " << rob.getTimer() << endl;
  }
  else if(fSubDet.nrobs_cached==fSubDet.nrobs){ 
    //
    // This ROB request was fully cached
    //
    ++fSubDet.nTrigMonROB_FullyCached;
    if(fDebug) cout << "TrigTimeROB::Finalize adding the caching time: " << rob.getTimer() << endl;
  }
  if(fSubDet.nrobs_retrieved==fSubDet.nrobs) ++fSubDet.nTrigMonROB_FullyRetriev;
  //write the histograms
  fHistCalls.Fill(fSubDet);
  SubDet &alg = GetAlg(rob.getRequestorId());
  //
  // Add current event and RESET event data
  //
  fRosReset.insert(&alg);
  alg.AddSubDet(fSubDet);
  fSubDetTotal.AddSubDet(fSubDet);
  fSubDetEvt.AddSubDet(fSubDet);
  fSubDet.Clear();
}

//--------------------------------------------------------------------------------------      
void Anp::TrigTimeROB::ResetEvent()
{
  //
  // Reset event - inputs: event number, vector of timers arranged by event numbers
  //
  ++fSubDetTotal.n_event;
  fHistEvent.Fill(fSubDetEvt);
  fSubDetEvt.Clear();
  fReset = false;
  for(set<SubDet *>::iterator iter = fRosReset.begin(); iter!=fRosReset.end();++iter) (*iter)->AddEvent();
  fRosReset.clear();
}
     
//--------------------------------------------------------------------------------------
void Anp::TrigTimeROB::Finalize()
{
  //
  // Compute and save variables
  //

  fVarMap["nrobs_requests"]        = static_cast<float>(fSubDetTotal.nrobs);  
  fVarMap["nrobs_cached"]          = static_cast<float>(fSubDetTotal.nrobs_cached); 
  fVarMap["nrobs_retrieved"]       = static_cast<float>(fSubDetTotal.nrobs_retrieved); 
  fVarMap["nrobs_ignored"]         = static_cast<float>(fSubDetTotal.nrobs_ignored); 
  fVarMap["nrobs_disabled"]        = static_cast<float>(fSubDetTotal.nrobs_disabled);
  fVarMap["cpu_time"]              = fSubDetTotal.time;
  fVarMap["TrigMonRobRq"]          = static_cast<float>(fSubDetTotal.nTrigMonRobRequests);
  fVarMap["CachedTrigMonRobRq"]    = static_cast<float>(fSubDetTotal.nTrigMonROB_FullyCached);
  fVarMap["RobReqSize"]            = 4.0*fSubDetTotal.requestSize/1024.0;
  fVarMap["RobReqSizeCached"]      = 4.0*fSubDetTotal.requestSizeCached/1024.0;
  fVarMap["nEvts"]                 = fSubDetTotal.n_event;
 
  if(fSubDetTotal.nTrigMonRobRequests!=0) { 
    fVarMap["cpu_time_perTrigmonrob"]= static_cast<float>(fSubDetTotal.time)/(float)fSubDetTotal.nTrigMonRobRequests;
    fVarMap["percent_cached"] = static_cast<float>(fSubDetTotal.nTrigMonROB_FullyCached)/(float)fSubDetTotal.nTrigMonRobRequests;
    float time_per_Req = fSubDetTotal.time/static_cast<float>(fSubDetTotal.nTrigMonRobRequests);
    fROSInfoMap["tot_time_pROSReq"]         = Anp::Round2Pair(time_per_Req, 0.01).first;
  }

  if(fSubDetTotal.n_event!=0){
    float time_per_Evt = fSubDetTotal.time/static_cast<float>(fSubDetTotal.n_event);
    fROSInfoMap["tot_time_pROSEvt"]         = Anp::Round2Pair(time_per_Evt, 0.01).first;
  }

  //while(fHistEvent.hRobTime->Integral() <= nEvts) fHistEvent.hRobTime->Fill(0.0);
  //while(fHistEvent.hCachedSize->Integral() <= nEvts) fHistEvent.hCachedSize->Fill(0.0);
  //while(fHistEvent.hRequestSize->Integral() <= nEvts) fHistEvent.hRequestSize->Fill(0.0);
  //while(fHistEvent.hnumRobRequests->Integral() <= nEvts) fHistEvent.hnumRobRequests->Fill(0);
}

//--------------------------------------------------------------------------------------      
const std::string Anp::TrigTimeROB::GetName() const
{
  //
  // Get SubDet Name
  //
  return RobService::Instance().GetSubDetFromId(fSubdetId);
}

//--------------------------------------------------------------------------------------      
const std::string Anp::TrigTimeROB::GetROSName() const
{
  //
  // Get SubDet Name
  //
  return RobService::Instance().GetRosFromId(fSubdetId);
}

//---------------------------------------------------------------------------------------
double Anp::TrigTimeROB::GetVar(const std::string &key) const
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
string Anp::TrigTimeROB::GetString(const std::string &key) const
{
  //
  // Find variable
  //
  ROSInfoMap::const_iterator vit = fROSInfoMap.find(key);
  if(vit != fROSInfoMap.end()) {
    return vit -> second;
  } 

  return "N/A";
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeROB::PrintVar(std::ostream &os,
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
void Anp::TrigTimeROB::PrintVar(std::ostream &os,
				const std::string &key,
				const std::string &prf,
				const std::string &suf,
				const int nEvts) const			
{
  //
  // Print variable if it exists
  //
  os << prf;

  VarMap::const_iterator vit = fVarMap.find(key);
  if(vit != fVarMap.end()) {
    os << static_cast<float>(vit -> second)/static_cast<float>(nEvts);
  }

  os << suf << endl;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeROB::WriteHtml(Handle<HtmlNode> node, TCanvas *canvas) const
{
  //
  // Write html summary
  //
  if(!node.valid()) { 
    cerr << "TrigTimeROB::WriteHtml - node is not valid" << endl;
    return;
  }
  
  ostream &os = node -> GetCont();

  os << "<pre>" << "Totals for all processed events: " << endl;

  PrintVar(os, "cpu_time_perTrigmonrob",  "  CPU Time per ROB Request:          ", " ms");
  PrintVar(os, "cpu_time",                "  Total Processing time:             ", " ms");
  PrintVar(os, "nrobs_requests",          "  ROB Requests:                      ", "");
  PrintVar(os, "nrobs_cached",            "  Total Cached ROB Requests:         ", "");
  PrintVar(os, "RobReqSize",              "  Total Size of requested ROBs:      ", " kBytes");
  PrintVar(os, "RobReqSizeCached",        "  Total Size of cached ROBs:         ", " kBytes");

  os << endl;
  if(fSubDetTotal.n_event>0){
    os << "Mean quantities per event: " << endl;
    PrintVar(os, "cpu_time",                "  Total Processing time per Event:   ", " ms"    ,fSubDetTotal.n_event);
    PrintVar(os, "nrobs_requests",          "  ROB Requests per Event:            ", ""       ,fSubDetTotal.n_event);
    PrintVar(os, "nrobs_cached",            "  Cached ROB Requests per Event:     ", ""       ,fSubDetTotal.n_event);
    PrintVar(os, "RobReqSize",              "  Size of requested ROBs per Event:  ", " kBytes",fSubDetTotal.n_event);
    PrintVar(os, "RobReqSizeCached",        "  Size of cached ROBs per Event:     ", " kBytes",fSubDetTotal.n_event);
    os << endl;
  }
  //
  // Make histograms
  //
  if(fPrefix.empty() || (fCanvasType == "png" && fCanvasType == "eps")) {
    return;
  }
 
  os << "Histograms filled per Rob Request: " << endl;
  
  const string &prf = fPrefix;
  const string &can = fCanvasType;
  if(fDebug) cerr << "TrigTimeROB::WriteHtml - Create histo in: " << prf << endl;
  PrintHist(node.ref(), fHistCalls.hRobTime, canvas, prf+"_call_rob_time."+can,
	    "  Total processing time", fHistOpts);
  
  PrintHist(node.ref(), fHistCalls.hCachedSize, canvas, prf+"_call_rob_cache."+can,
	    "  Total Cached Size", fHistOpts);
  
  PrintHist(node.ref(), fHistCalls.hRequestSize, canvas, prf+"_call_rob_request."+can,
	    "  Total Requested Size", fHistOpts);
  
  os << endl;
  if(fSubDetTotal.nTrigMonRobRequests>0){
    os << "Histograms filled per event: " << endl;
  
    PrintHist(node.ref(), fHistEvent.hRobTime, canvas, prf+"_evt_rob_time."+can,
	      "  Total processing time", fHistOpts);
    
    PrintHist(node.ref(), fHistEvent.hCachedSize, canvas, prf+"_evt_rob_cache."+can,
	      "  Total Cached Size", fHistOpts);
    
    PrintHist(node.ref(), fHistEvent.hRequestSize, canvas, prf+"_evt_rob_request."+can,
	      "  Total Requested Size", fHistOpts);
    
    PrintHist(node.ref(), fHistEvent.hnumRobRequests, canvas, prf+"_evt_rob_nreq."+can,
	      "  Number of Rob Requests", fHistOpts);
    
    os << endl;
  }
  os << "</pre>" << endl;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeROB::clearROSVars()
{
  fVarMap["tot_Req_pAllEvt"]  = 0.0;
  fVarMap["tot_Req_pROSEvt"]  = 0.0;
  fVarMap["tot_data_pAllEvt"] = 0.0;
  fVarMap["tot_data_pROSEvt"] = 0.0;
  fVarMap["tot_ReqRate_Hz"]   = 0.0;
  fVarMap["tot_Req_Count"]   = 0.0;
  fVarMap["tot_Req_Count_sc"]   = 0.0;
  fVarMap["tot_ReqBandwidth"] = 0.0;
  fVarMap["tot_Req_Data"] = 0.0;
  fVarMap["tot_Req_Data_sc"] = 0.0;

  fVarMap["tot_retr_Req_pAllEvt"]  = 0.0;
  fVarMap["tot_retr_Req_pROSEvt"]  = 0.0;
  fVarMap["tot_retr_data_pAllEvt"] = 0.0;
  fVarMap["tot_retr_data_pROSEvt"] = 0.0;
  fVarMap["tot_retr_ReqRate_Hz"]   = 0.0;
  fVarMap["tot_retr_Req_Count"]   = 0.0;
  fVarMap["tot_retr_Req_Count_sc"]   = 0.0;
  fVarMap["tot_retr_ReqBandwidth"] = 0.0;
  fVarMap["tot_retr_Req_Count"] = 0.0;
  fVarMap["tot_retr_Req_Count_sc"] = 0.0;

  fVarMap["tot_nrobs_all"]         = 0.0;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeROB::calcROS(const SubDet &data, const unsigned nEvts, const float runCoolTime, const float runCoolEvts, const float scale, const float /*nROSEvts*/)
{
  //
  // convert from hz to khz
  //
  double khz_scale = scale/1000.0;
  //
  // Calculates the ROS usage for each ROS and adds info to 
  //
  float totTime =0.0;
  for(AlgMap::iterator alg = fAlgMap.begin(); alg!=fAlgMap.end(); ++alg) totTime += alg->second.time;

  float Req_pROSEvt       = 0.0;
  float retr_Req_pROSEvt  = 0.0;
  float data_pROSEvt      = 0.0;
  float retr_data_pROSEvt = 0.0;
  float time_per_Evt      = 0.0;

  if(data.n_event !=0){ //retrievSize nTrigMonROB_FullyRetriev  
    Req_pROSEvt       = static_cast<float>(data.nrobs)/static_cast<float>(data.n_event);
    retr_Req_pROSEvt  = static_cast<float>(data.nrobs_retrieved)/static_cast<float>(data.n_event);
    data_pROSEvt      = 4.0*data.requestSize/1024.0/static_cast<float>(data.n_event);
    retr_data_pROSEvt = 4.0*(data.retrievSize)/1024.0/static_cast<float>(data.n_event);
    time_per_Evt      = data.time/static_cast<float>(data.n_event);
  }

  float time_per_Req = 0.0;
  if(data.nrobs !=0){ 
    time_per_Req = data.time/static_cast<float>(data.nrobs);
  }

  float Req_pAllEvt        = 0.0;
  float retr_Req_pAllEvt   = 0.0;
  float data_pAllEvt       = 0.0;
  float retr_data_pAllEvt  = 0.0;
  if(nEvts !=0){ 
    //
    // Must mult by 10 because only 10% of events have cost info
    //
    Req_pAllEvt       = 10.0*static_cast<float>(data.nrobs)/static_cast<float>(nEvts);
    retr_Req_pAllEvt  = 10.0*static_cast<float>(data.nrobs_retrieved)/static_cast<float>(nEvts);
    data_pAllEvt      = 4.0*10.0*data.requestSize/1024.0/static_cast<float>(nEvts);
    retr_data_pAllEvt = 4.0*10.0*(data.retrievSize)/1024.0/static_cast<float>(nEvts);
  }

  float retr_rate_hz     = 0.0;
  float rate_hz          = 0.0;
  float retr_bandwid_hz  = 0.0;
  float bandwid_hz       = 0.0;
  if(runCoolTime>0.0){
    if(fDebug){
      cout << "TrigTimeROB::calcROS - run cool time: " << runCoolTime << " cool evts: " << runCoolEvts << endl
	   << "       cost evts: " << nEvts << " evts with cost data: " << data.n_event << endl;
    }
    retr_rate_hz    = 10.0*static_cast<float>(data.nrobs_retrieved)/runCoolTime/1000.0;
    rate_hz         = 10.0*static_cast<float>(data.nrobs)/runCoolTime/1000.0;
    retr_bandwid_hz = 4.0*10.0*static_cast<float>(data.retrievSize)/1024.0/runCoolTime/1000.0;
    bandwid_hz      = 4.0*10.0*static_cast<float>(data.requestSize)/1024.0/runCoolTime/1000.0;
  }

  //
  // Write the ROS usage to a set of strings and floats
  //

  fROSInfoMap["percent_Evt"]       = "N/A";
  if(nEvts !=0){
      float percent_Evt            = 1000.0*data.n_event/static_cast<float>(nEvts);
      fROSInfoMap["percent_Evt"]   = Anp::Round2Pair(percent_Evt, 0.1).first;
  }

  fROSInfoMap["time_frac"]     = "N/A";
  if(totTime !=0.0){
    float time_frac            = 100.0*data.time/totTime;
    fROSInfoMap["time_frac"]   = Anp::Round2Pair(time_frac, 0.1).first;
  }

  fROSInfoMap["req_time"]         = Anp::Round2Pair(data.time, 1.0).first;
  fROSInfoMap["call_size"]        = Anp::Round2Pair(4.0*(data.retrievSize/1000000.0)/1024.0, 0.1).first;
  fROSInfoMap["req_size"]         = Anp::Round2Pair(4.0*data.requestSize/1000000.0/1024.0, 0.1).first;
  fROSInfoMap["Req_pROSEvt"]      = Anp::Round2Pair(Req_pROSEvt, 0.01).first;
  fROSInfoMap["retr_Req_pROSEvt"] = Anp::Round2Pair(retr_Req_pROSEvt, 0.01).first;
  fROSInfoMap["alg_name"]         = data.alg_name;
  fROSInfoMap["n_ROS_Req"]        = Anp::Round2Pair(static_cast<float>(data.nrobs), 10.0).first;
  fROSInfoMap["n_ROS_Retriev"]    = Anp::Round2Pair(static_cast<float>(data.nrobs_retrieved), 10.0).first;

  fVarMap["Req_pAllEvt"]          = Req_pAllEvt;
  fVarMap["Req_pROSEvt"]          = Req_pROSEvt;
  fVarMap["data_pAllEvt"]         = data_pAllEvt;
  fVarMap["data_pROSEvt"]         = data_pROSEvt;
  fVarMap["ReqRate_Hz"]           = rate_hz;
  fVarMap["ReqBandwidth"]         = bandwid_hz;
	   		             
  fVarMap["retr_Req_pAllEvt"]     = retr_Req_pAllEvt;
  fVarMap["retr_Req_pROSEvt"]     = retr_Req_pROSEvt;
  fVarMap["retr_data_pAllEvt"]    = retr_data_pAllEvt;
  fVarMap["retr_data_pROSEvt"]    = retr_data_pROSEvt;
  fVarMap["retr_ReqRate_Hz"]      = retr_rate_hz;
  fVarMap["retr_ReqBandwidth"]    = retr_bandwid_hz;
  fROSInfoMap["time_pROSReq"]     = Anp::Round2Pair(time_per_Req, 0.01).first;
  fROSInfoMap["time_pROSEvt"]     = Anp::Round2Pair(time_per_Evt, 0.01).first;

  fVarMap["nrobs_all"]            = data.nTrigMonRobRequests;

  //
  // Set as a string
  //
  fROSInfoMap["Req_pAllEvt"]          = Anp::Round2Pair(Req_pAllEvt, 0.01).first;
  fROSInfoMap["Req_pROSEvt"]          = Anp::Round2Pair(Req_pROSEvt, 0.01).first;
  fROSInfoMap["data_pAllEvt"]         = Anp::Round2Pair(data_pAllEvt, 0.1).first;
  fROSInfoMap["data_pROSEvt"]         = Anp::Round2Pair(data_pROSEvt, 1.0).first;
  fROSInfoMap["ReqRate_Hz"]           = Anp::Round2Pair(rate_hz, 0.01).first;
  fROSInfoMap["Req_Count"]            = Anp::Round2Pair(static_cast<float>(data.nrobs), 10.0).first;
  fROSInfoMap["Req_Count_scaled"]     = Anp::Round2Pair(khz_scale*static_cast<float>(data.nrobs), 0.01).first;
  fROSInfoMap["ReqBandwidth"]         = Anp::Round2Pair(bandwid_hz, 0.01).first;
  fROSInfoMap["Req_Data"]             = Anp::Round2Pair(4.0*static_cast<float>(data.requestSize)/1024.0, 0.01).first;
  fROSInfoMap["Req_Data_scaled"]      = Anp::Round2Pair(khz_scale*4.0*static_cast<float>(data.requestSize)/1024.0, 0.01).first;

  fROSInfoMap["retr_Req_pAllEvt"]     = Anp::Round2Pair(retr_Req_pAllEvt, 0.01).first;
  fROSInfoMap["retr_Req_pROSEvt"]     = Anp::Round2Pair(retr_Req_pROSEvt, 0.01).first;
  fROSInfoMap["retr_data_pAllEvt"]    = Anp::Round2Pair(retr_data_pAllEvt, 0.1).first;
  fROSInfoMap["retr_data_pROSEvt"]    = Anp::Round2Pair(retr_data_pROSEvt, 1.0).first;
  fROSInfoMap["retr_ReqRate_Hz"]      = Anp::Round2Pair(retr_rate_hz, 0.01).first;
  fROSInfoMap["retr_Req_Count"]       = Anp::Round2Pair(static_cast<float>(data.nrobs_retrieved), 10.0).first;
  fROSInfoMap["retr_Req_Count_scaled"]= Anp::Round2Pair(khz_scale*static_cast<float>(data.nrobs_retrieved), 0.01).first;
  fROSInfoMap["retr_ReqBandwidth"]    = Anp::Round2Pair(retr_bandwid_hz, 0.01).first;
  fROSInfoMap["retr_Req_Data"]        = Anp::Round2Pair(4.0*static_cast<float>(data.retrievSize)/1024.0, 0.01).first;
  fROSInfoMap["retr_Req_Data_scaled"] = Anp::Round2Pair(khz_scale*4.0*static_cast<float>(data.retrievSize)/1024.0, 0.01).first;

  fROSInfoMap["time_pROSReq"]         = Anp::Round2Pair(time_per_Req, 0.01).first;
  fROSInfoMap["time_pROSEvt"]         = Anp::Round2Pair(time_per_Evt, 0.01).first;

  fROSInfoMap["nrobs_all"]            = Anp::Round2Pair(data.nTrigMonRobRequests, 10.0).first;

  //
  // Calculating total variables
  //
  fVarMap["tot_Req_pAllEvt"]       += Req_pAllEvt;
  fVarMap["tot_Req_pROSEvt"]       += Req_pROSEvt;
  fVarMap["tot_data_pAllEvt"]      += data_pAllEvt;
  fVarMap["tot_data_pROSEvt"]      += data_pROSEvt;
  fVarMap["tot_ReqRate_Hz"]        += rate_hz;
  fVarMap["tot_Req_Count"]         += static_cast<float>(data.nrobs);
  fVarMap["tot_Req_Count_sc"]      += khz_scale*static_cast<float>(data.nrobs);
  fVarMap["tot_ReqBandwidth"]      += bandwid_hz;
  fVarMap["tot_Req_Data"]          += 4.0*static_cast<float>(data.requestSize)/1024.0;
  fVarMap["tot_Req_Data_sc"]       += khz_scale*4.0*static_cast<float>(data.requestSize)/1024.0;
				   
  fVarMap["tot_retr_Req_pAllEvt"]  += retr_Req_pAllEvt;
  fVarMap["tot_retr_Req_pROSEvt"]  += retr_Req_pROSEvt;
  fVarMap["tot_retr_data_pAllEvt"] += retr_data_pAllEvt;
  fVarMap["tot_retr_data_pROSEvt"] += retr_data_pROSEvt;
  fVarMap["tot_retr_ReqRate_Hz"]   += retr_rate_hz;
  fVarMap["tot_retr_Req_Count"]    += static_cast<float>(data.nrobs_retrieved);
  fVarMap["tot_retr_Req_Count_sc"] += khz_scale*static_cast<float>(data.nrobs_retrieved);
  fVarMap["tot_retr_ReqBandwidth"] += retr_bandwid_hz;
  fVarMap["tot_retr_Req_Data"]     += 4.0*static_cast<float>(data.retrievSize)/1024.0;
  fVarMap["tot_retr_Req_Data_sc"]  += khz_scale*4.0*static_cast<float>(data.retrievSize)/1024.0;

  fVarMap["tot_nrobs_all"]         += data.nTrigMonRobRequests;

  //
  // Total strings
  //
  fROSInfoMap["tot_Req_pAllEvt"]          = Anp::Round2Pair(fVarMap["tot_Req_pAllEvt"], 0.01).first;
  fROSInfoMap["tot_Req_pROSEvt"]          = Anp::Round2Pair(fVarMap["tot_Req_pROSEvt"], 0.01).first;
  fROSInfoMap["tot_data_pAllEvt"]         = Anp::Round2Pair(fVarMap["tot_data_pAllEvt"], 0.1).first;
  fROSInfoMap["tot_data_pROSEvt"]         = Anp::Round2Pair(fVarMap["tot_data_pROSEvt"], 1.0).first;
  fROSInfoMap["tot_ReqRate_Hz"]           = Anp::Round2Pair(fVarMap["tot_ReqRate_Hz"], 0.01).first;
  fROSInfoMap["tot_Req_Count"]            = Anp::Round2Pair(fVarMap["tot_Req_Count"], 10.0).first;
  fROSInfoMap["tot_Req_Count_scaled"]     = Anp::Round2Pair(fVarMap["tot_Req_Count_sc"], 0.01).first;
  fROSInfoMap["tot_ReqBandwidth"]         = Anp::Round2Pair(fVarMap["tot_ReqBandwidth"], 0.01).first;
  fROSInfoMap["tot_Req_Data"]             = Anp::Round2Pair(fVarMap["tot_Req_Data"], 1.0).first;
  fROSInfoMap["tot_Req_Data_scaled"]      = Anp::Round2Pair(fVarMap["tot_Req_Data_sc"], 1.0).first;

  fROSInfoMap["tot_retr_Req_pAllEvt"]     = Anp::Round2Pair(fVarMap["tot_retr_Req_pAllEvt"], 0.01).first;
  fROSInfoMap["tot_retr_Req_pROSEvt"]     = Anp::Round2Pair(fVarMap["tot_retr_Req_pROSEvt"], 0.01).first;
  fROSInfoMap["tot_retr_data_pAllEvt"]    = Anp::Round2Pair(fVarMap["tot_retr_data_pAllEvt"], 0.1).first;
  fROSInfoMap["tot_retr_data_pROSEvt"]    = Anp::Round2Pair(fVarMap["tot_retr_data_pROSEvt"], 1.0).first;
  fROSInfoMap["tot_retr_ReqRate_Hz"]      = Anp::Round2Pair(fVarMap["tot_retr_ReqRate_Hz"], 0.01).first;
  fROSInfoMap["tot_retr_Req_Count"]       = Anp::Round2Pair(fVarMap["tot_Req_Count"], 10.0).first;
  fROSInfoMap["tot_retr_Req_Count_scaled"]= Anp::Round2Pair(fVarMap["tot_Req_Count_sc"], 0.01).first;
  fROSInfoMap["tot_retr_ReqBandwidth"]    = Anp::Round2Pair(fVarMap["tot_retr_ReqBandwidth"], 0.01).first;
  fROSInfoMap["tot_retr_Req_Data"]        = Anp::Round2Pair(fVarMap["tot_Req_Data"], 1.0).first;
  fROSInfoMap["tot_retr_Req_Data_scaled"] = Anp::Round2Pair(fVarMap["tot_Req_Data_sc"], 1.0).first;

  fROSInfoMap["tot_nrobs_all"]            = Anp::Round2Pair(fVarMap["tot_Req_Data_sc"], 10.0).first;
}

//---------------------------------------------------------------------------------------
void Anp::TrigTimeROB::WriteROSHtml(Handle<HtmlNode> node, HtmlMenu &hmenu, TCanvas */*canvas*/, const unsigned nEvts, const set<unsigned> *lbs, const unsigned nROSEvts) 
{
  //
  // Write html summary
  //
  if(!node.valid()) { 
    cerr << "TrigTimeROB::WriteHtml - node is not valid" << endl;
    return;
  }
  string title = "Alg Name";
  clearROSVars();
  ostream &os = node -> GetCont();

  os << "<pre>";
  if(!fisGlobalROS)     os << "ROS: " << GetROSName() << " was requested by: " << fAlgMap.size() << " algorithms" << endl;
  else if(fisGlobalROS) os << "ROS: All ROS(es)"      << " was requested by: " << fAlgMap.size() << " algorithms" << endl;
    //     << "Number of Events requesting ROS: " << nEvts
  os << endl
     << "The total number of ROS requests are not computed with all L2 events. These results were NOT normalized to the total number of L2 accepted events." << endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> " << title << " </th>"
     << "<th> requests/all events </th>"
     << "<th title='requests/events that request from that ROS'> requests/ROS events </th>"
     << "<th title='data/all events'> data/all events (kb) </th>"
     << "<th title='data/events that request from that ROS'> data/ROS events (kb) </th>"
     << "<th title='request rate in kHz'> request rate (kHz) </th>"
     << "<th title='request counts'> request counts </th>"
     << "<th title='scaled request rate in kHz'> scaled request rate (kHz) </th>"
     << "<th> request bandwidth (Mb/s) </th>"
     << "<th> request data (kb) </th>"
     << "<th> scaled request bandwidth (Mb/s) </th>"
     << "<th> retr requests/all events </th>"
     << "<th title='Retrieved requests/events that request from that ROS'> retr requests/ROS events </th>"
     << "<th title='Retrieved data/all events'> retr data/all events (kb) </th>"
     << "<th title='Retrieved data/events that request from that ROS'> retr data/ROS events (kb) </th>"
     << "<th title='Retrieved request rate in kHz'> retr request rate (kHz) </th>"
     << "<th title='Retrieved request Counts'> retr request counts </th>"
     << "<th title='Scaled Retrieved request rate in kHz'> scaled retr request rate (kHz) </th>"
     << "<th> retrieval bandwidth (Mb/s) </th>"
     << "<th> retrieved data (kb) </th>"
     << "<th> scaled retrieval bandwidth (Mb/s) </th>"
     << "<th title='retrieved time/request'> retrieval time/request (ms) </th>"
     << "<th title='retrieved time/events that request from that ROS'> retrieval time/ROS event (ms) </th>"
     << "<th title='takes counts to hertz'> scale factor </th>"
     << "<th title='all ros requests'> all ros req count </th>"
     << "</tr>" << endl;

  //
  // Create scaling alg which talks to Cool
  //
  vector<Anp::Lumi::BlockInfo> blockInfo;
  float    runCoolTime = 0.0;
  unsigned runCoolEvts = 0;
  float    runCoolScal = 0.0;
  float    numLB       = 0.0;
  if(!fCoolLumiName.empty()) {
    fCoolLumi = Factory<LumiABC>::Instance().Get(fCoolLumiName);
    if(fCoolLumi.valid()) {
      cout << "TrigTimeROB::calcROS - retrieved lumi algorithm: " << fCoolLumiName << endl;
      
      blockInfo = fCoolLumi->GetInfos();
      for(vector<Anp::Lumi::BlockInfo>::iterator it = blockInfo.begin(); it!=blockInfo.end(); ++it){
	
	//
	// collect only the time for events run within the lb
	//
    
	if(lbs){

	  set<unsigned>::iterator lbiter = lbs->find(it->lb);
	  if(lbiter != lbs->end()){
	 
	    runCoolTime += it->GetLBDuration();
	    runCoolEvts += it->L1accept;
	    //
	    // compute the running avg for scale factor
	    //
	    //numLB = numLB + 1.0;
	    //runCoolScal = runCoolScal + (it->scaleCorrL2-runCoolScal)/numLB;
	    cout << "TrigTimeROB::Write - CoolLumABC lb: " << it->lb 
		 << " scaleL1: " << it->scaleCorrL1 
		 << " scaleL2: " << it->scaleCorrL2 
		 << " scaleEF: " << it->scaleCorrEF 
		 << " (#cost evts)/(#evts with ros): " << static_cast<float>(nEvts)/static_cast<float>(nROSEvts) << endl;
	    
	  }
	}
      }
    }
    else {
      cout << "Config - failed to get lumi algorithm: " << fCoolLumiName << endl;
    }
  }

  if(!fCostLumiName.empty()) {
    fCostLumi = Factory<LumiABC>::Instance().Get(fCostLumiName);
    if(fCostLumi.valid()) {
      cout << "TrigTimeROB::calcROS - retrieved lumi algorithm: " << fCostLumiName << endl;
      
      blockInfo = fCostLumi->GetInfos();
    
      for(vector<Anp::Lumi::BlockInfo>::iterator it = blockInfo.begin(); it!=blockInfo.end(); ++it){
	cout << "TrigTimeROB::Write - CostLumABC lb: " << it->lb << " scaleL1: " << it->scaleCorrL1 << " scaleL2: " << it->scaleCorrL2 
	     << " scaleEF: " << it->scaleCorrEF << endl;
      //
      // collect only the time for events run within the lb
      //

	if(lbs){

	  set<unsigned>::iterator lbiter = lbs->find(it->lb);
	  if(lbiter != lbs->end()){
	    //
	    // compute the running avg for scale factor
	    //
	    numLB = numLB + 1.0;
	    runCoolScal = runCoolScal + (it->scaleCorrL1-runCoolScal)/numLB;
	  }
	}
      }
    }
    else {
      cout << "Config - failed to get lumi algorithm: " << fCostLumiName << endl;
    }
  }
  if(numLB == 0.0) numLB = numLB + 1.0;
  vector<SubDet> algs;
  for(AlgMap::iterator alg = fAlgMap.begin(); alg!=fAlgMap.end(); ++alg) algs.push_back(alg->second);
  std::sort(algs.begin(), algs.end());

  //
  // Loop over ROS and write info
  //
  for(unsigned alg = 0; alg<algs.size(); ++alg){
    calcROS(algs.at(alg),nEvts,runCoolTime,static_cast<float>(runCoolEvts),runCoolScal/numLB,static_cast<float>(nROSEvts));
    Handle<HtmlNode> nodeA = hmenu.FindAlgName(fROSInfoMap["alg_name"]);
    if(nodeA.valid()) {
      fROSInfoMap["alg_name"] = nodeA->GetLink(fROSInfoMap["alg_name"]);
    }

    os  << "<tr>" 
  	<< "<td align=\"left\"> "   << fROSInfoMap["alg_name"]                     << " </td>"      
  	<< "<td align=\"left\"> "   << fROSInfoMap["Req_pAllEvt"]                  << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["Req_pROSEvt"]                  << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["data_pAllEvt"]                 << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["data_pROSEvt"]                 << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["ReqRate_Hz"]                   << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["Req_Count"]                    << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["Req_Count_scaled"]                    << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["ReqBandwidth"]                 << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["Req_Data"]                     << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["Req_Data_scaled"]                     << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["retr_Req_pAllEvt"]             << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["retr_Req_pROSEvt"]             << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["retr_data_pAllEvt"]            << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["retr_data_pROSEvt"]            << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["retr_ReqRate_Hz"]              << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["retr_Req_Count"]               << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["retr_Req_Count_scaled"]               << " </td>"
	<< "<td align=\"left\"> "   << fROSInfoMap["retr_ReqBandwidth"]            << " </td>"
	<< "<td align=\"left\"> "   << fROSInfoMap["retr_Req_Data"]                << " </td>"
	<< "<td align=\"left\"> "   << fROSInfoMap["retr_Req_Data_scaled"]                << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["time_pROSReq"]                 << " </td>"
  	<< "<td align=\"left\"> "   << fROSInfoMap["time_pROSEvt"]                 << " </td>"
      	<< "<td align=\"left\"> "   << runCoolScal/numLB                           << " </td>"
      	<< "<td align=\"left\"> "   << fROSInfoMap["nrobs_all"]                        << " </td>"
	<< "</tr>" << endl;
  }

  //
  // Write the totals for each column
  //
  os << "<tr>" << "<td align=\"left\"> Total </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_Req_pAllEvt"]                  << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_Req_pROSEvt"]                  << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_data_pAllEvt"]                 << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_data_pROSEvt"]                 << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_ReqRate_Hz"]                   << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_Req_Count"]                    << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_Req_Count_scaled"]                    << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_ReqBandwidth"]                 << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_Req_Data"]                     << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_Req_Data_scaled"]                     << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_Req_pAllEvt"]             << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_Req_pROSEvt"]             << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_data_pAllEvt"]            << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_data_pROSEvt"]            << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_ReqRate_Hz"]              << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_Req_Count"]               << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_Req_Count_scaled"]               << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_ReqBandwidth"]            << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_Req_Data"]                << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_retr_Req_Data_scaled"]                << " </td>"

     << "<td align=\"left\"> "   << "N/A"             << " </td>"
     << "<td align=\"left\"> "   << "N/A"             << " </td>"
     << "<td align=\"left\"> "   << "N/A"             << " </td>"
     << "<td align=\"left\"> "   << fROSInfoMap["tot_nrobs_all"]                             << " </td>"
     << "</tr>" << endl;
  os << "</table>" << endl;
  os << "</pre>" << endl;
}

//---------------------------------------------------------------------------------------
Anp::TrigTimeROB::SubDet &Anp::TrigTimeROB::GetAlg(int32_t algId)
{
  AlgMap::iterator alg = fAlgMap.find(algId);
  if(alg != fAlgMap.end()) return alg->second;
  
  SubDet algInfo;
  fAlgMap.insert(AlgMap::value_type(algId, algInfo));
  alg = fAlgMap.find(algId);
  return alg->second;
}

//---------------------------------------------------------------------------------------
bool Anp::TimeRobSort::operator()(const Anp::Handle<Anp::TrigTimeROB> &lhs,
				  const Anp::Handle<Anp::TrigTimeROB> &rhs)
{
  //
  // Compare two objects based on key
  //
  if(!lhs.valid() || !rhs.valid()) { 
    cerr << "TimeRobSort::operator() - lhs.ptr=" << lhs.get() << ", " << "rhs.ptr=" << rhs.get() << endl;
    return false;
  }
  if(!fKey.empty() && fKey != "name") {
    //
    // Compare by variables
    //
    const std::map<std::string, float> &lmap = lhs -> GetVarMap();
    const std::map<std::string, float> &rmap = rhs -> GetVarMap();
    
    const map<string, float>::const_iterator lit = lmap.find(fKey);
    const map<string, float>::const_iterator rit = rmap.find(fKey);
    
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

//--------------------------------------------------------------------------------------      
bool Anp::operator<(const Anp::TrigTimeROB::SubDet &lhs, const Anp::TrigTimeROB::SubDet &rhs)
{
  return rhs.nTrigMonRobRequests < lhs.nTrigMonRobRequests;
}
