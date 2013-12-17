/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>

// ROOT
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HistMan.h"

// Local
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/RobService.h"

REGISTER_ANP_OBJECT(ExecClient,ExecCount)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecCount::ExecCount()
  :fDir(0),
   fDebug  (false),
   fDoAlg  (false),
   fDoDet  (false),
   fDoROS  (false),
   fDoROBIN(false),
   fDoRoi  (false),
   fDoChn  (false),
   fDoSeq  (false),
   fMakeHist(false),
   fMakeAlgHist(false),
   fAlgId(0),
   fDetId(0),
   fROSId(0),
   fROBINId(0),
   fChnId(0),
   fSeqId(0),
   fSelectAlg  (false),
   fSelectDet  (false),
   fSelectROS  (false),
   fSelectROBIN(false),
   fSelectRoi  (false),
   fSelectChn  (false),
   fSelectSeq  (false),
   fTimeout(0.0)
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecCount::~ExecCount()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecCount::SetReg(const Registry &reg)
{
  //
  // Read configuration
  //
  reg.Get("Debug",       fDebug);
  reg.Get("DoAlg",       fDoAlg);
  reg.Get("DoDet",       fDoDet);
  reg.Get("DoROS",       fDoROS);
  reg.Get("DoROBIN",     fDoROBIN);
  reg.Get("DoRoi",       fDoRoi);
  reg.Get("DoChn",       fDoChn);
  reg.Get("DoSeq",       fDoSeq);
  reg.Get("Level",       fLevel);
  reg.Get("Timeout",     fTimeout);
  reg.Get("MakeHist",    fMakeHist);
  reg.Get("MakeAlgHist", fMakeAlgHist);
  reg.Get("HistKey",     fHistKey);
  //reg.Get("DoLBHists",   fCountEvent.get_roi_time);

  fSelectAlg   = reg.Get("AlgId",   fAlgId);
  fSelectDet   = reg.Get("DetId",   fDetId);
  fSelectROS   = reg.Get("ROSId",   fROSId);
  fSelectROBIN = reg.Get("ROBINId", fROBINId);
  fSelectRoi   = fDoRoi;
  fSelectChn   = reg.Get("ChnId", fChnId);
  fSelectSeq   = reg.Get("SeqId", fSeqId);

  fCountEvent.limit_slow = fTimeout;
  fCountCalls.limit_slow = fTimeout;
  fCountTotal.limit_slow = fTimeout;

  if(fSelectAlg)   { fCountTotal.alg_id     = fAlgId; fCountCalls.alg_id = fAlgId; fCountEvent.alg_id = fAlgId; }
  if(fSelectDet)   { fCountCalls.select_det   = true; fCountCalls.det_id = fDetId;                              }
  if(fSelectROS)   { fCountCalls.select_ros   = true; fCountCalls.ros_id = fROSId;                              }
  if(fSelectROBIN) { fCountCalls.select_robin = true; fCountCalls.robin_id = fROBINId;                          }
  if(fSelectRoi)   { fCountCalls.select_roi   = true;                                                           }
  if(fSelectChn)   { fCountTotal.chn_id     = fChnId; fCountCalls.chn_id = fChnId; fCountEvent.chn_id = fChnId; }
  if(fSelectSeq)   { fCountTotal.seq_id     = fSeqId; fCountCalls.seq_id = fSeqId; fCountEvent.seq_id = fSeqId; }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecCount::SetDir(TDirectory *dir)
{
  //
  // Get directory and create histograms
  //    -- create histograms for each LB or when all histograms are requested
  //
  if(!fMakeHist && !fMakeAlgHist) return;

  if(fDebug){
    cout << "ExecCount::SetDir - making histograms! " << GetClientName() << endl;
  }

  fDir = dir;
  
  hAlgCalls.hAlgTime     = GetTH1("calls_alg_time",    "Algorithm time (ms)",            "Algorithm calls");
  hAlgEvent.hAlgTime     = GetTH1("event_alg_time",    "Algorithm time (ms)",            "Events"); 
  hAlgEvent.hAlgNCalls   = GetTH1("event_num_calls",   "Algorithm Calls",                "Events"); 
  hAlgEvent.hAlgNCache   = GetTH1("event_num_cache",   "Algorithm Caches",               "Events");
  
  if(GetLevel() == 0 || GetLevel() == 2){

    hAlgEvent.hROBTime      = GetTH1("event_rob_time",          "ROB time (ms)",  "Events");
    hROBEvent.hROBNreq      = GetTH1("event_rob_nreq",          "ROB Requests",   "Events");
    hROBEvent.hROBNreqRetrv = GetTH1("event_rob_nreq_retrv",    "ROB Retrievals", "Events");

    hROSEvent.hROSNreqRetrv    = GetTH1("event_ros_nreq_retrv", "ROS Requests",   "Events");  	    
    hROSEvent.hROSSizeRetrv    = GetTH1("event_ros_size_retrv", "ROB Retrievals", "Events");
  } 

  //
  // Make all histograms
  //
  if(!fMakeHist) return;
  
  hAlgEvent.hAlgTimeFrac      = GetTH1("event_time_frac");
  hAlgEvent.hCpuTime          = GetTH1("event_cpu_time");
  hAlgEvent.hAlgTimeFirstCall = GetTH1("event_first_alg_time");

  hAlgCalls.hCpuTime = GetTH1("calls_cpu_time");  

  if(GetLevel() == 0 || GetLevel() == 2) {

    hAlgCalls.hROBTime      = GetTH1("calls_rob_time"      );    
    hROBCalls.hROBNreq      = GetTH1("calls_rob_nreq"      );
    hROBCalls.hROBNreqRetrv = GetTH1("calls_rob_nreq_retrv");
    hROBCalls.hROBSize      = GetTH1("calls_rob_size"      );
    hROBCalls.hROBSizeRetrv = GetTH1("calls_rob_size_retrv");
    
    hROBEvent.hROBSize      = GetTH1("event_rob_size"      );
    hROBEvent.hROBSizeRetrv = GetTH1("event_rob_size_retrv");

    hROSEvent.hROSSize         = GetTH1("event_ros_size");
    hROSEvent.hROSNreq         = GetTH1("event_ros_nreq"); 

    if(fDoROS){
      hROSCalls.hROSNreq         = GetTH1("calls_ros_nreq"        ); 	  
      hROSCalls.hROSNreqRetrv    = GetTH1("calls_ros_nreq_retrv"  );
      hROSCalls.hROSSize         = GetTH1("calls_ros_size"        );	  
      hROSCalls.hROSSizeRetrv    = GetTH1("calls_ros_size_retrv"  );
      hROSCalls.hROSNrobids      = GetTH1("calls_ros_robids"      );
      hROSCalls.hROSNrobidsRetrv = GetTH1("calls_ros_robids_retrv");
    }

    if(fDoRoi){
      hROSRoi.hROSNreq         = GetTH1("roi_ros_nreq"      );      
      hROSRoi.hROSNreqRetrv    = GetTH1("roi_ros_nreq_retrv");
      hROSRoi.hROSSize         = GetTH1("roi_ros_size"      );
      hROSRoi.hROSSizeRetrv    = GetTH1("roi_ros_size_retrv");
      hROSRoi.hROSNrobids      = GetTH1("roi_ros_robids"    );
    }
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecCount::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Check selection flags
  //
  if(fSelectAlg && fAlgId != alg.alg_conf->getNameId()) {
    return;
  }
  if(fSelectChn && fChnId != alg.chn_conf->getEncodedId()) {
    return;
  }
  if(fSelectSeq && fSeqId != alg.seq_conf->getId()) {
    return;
  }
  if(fSelectDet && alg.det_ids.count(fDetId) == 0) {
    return;
  }
  if(fSelectROS && alg.ros_ids.count(fROSId) == 0) {
    return;
  }
  if(fSelectROBIN && alg.robin_ids.count(fROBINId) == 0) {
    return;
  }

  //
  // Count algorithm
  //
  fCountCalls.AddCount(alg);
  
  //
  // Fill histograms
  //
  if(alg.alg_data.isCalled()) {
    if(hAlgCalls.hAlgTime) hAlgCalls.hAlgTime->Fill(fCountCalls.timer_alg);
    if(hAlgCalls.hCpuTime) hAlgCalls.hCpuTime->Fill(fCountCalls.timer_alg-fCountCalls.timer_rob);    

    if(fCountCalls.rob_nreq) {
      if(hAlgCalls.hROBTime)      hAlgCalls.hROBTime     ->Fill(fCountCalls.timer_rob);
      if(hROBCalls.hROBNreq)      hROBCalls.hROBNreq     ->Fill(fCountCalls.rob_nreq);
      if(hROBCalls.hROBNreqRetrv) hROBCalls.hROBNreqRetrv->Fill(fCountCalls.rob_nreq_retrv);      
      if(hROBCalls.hROBSize)      hROBCalls.hROBSize     ->Fill(Exec::ConvertWordsToKB(fCountCalls.rob_size));
      if(hROBCalls.hROBSizeRetrv) hROBCalls.hROBSizeRetrv->Fill(Exec::ConvertWordsToKB(fCountCalls.rob_size_retrv));
    }
  }

  //
  // Add to total count
  //
  fCountEvent.AddCount(fCountCalls);
  fCountCalls.Clear();

  //
  // If SelectAlg and alg time > 0, then this is a first call to that algorithm
  //
  if(fSelectAlg && (fCountEvent.alg_ncalls+fCountEvent.alg_ncache)==1){
    if(hAlgEvent.hAlgTimeFirstCall) hAlgEvent.hAlgTimeFirstCall->Fill(fCountEvent.timer_alg);
  }
  else if(fSelectAlg && fCountEvent.alg_ncalls==0){
    if(fDebug){
      cout << "EventCount::ProcessAlg - WARNING - Selecting algorithm " 
	   << alg.alg_conf->getName()
	   << " and it has 0 run time for the entire event...could be missing first Call!"
	   << endl;
    }
  }

  //
  // Fill counts specific for algorithms, detectors and ROSs
  //
  if(fDoAlg) {
    Exec::Count &count = GetAlgCount(alg.alg_conf->getNameId());
    count.ros_nrob.clear();
    count.ros_nrob_retrv.clear();
    count.robin_nrob.clear();
    count.robin_nrob_retrv.clear();
    count.AddCount(alg);
  }

  if(fDoChn) {
    Exec::Count &count = GetChnCount(alg.chn_conf->getEncodedId());
    count.ros_nrob.clear();
    count.ros_nrob_retrv.clear();
    count.robin_nrob.clear();
    count.robin_nrob_retrv.clear();
    count.AddCount(alg);
    fCountChnVec.push_back(&count);
  }

  if(fDoSeq) {
    Exec::Count &count = GetSeqCount(alg.seq_conf->getId());
    count.ros_nrob.clear();
    count.ros_nrob_retrv.clear();
    count.robin_nrob.clear();
    count.robin_nrob_retrv.clear();
    count.AddCount(alg);
    fCountSeqVec.push_back(&count);
  }

  if(fDoDet) {
    for(set<uint32_t>::const_iterator it = alg.det_ids.begin(); it != alg.det_ids.end(); ++it) {
      Exec::Count &det_count = GetDetCount(*it);
      Exec::Count &grp_count = GetGrpCount(*it);
      det_count.ros_nrob.clear();
      det_count.ros_nrob_retrv.clear();
      det_count.robin_nrob.clear();
      det_count.robin_nrob_retrv.clear();
      det_count.AddCount(alg);
      grp_count.ros_nrob.clear();
      grp_count.ros_nrob_retrv.clear();
      grp_count.robin_nrob.clear();
      grp_count.robin_nrob_retrv.clear();
      grp_count.AddCount(alg);
      fCountDetVec.push_back(&det_count);
      fCountDetVec.push_back(&grp_count);
    }
  }

  if(fDoROS) {
    for(set<uint32_t>::const_iterator it = alg.ros_ids.begin(); it != alg.ros_ids.end(); ++it) {
      Exec::Count &count = GetROSCount(*it);
      count.ros_nrob.clear();
      count.ros_nrob_retrv.clear();

      Exec::Count rosCount;
      rosCount.select_ros = true; 
      rosCount.ros_id     = *it; 
      rosCount.AddCount(alg);

      count.AddCount(rosCount);
      //
      // Only do the following for the ros id specified
      //
      if(*it != fROSId) continue;

      if(rosCount.ros_nreq>0) {
	//
	// setting var to true if the retrievals are zero for this ROS
	//
	if(hROSCalls.hROSNreq)         hROSCalls.hROSNreq        ->Fill(rosCount.ros_nreq);
	if(hROSCalls.hROSNreqRetrv)    hROSCalls.hROSNreqRetrv   ->Fill(rosCount.ros_nreq_retrv); 
	if(hROSCalls.hROSSize)         hROSCalls.hROSSize        ->Fill(Exec::ConvertWordsToKB(rosCount.rob_size));
	if(hROSCalls.hROSSizeRetrv)    hROSCalls.hROSSizeRetrv   ->Fill(Exec::ConvertWordsToKB(rosCount.rob_size_retrv));
	if(hROSCalls.hROSNrobids){   
	  for(unsigned j=0;j<rosCount.ros_nrob.size();++j)       hROSCalls.hROSNrobids->Fill(rosCount.ros_nrob.at(j));
	}
	if(hROSCalls.hROSNrobidsRetrv){
	  for(unsigned j=0;j<rosCount.ros_nrob_retrv.size();++j) hROSCalls.hROSNrobidsRetrv->Fill(rosCount.ros_nrob_retrv.at(j)); 
	}
      }
      rosCount.Clear();

    }
  }

  if(fDoROBIN) {
    for(set<uint32_t>::const_iterator it = alg.robin_ids.begin(); it != alg.robin_ids.end(); ++it) {
      Exec::Count &count = GetROBINCount(*it);
      count.robin_nrob.clear();
      count.robin_nrob_retrv.clear();

      Exec::Count robinCount;
      robinCount.select_robin = true; 
      robinCount.robin_id     = *it; 
      robinCount.AddCount(alg);

      count.AddCount(robinCount);

      //                                                                                                                                                  
      // Only do the following for the ros id specified       
      //                                          
      if(*it != fROBINId) continue;

      if(robinCount.robin_nreq>0) {

      }
      robinCount.Clear();
    }

  }

  if(fDoRoi) {
    set<unsigned> roi_ids;

    for(vector<Exec::ROILoad>::const_iterator iroi = alg.roi_data.begin(); iroi !=alg.roi_data.end();++iroi){
      if(iroi->roi_valid) {
	roi_ids.insert(iroi->roi_id);
      }
    }
    for(set<uint32_t>::const_iterator it = roi_ids.begin(); it != roi_ids.end(); ++it) {
      Exec::Count &count = GetRoiCount(*it);
      count.AddCount(alg);
    }

  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecCount::FinishEvent(const Exec::Event &event)
{
  //
  // Fill histograms
  //
  if(hAlgEvent.hAlgTime)   hAlgEvent.hAlgTime  ->Fill(fCountEvent.timer_alg);
  if(hAlgEvent.hCpuTime)   hAlgEvent.hCpuTime  ->Fill(fCountEvent.timer_alg-fCountEvent.timer_rob);  
  if(hAlgEvent.hAlgNCalls) hAlgEvent.hAlgNCalls->Fill(fCountEvent.alg_ncalls);
  if(hAlgEvent.hAlgNCache) hAlgEvent.hAlgNCache->Fill(fCountEvent.alg_ncache);

  if(hAlgEvent.hAlgTimeFrac) { 
    if(event.timer_alg > 0.0) {
      hAlgEvent.hAlgTimeFrac->Fill(fCountEvent.timer_alg/event.timer_alg);
    }
    else {
      hAlgEvent.hAlgTimeFrac->Fill(0.0);
    }
  }

  if(fCountEvent.rob_nreq) {
    if(hAlgEvent.hROBTime)      hAlgEvent.hROBTime     ->Fill(fCountEvent.timer_rob);
    if(hROBEvent.hROBNreq)      hROBEvent.hROBNreq     ->Fill(fCountEvent.rob_nreq);
    if(hROBEvent.hROBNreqRetrv) hROBEvent.hROBNreqRetrv->Fill(fCountEvent.rob_nreq_retrv);
    if(hROBEvent.hROBSize)      hROBEvent.hROBSize     ->Fill(Exec::ConvertWordsToKB(fCountEvent.rob_size));
    if(hROBEvent.hROBSizeRetrv) hROBEvent.hROBSizeRetrv->Fill(Exec::ConvertWordsToKB(fCountEvent.rob_size_retrv));
  }

  if(fCountEvent.ros_nreq) {
    if(hROSEvent.hROSNreq)         hROSEvent.hROSNreq        ->Fill(fCountEvent.ros_nreq);
    if(hROSEvent.hROSNreqRetrv)    hROSEvent.hROSNreqRetrv   ->Fill(fCountEvent.ros_nreq_retrv); 
    if(hROSEvent.hROSSize)         hROSEvent.hROSSize        ->Fill(Exec::ConvertWordsToKB(fCountEvent.rob_size));
    if(hROSEvent.hROSSizeRetrv)    hROSEvent.hROSSizeRetrv   ->Fill(Exec::ConvertWordsToKB(fCountEvent.rob_size_retrv));
  }
  //
  // Add ROI counts to histos and clear roi map
  //
  for(CountMap::iterator itroi = fCountRoi.begin(); itroi !=fCountRoi.end();++itroi){
    const Exec::Count &roiCount = itroi->second;
    if(hROSRoi.hROSNreq)         hROSRoi.hROSNreq        ->Fill(roiCount.ros_nreq);
    if(hROSRoi.hROSSize)         hROSRoi.hROSSize        ->Fill(Exec::ConvertWordsToKB(roiCount.rob_size));
    if(roiCount.ros_nreq_retrv>0){
      if(hROSRoi.hROSNreqRetrv)    hROSRoi.hROSNreqRetrv   ->Fill(roiCount.ros_nreq_retrv);
      if(hROSRoi.hROSSizeRetrv)    hROSRoi.hROSSizeRetrv   ->Fill(Exec::ConvertWordsToKB(roiCount.rob_size_retrv));
    }
    fCountRoiEvt.AddCount(roiCount);
    if(roiCount.ros_nreq_retrv==0){ 
      ++fCountTotal.ros_n0ret;
    }
  }
  
  fCountRoi.clear();

  //
  // Count the Chain and subdetector Events
  //
  std::sort(fCountChnVec.begin(), fCountChnVec.end());
  std::sort(fCountSeqVec.begin(), fCountSeqVec.end());
  std::sort(fCountDetVec.begin(), fCountDetVec.end());

  VecCount::iterator it = std::unique(fCountChnVec.begin(), fCountChnVec.end());
  fCountChnVec.erase(it, fCountChnVec.end());
  it = std::unique(fCountDetVec.begin(), fCountDetVec.end());
  fCountDetVec.erase(it, fCountDetVec.end());
  it = std::unique(fCountSeqVec.begin(), fCountSeqVec.end());
  fCountSeqVec.erase(it, fCountSeqVec.end());

  for(unsigned i = 0; i < fCountChnVec.size(); ++i) {
    fCountChnVec.at(i)->AddCount(event);
  }
  for(unsigned i = 0; i < fCountSeqVec.size(); ++i) {
    fCountSeqVec.at(i)->AddCount(event);
  }
  for(unsigned i = 0; i < fCountDetVec.size(); ++i) {  
    fCountDetVec.at(i)->AddCount(event);
  }

  fCountSeqVec.clear();
  fCountChnVec.clear();
  fCountDetVec.clear();

  //
  // Count one event
  //  
  fCountEvent.AddCount(event);
  fCountTotal.AddCount(fCountEvent);
  fCountEvent.Clear();
  fCountRoiEvt.AddCount(event);
  fCountRoiTot.AddCount(fCountRoiEvt);
  fCountRoiEvt.Clear();
}

//---------------------------------------------------------------------------------------
const Anp::Exec::Count& Anp::ExecCount::GetCount() const
{
  return fCountTotal;
}

//---------------------------------------------------------------------------------------
void Anp::ExecCount::ClearCount()
{
  fCountTotal.Clear();
  fCountAlg.clear();
  fCountROS.clear();
  fCountROBIN.clear();
  fCountDet.clear();
  fCountGrp.clear();
  fCountChn.clear();
  fCountSeq.clear();
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCount::GetAlgCount(uint32_t alg_id)
{
  CountMap::iterator it = fCountAlg.find(alg_id);
  if(it == fCountAlg.end()) {
    it = fCountAlg.insert(CountMap::value_type(alg_id, Exec::Count())).first;
    
    if(fSelectDet)   { it->second.select_det   = true;    it->second.det_id   = fDetId;   }
    if(fSelectROS)   { it->second.select_ros   = true;    it->second.ros_id   = fROSId;   }
    if(fSelectROBIN) { it->second.select_robin = true;    it->second.robin_id = fROBINId; }
    if(fSelectChn)   {/*it->second.select_chn  = true;*/  it->second.chn_id   = fChnId;   }
    if(fSelectSeq)   {/*it->second.select_seq  = true;*/  it->second.seq_id   = fSeqId;   }
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCount::GetGrpCount(uint32_t det_id)
{
  const RobService &rob_svc = RobService::Instance();
  unsigned grp_id = rob_svc.GetGroupIdFromSubDetId(det_id);
  CountMap::iterator it = fCountGrp.find(grp_id);
  if(it == fCountGrp.end()) {
    it = fCountGrp.insert(CountMap::value_type(grp_id, Exec::Count())).first;

    it->second.select_det = true; 
    it->second.det_id     = grp_id; 
  }
  return it->second;
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCount::GetDetCount(uint32_t det_id)
{
  CountMap::iterator it = fCountDet.find(det_id);
  if(it == fCountDet.end()) {
    it = fCountDet.insert(CountMap::value_type(det_id, Exec::Count())).first;
    
    it->second.select_det = true; 
    it->second.det_id     = det_id; 
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCount::GetROSCount(uint32_t ros_id)
{
  CountMap::iterator it = fCountROS.find(ros_id);
  if(it == fCountROS.end()) {
    it = fCountROS.insert(CountMap::value_type(ros_id, Exec::Count())).first;
    
    it->second.select_ros = true; 
    it->second.ros_id     = ros_id; 
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCount::GetROBINCount(uint32_t robin_id)
{
  CountMap::iterator it = fCountROBIN.find(robin_id);
  if(it == fCountROBIN.end()) {
    it = fCountROBIN.insert(CountMap::value_type(robin_id, Exec::Count())).first;
    
    it->second.select_robin = true; 
    it->second.robin_id     = robin_id; 
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCount::GetChnCount(uint32_t chn_id)
{
  CountMap::iterator it = fCountChn.find(chn_id);
  if(it == fCountChn.end()) {
    it = fCountChn.insert(CountMap::value_type(chn_id, Exec::Count())).first;
    
    it->second.chn_id = chn_id; 

    if(fSelectDet)   { it->second.select_det   = true; it->second.det_id   = fDetId;   }
    if(fSelectROS)   { it->second.select_ros   = true; it->second.ros_id   = fROSId;   }
    if(fSelectROBIN) { it->second.select_robin = true; it->second.robin_id = fROBINId; }
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCount::GetSeqCount(uint32_t seq_id)
{
  CountMap::iterator it = fCountSeq.find(seq_id);
  if(it == fCountSeq.end()) {
    it = fCountSeq.insert(CountMap::value_type(seq_id, Exec::Count())).first;
    
    it->second.seq_id = seq_id; 

    if(fSelectDet)   { it->second.select_det   = true; it->second.det_id   = fDetId;   }
    if(fSelectROS)   { it->second.select_ros   = true; it->second.ros_id   = fROSId;   }
    if(fSelectROBIN) { it->second.select_robin = true; it->second.robin_id = fROBINId; }
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCount::GetRoiCount(uint32_t roi_id)
{
  CountMap::iterator it = fCountRoi.find(roi_id);
  if(it == fCountRoi.end()) {
    it = fCountRoi.insert(CountMap::value_type(roi_id, Exec::Count())).first;
    
    it->second.select_roi = true; 
    it->second.roi_id     = roi_id; 

    if(fSelectROBIN){ it->second.select_robin = true;  it->second.robin_id     = fROBINId; }
    if(fSelectROS)  { it->second.select_ros   = true;  it->second.ros_id       = fROSId;   }
    if(fSelectDet)  { it->second.select_det   = true;  it->second.det_id       = fDetId;   }
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
TH1* Anp::ExecCount::GetTH1(const std::string &key,
			    const std::string &xaxis,
			    const std::string &yaxis) const
{
  //
  // If this is a full summary LB, write alg timing histograms
  //
  if(fMakeAlgHist && !fMakeHist){

    TH1 *h1 = new TH1F(key.c_str(), "timer", 200, 0.0, -1.0);

    h1->SetDirectory(fDir);
    h1->GetXaxis()->CenterTitle();
    h1->GetXaxis()->SetTitle(xaxis.c_str());
    h1->GetYaxis()->CenterTitle();
    h1->GetYaxis()->SetTitle(yaxis.c_str()); 

    return h1;
  }
  else{
    //
    // Histograms are owned by TDirectory
    //
    if(fDebug) { 
      cout << "ExecCount::GetTH1 - hist key : " << key << " xaxis: " << xaxis 
	   << " fHistKey: " <<  fHistKey << " yaxis " << yaxis << endl;
    }

    TH1 *h = Anp::HistMan::Instance().CreateTH1(key, fHistKey);
  
    if(h && !key.empty()) {
      h->SetTitle(key.c_str());
    }

    Anp::SetDir(h, fDir);
    
    return h;
  }

  return NULL;
}
