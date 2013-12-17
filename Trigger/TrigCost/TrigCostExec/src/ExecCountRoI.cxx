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
#include "TrigCostExec/ExecCountRoI.h"

REGISTER_ANP_OBJECT(ExecClient,ExecCountRoI)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecCountRoI::ExecCountRoI()
  :fDir(0),
   fDebug(true),
   fDoAlg(false),
   fDoDet(false),
   fDoROS(false),
   fDoChn(false),
   fMakeHist(false),
   fAlgId(0),
   fDetId(0),
   fROSId(0),
   fChnId(0),
   fSelectAlg(false),
   fSelectDet(false),
   fSelectROS(false),
   fSelectChn(false),
   fTimeout(0.0)
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecCountRoI::~ExecCountRoI()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecCountRoI::SetReg(const Registry &reg)
{
  //
  // Read configuration
  //
  reg.Get("Debug",    fDebug);
  reg.Get("DoAlg",    fDoAlg);
  reg.Get("DoDet",    fDoDet);
  reg.Get("DoROS",    fDoROS);
  reg.Get("DoChn",    fDoChn);
  reg.Get("Level",    fLevel);
  reg.Get("Timeout",  fTimeout);
  reg.Get("MakeHist", fMakeHist);
  reg.Get("HistKey",  fHistKey);

  fSelectAlg = reg.Get("AlgId", fAlgId);
  fSelectDet = reg.Get("DetId", fDetId);
  fSelectROS = reg.Get("ROSId", fROSId);
  fSelectChn = reg.Get("ChnId", fChnId);

  fCountTotal.limit_slow = fTimeout;

  //  if(fSelectDet) { fCountCalls.select_det = true; fCountCalls.det_id = fDetId; }
  //  if(fSelectROS) { fCountCalls.select_ros = true; fCountCalls.ros_id = fROSId; }
  //  if(fSelectChn) { fCountTotal.chn_id   = fChnId; fCountCalls.chn_id = fChnId; }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecCountRoI::SetDir(TDirectory *dir)
{
  //
  // Get directory and create histograms
  //
  if(!fMakeHist) return;
  
  fDir = dir;
  
  hAlgCalls.hAlgTime     = GetTH1("calls_alg_time");
  hAlgEvent.hAlgTime     = GetTH1("event_alg_time");
  hAlgEvent.hAlgTimeFrac = GetTH1("event_time_frac"); 
  hAlgEvent.hAlgNCalls   = GetTH1("event_num_calls"); 
  hAlgEvent.hAlgNCache   = GetTH1("event_num_cache"); 
  
  if(GetLevel() == 0 || GetLevel() == 2) {
    hAlgCalls.hCpuTime = GetTH1("calls_cpu_time");
    hAlgCalls.hROBTime = GetTH1("calls_rob_time");
    hAlgEvent.hCpuTime = GetTH1("event_cpu_time");
    hAlgEvent.hROBTime = GetTH1("event_rob_time");

    
    hROSRoi.hROSNreq         = GetTH1("roi_ros_nreq");      
    hROSRoi.hROSNreqRetrv    = GetTH1("roi_ros_nreq_retrv");
    hROSRoi.hROSSize         = GetTH1("roi_ros_size");
    hROSRoi.hROSSizeRetrv    = GetTH1("roi_ros_size_retrv");
    hROSRoi.hROSNrobids      = GetTH1("roi_ros_robids");
    
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecCountRoI::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Check selection flags
  //
  if(fSelectAlg && fAlgId != alg.alg_conf->getNameId()) {
    return;
  }
  if(fSelectDet && alg.det_ids.count(fDetId) == 0) {
    return;
  }  
  if(fSelectROS && alg.ros_ids.count(fROSId) == 0) {
    return;
  }

  if(fDoROS) {
    for(set<uint32_t>::const_iterator it = alg.ros_ids.begin(); it != alg.ros_ids.end(); ++it) {
      Exec::Count &count = GetROSCount(*it);
      count.ros_nrob.clear();
      count.ros_nrob_retrv.clear();
      count.AddCount(alg);
      //
      // Only do the following for the ros id specified
      //
      if(*it != fROSId) continue;

      Exec::Count rosCount;
      rosCount.select_ros = true; 
      rosCount.ros_id     = fROSId; 
      rosCount.AddCount(alg);

    }
  }

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

//--------------------------------------------------------------------------------------      
void Anp::ExecCountRoI::FinishEvent(const Exec::Event &event)
{
  //
  // Add ROI counts to histos and clear roi map
  //
  for(CountMap::iterator itroi = fCountRoi.begin(); itroi !=fCountRoi.end();++itroi){
    const Exec::Count &roiCount = itroi->second;
    if(hROSRoi.hROSNreq)         hROSRoi.hROSNreq        ->Fill(roiCount.ros_nreq);
    if(hROSRoi.hROSNreqRetrv)    hROSRoi.hROSNreqRetrv   ->Fill(roiCount.ros_nreq_retrv);
    if(hROSRoi.hROSSize)         hROSRoi.hROSSize        ->Fill(Exec::ConvertWordsToKB(roiCount.rob_size));
    if(hROSRoi.hROSSizeRetrv)    hROSRoi.hROSSizeRetrv   ->Fill(Exec::ConvertWordsToKB(roiCount.rob_size_retrv));
    fCountRoiEvt.AddCount(roiCount);
  }
  
  fCountRoi.clear();

  //
  // Count one event
  //  
  fCountRoiEvt.AddCount(event);
  fCountRoiTot.AddCount(fCountRoiEvt);
  fCountRoiEvt.Clear();
}

//---------------------------------------------------------------------------------------
const Anp::Exec::Count& Anp::ExecCountRoI::GetCount() const
{
  return fCountTotal;
}

//---------------------------------------------------------------------------------------
void Anp::ExecCountRoI::ClearCount()
{
  fCountTotal.Clear();
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCountRoI::GetAlgCount(uint32_t alg_id)
{
  CountMap::iterator it = fCountAlg.find(alg_id);
  if(it == fCountAlg.end()) {
    it = fCountAlg.insert(CountMap::value_type(alg_id, Exec::Count())).first;
    
    if(fSelectDet) { it->second.select_det = true; it->second.det_id = fDetId; }
    if(fSelectROS) { it->second.select_ros = true; it->second.ros_id = fROSId; }
    if(fSelectChn) { /* it->second.select_chn = true;*/  it->second.chn_id = fChnId;}
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
Anp::Exec::Count& Anp::ExecCountRoI::GetDetCount(uint32_t det_id)
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
Anp::Exec::Count& Anp::ExecCountRoI::GetROSCount(uint32_t ros_id)
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
Anp::Exec::Count& Anp::ExecCountRoI::GetRoiCount(uint32_t roi_id)
{
  CountMap::iterator it = fCountRoi.find(roi_id);
  if(it == fCountRoi.end()) {
    it = fCountRoi.insert(CountMap::value_type(roi_id, Exec::Count())).first;
    
    it->second.select_roi = true; 
    it->second.roi_id     = roi_id; 

    if(fSelectROS){ it->second.select_ros = true;  it->second.ros_id     = fROSId; }
    if(fSelectDet){ it->second.select_det = true;  it->second.det_id     = fDetId; }
  }

  return it->second;
}

//---------------------------------------------------------------------------------------
TH1* Anp::ExecCountRoI::GetTH1(const std::string &key, const std::string &name) const
{
  //
  // Histograms are owned by TDirectory
  //
  TH1 *h = Anp::HistMan::Instance().CreateTH1(key, fHistKey);
  
  if(h && !name.empty()) {
    h->SetName(name.c_str());
  }

  Anp::SetDir(h, fDir);

  return h;
}
