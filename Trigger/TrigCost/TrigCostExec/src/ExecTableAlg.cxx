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
#include "TrigCostExec/ExecTableAlg.h"
#include "TrigCostExec/ExecWrite.h"
#include "TrigCostExec/Util.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableAlg)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecTableAlg::ExecTableAlg()
  :fDir(0),
   fCanvas(0),
   fDebug(false),
   fFillNode(false),
   fMakeHist(false),
   fTimeout(0),
   fDirName()
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableAlg::~ExecTableAlg()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableAlg::SetReg(const Registry &reg)
{
  //
  // Copy configuration
  //
  fReg = reg;
  
  //
  // Read configuration
  //
  reg.Get("Debug",       fDebug);
  reg.Get("Level",       fLevel);
  reg.Get("Timeout",     fTimeout);
  reg.Get("HistOpt",     fHistOpt);
  reg.Get("MakeHist",    fMakeHist);
  reg.Get("ShortClient", fShortClient);
  reg.Get("FillNode",    fFillNode);
  reg.Get("ClientLink",  fClientLink);
  reg.Get("DirName",     fDirName);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableAlg::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableAlg::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Count one algorithm execution
  //
  ExecClient *client = GetAlgClient(*alg.alg_conf);
  client->ProcessAlg(alg);

  fActiveClients.push_back(client);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableAlg::FinishEvent(const Exec::Event &event)
{
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
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::ExecTableAlg::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------      
Anp::ExecClient* Anp::ExecTableAlg::GetAlgClient(const TrigConfAlg &alg_conf)
{
  //
  // Make client for algorithm
  //
  AlgMap::iterator ialg = fAlg.find(alg_conf.getNameId());
  if(ialg == fAlg.end()) {

    Registry reg(fReg);
    reg.Set("DoAlg", false);

    //
    // Only create det and ros maps when this is a full summary
    //
    if(!fShortClient){
      reg.Set("DoDet", true);
      reg.Set("DoROS", true);
      reg.Set("DoChn", true);
    }

    reg.Set("AlgId", alg_conf.getNameId());

    if(fDebug) {
      cout << "ExecTableAlg::GetAlgClient - " << alg_conf.getName() << endl;
    }

    stringstream link_str;
    link_str << "ALG_NAME_" << setw(10) << setfill('0') << alg_conf.getNameId();

    Handle<ExecClient> client(new ExecCount());
    ialg = fAlg.insert(AlgMap::value_type(alg_conf.getNameId(), client)).first;
    
    client->SetClientName(alg_conf.getName());
    client->SetClientLink(link_str.str());
    client->SetClientType("algorithm");
    client->SetReg(reg);
    client->SetDir(Anp::GetDir(fDir,alg_conf.getName()));
  }    

  return ialg->second.get();
}

//-------------------------------------------------------------------------------------- 
void Anp::ExecTableAlg::WriteExec(HtmlNode &anode, HtmlMenu &hmenu, const Exec::Summary &sum)
{
  //
  // Write out results
  //
  ostream &os = anode.GetCont();

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Algorithm </th>"
     << "<th> Events </th>"
     << "<th> Calls </th>"
     << "<th> Calls > " << Anp::Round2Pair(fTimeout, 10.0).first << " ms </th>"
     << "<th> Event rate (Hz)</th>"
     << "<th> Call rate (Hz)</th>"
     << "<th> Alg time/call (ms) </th>"
     << "<th> Alg time/event (ms) </th>"
     << "<th> Total alg time (s) </th>"
     << "<th> Total alg time (%) </th>";
  
  if(GetLevel() == 0 || GetLevel() == 2) {
    os << "<th> ROS request rate (Hz)</th>"
       << "<th> ROS retrieval rate (Hz)</th>";
  }

  os << "</tr>" << endl;
  
  const double sc = sum.scale_corr;
  ClientVec algs;
  
  for(AlgMap::iterator it = fAlg.begin(); it != fAlg.end(); ++it) {
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

    if(sum.write_full) {
      Handle<HtmlNode> nodeA(new HtmlNode(sum.html_base+"_"+alg->GetClientLink()+"_alg.html", HtmlNode::kHTML));
      anode.AddChild(nodeA);
      nodeA->SetBase(sum.html_base+"_"+alg->GetClientLink());
      
      nodeA->GetCont() << "<pre>" << endl;
      
      //
      // Insert link to main algorithm page 
      //
      Handle<HtmlNode> nodeP = hmenu.FindAlgName(alg->GetClientName());
      if(nodeP.valid()) {
	nodeA->GetCont() << "Algorithm: " << nodeP->GetLink(alg->GetClientName(),"../") << endl << endl;
      }
      else {
	nodeA->GetCont() << "Algorithm: " << alg->GetClientName() << endl << endl;
      }
      
      WriteAlg(nodeA.ref(), *alg, sum);
      nodeA->GetCont() << "</pre>" << endl;

      alg_link = nodeA->GetLink(alg->GetClientName());
    }

    //
    // Compute helper variables
    //
    double time_frac = 0.0;
    if(sum.total_time > 0.0) {
      time_frac = 100.0*count.timer_alg/sum.total_time;
    }
    
    const string time_robs_str  = Anp::Round2Pair(count.timer_rob/1000.0, 1.0).first;
    const string time_totl_str  = Anp::Round2Pair(count.timer_alg/1000.0, 1.0).first;
    const string time_frac_str  = Anp::Round2Pair(time_frac, 0.1).first;

    const string time_call_str  = Anp::Round2Pair(count.GetMeanAlgTimePerCall(),  0.1).first;
    const string time_evnt_str  = Anp::Round2Pair(count.GetMeanAlgTimePerEvent(), 0.1).first;

    const string rate_ncall_str = Anp::Round2Pair(count.alg_ncalls*sc, 10.0).first;
    const string rate_nevnt_str = Anp::Round2Pair(count.nevent*sc,     10.0).first;

    //
    // Write HTML table entries
    //
    os << "<tr>" 
       << "<td align=\"left\" style=\"word-wrap:break-word\">"   
       << alg_link                                            << " </td>"
       << "<td align=\"center\"> " << count.nevent            << " </td>"
       << "<td align=\"center\"> " << count.alg_ncalls        << " </td>"
       << "<td align=\"center\"> " << count.alg_nslow         << " </td>"
       << "<td align=\"center\"> " << rate_nevnt_str          << " </td>"
       << "<td align=\"center\"> " << rate_ncall_str          << " </td>"
       << "<td align=\"center\"> " << time_call_str           << " </td>"
       << "<td align=\"center\"> " << time_evnt_str           << " </td>"
       << "<td align=\"center\"> " << time_totl_str           << " </td>"
       << "<td align=\"center\"> " << time_frac_str           << " </td>";

    if(GetLevel() == 0 || GetLevel() == 2) {

      const string rate_nreq_str = Anp::Round2Pair(count.ros_nreq*sc,       10.0).first;
      const string rate_nret_str = Anp::Round2Pair(count.ros_nreq_retrv*sc, 10.0).first;

      os << "<td align=\"center\"> " << rate_nreq_str << " </td>"
	 << "<td align=\"center\"> " << rate_nret_str << " </td>";
    }

    os << "</tr>" << endl;
  }
  
  os << "</table>" << endl;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableAlg::WriteAlg(HtmlNode &anode, const ExecClient &alg, const Exec::Summary &sum)
{
  //
  // Write execution summary for single algorithm
  //
  ostream &os = anode.GetCont();
  
  const Exec::Count &c = alg.GetCount();
  
  const string evt_time_str = Anp::Round2Pair(c.GetMeanAlgTimePerEvent(), 0.1).first;

  os << "Summary for processed events: "                     <<          endl
     << "  Processing time:                " << c.timer_alg  << " ms" << endl
     << "  Processed events:               " << c.nevent     <<          endl
     << "  Processing time per event:      " << evt_time_str << " ms" << endl
     << "  Active algorithm calls:         " << c.alg_ncalls <<          endl
     << "  Cached algorithm calls:         " << c.alg_ncache <<          endl;
  
  if(GetLevel() == 0 || GetLevel() == 2) {
    
    const uint64_t rob_size = c.rob_size_retrv+c.rob_size_cache+c.rob_size_other;

    const string size_total_str = Anp::Round2Pair(Exec::ConvertWordsToKB(rob_size),         1.0).first;
    const string size_retrv_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size_retrv), 1.0).first;
    const string size_cache_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size_cache), 1.0).first;
    const string size_other_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size_other), 1.0).first;
    const string ostr = " (ignored, disabled or unclassified)\n";

    os << "  ROB DataCollector calls:        " << c.rob_nreq << endl
       << "    - retrieved:                  " << c.rob_nreq_retrv << endl
       << "    - cached:                     " << c.rob_nreq_cache << endl
       << "  Size of requested ROBs:         " << size_total_str   << " KB" << endl
       << "    - retrieved:                  " << size_retrv_str   << " KB" << endl
       << "    - cached:                     " << size_cache_str   << " KB" << endl
       << "    - other:                      " << size_other_str   << " KB" << ostr
       << "  Number of requested ROSs:       " << c.ros_nreq       << endl
       << "    - retrieved:                  " << c.ros_nreq_retrv << endl
       << "    - cached:                     " << c.ros_nreq_cache << endl
       << "    - other:                      " << c.ros_nreq_other << ostr
       << endl;
    
    //
    // Write HTML page with sub-detector requests
    //
    if(c.rob_nreq > 0) {
      Handle<HtmlNode> dnode(new HtmlNode(sum.html_base+"_"+alg.GetClientLink()+"_det.html", HtmlNode::kHTML));
      anode.AddChild(dnode);
      anode.GetCont() << dnode->GetLink("Data requests summary per sub-detector") << endl;
      
      dnode->GetCont() << "<pre>";
      dnode->AddLink("top ", "../", false);
      dnode->AddLink("index", "index.html", false);
      
      dnode->GetCont() << endl << endl << anode.GetLink(alg.GetClientName())
		       << " requested data from " << alg.GetDetCount().size() << " sub-detector(s): " 
		       << endl << endl;
      
      WriteDet(dnode.ref(), alg, sum);
      
      dnode->GetCont() << "</pre>" << endl;
      
      //
      // Write HTML page with ROS requests
      //
      Handle<HtmlNode> rnode(new HtmlNode(sum.html_base+"_"+alg.GetClientLink()+"_ros.html", HtmlNode::kHTML));
      anode.AddChild(rnode);
      anode.GetCont() << rnode->GetLink("Data requests summary per ROS") << endl;
      
      rnode->GetCont() << "<pre>";
      rnode->AddLink("top ", "../", false);
      rnode->AddLink("index", "index.html", false);
      
      rnode->GetCont() << endl << endl << anode.GetLink(alg.GetClientName())
		       << " requested data from " << alg.GetROSCount().size() << " ROS(s): " 
		       << endl << endl;
      
      WriteROS(rnode.ref(), alg, sum);

      rnode->GetCont() << "</pre>" << endl;
    }

    //
    // Write Chn request summary
    //
    Handle<HtmlNode> cnode(new HtmlNode(sum.html_base+"_"+alg.GetClientLink()+"_chn.html", HtmlNode::kHTML));
    anode.AddChild(cnode);
    anode.GetCont() << cnode->GetLink("Chains requesting algorithm") << endl;
    
    cnode->GetCont() << "<pre>";
    cnode->AddLink("top ", "../", false);
    cnode->AddLink("index", "index.html", false);
    
    cnode->GetCont() << endl << endl << anode.GetLink(alg.GetClientName())
		     << " was requested by " << alg.GetChnCount().size() << " Chain(s): " 
		     << endl << endl;
    
    WriteChn(cnode.ref(), alg, sum);

    cnode->GetCont() << "</pre>" << endl;

  }

  //
  // Print histograms if requested
  //
  if(!fMakeHist) {
    return;
  }

  string base = anode.GetBase()+"_";
  string suff;

  if     (fHistOpt.find("png") != string::npos) suff = ".png";
  else if(fHistOpt.find("eps") != string::npos) suff = ".eps";
  
  Exec::AlgHist ahc = alg.GetAlgHistCalls();
  Exec::ROBHist rhc = alg.GetROBHistCalls();  

  //
  // Make plots for histogram filled per algorithm execution
  //  
  if(fCanvas) {
    fCanvas->cd();
  }

  os << endl << "Histograms filled per algorithm execution:" << endl; 
  
  Anp::PrintHist(anode, ahc.hAlgTime, fCanvas, base+"calls_alg_time"+suff,
	    "  Algorithm total time", fHistOpt);

  if(GetLevel() == 0 || GetLevel() == 2) {
    Anp::PrintHist(anode, ahc.hCpuTime, fCanvas, base+"calls_cpu_time"+suff,
		   "  Algorithm CPU time", fHistOpt);
    Anp::PrintHist(anode, ahc.hROBTime, fCanvas, base+"calls_rob_time"+suff,
		   "  Algorithm ROB time", fHistOpt);

    Anp::PrintHist(anode, rhc.hROBNreq, fCanvas, base+"calls_rob_nreq"+suff,
		   "  Number of ROB data requests", fHistOpt);
    Anp::PrintHist(anode, rhc.hROBNreqRetrv, fCanvas, base+"calls_rob_nret"+suff,
		   "  Number of ROB data retrievals", fHistOpt);

    Anp::PrintHist(anode, rhc.hROBSize, fCanvas, base+"calls_rob_szrq"+suff,
		   "  Size of ROB data requests", fHistOpt);
    Anp::PrintHist(anode, rhc.hROBSizeRetrv, fCanvas, base+"calls_rob_szrt"+suff,
		   "  Size of ROB data retrievals", fHistOpt);
  }

  //
  // Make plots for histogram filled per event
  //  
  os << endl << "Histograms filled per event:" << endl; 

  Exec::AlgHist ahe = alg.GetAlgHistEvent();
  Exec::ROBHist rhe = alg.GetROBHistEvent();

  Anp::PrintHist(anode, ahe.hAlgTime, fCanvas, base+"event_alg_time"+suff,
		 "  Algorithm total time", fHistOpt);
  
  Anp::PrintHist(anode, ahe.hAlgTimeFrac, fCanvas, base+"evtfr_alg_time"+suff,
		 "  Algorithm total time/Event total time", fHistOpt);

  Anp::PrintHist(anode, ahe.hAlgTimeFirstCall, fCanvas, base+"event_first_alg_time"+suff,
		 "  Algorithm Time for First Call", fHistOpt);

  if(GetLevel() == 0 || GetLevel() == 2) {
    Anp::PrintHist(anode, ahe.hCpuTime, fCanvas, base+"event_cpu_time"+suff,
		   "  Algorithm CPU time", fHistOpt);
    Anp::PrintHist(anode, ahe.hROBTime, fCanvas, base+"event_rob_time"+suff,
		   "  Algorithm ROB time", fHistOpt);

    Anp::PrintHist(anode, rhe.hROBNreq, fCanvas, base+"event_rob_nreq"+suff,
		   "  Number of ROB data requests", fHistOpt);
    Anp::PrintHist(anode, rhe.hROBNreqRetrv, fCanvas, base+"event_rob_nret"+suff,
		   "  Number of ROB data retrievals", fHistOpt);

    Anp::PrintHist(anode, rhe.hROBSize, fCanvas, base+"event_rob_szrq"+suff,
		   "  Size of ROB data requests", fHistOpt);
    Anp::PrintHist(anode, rhe.hROBSizeRetrv, fCanvas, base+"event_rob_szrt"+suff,
		   "  Size of ROB data retrievals", fHistOpt);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableAlg::WriteDet(HtmlNode &dnode, const ExecClient &alg, const Exec::Summary &sum)
{
  //
  // Write execution summary for single algorithm
  //
  ostream &os = dnode.GetCont();

  //
  // Write algorithm summary
  //
  const Exec::Count &c = alg.GetCount();

  os << "Processed events:               " << c.nevent         << endl
     << "Active algorithm calls:         " << c.alg_ncalls     << endl
     << "ROB DataCollector calls:        " << c.rob_nreq       << endl
     << "  - retrieved:                  " << c.rob_nreq_retrv << endl
     << "  - cached:                     " << c.rob_nreq_cache << endl;

  Exec::WriteDETTable(dnode,alg,sum,sum.scale_corr);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableAlg::WriteROS(HtmlNode &rnode, const ExecClient &alg, const Exec::Summary &sum)
{
  //
  // Write execution summary for single algorithm
  //
  ostream &os = rnode.GetCont();

  //
  // Write algorithm summary
  //
  const Exec::Count &c = alg.GetCount();

  os << "Processed events:               " << c.nevent         << endl
     << "Active algorithm calls:         " << c.alg_ncalls     << endl
     << "ROB DataCollector calls:        " << c.rob_nreq       << endl
     << "  - retrieved:                  " << c.rob_nreq_retrv << endl
     << "  - cached:                     " << c.rob_nreq_cache << endl;

  Exec::WriteROSTable(rnode,alg,sum,sum.scale_corr,"Alg");
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableAlg::WriteChn(HtmlNode &cnode, const ExecClient &alg, const Exec::Summary &sum)
{
  //
  // Write execution summary for single algorithm
  //
  ostream &os = cnode.GetCont();

  //
  // Write algorithm summary
  //
  const Exec::Count &c = alg.GetCount();

  os << "Processed events:               " << c.nevent         << endl
     << "Active algorithm calls:         " << c.alg_ncalls     << endl
     << "ROB DataCollector calls:        " << c.rob_nreq       << endl
     << "  - retrieved:                  " << c.rob_nreq_retrv << endl
     << "  - cached:                     " << c.rob_nreq_cache << endl;

  Exec::WriteALGBYCHNTable(cnode,alg,sum,sum.scale_corr,fTimeout,fLevel);
}
