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
#include "TrigCostBase/UtilCore.h"

// Local
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostExec/TrigTimeSeq.h"
#include "TrigCostExec/PredExec.h"
#include "TrigCostExec/RemovedChn.h"

REGISTER_ANP_OBJECT(AlgEvent,PredExec)

using namespace std;
using namespace Trig;

namespace SeqBits
{
  const uint32_t maskLow16  = 0x0000ffff;  // mask low 16 bits
  const uint32_t maskIndex  = 0xfff00000;  // mask index bits  
  const uint32_t shiftIndex = 20;
}


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
// Begin PredExec code
//
Anp::PredExec::PredExec()
  :fDebug(false),
   fIsSubAlg(false),
   fhasL2(false),
   fhasEF(false),
   fLevel(0),
   frmChnName(""),
   fIsConfig(false),
   fnEvents(0),
   fRun(0),
   WriteNode(new HtmlNode("pred_exec_L2.html", HtmlNode::kHTML))
{
}

//-----------------------------------------------------------------------------
Anp::PredExec::~PredExec()
{
}

//-----------------------------------------------------------------------------
void Anp::PredExec::Config(const Registry &reg)
{
  cout << "PredExec::Config - Called" << endl;
  fReg = reg;

  reg.Get("PredExec", "Debug",        fDebug);
  reg.Get("PredExec::Level",          fLevel);
  reg.Get("PredExec::IsSubAlg",       fIsSubAlg);
  reg.Get("PredExec::rmChnName",      frmChnName);
  //reg.Get("PredExec::rmChnSeq",      frmChnSet);
  //frmChnSet.insert("L2_L2IdScan_FS");
  frmChnSet.insert("L2_L2SiTrack_FS");

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
    Anp::RegisterCallback(reg, "PredExec::ConfAlg", fConf, this);
    
    //
    // Register self for html writing callback
    //
    Factory<PredExec>::Instance().Hold(AlgEvent::GetName(), Handle<PredExec>(this));
  }
  cout << "PredExec::Config - Removing Chain: " << frmChnName << endl;
  fhasEF=false;
  fhasL2=false;
  WriteNode -> GetCont() << "<pre>" << endl;
  if(fDebug) {
    log() << "Config " << endl
	  << "   IsSubAlg  = " << fIsSubAlg  << endl
	  << "   Level     = " << fLevel     << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::PredExec::Signal(const Registry &, const string &signal)
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
bool Anp::PredExec::Init()
{  
  if(fDebug) cout << "PredExec::Init - Called" << endl;
  //
  // Read configuration from init file and release pointer
  //
  return true;
}

//-----------------------------------------------------------------------------
void Anp::PredExec::Exec()
{
  //
  // Process data for one event
  // 
  if(fDebug) cout << "PredExec::Exec - Called" << endl;
  if(fEntry.valid() && fEntry.GetState() == Ntuple::kREAD) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::PredExec::Done()
{
  if(fDebug) cout << "PredExec::Done - Called" << endl;
}
//-----------------------------------------------------------------------------
void Anp::PredExec::Set(const TrigMonConfig &config)
{
  //
  // Copy configuration and prepare algorithm, chain and sequence objects
  //
  fConfig = config;

  if(fDebug) {
    cout << "PredExec::Set " << level() << " #TrigConfSeq   = " << fConfig.size<TrigConfSeq>()   << endl
  	 << "PredExec::Set " << level() << " #TrigConfChain = " << fConfig.size<TrigConfChain>() << endl;
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
  actChn.clear();
  frmChn.clear();
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
      actChn.push_back(chn);
      set<string>::iterator foundChn = frmChnSet.find(chn.getChainName());
      if(foundChn != frmChnSet.end()){ 
	frmChn.push_back(chn);
	for(unsigned chnIt=0;chnIt<frmChn.size();++chnIt) cout << "PredExec::Set - the TrigConfChain name removed: " << frmChn.at(chnIt).getChainName() << endl;
	frmChnSet.erase(foundChn);
      }
    }
    else {
      if(fDebug) {
	cout << "PredExec::Set - wrong level " << level() << " for " << chn.getLevel() << endl;
      }
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::PredExec::Run(TrigMonEvent &event)
{
  //
  // Collect chain, sequence and algorithm timing entries
  //
  if(!fIsConfig) {
    log() << "Run - this algorithm is not configured" << endl;
    return;
  }
  //
  // Collecting chains to be removed for this event!
  //
  //set<TrigConfChain> diffChains;
  //for(unsigned actChnIt=0;actChnIt<actChn.size();++actChnIt){
  //  for(unsigned rmChnIt=0;rmChnIt<frmChn.size();++rmChnIt){
  //    if(actChn.at(actChnIt).getChainName() == frmChn.at(rmChnIt).getChainName()){
  //	diffChains.insert(actChn.at(actChnIt));
  //    }
  //  }
  //}

  ++fnEvents;
  if(fnEvents<500) WriteNode -> GetCont() << "Event: " << fnEvents << endl;
  fRun=event.getRun();
  if(fDebug) cout << "PredExec::Run - the number of events" << fnEvents << endl;

  //
  // Extract RoI objects
  //
  const unsigned nroi = event.size<TrigMonRoi>();
  if(fDebug) cout << "PredExec::Run - # rois: " << nroi << endl;
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
  // let's add up the alg time
  //
  //  bool has_L2 = false, has_EF = false;
  vector<TrigMonSeq> seq_list;
  for(unsigned iseq = 0; iseq < event.size<TrigMonSeq>(); ++iseq) {
    const TrigMonSeq &seq = event.at<TrigMonSeq>(iseq);
    seq_list.push_back(seq);
    const unsigned level = seq.getLevel();
  
//    if(level == 2){ 
//      has_L2 = true;
//      if(!fhasL2) fhasL2 = true;
//    }
//    if(level == 3){ 
//      has_EF = true;
//      if(!fhasEF) fhasEF = true;
//    }
    if(fLevel != 0 && level != fLevel) continue;   

    const std::vector<TrigMonAlg>  &alg_entry = seq.getAlg();
    const unsigned counter = seq.getChnCounter();
    const unsigned index   = seq.getSeqIndex();
    //
    // Lookup for chains
    //
    if(counter < fChnData.size() && fChnData.at(counter)) {
      TrigTimeChn *chn = fChnData.at(counter);
      if(fnEvents<500) WriteNode -> GetCont() << "Chn Name: " << chn->GetName() << " status: " << endl;
      vector<TrigConfChain> confChains = fChnData.at(counter)->GetChnVec();
      for(unsigned jk=0;jk<confChains.size();++jk) if(fnEvents<500) WriteNode -> GetCont() << "   --dependent Chn Name: " << confChains.at(jk).getName() << endl;
      vector<TrigConfSeq>   confSeq    = fChnData.at(counter)->GetSeqVec();
      for(unsigned jk=0;jk<confSeq.size();++jk) if(fnEvents<500) WriteNode -> GetCont() << "   --dependent Seq Name: " << confSeq.at(jk).getName() << endl;
    }
    else {
      cerr << "PredExec::Run - missing TrigTimeChn: " << counter << "<" << fChnData.size() << endl;
    }
    if(index < fSeqData.size() && fSeqData.at(index)){
      TrigTimeSeq *data = fSeqData.at(index); 
      if(fnEvents<500) WriteNode -> GetCont() << "      TrigMonSeq::::::level: " << level << " encoded status: " << getSeqStatus(seq) << " Seq Name: " << data->GetName() << " #algs: " << seq.getAlg().size() << " seqtime: " << seq.getSeqTimer() << endl;
      
    }
 
    if(index < fSeqData.size() && fSeqData.at(index)) {
      TrigTimeSeq *data = fSeqData.at(index);         
      for(unsigned int j = 0; j < alg_entry.size(); ++j) {
      	const TrigMonAlg  &entry = alg_entry.at(j);
	TrigTimeAlg *alg_name = data->GetAlgName(entry.getPosition());
	TrigTimeAlg *alg_type = data->GetAlgType(entry.getPosition());
	if(fnEvents<500) WriteNode -> GetCont() << "            Alg Name: " << alg_name->GetName() << " Alg Type: " << alg_type->GetName() << " status is: " << getAlgStatus(entry) << endl;
	if(entry.getNRoi()>1 && fDebug) cout << "PredExec::Run - #rois: " << int(entry.getNRoi()) << endl;
      }
    }
    else if(!(index < fSeqData.size()) && fnEvents<500) WriteNode -> GetCont() << "            ERROR info not found  for seq" << endl;
  }
  //
  // collect the TrigMonROBData vector
  //
  vector<TrigMonROB> rob_list = event.getVec<TrigMonROB>();

  //hand off instead of seq_list
  for(unsigned chnIt=0;chnIt<frmChn.size();++chnIt){ 
    if(fDebug) cout << "PredExec::Run - removing chain: " << frmChn.at(chnIt).getChainName() << " for event: " << fnEvents << endl;
    RemoveChain(frmChn.at(chnIt),seq_list,rob_list);
  }
  //
  // Resetting TrigMonEvent
  //
  vector<TrigMonROB> &ROB_LIST_FINAL = event.getVec<TrigMonROB>();
  //event.getVec<TrigMonROB>().clear();
  ROB_LIST_FINAL.clear();
  for(unsigned rob=0;rob<rob_list.size();++rob) ROB_LIST_FINAL.push_back(rob_list.at(rob));
  vector<TrigMonSeq> &SEQ_LIST_FINAL = event.getVec<TrigMonSeq>();
  SEQ_LIST_FINAL.clear();
  for(unsigned seq=0;seq<seq_list.size();++seq) SEQ_LIST_FINAL.push_back(seq_list.at(seq));

  seq_list.clear();
  rob_list.clear();
}

//-----------------------------------------------------------------------------
void Anp::PredExec::Write(HtmlNode &inode, HtmlMenu &)
{ 
  if((fhasL2 && fLevel==2) || (fhasEF && fLevel==3)){
    
    inode.AddChild(WriteNode);
    inode.GetCont() << WriteNode -> GetLink(level()+" prescale summary") << endl;
    WriteNode -> GetCont() << "</pre>" << endl;
    
  }
}

//-----------------------------------------------------------------------------
void Anp::PredExec::MakeAlg(const TrigConfSeq &seq, const TrigConfAlg &alg)
{
  //
  // Make algorithm classes
  //

  AlgMap::iterator iname = fAlgName.find(alg.getNameId());
  if(iname == fAlgName.end()) {

    Handle<Anp::TrigTimeAlg> handle(new Anp::TrigTimeAlg());
    //handle -> Config(reg);
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
  
    Handle<Anp::TrigTimeAlg> handle(new Anp::TrigTimeAlg());
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
	cerr << "PredExec::MakeAlg - failed to set TrigTimeAlg pointer for name" << endl;
      }
      
      if(!data->SetAlgType(alg.getPosition(), itype->second.get())) {
	cerr << "PredExec::MakeAlg - failed to set TrigTimeAlg pointer for type" << endl;
      }
    }
    else {
      cerr << "PredExec::MakeAlg - failed to get TrigTimeSeq at: " << seq.getIndex() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::PredExec::MakeChn(const TrigConfChain &chn)
{
  //
  // Make algorithm classes
  //
  if(std::find(fChn.begin(), fChn.end(), chn) == fChn.end()) {

    Registry reg(fReg);
    reg.Set("TrigTimeChn::Debug", "no");
    //reg.Set("TrigTimeChn::MakeHists", fMakeHists);
    
    Handle<Anp::TrigTimeChn> handle(new Anp::TrigTimeChn());
    // handle -> Config(Anp::GetDir(fDir, "chain/"+chn.getName()));
    handle -> Config(reg, chn);

    fChn.push_back(handle);
    
    const unsigned counter = chn.getCounter();

    if(counter >= fChnData.size()) {
      TrigTimeChn *data = 0;
      fChnData.insert(fChnData.end(), 1+counter-fChnData.size(), data);
    }

    if(fChnData.at(counter)) {
      cerr  << "PredExec::MakeChn - counter already exists: " << counter << endl;
    }
    else {
      fChnData.at(counter) = handle.get();
    }

    if(fDebug) {
      cout << "PredExec::Set - made new TrigTimeChn: " << chn.getName() << endl;
    }
  }
  else {
    if(fDebug) {
      cout << "PredExec::Set - TrigTimeChn already exists: " << chn.getName() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::PredExec::MakeSeq(const TrigConfSeq &seq)
{
  //
  // Make algorithm classes
  //
  if(std::find(fSeq.begin(), fSeq.end(), seq) == fSeq.end()) {
    // Make configuration for new sequence

    Registry reg(fReg);
    
    Handle<Anp::TrigTimeSeq> handle(new Anp::TrigTimeSeq());
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
      cerr << "PredExec::MakeSeq - duplicate sequences: " << seq.getName() << endl;
    }
    else {
      fSeqData.at(seq.getIndex()) = handle.get();
      
      if(fDebug) {
	cout << "PredExec::Set - made name TrigTimeSeq for " << seq.getName() << endl;
      }
    }
  }
  else {
    if(fDebug) {
      cout << "PredExecy::Set - sequence already exists: " << seq.getName() << endl;
    }
  }
}

//-----------------------------------------------------------------------------
string Anp::PredExec::level() const
{  
  if     (fLevel == 0) return "AL";
  if     (fLevel == 2) return "L2";
  else if(fLevel == 3) return "EF";
  
  return "UN";
}

//-----------------------------------------------------------------------------
string Anp::PredExec::getSeqStatus(const TrigMonSeq &seq) const
{
  string status="";
  if(seq.getEncoded() & TrigMonSeq::kUnknown)           status.append("kUnknown");
  if(seq.getEncoded() & TrigMonSeq::kInitial)           status.append("kInitial");
  if(seq.getEncoded() & TrigMonSeq::kStart)             status.append("kStart");
  if(seq.getEncoded() & TrigMonSeq::kAlreadyExecuted)   status.append("kAlreadyExecuted");
  if(seq.getEncoded() & TrigMonSeq::kPrevious)          status.append("kPrevious");
  return status;
}

//-----------------------------------------------------------------------------
//string Anp::PredExec::getChnStatus(const TrigMonChain &chn) const
//{
//  string status="";
//  if(chn.getEncoded() & TrigMonChain::kReset)                 status.append("kReset");
//  if(chn.getEncoded() & TrigMonChain::kPassed)                status.append("kPassed");
//  if(chn.getEncoded() & TrigMonChain::kPassedRaw)             status.append("kPassedRaw");
//  if(chn.getEncoded() & TrigMonChain::kPassedThrough)         status.append("kPassedThrough");
//  if(chn.getEncoded() & TrigMonChain::kPrescaled)             status.append("kPrescaled");
//  if(chn.getEncoded() & TrigMonChain::kL1AfterVeto)           status.append("kL1AfterVeto");
//  if(chn.getEncoded() & TrigMonChain::kL1BeforePrescale)      status.append("kL1BeforePrescale");
//  if(chn.getEncoded() & TrigMonChain::kL1AfterPrescale)       status.append("kL1AfterPrescale");
//  return status;
//}
//
////-----------------------------------------------------------------------------
//string Anp::PredExec::getChnStatus(const uint16_t encoded) const
//{
//  string status="";
//  if(encoded &  0x010)    status.append("kReset");
//  if(encoded &  0x020)    status.append("kPassed");
//  if(encoded &  0x040)    status.append("kPassedRaw");
//  if(encoded &  0x080)    status.append("kPassedThrough");
//  if(encoded &  0x100)    status.append("kPrescaled");
//  if(encoded &  0x200)    status.append("kL1AfterVeto");
//  if(encoded &  0x400)    status.append("kL1BeforePrescale");
//  if(encoded &  0x800)    status.append("kL1AfterPrescale");
//  return status;
//}

//-----------------------------------------------------------------------------
string Anp::PredExec::getAlgStatus(const TrigMonAlg &alg) const
{
  string status="";
  if(alg.isCached()) status.append("Cached");
  if(alg.isCalled()) status.append("Called");
  return status;
}

//-----------------------------------------------------------------------------
void Anp::PredExec::RemoveChain(const TrigConfChain &chain, 
				vector<TrigMonSeq> &seq_list,
				vector<TrigMonROB> &rob_list) const
{
  //
  // Iterate over seq list and remove 
  //   - TrigMonSeq executed by chain
  //   - TrigMonAlg executed by chain
  //   - Take into account effects from other chains
  //     eg, replace TrigMonSeq cached with called
  //     eg, replace TrigMonAlg cached with called
  //
  //   - take into account ROIs
  // 
  //   Be aware:
  //   - some algorithms have no ROI - AllTE
  //   - algorithms share tools and services
  //  
  if(fDebug) cout << "PredExec::RemoveChain - the chn name being removed: " << chain.getName() << " with counter: " << chain.getCounter() << endl;
  unsigned int rmCounter = chain.getCounter();
  RemovedChn dat;
  dat.Config(fReg);
  for(vector<TrigMonSeq>::iterator seqInst=seq_list.begin(); seqInst!=seq_list.end(); ++seqInst){
    std::vector<TrigMonAlg>  alg_entry = seqInst->getAlg();
    const unsigned counter = seqInst->getChnCounter();
    const unsigned index   = seqInst->getSeqIndex();
    if(index < fSeqData.size() && fSeqData.at(index)) {
      TrigTimeSeq *data = fSeqData.at(index);  
       if(seqInst->isAlreadyExecuted() && fDebug) cout << "PredExec::RemoveChain - SEQisCached is already executed: " << data->GetName() << " #algs: " << seqInst->getAlg().size() << endl;

       if(rmCounter != counter && dat.fSeqMap.size()!=0){
	 RemovedChn::SeqMap::iterator rmSeqDat = dat.fSeqMap.find(data->GetId());
	 if(rmSeqDat!=dat.fSeqMap.end() && seqInst->isAlreadyExecuted()){ 
	   dat.resetSeq(*seqInst,data);
	 }
	 //
	 // Check if there is a cached seq and add Cached Seq back
	 //
	 resetCachedSeq(seq_list,seqInst,dat,seqInst);

	 //
	 // Reset Cached Algs and Robs - needs to loop over all robs and algs add back time as well as reset it's own status
	 //
	 dat.resetAlgs(alg_entry,fRoiData,data);
	 if(fLevel==0 || fLevel==2){ 
	   for(vector<TrigMonAlg>::iterator alg = alg_entry.begin(); alg !=alg_entry.end();++alg) dat.resetRobs(rob_list,*alg,data);
	 }
       }
       //
       // Adding removed sequences with alg and rob info to RemovedChn class
       //
       if(rmCounter == counter){
	 if(fDebug) cout << "PredExec::RemoveChain - seq added to rmSeqData with seqname: " << data->GetName() << endl;
	 Anp::RemovedChn::rmSeqData seqDataRm = dat.addSeqEntry(*seqInst,data,rob_list);
	//
	// ERASING SEQ Associated with Removed Chain---------- Remove this seq from the the seq_list
	//
	seq_list.erase(seqInst);
	--seqInst; //just removed one element, so we need to keep the index consistent
       }
    }//checking index and alg data is filled
    else {
      cerr << "PredExec::RemoveChain - SEQ data not filled!!!!" << endl;
    }
    //------------------------------------------------------------------
    if(fDebug) cout << "PredExec::RemoveChain - GET Next seqInst; #seq left: " << seq_list.size() << endl;
    //------------------------------------------------------------------
  }//end loop over sequences seq_list
  //------------------------------------------------------------------
  if(fDebug) cout << "PredExec::RemoveChain - END loop over seq_list!" << endl;
  //------------------------------------------------------------------

  dat.fSeqMap.clear();

}//end RemoveChain function

//-----------------------------------------------------------------------------
void Anp::PredExec::resetCachedSeq(vector<TrigMonSeq> &seq_list, vector<TrigMonSeq>::iterator &riter, RemovedChn &dat, vector<TrigMonSeq>::iterator curriter) const
{
  //
  // Adding all dependent seq's
  //  - add back a chain kinitial
  //  - add chain for each dependent seq
  //
  //
  // Add a  kinitial for the current seq
  //
  const vector<uint32_t> depSeq = getDepSeqId(riter->getSeqIndex());
  for(unsigned id = 0;id <depSeq.size();++id){
    RemovedChn::SeqMap::iterator depSeqDat = dat.fSeqMap.find(depSeq.at(id));
    if(depSeqDat != dat.fSeqMap.end()){ 
      if(fDebug) cout << "PredExec::resetCachedSeq - size of seq_list  before adding back: " << seq_list.size() << endl;
      TrigMonSeq      &currSeq         = *curriter;
      uint32_t        initial_encoded  = static_cast<uint32_t>(currSeq.getChnEncodedId());
      const uint32_t  initial_index    = static_cast<uint32_t>(currSeq.getSeqIndex());
      if(initial_index >= 4096) {
	cerr << "PredExec::RemoveChain - TrigMonSeq ctor error! Index is out of range: " << initial_index << endl;
      }
      else {
	initial_encoded |= (initial_index << SeqBits::shiftIndex);
      }
      
      TrigMonSeq initialSeq(initial_encoded);
      initialSeq.addState(TrigMonSeq::kInitial);
      seq_list.insert(riter,initialSeq);
      ++riter;
      //
      // Add kPrevious to the current
      //
      RemovedChn::rmSeqData &rmSeqData = depSeqDat->second;
      if(rmSeqData.seq.size()>0){
	uint32_t prev_encoded = static_cast<uint32_t>(currSeq.getChnEncodedId());
	uint32_t prev_index   = static_cast<uint32_t>(rmSeqData.seqIndex);
	if(prev_index >= 4096) {
	  cerr << "PredExec::RemoveChain - TrigMonSeq ctor error! Index is out of range: " << prev_index << endl;
	}
	else {
	  prev_encoded |= (prev_index << SeqBits::shiftIndex);
	}
	TrigMonSeq prevSeq(prev_encoded);
	prevSeq.addState(TrigMonSeq::kInitial);
	prevSeq.addState(TrigMonSeq::kStart);
	prevSeq.addState(TrigMonSeq::kPrevious);
	//
	// Reset Algs
	//
	TrigMonSeq &rmSeq = rmSeqData.seq.at(0);
	vector<TrigMonAlg> algs = rmSeq.getAlg();
	for(unsigned alg=0;alg<algs.size();++alg) prevSeq.addAlg(algs.at(alg));
	
	for(unsigned var =0;var<rmSeq.getVar().size();++var) prevSeq.addVar(rmSeq.getVar().at(var));
	prevSeq.addTimer(rmSeq.getSeqTimer());
	seq_list.insert(riter,prevSeq);
	if(fDebug) cout << "PredExec::resetCachedSeq - just added back the seq: " << rmSeqData.seqName << endl;
	if(fDebug) cout << "PredExec::resetCachedSeq - size of seq_list after adding back: " << seq_list.size() << endl; 
	dat.fSeqMap.erase(depSeqDat);
	//
	// Ask if the new sequence has any uncalled dependencies
	//
	resetCachedSeq(seq_list,riter,dat,curriter);
	++riter;
      }
      else cerr << "RemovedChn::resetSeq - missing Seq data" << endl;
    }
  }
}

//-----------------------------------------------------------------------------
vector<uint32_t> Anp::PredExec::getDepSeqId(const unsigned index) const
{
  if(index < fSeqData.size() && fSeqData.at(index)){
    TrigTimeSeq *data     = fSeqData.at(index);
    TrigConfSeq confSeq   = data->GetConfSeq();
    return confSeq.getInputTEs(); //get TE Id
  }
  vector<uint32_t> dummy;
  return dummy;
}

//./share/runRates.py /afs/cern.ch/user/s/schae/testarea/AtlasTrig/Trigger/TriggerCommon/TriggerMenuPython/run/TrigCostAL.root --do-time -k "TrigCost" --test="pred-exec" --debug -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/costAL.root --do-time -k "TrigCost" --test="pred-exec" --debug -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/cost_html_PRED &> log_PRED & tail -f log_PRED
