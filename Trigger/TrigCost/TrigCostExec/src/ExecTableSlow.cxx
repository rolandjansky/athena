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
// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/ExecTableSlow.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableSlow)

using namespace std;

namespace SEQBits
{
  const uint32_t maskLow16  = 0x0000ffff;  // mask low 16 bits
  const uint32_t maskIndex  = 0xfff00000;  // mask index bits  
  const uint32_t shiftIndex = 20;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::SlowEvt::Clear()
{
  sEvt.Reset();
  totEvt.clear();
  sROS.clear();
  sChn.clear();
  sAlg.clear();
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableSlow::ExecTableSlow()
  :fDir(0),
   fCanvas(0),
   fDebug(true),
   fFillNode(false),
   fMakeHist(false),
   fWriteSeq(false),
   fCollectSlow(true),
   fDetailSpecific(false),
   fWriteTxt(false),
   fWriteXML(false),
   fTimeout(0),
   fDirName(),
   fnSlow(0),
   fnProc(0),
   fnEvent(0),
   fRanSel()
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableSlow::~ExecTableSlow()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::SetReg(const Registry &reg)
{
  //
  // Copy configuration
  //
  fReg = reg;
  
  //
  // Read configuration
  //
  reg.Get("Debug",           fDebug);
  reg.Get("Level",           fLevel);
  reg.Get("Timeout",         fTimeout);
  reg.Get("HistOpt",         fHistOpt);
  reg.Get("MakeHist",        fMakeHist);
  reg.Get("FillNode",        fFillNode);
  reg.Get("ClientLink",      fClientLink);
  reg.Get("NumSlowEvt",      fnSlow);
  reg.Get("WriteSeq",        fWriteSeq);
  reg.Get("CollectSlowest",  fCollectSlow);
  reg.Get("WriteTxtEvt",     fWriteTxt);
  reg.Get("WriteXMLEvt",     fWriteXML);
  reg.Get("DirName",         fDirName);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Save alg call in case the event times out
  //
  fAlg.push_back(alg);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::ProcessSlowAlg(const Exec::AlgCall &alg)
{
  //
  // Count one algorithm execution
  //
  if(fSlowEvt.totEvt.size()==0) cout << "invalid slow event pointer" << endl;
  fSlowEvt.totEvt.at(0)->ProcessAlg(alg);
  fActiveClients.push_back(fSlowEvt.totEvt.at(0).get());

  ExecClient *algClient = GetAlgClient(*alg.alg_conf);
  algClient->ProcessAlg(alg);
  fActiveClients.push_back(algClient);

  ExecClient *chnClient = GetChnClient(*alg.chn_conf);
  chnClient->ProcessAlg(alg);
  fActiveClients.push_back(chnClient);

  for(set<uint32_t>::const_iterator rit = alg.ros_ids.begin(); rit != alg.ros_ids.end(); ++rit) {
    const uint32_t ros_id = *rit;
    
    ExecClient *rosClient = GetROSClient(ros_id, alg);
    rosClient->ProcessAlg(alg);
    
    fActiveClients.push_back(rosClient);
  }

}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::FinishEvent(const Exec::Event &event)
{
  //
  // If event exceeds limits, then process and save it. Then clear fAlg for the next event
  //
  if((fTimeout< event.timer_alg || fTimeout< event.timer_steer) && fCollectSlow){

    //ProcessEvent(event);
    SaveEvent(event);
    //
    // Sort and then remove the fastest events
    //
    if((fnProc%200)==0 && fSlowVec.size()>fnSlow){
      
      std::sort(fSlowVec.begin(), fSlowVec.end(), SortExecSlow("timer_steer"));
      fSlowVec.resize(fnSlow);
    }
  }
  //
  // If random events are requested
  //
  if(!fCollectSlow){
    if(fDetailSpecific){
      fSlowEvt.sEvt     = event;
      fSlowEvt.AlgCalls = fAlg;
      ProcessEvent(fSlowEvt);  
    }
    else if(fRanSel.Rndm()> .999 && fSlowVec.size()<fnSlow)     SaveEvent(event);

    //if(fRanSel.Rndm()> .999 && fSlowVec.size()<fnSlow) ProcessEvent(event);
    //if( && fSlowVec.size()<fnSlow) SaveEvent(event);
  }

  fAlg.clear();
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::SaveEvent(const Exec::Event &event)
{
  //
  // Saving the event in a SlowEvt
  //
  fSlowEvt.sEvt     = event;
  fSlowEvt.AlgCalls = fAlg;
  
  fSlowVec.push_back(fSlowEvt);
  fSlowEvt.Clear();
  ++fnProc;

}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::ProcessEvent(Anp::ExecTableSlow::SlowEvt &se)
{
  //
  // Setting the event client in fSlowEvt
  //
  //fSlowEvt.sEvt     = event;
  const Exec::Event &event = se.sEvt;
  const AlgVec      &algs  = se.AlgCalls; 
  //cout << "SlowEvt::ProcessEvent - steering time: " << event.timer_steer << " number of algcalls: " << algs.size() << endl;
  if(!fDetailSpecific || fCollectSlow) fSlowEvt.Clear();
  fSlowEvt.sEvt     = event;
  fSlowEvt.AlgCalls = se.AlgCalls;
  
  GetEvtClient(event);
  
  for(unsigned alg =0;alg<algs.size();++alg){
    ProcessSlowAlg(algs.at(alg));
  }
  
  //
  // Remove duplicate pointers
  //
  std::sort(fActiveClients.begin(), fActiveClients.end());
  ClientVec::iterator it = std::unique(fActiveClients.begin(), fActiveClients.end());
  fActiveClients.erase(it, fActiveClients.end());
  
  //
  // Count one event
  //
  for(unsigned i = 0; i < fActiveClients.size(); ++i) {
    fActiveClients.at(i)->FinishEvent(event);
  }
  
  fActiveClients.clear();
    
  //
  // Comment out for normal running
  //
  if(fDetailSpecific && !fCollectSlow){

    ExecMap &roses = fSlowEvt.sROS;
    bool keepEvt = false;
    for(ExecMap::iterator ros = roses.begin();ros!= roses.end(); ++ros){
      //const string &ros_name   = ros->second->GetClientName();
      const Exec::Count &count = ros->second->GetCount();
      //const float  &ros_size   = Exec::ConvertWordsToKB(count.rob_size);
      //if(ros_name =="ROS-PIX-B-03" && ros_size>10.0) keepEvt=true;
      //const Exec::Count &count = ros->second->GetCount();
      //if(count.ros_nreq) {
      //	if(count.ros_nreq_retrv == 0){
      //	  cout << "ExecTableSlow::ProcessEvent - found event with with an ROI in which no retrievals were made but they were requested from ROS: " << ros->second->GetClientName() << endl;
      //	  keepEvt=true;
      //	}
      
      if(count.ros_n0ret>0) {
	//cout << "ExecTableSlow::ProcessEvent - found event with with an ROI in which no retrievals were made but they were requested from ROS: " << count.ros_n0ret << endl;
	keepEvt=true;
      }	  
      }
      //}
    
    if(!fCollectSlow && keepEvt) fSlowVec.push_back(fSlowEvt);
    fSlowEvt.Clear();
    ++fnProc;
  }
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::ExecTableSlow::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::GetEvtClient(const Exec::Event &event)
{
  //
  // Make client for event
  //

    Registry reg(fReg);
    reg.Set("DoAlg", true);
    reg.Set("DoDet", true);
    reg.Set("DoROS", true);
    reg.Set("DoChn", false);

    stringstream link_str;
    stringstream evtnum_str;
    link_str << "EVT_NAME_" << setw(10) << setfill('0') << event.event_id;
    evtnum_str << event.event_id;

    Handle<ExecClient> client(new ExecCount());

    client->SetClientName(evtnum_str.str());
    client->SetClientLink(link_str.str());
    client->SetClientType("event");
    client->SetReg(reg);
    //client->SetDir(Anp::GetDir(fDir, "slow_event/"+evtnum_str.str()));

    fSlowEvt.totEvt.push_back(client);
}

//--------------------------------------------------------------------------------------      
Anp::ExecClient* Anp::ExecTableSlow::GetChnClient(const TrigConfChain &chn_conf)
{
  //
  // Make client for chain
  //
  unsigned counter = chn_conf.getEncodedId();

  ExecMap::iterator ichn = fSlowEvt.sChn.find(counter);
  if(ichn == fSlowEvt.sChn.end()) {

    Registry reg(fReg);
    reg.Set("DoAlg", true);
    reg.Set("DoDet", false);
    reg.Set("DoROS", true);
    reg.Set("DoChn", true);
    reg.Set("ChnId", counter);

    if(fDebug) {
      cout << "ExecTableChn::GetChnClient - " << chn_conf.getName() << endl;
    }

    stringstream link_str;
    link_str << "SLOW_CHN_NAME_" << setw(10) << setfill('0') << counter;

    Handle<ExecClient> client(new ExecCount());
    ichn = fSlowEvt.sChn.insert(ExecMap::value_type(chn_conf.getEncodedId(), client)).first;
    
    client->SetClientName(chn_conf.getName());
    client->SetClientLink(link_str.str());
    client->SetClientType("chain");
    client->SetReg(reg);
    //client->SetDir(Anp::GetDir(fDir, "slow_chn/"+chn_conf.getName()));
  }    

  return ichn->second.get();
}

//--------------------------------------------------------------------------------------      
Anp::ExecClient* Anp::ExecTableSlow::GetAlgClient(const TrigConfAlg &alg_conf)
{
  //
  // Make client for algorithm
  //
  ExecMap::iterator ialg = fSlowEvt.sAlg.find(alg_conf.getNameId());
  if(ialg == fSlowEvt.sAlg.end()) {

    Registry reg(fReg);
    reg.Set("DoAlg", false);
    reg.Set("DoDet", false);
    reg.Set("DoROS", true);
    reg.Set("AlgId", alg_conf.getNameId());

    if(fDebug) {
      cout << "ExecTableAlg::GetAlgClient - " << alg_conf.getName() << endl;
    }

    Handle<ExecClient> client(new ExecCount());
    ialg = fSlowEvt.sAlg.insert(ExecMap::value_type(alg_conf.getNameId(), client)).first;
    
    client->SetClientName(alg_conf.getName());
    client->SetClientType("algorithm");
    client->SetReg(reg);
    //client->SetDir(Anp::GetDir(fDir, "slow_alg/"+alg_conf.getName()));
  }    

  return ialg->second.get();
}

//--------------------------------------------------------------------------------------
Anp::ExecClient* Anp::ExecTableSlow::GetROSClient(unsigned ros_id, const Exec::AlgCall &alg)
{
  //
  // Make client for algorithm
  //
  ExecMap::iterator iros = fSlowEvt.sROS.find(ros_id);
  if(iros == fSlowEvt.sROS.end()) {

    Registry reg(fReg);
    reg.Set("DoAlg", true);
    reg.Set("DoDet", false);
    reg.Set("DoROS", false);
    if(fDetailSpecific) reg.Set("DoRoi", true);
    reg.Set("ROSId", ros_id);
    
    Handle<ExecClient> client(new ExecCount());
    iros = fSlowEvt.sROS.insert(ExecMap::value_type(ros_id, client)).first;
    
    client->SetClientName(alg.GetROSName(ros_id));
    client->SetClientType("");
    client->SetReg(reg);
    //client->SetDir(Anp::GetDir(fDir, alg.GetROSName(ros_id)));
  }

  return iros->second.get();
}

//-------------------------------------------------------------------------------------- 
void Anp::ExecTableSlow::WriteExec(HtmlNode &anode, HtmlMenu &hmenu, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteExec - starting WriteExec" << endl;
  //
  // Write out results
  //
  ostream &os = anode.GetCont();

  if(fCollectSlow) os << "List of " << fnSlow << " Slowest Events" << endl;  
  else os << "List of " << fnSlow << " Randomly Selected Events" << endl;

  //
  // Write algCalls
  //

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Run </th>"
     << "<th> Lumi Block </th>"
     << "<th> Bunch Crossing Id </th>"
     << "<th title='The UTC time for the event'> L1 Event Time </th>"
     << "<th title='The UTC ns time for the event'> L1 Event Time (ns) </th>"
     << "<th> L2PU Hash Id </th>"
     << "<th> Steering Time (ms) </th>";
  if(GetLevel() == 0 || GetLevel() == 2) os << "<th> Alg Time (ms) </th>";
  os << "<th> ROB Time (ms) </th>"
     << "<th> Event Number </th>"
     << "</tr>" << endl;

  //
  // Sort evts by total execution time
  //
  std::sort(fSlowVec.begin(), fSlowVec.end(), SortExecSlow("timer_steer"));

  //
  // Process the slow Events
  //
  //cout << "Length of Slow Vector: " << fSlowVec.size() << endl;
  if(!fDetailSpecific || fCollectSlow){
    for(SlowEvtVec::iterator sl = fSlowVec.begin(); sl !=fSlowVec.end(); ++sl){ 
      ProcessEvent(*sl);
      *sl = fSlowEvt;
    }
  }
  unsigned maxPrint = min(fnSlow,static_cast<unsigned int>(fSlowVec.size()));

  for(unsigned i = 0; i < maxPrint; ++i) {
    Exec::Event &evt = fSlowVec.at(i).sEvt;    
    if(fSlowVec.at(i).totEvt.size() == 0) return;
    //
    // Creating Node for detailed Event information
    //
    stringstream evt_id;
    stringstream lb_id;
    evt_id << evt.event_id;
    lb_id  << "lb"  << evt.lb << "_" << fnEvent << "evt";
    Handle<HtmlNode> nodeA(new HtmlNode(sum.html_base+"_"+lb_id.str()+evt_id.str()+"_slowt.html", HtmlNode::kHTML));
    ++fnEvent;

    anode.AddChild(nodeA);

    nodeA->GetCont() << "<pre>" << endl;
    WriteEvt(nodeA.ref(), hmenu, fSlowVec.at(i), sum);
    nodeA->GetCont() << "</pre>" << endl;

    const std::string evt_num_link = nodeA->GetLink(evt_id.str());
    
    //
    // Write HTML table entries
    //
    os << "<tr>" 
       << "<td align=\"left\"> "   << evt.run                      << " </td>"
       << "<td align=\"center\"> " << evt.lb                       << " </td>"
       << "<td align=\"center\"> " << evt.bcid                     << " </td>"
       << "<td align=\"center\"> " << evt.L1_time                  << " </td>"
       << "<td align=\"center\"> " << evt.nsec                     << " </td>"
       << "<td align=\"center\"> " << evt.host_id                  << " </td>"
       << "<td align=\"center\"> " << evt.timer_steer              << " </td>"
       << "<td align=\"center\"> " << evt.timer_alg                << " </td>";
    if(fLevel==2) os << "<td align=\"center\"> " << evt.timer_rob                << " </td>";
    os << "<td align=\"center\"> " << evt_num_link                 << " </td>"
       << "</tr>" << endl;
  }
  
  os << "</table>" << endl;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteEvt(HtmlNode &nodeE, HtmlMenu &hmenu, const SlowEvt &sevt, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteEvt - starting WriteEvt" << endl;
  //
  // Write detailed information about Event
  //
  ostream &os = nodeE.GetCont();

  const Exec::Event &evt = sevt.sEvt;
  if(sevt.totEvt.size()==0){
    cerr << "ExecTableSlow::WriteEvt - bad event pointer" << endl;
    return;
  }
  const Exec::Count &c   = sevt.totEvt.at(0)->GetCount();

  if(fWriteXML) os << "<event"
		   << " id='"             << evt.event_id                 << "'"
		   << " run='"            << evt.run                      << "'"
		   << " lb='"             << evt.lb                       << "'"
		   << " bcid='"           << evt.bcid 	                  << "'"
		   << " L1evtTime='"      << evt.L1_time                  << "'"
		   << " L1evtTimens='"    << evt.nsec 	                  << "'"
		   << " L2PUid='"         << evt.host_id                  << "'"
		   << " L2PUName='"       << sum.GetNodeName(evt.host_id) << "'"
		   << " algTime='"        << evt.timer_alg                << "'"
		   << " steerTime='"      << evt.timer_steer              << "'"
		   << " ROBTime='"        << evt.timer_rob                << "'"
		   << " ActiveAlgCalls='" << c.alg_ncalls                 << "'"
		   << " CachedAlgCalls='" << c.alg_ncache                 << "'";
  else if(fWriteTxt){ 

    os << "EVENT_HEADER,"
       << "evtid,"            
       << "run,"           
       << "lb,"            
       << "bcid,"          
       << "L1evtTime,"     
       << "L1evtTimens,"   
       << "L2PUid,"        
       << "L2PUName,"      
       << "algTime,"       
       << "steerTime,"     
       << "ROBTime,"       
       << "ActiveAlgCalls,"
       << "CachedAlgCalls";
    if(GetLevel() == 0 || GetLevel() == 2) os << ",ROBDataCollCalls," 
					      << "dataCollretrieved,"
					      << "dataCollcached,"   
					      << "reqROBSize,"       
					      << "retROBSize,"       
					      << "cachedROBSize,"    
					      << "otherROBSize,"     
					      << "nROSReq,"          
					      << "RetnROSReq,"       
					      << "cachednROSReq,"    
					      << "othernROSReq";
    os << endl;
    os << "EVENT_HEADER,"
       << evt.event_id                 << ","
       << evt.run                      << ","
       << evt.lb                       << ","
       << evt.bcid 	               << ","
       << evt.L1_time                  << ","
       << evt.nsec 	               << ","
       << evt.host_id                  << ","
       << sum.GetNodeName(evt.host_id) << ","
       << evt.timer_alg                << ","
       << evt.timer_steer              << ","
       << evt.timer_rob                << ","
       << c.alg_ncalls                 << ","
       << c.alg_ncache;
  }

  else {
    os << "Run:                  "  << evt.run                         << endl
       << "  Lumi Block:         "  << evt.lb                          << endl
       << "  Event Number:       "  << evt.event_id                    << endl  
       << "  Bunch Crossing Id:  "  << evt.bcid 	               << endl
       << "  L1 Event Time:      "  << evt.L1_time
       << "  L1 Event Time (ns): "  << evt.nsec 	               << endl
       << "  L2PU Hash Id:       "  << evt.host_id                     << endl
       << "  L2PU Name:          "  << sum.GetNodeName(evt.host_id)    << endl
       << endl			    
       << endl
       << "Summary for event: "                                   <<          endl
       << "  Algorithm time:                 " << evt.timer_alg   << " ms" << endl
       << "  Steering time:                  " << evt.timer_steer << " ms" << endl;
    if(GetLevel() == 0 || GetLevel() == 2)  os << "  ROB request time:               " << evt.timer_rob   << " ms" << endl;
    os << "  Active algorithm calls:         " << c.alg_ncalls    <<          endl
       << "  Cached algorithm calls:         " << c.alg_ncache    <<          endl;
  }
  if(GetLevel() == 0 || GetLevel() == 2) {

    const uint64_t rob_size = c.rob_size_retrv+c.rob_size_cache+c.rob_size_other;

    const string size_total_str = Anp::Round2Pair(Exec::ConvertWordsToKB(rob_size),         1.0).first;
    const string size_retrv_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size_retrv), 1.0).first;
    const string size_cache_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size_cache), 1.0).first;
    const string size_other_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size_other), 1.0).first;
    const string ostr = " (ignored, disabled or unclassified)\n";			    

    if(fWriteXML) os << "  ROBDataCollCalls='" << c.rob_nreq       << "'"
		     << "  dataCollretrieved='"<< c.rob_nreq_retrv << "'"
		     << "  dataCollcached='"   << c.rob_nreq_cache << "'"
		     << "  reqROBSize='"       << size_total_str   << "'"
		     << "  retROBSize='"       << size_retrv_str   << "'"
		     << "  cachedROBSize='"    << size_cache_str   << "'"
		     << "  otherROBSize='"     << size_other_str   << "'"
		     << "  nROSReq='"          << c.ros_nreq       << "'"
		     << "  RetnROSReq='"       << c.ros_nreq_retrv << "'"
		     << "  cachednROSReq='"    << c.ros_nreq_cache << "'"
		     << "  othernROSReq='"     << c.ros_nreq_other << "'";
    else if(fWriteTxt){
      os << ","
	 << c.rob_nreq       << ","
	 << c.rob_nreq_retrv << ","
	 << c.rob_nreq_cache << ","
	 << size_total_str   << ","
	 << size_retrv_str   << ","
	 << size_cache_str   << ","
	 << size_other_str   << ","
	 << c.ros_nreq       << ","
	 << c.ros_nreq_retrv << ","
	 << c.ros_nreq_cache << ","
	 << c.ros_nreq_other;
    }
    else{
      os << "  ROB DataCollector calls:        " << c.rob_nreq       << endl
	 << "    - retrieved:                  " << c.rob_nreq_retrv << endl
	 << "    - cached:                     " << c.rob_nreq_cache << endl
	 << "  Size of requested ROBs:         " << size_total_str   << " KB " << endl
	 << "    - retrieved:                  " << size_retrv_str   << " KB " << endl
	 << "    - cached:                     " << size_cache_str   << " KB " << endl
	 << "    - other:                      " << size_other_str   << " KB " << ostr
	 << "  Number of requested ROSs:       " << c.ros_nreq       << endl
	 << "    - retrieved:                  " << c.ros_nreq_retrv << endl
	 << "    - cached:                     " << c.ros_nreq_cache << endl
	 << "    - other:                      " << c.ros_nreq_other << ostr << endl;

      os << nodeE.GetAnchor("Summary of Subdetector requests for Event","subdetcalls") << endl
	 << nodeE.GetAnchor("Summary of ROS requests for Event","roscalls")            << endl;
    }
  }

  //
  // Write seq structure
  //
  if(fWriteXML){ 
    os << ">" << endl;
    WriteXMLEvent(nodeE,hmenu,sevt,sum);
    os << "</event>" << endl;
  }
  else if(fWriteTxt){
    os << endl << endl;
    WriteTxtEvent(nodeE,hmenu,sevt,sum);
  }
  else{
    os << endl << "Event Sequence Structure Summary Table: ";
    WriteMonEvent(nodeE,hmenu,sevt,sum);
  }
  //
  // Write the Chain table
  //
  //os << endl << "Chain Summary Table: " << endl;
  //
  //WriteChn(nodeE,hmenu,sevt,sum);

  //
  // Write Alg table
  //
  //os << endl << endl 
  //   << "Algorithm Summary Table: " << endl;
  //
  //WriteAlg(nodeE,hmenu,sevt,sum);

  if((GetLevel() == 0 || GetLevel() == 2) && !fWriteTxt) {
    //
    // Write Ros table
    //
    WriteROS(nodeE,hmenu,sevt,sum);

    //
    // Write SubDet table
    //
    WriteDet(nodeE,hmenu,sevt,sum);

  }

}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteMonEvent(HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - starting WriteMonEvent" << endl;
  //
  // Write execution summary for single event
  //
  ostream &os = anode.GetCont();
  unsigned nAlg     = 0;
  double   lastAlgt = 0.0;
  stringstream evt_id;
  evt_id << sevt.sEvt.event_id;
  
  os << "<head>"                                                                      
     << "<title>" << evt_id.str() << " Execution</title>" 		      	      << endl
     << "<script language=\"JavaScript\" src=\"jquery.min.js\"></script>"             << endl
     << "<script language=\"JavaScript\" src=\"jquery.columnfilters.js\"></script>"   << endl 
     << "<style type=\"text/css\">" 						      << endl
     << "table tr th {" 							      << endl
     << "	background-color: #d3DADE;" 					      << endl
     << "	padding: 3px;" 							      << endl
     << "} " 									      << endl
     << "table tr.rowb { background-color:#EAf2FD; }" 				      << endl
     << "table tr.filterColumns td { padding:2px; }" 				      << endl
     << "body { padding-bottom:150px; font-size:13px; }" 		              << endl
     << "</style>" 								      
     << "</head>";
  
  os << "--------------------------------------------------------------------------------------" << endl
     << "For Searches, the following commands are acceptable:"                                   << endl
     << "    wildCard: *"                                                                        << endl  
     << "    notCharacter: !"                                                                    << endl
     << endl
     << "When Searching for phrases not at the beginning of a column, a * must be used. "
     << "(i.e. when searching for ROS-PIX, use *ROS-PIX)"                                        << endl
     << "--------------------------------------------------------------------------------------" << endl;

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table id=\"filterTable1\" border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl 
     << "<thead>"
     << "<tr>"
     << "<th> Time (s) </th>"           
     << "<th> Chain Name </th>";
  if(fWriteSeq) os << "<th> Sequence Name </th>";
  os << "<th> Algorithm Name </th>" 
     << "<th> Signature Step </th>"
     << "<th> ROI Id </th>";
  if(fWriteSeq) os << "<th> Sequence Status </th>";
  os << "<th> Algorithm Status </th>"  
     << "<th> Algorithm time (ms) </th>";

  if(GetLevel() == 0 || GetLevel() == 2) {
    os << "<th> # Data request </th>"     
       << "<th> # Data retrievals </th>"
       << "<th> # ROB requests </th>"   
       << "<th> # ROB retrievals </th>"   
       << "<th> ROB request time (ms) </th>"   
       << "<th> ROB request size (KB) </th>"    
       << "<th> ROB retrieved size (KB) </th>"
       << "<th> ROS Retrieval Calls  </th>";
  }

    os << "</tr>"    << endl
       << "</thead>" << endl;

    vector<TrigMonSeq> seqs = sevt.sEvt.evt_data->getVec<TrigMonSeq>();

    if(fWriteSeq){
      //
      // Fill the seq conf map
      //
      vector<Exec::AlgCall> acal = sevt.AlgCalls;
      for(vector<Exec::AlgCall>::iterator it = acal.begin();it!=acal.end();++it){
	SeqConfMap::iterator seqConf = fSeqConfMap.find(it->seq_conf->getId());
	if(seqConf == fSeqConfMap.end()) fSeqConfMap.insert(SeqConfMap::value_type(it->seq_conf->getId(),it->seq_conf));
      }
      
      //for(unsigned seq_it = 0; seq_it<seqs.size();++seq_it){
      //	resetCachedSeq(seqs, seq_it, seqs.at(seq_it),sum);
      //	cout << "ExecTableSlow::WriteMonEvt - end reset size seqs: " << seqs.size() << " iterator: " << seq_it << endl;
      //}
    }

    for(vector<TrigMonSeq>::iterator seq = seqs.begin(); seq!=seqs.end();++seq){
      if(fWriteSeq){
	//
	// Write Sequence
	//
	os << "<tr>" 
	   << "<td align=\"center\">" << ""                                                << "</td>"
	   << "<td align=\"center\">" << sum.GetChnName(unsigned(seq->getChnCounter()))    << "</td>"
	   << "<td align=\"left\">"   << sum.GetSeqName(unsigned(seq->getSeqIndex()))      << "</td>"
	   << "<td align=\"center\">" << ""                                                << "</td>"
	   << "<td align=\"center\">" << ""                                                << "</td>"
	   << "<td align=\"center\">" << ""                                                << "</td>"
	   << "<td align=\"center\">" << Exec::ConvertSeqToStatus(*seq)                    << "</td>"
	   << "<td align=\"center\">" << ""                                                << "</td>"
	   << "<td align=\"center\">" << ""                                                << "</td>";

	if(GetLevel() == 0 || GetLevel() == 2) {
	  
	  os << "<td align=\"center\">" << "" << "</td>"
	     << "<td align=\"center\">" << "" << "</td>"
	     << "<td align=\"center\">" << "" << "</td>"
	     << "<td align=\"center\">" << "" << "</td>"
	     << "<td align=\"center\">" << "" << "</td>"
	     << "<td align=\"center\">" << "" << "</td>";
	}
	os << "</tr>" << endl;
      }
      //
      // get algorithms
      //
      const vector<TrigMonAlg> &algs = seq->getAlg();
	
	for(vector<TrigMonAlg>::const_iterator alg = algs.begin(); alg!=algs.end();++alg){
	  //
	  // Write algorithm
	  //

	  string alg_stat = "Called";
	  if(alg->isCached()) alg_stat = "Cached";
	  
	  double start_time = Exec::ConvertTimerToSec(alg->start());
	  if(start_time==0.0) start_time = lastAlgt+1.0e-6;
	  else lastAlgt = Exec::ConvertTimerToSec(alg->start());

	  const std::string start_timeS = Anp::Round2Pair(start_time,0.0001).first;

	  Exec::AlgCall call = GetAlgCall(*alg,*seq,sevt);
	  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - found GetAlgCall" << endl;
	  const TrigConfSeq   *seq_conf = call.seq_conf.get();
	  const TrigConfChain *chn_conf = call.chn_conf.get();
	  const TrigConfAlg   *alg_conf = call.alg_conf.get();
	  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - valid? seq: " << seq_conf << " chn: " << chn_conf << " alg: " << alg_conf << endl;
	  if(!(seq_conf && chn_conf && alg_conf)){ 
	    cout << "ExecTableSlow::WriteMonEvent  - unable to find the conf seq: " << seq_conf << "  chn: " << chn_conf << " alg: " << alg_conf << endl;
	    return;
	  }
	  const string seq_name = seq_conf->getName();
	  std::string chn_name  = chn_conf->getName();
	  std::string alg_name  = alg_conf->getName();

	  // 
	  // Find Signature step 
	  // 
	  const uint32_t seq_id = seq_conf->getId(); 
	  const vector<TrigConfSig> signatures = chn_conf->getSignature(); 
	  
	  unsigned sig_step = 0; 
	  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - looking for signature" << endl;
	  for(vector<TrigConfSig>::const_iterator sig = signatures.begin(); sig != signatures.end(); ++sig){ 
	    ++sig_step; 
	    if(sig->matchOutputTE(seq_id)) break; 
	  }
	  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - Found signature" << endl;
	  //
	  // Find RoIs
	  //
	  const vector<Exec::ROILoad> &roiLoad = call.roi_data;

	  stringstream roi_id_str;
	  if(roiLoad.size() ==0) roi_id_str << "No ROI";
	  else if(roiLoad.size() >0){
	    for(unsigned roi = 0; roi < roiLoad.size();++roi){
	      if(roi==0) roi_id_str << roiLoad.at(roi).roi_id;
	      else roi_id_str << ", " << roiLoad.at(roi).roi_id;
	    }
	  }
	  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - found ROI" << endl;
	  if(sum.write_full && (GetLevel()==0 || GetLevel()==2) && (0 < call.rob_data.size())) {
	    //
	    // Save algorithms to write at the end of the event
	    //
	    fAlg.push_back(call);

	    stringstream alg_id;
	    alg_id << "id" << alg_conf->getNameId() << "_alg" << nAlg;
	    alg_name = anode.GetAnchor(alg_name,alg_id.str());
	    ++nAlg;
	  }
	  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - Done Saving event" << endl;

	  os << "<tr>" 
	     << "<td align=\"center\">" << start_timeS          << "</td>"
	     << "<td align=\"center\">" << chn_name             << "</td>";
	  if(fWriteSeq)	os << "<td align=\"left\">"   << ""                   << "</td>";
	  os << "<td align=\"center\">" << alg_name             << "</td>"
	     << "<td align=\"center\">" << sig_step             << "</td>"
	     << "<td align=\"center\">" << roi_id_str.str()     << "</td>";
	  if(fWriteSeq) os  << "<td align=\"center\">" << ""                  << "</td>";
	  os << "<td align=\"center\">" << alg_stat             << "</td>"
	     << "<td align=\"center\">" << alg->getTimer()      << "</td>";
	  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - about to process call" << endl;
	  if(GetLevel() == 0 || GetLevel() == 2) {
	    //
	    // Calculating ros and rob counts for the AlgCall
	    //
	    fSlowEvt.Clear();
	    ExecClient *algClient = GetAlgClient(*(call.alg_conf));
	    algClient->ProcessAlg(call);
	    algClient->FinishEvent(sevt.sEvt);
	    
	    const Exec::Count &count = algClient->GetCount();
	    
	    const uint64_t rob_size = count.rob_size_retrv+count.rob_size_cache+count.rob_size_other;
	    
	    const string rob_timer_str  = Anp::Round2Pair(count.timer_rob,         0.1).first;
	    const string size_total_str = Anp::Round2Pair(Exec::ConvertWordsToKB(rob_size),         1.0).first;
	    const string size_retrv_str = Anp::Round2Pair(Exec::ConvertWordsToKB(count.rob_size_retrv), 1.0).first;

	    os << "<td align=\"center\">"                 << count.rob_nreq       << "</td>"
	       << "<td align=\"center\">"                 << count.rob_nreq_retrv << "</td>"
	       << "<td align=\"center\">"                 << count.rob_nids       << "</td>"
	       << "<td align=\"center\">"                 << count.rob_nids_retrv << "</td>"
	       << "<td align=\"center\">"                 << rob_timer_str        << "</td>"
	       << "<td align=\"center\">"                 << size_total_str       << "</td>"
	       << "<td align=\"center\">"                 << size_retrv_str       << "</td>"
	       << "<td align=\"left\" nowrap=\"nowrap\">" << getROSCalls(call,sum)<< "</td>";
	    
	  }
	  if(fDebug)  cout << "ExecTableSlow::WriteMonEvent - call processed" << endl;
	  os << "</tr>" << endl;
	}
    }

    string excludeCol="0,6,7,8,9,10,11,12,13";
    if(fWriteSeq) excludeCol="0,8,9,10,11,12,13,14,15";

    os << "<script>" << endl
       << "$(document).ready(function() {" << endl
       << "$('table#filterTable1').columnFilters({alternateRowClassNames:['rowa','rowb'], excludeColumns:[" 
       << excludeCol << "]});" << endl
       << "});"       << endl
       << "</script>" << endl
       << "<br />"    << endl
       << "</table>"  << endl 
       << endl;
    
    //
    // Create algorithm request page
    //
    if((int(nAlg)-int(fAlg.size())) > -1) nAlg-=fAlg.size();
    for(AlgVec::iterator alg = fAlg.begin(); alg!=fAlg.end();++alg){

      stringstream alg_id;

      alg_id << "id" << alg->alg_conf->getNameId() << "_alg" << nAlg;
      
     os << "<pre>"
	<< "-------------------------------------------------------------------------------------------------------------------------" << endl;
     anode.AddAnchor(alg->alg_conf->getName(),alg_id.str(),true);

      ++nAlg;
      //
      // Write Detail about the AlgCall
      //
      WriteAlgCall(anode,hmenu,*alg,sum);
    }
    fAlg.clear();
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteTxtEvent(HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteTxtEvent - starting WriteTxtEvent" << endl;
  //
  // Write execution summary for single event
  //
  ostream &os = anode.GetCont();
  double   lastAlgt = 0.0;
  stringstream evt_id;
  evt_id << sevt.sEvt.event_id;

    vector<TrigMonSeq> seqs = sevt.sEvt.evt_data->getVec<TrigMonSeq>();
    //
    // alg heading
    //
    os << "ALG_CALL,"
       << "start_time,"
       << "       chn_name,"  
       << "       seq_name,"  
       << "       alg_name,"  
       << "  sig_step,"  
       << "  roi_id,"    
       << "  alg_status,"
       << "  alg_timer";
    //
    // ROB headings
    //
    if(GetLevel() == 0 || GetLevel() == 2) {
      os  << ",  data_nreq,"      
	  << "  data_nreq_retrv,"
	  << "  rob_nids,"      
	  << "  rob_nids_retrv,"
	  << "  rob_timer,"       
	  << "  data_size_req,"    
	  << "  data_size_retrv";   
      //	  << "rosCallsRet";
    }
    os << endl;

    for(vector<TrigMonSeq>::iterator seq = seqs.begin(); seq!=seqs.end();++seq){
      //
      // get algorithms
      //
      const vector<TrigMonAlg> &algs = seq->getAlg();
	
	for(vector<TrigMonAlg>::const_iterator alg = algs.begin(); alg!=algs.end();++alg){
	  //
	  // Write algorithm
	  //
	  string alg_stat = "Called";
	  if(alg->isCached()) alg_stat = "Cached";
	  
	  double start_time = Exec::ConvertTimerToSec(alg->start());
	  if(start_time==0.0) start_time = lastAlgt+1.0e-6;
	  else lastAlgt = Exec::ConvertTimerToSec(alg->start());

	  const std::string start_timeS = Anp::Round2Pair(start_time,0.0001).first;

	  Exec::AlgCall call = GetAlgCall(*alg,*seq,sevt);

	  const TrigConfSeq   *seq_conf = call.seq_conf.get();
	  const TrigConfChain *chn_conf = call.chn_conf.get();
	  const TrigConfAlg   *alg_conf = call.alg_conf.get();
	  
	  if(!(seq_conf && chn_conf && alg_conf)){ 
	    cout << "ExecTableSlow::WriteTxtEvent  - ERROR unable to find the conf seq: " << seq_conf << "  chn: " << chn_conf << " alg: " << alg_conf << endl;
	    return;
	  }

	  const string seq_name = seq_conf->getName();
	  std::string chn_name  = chn_conf->getName();
	  std::string alg_name  = alg_conf->getName();

	  // 
	  // Find Signature step 
	  // 
	  const uint32_t seq_id = seq_conf->getId(); 
	  const vector<TrigConfSig> signatures = chn_conf->getSignature(); 
	  
	  unsigned sig_step = 0; 
	  
	  for(vector<TrigConfSig>::const_iterator sig = signatures.begin(); sig != signatures.end(); ++sig){ 
	    ++sig_step; 
	    if(sig->matchOutputTE(seq_id)) break; 
	  }
	  //
	  // Find RoIs
	  //
	  const vector<Exec::ROILoad> &roiLoad = call.roi_data;

	  stringstream roi_id_str;
	  if(roiLoad.size() ==0) roi_id_str << "No ROI";
	  else if(roiLoad.size() >0){
	    for(unsigned roi = 0; roi < roiLoad.size();++roi){
	      if(roi==0) roi_id_str << roiLoad.at(roi).roi_id;
	      else roi_id_str << "-" << roiLoad.at(roi).roi_id;
	    }
	  }

	  os  << "ALG_CALL,"
	      << start_timeS                                  << ","
	      << chn_name                                     << ","
	      << sum.GetSeqName(unsigned(seq->getSeqIndex())) << ","
	      << alg_name                                     << ",     "
	      << sig_step                                     << ",  "
	      << roi_id_str.str()                             << ",  "
	      << alg_stat                                     << ",  "
	      << alg->getTimer();
	  
	  if(GetLevel() == 0 || GetLevel() == 2) {
	    //
	    // Calculating ros and rob counts for the AlgCall
	    //
	    fSlowEvt.Clear();
	    ExecClient *algClient = GetAlgClient(*(call.alg_conf));
	    algClient->ProcessAlg(call);
	    algClient->FinishEvent(sevt.sEvt);
	    
	    const Exec::Count &count = algClient->GetCount();
	    
	    const uint64_t rob_size = count.rob_size_retrv+count.rob_size_cache+count.rob_size_other;
	    
	    const string rob_timer_str  = Anp::Round2Pair(count.timer_rob,         0.1).first;
	    const string size_total_str = Anp::Round2Pair(Exec::ConvertWordsToKB(rob_size),         1.0).first;
	    const string size_retrv_str = Anp::Round2Pair(Exec::ConvertWordsToKB(count.rob_size_retrv), 1.0).first;

	    os << ","
	       << count.rob_nreq        << ",  "  
	       << count.rob_nreq_retrv  << ",  "
	       << count.rob_nids        << ",  "  
	       << count.rob_nids_retrv  << ",  "
	       << rob_timer_str         << ",  "  
	       << size_total_str        << ",  "  
	       << size_retrv_str;//        << ","  
	    //	       << getROSCalls(call,sum);
	    os << endl;
	    if(0 < call.rob_data.size()) WriteTxtCall(anode,hmenu,call,sum);
	    
	  }
	  else os << endl;
	}
    }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteXMLEvent(HtmlNode &anode, HtmlMenu &hmenu, const SlowEvt &sevt, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteXMLEvent - starting WriteXMLEvent" << endl;
  //
  // Write execution summary for single event
  //
  ostream &os = anode.GetCont();
  double   lastAlgt = 0.0;
  stringstream evt_id;
  evt_id << sevt.sEvt.event_id;

    vector<TrigMonSeq> seqs = sevt.sEvt.evt_data->getVec<TrigMonSeq>();

    os << "  <AlgCalls>" << endl;

    for(vector<TrigMonSeq>::iterator seq = seqs.begin(); seq!=seqs.end();++seq){
      //
      // get algorithms
      //
      const vector<TrigMonAlg> &algs = seq->getAlg();
	
	for(vector<TrigMonAlg>::const_iterator alg = algs.begin(); alg!=algs.end();++alg){
	  //
	  // Write algorithm
	  //
	  os << "    <algCall";
	  string alg_stat = "Called";
	  if(alg->isCached()) alg_stat = "Cached";
	  
	  double start_time = Exec::ConvertTimerToSec(alg->start());
	  if(start_time==0.0) start_time = lastAlgt+1.0e-6;
	  else lastAlgt = Exec::ConvertTimerToSec(alg->start());

	  const std::string start_timeS = Anp::Round2Pair(start_time,0.0001).first;

	  Exec::AlgCall call = GetAlgCall(*alg,*seq,sevt);

	  const TrigConfSeq   *seq_conf = call.seq_conf.get();
	  const TrigConfChain *chn_conf = call.chn_conf.get();
	  const TrigConfAlg   *alg_conf = call.alg_conf.get();

	  if(!(seq_conf && chn_conf && alg_conf)){ 
	    cout << "ExecTableSlow::WriteXMLEvent  - ERROR unable to find the conf seq: " << seq_conf 
		 << "  chn: " << chn_conf 
		 << " alg: "  << alg_conf << endl
		 << "                              - chn counter: " << seq->getChnCounter() 
		 << "  seqIndex: "     << seq->getSeqIndex() 
		 << " alg RoiId: "     << alg->getRoiId()
		 << " alg start time:" << start_time 
		 << " alg status "     << alg_stat << endl;

	    return;
	  }

	  const string seq_name = seq_conf->getName();
	  std::string chn_name  = chn_conf->getName();
	  std::string alg_name  = alg_conf->getName();

	  // 
	  // Find Signature step 
	  // 
	  const uint32_t seq_id = seq_conf->getId(); 
	  const vector<TrigConfSig> signatures = chn_conf->getSignature(); 
	  
	  unsigned sig_step = 0; 
	  
	  for(vector<TrigConfSig>::const_iterator sig = signatures.begin(); sig != signatures.end(); ++sig){ 
	    ++sig_step; 
	    if(sig->matchOutputTE(seq_id)) break; 
	  }
	  //
	  // Find RoIs
	  //
	  const vector<Exec::ROILoad> &roiLoad = call.roi_data;

	  stringstream roi_id_str;
	  if(roiLoad.size() ==0) roi_id_str << "No ROI";
	  else if(roiLoad.size() >0){
	    for(unsigned roi = 0; roi < roiLoad.size();++roi){
	      if(roi==0) roi_id_str << roiLoad.at(roi).roi_id;
	      else roi_id_str << ", " << roiLoad.at(roi).roi_id;
	    }
	  }

	  if(sum.write_full && (GetLevel()==0 || GetLevel()==2) && (0 < call.rob_data.size())) {
	    //
	    // Save algorithms to write at the end of the event
	    //
	    fAlg.push_back(call);
	  }

	  os << " startTime='" << start_timeS                                  << "'"
	     << " chnName='"   << chn_name                                     << "'"
	     << " seqName='"   << sum.GetSeqName(unsigned(seq->getSeqIndex())) << "'"
	     << " algName='"   << alg_name                                     << "'"
	     << " sigStep='"   << sig_step                                     << "'"
	     << " roiId='"     << roi_id_str.str()                             << "'"
	     << " algStatus='" << alg_stat                                     << "'"
	     << " algTime='"   << alg->getTimer()                              << "'";
	  
	  if(GetLevel() == 0 || GetLevel() == 2) {
	    //
	    // Calculating ros and rob counts for the AlgCall
	    //
	    fSlowEvt.Clear();
	    ExecClient *algClient = GetAlgClient(*(call.alg_conf));
	    algClient->ProcessAlg(call);
	    algClient->FinishEvent(sevt.sEvt);
	    
	    const Exec::Count &count = algClient->GetCount();
	    
	    const uint64_t rob_size = count.rob_size_retrv+count.rob_size_cache+count.rob_size_other;
	    
	    const string rob_timer_str  = Anp::Round2Pair(count.timer_rob,         0.1).first;
	    const string size_total_str = Anp::Round2Pair(Exec::ConvertWordsToKB(rob_size),         1.0).first;
	    const string size_retrv_str = Anp::Round2Pair(Exec::ConvertWordsToKB(count.rob_size_retrv), 1.0).first;

	    os << " rob_nreq='"       << count.rob_nreq        << "'"  
	       << " rob_nreq_retrv='" << count.rob_nreq_retrv  << "'"
	       << " rob_nids='"       << count.rob_nids        << "'"  
	       << " rob_nids_retrv='" << count.rob_nids_retrv  << "'"
	       << " robTime='"        << rob_timer_str         << "'"  
	       << " robSizeReq='"     << size_total_str        << "'"  
	       << " robSizeRet='"     << size_retrv_str        << "'"  
	       << " rosCallsRet='"    << getROSCalls(call,sum) << "'";
	    
	  }

	  os << "/>" << endl;
	}
    }

    os << "  </AlgCalls>" << endl;

    //
    // ROB requests
    //
    os << "  <ROBCalls>" << endl;
    for(AlgVec::iterator alg = fAlg.begin(); alg!=fAlg.end();++alg){

      WriteXMLCall(anode,hmenu,*alg,sum);
    }
    os << "  </ROBCalls>" << endl;
    fAlg.clear();
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteAlgCall(HtmlNode &anode, HtmlMenu &/*hmenu*/, const Exec::AlgCall &alg, const Exec::Summary &/*sum*/)
{
  if(fDebug)  cout << "ExecTableSlow::WriteAlgCall - starting WriteAlgCall" << endl;
  //
  // Write all detail of the alg call
  // 
  ostream &os = anode.GetCont();

  std::string chn_link =  alg.chn_conf->getName();
  std::string alg_link =  alg.alg_conf->getName();
  std::string alg_type =  alg.alg_conf->getName();
  std::string seq_link =  alg.seq_conf->getName();

  const vector<Exec::ROILoad> &roiLoad = alg.roi_data;

  stringstream roi_id_str;
  if(roiLoad.size() ==0) roi_id_str << "No ROI";
  else if(roiLoad.size() >0){
    for(unsigned roi = 0; roi < roiLoad.size();++roi){
      if(roi==0) roi_id_str << roiLoad.at(roi).roi_id;
      else roi_id_str << ", " << roiLoad.at(roi).roi_id;
    }
  }

  //
  // Print Event, chain, seq, and ROI information
  //
  os << "Chain:    " << chn_link             << endl
     << "Sequence: " << seq_link             << endl
     << "Alg Name: " << alg_link             << endl
     << "Alg Type: " << alg_type             << endl
     << "#Roi:     " << alg.roi_data.size()  << endl
     << "Roi IDs:  " << roi_id_str.str()     << endl
     << "#Requests:" << alg.rob_data.size()  << endl
     << endl         << "</pre>";
  
  const vector<Exec::ROBCall> &robCalls = alg.rob_data;
  for(unsigned rob = 0; rob<robCalls.size(); ++rob){
    const Exec::ROBCall &robCall = robCalls.at(rob);
    //
    // Collect ROB summary statistics
    //
    const vector<Exec::ROBData> &robDatas = robCall.rob_data;
    float    words   = 0.0;
    float    kbs     = 0.0;
    unsigned nCached = 0;

    for(unsigned data = 0; data<robDatas.size();++data){
      
      const Exec::ROBData &robData = robDatas.at(data);
      if(robData.rob_data.isCached()) ++nCached;
      words += robData.rob_data.getROBSize();
      kbs   += Exec::ConvertWordsToKB(robData.rob_data.getROBSize());
    }
    string rob_stat = "N/A";
    if(robDatas.size()>0)          rob_stat = "retrieved";
    if(nCached == robDatas.size()) rob_stat = "cached";

    const string rob_time_str = Anp::Round2Pair(robCall.rob_timer, 0.1).first;
    const string tot_req_size = Anp::Round2Pair(kbs, 0.01).first;

    os << "<pre>"                                       << endl
       << "     Algorithm:           " << alg_link      << endl 
       << "     Data request Status: " << rob_stat      << endl
       << "     Request Time (ms):   " << rob_time_str  << endl
       << "     # Words requested:   " << words         << endl
       << "     Size requested (KB): " << tot_req_size  << endl
       << "</pre>";
 
    //
    // Writing Table of ROS requests for one ROB
    //
    os << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
       << "<tr>"
       << "<th> ROB ID </th>"
       << "<th> ROS Name </th>"
       << "<th> SubDetector Name </th>"
       << "<th> Request Status </th>"
       << "<th> Size Requested (KB) </th>"
       << "<th> Size Requested (words) </th>"
       << "</tr>" << endl;
    
    for(unsigned data = 0; data<robDatas.size();++data){
      
      const Exec::ROBData &robData = robDatas.at(data);
     
      const string req_size = Anp::Round2Pair(Exec::ConvertWordsToKB(robData.rob_data.getROBSize()), 0.01).first;
      
      os << "<tr>" 
	 << "<td align=\"left\"> "   << hex << robData.rob_conf->rob_id             << " </td>"
	 << "<td align=\"center\"> " << dec << robData.rob_conf->ros_name           << " </td>"
	 << "<td align=\"center\"> " << robData.rob_conf->det_name                  << " </td>"
	 << "<td align=\"center\"> " << Exec::ConvertHistToString(robData.rob_data) << " </td>"
	 << "<td align=\"center\"> " << req_size                                    << " </td>"
	 << "<td align=\"center\"> " << robData.rob_data.getROBSize()               << " </td>"
	 << "</tr>" << endl;
    }
    os << "</table>" << endl;
  } 
}


//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteTxtCall(HtmlNode &anode, HtmlMenu &/*hmenu*/, const Exec::AlgCall &alg, const Exec::Summary &/*sum*/)
{
  if(fDebug)  cout << "ExecTableSlow::WriteTxtCall - starting WriteTxtCall" << endl;
  //
  // Write all detail of the alg call
  // 
  ostream &os = anode.GetCont();
  
  const vector<Exec::ROBCall> &robCalls = alg.rob_data;
  for(unsigned rob = 0; rob<robCalls.size(); ++rob){
    const Exec::ROBCall &robCall = robCalls.at(rob);
    //
    // Collect ROB summary statistics
    //

    const vector<Exec::ROBData> &robDatas = robCall.rob_data;
    float    words   = 0.0;
    float    kbs     = 0.0;
    unsigned nCached = 0;

    for(unsigned data = 0; data<robDatas.size();++data){
      
      const Exec::ROBData &robData = robDatas.at(data);
      if(robData.rob_data.isCached()) ++nCached;
      words += robData.rob_data.getROBSize();
      kbs   += Exec::ConvertWordsToKB(robData.rob_data.getROBSize());
    }
    string rob_stat = "N/A   ";
    if(robDatas.size()>0)          rob_stat = "retrvd";
    if(nCached == robDatas.size()) rob_stat = "cached";

    const string rob_time_str = Anp::Round2Pair(robCall.rob_timer, 0.1).first;
    const string tot_req_size = Anp::Round2Pair(kbs, 0.01).first;
    os << "      DATA_REQUEST," 
       << " req_status,"
       << " req_timer,"   
       << " req_word,"   
       << " req_size" ;

    for(unsigned dat =0;dat<robDatas.size();++dat) os << ",ROB_ID:ros_name:det_name:status:size:words";

    os << endl; 
    
    os << "      DATA_REQUEST," 
       << rob_stat     << ","
       << rob_time_str << ","
       << words        << ","
       << tot_req_size;

    //
    // Writing ROS requests for one ROB
    //
    for(unsigned data = 0; data<robDatas.size();++data){
      
      const Exec::ROBData &robData = robDatas.at(data);
     
      const string req_size = Anp::Round2Pair(Exec::ConvertWordsToKB(robData.rob_data.getROBSize()), 0.01).first;
      
      os << ", ";

      os << hex << robData.rob_conf->rob_id             << ":"
	 << dec << robData.rob_conf->ros_name           << ":"
	 << robData.rob_conf->det_name                  << ":"
	 << Exec::ConvertHistToString(robData.rob_data) << ":"
	 << req_size                                    << ":"
	 << robData.rob_data.getROBSize();
    }
    os << endl;
  } 
}


//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteXMLCall(HtmlNode &anode, HtmlMenu &/*hmenu*/, const Exec::AlgCall &alg, const Exec::Summary &/*sum*/)
{
  if(fDebug)  cout << "ExecTableSlow::WriteXMLCall - starting WriteXMLCall" << endl;
  //
  // Write all detail of the alg call
  // 
  ostream &os = anode.GetCont();

  std::string chn_link =  alg.chn_conf->getName();
  std::string alg_link =  alg.alg_conf->getName();
  std::string alg_type =  alg.alg_conf->getName();
  std::string seq_link =  alg.seq_conf->getName();

  //
  // Need to match ROI to robcall
  //
  const vector<Exec::ROILoad> &roiLoad = alg.roi_data;

  stringstream roi_id_str;
  if(roiLoad.size() ==0) roi_id_str << "No ROI";
  else if(roiLoad.size() >0){
    for(unsigned roi = 0; roi < roiLoad.size();++roi){
      if(roi==0) roi_id_str << roiLoad.at(roi).roi_id;
      else roi_id_str << ", " << roiLoad.at(roi).roi_id;
    }
  }
  
  const vector<Exec::ROBCall> &robCalls = alg.rob_data;
  for(unsigned rob = 0; rob<robCalls.size(); ++rob){
    const Exec::ROBCall &robCall = robCalls.at(rob);
    //
    // Collect ROB summary statistics
    //

    os << "    <ROBCall"
       << " chnName='"  << chn_link             << "'"
       << " seqName='"  << seq_link             << "'"
       << " algName='"  << alg_link             << "'"
       << " algType='"  << alg_type             << "'"
       << " nRoi='"     << alg.roi_data.size()  << "'"
       << " roiIds='"   << roi_id_str.str()     << "'"
       << " nDataReq='" << alg.rob_data.size()  << "'";

    const vector<Exec::ROBData> &robDatas = robCall.rob_data;
    float    words   = 0.0;
    float    kbs     = 0.0;
    unsigned nCached = 0;

    for(unsigned data = 0; data<robDatas.size();++data){
      
      const Exec::ROBData &robData = robDatas.at(data);
      if(robData.rob_data.isCached()) ++nCached;
      words += robData.rob_data.getROBSize();
      kbs   += Exec::ConvertWordsToKB(robData.rob_data.getROBSize());
    }
    string rob_stat = "N/A";
    if(robDatas.size()>0)          rob_stat = "retrieved";
    if(nCached == robDatas.size()) rob_stat = "cached";

    const string rob_time_str = Anp::Round2Pair(robCall.rob_timer, 0.1).first;
    const string tot_req_size = Anp::Round2Pair(kbs, 0.01).first;

    os 
       << " callStatus='" << rob_stat   << "'"
       << " reqTime='"  << rob_time_str << "'"
       << " wordReq='"  << words        << "'"
       << " sizeReq='"  << tot_req_size << "'>" << endl;
 
    //
    // Writing ROS requests for one ROB
    //
    for(unsigned data = 0; data<robDatas.size();++data){
      
      const Exec::ROBData &robData = robDatas.at(data);
     
      const string req_size = Anp::Round2Pair(Exec::ConvertWordsToKB(robData.rob_data.getROBSize()), 0.01).first;
      
      os << "       <ROBData" 
	 << " chnName='"    << chn_link                                    << "'"
	 << " algName='"    << alg_link                                    << "'"
	 << " robId='"      << hex << robData.rob_conf->rob_id             << "'"
	 << " rosName='"    << dec << robData.rob_conf->ros_name           << "'"
	 << " detName='"    << robData.rob_conf->det_name                  << "'"
	 << " robReqStat='" << Exec::ConvertHistToString(robData.rob_data) << "'"
	 << " robReqSize='" << req_size                                    << "'"
	 << " robReqWord='" << robData.rob_data.getROBSize()               << "'"
	 << " roiIds='"     << roi_id_str.str()                            << "'"
	 << " />" << endl;
    }
    os << "    </ROBCall>" << endl;
  } 
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteChn(HtmlNode &anode, HtmlMenu &/*hmenu*/, const SlowEvt &sevt, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteChn - starting WriteChn" << endl;
  //
  // Write execution summary for single chain
  //
  ostream &os = anode.GetCont();

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Chain </th>"
     << "<th> Alg Calls </th>"
     << "<th> Alg Caches </th>"
     << "<th> Total Chn Time (ms) </th>"
     << "<th> Total Chn Time (%) </th>";

  if(GetLevel() == 0 || GetLevel() == 2) {
    os << "<th> ROS requests </th>"
       << "<th> ROS retrievals </th>"
       << "<th> ROS request size (KB) </th>"
       << "<th> ROS retrieval size (KB) </th>";
  }

  os << "</tr>" << endl;
  
  ClientVec chns;

  for(ExecMap::const_iterator it = sevt.sChn.begin(); it != sevt.sChn.end(); ++it) {
    chns.push_back(it->second.get());
  }

  //
  // Sort chains by total execution time
  //
  std::sort(chns.begin(), chns.end(), SortExecClient("timer_alg"));

  for(unsigned i = 0; i < chns.size(); ++i) {
    ExecClient *chn = chns.at(i);    

    const Exec::Count &count = chn->GetCount();

    //
    // Find web page for current algorithm
    //
    string chn_link = chn->GetClientName();

    //if(sum.write_full) {
    //  //
    //  // Insert link to main algorithm page
    //  //
    //  Handle<HtmlNode> nodeP = hmenu.FindChnNode(chn->GetClientName());
    //  if(nodeP.valid()) {
    //	chn_link = nodeP->GetLink(chn->GetClientName());
    //  }
    //}

    //
    // Compute helper variables
    //
    double time_frac = 0.0;
    if(sum.total_time > 0.0) {
      time_frac = 100.0*count.timer_alg/sevt.sEvt.timer_alg;
    }

    const string time_totl_str = Anp::Round2Pair(count.timer_alg, 1.0).first;
    const string time_frac_str = Anp::Round2Pair(time_frac, 0.1).first;

    //
    // Write HTML table entries
    //
    os << "<tr>" 
       << "<td align=\"left\"> "   << chn_link                     << " </td>"
       << "<td align=\"center\"> " << count.alg_ncalls             << " </td>"
       << "<td align=\"center\"> " << count.alg_ncache             << " </td>"
       << "<td align=\"center\"> " << time_totl_str                << " </td>"
       << "<td align=\"center\"> " << time_frac_str                << " </td>";

    if(GetLevel() == 0 || GetLevel() == 2) {
      const uint64_t rob_size = count.rob_size_retrv+count.rob_size_cache+count.rob_size_other;

      const string size_total_str = Anp::Round2Pair(Exec::ConvertWordsToKB(rob_size),         1.0).first;
      const string size_retrv_str = Anp::Round2Pair(Exec::ConvertWordsToKB(count.rob_size_retrv), 1.0).first;

      os << "<td align=\"center\"> " << count.ros_nreq       << " </td>"
	 << "<td align=\"center\"> " << count.ros_nreq_retrv << " </td>"
	 << "<td align=\"center\"> " << size_total_str       << " </td>"
	 << "<td align=\"center\"> " << size_retrv_str       << " </td>";
    }

    os << "</tr>" << endl;
  }
  
  os << "</table>" << endl;


}

//--------------------------------------------------------------------------------------      
  void Anp::ExecTableSlow::WriteAlg(HtmlNode &anode, HtmlMenu &/*hmenu*/, const SlowEvt &sevt, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteAlg - starting WriteAlg" << endl;
  //
  // Write execution table for all algorithms
  //
  ostream &os = anode.GetCont();

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Algorithm </th>"
     << "<th> Calls </th>"
     << "<th> Cached Calls </th>"
     << "<th> Total alg time (ms) </th>"
     << "<th> Total alg time (%) </th>";
  
  if(GetLevel() == 0 || GetLevel() == 2) {
    os << "<th> ROS request rate (Hz)</th>"
       << "<th> ROS retrieval rate (Hz)</th>";
  }

  os << "</tr>" << endl;
  
  ClientVec algs;
  
  for(ExecMap::const_iterator it = sevt.sAlg.begin(); it != sevt.sAlg.end(); ++it) {
    algs.push_back(it->second.get());
  }

  //
  // Sort algorithms by total execution time
  //
  std::sort(algs.begin(), algs.end(), SortExecClient("timer_alg"));

  for(unsigned i = 0; i < algs.size(); ++i) {
    ExecClient *alg = algs.at(i);    

    const Exec::Count &count = alg->GetCount();

    //
    // Make web page for current algorithm if full summary is requested
    //
    string alg_link = alg->GetClientName();

    //if(sum.write_full) {
    //  //
    //  // Insert link to main algorithm page - BROKEN because of new LB directory structure
    //  //
    //  Handle<HtmlNode> nodeP = hmenu.FindAlgName(alg->GetClientName());
    //  if(nodeP.valid()) {
    //	alg_link = nodeP->GetLink(alg->GetClientName());
    //  }
    //}

    //
    // Compute helper variables
    //
    double time_frac = 0.0;
    if(sum.total_time > 0.0) {
      time_frac = 100.0*count.timer_alg/sevt.sEvt.timer_alg;
    }
    
    const string time_totl_str  = Anp::Round2Pair(count.timer_alg, 0.1).first;
    const string time_frac_str  = Anp::Round2Pair(time_frac, 0.1).first;

    //
    // Write HTML table entries
    //
    os << "<tr>" 
       << "<td align=\"left\" style=\"word-wrap:break-word\">"   
       << alg_link                                            << " </td>"
       << "<td align=\"center\"> " << count.alg_ncalls        << " </td>"
       << "<td align=\"center\"> " << count.alg_ncache        << " </td>"
       << "<td align=\"center\"> " << time_totl_str           << " </td>"
       << "<td align=\"center\"> " << time_frac_str           << " </td>";

    if(GetLevel() == 0 || GetLevel() == 2) {

      os << "<td align=\"center\"> " << count.ros_nreq       << " </td>"
	 << "<td align=\"center\"> " << count.ros_nreq_retrv << " </td>";
    }

    os << "</tr>" << endl;
  }
  
  os << "</table>" << endl;

}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteDet(HtmlNode &anode, HtmlMenu &/*hmenu*/, const SlowEvt &sevt, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteDet - starting WriteDet" << endl;
  //
  // Write a summary of requests to each subdetector
  //
  ostream &os = anode.GetCont();
  if(sevt.totEvt.size()==0) return;
  const Exec::Count &c = sevt.totEvt.at(0)->GetCount();
  //
  // Write event summary
  //

  anode.AddAnchor("Summary of Subdetector calls for entire event:","subdetcalls",true);

  os << "<pre>" << endl
     << "Active algorithm calls:         " << c.alg_ncalls                              << endl
     << "ROB DataCollector calls:        " << c.rob_nreq                                << endl
     << "  - retrieved:                  " << c.rob_nreq_retrv                          << endl
     << "  - cached:                     " << c.rob_nreq_cache                          << endl
     << "ROB calls:                      " << c.rob_nids                                << endl
     << "  - retrieved:                  " << c.rob_nids_retrv                          << endl
     << "  - cached:                     " << c.rob_nids_cache                          << endl
     << "</pre>"
     << endl;

  //
  // Write subdetector summary
  //
  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Sub-detector </th>"
     << "<th> Data requests </th>"
     << "<th> Retrieved data requests </th>"
     << "<th> ROB requests </th>"
     << "<th> Retrieved ROB requests </th>"
     << "<th> Requested data (kB) </th>"
     << "<th> Retrieved data (kB) </th>"
     << "<th> Cached data (kB) </th>"
     << "</tr>" 
     << endl;

  const std::map<uint32_t, Exec::Count> &det_map = sevt.totEvt.at(0)->GetDetCount();  
  
  double total_req = 0.0, total_ret = 0.0, total_chd = 0.0;

  for(map<uint32_t, Exec::Count>::const_iterator dit = det_map.begin(); dit != det_map.end(); ++dit) {
    const uint32_t det_id = dit->first;
    const Exec::Count &dc = dit->second;
    
    const string detname = sum.GetDetName(det_id);
    const string str_req = Anp::Round2Pair(Exec::ConvertWordsToKB(dc.rob_size), 1.0).first;
    const string str_ret = Anp::Round2Pair(Exec::ConvertWordsToKB(dc.rob_size_retrv), 1.0).first;
    const string str_chd = Anp::Round2Pair(Exec::ConvertWordsToKB(dc.rob_size_cache), 1.0).first;
    
    os << "<tr>" 
       << "<td align=\"left\">"  << detname           << "</td>"
       << "<td align=\"right\">" << dc.rob_nreq       << "</td>"
       << "<td align=\"right\">" << dc.rob_nreq_retrv << "</td>"
       << "<td align=\"right\">" << dc.rob_nids       << "</td>"
       << "<td align=\"right\">" << dc.rob_nids_retrv << "</td>"
       << "<td align=\"right\">" << str_req           << "</td>"
       << "<td align=\"right\">" << str_ret           << "</td>"
       << "<td align=\"right\">" << str_chd           << "</td>"
       << endl;

    total_req += dc.rob_size;
    total_ret += dc.rob_size_retrv;
    total_chd += dc.rob_size_cache;    
  }
  
  const string tot_req = Anp::Round2Pair(Exec::ConvertWordsToKB(total_req), 1.0).first;
  const string tot_ret = Anp::Round2Pair(Exec::ConvertWordsToKB(total_ret), 1.0).first;
  const string tot_chd = Anp::Round2Pair(Exec::ConvertWordsToKB(total_chd), 1.0).first;

  os << "<tr>" 
     << "<td align=\"left\">Total</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">" << tot_req    << "</td>"
     << "<td align=\"right\">" << tot_ret    << "</td>"
     << "<td align=\"right\">" << tot_chd    << "</td>"
     << "</table>"
     << endl;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableSlow::WriteROS(HtmlNode &anode, HtmlMenu &/*hmenu*/, const SlowEvt &sevt, const Exec::Summary &sum)
{
  if(fDebug)  cout << "ExecTableSlow::WriteROS - starting WriteROS" << endl;
  //
  // Write a summary of requests to each ROS
  //
  ostream &os = anode.GetCont();

  anode.AddAnchor("Table shows summary for data requests and retrievals by ROS for the entire event:","roscalls",true);

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> ROS </th>"
     << "<th> Data Requests </th>"
     << "<th> Data Retrievals </th>"
     << "<th> ROB Requests </th>"
     << "<th> ROB Retrievals </th>"
     << "<th> Retrieved data (Kb)</th>"
     << "<th> Time per ROS retrieval (ms) </th>"
     << "<th> Total Time (ms) </th>"
     << "<th> ROB/request </th>"
     << "</tr>" << endl;
  
  ClientVec roses;

  for(ExecMap::const_iterator it = sevt.sROS.begin(); it != sevt.sROS.end(); ++it) {
    roses.push_back(it->second.get());
  }

  //
  // Sort algorithms by total execution time
  //
  std::sort(roses.begin(), roses.end(), SortExecClient("rob_nreq_retrv"));

  for(unsigned i = 0; i < roses.size(); ++i) {
    ExecClient *ros = roses.at(i);
    
    const Exec::Count &count = ros->GetCount();
    const string &ros_name = ros->GetClientName();

    //
    // Create web page for current ROS
    //
    string ros_link = ros_name;

    if(sum.write_full) {      

    double rob_per_req   = 0.0;

    if(count.rob_nreq > 0) {
      rob_per_req = count.rob_nids/double(count.rob_nreq);
    }

    const string rob_per_req_str   = Anp::Round2Pair(rob_per_req,   1.0).first;

    const double size_ret_dbl = Exec::ConvertWordsToKB(count.rob_size_retrv);
    const string size_ret_str = Anp::Round2Pair(size_ret_dbl, 1.0).first;        

    //
    // Write HTML table entries
    //
    os << "<tr>" 
       << "<td align=\"left\"> "   << ros_link                                    << " </td>"
       << "<td align=\"center\"> " << count.rob_nreq                              << " </td>"
       << "<td align=\"center\"> " << count.rob_nreq_retrv                        << " </td>"
       << "<td align=\"center\"> " << count.rob_nids                              << " </td>"
       << "<td align=\"center\"> " << count.rob_nids_retrv                        << " </td>"
       << "<td align=\"center\"> " << size_ret_str                                << " </td>"
       << "<td align=\"center\"> " << count.GetMeanROBTimePerRetr()               << " </td>"
       << "<td align=\"center\"> " << count.GetMeanROBTimePerEvent()              << " </td>"
       << "<td align=\"center\"> " << rob_per_req_str                             << " </td>"
       << "</tr>" << endl;
    }
  }
  
  os << "</table>" << endl;
}

//---------------------------------------------------------------------------------------
Anp::Exec::AlgCall Anp::ExecTableSlow::GetAlgCall(const TrigMonAlg &alg,const TrigMonSeq &seq, const Anp::ExecTableSlow::SlowEvt &sevt)
{
  if(fDebug)  cout << "ExecTableSlow::GetAlgCall - starting GetAlgCall" << endl;
  const vector<Exec::AlgCall> calls = sevt.AlgCalls;

  Exec::AlgCall theCall;

  unsigned counter = seq.getChnCounter();
  unsigned index   = seq.getSeqIndex();

  for(vector<Exec::AlgCall>::const_iterator call = calls.begin();call!=calls.end();++call){
    if(counter == call->chn_conf->getCounter() && index == call->seq_conf->getIndex()){
      //
      // Check if alg time matches
      //
      bool position(alg.getPosition() == call->alg_data.getPosition());
      bool time(alg.start() == call->alg_data.start());
      bool roiId(alg.getRoiId() == call->alg_data.getRoiId());
      if(position && time && roiId) theCall = *call;
    }
  }

  return theCall;  
}

//-----------------------------------------------------------------------------
void Anp::ExecTableSlow::resetCachedSeq(vector<TrigMonSeq> &seq_list, unsigned &seq_pos, TrigMonSeq currSeq, const Exec::Summary &sum)
{
  //
  // Adding all dependent seq's
  //  - add back a chain kinitial
  //  - add chain for each dependent seq
  //
  //
  // Add a  kinitial for the current seq
  //
  vector<TrigMonSeq>::iterator riter = seq_list.begin();
  if(seq_pos>=seq_list.size()) return;
  for(unsigned i=0;i<seq_pos;++i) ++riter;
  cout << "ExecTableSlow::resetCachedSeq - starting" << endl;
  const vector<uint32_t> depSeq = getDepSeqId(unsigned(riter->getSeqIndex()),sum);
  for(unsigned id = 0;id <depSeq.size();++id){
      uint32_t       initial_encoded  = static_cast<uint32_t>(currSeq.getChnEncodedId());
      const uint32_t initial_index    = static_cast<uint32_t>(currSeq.getSeqIndex());
      if(initial_index >= 4096) {
	cerr << "ExecTableSlow::RemoveChain - TrigMonSeq ctor error! Index is out of range: " << initial_index << endl;
	continue;
      }
      else {
	initial_encoded |= (initial_index << SEQBits::shiftIndex);
      }

      //
      // Check to see if this seq is already present in vector
      //
      vector<TrigMonSeq>::iterator testIt = riter;
      bool isSeqCalled = false;
      --testIt;
      while(testIt != seq_list.end()){
	if(testIt->getSeqIndex() == riter->getSeqIndex() && testIt->getChnEncodedId() == riter->getChnEncodedId()){
	  isSeqCalled = true;
	  break;
	}
	--testIt;
      }
      if(isSeqCalled) continue;
      
      TrigMonSeq initialSeq(initial_encoded);
      initialSeq.addState(TrigMonSeq::kInitial);
      cout << "ExecTableSlow::resetCachedSeq - insert initialize" << endl;
      seq_list.insert(riter,initialSeq);
      ++riter;
      ++seq_pos;
      if(riter == seq_list.end()){
	cout << "ExecTableSlow::resetCachedSeq - invalid iterator" << endl;
	break;
      }
      cout << "ExecTableSlow::resetCachedSeq - insert and increment" << endl;
      //
      // Add kPrevious to the current
      //
      uint32_t prev_encoded = static_cast<uint32_t>(currSeq.getChnEncodedId());
      SeqConfMap::iterator confSeqit = fSeqConfMap.find(depSeq.at(id));
      uint32_t prev_index   = 5000;
      if(confSeqit !=fSeqConfMap.end()) prev_index = unsigned(confSeqit->second->getIndex());

      if(prev_index >= 4096) {
	cerr << "ExecTableSlow::RemoveChain - TrigMonSeq ctor error! Index is out of range: " << prev_index << endl;
	break;
      }
      else {
	prev_encoded |= (prev_index << SEQBits::shiftIndex);
      }
      TrigMonSeq prevSeq(prev_encoded);
      prevSeq.addState(TrigMonSeq::kInitial);
      prevSeq.addState(TrigMonSeq::kPrevious);
      prevSeq.addState(TrigMonSeq::kAlreadyExecuted);
      cout << "ExecTableSlow::resetCachedSeq - insert previous seq" << endl;
      seq_list.insert(riter,prevSeq);
      //
      // Ask if the new sequence has any uncalled dependencies
      //
      cout << "ExecTableSlow::resetCachedSeq - get next seq" << endl;
      resetCachedSeq(seq_list,seq_pos,currSeq,sum);
      ++riter;
      ++seq_pos;
      if(riter == seq_list.end()){
	cout << "ExecTableSlow::resetCachedSeq - invalid iterator" << endl;
	break;
      }
  }
}

//-----------------------------------------------------------------------------
std::string Anp::ExecTableSlow::getROSCalls(const Exec::AlgCall &alg,const Exec::Summary &sum) const
{
  if(fDebug)  cout << "ExecTableSlow::getROSCalls - starting getROSCalls" << endl;
  std::map<unsigned,unsigned> roses;
  const vector<Exec::ROBCall> &calls = alg.rob_data;
  set<unsigned> ros_req;

  for(vector<Exec::ROBCall>::const_iterator call = calls.begin();call!=calls.end();++call){

    const vector<Exec::ROBData> datas = call->rob_data;
    //
    // fill if ros id is found in data request
    //
    for(vector<Exec::ROBData>::const_iterator data = datas.begin();data!=datas.end();++data){

      if(data->rob_data.isRetrieved()) ros_req.insert(data->ros_id);
    }
    //
    // Fill map of ros requests to ros id
    //
    for(set<unsigned>::iterator rosit = ros_req.begin(); rosit != ros_req.end();++rosit){
      std::map<unsigned,unsigned>::iterator ros = roses.find(*rosit);	
      if(ros==roses.end()) ros = roses.insert(std::map<unsigned,unsigned>::value_type(*rosit,0)).first;
      ++(ros->second);
    }
    ros_req.clear();
  }

  stringstream req;

  for(std::map<unsigned,unsigned>::iterator ros = roses.begin();ros!=roses.end();++ros){
    req << ros->second << "x" << sum.GetROSName(ros->first);
    ++ros;
    if(ros !=roses.end()) req << ", ";
    --ros;
  }
  return req.str();

}

//-----------------------------------------------------------------------------
vector<uint32_t> Anp::ExecTableSlow::getDepSeqId(const unsigned index, const Exec::Summary &sum) const
{
  if(!sum.seq_conf){
    cout << "ERROR invalid sequence conf" << endl;
    vector<uint32_t> dummy;
    return dummy;
  }
  if(sum.seq_conf->end()!= sum.seq_conf->find(index)){
    Ptr<TrigConfSeq> seq_conf = sum.seq_conf->find(index)->second;
    return seq_conf->getInputTEs(); //get TE Id
    }
  vector<uint32_t> dummy;
  return dummy;
}

//---------------------------------------------------------------------------------------
bool Anp::SortExecSlow::operator()(const Anp::ExecTableSlow::SlowEvt &lhs, const Anp::ExecTableSlow::SlowEvt &rhs) const
{
  //
  // Compare two objects based on key
  //
  if(fKey == "timer_alg") {
    return lhs.sEvt.timer_alg > rhs.sEvt.timer_alg;
  }
  else if(fKey == "timer_rob") {
    return lhs.sEvt.timer_rob > rhs.sEvt.timer_rob;
  }
  else if(fKey == "timer_steer") {
    return lhs.sEvt.timer_steer > rhs.sEvt.timer_steer;
  }

  //
  // Compare by total time
  //
  return lhs.sEvt.timer_tcost > rhs.sEvt.timer_tcost;
}
