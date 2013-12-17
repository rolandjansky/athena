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
#include "TrigCostExec/ExecTableROS.h"
#include "TrigCostExec/ExecWrite.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableROS)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecTableROS::ExecTableROS()
  :fDir(0),
   fCanvas(0),
   fDebug(false),
   fFillNode(false),
   fMakeHist(false),
   fPlots(false),
   fTimeout(0),
   fDirName()
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableROS::~ExecTableROS()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROS::SetReg(const Registry &reg)
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
  reg.Get("RoiPlots",    fPlots);
  reg.Get("DirName",     fDirName);
  reg.Get("FillNode",    fFillNode);
  reg.Get("ClientLink",  fClientLink);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROS::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROS::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Count one algorithm execution
  //

  for(set<uint32_t>::const_iterator rit = alg.ros_ids.begin(); rit != alg.ros_ids.end(); ++rit) {
    const uint32_t ros_id = *rit;
    
    ExecClient *client = GetROSClient(ros_id, alg);
    client->ProcessAlg(alg);
    
    fActiveClients.push_back(client);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROS::FinishEvent(const Exec::Event &event)
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
const Anp::Exec::Count& Anp::ExecTableROS::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------
Anp::ExecClient* Anp::ExecTableROS::GetROSClient(unsigned ros_id, const Exec::AlgCall &alg)
{
  //
  // Make client for algorithm
  //
  ROSMap::iterator iros = fROS.find(ros_id);
  if(iros == fROS.end()) {

    Registry reg(fReg);

    //
    // Only create alg, roi and ros maps when this is a full summary
    //
    if(!fShortClient){
      reg.Set("DoAlg", true);
      reg.Set("DoDet", false);
      reg.Set("DoROS", true);
      reg.Set("DoRoi", true);
      reg.Set("DoChn", true);
    }

    reg.Set("ROSId", ros_id);
    
    if(fDebug) {
      log() << "GetROSClient - " << alg.GetROSName(ros_id) << endl;
    }

    Handle<ExecClient> client(new ExecCount());
    iros = fROS.insert(ROSMap::value_type(ros_id, client)).first;
    
    client->SetClientName(alg.GetROSName(ros_id));
    client->SetClientType("");
    client->SetReg(reg);
    client->SetDir(Anp::GetDir(fDir, alg.GetROSName(ros_id)));
  }

  return iros->second.get();
}

//--------------------------------------------------------------------------------------
void Anp::ExecTableROS::WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum)
{
  //
  // Write out results for entire run (regardless of lumi block)
  //
  if(fDebug) log() << "WriteExec - begin" << endl;
  ostream &os = anode.GetCont();
  
  os << endl << "Table shows summary for data requests and retrievals per ROS:";

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> ROS </th>"
     << "<th> Events </th>"
     << "<th> Requests </th>"
     << "<th> Retrievals </th>"
     << "<th> Caches </th>"
     << "<th> Retrievals/event </th>"
     << "<th> Request rate (Hz)</th>"
     << "<th> Retrieval rate (Hz)</th>"
     << "<th> Cache rate (Hz)</th>"
     << "<th> Retrieved data (Kb/s)</th>"
     << "<th> Cached data (kB) </th>"
     << "<th> Time per ROS retrieval (ms) </th>"
     << "<th> Time per event (ms) </th>"
     << "<th> ROBs/retrieval </th>"
     << "</tr>" << endl;
  
  ClientVec roses;

  for(ROSMap::iterator it = fROS.begin(); it != fROS.end(); ++it) {
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
    const string  ros_base = sum.html_base+"_"+ros_name;

    //
    // Create web page for current ROS
    //
    string ros_link = ros_name;

    if(sum.write_full) {      
      Handle<HtmlNode> nodeR(new HtmlNode(ros_base+".html", HtmlNode::kHTML));
      nodeR->SetBase(ros_base);
      anode.AddChild(nodeR);

      WriteROS(nodeR.ref(), *ros, sum);
      ros_link = nodeR->GetLink(ros_name);


      //
      // Write LB dependent histograms
      //
      if(!sum.sum_lbs.empty()) {
	WriteLBs(nodeR.ref(), *ros, sum);
      }
    }
    
    double ret_per_event = 0.0;
    double rob_per_ret   = 0.0;

    if(count.nevent > 0) {
      ret_per_event = count.rob_nreq_retrv/double(count.nevent);
    }
    if(count.rob_nreq_retrv > 0) {
      rob_per_ret = count.rob_nids_retrv/double(count.rob_nreq_retrv);
    }

    const string ret_per_event_str = Anp::Round2Pair(ret_per_event, 1.0).first;
    const string rob_per_ret_str   = Anp::Round2Pair(rob_per_ret,   1.0).first;


    const double size_ret_dbl = Exec::ConvertWordsToKB(count.rob_size_retrv)*sum.scale_corr;
    const double size_cache_dbl = Exec::ConvertWordsToKB(count.rob_size_cache)*sum.scale_corr;

    const string size_ret_str = Anp::Round2Pair(size_ret_dbl, 1.0).first;        
    const string size_cache_str = Anp::Round2Pair(size_cache_dbl, 1.0).first;

    const string rate_req_str = Anp::Round2Pair(count.rob_nreq      *sum.scale_corr, 1.0).first;    
    const string rate_ret_str = Anp::Round2Pair(count.rob_nreq_retrv*sum.scale_corr, 1.0).first;    
    const string rate_cache_str = Anp::Round2Pair(count.rob_nreq_cache*sum.scale_corr, 1.0).first;

    //
    // Write HTML table entries
    //
    os << "<tr>" 
       << "<td align=\"left\"> "   << ros_link                                    << " </td>"
       << "<td align=\"center\"> " << count.nevent                                << " </td>"
       << "<td align=\"center\"> " << count.rob_nreq                              << " </td>"
       << "<td align=\"center\"> " << count.rob_nreq_retrv                        << " </td>"
       << "<td align=\"center\"> " << count.rob_nreq_cache                        << " </td>"
       << "<td align=\"center\"> " << ret_per_event_str                           << " </td>"
       << "<td align=\"center\"> " << rate_req_str                                << " </td>"
       << "<td align=\"center\"> " << rate_ret_str                                << " </td>"
       << "<td align=\"center\"> " << rate_cache_str                              << " </td>"
       << "<td align=\"center\"> " << size_ret_str                                << " </td>"
       << "<td align=\"center\"> " << size_cache_str                              << " </td>"
       << "<td align=\"center\"> " << count.GetMeanROBTimePerRetr()               << " </td>"
       << "<td align=\"center\"> " << count.GetMeanROBTimePerEvent()              << " </td>"
       << "<td align=\"center\"> " << rob_per_ret_str                             << " </td>"
       << "</tr>" << endl;
  }
  
  os << "</table>" << endl;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROS::WriteROS(HtmlNode &rnode, const ExecClient &ros, const Exec::Summary &sum)
{
  if(fDebug) log() << "WriteROS - begin" << endl;
  //
  // Write execution summary for single algorithm
  //
  ostream &os = rnode.GetCont();
  os << "<pre>";
  //
  // Write algorithm summary
  //
  const Exec::Count &c = ros.GetCount();

  const string size_total_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size),       1.0).first;
  const string size_retrv_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size_retrv), 1.0).first;

  os << "ROS:                            " << ros.GetClientName() << endl
     << "Processed events:               " << c.nevent            << endl
     << "ROB DataCollector calls:        " << c.rob_nreq          << endl
     << "  - retrieved:                  " << c.rob_nreq_retrv    << endl
     << "  - cached:                     " << c.rob_nreq_cache    << endl
     << "Size of requested ROBs:         " << size_total_str      << " KB" << endl
     << "  - retrieved:                  " << size_retrv_str      << " KB" << endl;

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
     << "<th> Algorithm </th>"
     << "<th> ROS requests</th>"
     << "<th> ROS retrievals</th>"
     << "<th> Requested data (kB) </th>"
     << "<th> Retrieved data (kB) </th>"
     << "<th> Cached data (kB) </th>"
     << "<th> Request rate (Hz) </th>"
     << "<th> Retrieval rate (Hz) </th>"
     << "<th> Cache rate (Hz) </th>"
     << "<th> Requested bandwidth (kB/s) </th>"
     << "<th> Retrieved bandwidth (kB/s) </th>"
     << "<th> Cached bandwidth (kB/s) </th>"
     << "<th> Requests/call</th>"
     << "<th> Retrievals/call</th>"
     << "<th> ROBs/retrieval</th>"
     << "</tr>" 
     << endl;

  const std::map<uint32_t, Exec::Count> &alg_map = ros.GetAlgCount();
  vector<Exec::Count> counts;
  
  for(map<uint32_t, Exec::Count>::const_iterator it = alg_map.begin(); it != alg_map.end(); ++it) {
    Exec::Count c = it->second;
    c.alg_id      = it->first;    
    counts.push_back(c);
  }
  
  //
  // Sort algorithms by number of retrieved ROBs
  //
  std::sort(counts.begin(), counts.end(), SortExecCount("rob_nreq_retrv"));

  uint64_t total_nreq = 0.0,   total_nret = 0.0;
  uint64_t total_sreq = 0.0,   total_sret = 0.0;
  uint64_t total_ncache = 0.0, total_scache = 0.0;

  for(unsigned i = 0; i < counts.size(); ++i) {
    const Exec::Count &ac = counts.at(i);
    
    const string str_req_size   = Anp::Round2Pair(Exec::ConvertWordsToKB(ac.rob_size),       0.1).first;
    const string str_ret_size   = Anp::Round2Pair(Exec::ConvertWordsToKB(ac.rob_size_retrv), 0.1).first;
    const string str_cache_size = Anp::Round2Pair(Exec::ConvertWordsToKB(ac.rob_size_cache), 0.1).first;
    
    double req_evnt  = 0.0, ret_evnt = 0.0;
    double req_call  = 0.0, ret_call = 0.0;
    double rob_pret  = 0.0;

    if(ac.nevent > 0) {
      req_evnt = ac.ros_nreq      /double(ac.nevent);
      ret_evnt = ac.ros_nreq_retrv/double(ac.nevent);
    }
    if(ac.alg_ncalls > 0) {
      req_call = ac.ros_nreq      /double(ac.alg_ncalls);
      ret_call = ac.ros_nreq_retrv/double(ac.alg_ncalls);
    }
    if(ac.rob_nreq_retrv > 0) {
      rob_pret = ac.rob_nids_retrv/double(ac.rob_nreq_retrv);
    }
      
    const string str_req_evnt = Anp::Round2Pair(req_evnt, 0.1).first;
    const string str_ret_evnt = Anp::Round2Pair(ret_evnt, 0.1).first;

    const string str_req_call = Anp::Round2Pair(req_call, 0.1).first;
    const string str_ret_call = Anp::Round2Pair(ret_call, 0.1).first;  

    const string str_req_rate = Anp::Round2Pair(sum.scale_corr*double(ac.ros_nreq),       0.1).first;  
    const string str_ret_rate = Anp::Round2Pair(sum.scale_corr*double(ac.ros_nreq_retrv), 0.1).first; 
    const string str_cache_rate = Anp::Round2Pair(sum.scale_corr*double(ac.ros_nreq_cache), 0.1).first;

    const string str_req_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(ac.rob_size), 0.1).first;  
    const string str_ret_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(ac.rob_size_retrv), 0.1).first;  
    const string str_cache_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(ac.rob_size_cache), 0.1).first;
    
    const string str_rob_pret = Anp::Round2Pair(rob_pret, 1.0).first;

    os << "<tr>" 
       << "<td align=\"left\">"  << sum.GetAlgName(ac.alg_id) << "</td>"
       << "<td align=\"right\">" << ac.ros_nreq               << "</td>"
       << "<td align=\"right\">" << ac.ros_nreq_retrv         << "</td>"
       << "<td align=\"right\">" << str_req_size              << "</td>"
       << "<td align=\"right\">" << str_ret_size              << "</td>"
       << "<td align=\"right\">" << str_cache_size            << "</td>"
       << "<td align=\"right\">" << str_req_rate              << "</td>"
       << "<td align=\"right\">" << str_ret_rate              << "</td>"
       << "<td align=\"right\">" << str_cache_rate            << "</td>"
       << "<td align=\"right\">" << str_req_band              << "</td>"
       << "<td align=\"right\">" << str_ret_band              << "</td>"
       << "<td align=\"right\">" << str_cache_rate            << "</td>"
       << "<td align=\"right\">" << str_req_call              << "</td>"
       << "<td align=\"right\">" << str_ret_call              << "</td>"
       << "<td align=\"right\">" << str_rob_pret              << "</td>"
       << "</tr>"
       << endl;

    total_nreq     += ac.ros_nreq;
    total_nret     += ac.ros_nreq_retrv;
    total_ncache   += ac.ros_nreq_cache;
    total_sreq     += ac.rob_size;
    total_sret     += ac.rob_size_retrv;
    total_scache   += ac.rob_size_cache;
  }

  const string tot_sreq = Anp::Round2Pair(Exec::ConvertWordsToKB(total_sreq), 1.0).first;
  const string tot_sret = Anp::Round2Pair(Exec::ConvertWordsToKB(total_sret), 1.0).first;
  const string tot_scache = Anp::Round2Pair(Exec::ConvertWordsToKB(total_scache), 1.0).first;


  const string tot_req_rate = Anp::Round2Pair(sum.scale_corr*total_nreq, 1.0).first;
  const string tot_ret_rate = Anp::Round2Pair(sum.scale_corr*total_nret, 1.0).first;
  const string tot_cache_rate = Anp::Round2Pair(sum.scale_corr*total_ncache, 1.0).first;
  const string tot_req_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(total_sreq), 1.0).first;
  const string tot_ret_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(total_sret), 1.0).first;
  const string tot_cache_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(total_scache), 1.0).first;
  
  os << "<tr>" 
     << "<td align=\"left\">Total</td>"
     << "<td align=\"right\">" << total_nreq     << "</td>"
     << "<td align=\"right\">" << total_nret     << "</td>"
     << "<td align=\"right\">" << tot_sreq       << "</td>"
     << "<td align=\"right\">" << tot_sret       << "</td>"
     << "<td align=\"right\">" << tot_scache     << "</td>"
     << "<td align=\"right\">" << tot_req_rate   << "</td>"
     << "<td align=\"right\">" << tot_ret_rate   << "</td>"
     << "<td align=\"right\">" << tot_cache_rate << "</td>"
     << "<td align=\"right\">" << tot_req_band   << "</td>"
     << "<td align=\"right\">" << tot_ret_band   << "</td>"
     << "<td align=\"right\">" << tot_cache_band << "</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << endl;
  
  os << "</table><pre>"<< endl << endl;

  Exec::WriteCHNBYROSTable(rnode,ros,sum,sum.scale_corr,"Chain");

  if(!fMakeHist){
    return;
  }
    //
    // Plot ROBs in ROS request...scaled rates
    //    - one plot for each ROS
    //    - normalized to total request rates
    //
    
    string suff;  
    stringstream ros_id;
    ros_id << c.ros_id;
    if     (fHistOpt.find("png") != string::npos) suff = ".png";
    else if(fHistOpt.find("eps") != string::npos) suff = ".eps";
    
    Exec::ROSHist rhe = ros.GetROSHistEvent();
    Exec::ROSHist rhc = ros.GetROSHistCalls();
    Exec::ROSHist rhr = ros.GetROSHistROI();  
    //
    // scale histograms to the correct ROS rate
    //
    if(rhc.hROSNrobidsRetrv) rhc.hROSNrobidsRetrv->Scale(sum.scale_corr);
    if(rhc.hROSNrobids)      rhc.hROSNrobids     ->Scale(sum.scale_corr);
    //
    // Print histograms and insert links
    //
    if(fCanvas) {
      fCanvas->cd();
    }

    //
    // Plot ROBs in ROS request...scaled rates
    //    - one plot for each ROS
    //    - normalized to total request rates
    //    - plot by roi
    //    - plot by event
    //    - plot by alg call
    //

    os << "Request Rate Histograms by ROS Call: " << endl;
    if(rhc.hROSNrobids) Anp::PrintHist(rnode, rhc.hROSNrobids, fCanvas, rnode.GetBase()+ros_id.str()+"_hret_rate_vs_nROBs_req"+suff,
		   "  ROS Retrieval Rate Binned by Number of ROBs Requested", fHistOpt);
    if(rhc.hROSNrobidsRetrv) Anp::PrintHist(rnode, rhc.hROSNrobidsRetrv, fCanvas, rnode.GetBase()+ros_id.str()+"_hret_rate_vs_nROBs_ret"+suff,
		   "  ROS Retrieval Rate Binned by Number of ROBs Retrieved", fHistOpt);

    os << "Histograms by Events: " << endl;
    if(rhe.hROSNreq) Anp::PrintHist(rnode, rhe.hROSNreq, fCanvas, rnode.GetBase()+ros_id.str()+"_evt_hreq_count"+suff,
		   "  ROS Number of Requests", fHistOpt);
    if(rhe.hROSNreqRetrv) Anp::PrintHist(rnode, rhe.hROSNreqRetrv, fCanvas, rnode.GetBase()+ros_id.str()+"_evt_hret_count"+suff,
		   "  ROS Number of Retrievals", fHistOpt);
    if(rhe.hROSSize) Anp::PrintHist(rnode, rhe.hROSSize, fCanvas, rnode.GetBase()+ros_id.str()+"_evt_hreq_size"+suff,
		   "  Size Requested (kB) from ROS", fHistOpt);
    if(rhe.hROSSizeRetrv) Anp::PrintHist(rnode, rhe.hROSSizeRetrv, fCanvas, rnode.GetBase()+ros_id.str()+"_evt_hret_size"+suff,
		   "  Size Retrieved (kB) from ROS", fHistOpt);

    if(fPlots){
      os << "Histograms by Algorithm Call: " << endl;
      if(rhc.hROSNreq) Anp::PrintHist(rnode, rhc.hROSNreq, fCanvas, rnode.GetBase()+ros_id.str()+"_call_hreq_count"+suff,
				      "  ROS Number of Requests", fHistOpt);
      if(rhc.hROSNreqRetrv) Anp::PrintHist(rnode, rhc.hROSNreqRetrv, fCanvas, rnode.GetBase()+ros_id.str()+"_call_hret_count"+suff,
					   "  ROS Number of Retrievals", fHistOpt);
      if(rhc.hROSSize) Anp::PrintHist(rnode, rhc.hROSSize, fCanvas, rnode.GetBase()+ros_id.str()+"_call_hreq_size"+suff,
				      "  Size Requested (kB) from ROS", fHistOpt);
      if(rhc.hROSSizeRetrv) Anp::PrintHist(rnode, rhc.hROSSizeRetrv, fCanvas, rnode.GetBase()+ros_id.str()+"_call_hret_size"+suff,
					   "  Size Retrieved (kB) from ROS", fHistOpt);
      
      os << "Histograms by ROI: " << endl;
      if(rhr.hROSNreq) Anp::PrintHist(rnode, rhr.hROSNreq, fCanvas, rnode.GetBase()+ros_id.str()+"_roi_hreq_count"+suff,
				      "  ROS Number of Requests", fHistOpt);
      if(rhr.hROSNreqRetrv) Anp::PrintHist(rnode, rhr.hROSNreqRetrv, fCanvas, rnode.GetBase()+ros_id.str()+"_roi_hret_count"+suff,
					   "  ROS Number of Retrievals", fHistOpt);
      if(rhr.hROSSize) Anp::PrintHist(rnode, rhr.hROSSize, fCanvas, rnode.GetBase()+ros_id.str()+"_roi_hreq_size"+suff,
				      "  Size Requested (kB) from ROS", fHistOpt);
      if(rhr.hROSSizeRetrv) Anp::PrintHist(rnode, rhr.hROSSizeRetrv, fCanvas, rnode.GetBase()+ros_id.str()+"_roi_hret_size"+suff,
					   "  Size Retrieved (kB) from ROS", fHistOpt);
    }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROS::WriteLBs(HtmlNode &rnode, const ExecClient &exec, const Exec::Summary &sum)
{
  if(fDebug) log() << "WriteLBs - begin" << endl;
  //
  // Find ROS id
  //
  uint32_t ros_id = 0;

  for(ROSMap::const_iterator rit = fROS.begin(); rit != fROS.end(); ++rit) {
    if(rit->second->GetClientName() == exec.GetClientName()) {
      ros_id = rit->first;
    }
  }
  
  if(ros_id == 0) {
    return;
  }

  //
  // Find LB range
  //
  unsigned min_lb = 0, max_lb = 0;

  for(unsigned i = 0; i < sum.sum_lbs.size(); ++i) {
    const Exec::SummaryLB &s = sum.sum_lbs.at(i);

    if(i == 0) {
      min_lb = s.lb;
      max_lb = s.lb;
    }
    else {
      min_lb = std::min<unsigned>(min_lb, s.lb);
      max_lb = std::max<unsigned>(max_lb, s.lb);
    }
  }

  //
  // Create histograms
  //
  TDirectory *dir = Anp::GetDir(fDir, exec.GetClientName());

  TH1 *hreq_rate = MakeHist("hreq_rate_vs_lb", "Request rate (Hz)",                min_lb, max_lb, dir);
  TH1 *hret_rate = MakeHist("hret_rate_vs_lb", "Retrieval rate (Hz)",              min_lb, max_lb, dir);
  TH1 *hreq_size = MakeHist("hreq_size_vs_lb", "Request bandwidth (kB/s)",         min_lb, max_lb, dir);
  TH1 *hret_size = MakeHist("hret_size_vs_lb", "Retrieval bandwidth (kB/s)",       min_lb, max_lb, dir);  
  TH1 *hret_time = MakeHist("hret_time_vs_lb", "Mean time per ROB retrieval (ms)", min_lb, max_lb, dir);

  //
  // Iterate over lumi blocks
  //
  for(unsigned i = 0; i < sum.sum_lbs.size(); ++i) {
    const Exec::SummaryLB &s = sum.sum_lbs.at(i);

    const ExecTableROS *exec = dynamic_cast<const ExecTableROS *>(s.client.get());
    if(!exec) {
      log() << "WriteLBs - missing ExecTableROS algorithm" << endl;
      continue;
    }

    const std::map<uint32_t, Handle<ExecClient> >::const_iterator rit = exec->fROS.find(ros_id);
    if(rit == exec->fROS.end()) {
      if(fDebug) {
	log() << "WriteLBs - missing ROS id=" << ros_id << " at lb=" << s.lb << endl;
      }
      continue;
    }

    const Exec::Count &rc = rit->second->GetCount();
    
    //
    // Fill histograms
    //
    if(hreq_rate) hreq_rate->Fill(s.lb, s.scale_corr*rc.rob_nreq);
    if(hret_rate) hret_rate->Fill(s.lb, s.scale_corr*rc.rob_nreq_retrv);
    if(hreq_size) hreq_size->Fill(s.lb, s.scale_corr*Exec::ConvertWordsToKB(rc.rob_size));
    if(hret_size) hret_size->Fill(s.lb, s.scale_corr*Exec::ConvertWordsToKB(rc.rob_size_retrv));
    if(hret_time) hret_time->Fill(s.lb, rc.GetMeanROBTimePerRetr());    
  }

  string suff;  
  if     (fHistOpt.find("png") != string::npos) suff = ".png";
  else if(fHistOpt.find("eps") != string::npos) suff = ".eps";
  
  //
  // Print histograms and insert links
  //
  if(fCanvas) {
    fCanvas->cd();
  }

  rnode.GetCont() << endl << "ROS measurements versus lumi block number:" << endl;

  Anp::PrintHist(rnode, hreq_rate, fCanvas, rnode.GetBase()+"_hreq_rate"+suff,
		 "  Requested rate", fHistOpt);

  Anp::PrintHist(rnode, hret_rate, fCanvas, rnode.GetBase()+"_hret_rate"+suff,
		 "  Retrieved rate", fHistOpt);

  Anp::PrintHist(rnode, hreq_size, fCanvas, rnode.GetBase()+"_hreq_size"+suff,
		 "  Requested bandwidth", fHistOpt);

  Anp::PrintHist(rnode, hret_size, fCanvas, rnode.GetBase()+"_hret_size"+suff,
		 "  Retrieved bandwidth", fHistOpt);

  Anp::PrintHist(rnode, hret_time, fCanvas, rnode.GetBase()+"_hret_time"+suff,
		 "  Mean time per ROB retrieval", fHistOpt);
}

//--------------------------------------------------------------------------------------      
TH1* Anp::ExecTableROS::MakeHist(const std::string &hname, 
				 const std::string &yaxis, 
				 unsigned min_lb, 
				 unsigned max_lb,
				 TDirectory *dir)
{
  //
  // Create histogram
  //
  if(!(min_lb > 0 && min_lb < max_lb)) {
    return 0;
  }

  TH1 *h = new TH1F(hname.c_str(), "", max_lb-min_lb+1, min_lb, max_lb+1);
  h->SetDirectory(dir);
  h->SetStats(false);
  h->GetXaxis()->CenterTitle();
  h->GetXaxis()->SetTitle("luminosity block");
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(yaxis.c_str());  

  return h;
}

//--------------------------------------------------------------------------------------      
std::ostream& Anp::ExecTableROS::log() const
{
  std::cout << GetClientName() << "::";
  return std::cout;
}
