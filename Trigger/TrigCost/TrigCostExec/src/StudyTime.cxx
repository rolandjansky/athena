/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>

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
#include "TrigCostExec/StudyTime.h"

REGISTER_ANP_OBJECT(AlgEvent,StudyTime)

using namespace std;
using namespace Trig;

//-----------------------------------------------------------------------------
namespace Anp
{
  inline bool operator<(const TrigConfAlg &alg, unsigned id) {
    return alg.getNameId() < id;
  }
  inline bool operator<(unsigned id, const TrigConfAlg &alg) {
    return id < alg.getNameId();
  }
}

//-----------------------------------------------------------------------------
void Anp::TimeRoiData::SetRoi(const TrigMonRoi &roi)
{
  const std::vector<uint16_t>&  keys = roi.getVarKey();
  const std::vector<float>&     vals = roi.getVarVal();
  
  eta = -9999.0;
  phi = -9999.0;

  if(keys.size() == vals.size()) {
    for(unsigned int i = 0; i < keys.size(); ++i) {
      if(keys[i] == 0) eta = vals[i];
      if(keys[i] == 1) phi = vals[i];
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::LumiHist::Config(const Registry &reg, TDirectory *dir)
{
  //
  // Create histograms filled once per luminosity block
  //
  string level, hopts;
  reg.Get("level", level);
  reg.Get("hopts", hopts);

  hLumiCost = Anp::HistMan::Instance().CreateTH1("time_cost", "study_time_"+level);
  hLumiCpus = Anp::HistMan::Instance().CreateTH1("time_cpus", "study_time_"+level);
  hLumiRobs = Anp::HistMan::Instance().CreateTH1("time_robs", "study_time_"+level);

  Anp::SetDir(hLumiCost, dir);
  Anp::SetDir(hLumiCpus, dir);
  Anp::SetDir(hLumiRobs, dir);

  if(hopts.find("rebin") != string::npos) {
    if(hLumiCost) hLumiCost->SetBit(TH1::kCanRebin);
    if(hLumiCpus) hLumiCpus->SetBit(TH1::kCanRebin);
    if(hLumiRobs) hLumiRobs->SetBit(TH1::kCanRebin);
  }
}

//-----------------------------------------------------------------------------
// Begin StudyTime code
//
Anp::StudyTime::StudyTime()
  :fDir(0),
   fDirName("time"),
   fRootName(""),
   fDebug(false),
   fIsSubAlg(false),
   fMakeHists(true),
   fRunCorr(false),
   fShowNAlg(100),
   fLevel(0),
   fTimeout(100.0),
   fTimeYMaxTH2(5.0e2),
   fTimeNBinTH2(100),
   fIsConfig(false),
   fTimeEvent(0.0),
   fTimeSteer(0.0),
   fCountL2(0),
   fCountEF(0),
   fnEvents(0),
   fROSEvts(0),
   hTimeSteer(0),
   hTimeSteerOpi(0),
   hTimeSteerOff(0),
   hTimeCost(0),
   hTimeCpus(0),
   hTimeRobs(0),
   hTimeCostTimer(0)
{
}

//-----------------------------------------------------------------------------
Anp::StudyTime::~StudyTime()
{
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::Config(const Registry &reg)
{
  fReg = reg;

  unsigned SlowNEvent = 100;

  reg.Get("StudyTime", "Debug",        fDebug);
  reg.Get("StudyTime::HistOpts",       fHistOpts);
  reg.Get("StudyTime::DirName",        fDirName);
  reg.Get("StudyTime::MakeHists",      fMakeHists);
  reg.Get("StudyTime::RunCorr",        fRunCorr);
  reg.Get("StudyTime::ShowNAlg",       fShowNAlg);
  reg.Get("StudyTime::SlowNEvent",     SlowNEvent);
  reg.Get("StudyTime::Timeout",        fTimeout);
  reg.Get("StudyTime::Level",          fLevel);
  reg.Get("StudyTime::IsSubAlg",       fIsSubAlg);
  reg.Get("StudyTime::RootFile",       fRootName);

  if(!fIsSubAlg) {
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
    Anp::RegisterCallback(reg, "StudyTime::ConfAlg", fConf, this);
    
    fSlow.SetNEvent(SlowNEvent);
  
    //
    // Register self for clients
    //
    Factory<StudyTime>::Instance().Hold(AlgEvent::GetName(), Handle<StudyTime>(this));
  }
  
  if(fDebug) {
    log() << "Config " << endl
	  << "   IsSubAlg  = " << fIsSubAlg  << endl
	  << "   MakeHists = " << fMakeHists << endl
	  << "   Level     = " << fLevel     << endl;
  }

  //
  //Configure TrigTimeROB
  //
  Registry rosReg(fReg);
  rosReg.Set("TrigTimeROB::UseType",      "no");
  rosReg.Set("TrigTimeROB::Timeout",      fTimeout);
  rosReg.Set("TrigTimeROB::MakeHists",    fMakeHists);
  rosReg.Set("TrigTimeROB::HistOpts",     fHistOpts);
  rosReg.Set("TrigTimeROB::isGlobalROS",  true);

  fTimeROS.Config(Anp::GetDir(fDir, "ros_name/total"));
  fTimeROS.Config(rosReg,0);

}

//-----------------------------------------------------------------------------
void Anp::StudyTime::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //
  if(signal != "TRIG_CONFIG" && signal != "TRIG_CONFIG_DB") return;

  if(fConf.valid()) { 
    if(!fIsConfig) {
      Set(fConf->GetConfig()); 
      fIsConfig = true;
    }
  }
  else {
    log() << "Signal - invalid configuration algorithm" << endl;   
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::Save(TDirectory *dir)
{
  fDir = Anp::GetDir(dir, fDirName);
}

//-----------------------------------------------------------------------------
bool Anp::StudyTime::Init()
{  
  //
  // Read configuration from init file and release pointer
  //
  if(fMakeHists) {
    Anp::SetROOT_defaults();

    hTimeSteer     = GetTH1("time_steer");
    hTimeSteerOpi  = GetTH1("time_steer_opi");
    hTimeSteerOff  = GetTH1("time_steer_off");
    hTimeCost      = GetTH1("time_cost");
    hTimeCpus      = GetTH1("time_cpus");
    hTimeRobs      = GetTH1("time_robs");
    hTimeCostTimer = GetTH1("time_cost_timer");
  }

  return true;
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::Exec()
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
void Anp::StudyTime::Done()
{
  //
  // Reset active chains, sequences and algorithms from last event
  //
  for(vector<TrigTimeChn *>::iterator cit = fChnReset.begin(); cit != fChnReset.end(); ++cit) {
    (*cit)->AddEvent(fEvent.event_timer);
  }
  for(vector<TrigTimeSeq *>::iterator sit = fSeqReset.begin(); sit != fSeqReset.end(); ++sit) {
    (*sit)->ResetEvent(fEvent);
  }
  if(fLevel==2){
    for(set<TrigTimeROB *>::iterator sit = fRobReset.begin(); sit != fRobReset.end(); ++sit) {
      (*sit)->ResetEvent();
    }
    for(set<TrigTimeROB *>::iterator sit = fRosReset.begin(); sit != fRosReset.end(); ++sit) {
      (*sit)->ResetEvent();
    }
    if(fTimeROS.GetReset()) fTimeROS.ResetEvent();
  }
  fChnReset.clear();
  fSeqReset.clear();
  if(fLevel==2){ 
    fRobReset.clear();
    fRosReset.clear();
  }
  //
  // Finalize all objects
  // 
  for(AlgMap::iterator it = fAlgType.begin(); it != fAlgType.end(); ++it) {
    it -> second -> Finalize();
  }
  for(AlgMap::iterator it = fAlgName.begin(); it != fAlgName.end(); ++it) {
    it -> second -> Finalize();
  }
  for(ChnVec::iterator it = fChn.begin(); it != fChn.end(); ++it) {
    (*it) -> Finalize();
  }
  for(SeqVec::iterator it = fSeq.begin(); it != fSeq.end(); ++it) {
    (*it) -> Finalize();
  }
  for(ROBMap::iterator it = fROB.begin(); it != fROB.end(); ++it){
      it -> second -> Finalize();
    }
  for(ROBMap::iterator it = fROS.begin(); it != fROS.end(); ++it){
      it -> second -> Finalize();
    }
  fTimeROS.Finalize();
}
//-----------------------------------------------------------------------------
void Anp::StudyTime::Set(const TrigMonConfig &config)
{
  //
  // Copy configuration and prepare algorithm, chain and sequence objects
  //
  fConfig = config;

  if(fDebug) {
    cout << "StudyTime::Set " << level() << " #TrigConfSeq   = " << fConfig.size<TrigConfSeq>()   << endl
	 << "StudyTime::Set " << level() << " #TrigConfChain = " << fConfig.size<TrigConfChain>() << endl;
  }

  //
  // Make sequence before algo objects - need seq table for algs
  //
  for(unsigned int i = 0; i < fConfig.size<TrigConfSeq>(); ++i) {
    const TrigConfSeq &seq = fConfig.at<TrigConfSeq>(i);    
    MakeSeq(seq);
  }

  //
  // Make algorithm objects and populate lookup table for sequences
  //
  for(unsigned int i = 0; i < fConfig.size<TrigConfSeq>(); ++i) {
    const TrigConfSeq &seq = fConfig.at<TrigConfSeq>(i);

    for(vector<TrigConfAlg>::const_iterator ialg = seq.algBeg(); ialg != seq.algEnd(); ++ialg) {
      MakeAlg(seq, *ialg);
    }
  }

  for(unsigned int i = 0; i < fConfig.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chn = fConfig.at<TrigConfChain>(i);    
    if(fLevel == 0 || chn.getLevelId() == fLevel) {
      MakeChn(chn);
    }
    else {
      if(fDebug) {
	cout << "StudyTime::Set - wrong level " << level() << " for " << chn.getLevel() << endl;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::Run(const TrigMonEvent &event)
{
  //
  // Collect chain, sequence and algorithm timing entries
  //
  if(!fIsConfig) {
    log() << "Run - this algorithm is not configured" << endl;
    return;
  }

  ++fnEvents;

  //
  // Reset active chains, sequences and algorithms from previous event - used by CostInterface
  //
  for(vector<TrigTimeChn *>::iterator cit = fChnReset.begin(); cit != fChnReset.end(); ++cit) {
    (*cit)->AddEvent(fEvent.event_timer);
  }
  for(vector<TrigTimeSeq *>::iterator sit = fSeqReset.begin(); sit != fSeqReset.end(); ++sit) {
    (*sit)->ResetEvent(fEvent);
  }
  if(fLevel==2){
    for(set<TrigTimeROB *>::iterator sit = fRobReset.begin(); sit != fRobReset.end(); ++sit) {
      (*sit)->ResetEvent();
    }
    for(set<TrigTimeROB *>::iterator sit = fRosReset.begin(); sit != fRosReset.end(); ++sit) {
      (*sit)->ResetEvent();
    }
    if(fTimeROS.GetReset()) fTimeROS.ResetEvent();
  }
  fChnReset.clear();
  fSeqReset.clear();
  if(fLevel==2){ 
    fRobReset.clear();
    fRosReset.clear();
  }
  //
  // Extract RoI objects
  //
  const unsigned nroi = event.size<TrigMonRoi>();

  for(std::vector<TimeRoiData>::iterator rit = fRoiData.begin(); rit != fRoiData.end(); ++rit) {
    rit->valid = false;
  }

  for(unsigned i = 0; i < nroi; ++i) {
    const TrigMonRoi &roi = event.at<TrigMonRoi>(i);
    if(roi.getVarVal().size() < 2) {
      continue;
    }

    if(roi.getRoiId() >= fRoiData.size()) {
      fRoiData.insert(fRoiData.end(), 1+roi.getRoiId()-fRoiData.size(), TimeRoiData());
    }
    
    TimeRoiData &data = fRoiData.at(roi.getRoiId());    
    data.valid = true;
    data.id    = roi.getRoiId();
    data.SetRoi(roi);
  }

  //
  // Run over sequence objects
  //
  fEvent.run   = event.getRun();
  fEvent.lumi  = event.getLumi();
  fEvent.bxid  = event.getBunchId();
  fEvent.sec   = event.getSec();
  fEvent.nsec  = event.getNanoSec();

  fEvent.ClearTimers();

  //
  // Add lb
  //
  flb.insert(event.getLumi());

  bool has_L2 = false, has_EF = false;
  
  for(unsigned iseq = 0; iseq < event.size<TrigMonSeq>(); ++iseq) {
    const TrigMonSeq &seq = event.at<TrigMonSeq>(iseq);
    const unsigned level = seq.getLevel();

    if(level == 2) has_L2 = true;
    if(level == 3) has_EF = true;

    if(fLevel != 0 && level != fLevel) continue;   

    const std::vector<TrigMonAlg>  &alg_entry = seq.getAlg();    
    const unsigned counter = seq.getChnCounter();
    const unsigned index   = seq.getSeqIndex();

    //
    // Lookup for chains
    //
    if(counter < fChnData.size() && fChnData.at(counter)) {
      //
      // Fill data and save counter so this chain is reset after this event
      //
      TrigTimeChn *chn = fChnData.at(counter);
      fChnReset.push_back(chn);
      chn -> AddEntry(seq);
    }
    else {
      cerr << "StudyTime::Run - missing TrigTimeChn: " << counter << "<" << fChnData.size() << endl;
    }
    
    //
    // Lookup for sequences
    //
    if(index < fSeqData.size() && fSeqData.at(index)) {
      //
      // Save counter so this chain is reset after this event
      //
      TrigTimeSeq *data = fSeqData.at(index);      
      data -> AddEntry(seq);

      fSeqReset.push_back(data);

      //
      // Fill algorithm objects for this sequence object
      //
      const TrigConfSeq &conf_seq = data->GetConfSeq();      

      for(unsigned int j = 0; j < alg_entry.size(); ++j) {
	const TrigMonAlg  &entry = alg_entry.at(j);
	const TrigConfAlg &confg = conf_seq.getAlg(entry.getPosition());

	float eta = -1.0e6, phi = -1.0e6;
	if(entry.getNRoi() == 1) {

	  if(entry.getRoiId(0) < fRoiData.size()) {
	    const TimeRoiData &roi = fRoiData.at(entry.getRoiId(0));
	    
	    if(roi.valid) {
	      if(fDebug) {
		cout << "StudyTime::Run - RoI for " << confg.getName() 
		     << " eta, phi = " << roi.eta << ", " << roi.phi << endl;
	      }
	      eta = roi.eta;
	      phi = roi.phi;
	    }
	    else {
	      if(fDebug) {
		cout << "StudyTime::Run - invalid RoI data" << endl;
	      }
	    }
	  }
	  else {
	    if(fDebug) {
	      cout << "StudyTime::Run - RoI id out of range: " << int(entry.getRoiId(0)) << endl;
	    }
	  }
	}
	else {
	  if(fDebug) {
	    cout << "StudyTime::Run - alg rois size: " << int(entry.getNRoi()) << endl;
	  }
	}

	fEvent.event_timer += entry.getTimer();

	TrigTimeAlg *alg_name = data->GetAlgName(entry.getPosition());
	TrigTimeAlg *alg_type = data->GetAlgType(entry.getPosition());
	
	if(alg_name) {
	  if(fRunCorr){
	    vector<string>   algNAMES;
	    vector<uint32_t> algIds = algMatchRoi(entry,event,"name",algNAMES);
	    //alg_name -> AddAlgsRunOnRoi(algIds,entry,algNAMES);
	  }
	  alg_name -> AddAlgEntry(seq, entry, conf_seq, confg, eta, phi);
	}
	else {
	  cerr << "StudyTime::Run - no alg name: " << confg.getName()
	       << " for sequence " << conf_seq.getName() << endl;
	}

	if(alg_type) {
	  if(fRunCorr){
	    vector<string>   algNAMES;
	    vector<uint32_t> algIds = algMatchRoi(entry,event,"type",algNAMES);
	    //alg_type -> AddAlgsRunOnRoi(algIds,entry,algNAMES);
	  }
	  alg_type -> AddAlgEntry(seq, entry, conf_seq, confg, eta, phi);
	}
	else {
	  cerr << "StudyTime::Run - no alg type: " << confg.getType() 
	       << " for sequence " << conf_seq.getName() << endl;
	}
      }//end for loop over alg_entry
    }
    else {
      cerr << "StudyTime::Run - missing TrigTimeSeq: " << index << "<" << fSeqData.size() << endl;
    }
  }

  if(event.size<TrigMonROB>()>0) ++fROSEvts;

  if(fLevel == 0 || fLevel == 2) {
    for(unsigned i = 0; i < event.size<TrigMonROB>(); ++i) {
      const TrigMonROB &rob = event.at<TrigMonROB>(i);
      
      if(fDebug) {
	for(unsigned int i = 0; i < rob.getData().size(); ++i) {
	  const TrigMonROBData &robdata = rob.getData().at(i);
	  cout << "StudyTime::Run - ROB id=" << robdata.getROBId() << " size=" << robdata.getROBSize() << endl;
	}
      }

      fEvent.rob_timer += rob.getTimer();
      const set<uint32_t> rosIDs = RobService::Instance().GetROSFromROB(rob);

      AlgMap::iterator iname = fAlgName.find(rob.getRequestorId());
      if(iname != fAlgName.end()) {
	iname -> second -> AddRobEntry(rob);
	if(fLevel==2){ 
	  for(set<uint32_t>::const_iterator rsit = rosIDs.begin(); rsit != rosIDs.end(); ++rsit) fTimeROS.AddROSEntry(rob,iname -> second -> GetName(),*rsit);
	}
      }
      else {
	if(fDebug) {
	  cerr << "StudyTime::Run - missing name TrigTimeAlg: " << rob.getRequestorId() << endl;
	}
	if(fLevel==2){ 
	  for(set<uint32_t>::const_iterator rsit = rosIDs.begin(); rsit != rosIDs.end(); ++rsit) fTimeROS.AddROSEntry(rob,"UNKNOWN_ALG",*rsit);
	}
	continue;
      }
      
      AlgMap::iterator itype = fAlgType.find(iname->second->GetTypeId());
      if(itype != fAlgType.end()) {
	itype -> second -> AddRobEntry(rob);
      }
      else {
	if(fDebug) {
	  cerr << "StudyTime::Run - missing type TrigTimeAlg: " << rob.getRequestorId() << endl;
	}
      }

      //
      // Fill timing plots per subdetector
      //
      if(fLevel==2){
	for(set<uint32_t>::const_iterator rsit = rosIDs.begin(); rsit != rosIDs.end(); ++rsit) {
	  Handle<TrigTimeROB> ros_alg = GetROSROB(*rsit);
	  if(ros_alg.valid()) {
	    //
	    // Do work - fill TrigMonROB data: rob_alg -> FillROBData(rob);
	    //
	    if(fDebug) cout << "StudyTime::Run - filling ros info for rosid: " << *rsit << " name: " << RobService::Instance().GetRosFromId(*rsit) << endl;
	    iname = fAlgName.find(rob.getRequestorId());
	    if(iname != fAlgName.end()) {
	      ros_alg->AddROSEntry(rob,iname -> second -> GetName(),*rsit);
	    }
	    else {
	      if(fDebug) {
		cerr << "StudyTime::Run - missing name TrigTimeAlg: " << rob.getRequestorId() << endl;
	      }
	      ros_alg->AddROSEntry(rob,"UNKNOWN_ALG",*rsit);	     
	    }
	    
	    //
	    // Save TrigTimeROB for reseting after this event
	    //
	    fRosReset.insert(ros_alg.get());
	  }
	}

	const set<uint32_t> subdets = RobService::Instance().GetSubDetFromROB(rob);
	
	for(set<uint32_t>::const_iterator sit = subdets.begin(); sit != subdets.end(); ++sit) {
	  Handle<TrigTimeROB> rob_alg = GetSubDetROB(*sit);
	  if(rob_alg.valid()) {
	    //
	    // Do work - fill TrigMonROB data: rob_alg -> FillROBData(rob);
	    //
	    rob_alg->AddROBEntry(rob);
	    
	    //
	    // Save TrigTimeROB for reseting after this event
	    //
	    fRobReset.insert(rob_alg.get());
	  }
	}
      }
    }
  }

  //
  // Count events
  //
  fCountL2 += has_L2;
  fCountEF += has_EF;
  
  //
  // Read event timers: steering Tproc and internal cost timer
  //
  if(fLevel == 0 || fLevel == 2) { 
    event.getVar(100, fEvent.icost_timer);
    event.getVar(102, fEvent.tproc_timer);
  }  
  if(fLevel == 0 || fLevel == 3) { 
    event.getVar(110, fEvent.icost_timer);
    event.getVar(112, fEvent.tproc_timer);
  }
  
  if(event.getTimer() > 0.0) {
    fEvent.tproc_timer = event.getTimer();
  }
  
  FillEventHists(event);
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::FillEventHists(const TrigMonEvent &event)
{
  //
  // Filling histograms and save slowests events
  //
  fTimeSteer += fEvent.tproc_timer;
  fTimeEvent += fEvent.event_timer;
  fEvent.cpu_timer = fEvent.event_timer - fEvent.rob_timer;

  //
  //
  //
  const bool is_cost_event = (event.size<TrigMonSeq>() > 0);

  //
  // Fill event histograms
  //
  if(hTimeSteer) { 
    hTimeSteer->Fill(fEvent.tproc_timer);
  }
  if(is_cost_event) { 
    if(hTimeCost)     hTimeCost    ->Fill(fEvent.event_timer);
    if(hTimeCpus)     hTimeCpus    ->Fill(fEvent.cpu_timer);
    if(hTimeRobs)     hTimeRobs    ->Fill(fEvent.rob_timer);
    if(hTimeSteerOpi) hTimeSteerOpi->Fill(fEvent.tproc_timer);
  }
  else {
    if(hTimeSteerOff)  hTimeSteerOff ->Fill(fEvent.tproc_timer);
  }

  if(hTimeCostTimer && fEvent.icost_timer > 0.0) { 
    hTimeCostTimer ->Fill(fEvent.icost_timer);
  }
 
  //
  // Fill lumi block histograms
  //
  std::map<unsigned, LumiHist>::iterator lit = fLumis.find(event.getLumi());
  if(lit == fLumis.end()) {
    Registry reg;
    reg.Set("level", level());
    
    lit = fLumis.insert(map<unsigned, LumiHist>::value_type(event.getLumi(), LumiHist())).first;    

    std::stringstream lstr;
    lstr << "lumis/lumi_" << std::setw(4) << setfill('0') << event.getLumi();    
    lit->second.Config(reg, Anp::GetDir(fDir, lstr.str()));
  }

  LumiHist &lumi = lit->second;
  
  if(is_cost_event) {
    if(lumi.hLumiCost) lumi.hLumiCost->Fill(fEvent.event_timer);
    if(lumi.hLumiCpus) lumi.hLumiCpus->Fill(fEvent.cpu_timer);
    if(lumi.hLumiRobs) lumi.hLumiRobs->Fill(fEvent.rob_timer);
  }

  //
  // Save N slowest events
  //
  fSlow.AddSlowEvent(event, fEvent.tproc_timer);
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::Write(HtmlNode &inode, HtmlMenu &hmenu)
{
  //
  // Write html summary
  //

  TCanvas *canvas = new TCanvas("StudyTimeCanvas", "", 650, 400);
  canvas -> SetRightMargin(0.05);
  canvas -> SetLeftMargin(0.15);

  //
  // Write the link to the root file
  //
  if(fLevel == 2 && fRootName.size()>0){
    Handle<HtmlNode> rootnode(new HtmlNode(fRootName, HtmlNode::kHTML));
    inode.AddChild(rootnode);
    inode.GetCont() << rootnode -> GetLink(level()+" root file with plots") << endl << endl;
  }

  Handle<HtmlNode> snode(new HtmlNode("study_time_"+level()+".html", HtmlNode::kHTML));
  inode.AddChild(snode);
  inode.GetCont() << snode -> GetLink(level()+" trigger timing summary") << endl;
  snode -> GetCont() << "<pre>" << endl;

  Handle<HtmlNode> nodeN(new HtmlNode("time_alg_name_"+level()+".html", HtmlNode::kHTML));
  Handle<HtmlNode> nodeT(new HtmlNode("time_alg_type_"+level()+".html", HtmlNode::kHTML));
  Handle<HtmlNode> nodeC(new HtmlNode("time_hltchain_"+level()+".html", HtmlNode::kHTML));
  Handle<HtmlNode> nodeDET(new HtmlNode("time_subdet_"+level()+".html", HtmlNode::kHTML)); 
  Handle<HtmlNode> nodeROS(new HtmlNode("time_ros_"+level()+".html", HtmlNode::kHTML)); 
  Handle<HtmlNode> nodeALLROS(new HtmlNode("time_all_ros_"+level()+".html", HtmlNode::kHTML)); 

  Handle<HtmlNode> nodeS(new HtmlNode("time_slow_evt_"+level()+".html", HtmlNode::kHTML));
  Handle<HtmlNode> nodeD(new HtmlNode("time_slow_det_"+level()+".html", HtmlNode::kHTML));
  
  nodeS  ->SetBase("time_slow_evt_"+level());
  nodeD  ->SetBase("time_slow_det_"+level());
  if(fLevel==2){ 
    nodeDET   ->SetBase("time_subdet_"+level());
    nodeROS   ->SetBase("time_ros_"+level());
    nodeALLROS->SetBase("time_all_ros_"+level());
    if(fDebug) cout << "StudyTime::Write - ros node base: " << nodeROS->GetBase() << endl;
  }
  inode.AddChild(nodeN);
  inode.AddChild(nodeT);
  inode.AddChild(nodeC);
  if(fLevel==2){ 
    inode.AddChild(nodeDET);
    inode.AddChild(nodeROS);
    inode.AddChild(nodeALLROS);
  }
  inode.AddChild(nodeS);
  inode.AddChild(nodeD);
  nodeC -> SetName("time_hltchain_"+level());
  nodeN -> SetName("time_alg_name_"+level());
  nodeT -> SetName("time_alg_type_"+level());
  
  snode -> GetCont()
    << "Tables:" << endl
    << nodeC   -> GetLink("  Timing for "+level()+" chains")                                       << endl
    << nodeN   -> GetLink("  Timing for "+level()+" algorithm instances")                          << endl
    << nodeT   -> GetLink("  Timing for "+level()+" algorithm classes")                            << endl;
  if(fLevel==2) {
    snode -> GetCont() 
      << nodeDET    -> GetLink("  Timing for "+level()+" ROB Requests by Subdetector")                  << endl
      << nodeROS    -> GetLink("  Timing for "+level()+" ROS")                                          << endl
      << nodeALLROS -> GetLink("  Timing for ALL "+level()+" ROS by Algorithm")                                          << endl;
    
  }
  snode -> GetCont()  
    << endl
    << nodeD   -> GetLink("  List of "+level()+" slowest events with detailed timing information") << endl
    << nodeS   -> GetLink("  List of "+level()+" slowest events")                                  << endl
    << endl;
  
  //
  // Insert plots before tables
  //
  string ctype;
  fReg.Get("StudyTime", "CanvasType", ctype);
  
  if((ctype == "eps" || ctype == "png") && 
     ((fLevel == 2 && fCountL2 > 0) || (fLevel == 3 && fCountEF > 0))) {
    //
    // Make plots for total timers
    //
    snode->GetCont() << "Histograms:" << endl;

    Anp::PrintHist(snode.ref(), hTimeSteer, canvas, "time_steer_"+level()+"."+ctype, 
		   "  Total event time measured by "+level()+" trigger steering", "logy");
    
    Anp::PrintHist(snode.ref(), hTimeCost, canvas, "time_cost_"+level()+"."+ctype, 
		   "  Total event time measured by "+level()+" cost tool", "logy");

    if(fLevel == 2) {
      Anp::PrintHist(snode.ref(), hTimeCpus, canvas, "time_cpus_"+level()+"."+ctype, 
		     "  Event CPU   time measured by "+level()+" cost tool", "logy");
      
      Anp::PrintHist(snode.ref(), hTimeRobs, canvas, "time_robs_"+level()+"."+ctype, 
		     "  Event ROB   time measured by "+level()+" cost tool", "logy");
    }

    //
    // Make 2d histograms time versus lumi blocks
    //
    TH2 *hcost = GetTH2("cost");
    TH2 *hcpus = GetTH2("cpus");
    TH2 *hrobs = GetTH2("robs");
    
    snode->GetCont() << endl;
    
    Anp::PrintHist(snode.ref(), hcost, canvas, "time2d_cost_"+level()+"."+ctype, 
		   "  Total event time measured by cost tool versus lumi block", "logz");
    
    Anp::PrintHist(snode.ref(), hcpus, canvas, "time2d_cpus_"+level()+"."+ctype, 
		   "  Event CPU   time measured by cost tool versus lumi block", "logz");
    
    Anp::PrintHist(snode.ref(), hrobs, canvas, "time2d_robs_"+level()+"."+ctype, 
		   "  Event ROB   time measured by cost tool versus lumi block", "logz");
    
    snode -> GetCont() << endl;
  }

  //
  // Fill timing table for algorithms - sorted by total time
  //
  vector<Handle<TrigTimeAlg> > alg_names, alg_types;
  
  for(AlgMap::iterator it = fAlgName.begin(); it != fAlgName.end(); ++it) {
    alg_names.push_back(it->second);
  }
  for(AlgMap::iterator it = fAlgType.begin(); it != fAlgType.end(); ++it) {
    alg_types.push_back(it->second);
  }
  
  WriteAlgs(nodeN, hmenu, canvas, alg_names, "ALG_NAME", fShowNAlg);
  WriteAlgs(nodeT, hmenu, canvas, alg_types, "ALG_TYPE", fShowNAlg);

  //
  // Sort TrigMonROB requests to subdetectors by time. Then write to html
  //
  fTimeSubDets=0.0;
  if(fLevel ==2){
    vector<Handle<TrigTimeROB> > rob_names;
    for(ROBMap::iterator it = fROB.begin(); it != fROB.end(); ++it) {
      if(fDebug) cout << "StudyTime::Write - the subdetector id: " << it->first << " name: " << it->second->GetName() << " took this long to get data: " << it->second->GetVar("cpu_time") << " ms" << endl;
      rob_names.push_back(it->second);
      fTimeSubDets += (it->second->GetVar("cpu_time"))/1000.0;
      if(fDebug) cout << "StudyTime::Write - the total time: " << fTimeSubDets << " s" << endl;
    }
    vector<Handle<TrigTimeROB> > ros_names;
    for(ROBMap::iterator rit = fROS.begin(); rit != fROS.end(); ++rit) {
      if(fDebug) cout << "StudyTime::Write - the ros id: " << rit->first << " name: " << rit->second->GetROSName() << " took this long to get data: " << rit->second->GetVar("cpu_time") << " ms" << endl;
      ros_names.push_back(rit->second);
      fTimeSubDets += (rit->second->GetVar("cpu_time"))/1000.0;
      if(fDebug) cout << "StudyTime::Write - the total time: " << fTimeSubDets << " s" << endl;
    }
    if(fDebug) cout << "StudyTime::Write - the number of TrigTimeROBs called for subdet: " << rob_names.size() << endl;
    if(fDebug) cout << "StudyTime::Write - the number of TrigTimeROBs called for ros: " << ros_names.size() << endl;
    WriteSubDetCalls(nodeDET, hmenu,  canvas, rob_names,    "SUBDET_NAME");
    WriteSubDetCalls(nodeROS, hmenu,  canvas, ros_names,    "ROS_NAME");

    fTimeROS.WriteROSHtml(nodeALLROS, hmenu, canvas,fnEvents,&flb,fROSEvts);
  }
  //
  // Fill timing table for chains sorted by total time
  //
  vector<Handle<TrigTimeChn> > chains;
  
  for(ChnVec::iterator it = fChn.begin(); it != fChn.end(); ++it) {
    Handle<TrigTimeChn> chain = *it;
    if(chain->GetNEvent() > 0) {
      chains.push_back(*it);
    }
  }

  WriteChns(nodeC, hmenu, canvas, chains, 0);

  fSlow.WriteSlow(nodeD, hmenu, fConfig, fSlow.GetSlowDetail(), "detail");
  fSlow.WriteSlow(nodeS, hmenu, fConfig, fSlow.GetSlowEvents(), "events");

  delete canvas;

  snode -> GetCont() << "</pre>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::WriteSubDetCalls(Handle<HtmlNode> nodeDE,
				      HtmlMenu &hmenu,
				      TCanvas *canvas,
				      std::vector<Handle<TrigTimeROB> > &algs,
				      const std::string &key)
{
  //
  // Write the tables for subdet timing
  //
  if(fDebug) {
    cout << "StudyTime::WriteSubDetCalls - processing " << algs.size() << " sub detectors "
	 << "with key=" << key << endl;
  }

  nodeDE -> GetCont() << "<pre>";

  string title;
  if     (key == "SUBDET_NAME"){ 
    title = "SubDet Name";
    nodeDE -> GetCont() 
      << "The total number of ROB requests are not computed with all L2 events. These results were NOT normalized to the total number of L2 accepted events." << endl
      << "<script src=\"sorttable.js\"></script>" << endl
      << "<p style=\"font-family:times; font-size:11px;\">" << endl
      << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
      << "<tr>"
      << "<th> " << title << " </th>"
      << "<th> Time per Event (ms) </th>"//&mu;s for micro seconds
      << "<th> Rob Requests per Event </th>"
      << "<th> % Events with Rob Requests </th>"
      << "<th> Time Per Rob Request (ms) </th>"
      << "<th> Total Size Cached (GB) </th>"
      << "<th> Total Size Requested (GB) </th>"
      << "<th> Rob Requests Cached (%) </th>"
      << "<th> Total Time (s) </th>"
      << "<th> Total Time (%) </th>"
      << "</tr>" << endl;
  }
  else if(key == "ROS_NAME"){ 
    title = "ROS Name";
    nodeDE -> GetCont()  << endl
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
			 << "<th title='request rate in Hz'> request rate (kHz) </th>"
			 << "<th title='request counts'> request count </th>"
			 << "<th title='scaled request rate in kHz'> scaled request rate (kHz) </th>"
			 << "<th> request bandwidth (Mb/s) </th>"
			 << "<th> request data (kb) </th>"
			 << "<th> scaled request bandwidth (Mb/s) </th>"
			 << "<th> retr requests/all events </th>"
			 << "<th title='Retrieved requests/events that request from that ROS'> retr requests/ROS events </th>"
			 << "<th title='Retrieved data/all events'> retr data/all events (kb) </th>"
			 << "<th title='Retrieved data/events that request from that ROS'> retr data/ROS events (kb) </th>"
			 << "<th title='Retrieved request rate in kHz'> retr request rate (kHz) </th>"
			 << "<th title='Retrieved request Counts'> retr request count </th>"
			 << "<th title='scaled Retrieved request rate in kHz'> scaled retr request rate (kHz) </th>"
			 << "<th> retrieval bandwidth (Mb/s) </th>"
			 << "<th> retrieved data (kb) </th>"
			 << "<th> scaled retrieval bandwidth (Mb/s) </th>"
			 << "<th title='retrieved time/request'> retrieval time/request (ms) </th>"
			 << "<th title='retrieved time/events that request from that ROS'> retrieval time/ROS event (ms) </th>"
			 << "<th title='all ros requests'> count of all ros requests </th>"
			 << "</tr>" << endl;
  }
  else return;

  //
  // need to get the subdetector that were never called and increment iskip
  //
  if(key == "SUBDET_NAME") std::sort(algs.begin(), algs.end(), Anp::TimeRobSort("cpu_time"));
  if(key == "ROS_NAME")    std::sort(algs.begin(), algs.end(), Anp::TimeRobSort("TrigMonRobRq"));
  
  if(fDebug) {
    cout << "StudyTime::WriteSubDetCalls - will read " << algs.size() << " algorithm(s)" << endl;
  }
  
  unsigned iskip = 0;
  float varTot[21];
  for(unsigned j=0;j<21;++j) varTot[j]=0.0;
  for(unsigned i = 0; i <  algs.size(); ++i) {
    Handle<TrigTimeROB> alg = algs.at(i);//fROB.find(algs.at(i))->second;
    string subdet_name;
    if     (key == "SUBDET_NAME") subdet_name = alg->GetName();
    else if(key == "ROS_NAME"){ 
      subdet_name = alg->GetROSName();
      if(fDebug) cout << "StudyTime::WriteSubDetCalls - ros name: " << alg->GetROSName() << endl;
    }
    //
    // Ignore algorithms which were never called
    //
    if(fnEvents < 1.0) { 
      if(fDebug) {
    	cout << "StudyTime::WriteSubDetCalls - no events for: " << alg->GetName() << endl;
      }
      
      ++iskip;
      continue;
    }
    if(subdet_name=="UNKNOWN_SUBDET") continue;
    //Handle<HtmlNode> nodeA;
    if(fDebug) cout << "StudyTime::WriteSubDetCalls - the base for html: " << nodeDE->GetBase()+"_"+subdet_name+".html" << endl;
    Handle<HtmlNode> nodeA(new HtmlNode(nodeDE->GetBase()+"_"+subdet_name+".html", HtmlNode::kHTML));
    nodeDE -> AddChild(nodeA);
    
    if(nodeA.valid()) {
      subdet_name = nodeA->GetLink(subdet_name);
      if     (key == "SUBDET_NAME") alg -> WriteHtml(nodeA, canvas);
      else if(key == "ROS_NAME")    alg -> WriteROSHtml(nodeA, hmenu, canvas,fnEvents,&flb,fROSEvts);
    }
    
    double time_frac = 0.0;
    if(fTimeSubDets > 0.0){ 
      double subdettime = static_cast<double>(alg->GetVar("cpu_time"))/1000.0;
      time_frac = 100.0*subdettime/fTimeSubDets;
    }
    else cout << "StudyTime::WriteSubDetCalls - ERROR:::the value for total subdet time: " << fTimeSubDets << endl;
    double robReq_pEv    = 0.0;
    double cpu_pEv       = 0.0;
    double req_frac      = 0.0;
    if(alg->GetVar("nEvts")>0){
      robReq_pEv   =static_cast<double>(alg->GetVar("TrigMonRobRq"))/static_cast<double>(alg->GetVar("nEvts")); 
      cpu_pEv      =static_cast<double>(alg->GetVar("cpu_time"))/static_cast<double>(alg->GetVar("nEvts"));
    }
    if(fnEvents>0){
      req_frac     =10.0*static_cast<double>(alg->GetVar("nEvts"))/static_cast<double>(fnEvents);
    }
    double sizeCached = alg->GetVar("RobReqSizeCached")/1000000.0;
    double sizeReq    = alg->GetVar("RobReqSize")/1000000.0;
    const string time_fracS       = Anp::Round2Pair(time_frac, 0.1).first;
    const string time_total       = Anp::Round2Pair(alg->GetVar("cpu_time")/1000.0, 0.1).first;
    const string time_per_robreq  = Anp::Round2Pair(static_cast<double>(alg->GetVar("cpu_time_perTrigmonrob")), 0.01).first;
    const string percentCached    = Anp::Round2Pair(100.0*static_cast<float>(alg->GetVar("percent_cached")), 0.1).first;
    const string percentCalled    = Anp::Round2Pair(100.0-100.0*static_cast<float>(alg->GetVar("percent_cached")), 0.1).first;
    const string robReq_pEvS      = Anp::Round2Pair(robReq_pEv, 0.1).first;
    const string cpu_pEvS         = Anp::Round2Pair(cpu_pEv, 0.01).first;
    const string sizeCachedS      = Anp::Round2Pair(sizeCached, 1.0).first;
    const string sizeCalledS      = Anp::Round2Pair(sizeReq-sizeCached, 1.0).first;
    const string sizeReqS         = Anp::Round2Pair(sizeReq, 1.0).first;
    const string req_fracS        = Anp::Round2Pair(100.0*req_frac, 0.01).first;

    if(fDebug) {
      cout << "StudyTime::WriteSubDetCalls - processing sub detector: " << alg->GetName() << endl;
    }
    if(alg->GetName()!="UNKNOWN_SUBDET" && key == "SUBDET_NAME"){
    nodeDE -> GetCont()
      << "<tr>" 
      << "<td align=\"left\"> "   << subdet_name              << " </td>"
      << "<td align=\"center\"> " << cpu_pEvS                 << " </td>"
      << "<td align=\"center\"> " << robReq_pEvS              << " </td>"
      << "<td align=\"center\"> " << req_fracS                << " </td>"
      << "<td align=\"center\"> " << time_per_robreq          << " </td>"
      << "<td align=\"center\"> " << sizeCachedS              << " </td>"
      << "<td align=\"center\"> " << sizeReqS                 << " </td>"
      << "<td align=\"center\"> " << percentCached            << " </td>"
      << "<td align=\"center\"> " << time_total               << " </td>"
      << "<td align=\"center\"> " << time_frac                << " </td>"
      << "</tr>" << endl;
    }
    else if(key == "ROS_NAME") {
      nodeDE -> GetCont()
	<< "<tr>" 
  	<< "<td align=\"left\"> "   << subdet_name                                        << " </td>"      
  	<< "<td align=\"left\"> "   << alg->GetString("tot_Req_pAllEvt")                  << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_Req_pROSEvt")                  << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_data_pAllEvt")                 << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_data_pROSEvt")                 << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_ReqRate_Hz")                   << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_Req_Count")                    << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_Req_Count_scaled")                    << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_ReqBandwidth")                 << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_Req_Data")                     << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_Req_Data_scaled")                     << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_Req_pAllEvt")             << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_Req_pROSEvt")             << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_data_pAllEvt")            << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_data_pROSEvt")            << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_ReqRate_Hz")              << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_Req_Count")               << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_Req_Count_scaled")               << " </td>"
	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_ReqBandwidth")            << " </td>"
	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_Req_Data")                << " </td>"
	<< "<td align=\"left\"> "   << alg->GetString("tot_retr_Req_Data_scaled")                << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_time_pROSReq")                 << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_time_pROSEvt")                 << " </td>"
  	<< "<td align=\"left\"> "   << alg->GetString("tot_nrobs_all")                 << " </td>"
	<< "</tr>" << endl;


        varTot[0]  += alg->GetVar("tot_Req_pAllEvt")       ;
  	varTot[1]  += alg->GetVar("tot_Req_pROSEvt")       ;
  	varTot[2]  += alg->GetVar("tot_data_pAllEvt")      ;
  	varTot[3]  += alg->GetVar("tot_data_pROSEvt")      ;
  	varTot[4]  += alg->GetVar("tot_ReqRate_Hz")        ;
  	varTot[5]  += alg->GetVar("tot_Req_Count")         ;
	varTot[6]  += alg->GetVar("tot_Req_Count_sc")         ;
  	varTot[7]  += alg->GetVar("tot_ReqBandwidth")      ;
	varTot[8]  += alg->GetVar("tot_Req_Data")          ;
	varTot[9]  += alg->GetVar("tot_Req_Data_sc")          ;
  	varTot[10]  += alg->GetVar("tot_retr_Req_pAllEvt")  ;
  	varTot[11]  += alg->GetVar("tot_retr_Req_pROSEvt")  ;
  	varTot[12] += alg->GetVar("tot_retr_data_pAllEvt") ;
  	varTot[13] += alg->GetVar("tot_retr_data_pROSEvt") ;
  	varTot[14] += alg->GetVar("tot_retr_ReqRate_Hz")   ;
	varTot[15] += alg->GetVar("tot_retr_Req_Count")    ;
	varTot[16] += alg->GetVar("tot_retr_Req_Count_sc")    ;
	varTot[17] += alg->GetVar("tot_retr_ReqBandwidth") ;
	varTot[18] += alg->GetVar("tot_retr_Req_Data")     ;
	varTot[19] += alg->GetVar("tot_retr_Req_Data_sc")     ;
	varTot[20] += alg->GetVar("tot_nrobs_all")     ;

  	//varTot[12] += alg->GetVar("time_pROSReq")      ;
  	//varTot[13] += alg->GetVar("time_pROSEvt")      ;
    }
  }
  if(key == "ROS_NAME") {
    nodeDE -> GetCont() << "<tr>" << "<td align=\"left\"> Total </td>";
    for(unsigned j=0;j<21;++j) nodeDE -> GetCont() << "<td align=\"center\"> " << varTot[j] << " </td>";
    nodeDE -> GetCont() << "<td align=\"center\"> N/A </td>"
       << "<td align=\"center\"> N/A </td>"
       << "<td align=\"center\"> N/A </td>"
       << "</tr>" << endl;
  }

  nodeDE -> GetCont() << "</table></pre>" << endl;
		      
  if(fDebug) {
    cout << "StudyTime::WriteSubDetCalls - skipped " << iskip << " alg(s) without events" << endl;
  }
}
//-----------------------------------------------------------------------------
void Anp::StudyTime::WriteAlgs(Handle<HtmlNode> nodeS,
			       HtmlMenu &hmenu,
			       TCanvas *canvas,
			       vector<Handle<TrigTimeAlg> > &algs,
			       const std::string &key,
			       unsigned nalg)
{
  //
  // Write out table for algorithm timing results
  //  
  if(fDebug) {
    cout << "StudyTime::WriteAlgs - processing " << algs.size() << " algorithms "
	 << "with key=" << key << endl;
   }

  string title;
  if     (key == "ALG_NAME") title = "Algorithm name";
  else if(key == "ALG_TYPE") title = "Algorithm class";
  else return;

  nodeS -> GetCont() 
    << "<script src=\"sorttable.js\"></script>" << endl
    << "<p style=\"font-family:times; font-size:11px;\">" << endl
    << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
    << "<tr>"
    << "<th> " << title << " </th>"
    << "<th> Events </th>"
    << "<th> Calls </th>"
    << "<th> Calls > " << Anp::Round2Pair(fTimeout, 1.0).first << " ms </th>"
    << "<th> Time per event (ms) </th>"
    << "<th> Time per call (ms) </th>"
    << "<th> Total time (ms) </th>"
    << "<th> Total time (%) </th>"
    << "<th> Total requested #ROS </th>"
    << "<th> Total retrieved #ROS </th>"
    << "</tr>" << endl;
  
  std::sort(algs.begin(), algs.end(), Anp::TimeAlgSort("timer_sum"));

  if(nalg == 0 || nalg > algs.size()) { 
    nalg = algs.size();
  }
  
  if(fDebug) {
    cout << "StudyTime::WriteAlgs - will read " << nalg << " algorithm(s)" << endl;
  }
  
  unsigned iskip = 0;

  for(unsigned i = 0; i < nalg; ++i) {
    Handle<TrigTimeAlg> alg = algs.at(i);
    string alg_name = alg->GetName();

    //
    // Ignore algorithms which were never called
    //
    if(alg->GetVar("nevent") < 1.0) { 
      if(fDebug) {
	cout << "StudyTime::WriteAlgs - no events for: " << alg->GetName() << endl;
      }
      
      ++iskip;
      continue;
    }

    Handle<HtmlNode> nodeA;
    if     (key == "ALG_NAME") nodeA = hmenu.FindAlgName(alg->GetName());
    else if(key == "ALG_TYPE") nodeA = hmenu.FindAlgType(alg->GetName());

    if(nodeA.valid()) {
      alg_name = nodeA->GetLink(alg_name);
      alg -> WriteHtml(nodeA, canvas);
    }
    
    double time_frac = 0.0;
    if(fTimeEvent > 0.0) { 
      time_frac = 100.0*alg->GetVar("timer_sum")/fTimeEvent;
    }
    
    const string time_fracS = Anp::Round2Pair(time_frac, 0.1).first;
    const string time_total = Anp::Round2Pair(alg->GetVar("timer_sum"), 0.1).first;

    if(fDebug) {
      cout << "StudyTime::WriteAlgs - processing algorithm: " << alg->GetName() << endl;
    }

    nodeS -> GetCont()
      << "<tr>" 
      << "<td align=\"left\"> "   << alg_name                        << " </td>"
      << "<td align=\"center\"> " << int(alg->GetVar("nevent"))      << " </td>"
      << "<td align=\"center\"> " << int(alg->GetVar("ncalls"))      << " </td>"
      << "<td align=\"center\"> " << int(alg->GetVar("ntimeout"))    << " </td>"
      << "<td align=\"center\"> " << alg->GetVar("timer_event_mean") << " </td>"
      << "<td align=\"center\"> " << alg->GetVar("timer_call_mean")  << " </td>"
      << "<td align=\"center\"> " << time_total                      << " </td>"
      << "<td align=\"center\"> " << time_fracS                      << " </td>"
      << "<td align=\"center\"> " << alg->GetVar("ros_nreq")         << " </td>"
      << "<td align=\"center\"> " << alg->GetVar("ros_nreq_ret")     << " </td>"
      << "</tr>" << endl;
  }

  nodeS -> GetCont() << "</table>" << endl;

  if(fDebug) {
    cout << "StudyTime::WriteAlgs - skipped " << iskip << " alg(s) without events" << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::WriteChns(Handle<HtmlNode> nodeS,
			       HtmlMenu &hmenu,
			       TCanvas *canvas,
			       vector<Handle<TrigTimeChn> > &chains,
			       unsigned nchn)
{
  //
  // Write out table for algorithm timing results
  //  
  if(fDebug) {
    cout << "StudyTime::WriteChns - processing " << chains.size() << " chains" << endl;
   }

  nodeS -> GetCont() 
    << "<script src=\"sorttable.js\"></script>" << endl
    << "<p style=\"font-family:times; font-size:11px;\">" << endl
    << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
    << "<tr>"
    << "<th> Chain </th>"
    << "<th> Active events </th>"
    << "<th> Time per event (ms) </th>"
    << "<th> Total time (ms) </th>"
    << "<th> Total time (%) </th>"
    << "</tr>" << endl;
  
  std::sort(chains.begin(), chains.end(), Anp::TimeChnSort("timer_sum"));

  if(nchn == 0 || nchn > chains.size()) { 
    nchn = chains.size();
  }
  
  for(unsigned i = 0; i < nchn; ++i) {
    Handle<TrigTimeChn> chain = chains.at(i);
    string chain_name = chain->GetName();

    //
    // Ignore algorithms which were never called
    //
    if(chain->GetVar("nevent") < 1.0) { 
      continue;
    }
    
    Handle<HtmlNode> nodeC = hmenu.FindChnNode(chain->GetName());
    if(nodeC.valid()) {
      chain_name = nodeC->GetLink(chain_name);
      chain -> WriteHtml(nodeC, canvas);
    }
    
    double time_frac = 0.0;
    if(fTimeEvent > 0.0) { 
      time_frac = 100.0*chain->GetVar("timer_sum")/fTimeEvent;
    }
    
    const string time_fracS = Anp::Round2Pair(time_frac, 0.1).first;
    const string time_total = Anp::Round2Pair(chain->GetVar("timer_sum"), 0.1).first;

    if(fDebug) {
      cout << "StudyTime::WriteChns - processing chain: " << chain->GetName() << endl;
    }

    nodeS -> GetCont()
      << "<tr>" 
      << "<td align=\"left\"> "   << chain_name                      << " </td>"
      << "<td align=\"center\"> " << int(chain->GetVar("nevent"))      << " </td>"
      << "<td align=\"center\"> " << chain->GetVar("timer_event_mean") << " </td>"
      << "<td align=\"center\"> " << time_total                      << " </td>"
      << "<td align=\"center\"> " << time_fracS                      << " </td>"
      << "</tr>" << endl;
  }

  nodeS -> GetCont() << "</table>" << endl;
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::MakeAlg(const TrigConfSeq &seq, const TrigConfAlg &alg)
{
  //
  // Make algorithm classes
  //

  AlgMap::iterator iname = fAlgName.find(alg.getNameId());
  if(iname == fAlgName.end()) {

    stringstream prefx;
    prefx << "ALG_NAME_" << setfill('0') << setw(10) << alg.getNameId();

    Registry reg(fReg);
    reg.Set("TrigTimeAlg::UseType", "no");
    reg.Set("TrigTimeAlg::Prefix", prefx.str());
    reg.Set("TrigTimeAlg::Timeout", fTimeout);
    reg.Set("TrigTimeAlg::MakeHists", fMakeHists);
    
    Handle<Anp::TrigTimeAlg> handle(new Anp::TrigTimeAlg());
    handle -> Config(Anp::GetDir(fDir, "alg_name/"+alg.getName()));
    handle -> Config(reg);
    handle -> Config(alg);

    iname = fAlgName.insert(AlgMap::value_type(alg.getNameId(), handle)).first;
  }

  //
  // Make objects for algorithm base classes
  //
  AlgMap::iterator itype = fAlgType.find(alg.getTypeId());
  if(itype == fAlgType.end()) {

    stringstream prefx;
    prefx << "ALG_TYPE_" << setfill('0') << setw(10) << alg.getTypeId();

    Registry reg(fReg);
    reg.Set("TrigTimeAlg::UseType", "yes");
    reg.Set("TrigTimeAlg::Prefix", prefx.str());
    reg.Set("TrigTimeAlg::Timeout", fTimeout);
    reg.Set("TrigTimeAlg::MakeHists", fMakeHists);

    Handle<Anp::TrigTimeAlg> handle(new Anp::TrigTimeAlg());
    handle -> Config(Anp::GetDir(fDir, "alg_type/"+alg.getName()));
    handle -> Config(reg);
    handle -> Config(alg);

    itype = fAlgType.insert(AlgMap::value_type(alg.getTypeId(), handle)).first;
  }
  else {
    itype -> second -> Config(alg);
  }

  //
  // Set algorithm pointer for sequence table
  //
  if(seq.getIndex() < fSeqData.size()) {
    TrigTimeSeq *data = fSeqData.at(seq.getIndex());
    
    if(data) {
      if(!data->SetAlgName(alg.getPosition(), iname->second.get())) {
	cerr << "StudyTime::MakeAlg - failed to set TrigTimeAlg pointer for name" << endl;
      }
      
      if(!data->SetAlgType(alg.getPosition(), itype->second.get())) {
	cerr << "StudyTime::MakeAlg - failed to set TrigTimeAlg pointer for type" << endl;
      }
    }
    else {
      cerr << "StudyTime::MakeAlg - failed to get TrigTimeSeq at: " << seq.getIndex() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::MakeChn(const TrigConfChain &chn)
{
  //
  // Make algorithm classes
  //
  if(std::find(fChn.begin(), fChn.end(), chn) == fChn.end()) {

    Registry reg(fReg);
    reg.Set("TrigTimeChn::Debug", "no");
    reg.Set("TrigTimeChn::MakeHists", fMakeHists);
    
    Handle<Anp::TrigTimeChn> handle(new Anp::TrigTimeChn());
    handle -> Config(Anp::GetDir(fDir, "chain/"+chn.getName()));
    handle -> Config(reg, chn);

    fChn.push_back(handle);
    
    const unsigned counter = chn.getCounter();

    if(counter >= fChnData.size()) {
      TrigTimeChn *data = 0;
      fChnData.insert(fChnData.end(), 1+counter-fChnData.size(), data);
    }

    if(fChnData.at(counter)) {
      cerr  << "StudyTime::MakeChn - counter already exists: " << counter << endl;
    }
    else {
      fChnData.at(counter) = handle.get();
    }

    if(fDebug) {
      cout << "StudyTime::Set - made new TrigTimeChn: " << chn.getName() << endl;
    }
  }
  else {
    if(fDebug) {
      cout << "StudyTime::Set - TrigTimeChn already exists: " << chn.getName() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyTime::MakeSeq(const TrigConfSeq &seq)
{
  //
  // Make algorithm classes
  //
  if(std::find(fSeq.begin(), fSeq.end(), seq) == fSeq.end()) {
    // Make configuration for new sequence

    Registry reg(fReg);
    reg.Set("TrigTimeSeq::MakeHists", fMakeHists);
    
    Handle<Anp::TrigTimeSeq> handle(new Anp::TrigTimeSeq());
    handle -> Config(Anp::GetDir(fDir, "seq/"+seq.getName()));
    handle -> Config(reg, seq);

    fSeq.push_back(handle);

    if(seq.getIndex() >= fSeqData.size()) {
      TrigTimeSeq *data = 0;
      fSeqData.insert(fSeqData.end(), 1+seq.getIndex()-fSeqData.size(), data);
    }
    
    //
    // Build "lookup" table for fast sequence lookup
    //
    if(fSeqData.at(seq.getIndex())) {
      cerr << "StudyTime::MakeSeq - duplicate sequences: " << seq.getName() << endl;
    }
    else {
      fSeqData.at(seq.getIndex()) = handle.get();
      
      if(fDebug) {
	cout << "StudyTime::Set - made name TrigTimeSeq for " << seq.getName() << endl;
      }
    }
  }
  else {
    cout << "StudyTime::Set - sequence already exists: " << seq.getName() << endl;
  }
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::TrigTimeROB> Anp::StudyTime::GetSubDetROB(int subdet)
{
  //
  // Check if TrigTimeROB exists
  //
  ROBMap::iterator it = fROB.find(subdet);
  if(it != fROB.end()) {
    return it->second;
  }

  //
  // Create and configure TrigTimeROB on demand
  //
  Handle<TrigTimeROB> rob(new TrigTimeROB());
  fROB.insert(ROBMap::value_type(subdet, rob));

  //
  //Configure
  //
  Registry reg(fReg);
  reg.Set("TrigTimeROB::UseType",      "no");
  reg.Set("TrigTimeROB::Prefix",       RobService::Instance().GetSubDetFromId(subdet));
  reg.Set("TrigTimeROB::Timeout",      fTimeout);
  reg.Set("TrigTimeROB::MakeHists",    fMakeHists);
  reg.Set("TrigTimeROB::HistOpts",     fHistOpts);

  rob -> Config(Anp::GetDir(fDir, "subdet_name/"+RobService::Instance().GetSubDetFromId(subdet)));
  rob -> Config(reg,subdet);

  return rob;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::TrigTimeROB> Anp::StudyTime::GetROSROB(int rosid)
{
  //
  // Check if TrigTimeROB exists
  //
  ROBMap::iterator it = fROS.find(rosid);
  if(it != fROS.end()) {
    return it->second;
  }

  //
  // Create and configure TrigTimeROB on demand
  //
  Handle<TrigTimeROB> ros(new TrigTimeROB());
  fROS.insert(ROBMap::value_type(rosid, ros));

  //
  //Configure
  //
  Registry reg(fReg);
  reg.Set("TrigTimeROB::UseType",      "no");
  reg.Set("TrigTimeROB::Prefix",       RobService::Instance().GetRosFromId(rosid));
  reg.Set("TrigTimeROB::Timeout",      fTimeout);
  reg.Set("TrigTimeROB::MakeHists",    fMakeHists);
  reg.Set("TrigTimeROB::HistOpts",     fHistOpts);

  ros -> Config(Anp::GetDir(fDir, "ros_name/"+RobService::Instance().GetRosFromId(rosid)));
  ros -> Config(reg,rosid);

  return ros;
}

//-----------------------------------------------------------------------------
string Anp::StudyTime::level() const
{  
  if     (fLevel == 0) return "AL";
  if     (fLevel == 2) return "L2";
  else if(fLevel == 3) return "EF";
  
  return "UN";
}

//---------------------------------------------------------------------------------------
TH1* Anp::StudyTime::GetTH1(const std::string &key) const
{
  //
  // Histograms are owned by TDirectory
  //

  TH1 *h = Anp::HistMan::Instance().CreateTH1(key, "study_time_"+level());
  Anp::SetDir(h, fDir);

  if(fHistOpts.find("rebin") != string::npos) {
    if(h) h->SetBit(TH1::kCanRebin);
  }

  if(!h && fDebug) {
    cerr << "TrigTimeAlg::FindTH1 - no HistMan histogram for: " << key << endl;
  }

  return h;
}

//---------------------------------------------------------------------------------------
TH2* Anp::StudyTime::GetTH2(const std::string &key) const
{
  //
  // Make TH2 histogram from TH1 for individual lumi blocks
  //
  if(fLumis.size() < 2) {
    cerr << "StudyTime::GetTH2 - insufficient number of lumi blocks: " << fLumis.size() << endl;
    return 0;
  }

  //
  // Determine TH2 ranges 
  //
  unsigned min_lumi = 0, max_lumi = 0;
  int nybin = fTimeNBinTH2;
  double max_time = fTimeYMaxTH2, max_cont = -1.0;

  string ytitle;
  map<unsigned, TH1 *> hists;
  
  for(map<unsigned, LumiHist>::const_iterator it = fLumis.begin(); it != fLumis.end(); ++it) {
    const LumiHist &lumi = it->second;
    
    TH1 *h = 0;
    if     (key == "cost") h = lumi.hLumiCost;
    else if(key == "cpus") h = lumi.hLumiCpus;
    else if(key == "robs") h = lumi.hLumiRobs;
    
    if(!h) continue;
    
    min_lumi = std::min(it->first, min_lumi);
    max_lumi = std::max(it->first, max_lumi);
    
    //
    // Compute Y axis limit if not set through property
    //
    if(!(max_time > 0.0)) {
      max_time = max(max_time, h->GetXaxis()->GetXmax());
    }
    
    if(ytitle.empty()) {
      ytitle = h->GetXaxis()->GetTitle();
    }

    if(!(nybin > 0)) {
      nybin = max(nybin, h->GetNbinsX());
    }

    for(int i = 1; i <= h->GetNbinsX(); ++i) {
      if(h->GetBinContent(i) > 0.0) {
	max_cont = max(max_cont, h->GetXaxis()->GetBinUpEdge(i));
      }
    }

    hists[it->first] = h;
  }

  if(min_lumi >= max_lumi) {
    cerr << "StudyTime::GetTH2 - bad lumi block range: " << min_lumi << "-" << max_lumi 
	 << " for nlumi=" << fLumis.size() << ", key=" << key << endl;
    return 0;
  }

  //
  // Create TH2
  //
  if(max_cont > 0.0) {
    max_time = min(max_time, max_cont);
  }

  TH2 *hl = new TH2F(key.c_str(), key.c_str(), max_lumi-min_lumi+1, min_lumi, max_lumi,
		     nybin, 0.0, max_time);

  hl->SetDirectory(Anp::GetDir(fDir, "lumi2d"));
  hl->GetYaxis()->SetTitle(ytitle.c_str());
  hl->GetYaxis()->CenterTitle();
  hl->GetXaxis()->SetTitle("luminosity block");
  hl->GetXaxis()->CenterTitle();  
  hl->SetStats(false);
  
  for(map<unsigned, TH1 *>::iterator hit = hists.begin(); hit != hists.end(); ++hit) {
    TH1 *h = hit->second;
    const double lumi = hit->first+0.5;

    for(int j = 0; j <= h->GetXaxis()->GetNbins()+1; ++j) {
      double value = h->GetXaxis()->GetBinCenter(j);
      
      // 
      // Reset to visible range timer value if it is over limit
      //
      if(!(value < max_time)) {
	value = hl->GetYaxis()->GetBinCenter(hl->GetYaxis()->GetNbins());
      }

      hl->Fill(lumi, value, h->GetBinContent(j));
    }
  }

  return hl;
}


//-----------------------------------------------------------------------------
vector<uint32_t> Anp::StudyTime::algMatchRoi(const TrigMonAlg &alg1, const TrigMonEvent &event, string key,vector<string>& algNames)
{
  vector<uint32_t> algIds;
  for(unsigned iseq = 0; iseq < event.size<TrigMonSeq>(); ++iseq) {
    const TrigMonSeq &seq = event.at<TrigMonSeq>(iseq);
    const unsigned level  = seq.getLevel();
    if(fLevel != 0 && level != fLevel) break;   
    const std::vector<TrigMonAlg>  &alg_entry = seq.getAlg();    
    const unsigned index                      = seq.getSeqIndex();
    //
    // Lookup for sequences
    //
    if(index < fSeqData.size() && fSeqData.at(index)) {
      TrigTimeSeq *data = fSeqData.at(index);
      for(unsigned algiter = 0; algiter<alg_entry.size(); ++algiter){
	const TrigMonAlg &alg2 = alg_entry.at(algiter);
	TrigTimeAlg *alg_type  = data->GetAlgType(alg2.getPosition());
	TrigTimeAlg *alg_name = data->GetAlgName(alg2.getPosition());
	if(alg1.getNRoi() > 0 && alg2.getNRoi() > 0) {
	  for(unsigned roi =0; roi<alg1.getNRoi();++roi){
	    for(unsigned roi2 =0; roi2<alg2.getNRoi();++roi2){
	      if(alg1.getRoiId(roi) == alg2.getRoiId(roi2)){
		if(fDebug) cout << "StudyTime::algMatchRoi - SUCCESSFUL Match: roi id: " <<  int(alg2.getRoiId(roi2)) << endl;
		if(key=="type"){ 
		  algIds.push_back(alg_type->GetTypeId());
		  //AlgMap::iterator algmapiter = fAlgType.find(alg_type->GetTypeId());
		  //if(algmapiter != fAlgType.end()) algNames.push_back(algmapiter->second->GetName());
		  //else algNames.push_back("");
		  algNames.push_back(alg_type->GetName());
		}
		else if(key == "name"){ 
		  algIds.push_back(alg_type->GetNameId());
		  //AlgMap::iterator algmapiter = fAlgName.find(alg_type->GetNameId());
		  //if(algmapiter != fAlgName.end()) algNames.push_back(algmapiter->second->GetName());
		  //else algNames.push_back("");
		  algNames.push_back(alg_name->GetName());
		}
	      }
	    }
	  }//end loop over rmAlg roi's
	  //continue;
	}//end loop over alg1 roi's
	else if(alg1.getNRoi() == 0 && alg2.getNRoi() == 0){
	  if(fDebug) cout << "StudyTime::algMatchRoi - SUCCESSFUL Math: Both ALGs have NO Roi" << endl;
	  if(key=="type"){ 
	    algIds.push_back(alg_type->GetTypeId());
	    //AlgMap::iterator algmapiter = fAlgType.find(alg_type->GetTypeId());
	    //if(algmapiter != fAlgType.end()) algNames.push_back(algmapiter->second->GetName());
	    //else algNames.push_back("");
	    algNames.push_back(alg_type->GetName());
	  }
	  else if(key == "name"){ 
	    algIds.push_back(alg_type->GetNameId());
	    //AlgMap::iterator algmapiter = fAlgName.find(alg_type->GetNameId());
	    //if(algmapiter != fAlgName.end()) algNames.push_back(algmapiter->second->GetName());
	    //else algNames.push_back("");
	    algNames.push_back(alg_name->GetName());
	  }
	}
      }//end loop over algs
    }
  }//end loop over sequences

  return algIds;
}
