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
#include "TrigCostExec/RemovedChn.h"
#include "TrigCostExec/TrigTimeAlg.h"
#include "TrigCostExec/TrigTimeChn.h"
#include "TrigCostExec/TrigTimeSeq.h"

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
void Anp::RemovedChn::rmAlgData::getalgdata(const TrigMonAlg &algPointer, TrigTimeSeq *&data, const unsigned roiKey)
{
  TrigTimeAlg *alg_type = data->GetAlgType(algPointer.getPosition());
  TrigTimeAlg *alg_name = data->GetAlgName(algPointer.getPosition());
  name   = alg_name->GetName();
  type   = alg_type->GetName();
  nameId = alg_name->GetNameId();
  typeId = alg_type->GetTypeId();
  alg    = algPointer;
  roiId  = algPointer.getRoiId(roiKey);
  m_byte = algPointer.getByte();
  m_word = algPointer.getWord();
  algTime= algPointer.getTimer();
}
//-----------------------------------------------------------------------------
void Anp::RemovedChn::rmRobData::addrobdata(TrigMonROB &robPointer, const unsigned robKey) 
{
  if(robKey<robPointer.getData().size()){
    TrigMonROBData &data = robPointer.getData().at(robKey);
    robId = data.getROBId();
    start = robPointer.getStart();
    stop  = robPointer.getStop();
    rob   = robPointer;
  }
  else cerr << "RemovedChn::rmRobData::addrobdata - ERROR rob data missing; size: " << robPointer.getData().size() << endl;
}

//-----------------------------------------------------------------------------
void Anp::RemovedChn::rmSeqData::Clear()
{
  //seqTime.clear();
  seq.clear();
  vecRmRobData.clear();  
  vecRmAlgData.clear(); 
}

//-----------------------------------------------------------------------------
void Anp::RemovedChn::rmSeqData::addAlgs(TrigTimeSeq *data)
{
  if(seq.size()>0){
    vector<TrigMonAlg> algs = seq.at(0).getAlg();
    for(int i=0;i<static_cast<int>(algs.size());++i){
      //
      // fill with algs from Seq
      //
      if(algs.at(i).getNRoi()>0){ 
	for(unsigned roi=0; roi<algs.at(i).getNRoi();++roi){
	  rmAlgData algdata;
	  algdata.getalgdata(algs.at(i),data,roi);
	  vecRmAlgData.push_back(algdata);
	}
      }
      else if(algs.at(i).getNRoi()==0){
	rmAlgData algdata;
	algdata.getalgdata(algs.at(i),data,1000);
	vecRmAlgData.push_back(algdata);
      }
    }//end for loop over algs
  }
  else cerr << "RemovedChn::rmSeqData::addAlgs - Error missing seq!" << endl;
}//end addAlgs

//-----------------------------------------------------------------------------
void Anp::RemovedChn::rmSeqData::addRobs(vector<TrigMonROB> &rob_list, TrigTimeSeq *&data) 
{
  //
  // Add retrieved Robs resulting from removed Chain
  //
  if(seq.size()>0){
    vector<TrigMonAlg> alg_entry = seq.at(0).getAlg();
    for(vector<TrigMonROB>::iterator rob=rob_list.begin();rob!=rob_list.end();++rob){
      unsigned robrequestID = rob->getRequestorId();
      for(unsigned i=0; i < alg_entry.size(); ++i){
	const TrigMonAlg &entry = alg_entry.at(i);
	//TrigTimeAlg *alg_type  = data->GetAlgType(entry.getPosition());
	TrigTimeAlg *alg_name  = data->GetAlgName(entry.getPosition());
	unsigned algId = alg_name->GetTypeId();
	if(robrequestID==algId){
	  //
	  // Check the timing is consistent
	  //
	  vector<unsigned> word = entry.getWord();
	  vector<TrigMonROBData> &vecROBData = rob->getData();
	  if(word.size()>1){
	    TrigMonTimer t_alg_start = entry.start();
	    TrigMonTimer t_alg_stop  = entry.stop();
	    TrigMonTimer t_rob_start = rob->start();
	    TrigMonTimer t_rob_stop  = rob->stop();
	    if(t_alg_start < t_rob_start && t_rob_stop < t_alg_stop){
	      for(unsigned r=0; r<vecROBData.size();++r){
		rmRobData rdata;
		rdata.addrobdata(*rob,r);
		vecRmRobData.push_back(rdata);
	      }//end loop over TrigMonROBData in TrigMonROB request
	      rob_list.erase(rob);
	      --rob;
	      break;
	    }//end checking that time matches
	  }//end check if timing info is present
	}//end matching rob request alg name to alg name in alg_entry
      }//end 
    }
  }
  else cerr << "RemovedChn::rmSeqData::addRobs - Error missing seq!" << endl;    
}//end addRobs function

//--------------------------------------------------------------------------------------      
// Start Event sub class
//
Anp::RemovedChn::Event::Event()
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
void Anp::RemovedChn::Event::AddEvent(const Event &rhs)
{
  n_event       += rhs.n_event;
  n_alg_calls   += rhs.n_alg_calls;
  n_alg_cache   += rhs.n_alg_cache;
  n_alg_timeout += rhs.n_alg_timeout;
  
  timer_sum     += rhs.timer_sum;
  timer_sum2    += rhs.timer_sum2;

  rob_active.AddOneROB(rhs.rob_active);
  rob_cached.AddOneROB(rhs.rob_cached);
}

//--------------------------------------------------------------------------------------      
void Anp::RemovedChn::Event::Clear()
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

  rob_active.Clear();
  rob_cached.Clear();
  rois.clear();
}

//--------------------------------------------------------------------------------------      
// Start Hists sub class
//
Anp::RemovedChn::Hists::Hists()
  :isConfig(false),
   hAlgTime(0),
   hCpuTime(0),
   hROBTime(0),
   hROBNreq(0),
   hROBNids(0),
   hROBSize(0)
{
}

//--------------------------------------------------------------------------------------      
void Anp::RemovedChn::Hists::Set(TDirectory *dir, const Registry &reg)
{
  if(!isConfig) {
    isConfig = true;
  }
  else {
    cerr << "RemovedChn::Hists::Set - error: executing 2nd time" << endl;
    return;
  }

  std::string prefix, opts;
  bool save_dir = true;
  reg.Get("RemovedChn::Hists::Prefix", prefix);
  reg.Get("RemovedChn::Hists::Opts",   opts);
  reg.Get("RemovedChn::Hists::Save",   save_dir);

  hAlgTime = Anp::HistMan::Instance().CreateTH1(prefix+"_alg_time", "time_alg");
  hCpuTime = Anp::HistMan::Instance().CreateTH1(prefix+"_cpu_time", "time_alg");
  hROBTime = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_time", "time_alg");
  hROBNreq = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_nreq", "time_alg");
  hROBNids = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_nids", "time_alg");
  hROBSize = Anp::HistMan::Instance().CreateTH1(prefix+"_rob_size", "time_alg");


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
  }
}

//--------------------------------------------------------------------------------------      
//void Anp::RemovedChn::Hists::Fill(const ROBEntry &rob){
//
//}

//--------------------------------------------------------------------------------------      
// Start RemovedChn class
//
Anp::RemovedChn::RemovedChn()
  :fDir(0),
   fDebug(true),
   fCanRebin(true),
   fMakeHists(true),
   fTimeout(0.0),
   fIsConfig(false),
   fLevel(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::RemovedChn::~RemovedChn()
{
}

//--------------------------------------------------------------------------------------      
void Anp::RemovedChn::Config(const Registry &reg)
{

  if(!fIsConfig) {
    fIsConfig = true;
  }
  else {
    cout << "RemovedChn::Config - error: executing second time" << endl;    
  }

  reg.Get("RemovedChn", "Debug",      fDebug);
  reg.Get("RemovedChn", "CanvasType", fCanvasType);
  reg.Get("RemovedChn::HistOpts",     fHistOpts);
  reg.Get("RemovedChn::CanRebin",     fCanRebin);
  reg.Get("RemovedChn::MakeHists",    fMakeHists);
  reg.Get("RemovedChn::Timeout",      fTimeout);
  reg.Get("RemovedChn::Prefix",       fPrefix);

  //
  // Create histograms
  //
  if(fMakeHists) {
    Registry hreg(reg);
    hreg.Set("RemovedChn::Hists::Opts", fHistOpts);

    hreg.Set("RemovedChn::Hists::Prefix", "evt");
    //    fHistEvent.Set(fDir, hreg);

    hreg.Set("RemovedChn::Hists::Prefix", "roi");
    //fHistCalls.Set(fDir, hreg);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::RemovedChn::Finalize()
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

}

//--------------------------------------------------------------------------------------      
unsigned Anp::RemovedChn::GetNameId() const
{
  //
  // Get algorithm name hash id
  //
  if(!fAlgVec.empty()) return fAlgVec.front().getNameId();
  return 0;
}

//--------------------------------------------------------------------------------------      
unsigned Anp::RemovedChn::GetTypeId() const
{
  //
  // Get algorithm name hash id
  //
  if(!fAlgVec.empty()) return fAlgVec.front().getTypeId();
  return 0;
}

//--------------------------------------------------------------------------------------      
const std::string Anp::RemovedChn::GetName() const
{
  //
  // Get algorithm name hash id
  //
  
  return "UNKNOWN";
}

//---------------------------------------------------------------------------------------
double Anp::RemovedChn::GetVar(const std::string &key) const
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
std::string Anp::RemovedChn::GetVarTitle(const std::string &key) const
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
void Anp::RemovedChn::PrintVar(std::ostream &os,
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
double Anp::RemovedChn::ConvertWord2kB(unsigned nwords) const
{
  return 4.0*nwords/1024.0;
}

//---------------------------------------------------------------------------------------
void Anp::RemovedChn::WriteHtml(Handle<HtmlNode> node, TCanvas *) const
{
  //
  // Write html summary
  //
  if(!node.valid()) { 
    return;
  }
  
  ostream &os = node -> GetCont();

  if(node->GetBase().find("ALG_TYPE_3927684433") != string::npos) {
    cout << "RemovedChn::WriteHtml - name: " << GetName() 
	 << " name id: " << GetNameId()
	 << " type id: " << GetTypeId()
	 << endl;
  }
  
  os << "<pre>" << endl;

  //
  // Write list of slowest events
  //
  //Handle<HtmlNode> nodeSE(new HtmlNode(node->GetBase()+"_slwevt.html", HtmlNode::kHTML));
  //node -> AddChild(nodeSE);
 
}

//-----------------------------------------------------------------------------
bool Anp::RemovedChn::algMatchRoi(const TrigMonAlg &alg1, const rmAlgData &alg2, const vector<TimeRoiData> &RoiData) const
{
  if(alg1.getNRoi() > 0 && alg2.alg.getNRoi() > 0) {
    for(unsigned roi =0; roi<alg1.getNRoi();++roi){
      if(alg1.getRoiId(roi) < RoiData.size() && alg2.roiId < RoiData.size()) {
	if(alg1.getRoiId(roi) == alg2.roiId){
      //------------------------REMOVE---------------------------
	  if(fDebug) cout << "RemovedChn::algMatchRoi - SUCCESSFUL Match: rmAlg RoiID: " << int(alg2.roiId) << " rmAlg type: " << alg2.type << " alg.RoiID: " << int(alg1.getRoiId(roi)) << endl;
      //------------------------REMOVE---------------------------
	  return true;
	}
	else{ 
      //------------------------REMOVE---------------------------
	  if(fDebug) cout << "RemovedChn::algMatchRoi - FAILED to Match: rmAlg RoiID: " << int(alg2.roiId) << " rmAlg type: " << alg2.type << " alg.RoiID: " << int(alg1.getRoiId(roi)) << endl;
      //------------------------REMOVE---------------------------
	  return false;
	  }
      } // end data storage check
      else return false;
    }//end loop over rmAlg roi's
  }//end loop over alg1 roi's
  else if(alg1.getNRoi() == 0 && alg2.alg.getNRoi() == 0){
    if(fDebug) cout << "RemovedChn::algMatchRoi - SUCCESSFUL Math: Both ALGs have NO Roi" << endl;
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
bool Anp::RemovedChn::algMatchRob(vector<TrigMonROB> &rob_list,
				const TrigMonAlg &entry, 
				const rmRobData &rmRob, 
				TrigTimeSeq *&data) const
{
  for(vector<TrigMonROB>::iterator rob=rob_list.begin();rob!=rob_list.end();++rob){
    unsigned robrequestID = rob->getRequestorId();
    TrigTimeAlg *alg_type  = data->GetAlgType(entry.getPosition());
    TrigTimeAlg *alg_name  = data->GetAlgName(entry.getPosition());
    unsigned algId = alg_name->GetNameId();
    cout << "RemovedChn::algMatchRob - alg inst:  " << alg_type->GetName() << " with id: " << alg_type->GetTypeId() << endl;
    cout << "RemovedChn::algMatchRob - alg class: " << alg_name->GetName() << " with id: " << alg_name->GetNameId() << endl;
    if(robrequestID==algId){
      //
      // Check the timing is consistent
      //
      vector<unsigned> word = entry.getWord();
      vector<TrigMonROBData> &vecROBData = rob->getData();
      if(word.size()>1){
	TrigMonTimer t_alg_start = entry.start();
	TrigMonTimer t_alg_stop  = entry.stop();
	TrigMonTimer t_rob_start = rob->start();
	TrigMonTimer t_rob_stop  = rob->stop();
	if(t_alg_start < t_rob_start && t_rob_stop < t_alg_stop){
	  for(unsigned r=0; r<vecROBData.size();++r){
	    if(fDebug) cout << "RemovedChn::algMatchRob - robData for rob: " << vecROBData.at(r).getROBId() << "  isRetrieved: " << vecROBData.at(r).isRetrieved() << " isCached: " << vecROBData.at(r).isCached() << endl;
	    if(vecROBData.at(r).isCached() && rmRob.robId==vecROBData.at(r).getROBId()){
	      vecROBData.at(r).setHistory(TrigMonROBData::kRETRIEVED);	
	      if(fDebug) cout << "RemovedChn::algMatchRob - SUCCESSFUL rob extraction : " << vecROBData.at(r).getROBId() << endl;
	      return true;
	    }//end checking if rob is cached and rob id matches
	  }//end loop over TrigMonROBData in TrigMonROB request
	}//end checking that time matches
      }//end check if timing info is present
    }//end matching rob request alg name to alg name in alg_entry
  }
  return false;
}

//-----------------------------------------------------------------------------
Anp::RemovedChn::rmSeqData Anp::RemovedChn::addSeqEntry(const TrigMonSeq &mSeq, TrigTimeSeq *seqData, vector<TrigMonROB> &rob_entry)
{
  //
  // Find Seq or add to Map
  //
  SeqMap::iterator iter = fSeqMap.find(seqData->GetId());
  if(iter != fSeqMap.end()) return iter->second;
  //
  // Configure rmSeqData on demand
  //
  rmSeqData data;
  data.seqId           = seqData->GetId();
  data.seqName         = seqData->GetName();
  data.level           = seqData->GetLevel();
  TrigConfSeq confSeq  = seqData->GetConfSeq();
  data.seqIndex        = confSeq.getIndex();
  data.seq.push_back(mSeq);
  data.addAlgs(seqData);
  if(data.level == 0 || data.level == 2) data.addRobs(rob_entry,seqData);
  fSeqMap.insert(SeqMap::value_type(seqData->GetId(), data));
  return data;
}

//-----------------------------------------------------------------------------
void Anp::RemovedChn::resetSeq(TrigMonSeq &seq, TrigTimeSeq *data)
{
  //
  // Loop over removed sequences and reset cached ones
  //   - reset algs
  //   - reset robs
  //
  SeqMap::iterator replac = fSeqMap.find(data->GetId());
  if(replac != fSeqMap.end()){
    if(replac->second.seq.size()>0){
      TrigMonSeq &oldSeq = replac->second.seq.at(0);
  //
  // Adding back time and resetting status of alg
  //
      //------------------------REMOVE---------------------------
      if(fDebug) {
	cout << "RemovedChn::resetSeq - adding back time to cached Seq"               << endl;
	cout << "RemovedChn::resetSeq - oldSeq status:  " << oldSeq.isExecuted()      << endl;
	cout << "RemovedChn::resetSeq - seq status:    "  << seq.isAlreadyExecuted()  << endl;
	cout << "RemovedChn::resetSeq - seq timer:     "  << seq.getSeqTimer()        << endl;
	cout << "RemovedChn::resetSeq - oldSeq timer:   " << oldSeq.getSeqTimer()     << endl;
      }
      //------------------------REMOVE---------------------------
      TrigMonSeq resetSeq(oldSeq.getEncoded());
      //resetAlgs(resetSeq, oldSeq);
      // Reset Algs
      //
      vector<TrigMonAlg> algs = oldSeq.getAlg();
      for(unsigned alg=0;alg<algs.size();++alg) resetSeq.addAlg(algs.at(alg));

      for(unsigned var =0;var<seq.getVar().size();++var) resetSeq.addVar(seq.getVar().at(var));
      resetSeq.addTimer(oldSeq.getSeqTimer());
      //------------------------REMOVE---------------------------
      if(fDebug) {
	cout << "RemovedChn::resetCachedSeq - oldSeq nAlgs: " << algs.size() << "  resetSeq #algs " << resetSeq.getAlg().size() << " thinks cach seq #alg: " << seq.getAlg().size() << endl;
	cout << "RemovedChn::resetCachedSeq - oldSeqState initial:             " << oldSeq.isInitial()     << " exe: " << oldSeq.isExecuted() 
	     << " alrdExe: " << oldSeq.isAlreadyExecuted() << " prev: "        << oldSeq.isPrevious()    << endl;
	cout << "RemovedChn::resetCachedSeq - Thinks Cached seqState initial: " << seq.isInitial()       << " exe: " << seq.isExecuted() 
	     << " alrdExe: " << seq.isAlreadyExecuted() << " prev: "          << seq.isPrevious()      << endl;
	cout << "RemovedChn::resetCachedSeq - resetSeqState initial:          " << resetSeq.isInitial()  << " exe: " << resetSeq.isExecuted() 
	     << " alrdExe: " << resetSeq.isAlreadyExecuted() << " prev: "     << resetSeq.isPrevious() << endl;
      }
      //------------------------REMOVE---------------------------
      //should already be inside of the algorithms
      seq = resetSeq;
      //
      // It is important to erase the replaced seq so that the associated algs and robs do not get propogated later on
      //
      fSeqMap.erase(replac);
    }
    else cerr << "RemovedChn::resetSeq - missing Seq data" << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::RemovedChn::resetRobs(vector<TrigMonROB> &rob_list,const TrigMonAlg &entry, TrigTimeSeq *data) 
{
  //
  // Reset robs that think they are cached
  //
    for(vector<TrigMonROB>::iterator rob=rob_list.begin();rob!=rob_list.end();++rob){
    unsigned robrequestID = rob->getRequestorId();
    //TrigTimeAlg *alg_type  = data->GetAlgType(entry.getPosition());
    TrigTimeAlg *alg_name  = data->GetAlgName(entry.getPosition());
    unsigned algId = alg_name->GetTypeId();
    if(robrequestID==algId){
      //
      // Check the timing is consistent
      //
      vector<unsigned> word = entry.getWord();
      vector<TrigMonROBData> &vecROBData = rob->getData();
      if(word.size()>1){
	TrigMonTimer t_alg_start = entry.start();
	TrigMonTimer t_alg_stop  = entry.stop();
	TrigMonTimer t_rob_start = rob->start();
	TrigMonTimer t_rob_stop  = rob->stop();
	if(t_alg_start < t_rob_start && t_rob_stop < t_alg_stop){
	  for(unsigned r=0; r<vecROBData.size();++r){
	    if(fDebug) cout << "RemovedChn::resetRobs - robData for rob: " << vecROBData.at(r).getROBId() << "  isRetrieved: " << vecROBData.at(r).isRetrieved() << " isCached: " << vecROBData.at(r).isCached() << endl;
	    
	    for(SeqMap::iterator mapit = fSeqMap.begin(); mapit!=fSeqMap.end(); ++mapit){
	      vector<rmRobData> &rmRobs = mapit->second.getRobs();
	      for(unsigned rrob = 0; rrob<rmRobs.size(); ++rrob){
		vector<TrigMonROBData> &rDatas = rmRobs.at(rrob).rob.getData();
		for(unsigned rdata = 0; rdata<rDatas.size(); ++rdata){
		  if(vecROBData.at(r).isCached() && rDatas.at(rdata).getROBId()==vecROBData.at(r).getROBId()){
		    vecROBData.at(r).setHistory(TrigMonROBData::kRETRIEVED);
		    rDatas.at(rdata).setHistory(TrigMonROBData::kCACHED);
		    if(fDebug) cout << "RemovedChn::resetRobs - SUCCESSFUL rob extraction : " << vecROBData.at(r).getROBId() << endl;
		    if(fDebug) cout << "RemovedChn::resetRobs - after reset hist to retrieved: " << vecROBData.at(r).getROBId() << "  isRetrieved: " << vecROBData.at(r).isRetrieved() << " isCached: " << vecROBData.at(r).isCached() << endl;
		    if(fDebug) cout << "RemovedChn::resetRobs - after reset hist to retrieved - removed rob should now be cached: " << rDatas.at(rdata).isCached() << endl;
		    break;
		  }//end checking if rob is cached and rob id matches
		}
	      }
	    }
	  }//end loop over TrigMonROBData in TrigMonROB request
	}//end checking that time matches
      }//end check if timing info is present
    }//end matching rob request alg name to alg name in alg_entry
  }
}

//-----------------------------------------------------------------------------
void Anp::RemovedChn::resetAlgs(vector<TrigMonAlg> &resetAlgs, const std::vector<TimeRoiData> &RoiData, TrigTimeSeq *data) 
{
  //
  // Reset algs that think they are cached
  //   - Collect all algs and match algs
  //
  //  vector<TrigMonAlg> &resetAlgs = resetSeq.getAlg();
  for(unsigned resetAlg = 0; resetAlg<resetAlgs.size();++resetAlg){
    if(resetAlgs.at(resetAlg).isCached()){
      TrigTimeAlg *alg_type = data->GetAlgType(resetAlgs.at(resetAlg).getPosition());
      for(SeqMap::iterator mapit = fSeqMap.begin(); mapit!=fSeqMap.end(); ++mapit){
	vector<rmAlgData> &rmAlgs = mapit->second.getAlgs();
	for(unsigned ralg = 0; ralg<rmAlgs.size(); ++ralg){
	  if(rmAlgs.at(ralg).type == alg_type->GetName() && algMatchRoi(resetAlgs.at(resetAlg),rmAlgs.at(ralg),RoiData) && !(rmAlgs.at(ralg).alg.isCached())){
	    TrigMonAlg replacAlg(resetAlgs.at(resetAlg).getPosition(),rmAlgs.at(ralg).alg.isCached());
	    for(unsigned roi=0;roi<resetAlgs.at(resetAlg).getNRoi();++roi) replacAlg.addRoiId(resetAlgs.at(resetAlg).getRoiId(roi));
	    //replacAlg->addWord(unsigned int word);
	    replacAlg.addTimer(rmAlgs.at(ralg).alg.start(),rmAlgs.at(ralg).alg.stop());
	    rmAlgs.at(ralg).alg = resetAlgs.at(resetAlg);
	    resetAlgs.at(resetAlg) = replacAlg;
	    break;
	  }//end check that removed alg isCalled, algs match roi, and match name
	}//end loop over removed algs
      }
    }
  }
}

