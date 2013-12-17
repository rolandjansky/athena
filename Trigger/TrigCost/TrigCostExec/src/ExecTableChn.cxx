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
#include "TrigCostExec/ExecTableChn.h"
#include "TrigCostExec/ExecWrite.h"
#include "TrigCostExec/Util.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableChn)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecTableChn::ExecTableChn()
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
Anp::ExecTableChn::~ExecTableChn()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableChn::SetReg(const Registry &reg)
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
  reg.Get("DirName",     fDirName);
  reg.Get("FillNode",    fFillNode);
  reg.Get("ClientLink",  fClientLink);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableChn::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableChn::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Count one algorithm execution
  //
  ExecClient *client = GetChnClient(*alg.chn_conf);
  client->ProcessAlg(alg);

  fActiveClients.push_back(client);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableChn::FinishEvent(const Exec::Event &event)
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
const Anp::Exec::Count& Anp::ExecTableChn::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------      
Anp::ExecClient* Anp::ExecTableChn::GetChnClient(const TrigConfChain &chn_conf)
{
  //
  // Make client for chain
  //
  unsigned counter = chn_conf.getEncodedId();

  ChnMap::iterator ichn = fChn.find(counter);
  if(ichn == fChn.end()) { 

    Registry reg(fReg);

    //
    // Only create alg, roi and ros maps when this is a full summary
    //    -- not really necessary for ExecTableChn
    //    -- note: the default for all of these are false
    //
    if(!fShortClient){
      reg.Set("DoAlg", false);
      reg.Set("DoDet", false);
      reg.Set("DoROS", true);
      reg.Set("DoChn", false);

    }

    reg.Set("ChnId", counter);

    if(fDebug) {
      cout << "ExecTableChn::GetChnClient - " << chn_conf.getName() << endl;
    }

    stringstream link_str;
    link_str << "CHN_NAME_" << setw(10) << setfill('0') << counter;

    Handle<ExecClient> client(new ExecCount());
    ichn = fChn.insert(ChnMap::value_type(chn_conf.getEncodedId(), client)).first;
    
    client->SetClientName(chn_conf.getName());
    client->SetClientLink(link_str.str());
    client->SetClientType("chain");
    client->SetReg(reg);
    client->SetDir(Anp::GetDir(fDir, chn_conf.getName()));
  }    

  return ichn->second.get();
}

//-------------------------------------------------------------------------------------- 
void Anp::ExecTableChn::WriteExec(HtmlNode &anode, HtmlMenu &hmenu, const Exec::Summary &sum)
{
  //
  // Write out results
  //
  ostream &os = anode.GetCont();

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Chain </th>"
     << "<th> Active Events </th>"
     << "<th> Time Per Event (ms) </th>"
     << "<th> Chain Rate (Hz) </th>"
     << "<th> Calls > " << Anp::Round2Pair(fTimeout, 10.0).first << " ms </th>"
     << "<th> Total Chn Time (s) </th>"
     << "<th> Total Chn Time (%) </th>";

  if(GetLevel() == 0 || GetLevel() == 2) {
    os << "<th> ROS request rate (Hz)</th>"
       << "<th> ROS retrieval rate (Hz)</th>";
  }

  os << "</tr>" << endl;
  
  const double sc = sum.scale_corr;
  ClientVec chns;

  for(ChnMap::iterator it = fChn.begin(); it != fChn.end(); ++it) {
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

    if(sum.write_full) {
      Handle<HtmlNode> nodeC(new HtmlNode(sum.html_base+"_"+chn->GetClientLink()+"_chn.html", HtmlNode::kHTML));
      anode.AddChild(nodeC);
      nodeC->SetBase(sum.html_base+"_"+chn->GetClientLink());
      
      nodeC->GetCont() << "<pre>" << endl;
      
      //
      // Insert link to Chain Configuration page
      //
      Handle<HtmlNode> nodeP = hmenu.FindChnNode(chn->GetClientName());
      if(nodeP.valid()) {
	nodeC->GetCont() << "Chain: " << nodeP->GetLink(chn->GetClientName(),"../") << endl << endl;
      }
      else {
	nodeC->GetCont() << "Chain: " << chn->GetClientName() << endl << endl;
      }
      
      WriteChn(nodeC.ref(), *chn, sum);
      nodeC->GetCont() << "</pre>" << endl;

      chn_link = nodeC->GetLink(chn->GetClientName());
    }

    //
    // Compute helper variables
    //
    double time_frac = 0.0;
    if(sum.total_time > 0.0) {
      time_frac = 100.0*count.timer_alg/sum.total_time;
    }

    const string chain_rate_str = Anp::Round2Pair(static_cast<double>(count.nevent)*sc,       1.0).first;
    const string time_totl_str  = Anp::Round2Pair(count.timer_alg/1000.0,                     1.0).first;
    const string time_frac_str  = Anp::Round2Pair(time_frac,                                  0.1).first;

    //
    // Write HTML table entries
    //
    os << "<tr>" 
       << "<td align=\"left\"> "   << chn_link                                    << " </td>"
       << "<td align=\"center\"> " << count.nevent                                << " </td>"
       << "<td align=\"center\"> " << count.GetMeanAlgTimePerEvent()              << " </td>"
       << "<td align=\"center\"> " << chain_rate_str                              << " </td>"
       << "<td align=\"center\"> " << count.chn_nslow                             << " </td>"
       << "<td align=\"center\"> " << time_totl_str                               << " </td>"
       << "<td align=\"center\"> " << time_frac_str                               << " </td>";

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
void Anp::ExecTableChn::WriteChn(HtmlNode &anode, const ExecClient &chn, const Exec::Summary &sum)
{
  //
  // Write execution summary for single chain
  //
  ostream &os = anode.GetCont();

  const Exec::Count &c = chn.GetCount();

  TrigConfChain *chn_conf = sum.chn_conf->find(c.chn_id)->second.get();

  const string evt_time_str = Anp::Round2Pair(c.GetMeanAlgTimePerEvent(), 0.1).first;
  const string str_chn_rate = Anp::Round2Pair(sum.scale_corr*double(c.nevent), 0.1).first;
	
  os   << "Settings for processed events: "
       << endl
       << "Prescale:     "  << chn_conf->getPS()
       << endl
       << "Pass Through: "  << chn_conf->getPT()
       << endl << endl;

  os   << "Timing information for processed events: "
       << endl
       << "Number of active events:    " << c.nevent                  << endl
       << "Chain Rate:                 " << str_chn_rate   << " Hz"   << endl
       << "Total time:                 " << c.timer_alg    << " ms"   << endl
       << "Mean time per event:        " << evt_time_str   << " ms"   << endl
       << "Number of algorithm calls:  " << c.alg_ncalls	      << endl
       << "Number of algorithm caches: " << c.alg_ncache	      << endl;

  if(GetLevel() == 0 || GetLevel() == 2) {

    const string str_req_rate = Anp::Round2Pair(sum.scale_corr*double(c.ros_nreq), 0.1).first;  
    const string str_ret_rate = Anp::Round2Pair(sum.scale_corr*double(c.ros_nreq_retrv), 0.1).first;
    const string str_req_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToMB(c.rob_size), 0.1).first;  
    const string str_ret_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToMB(c.rob_size_retrv), 0.1).first;

    os   << endl
	 << "ROS request Rate:           " << str_req_rate   << " hz"   << endl
	 << "ROS retrieval Rate:         " << str_ret_rate   << " hz"   << endl
	 << "ROS request Bandwidth:      " << str_req_band   << " Mb/s" << endl
	 << "ROS retrieval Bandwidth:    " << str_ret_band   << " Mb/s" << endl;

    if(c.ros_nreq>0){
      //
      // Write HTML page with ROS requests
      //
      //Handle<HtmlNode> rnode(new HtmlNode(sum.html_base+"_"+chn.GetClientLink()+"_ros.html", HtmlNode::kHTML));
      //anode.AddChild(rnode);
      //anode.GetCont() << rnode->GetLink("Data requests summary per ROS") << endl;
      //
      //rnode->GetCont() << "<pre>";
      //rnode->AddLink("top ", "../", false);
      //rnode->AddLink("index", "index.html", false);
      //
      //rnode->GetCont() << endl << endl << anode.GetLink(chn.GetClientName())
      //		       << " requested data from " << chn.GetROSCount().size() << " ROS(s): " 
      //		       << endl << endl;
      //
      //WriteROS(rnode.ref(), chn, sum);
      //
      //rnode->GetCont() << "</pre>" << endl;
      os << endl << "Data requests summary per ROS" << endl << endl;
      Exec::WriteROSTable(anode,chn,sum,sum.scale_corr,"Chain");
    }
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

  const Exec::AlgHist &ahe = chn.GetAlgHistEvent();
  const Exec::ROSHist &rhe = chn.GetROSHistEvent();

  //
  // Writing histograms for chain
  //
  if(fCanvas) {
    fCanvas->cd();
  }

  os   << endl 
       << "Histograms Filled per Event: "                << endl;

  if(ahe.hAlgTime)     Anp::PrintHist(anode, ahe.hAlgTime, fCanvas, base+"event_alg_time"+suff,
				      "  Chain total time", fHistOpt);
  if(ahe.hAlgTimeFrac) Anp::PrintHist(anode, ahe.hAlgTimeFrac, fCanvas, base+"event_time_frac"+suff,
				      "  Chain time/Event time", fHistOpt);
  if(ahe.hAlgNCalls)   Anp::PrintHist(anode, ahe.hAlgNCalls, fCanvas, base+"event_num_calls"+suff,
				      "  Number of executed algorithm calls", fHistOpt);
  if(ahe.hAlgNCache)   Anp::PrintHist(anode, ahe.hAlgNCache, fCanvas, base+"event_num_cache"+suff,
				      "  Number of cached algorithm calls", fHistOpt);

  if(GetLevel() == 0 || GetLevel() == 2) {

    if(rhe.hROSNreq)      Anp::PrintHist(anode, rhe.hROSNreq, fCanvas, base+"event_ros_nreq"+suff,
					 "  Number of ROS requests", fHistOpt);
    if(rhe.hROSNreqRetrv) Anp::PrintHist(anode, rhe.hROSNreqRetrv, fCanvas, base+"event_ros_nreq_retrv"+suff,
					 "  Number of ROS retrieved", fHistOpt);
    if(rhe.hROSSize)      Anp::PrintHist(anode, rhe.hROSSize, fCanvas, base+"event_ros_size"+suff,
					 "  Size of ROS requests", fHistOpt);
    if(rhe.hROSNreqRetrv) Anp::PrintHist(anode, rhe.hROSNreqRetrv, fCanvas, base+"event_ros_size_retrv"+suff,
					 "  Size of ROS retrieved", fHistOpt);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableChn::WriteROS(HtmlNode &rnode, const ExecClient &chn, const Exec::Summary &sum)
{
  //
  // Write execution summary for single chain
  //
  ostream &os = rnode.GetCont();

  //
  // Write ros summary
  //
  const Exec::Count &c = chn.GetCount();

  os << "Processed events:               " << c.nevent         << endl
     << "Active Chain calls:             " << c.alg_ncalls     << endl
     << "ROB DataCollector calls:        " << c.rob_nreq       << endl
     << "  - retrieved:                  " << c.rob_nreq_retrv << endl
     << "  - cached:                     " << c.rob_nreq_cache << endl;

  Exec::WriteCHNBYROSTable(rnode,chn,sum,sum.scale_corr,"ROS");
}

