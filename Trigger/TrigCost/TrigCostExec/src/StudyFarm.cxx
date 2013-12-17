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
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostExec/StudyFarm.h"

REGISTER_ANP_OBJECT(AlgEvent,StudyFarm)

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
void Anp::LumiCpuUse::Clear()
{
  // clear the variables
  n_nodes        = 0;
  n_algs         = 0;
  cpu_time       = 0.0;
  farmNodes.clear();
  alg_time       = 0.0;
  evt_time       = 0.0;
  ievt_time      = 0.0;
  n_events       = 0;
  req_time       = 0.0;
}

//-----------------------------------------------------------------------------
// Begin StudyFarm code
//
Anp::StudyFarm::StudyFarm()
  :fDebug(false),
   fIsSubAlg(false),
   fhasL2(false),
   fhasEF(false),
   fLevel(0),
   fIsConfig(false),
   fnEvents(0),
   fnNodes(0),
   fRun(0)
{
}

//-----------------------------------------------------------------------------
Anp::StudyFarm::~StudyFarm()
{
}

//-----------------------------------------------------------------------------
void Anp::StudyFarm::Config(const Registry &reg)
{
  cout << "StudyFarm::Config - Called" << endl;
  fReg = reg;

  reg.Get("StudyFarm", "Debug",        fDebug);
  reg.Get("StudyFarm::Level",          fLevel);
  reg.Get("StudyFarm::IsSubAlg",       fIsSubAlg);
  reg.Get("StudyFarm::CoolData",       fCoolLumiName);
  reg.Get("StudyFarm::CostData",       fCostLumiName);

  if(!fIsSubAlg) {
    //
    // Initiliaze input data pointers
    //
    fConfg = NtupleSvc::Instance("config").Branch<TrigMonConfig>("conf");
    fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
    
    //
    // Add self to global Factory list - used by CostMaker and others
    //
    Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));
    
    //
    // Register with trigger configuration algorithm
    //
    Anp::RegisterCallback(reg, "StudyFarm::ConfAlg", fConf, this);
    
    //
    // Register self for html writing callback
    //
    Factory<StudyFarm>::Instance().Hold(AlgEvent::GetName(), Handle<StudyFarm>(this));
  }

  fhasEF=false;
  fhasL2=false;
  if(fDebug) {
    log() << "Config " << endl
	  << "   IsSubAlg  = " << fIsSubAlg  << endl
	  << "   Level     = " << fLevel     << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyFarm::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //
  if(signal != "TRIG_CONFIG") return;

  if(fConf.valid()) { 
    Set(fConf->GetConfig()); 
  }
  else {
    log() << "Signal - invalid configuration algorithm" << endl;
  }
}

//-----------------------------------------------------------------------------
bool Anp::StudyFarm::Init()
{  
  if(fDebug) cout << "StudyFarm::Init - Called" << endl;
  //
  // Read configuration from init file and release pointer
  //
  fnNodes=0;
  return true;
}

//-----------------------------------------------------------------------------
void Anp::StudyFarm::Exec()
{
  //
  // find new xpu's with set() for each trigmonconfig
  //
  if(fConfg.valid() && fConfg.GetState() == Ntuple::kREAD) { 
    Set(fConfg.GetRef());
  }
  //
  // Process data for one event
  // 
  if(fDebug) cout << "StudyFarm::Exec - Called" << endl;
  if(fEntry.valid() && fEntry.GetState() == Ntuple::kREAD) {
    Run(fEntry.GetRef());
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyFarm::Done()
{
  if(fDebug) cout << "StudyFarm::Done - Called" << endl;

}
//-----------------------------------------------------------------------------
void Anp::StudyFarm::Set(const TrigMonConfig &config)
{
  //
  // Copy configuration and prepare algorithm, chain and sequence objects
  //
  fConfig = config;
  fIsConfig = true;

  if(fDebug) {
    cout << "StudyFarm::Set " << level() << " #TrigConfSeq   = " << fConfig.size<TrigConfSeq>()   << endl
  	 << "StudyFarm::Set " << level() << " #TrigConfChain = " << fConfig.size<TrigConfChain>() << endl;
  }

  //
  // Process configuration
  //
  const std::vector<std::string> &var_nm = config.getVarName();
  const std::vector<uint32_t>    &var_id = config.getVarId();
  
  if(var_nm.size() != var_id.size()) {
    log() << "Run - variable vector size error" << endl;
    return;
  }

  if(fDebug) {
    log()<< "Run - TrigMonConfig: nvar=" << var_nm.size() << endl;

    for(unsigned i = 0; i < var_id.size(); ++i) {
      cout << "  var[" << var_id.at(i) << "]=" << var_nm.at(i) << endl;
    }
  }
  
  for(unsigned i = 0; i < var_id.size(); ++i) {
    const std::string &vnm = var_nm.at(i);
    const uint32_t     vid = var_id.at(i);   

    if(vnm.find("xpu") == string::npos) {
      continue;
    }
    //cout << "StudyFarm::Set - the farm node: " << vnm << " with ID: " << vid <<  endl;
    FarmNodeMap::iterator it = fFarmNodeMap.find(vid);
    if(it == fFarmNodeMap.end()){
      ++fnNodes;
      fFarmNodeMap.insert(FarmNodeMap::value_type(vid, vnm));
    }
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyFarm::Run(const TrigMonEvent &event)
{
  //
  // Collect chain, sequence and algorithm timing entries
  //
  if(!fIsConfig) {
    log() << "Run - this algorithm is not configured" << endl;
    return;
  }

  ++fnEvents;
  fRun=event.getRun();
  if(fDebug) cout << "StudyFarm::Run - the number of events" << fnEvents << endl;

  LumiCpuUse &lumiInfo = GetLumiCpuUse(event.getLumi());

  //
  // Count events from individual farm hosts
  // 
  for(unsigned i = 0; i < event.getWord().size(); ++i) {
    if(event.getWord().size()>3) cout << "StudyFarm::Run - word[" << i << "]=" << event.getWord().at(i) << endl;
  }
  if(event.getWord().size() == 3) {
    //const unsigned sec  = event.getWord().at(0);
    //const unsigned msec = event.getWord().at(1);
    const unsigned tid  = event.getWord().at(2);   
    //
    // Add the xpu or find it if we already used it
    //
    lumiInfo.farmNodes.insert(tid);
    lumiInfo.cpu_time += static_cast<double>(event.getTimer());
    ++lumiInfo.n_events;
  }

  //
  // let's add up the alg time
  //
  //  bool has_L2 = false, has_EF = false;
  
  for(unsigned iseq = 0; iseq < event.size<TrigMonSeq>(); ++iseq) {
    const TrigMonSeq &seq = event.at<TrigMonSeq>(iseq);
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
    //
    // Fill algorithm objects for this sequence object
    //      
    for(unsigned int j = 0; j < alg_entry.size(); ++j) {
      const TrigMonAlg  &entry = alg_entry.at(j);
      //
      // Collecting the time!
      //
      lumiInfo.alg_time += entry.getTimer();
      //entry.isCalled()
    }
  }
  //
  // Adding the ROB request time
  //
    if(fLevel == 0 || fLevel == 2) {
      for(unsigned i = 0; i < event.size<TrigMonROB>(); ++i) {
	const TrigMonROB &rob = event.at<TrigMonROB>(i);
	lumiInfo.req_time += rob.getTimer();
	if(fDebug) cout << "StudyFarm::Run - get rob request time: " << rob.getTimer() << endl;
      }
    }
  //
  // Read event timers: steering Tproc and internal cost timer
  //
  float ievtTime=0.0;
  float evtTime=0.0;
  if(event.getTimer() > 0.0) {
    lumiInfo.evt_time += event.getTimer();
  }
  else if(fLevel == 2) {
    if(event.getVar(100,ievtTime)) lumiInfo.ievt_time += ievtTime;
    if(event.getVar(102,evtTime))  lumiInfo.evt_time  += evtTime;
  }
  else if(fLevel == 0 || fLevel == 3) { 
    if(event.getVar(110,ievtTime)) lumiInfo.ievt_time += ievtTime;
    if(event.getVar(112,evtTime))  lumiInfo.evt_time  += evtTime;
  }
}

//-----------------------------------------------------------------------------
void Anp::StudyFarm::Write(HtmlNode &inode, HtmlMenu &)
{ 
  //
  // Check that we have correct input data
  //
  if(!(fhasL2 && fLevel==2) && !(fhasEF && fLevel==3)){
    return;
  }

  //
  // Create scaling alg which talks to Cool
  //
  vector<Anp::Lumi::BlockInfo> blockInfo;
  vector<Anp::Lumi::BlockInfo> blockInfoCost;
  //
  // Get Cool counts
  //
  if(!fCoolLumiName.empty()) {

    fCoolLumi = Factory<LumiABC>::Instance().Get(fCoolLumiName);
    
    if(fCoolLumi.valid()) {

      cout << "StudyFarm::Write - retrieved lumi algorithm: " << fCoolLumiName << endl;
      
      blockInfo = fCoolLumi->GetInfos();
      if(fDebug){
	for(vector<Anp::Lumi::BlockInfo>::iterator it = blockInfo.begin(); it!=blockInfo.end(); ++it){
	  cout << "StudyFarm::Write - the output from ReadCool with lbs: " << it->lb << endl;
	  cout << "StudyFarm::Write - the output from ReadCool with globalL1count: " << it->scaleCorrL2 << endl;
	}
      }
    }
    else {
      cout << "Config - failed to get lumi algorithm: " << fCoolLumiName << endl;
    }
  } 

  //
  // Get Cost scale factor
  //
    if(!fCostLumiName.empty()) {

    fCostLumi = Factory<LumiABC>::Instance().Get(fCostLumiName);
    
    if(fCostLumi.valid()) {

      cout << "StudyFarm::Write - retrieved lumi algorithm: " << fCostLumiName << endl;
      
      blockInfoCost = fCostLumi->GetInfos();
      if(fDebug){
	for(vector<Anp::Lumi::BlockInfo>::iterator it = blockInfoCost.begin(); it!=blockInfoCost.end(); ++it){
	  cout << "StudyFarm::Write - the output from ReadCool with lbs: " << it->lb << endl;
	  cout << "StudyFarm::Write - the output from ReadCool with globalL1count: " << it->scaleCorrL2 << endl;
	}
      }
    }
    else {
      cout << "Config - failed to get lumi algorithm: " << fCostLumiName << endl;
    }
  } 


  //
  // writing the Html with info about run time per farm node
  //
  Handle<HtmlNode> snode(new HtmlNode("study_farm_"+level()+".html", HtmlNode::kHTML));
  inode.AddChild(snode);
  inode.GetCont() << snode -> GetLink(level()+" farm usage summary") << endl;
  
  snode -> GetCont() << "<pre>" << endl
		     << "Total number of available L2PU's for run " << fRun << " is: " << fnNodes << endl
		     << "The following table shows the estimated Hlt Farm Usage" << endl
		     << "If you would like to see the definitions of the column titles, hover the mouse overtop of them to see the Tool Tips" << endl;
  
  if(fLevel == 3) snode -> GetCont() << "EF Calculations are NOT READY YET!!!!!" << endl;
  //
  // Find the chain, seq, alg timing links
  //
  snode -> GetCont() << "Tables:" << endl;
  Handle<HtmlNode> nodeA, nodeC, nodeS;
  nodeA = inode.FindNode("time_hltchain_"+level());
  nodeC = inode.FindNode("time_alg_name_"+level());
  nodeS = inode.FindNode("time_alg_type_"+level());
  if(nodeA.valid()) {
    snode -> GetCont() << nodeA->GetLink("  Timing for "+level()+" chains") << endl;
  }
  if(nodeC.valid()) {
    snode -> GetCont() << nodeC->GetLink("  Timing for "+level()+" algorithm instances") << endl;
  }
  if(nodeS.valid()) {
    snode -> GetCont() << nodeS->GetLink("  Timing for "+level()+" algorithm classes") << endl;
  }
  
  
  snode -> GetCont() 
    << "<script src=\"sorttable.js\"></script>" << endl
    << "<p style=\"font-family:times; font-size:11px;\">" << endl
    << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
    << "<tr>"
    << "<th> Lumi Block # </th>"
    << "<th> Lumi Block Start Time </th>";
    //<< "<th title='Number of L2PU with " << level() << " Events run on them'> #L2PU Used in lb </th>"
    //<< "<th title='Number of L2PU seen during the lb divided by the total number of L2PU seen for entire run'> % Available L2PU Used for lb </th>"
  if(fLevel == 2){
    snode -> GetCont() 
      << "<th title='Number of accepted L1 events read from COOL'> # L1 Accepted Evts </th>"
      << "<th title='Number of L2 events processed by StudyFarm'> # L2 Cost Events </th>"
      << "<th> Rob Request Time (ms) </th>";
  }
  if(fLevel == 3){
    snode -> GetCont() 
      << "<th title='Number of accepted L2 events read from COOL'> # L2 Accepted Evts </th>"
      << "<th title='Number of EF events processed by StudyFarm'> # EF Cost Events </th>";
    //      << "<th> # L2 Accepted Evts </th>";
  }
  snode -> GetCont() 
    << "<th title='the actual time for a lumi block'> lb Time (s) </th>"
    << "<th title='the sum of wall time for all algorithms run during the event'> Internal Alg Time (ms) </th>"
    << "<th> Steering Time (ms) </th>";
  
  if(fLevel == 2){ 
    //
    // Need to multiply the L2 and EF by 10 because alg time is only stored for every tenth event
    //
    snode -> GetCont() 
      << "<th title='Estimate of farm usage by lb: calculated by (L1 accept evts)/(L2Evts)*(10*alg wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used (alg estimate) </th>";
    snode -> GetCont()
      << "<th title='Estimate of L2 Hlt Farm usage by lb: calculated by (L1 accept evts)/(L2Evts)*(steering wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used (steering estimate) </th>" << endl;

    snode -> GetCont()
      << "<th title='Estimate of L2 Hlt Farm usage by lb: calculated by (L1 accept evts)/(L2Evts)*(steering wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used - New Interface (steering estimate) </th>"
      << "</tr>" << endl;

  }
  else if(fLevel == 3){
    snode -> GetCont()
      << "<th title='Estimate of Hlt farm usage by lb: calculated by (L2 accept evts)/(EF Cost Evts)*(10*alg wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used (alg estimate) </th>";
    snode -> GetCont()
      << "<th title='Estimate of EF Hlt Farm usage by lb: calculated by (L2 accept evts)/(EF Cost Evts)*(steering wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used (steering estimate) </th>" << endl;

    snode -> GetCont()
      << "<th title='Estimate of EF Hlt Farm usage by lb: calculated by (L2 highest acc rate Chain count)/(EF highest rate Cost Evts)*(alg wall time)/(lbfrac*lbtime*NumL2PUs)'> (%) Hlt Farm Used - New interface (alg estimate) </th>"
      << "</tr>" << endl;
  }

  //
  // sort by lumi block
  //
 
  for(LumiMap::iterator it = fLumiMap.begin(); it!=fLumiMap.end(); ++it){
    //
    // Find the Cool counts
    //
    vector<Anp::Lumi::BlockInfo>::iterator itCool; //= find(blockInfo.begin(),blockInfo.end(),(it->first);
    bool foundcool = false;
    for(vector<Anp::Lumi::BlockInfo>::iterator iblk = blockInfo.begin(); iblk != blockInfo.end(); ++iblk){
      if(iblk->lb == it->first){ 
	itCool = iblk;
	foundcool =true;
	break;				       
      }
    }
    if(!foundcool) continue;

    //
    // Find Cost scale factors
    // 
      vector<Anp::Lumi::BlockInfo>::iterator itCost; 
      bool foundcost = false;
      for(vector<Anp::Lumi::BlockInfo>::iterator iblk = blockInfoCost.begin(); iblk != blockInfoCost.end(); ++iblk){
	if(iblk->lb == it->first){ 
	  itCost = iblk;
	  foundcost =true;
	  break;				       
	}
      }
      if(!foundcost) continue;
    
    cout << "StudyFarm::Write - Cool L1scale: " << itCool->scaleCorrL1 << " L2Scale: " << itCool->scaleCorrL2 << " EFScale: " << itCool->scaleCorrEF << endl;
    cout << "StudyFarm::Write - Cost L1scale: " << itCost->scaleCorrL1 << " L2Scale: " << itCost->scaleCorrL2 << " EFScale: " << itCost->scaleCorrEF << endl;

    double lumi_time             = itCool->GetLBDuration();
    double scaleL2Acc_new        = itCost->scaleCorrL1;
    double scaleEFAcc_new        = itCost->scaleCorrL2;
    double scaleL1Acc            = 0.0;
    double scaleL2Acc            = 0.0;
    if(it->second.n_events!=0){ 
      scaleL1Acc= static_cast<double>(itCool->L1accept)/static_cast<double>(it->second.n_events);
      scaleL2Acc = static_cast<double>(itCool->L2accept)/static_cast<double>(it->second.n_events);
    }
    double cpu_avail             = static_cast<double>(fnNodes)*lumi_time;
    double cpu_consumSteer       = 0.0;
    double cpu_usageSteer        = it->second.evt_time/1000.0;
    std::string cpu_consumSteerS;
    if(fnNodes==0) cpu_consumSteerS = "N/A";
    else{ 
      cpu_consumSteer        = scaleL1Acc*cpu_usageSteer/cpu_avail;
      cpu_consumSteerS       = Anp::Round2Pair(100.0*cpu_consumSteer, 0.01).first;
    }

    if(fLevel == 3 && fnNodes!=0){
      cpu_consumSteer  = scaleL2Acc*cpu_usageSteer/cpu_avail;
      cpu_consumSteerS = Anp::Round2Pair(100.0*cpu_consumSteer, 0.01).first;
    }

    //
    // Divide by 100 on the L2 and EF alg time because we want to convert ms to s and info is only saved for every 10th event, so we scale up by 10
    //
    double cpu_usageievt         = it->second.ievt_time/100.0;
    std::string cpu_consumievtS;
    if(fnNodes==0) cpu_consumievtS = "N/A";
    else{ 
      double cpu_consumievt        = scaleL1Acc*cpu_usageievt/cpu_avail;
      cpu_consumievtS             = Anp::Round2Pair(100.0*cpu_consumievt, 0.01).first;
    }
    double cpu_consum            = 0.0;
    double cpu_usage             = it->second.alg_time/100.0;
    
    std::string cpu_consumS;
    if(fnNodes==0) cpu_consumS = "N/A";
    else{ 
      cpu_consum                 = scaleL1Acc*cpu_usage/cpu_avail;
      cpu_consumS                = Anp::Round2Pair(100.0*cpu_consum, 0.01).first;
    }
    
    if(fLevel == 3 && fnNodes!=0){
      cpu_consum  = scaleL2Acc*cpu_usage/cpu_avail;
      cpu_consumS = Anp::Round2Pair(100.0*cpu_consum, 0.01).first;
    }
    
    std::string lumi_timeS       = Anp::Round2Pair(lumi_time, 0.01).first;
    std::string frac_nodes;
    if(fnNodes==0) frac_nodes    = "N/A";
    else frac_nodes              = Anp::Round2Pair(100.0*static_cast<double>(it->second.farmNodes.size())/static_cast<double>(fnNodes), 0.01).first;

    //
    // Scale factor method
    //
    std:: string scal_consumS;
    if(fnNodes==0) scal_consumS = "N/A";
    else{ 
      if(fDebug) cout << "StudyFarm::Calc - the l2 scale factor: " << scaleEFAcc_new << endl;
      double scal_consum          = scaleL2Acc_new*cpu_usage/static_cast<double>(fnNodes);
      scal_consumS                = Anp::Round2Pair(100.0*scal_consum, 0.01).first;
    }
    
    if(fLevel == 3 && fnNodes!=0){
      double scal_consum  = scaleEFAcc_new*cpu_usage/static_cast<double>(fnNodes);
      scal_consumS        = Anp::Round2Pair(100.0*scal_consum, 0.01).first;
    }

    
    snode -> GetCont()
      << "<tr>" 
      << "<td align=\"left\"> "   << it->first                                   << " </td>"
      << "<td align=\"center\"> " << itCool->GetBegTimeAsString()         << " </td>";
      //<< "<td align=\"center\"> " << it->second.farmNodes.size()                 << " </td>"
      //<< "<td align=\"center\"> " << frac_nodes                                  << " </td>"

    if(fLevel==2) snode -> GetCont() << "<td align=\"center\"> " << itCool->L1accept                     << " </td>";
    if(fLevel==3) snode -> GetCont() << "<td align=\"center\"> " << itCool->L2accept                     << " </td>";

    snode -> GetCont()  << "<td align=\"center\"> " << it->second.n_events                         << " </td>";
    
    if(fLevel==2){
      snode -> GetCont() 
	<< "<td align=\"center\"> " << it->second.req_time                       << " </td>";
    }
    snode -> GetCont()
      << "<td align=\"center\"> " << lumi_timeS                                  << " </td>"
      << "<td align=\"center\"> " << it->second.alg_time                         << " </td>"
      << "<td align=\"center\"> " << it->second.evt_time                         << " </td>"
      << "<td align=\"center\"> " << cpu_consumS                                 << " </td>"
      << "<td align=\"center\"> " << cpu_consumSteerS                            << " </td>"
      << "<td align=\"center\"> " << scal_consumS                                << " </td>"
      << "</tr>" << endl;
  }

  snode -> GetCont() << "</pre>" << endl;  
}

//-----------------------------------------------------------------------------
string Anp::StudyFarm::level() const
{  
  if     (fLevel == 0) return "AL";
  if     (fLevel == 2) return "L2";
  else if(fLevel == 3) return "EF";
  
  return "UN";
}

//---------------------------------------------------------------------------------------
//uint32_t 
Anp::LumiCpuUse &Anp::StudyFarm::GetLumiCpuUse(uint32_t lumiId)
{
  //
  // Check if LumiCpuUse exists
  //
  LumiMap::iterator it = fLumiMap.find(lumiId);
  if(it != fLumiMap.end()) {
    return it->second;
  }

  //
  // Create and map LumiCpuUse on demand
  //
  LumiCpuUse LumiCpuUseInst;
  fLumiMap.insert(LumiMap::value_type(lumiId, LumiCpuUseInst));
  it = fLumiMap.find(lumiId);
  return it->second;
}
