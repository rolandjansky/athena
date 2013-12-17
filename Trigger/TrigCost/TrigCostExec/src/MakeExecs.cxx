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

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/HistMan.h"
#include "TrigCostBase/UtilCore.h"
#include "TrigCostBase/UtilTrig.h"

// Local
//#include "TrigCostExec/Util.h"
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/RobService.h"
#include "TrigCostExec/MakeExecs.h"

REGISTER_ANP_OBJECT(AlgEvent,MakeExecs)

using namespace std;

//-----------------------------------------------------------------------------
void Anp::MakeExecs::LBData::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Process algorithm
  //
  summary.total_time += alg.alg_data.getTimer();

  for(unsigned j = 0; j < clients.size(); ++j) {
    clients.at(j)->ProcessAlg(alg);
  }
  
  count->ProcessAlg(alg);
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::LBData::FinishEvent(const Exec::Event &event)
{
  //
  // Process event
  //
  for(unsigned j = 0; j < clients.size(); ++j) {
    clients.at(j)->FinishEvent(event);
  }

  count->FinishEvent(event);
}

//-----------------------------------------------------------------------------
// Begin MakeExecs code
//
Anp::MakeExecs::MakeExecs()
  :fDirName("execs"),
   fRootFileName(""),
   fDebug(false),
   fNewMenu(false),
   fCostEvtPassL2(false),
   fLevel(0),   
   fNLBFullHtml(10),
   fNLBSummHtml(1),
   fDir(0),
   fIsConfig(false)
{
}

//-----------------------------------------------------------------------------
Anp::MakeExecs::~MakeExecs()
{
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Config(const Registry &reg)
{
  //
  // Read configuration
  //
  fReg = reg;
  
  reg.Get("MakeExecs", "Debug",       fDebug);
  reg.Get("MakeExecs::DirName",       fDirName);
  reg.Get("MakeExecs::Level",         fLevel);
  reg.Get("MakeExecs::NLBFullHtml",   fNLBFullHtml);
  reg.Get("MakeExecs::WrtLB",         fNLBSummHtml);
  reg.Get("MakeExecs::RootFile",      fRootFileName);
  reg.Get("MakeExecs::CostEvtPassL2", fCostEvtPassL2);

  //
  // Register with trigger configuration algorithm
  //
  Anp::RegisterCallback(reg, "MakeExecs::ConfAlg", fConf, this);

  //
  // Initiliaze input data pointers
  //
  fEntry = NtupleSvc::Instance("event").Branch<TrigMonEvent>("data");
  
  //
  // Register self for html writing callback
  //
  Factory<HtmlABC>::Instance().Hold(AlgEvent::GetName(), Handle<HtmlABC>(this));

  //
  // Register self for cost interface
  //
  Factory<MakeExecs>::Instance().Hold(AlgEvent::GetName(), Handle<MakeExecs>(this));

  //
  // Set ROOT defaults for graphics
  //
  Anp::SetROOT_defaults();

  //
  // Declare pointers
  //
  fSummary.chn_conf = new Exec::ChnConfMap();
  fSummary.seq_conf = new Exec::SeqConfMap();
  fSummary.alg_conf = new Exec::AlgConfMap();
  fSummary.rob_conf = new Exec::ROBConfMap();
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Signal(const Registry &, const string &signal)
{
  //
  // Handle callback or parent signal
  //
  if(signal != "TRIG_CONFIG" && signal != "TRIG_CONFIG_DB") { 
    return;
  }

  if(fConf.valid()) { 
    if(fDebug) {
      log() << "Signal - Reading the new Trigger Configuration for " << endl;
    }
    Run(fConf->GetConfig());
    //fNewMenu=true;
  }
  else {
    log() << "Signal - invalid configuration algorithm" << endl;   
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Save(TDirectory *dir)
{
  fDir = Anp::GetDir(dir, fDirName);
}

//-----------------------------------------------------------------------------
bool Anp::MakeExecs::Init()
{  
  //
  // Book clients for analysis of cost data - after Save()
  //
  fClientVec = BookClients("ClientListAll", "", "print");
  fSurveyVec = BookClients("SurveyList",    "", "print");
 
  return true;
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Exec()
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
void Anp::MakeExecs::Done()
{
  //
  // Set scaling factors for rate normalization
  //
  string lumi_name;    
  if(fReg.Get("MakeExecs::LumiAlg", lumi_name)) {
    fLumi = Factory<LumiABC>::Instance().Get(lumi_name);
  }
  else {
    log() << "Done - lumi algorithm name is not set" << endl;
  }

  if(fLumi.valid()) {       
    //
    // Set global scale factors using luminosity algorithm
    //
    log() << "Done - using lumi algorithm: " << lumi_name << endl;

    if     (fLevel == 2) fSummary.scale_corr = 10.0*fLumi->GetRateCorrection(2);
    else if(fLevel == 3) fSummary.scale_corr = fLumi->GetRateCorrection(3); 

    //
    // Set scale factors for individual lumi blocks
    //
    const std::vector<Lumi::BlockInfo> &infos = fLumi->GetInfos();
    
    for(ClientMap::iterator cit = fClientMap.begin(); cit != fClientMap.end(); ++cit) {
      LBData &data = cit->second;

      for(unsigned i = 0; i < infos.size(); ++i) {
	const Lumi::BlockInfo &info = infos.at(i);
	
	if(data.run == info.run && data.lb == info.lb) {
	  data.lb_start = info.GetBegTimeAsString();
	  data.L1Count  = info.L1accept;
	  data.L2Count  = info.L2accept;
	  data.lb_time  = info.GetLBDuration();

	  if     (fLevel == 2) data.summary.scale_corr = info.scaleCorrL2*10.0;
	  else if(fLevel == 3) data.summary.scale_corr = info.scaleCorrEF;
	}
      }

      if(fDebug) {
	cout << "  lb=" << data.lb << ": " << data.summary.scale_corr << endl;
      }
    }
  }
  else {
    log() << "Done - missing lumi algorithm: " << lumi_name << endl;
  }
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Write(HtmlNode &inode, HtmlMenu &hmenu)
{
  //
  // Find top level root node
  //
  HtmlNode *top_root = inode.GetRoot();
  if(!top_root) {
    log() << "Write - missing top root HtmlNode required for writing LB pages" << endl;
    return;
  }

  //
  // If there are no events, then return without writing htmls
  //
  if(fSummary.nevent==0) return;

  //
  // Create index page for this algorithm
  //
  Handle<HtmlNode> enode(new HtmlNode(GetName()+".html", HtmlNode::kHTML));
  enode->SetBase(GetName());

  inode.AddChild(enode);
  inode.GetCont() << enode->GetLink(level()+" trigger execution summary") << endl;
 
  //
  // Adding Root file
  //
  if(fRootFileName.size()>4){
    Handle<HtmlNode> rootnode(new HtmlNode(fRootFileName, HtmlNode::kHTML));
    inode.AddChild(rootnode);
    inode.GetCont() << endl << rootnode->GetLink("Root File with plots") << endl;
  }

  ostream &es = enode->GetCont();
  es << "<pre>" << endl;

  //
  // Make new root HtmlNode - creates sub-directory
  //
  Handle<HtmlNode> eroot(new HtmlNode(top_root->GetAbsolutePath()+"/"+GetName(), HtmlNode::kROOT));
  //  eroot->SetParent(top_root);

  //
  // Process event algorithms
  //   -- loop over clients run on ALL events
  //
  for(unsigned i = 0; i < fClientVec.size(); ++i) {
    Handle<ExecClient> client = fClientVec.at(i);

    //
    // Prepare unique summary for this client
    //
    fSummary.html_base  = client->GetClientName();
    fSummary.write_full = true;
    fSummary.sum_lbs.clear();
    fSummary.wall_time = 0.0;

    Handle<HtmlNode> cnode(new HtmlNode(fSummary.html_base+".html", HtmlNode::kHTML));
    cnode->SetBase(fSummary.html_base);
    eroot->AddChild(cnode);

    es << cnode->GetLink("Summary: "+client->GetClientLink(), GetName()+"/") << endl;
    
    //
    // Loop over GLOBAL clients for ROS, ALGs, CHNs, FARM, etc
    //   -- fill summary for each LB
    //
    for(ClientMap::const_iterator cit = fClientMap.begin(); cit != fClientMap.end(); ++cit) {
      const LBData &data = cit->second;

      Exec::SummaryLB sum_lb;
      sum_lb.run        = data.run;
      sum_lb.lb         = data.lb;
      sum_lb.total_time = data.summary.total_time;
      sum_lb.scale_corr = data.summary.scale_corr;
      sum_lb.L1Count    = data.L1Count;
      sum_lb.L2Count    = data.L2Count;
      sum_lb.lb_time    = data.lb_time;
      sum_lb.lb_start   = data.lb_start;
      sum_lb.client     = SelectLBClient(data.clients, client.ref());
   
      //
      // Pass individual LB summary info 
      //   -- when requested by global client (e.g. ExecTableFarm)
      //   -- for LB client
      //
      if(sum_lb.client.valid() || client->GetClientPass()) {
      	fSummary.sum_lbs.push_back(sum_lb);
	fSummary.wall_time += data.lb_time;
      }
      else {
	if(fDebug) {
	  log() << "Write - missing client, lb=" << client->GetClientName() << ", " << data.lb << endl;
	}
      }
    }
    
    //
    // Write Global client's data
    //
    if(fDebug) log() << "Write - about to WriteExec for client: " << client->GetClientName() << endl;

    cnode->GetCont() << "<pre>" << endl; 
   
    //
    // If index page is required, then pass it to the client. used for debug streams to write to the index page
    // If not, then pass the client a child node.
    //
    if(client->GetClientIPag()) client->WriteExec(inode, hmenu, fSummary);
    else client->WriteExec(cnode.ref(), hmenu, fSummary);

    cnode->GetCont() << "</pre>" << endl;

    if(fDebug) log() << "Write - Finished WriteExec for client: " << client->GetClientName() << endl;

    fSummary.sum_lbs.clear(); 
  }
  
  //
  // Process Global survey algorithms 
  //   -- save informartion per LB 
  //
  fSummary.sum_lbs.clear();
  if(fDebug) log() << "Write - about to write the Survey" << endl;
  for(ClientMap::const_iterator cit = fClientMap.begin(); cit != fClientMap.end(); ++cit) {
    const LBData &data = cit->second;

    Exec::SummaryLB sum_lb;    
    sum_lb.run        = data.run;
    sum_lb.lb         = data.lb;
    sum_lb.total_time = data.summary.total_time;
    sum_lb.scale_corr = data.summary.scale_corr;
    sum_lb.client     = data.count;
    
    fSummary.sum_lbs.push_back(sum_lb);
  }
  //
  // Write Global Survey clients
  //
  for(unsigned i = 0; i < fSurveyVec.size(); ++i) {
    Handle<ExecClient> client = fSurveyVec.at(i);

    fSummary.html_base  = client->GetClientName();

    Handle<HtmlNode> cnode(new HtmlNode(fSummary.html_base+".html", HtmlNode::kHTML));
    cnode->SetBase(fSummary.html_base);
    eroot->AddChild(cnode);
    es << cnode->GetLink("Summary: "+client->GetClientLink(), GetName()+"/") << endl;
    
    //
    // Write client's data
    //  
    if(fDebug) log() << "Write - about to write the Survey client: " << client->GetClientName() << endl;
    cnode->GetCont() << "<pre>" << endl;
    client->WriteExec(cnode.ref(), hmenu, fSummary);
    cnode->GetCont() << "</pre>" << endl;
    if(fDebug) log() << "Write - finished the Survey client: " << client->GetClientName() << endl;
  }
  
  fSummary.sum_lbs.clear();

  //
  // Write summary table with entry for each processed LB
  //   -- Write table headers
  //
  es << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th>LB</th>"
     << "<th>Cost events</th>"
     << "<th>" << level() << " input rate (kHz)</th>";

  if(fLevel==2) es << "<th>ROS request rate (kHz)</th>"
		   << "<th>ROS retrieval rate (kHz)</th>";

  es << "<th>Alg call rate (kHz)</th>"
     << "<th>Alg time/event (ms)</th>"
     << "<th>Total time/event (ms)</th>";

  if(fLevel==2) es << "<th>ROB time/event (ms)</th>";

  es << "<th>Short summary</th>"
     << "<th>Full summary</th>"
     << "</tr>" << endl;
  
  //
  // Fill table content for each LB
  //
  for(ClientMap::const_iterator it = fClientMap.begin(); it != fClientMap.end(); ++it) {
    const LBData &lb         = it->second;
    const Exec::Count &count = lb.count.ref().GetCount();
    const double sc          = lb.summary.scale_corr;
    
    const string str_rate = Anp::Round2Pair(0.001*count.nevent*sc,          0.1).first;
    const string str_nreq = Anp::Round2Pair(0.001*count.ros_nreq*sc ,       0.1).first;
    const string str_nret = Anp::Round2Pair(0.001*count.ros_nreq_retrv*sc,  0.1).first;
    const string str_call = Anp::Round2Pair(0.001*count.alg_ncalls*sc,      0.1).first;
    const string str_algt = Anp::Round2Pair(count.GetMeanAlgTimePerEvent(), 0.1).first;
    const string str_tott = Anp::Round2Pair(count.GetMeanStrTimePerEvent(), 0.1).first;
    const string str_robt = Anp::Round2Pair(count.GetMeanROBTimePerEvent(), 0.1).first;

    es << "<tr>" 
       << "<td align=\"left\"> "   << lb.lb        << " </td>"
       << "<td align=\"center\"> " << count.nevent << " </td>"
       << "<td align=\"center\"> " << str_rate     << " </td>";
    if(fLevel==2) es << "<td align=\"center\"> " << str_nreq     << " </td>"
		     << "<td align=\"center\"> " << str_nret     << " </td>";
    es << "<td align=\"center\"> " << str_call     << " </td>"
       << "<td align=\"center\"> " << str_algt     << " </td>"
       << "<td align=\"center\"> " << str_tott     << " </td>";
    if(fLevel==2) es << "<td align=\"center\"> " << str_robt     << " </td>";
    es  << "<td align=\"center\"> ";
    
    //
    // Prepare summary configuration for this LB
    //
    Exec::Summary summary = it->second.summary;

    summary.alg_conf   = fSummary.alg_conf;
    summary.chn_conf   = fSummary.chn_conf;
    summary.seq_conf   = fSummary.seq_conf;    
    summary.rob_conf   = fSummary.rob_conf;
    summary.farm_node  = fSummary.farm_node;
    summary.wall_time  = lb.lb_time;
    summary.write_full = false;

    //
    // Write short summary clients for each LB
    //   -- contain only information from one LB
    //   -- each LB contains an ExecTable*** object for chn, alg and ros
    //
    const vector<Handle<ExecClient> > &clients = it->second.clients;

    for(unsigned i = 0; i < clients.size(); ++i) {
      Handle<ExecClient> client = clients.at(i);
     
      //
      // Create new HtmlNode owned by root node for this LB
      //
      summary.html_base = GetName()+"_"+client->GetClientName();

      Handle<HtmlNode> cnode(new HtmlNode(summary.html_base+".html", HtmlNode::kHTML));
      cnode->SetBase(summary.html_base);
      enode->AddChild(cnode);

      es << cnode->GetLink(client->GetClientLink()) << endl;

      cnode->GetCont() 
	<< "<pre>" 
	<< "Run:                " << lb.run           << endl
	<< "LB:                 " << lb.lb            << endl
	<< "LB start:           " << lb.lb_start      << endl
	<< "Cost events:        " << count.nevent     << endl
	<< "Input level rate:   " << str_rate         << " kHz" << endl;
	if(fLevel==2) cnode->GetCont() << "ROS request rate:   " << str_nreq         << " kHz" << endl
				       << "ROS retrieval rate: " << str_nret         << " kHz" << endl;

      client -> WriteExec(cnode.ref(), hmenu, summary);
      
      cnode->GetCont() << "</pre>" << endl;

      //
      // Write full client summary for this LB
      //
      summary.write_full = !(client->IsShortClient());
      if(fDebug) cout << "MakeExecs::Write - " << client->GetClientName() << " write full summary client? " << summary.write_full << endl;

    }

    //
    // Write full client summary for this LB
    //
    if(!summary.write_full) {
      es << "</td><td align=\"center\"></td>" << endl;
    }
    else {
      es << "</td><td align=\"center\">";
          
      //
      // Make new root HtmlNode - creates sub-directory on output file system
      //
      stringstream lb_dir;
      lb_dir << GetName() << "_lb_" << setw(4) << setfill('0') << lb.lb;
      
      Handle<HtmlNode> lb_root(new HtmlNode(top_root->GetAbsolutePath()+"/"+lb_dir.str(), HtmlNode::kROOT));

      for(unsigned i = 0; i < clients.size(); ++i) {
	Handle<ExecClient> client = clients.at(i);	

	//
	// Create new HtmlNode owned by root node for this LB
	//
	summary.html_base = client->GetClientName() + "_full";

	Handle<HtmlNode> cnode(new HtmlNode(summary.html_base+".html", HtmlNode::kHTML));
	lb_root->AddChild(cnode);
	
	cnode->SetBase(summary.html_base);
	enode->GetCont() << cnode->GetLink(client->GetClientLink(), lb_dir.str()+"/") << endl;

	cnode->GetCont() 
	  << "<pre>" 
	  << "Run:                " << lb.run           << endl
	  << "LB:                 " << lb.lb            << endl
	  << "LB start:           " << lb.lb_start      << endl
	  << "Cost events:        " << count.nevent     << endl
	  << "Input level rate:   " << str_rate         << " kHz" << endl
	  << "ROS request rate:   " << str_nreq         << " kHz" << endl
	  << "ROS retrieval rate: " << str_nret         << " kHz" << endl;
	
	client -> WriteExec(cnode.ref(), hmenu, summary);
	
	cnode->GetCont() << "</pre>" << endl;
      }
      
      es << "</td></tr>" << endl;
    
      //
      // Write out and delete root HtmlNode for this LB    
      //
      lb_root->Write();
    }
  }

  es << "<table></pre>" << endl;
  eroot->Write();
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Run(const TrigMonConfig &config)
{
  //
  // Copy configuration and prepare algorithm, chain and sequence objects
  //
  fIsConfig = true;

  //
  // Clear previous data
  //
  fConfSeq.clear();
  fConfChainL2.clear();
  fConfChainEF.clear();
  fSummary.ClearMenuConfig();

  //
  // Determine max sequence index and max chain counter
  //
  unsigned max_seq_index = 0, max_chain_counter = 0;

  for(unsigned i = 0; i < config.size<TrigConfSeq>(); ++i) {
    max_seq_index = max<unsigned>(max_seq_index, config.at<TrigConfSeq>(i).getIndex());
  }
  for(unsigned i = 0; i < config.size<TrigConfChain>(); ++i) {
    max_chain_counter = max<unsigned>(max_chain_counter, config.at<TrigConfChain>(i).getCounter());
  }

  //
  // Resize vector to max sequence index
  //
  fConfSeq.insert(fConfSeq.end(), max_seq_index+1, SeqConf());

  //
  // Resize vector to max chain counter
  //  
  fConfChainL2.insert(fConfChainL2.end(), max_chain_counter+1, Ptr<TrigConfChain>());
  fConfChainEF.insert(fConfChainEF.end(), max_chain_counter+1, Ptr<TrigConfChain>());

  //
  // Build "lookup" tables for sequences
  //
  for(unsigned int i = 0; i < config.size<TrigConfSeq>(); ++i) {
    const TrigConfSeq &seq = config.at<TrigConfSeq>(i);    

    SeqConf &conf = fConfSeq.at(seq.getIndex());

    conf.conf_seq = ObjectFactory<TrigConfSeq>::Instance().CreateObject(seq);
    fSummary.seq_conf->insert(Exec::SeqConfMap::value_type(seq.getIndex(), conf.conf_seq));
    
    //
    // Build "lookup" tables for algorithms
    //
    conf.conf_alg.resize(seq.getAlg().size());
    //cout << "seq: " << seq.getName() << " index: " << seq.getIndex() << endl; //" position: " << seq.getPosition() << endl;
    for(unsigned j = 0; j < seq.getAlg().size(); ++j) {
      const TrigConfAlg &alg = seq.getAlg().at(j);
      //cout << "alg name: " << alg.getName() << " id: " << alg.getNameId() << " position: " << static_cast<int>(alg.getPosition()) << endl;
      //
      // Look up algorithm by id
      //
      Exec::AlgConfMap::iterator ialg = fSummary.alg_conf->find(alg.getNameId());

      if(ialg == fSummary.alg_conf->end()) {
	Ptr<TrigConfAlg> alg_ptr = ObjectFactory<TrigConfAlg>::Instance().CreateObject(alg); 
	ialg = fSummary.alg_conf->insert(Exec::AlgConfMap::value_type(alg.getNameId(), alg_ptr)).first;
      }

      
      conf.conf_alg.at(j) = ialg->second;
    }
  }

  //
  // Build "lookup" tables for chains
  //
  for(unsigned int i = 0; i < config.size<TrigConfChain>(); ++i) {
    const TrigConfChain &chn = config.at<TrigConfChain>(i);

    //    if(chn.getLevelId()!=fLevel) continue;

    Ptr<TrigConfChain> chn_ptr = ObjectFactory<TrigConfChain>::Instance().CreateObject(chn);

      fSummary.chn_conf->insert(Exec::ChnConfMap::value_type(chn.getEncodedId(), chn_ptr));
    
    if     (chn.getLevelId() == 2) fConfChainL2.at(chn.getCounter()) = chn_ptr;
    else if(chn.getLevelId() == 3) fConfChainEF.at(chn.getCounter()) = chn_ptr;
  }
  //
  // Collect the farm node map
  //
  fSummary.farm_node = RobService::Instance().GetFarmNodeMap();

  //
  // TODO: add call back for TrigMonConfig from ROOT files
  //

}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Run(const TrigMonEvent &event)
{
  //
  // Convert input data to exec structures
  //
  if(!fCostEvtPassL2 || PassLevel(event)){
    Read(event);

    //
    // Fill algorithm and ROB performance summaries
    //
    if(!fAlgCall.empty()) {
      Fill();
    }
  }
}

//-----------------------------------------------------------------------------
bool Anp::MakeExecs::PassLevel(const TrigMonEvent &event)
{
  //
  // Check if the event passes
  //
  const vector<TrigMonChain> &chns = event.getChains();
  for(unsigned i=0;i<chns.size();++i){
    if(chns.at(i).getLevel()==2 && chns.at(i).isPassed(TrigMonChain::kPassed)){

      if(fDebug) cout << "chain is passed: " << chns.at(i).getCounter() << endl;
      return true;
    }
    if(fDebug) cout << "chain is failed: " << chns.at(i).getCounter() << endl;
  }

  return false;
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Read(const TrigMonEvent &event)
{
  if(fDebug) {
    log() << "Fill - reading cost data..." << endl;
  }

  //
  // Step 0: reset previous data and read general event info
  // 
  fAlgCall.clear();
  fEvent.Reset();
  fROILoad.clear();
  
  //
  // Read event header
  //
  fEvent.run      = event.getRun();
  fEvent.lb       = event.getLumi();
  fEvent.event_id = event.getEvent();
  fEvent.bcid     = event.getBunchId();
  fEvent.sec      = event.getSec();
  fEvent.nsec     = event.getNanoSec();
  fEvent.L1_time  = ConvertUTC(event.getSec(),event.getNanoSec());
  fEvent.evt_data = ObjectFactory<TrigMonEvent>::Instance().CreateObject(event);

  //
  // Add the HLT application id - contains HLT host id
  //
  if(event.getWord().size() > 2) {
    fEvent.host_sec  = event.getWord().at(0);
    fEvent.host_usec = event.getWord().at(1);
    fEvent.host_id   = event.getWord().at(2);
    fEvent.host      = RobService::Instance().GetHLTFromId(fEvent.host_id);
  }
    
  //
  // Read event timers: steering Tproc and internal cost timer
  //
  if(fLevel == 2) { 
    event.getVar(100, fEvent.timer_tcost);
    event.getVar(102, fEvent.timer_steer);
  }  
  else if(fLevel == 3) { 
    event.getVar(110, fEvent.timer_tcost);
    event.getVar(112, fEvent.timer_steer);
  }
  else {
    float timer_tcost = 0.0;
    float timer_steer = 0.0;
    
    event.getVar(100, timer_tcost);
    event.getVar(102, timer_steer);
    
    fEvent.timer_tcost = timer_tcost;
    fEvent.timer_steer = timer_steer;
    
    event.getVar(110, timer_tcost);
    event.getVar(112, timer_steer);    
    
    fEvent.timer_tcost += timer_tcost;
    fEvent.timer_steer += timer_steer;
  }
  
  if(event.getTimer() > 0.0) {
    fEvent.timer_steer = event.getTimer();
  }

  //
  // Step 1: extract RoI data
  //
  if(fDebug) log() << "Read - #roi for event: " <<  event.size<TrigMonRoi>() << endl;
  std::set<unsigned> roi_ids;
  for(unsigned i = 0; i < event.size<TrigMonRoi>(); ++i) {
    const TrigMonRoi &roi_mon = event.at<TrigMonRoi>(i);
    
    if(fDebug) {
      log() << "Read - roi eta : " << roi_mon.getEta()                      << endl
	    << "           phi : " << roi_mon.getPhi()                      << endl
	    << "           id  : " << roi_mon.getRoiId()                    << endl
	    << "           L1th: " << roi_mon.getNL1th()                    << endl
	    << "           type: " << roi_mon.getRoiType() << endl;//AsStringRoIType(roi_mon.getRoiType())
    }

    Exec::ROILoad &roi = GetROILoad(roi_mon.getRoiId());
    
    roi.SetRoi(roi_mon);
  }
  fEvent.nRoi = roi_ids.size();

  //
  // Step 2: extract algorithm timing data
  //  
  for(unsigned iseq = 0; iseq < event.size<TrigMonSeq>(); ++iseq) {
    const TrigMonSeq &seq_mon = event.at<TrigMonSeq>(iseq);
    //cout << "mon index: " << seq_mon.getSeqIndex() << " nalgs: " << seq_mon.getAlg().size() << " chnID: " << seq_mon.getChnEncodedId() << endl;
    Ptr<TrigConfChain> chn_conf = GetConfChn(seq_mon.getLevel(), seq_mon.getChnCounter());
    Ptr<TrigConfSeq>   seq_conf = GetConfSeq(seq_mon.getSeqIndex());
    
    if(!chn_conf.valid()) {
      log() << "Run - missing TrigConfChain at counter: " << seq_mon.getChnCounter() << endl;
      continue;
    }
    if(!seq_conf.valid()) {
      log() << "Run - missing TrigConfSeq at index: " << seq_mon.getSeqIndex() << endl;
      continue;
    }

    if(fLevel != 0 && fLevel != chn_conf -> getLevelId()) {
      continue;
    }

    for(unsigned ialg = 0; ialg < seq_mon.getAlg().size(); ++ialg) {
      const TrigMonAlg &alg_mon = seq_mon.getAlg().at(ialg);
      fEvent.timer_alg += alg_mon.getTimer();
      
      //cout << "alg mon pos: " << static_cast<int>(alg_mon.getPosition()) << endl;
      Ptr<TrigConfAlg> alg_conf  = GetConfAlg(seq_mon.getSeqIndex(), alg_mon.getPosition());
      
      if(!alg_conf.valid()) {
	log() << "Run - missing algorithm configuration pointer" << endl;
	continue;
      }

      Ptr<Exec::AlgCall> alg_ptr = ObjectFactory<Exec::AlgCall>::Instance().CreateObject();
      
      //
      // Fill algorithm data for one algorihm execution
      //
      alg_ptr->alg_conf  = alg_conf;
      alg_ptr->seq_conf  = seq_conf;
      alg_ptr->chn_conf  = chn_conf;
      alg_ptr->alg_data  = alg_mon;

      //
      // Fill ROI data
      //   -- Adding RoILoad if the TrigMonRoI is missing
      //
      const set<uint8_t> roi_ids = alg_mon.getRoiIdSet();

      if(fDebug) log() << "Read - TrigMonAlg number of RoIs: " << roi_ids.size() << endl;

      for(set<uint8_t>::const_iterator iroi = roi_ids.begin(); iroi != roi_ids.end(); ++iroi) {
	const unsigned roi_id = *iroi;
	
	Exec::ROILoad &iroi_load = GetROILoad(roi_id);
	if(!iroi_load.roi_valid){ 

	  if(fDebug) log() << "Read - Hacking ROILoad because TrigMonRoI is missing. Roi id: " << roi_id << " eta and phi set to 0!!!" << endl;

	  iroi_load.SetRoi(roi_id);
	  
	  if(fDebug) {
	    log() << "Run - invalid RoI index with id: " << roi_id << "/" << roi_ids.size() << endl;
	    cout << "  ";
	    alg_ptr->Print();
	    cout << endl;
	  }
	  if(!iroi_load.roi_valid) continue;
	}

	const Exec::ROILoad &roi = GetROILoad(roi_id);
	    
	if(roi.roi_valid) {
	  alg_ptr->roi_data.push_back(roi);
	  //++fEvent.nRoi;
	}
	else {
	  if(fDebug) {
	    log() << "Run - invalid RoI: id=" << roi_id << "/" << roi_ids.size() << endl;
	    cout << "  ";
	    alg_ptr->Print();
	    cout << endl;
	  }
	}
      }

      //
      // Save algorithm data at unique algorithm id
      //
      fAlgCall[alg_ptr->alg_conf->getNameId()].push_back(alg_ptr);
    }
  }

  
  //
  // Check with Trigger Elements which AlgCalls lead to a passed chain.
  //
  for(AlgMap::const_iterator it = fAlgCall.begin(); it != fAlgCall.end(); ++it) {

    const vector<Ptr<Exec::AlgCall> > &alg_vec = it->second;

    for(unsigned i = 0; i < alg_vec.size(); ++i) {

      Exec::AlgCall &call = alg_vec.at(i).ref();
      std::vector<TrigMonTE> TEs = event.getVec<TrigMonTE>();
      call.FillTEs(TEs);

    }
  }

  //
  // Step 3: extract ROB data
  //
  if(fLevel == 0 || fLevel == 2) {
    
    for(unsigned i = 0; i < event.size<TrigMonROB>(); ++i) {
      const TrigMonROB &rob_mon = event.at<TrigMonROB>(i);
      fEvent.timer_rob += rob_mon.getTimer();
      
      Exec::ROBCall rob_call;
      rob_call.rob_timer = rob_mon.getTimer();
      rob_call.rob_tbeg  = rob_mon.getStart(); 
      rob_call.rob_tend  = rob_mon.getStop(); 

      for(unsigned j = 0; j < rob_mon.getData().size(); ++j) {
	const TrigMonROBData &data = rob_mon.getData().at(j);
	
	Exec::ROBData rob;

	rob.rob_data = data;
	rob.rob_conf = GetConfROB(data.getROBId());
	rob.det_id   = rob.rob_conf->det_id;
	rob.ros_id   = rob.rob_conf->ros_id;
	rob.robin_id = rob.rob_conf->robin_id;

	rob_call.det_ids.insert(rob.det_id);
	rob_call.ros_ids.insert(rob.ros_id);
	rob_call.robin_ids.insert(rob.robin_id);
	
	rob_call.rob_data.push_back(rob);
	//cout << "MakeExec - I_AM_A_ROB - id: " << data.getROBId() << " ROS: "<< rob.ros_id << endl; 
      }
      
      //
      // Fill ROB data for matching algorithm
      //
      //cout << "my requestor alg id is: " << rob_mon.getRequestorId() << endl;
      AlgMap::iterator ialg = fAlgCall.find(rob_mon.getRequestorId());
      if(ialg == fAlgCall.end()) {
	if(rob_mon.getRequestorId()==1498476240){

	  Ptr<Exec::AlgCall> alg_ptr_steer = ObjectFactory<Exec::AlgCall>::Instance().CreateObject();

	  const TrigMonAlg    *mon_alg  = new TrigMonAlg(0,false);
	  const TrigConfAlg   *conf_alg = new TrigConfAlg(0,0,"TrigSteer_L2","TrigSteer_L2",1498476240,1498476240);
	  const TrigConfChain *conf_chn = new TrigConfChain("TrigSteer_L2",0,1498476240,"L2","TrigSteer_L2",0,1498476240,1.0,0.0);
	  const TrigConfSeq   *conf_seq = new TrigConfSeq(1498476240,0,"TrigSteer_L2");

	  Ptr<TrigConfAlg>   alg_ptr_t  = ObjectFactory<TrigConfAlg>::Instance().CreateObject(*conf_alg);
	  Ptr<TrigConfChain> chn_ptr_t  = ObjectFactory<TrigConfChain>::Instance().CreateObject(*conf_chn);
	  Ptr<TrigConfSeq>   seq_ptr_t  = ObjectFactory<TrigConfSeq>::Instance().CreateObject(*conf_seq);

	  //
	  // Adding an AlgCall for TrigSteer_L2 because it was not present in the TrigMonAlg list
	  //
	  alg_ptr_steer->alg_data = *mon_alg;
	  alg_ptr_steer->alg_conf = alg_ptr_t;
	  alg_ptr_steer->chn_conf = chn_ptr_t;
	  alg_ptr_steer->seq_conf = seq_ptr_t;

	  fAlgCall[1498476240].push_back(alg_ptr_steer);

	  //
	  ///HACK - Remove immediately - !!!!!!!!!!
	  //fAlgCall[2521113464].push_back(alg_ptr_steer);
	  //fAlgCall[3813859128].push_back(alg_ptr_steer);
	  //fAlgCall[3090706741].push_back(alg_ptr_steer);
	  //fAlgCall[220805768].push_back(alg_ptr_steer);
	  // END HACK!!!!!!
	  ialg = fAlgCall.find(rob_mon.getRequestorId());

	  //
	  // Add TrigSteer_L2 sequence to the TrigMonEvent
	  //
	  TrigMonSeq              *mon_seq      = new TrigMonSeq(*conf_chn, *conf_seq);
	  vector<TrigMonSeq>      &new_mon_seqs = fEvent.evt_data->getVec<TrigMonSeq>();
	  //	  const vector<TrigMonSeq> old_mon_seqs = fEvent.evt_data->getVec<TrigMonSeq>();

	  mon_seq->addAlg(*mon_alg);
	  mon_seq->addState(TrigMonSeq::kStart);
    
	  //new_mon_seqs.clear();
	  //new_mon_seqs.push_back(*mon_seq);
	  //new_mon_seqs.insert(new_mon_seqs.end(),old_mon_seqs.begin(),old_mon_seqs.end());	  	 
	  new_mon_seqs.insert(new_mon_seqs.begin(),*mon_seq);	  	 
	}

	if(ialg == fAlgCall.end()){
	  log() << "Run - unknown algorithm id: " << rob_mon.getRequestorId() << endl;
	  continue;
	}
      }
      
      vector<Ptr<Exec::AlgCall> > &alg_vec = ialg->second;
      
      //
      // Find algorithm execution matching this ROB request
      //
      unsigned count_rob_match = 0;
      
      for(unsigned j = 0; j < alg_vec.size(); ++j) {
	Exec::AlgCall &alg = alg_vec.at(j).ref();
	
	if((alg.alg_data.start().getEncoded() <= rob_mon.start().getEncoded() && 
	    alg.alg_data.stop ().getEncoded() >= rob_mon.stop ().getEncoded()) || 
	   rob_mon.getRequestorId()==1498476240) {
	  if(count_rob_match == 0) {
	    //
	    // Add ROB request data to matching algorithm
	    //
	    alg.rob_data.push_back(rob_call);
	    
	    alg.robin_ids.insert(rob_call.robin_ids.begin(), rob_call.robin_ids.end());
	    alg.ros_ids.insert  (rob_call.ros_ids.begin(),   rob_call.ros_ids.end());
	    alg.det_ids.insert  (rob_call.det_ids.begin(),   rob_call.det_ids.end());
	  }
	  
	  ++count_rob_match;		
	}
      }
      
      if(count_rob_match != 1 && fDebug) {

	unsigned rob_schd = 0;
	for(unsigned j = 0; j < rob_mon.getData().size(); ++j) {
	  const TrigMonROBData &data = rob_mon.getData().at(j);
	  
	  rob_schd += (data.getHistory() == TrigMonROBData::kCACHED);
	}

	log() << "Run - wrong number of ROB matches: " << count_rob_match << endl
	      << "   ROB t0: " << rob_mon.start().getSec() << "-" << rob_mon.start().getUSec() << endl
	      << "   ROB t1: " << rob_mon.stop ().getSec() << "-" << rob_mon.stop ().getUSec() << endl
	      << "   ROB no: " << rob_mon.getData().size() << endl
	      << "   ROB ch: " << rob_schd                 << endl;	

	if(!alg_vec.empty()) {
	  cout << "   " << alg_vec.front()->alg_conf->getName() << endl;
	}
	for(unsigned j = 0; j < alg_vec.size(); ++j) {
	  const Exec::AlgCall &alg = alg_vec.at(j).ref();

	  const string &n = alg.alg_conf->getName();
	  const TrigMonAlg &a = alg.alg_data;

	  cout << "   " << n << ": t0: " << a.start().getSec() << "-" << a.start().getUSec() << endl
	       << "   " << n << ": t1: " << a.stop ().getSec() << "-" << a.stop ().getUSec() << endl;
	  
	}
      }
    }
  }
}

//-----------------------------------------------------------------------------
const std::vector<Anp::Ptr<Anp::Exec::AlgCall> > Anp::MakeExecs::GetAlgCalls() const
{
  //
  // Return sorted vector of AlgCall entries - only executed algorithms are included
  //
  std::vector<Ptr<Exec::AlgCall> > calls;
  
  for(AlgMap::const_iterator it = fAlgCall.begin(); it != fAlgCall.end(); ++it) {
    const vector<Ptr<Exec::AlgCall> > &cvec = it->second;
    
    for(unsigned i = 0; i < cvec.size(); ++i) {
      const Ptr<Exec::AlgCall> &call = cvec.at(i);

      if(call->alg_data.isCalled()) {
	calls.push_back(call);
      }
    }
  }

  //
  // Sort algorithms by start time
  //
  std::sort(calls.begin(), calls.end(), SortAlgCall(""));
  
  return calls;
}

//-----------------------------------------------------------------------------
void Anp::MakeExecs::Fill()
{
  //
  // Fill algorithm and ROB summaries - run after Read() function
  //
  if(fDebug) {
    log() << "Fill - read " << fAlgCall.size() << " algorithm execution(s)" << endl;
  }

  ++fSummary.nevent;

  //
  // Get LB data
  //
  LBData *data = GetLB(fEvent.run, fEvent.lb);

  for(AlgMap::const_iterator it = fAlgCall.begin(); it != fAlgCall.end(); ++it) {
    const vector<Ptr<Exec::AlgCall> > &alg_vec = it->second;
    
    for(unsigned i = 0; i < alg_vec.size(); ++i) {
      const Exec::AlgCall &alg = alg_vec.at(i).ref();      
      fSummary.total_time += alg.alg_data.getTimer();

      for(unsigned j = 0; j < fClientVec.size(); ++j) {
	fClientVec.at(j)->ProcessAlg(alg);
      }
      
      if(data) data->ProcessAlg(alg);

      if(fDebug) {
	alg.Print();
	cout << endl;
      }
    }
  }
  
  for(unsigned j = 0; j < fClientVec.size(); ++j) {
    fClientVec.at(j)->FinishEvent(fEvent);
  }
  
  if(data) data->FinishEvent(fEvent);
}

//-----------------------------------------------------------------------------
Anp::MakeExecs::LBData* Anp::MakeExecs::GetLB(unsigned run, unsigned lb)
{
  //
  // Find - prepare on first call - clients for single lumiblock
  //
  ClientMap::iterator it = fClientMap.find(lb);
  if(it == fClientMap.end()) {

    //
    // Client for one LB
    //
    stringstream suffix_str;
    suffix_str << "_run" << setw(7) << setfill('0') << run
	       << "_lb" << setw(4) << setfill('0') << lb;

    //
    // Create clients for new lumi block
    //
    LBData data;
    
    data.run        = run;
    data.lb         = lb;    
    data.count      = Handle<ExecClient>(new ExecCount());

    if((fEvent.lb % fNLBSummHtml)==0){
      //
      // Preparing full data summaries for certain LBs
      //
      if(WriteFullHtml(lb)){ 
	data.clients    = BookClients("ClientListLBs", suffix_str.str(), "longLB");
	
	//
	// For Full summaries - make sure to write a few histograms
	//
	Registry count_reg; 
      
	count_reg.Set("MakeAlgHist",  "yes");
      
	data.count->SetReg(count_reg);
      
	if(!suffix_str.str().empty()) {
	  data.count->SetDir(Anp::GetDir(fDir, "sum"+suffix_str.str()));
	}
      }
      else                  data.clients    = BookClients("ClientListLBs", suffix_str.str(), "short");

      data.summary.alg_conf   = fSummary.alg_conf;
      data.summary.chn_conf   = fSummary.chn_conf;
      data.summary.seq_conf   = fSummary.seq_conf;    
      data.summary.rob_conf   = fSummary.rob_conf;
    }
    data.summary.farm_node  = fSummary.farm_node;

    it = fClientMap.insert(ClientMap::value_type(lb, data)).first;
  }

  return &(it ->second);
}

//-----------------------------------------------------------------------------
std::vector<Anp::Handle<Anp::ExecClient> > Anp::MakeExecs::BookClients(const std::string &regkey,
								       const std::string &suffix,
								       const std::string &option) const
{
  //
  // Book all clients
  //
  vector<string> client_list;
  vector<Handle<ExecClient> > clients;

  //
  // Read list of clients
  //
  fReg.Get("MakeExecs::"+regkey, client_list);

  for(unsigned i = 0; i < client_list.size(); ++i) {
    const string &client_name = client_list.at(i);

    Registry client_reg;    
    if(!fReg.Get(client_name, client_reg)) {
      continue;
    }
    
    //
    // Client common for all luminosity blocks
    //
    client_reg.Set("ClientName", client_name+suffix);

    if(option.find("short") != string::npos) {
      // Making short lb summary
      client_reg.Set("ClientDir",   "");
      client_reg.Set("MakeHist",    "no");
      client_reg.Set("ShortClient", "yes");
    }
    else if(option.find("longLB") != string::npos) {
      // Making a lb full summary - only make a few plots
      //client_reg.Set("ClientDir",   client_name+suffix);
      client_reg.Set("ClientDir",   "");
      client_reg.Set("MakeHist",    "no");
      client_reg.Set("MakeAlgHist", "no");
    }
    else{
      // Making a global full summary
      //client_reg.Set("MakeHist",    "yes");
      client_reg.Set("ClientDir",   client_name+suffix);
    }

    //
    // Book one client
    //   

    Handle<ExecClient> client = BookClient(client_reg);
    
    if(client.valid()) {

      clients.push_back(client);

      if(fDebug) log() << "BookClients - created new client - check: " << client_name 
		       << " num clients: " << clients.size() 
		       << " client type: " << client->GetClientType() 
		       << " link: "        << client->GetClientLink()  << endl;

      if(option.find("print") != string::npos) {
	log() << "BookClients - created new client: " << client_name << endl;
      }

    }
  }

  return clients;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::ExecClient> Anp::MakeExecs::BookClient(const Registry &reg) const
{
  //
  // Book one client for cost summary
  //
  Handle<ExecClient> client(0);
  
  string client_type, client_name;
  if(!reg.Get("ClientType", client_type) ||
     !reg.Get("ClientName", client_name)) {
    return client;
  }
  
  //
  // Create client of cost data
  //
  client = Factory<ExecClient>::Instance().Create(client_type);

  if(!client.valid()) {
    log() << "BookClient - failed to create client: " << client_type << endl;
    return client;
  }

  if(fDebug) {
    log() << "BookClient - created new client: " << client_name << endl;
  }

  string client_dir  = client_name;
  bool   client_pass = false;
  bool   client_IPag = false;

  reg.Get("ClientDir", client_dir);
  reg.Get("PassLB",    client_pass);  
  reg.Get("PassIPage", client_IPag);

  client->SetClientName(client_name);
  client->SetClientType(client_type);
  client->SetClientPass(client_pass);
  client->SetClientIPag(client_IPag);
  client->SetReg(reg);

  if(!client_dir.empty()) {
    client->SetDir(Anp::GetDir(fDir, client_dir));
  }

  return client;
}

//-----------------------------------------------------------------------------
Anp::Ptr<Anp::Exec::ROBConf> Anp::MakeExecs::GetConfROB(unsigned robid)
{
  //
  // Lookup pointer to trigger sequence configuration
  // 
  if(!fSummary.rob_conf){
    fSummary.rob_conf = new Exec::ROBConfMap();
  }

  Exec::ROBConfMap::iterator it = fSummary.rob_conf->find(robid);
  if(it == fSummary.rob_conf->end()) {
    const RobService &rob_svc = RobService::Instance();
    
    //
    // First instance of this rob id: create and fill new ROBConf object
    //
    Ptr<Exec::ROBConf> rob = ObjectFactory<Exec::ROBConf>::Instance().CreateObject();

    rob->rob_id   = robid;
    rob->det_id   = rob_svc.GetSubDetId(robid);
    rob->ros_id   = rob_svc.GetRosId   (robid); 
    rob->robin_id = rob_svc.GetRobinId (robid); 
    
    if(rob->ros_id==0){
      cout << "ROS ID 0 for ROB id: " << robid << endl;
    }

    rob->det_name   = rob_svc.GetSubDetFromId(rob->det_id);
    rob->ros_name   = rob_svc.GetRosFromId   (rob->ros_id);
    rob->robin_name = rob_svc.GetRobinFromId (rob->robin_id);
    
    it = fSummary.rob_conf->insert(Exec::ROBConfMap::value_type(robid, rob)).first;
  }
  
  return it->second;
}

//-----------------------------------------------------------------------------
string Anp::MakeExecs::level() const
{  
  if     (fLevel == 0) return "AL";
  if     (fLevel == 2) return "L2";
  else if(fLevel == 3) return "EF";
  
  return "UN";
}

//-----------------------------------------------------------------------------
std::string Anp::MakeExecs::ConvertUTC(const uint32_t sec, const uint32_t nsec) const
{
  //
  // L1 Event time as string
  //
  uint64_t utc = static_cast<uint64_t>(static_cast<double>(sec)*1.0e+9);
  utc += nsec;
  time_t timeval = static_cast<uint64_t>(static_cast<double>(utc)*1.0e-9);
  return ctime(&timeval);
}

//-----------------------------------------------------------------------------
bool Anp::MakeExecs::WriteFullHtml(unsigned lb)
{
  //
  // Write full html summary for this LB
  //
  if(fNLBFullHtml < 1) {
    return false;
  }
 
  //
  // If menu has changed, then write a full summary. 
  //   -- Reset flag fNewMenu to wait for the next new menu/prescale change
  //
  if(fNewMenu){
    fNewMenu=false;
    return true;
  }
  
  if((lb % fNLBFullHtml) == 0) {
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
Anp::Handle<Anp::ExecClient> Anp::MakeExecs::SelectLBClient(const std::vector<Handle<ExecClient> > &clients,
							    const ExecClient& parent) const
{
  //
  // Select matching client from list of clients
  //
  Anp::Handle<Anp::ExecClient> client;

  for(unsigned j = 0; j < clients.size(); ++j) {
    const Handle<ExecClient> &client_lb = clients.at(j);
	
    if(client_lb->GetClientType() != parent.GetClientType()) {
      continue;
    }
    
    if(client_lb->GetClientName().find(parent.GetClientName()) == string::npos) {
      continue;
    }
    
    if(client.valid()) {
      log() << "SelectLBClient - logic error: client: " << client->GetClientName() << endl;
    }
    else {
      client = client_lb;
    }
  }

  return client;
}
