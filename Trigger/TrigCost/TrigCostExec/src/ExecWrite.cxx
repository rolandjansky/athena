/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// C/C++
#include <sstream>
#include <cmath>
#include <iostream>

// Local
#include "TrigCostExec/ExecWrite.h"
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/ExecTableAlg.h"
#include "TrigCostExec/Util.h"

using namespace std;

//--------------------------------------------------------------------
void Anp::Exec::WriteROSTable(HtmlNode &rnode, const ExecClient &alg, const Exec::Summary &sum, const double scale,const string type){
  
  //
  // Write execution summary for single algorithm
  //
  ostream &os = rnode.GetCont();

  //
  // Write algorithm summary
  //
  const Exec::Count &c = alg.GetCount();

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
     << "<th> ROS </th>"
     << "<th> Request rate (Hz) </th>"
     << "<th> Retrieval rate (Hz) </th>" 
     << "<th> Request data rate (kB/s) </th>"
     << "<th> Retrieval data rate (kB/s) </th>"
     << "<th> Requests </th>"
     << "<th> Retrievals</th>"
     << "<th> Requested data (kB) </th>"
     << "<th> Retrieved data (kB) </th>";

  if(type=="Alg"){
    os << "<th> Requests/call</th>"
       << "<th> Retrievals/call</th>";
  }

  if(type=="Chain" || type=="Sequence"){
    os << "<th> Requests/evt</th>"
       << "<th> Retrievals/evt</th>";
  }

  os << "</tr>" 
     << endl;
  
  const double sc = scale;

  const std::map<uint32_t, Exec::Count> &ros_map = alg.GetROSCount();  

  vector<Exec::Count> counts;
  
  for(map<uint32_t, Exec::Count>::const_iterator it = ros_map.begin(); it != ros_map.end(); ++it) {
    Exec::Count c = it->second;
    c.ros_id      = it->first;    
    counts.push_back(c);
  }

  //
  // Sort algorithms by number of retrieved ROBs
  //
  std::sort(counts.begin(), counts.end(), SortExecCount("rob_nreq_retrv"));

  uint64_t total_rob_sreq = 0, total_rob_sret = 0;
  uint64_t total_ros_nreq = 0, total_ros_nret = 0;

  for(unsigned i = 0; i < counts.size(); ++i) {
    const Exec::Count &rc = counts.at(i);
    
    const string rosname = sum.GetROSName(rc.ros_id);
    const string str_sreq = Anp::Round2Pair(Exec::ConvertWordsToKB(rc.rob_size),       1.0).first;
    const string str_sret = Anp::Round2Pair(Exec::ConvertWordsToKB(rc.rob_size_retrv), 1.0).first;
    
    const string str_nreq_rate = Anp::Round2Pair(sc*rc.rob_nreq,       1.0).first;
    const string str_nret_rate = Anp::Round2Pair(sc*rc.rob_nreq_retrv, 1.0).first;

    const string str_sreq_rate = Anp::Round2Pair(sc*Exec::ConvertWordsToKB(rc.rob_size),       1.0).first;
    const string str_sret_rate = Anp::Round2Pair(sc*Exec::ConvertWordsToKB(rc.rob_size_retrv), 1.0).first;        

    double req_evnt = 0.0, ret_evnt = 0.0;
    double req_call = 0.0, ret_call = 0.0;

    if(c.nevent > 0) {
      req_evnt = rc.ros_nreq      /double(c.nevent);
      ret_evnt = rc.ros_nreq_retrv/double(c.nevent);
    }
    if(c.alg_ncalls > 0) {
      req_call = rc.ros_nreq      /double(c.alg_ncalls);
      ret_call = rc.ros_nreq_retrv/double(c.alg_ncalls);
    }

    const string str_req_call = Anp::Round2Pair(req_call, 0.1).first;
    const string str_ret_call = Anp::Round2Pair(ret_call, 0.1).first;  
    const string str_req_evnt = Anp::Round2Pair(req_evnt, 0.1).first;
    const string str_ret_evnt = Anp::Round2Pair(ret_evnt, 0.1).first;   

    os << "<tr>" 
       << "<td align=\"left\"><nobr>" << rosname       << "</nobr></td>"
       << "<td align=\"right\">" << str_nreq_rate      << "</td>"
       << "<td align=\"right\">" << str_nret_rate      << "</td>"
       << "<td align=\"right\">" << str_sreq_rate      << "</td>"
       << "<td align=\"right\">" << str_sret_rate      << "</td>"
       << "<td align=\"right\">" << rc.ros_nreq        << "</td>"
       << "<td align=\"right\">" << rc.ros_nreq_retrv  << "</td>"
       << "<td align=\"right\">" << str_sreq           << "</td>"
       << "<td align=\"right\">" << str_sret           << "</td>";  
    if(type=="Alg"){
      os       << "<td align=\"right\">" << str_req_call       << "</td>"
	       << "<td align=\"right\">" << str_ret_call       << "</td>";   
    }
    if(type=="Chain" || type=="Sequence"){
      os       << "<td align=\"right\">" << str_req_evnt       << "</td>"
	       << "<td align=\"right\">" << str_ret_evnt       << "</td>";   
    }

    os << endl;


    total_ros_nreq += rc.ros_nreq;
    total_ros_nret += rc.ros_nreq_retrv;

    total_rob_sreq += rc.rob_size;
    total_rob_sret += rc.rob_size_retrv;
  }

  //
  // Write totals
  //
  const string str_nreq      = Anp::Round2Pair(total_ros_nreq, 1.0).first;
  const string str_nret      = Anp::Round2Pair(total_ros_nret, 1.0).first;
  
  const string str_nreq_rate = Anp::Round2Pair(sc*total_ros_nreq, 1.0).first;
  const string str_nret_rate = Anp::Round2Pair(sc*total_ros_nret, 1.0).first;

  const string str_sreq      = Anp::Round2Pair(Exec::ConvertWordsToKB(total_rob_sreq), 1.0).first;
  const string str_sret      = Anp::Round2Pair(Exec::ConvertWordsToKB(total_rob_sret), 1.0).first;

  const string str_sreq_rate = Anp::Round2Pair(sc*Exec::ConvertWordsToKB(total_rob_sreq), 1.0).first;
  const string str_sret_rate = Anp::Round2Pair(sc*Exec::ConvertWordsToKB(total_rob_sret), 1.0).first;

  double total_req_call = 0.0;
  double total_ret_call = 0.0;
  double total_req_evnt = 0.0;
  double total_ret_evnt = 0.0;
  
  if(c.nevent>0){
    total_req_evnt = total_ros_nreq/double(c.nevent);
    total_ret_evnt = total_ros_nret/double(c.nevent);
  }

  if(c.alg_ncalls > 0) {
    total_req_call = total_ros_nreq/double(c.alg_ncalls);
    total_ret_call = total_ros_nret/double(c.alg_ncalls);
  }
  
  const string tot_req_call = Anp::Round2Pair(total_req_call, 0.1).first;
  const string tot_ret_call = Anp::Round2Pair(total_ret_call, 0.1).first;  
  const string tot_req_evnt = Anp::Round2Pair(total_req_evnt, 0.1).first;
  const string tot_ret_evnt = Anp::Round2Pair(total_ret_evnt, 0.1).first; 

  os << "<tr>" 
     << "<td align=\"left\">Total</td>"
     << "<td align=\"right\">" << str_nreq_rate << "</td>"
     << "<td align=\"right\">" << str_nret_rate << "</td>"
     << "<td align=\"right\">" << str_sreq_rate << "</td>"
     << "<td align=\"right\">" << str_sret_rate << "</td>"
     << "<td align=\"right\">" << str_nreq      << "</td>"
     << "<td align=\"right\">" << str_nret      << "</td>"
     << "<td align=\"right\">" << str_sreq      << "</td>"
     << "<td align=\"right\">" << str_sret      << "</td>";
  if(type=="Alg"){
    os << "<td align=\"right\">" << tot_req_call  << "</td>"
       << "<td align=\"right\">" << tot_ret_call  << "</td>";
  }
  if(type=="Chain" || type=="Sequence"){
    os << "<td align=\"right\">" << tot_req_call  << "</td>"
       << "<td align=\"right\">" << tot_ret_call  << "</td>";
  }
  os << endl
     << "</table>" << endl;
}

//--------------------------------------------------------------------
void Anp::Exec::WriteDETTable(HtmlNode &dnode, const ExecClient &alg, const Exec::Summary &sum, const double ){
  //
  // Write execution summary for single algorithm
  //
  ostream &os = dnode.GetCont();

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
     << "<th> Total ROB requests </th>"
     << "<th> Retrieved ROB requests </th>"
     << "<th> Requested data (kB) </th>"
     << "<th> Retrieved data (kB) </th>"
     << "<th> Cached data (kB) </th>"
     << "</tr>" 
     << endl;

  const std::map<uint32_t, Exec::Count> &det_map = alg.GetDetCount();  
  
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
     << "<td align=\"right\">" << tot_req    << "</td>"
     << "<td align=\"right\">" << tot_ret    << "</td>"
     << "<td align=\"right\">" << tot_chd    << "</td>"
     << "</table>"
     << endl;
}

//--------------------------------------------------------------------
void Anp::Exec::WriteALGTable(HtmlNode &anode, const ExecClient &ros, const Exec::Summary &sum, const double scale){
  //
  // Write out results
  //
  ostream &os = anode.GetCont();

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
     << "<th> Request rate (Hz) </th>"
     << "<th> Retrieval rate (Hz) </th>"
     << "<th> Requested bandwidth (kB/s) </th>"
     << "<th> Retrieved bandwidth (kB/s) </th>"
     << "<th> Requests/call</th>"
     << "<th> Retrievals/call</th>"
     << "<th> ROB/request</th>"
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

  uint64_t total_nreq = 0.0, total_nret = 0.0;
  uint64_t total_sreq = 0.0, total_sret = 0.0;

  for(unsigned i = 0; i < counts.size(); ++i) {
    const Exec::Count &ac = counts.at(i);
    
    const string str_req_size = Anp::Round2Pair(Exec::ConvertWordsToKB(ac.rob_size),       0.1).first;
    const string str_ret_size = Anp::Round2Pair(Exec::ConvertWordsToKB(ac.rob_size_retrv), 0.1).first;
    
    double req_evnt  = 0.0, ret_evnt = 0.0;
    double req_call  = 0.0, ret_call = 0.0;
    double rob_preq  = 0.0;

    if(ac.nevent > 0) {
      req_evnt = ac.ros_nreq      /double(ac.nevent);
      ret_evnt = ac.ros_nreq_retrv/double(ac.nevent);
    }
    if(ac.alg_ncalls > 0) {
      req_call = ac.ros_nreq      /double(ac.alg_ncalls);
      ret_call = ac.ros_nreq_retrv/double(ac.alg_ncalls);
    }
    if(ac.rob_nreq > 0) {
      rob_preq = ac.rob_nids/double(ac.rob_nreq);
    }
      
    const string str_req_evnt = Anp::Round2Pair(req_evnt, 0.1).first;
    const string str_ret_evnt = Anp::Round2Pair(ret_evnt, 0.1).first;

    const string str_req_call = Anp::Round2Pair(req_call, 0.1).first;
    const string str_ret_call = Anp::Round2Pair(ret_call, 0.1).first;  

    const string str_req_rate = Anp::Round2Pair(scale*double(ac.ros_nreq),       0.1).first;  
    const string str_ret_rate = Anp::Round2Pair(scale*double(ac.ros_nreq_retrv), 0.1).first; 

    const string str_req_band = Anp::Round2Pair(scale*Exec::ConvertWordsToKB(ac.rob_size), 0.1).first;  
    const string str_ret_band = Anp::Round2Pair(scale*Exec::ConvertWordsToKB(ac.rob_size_retrv), 0.1).first;  
    
    const string str_rob_preq = Anp::Round2Pair(rob_preq, 1.0).first;

    os << "<tr>" 
       << "<td align=\"left\">"  << sum.GetAlgName(ac.alg_id) << "</td>"
       << "<td align=\"right\">" << ac.ros_nreq               << "</td>"
       << "<td align=\"right\">" << ac.ros_nreq_retrv         << "</td>"
       << "<td align=\"right\">" << str_req_size              << "</td>"
       << "<td align=\"right\">" << str_ret_size              << "</td>"
       << "<td align=\"right\">" << str_req_rate              << "</td>"
       << "<td align=\"right\">" << str_ret_rate              << "</td>"
       << "<td align=\"right\">" << str_req_band              << "</td>"
       << "<td align=\"right\">" << str_ret_band              << "</td>"
       << "<td align=\"right\">" << str_req_call              << "</td>"
       << "<td align=\"right\">" << str_ret_call              << "</td>"
       << "<td align=\"right\">" << str_rob_preq              << "</td>"
       << "</tr>"
       << endl;

    total_nreq     += ac.ros_nreq;
    total_nret     += ac.ros_nreq_retrv;
    total_sreq     += ac.rob_size;
    total_sret     += ac.rob_size_retrv;
  }

  const string tot_sreq = Anp::Round2Pair(Exec::ConvertWordsToKB(total_sreq), 1.0).first;
  const string tot_sret = Anp::Round2Pair(Exec::ConvertWordsToKB(total_sret), 1.0).first;

  const string tot_req_rate = Anp::Round2Pair(scale*total_nreq, 1.0).first;
  const string tot_ret_rate = Anp::Round2Pair(scale*total_nret, 1.0).first;
  const string tot_req_band = Anp::Round2Pair(scale*Exec::ConvertWordsToKB(total_sreq), 1.0).first;
  const string tot_ret_band = Anp::Round2Pair(scale*Exec::ConvertWordsToKB(total_sret), 1.0).first;

  os << "<tr>" 
     << "<td align=\"left\">Total</td>"
     << "<td align=\"right\">" << total_nreq   << "</td>"
     << "<td align=\"right\">" << total_nret   << "</td>"
     << "<td align=\"right\">" << tot_sreq     << "</td>"
     << "<td align=\"right\">" << tot_sret     << "</td>"
     << "<td align=\"right\">" << tot_req_rate << "</td>"
     << "<td align=\"right\">" << tot_ret_rate << "</td>"
     << "<td align=\"right\">" << tot_req_band << "</td>"
     << "<td align=\"right\">" << tot_ret_band << "</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << endl;
  
  os << "</table><pre>"<< endl;
}

//--------------------------------------------------------------------
void Anp::Exec::WriteALGBYCHNTable(HtmlNode &anode, const ExecClient &ros, const Exec::Summary &sum, const double scale, const double timeout, const unsigned level){
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
     << "<th> Algorithm Calls </th>"
     << "<th> Algorithm Caches </th>"
     << "<th> Alg Calls/Event </th>"
     << "<th> Alg Caches/Event </th>"
     << "<th> Alg Time (ms)/Event </th>"
     << "<th> Alg Time (ms)/Call </th>"
     << "<th> Call Rate (Hz) </th>"
     << "<th> Calls > " << Anp::Round2Pair(timeout, 10.0).first << " ms </th>"
     << "<th> Total Alg Time (s) </th>"
     << "<th> Total Alg Time (%) </th>";

  if(level == 0 || level == 2) {
    os << "<th> ROS request rate (Hz)</th>"
       << "<th> ROS retrieval rate (Hz)</th>";
  }

  os << "</tr>" << endl;
  
  const double sc = scale;

  vector<Exec::Count> counts;

  const std::map<uint32_t, Exec::Count> &chn_map = ros.GetChnCount();

  for(std::map<uint32_t, Exec::Count>::const_iterator it = chn_map.begin(); it != chn_map.end(); ++it) {
    Exec::Count c = it->second;
    c.chn_id      = it->first;    
    counts.push_back(c);
  }

  //
  // Sort chains by total execution time
  //
  std::sort(counts.begin(), counts.end(), SortExecCount("timer_alg"));

  for(unsigned i = 0; i < counts.size(); ++i) {

    const Exec::Count &count = counts.at(i);

    //
    // Find web page for current algorithm
    //
    string chn_link = sum.GetChnName(count.chn_id);

    //
    // Compute helper variables
    //
    double time_frac = 0.0;
    double call_per_evt = 0.0;
    double cach_per_evt = 0.0;

    if(sum.total_time > 0.0) {
      time_frac = 100.0*count.timer_alg/sum.total_time;
    }

    if(count.nevent>0){
      call_per_evt=static_cast<double>(count.alg_ncalls)/static_cast<double>(count.nevent);
      cach_per_evt=static_cast<double>(count.alg_ncache)/static_cast<double>(count.nevent);
    }

    const string chain_rate_str = Anp::Round2Pair(static_cast<double>(count.nevent)*sc,       1.0).first;
    const string time_totl_str  = Anp::Round2Pair(count.timer_alg/1000.0,                     1.0).first;
    const string time_frac_str  = Anp::Round2Pair(time_frac,                                  0.1).first;
    const string call_pevt_str  = Anp::Round2Pair(call_per_evt,                               0.1).first;
    const string cach_pevt_str  = Anp::Round2Pair(cach_per_evt,                               0.1).first;

    //
    // Write HTML table entries
    //
    os << "<tr>" 
       << "<td align=\"left\"> "   << chn_link                                    << " </td>"
       << "<td align=\"center\"> " << count.nevent                                << " </td>"
       << "<td align=\"center\"> " << count.alg_ncalls                            << " </td>"
       << "<td align=\"center\"> " << count.alg_ncache                            << " </td>"
       << "<td align=\"center\"> " << call_pevt_str                               << " </td>"
       << "<td align=\"center\"> " << cach_pevt_str                               << " </td>"
       << "<td align=\"center\"> " << count.GetMeanAlgTimePerEvent()              << " </td>"
       << "<td align=\"center\"> " << count.GetMeanAlgTimePerCall()               << " </td>"
       << "<td align=\"center\"> " << chain_rate_str                              << " </td>"
       << "<td align=\"center\"> " << count.chn_nslow                             << " </td>"
       << "<td align=\"center\"> " << time_totl_str                               << " </td>"
       << "<td align=\"center\"> " << time_frac_str                               << " </td>";

    if(level == 0 || level == 2) {

      const string rate_nreq_str = Anp::Round2Pair(count.ros_nreq*sc,       10.0).first;
      const string rate_nret_str = Anp::Round2Pair(count.ros_nreq_retrv*sc, 10.0).first;

      os << "<td align=\"center\"> " << rate_nreq_str << " </td>"
	 << "<td align=\"center\"> " << rate_nret_str << " </td>";
    }

    os << "</tr>" << endl;
  }
  
  os << "</table>" << endl;

}

//--------------------------------------------------------------------
void Anp::Exec::WriteCHNBYROSTable(HtmlNode &anode, const ExecClient &chn, const Exec::Summary &sum, const double scale, const string type){
  //
  // Write execution summary for single algorithm
  //
  ostream &os = anode.GetCont();

  //
  // Write algorithm summary
  //
  const Exec::Count &c = chn.GetCount();

  //
  // Write ROS summary
  //
  os << "<style type=\"text/css\">"<< endl
     << "table,  th {border:1px solid black;}"<< endl
     << "th { padding:10px; background-color:white; color:black; }"<< endl
     << "td {padding-left:15px;padding-right:15px;}"<< endl
     << "</style>"<< endl
     << "<script src=\"sorttable.js\"></script>" << endl
     << "<table border=1 frame=box rules=cols class=\"sortable\">" << endl
     << "<tr>"
     << "<th> " << type << " </th>"
     << "<th> Request rate (Hz) </th>"
     << "<th> Retrieval rate (Hz) </th>" 
     << "<th> Request data rate (kB/s) </th>"
     << "<th> Retrieval data rate (kB/s) </th>"
     << "<th> Requests </th>"
     << "<th> Retrievals</th>"
     << "<th> Requested data (kB) </th>"
     << "<th> Retrieved data (kB) </th>"
     << "<th> Requests/evt</th>"
     << "<th> Retrievals/evt</th>"
     << "</tr>" 
     << endl;
  
  const double sc = scale;

  const std::map<uint32_t, Exec::Count> &ros_map = chn.GetROSCount(); 
  const std::map<uint32_t, Exec::Count> &chn_map = chn.GetChnCount(); 

  vector<Exec::Count> counts;
  
  if(type=="ROS"){
    for(map<uint32_t, Exec::Count>::const_iterator it = ros_map.begin(); it != ros_map.end(); ++it) {
      Exec::Count c = it->second;
      c.ros_id      = it->first;    
      counts.push_back(c);
    }
  }
  else if(type=="Chain"){
    for(map<uint32_t, Exec::Count>::const_iterator it = chn_map.begin(); it != chn_map.end(); ++it) {
      Exec::Count c = it->second;
      c.chn_id      = it->first;    
      counts.push_back(c);
    }
  }

  //
  // Sort ROS by number of retrieved ROBs
  //
  std::sort(counts.begin(), counts.end(), SortExecCount("rob_nreq_retrv"));

  uint64_t total_rob_sreq = 0, total_rob_sret = 0;
  uint64_t total_ros_nreq = 0, total_ros_nret = 0;

  for(unsigned i = 0; i < counts.size(); ++i) {
    const Exec::Count &rc = counts.at(i);
    
    string rosname = "N/A";

    if(type.find("ROS")   != string::npos) rosname=sum.GetROSName(rc.ros_id);
    if(type.find("Chain") != string::npos) rosname=sum.GetChnName(rc.chn_id);

    const string str_sreq = Anp::Round2Pair(Exec::ConvertWordsToKB(rc.rob_size),       1.0).first;
    const string str_sret = Anp::Round2Pair(Exec::ConvertWordsToKB(rc.rob_size_retrv), 1.0).first;
    
    const string str_nreq_rate = Anp::Round2Pair(sc*rc.rob_nreq,       1.0).first;
    const string str_nret_rate = Anp::Round2Pair(sc*rc.rob_nreq_retrv, 1.0).first;

    const string str_sreq_rate = Anp::Round2Pair(sc*Exec::ConvertWordsToKB(rc.rob_size),       1.0).first;
    const string str_sret_rate = Anp::Round2Pair(sc*Exec::ConvertWordsToKB(rc.rob_size_retrv), 1.0).first;        

    double req_evnt = 0.0, ret_evnt = 0.0;

    if(c.nevent > 0) {
      req_evnt = rc.ros_nreq      /double(c.nevent);
      ret_evnt = rc.ros_nreq_retrv/double(c.nevent);
    }

    const string str_req_evnt = Anp::Round2Pair(req_evnt, 0.1).first;
    const string str_ret_evnt = Anp::Round2Pair(ret_evnt, 0.1).first;    

    os << "<tr>" 
       << "<td align=\"left\"><nobr>" << rosname       << "</nobr></td>"
       << "<td align=\"right\">" << str_nreq_rate      << "</td>"
       << "<td align=\"right\">" << str_nret_rate      << "</td>"
       << "<td align=\"right\">" << str_sreq_rate      << "</td>"
       << "<td align=\"right\">" << str_sret_rate      << "</td>"
       << "<td align=\"right\">" << rc.ros_nreq        << "</td>"
       << "<td align=\"right\">" << rc.ros_nreq_retrv  << "</td>"
       << "<td align=\"right\">" << str_sreq           << "</td>"
       << "<td align=\"right\">" << str_sret           << "</td>"  
       << "<td align=\"right\">" << str_req_evnt       << "</td>"
       << "<td align=\"right\">" << str_ret_evnt       << "</td>"    
       << endl;


    total_ros_nreq += rc.ros_nreq;
    total_ros_nret += rc.ros_nreq_retrv;

    total_rob_sreq += rc.rob_size;
    total_rob_sret += rc.rob_size_retrv;
  }

  //
  // Write totals
  //
  const string str_nreq      = Anp::Round2Pair(total_ros_nreq, 1.0).first;
  const string str_nret      = Anp::Round2Pair(total_ros_nret, 1.0).first;
  
  const string str_nreq_rate = Anp::Round2Pair(sc*total_ros_nreq, 1.0).first;
  const string str_nret_rate = Anp::Round2Pair(sc*total_ros_nret, 1.0).first;

  const string str_sreq      = Anp::Round2Pair(Exec::ConvertWordsToKB(total_rob_sreq), 1.0).first;
  const string str_sret      = Anp::Round2Pair(Exec::ConvertWordsToKB(total_rob_sret), 1.0).first;

  const string str_sreq_rate = Anp::Round2Pair(sc*Exec::ConvertWordsToKB(total_rob_sreq), 1.0).first;
  const string str_sret_rate = Anp::Round2Pair(sc*Exec::ConvertWordsToKB(total_rob_sret), 1.0).first;

  double total_req_evnt = 0.0;
  double total_ret_evnt = 0.0;
  
  if(c.nevent > 0) {
    total_req_evnt = total_ros_nreq/double(c.nevent);
    total_ret_evnt = total_ros_nret/double(c.nevent);
  }
  
  const string tot_req_evnt = Anp::Round2Pair(total_req_evnt, 0.1).first;
  const string tot_ret_evnt = Anp::Round2Pair(total_ret_evnt, 0.1).first;  

  os << "<tr>" 
     << "<td align=\"left\">Total</td>"
     << "<td align=\"right\">" << str_nreq_rate << "</td>"
     << "<td align=\"right\">" << str_nret_rate << "</td>"
     << "<td align=\"right\">" << str_sreq_rate << "</td>"
     << "<td align=\"right\">" << str_sret_rate << "</td>"
     << "<td align=\"right\">" << str_nreq      << "</td>"
     << "<td align=\"right\">" << str_nret      << "</td>"
     << "<td align=\"right\">" << str_sreq      << "</td>"
     << "<td align=\"right\">" << str_sret      << "</td>"
     << "<td align=\"right\">" << tot_req_evnt  << "</td>"
     << "<td align=\"right\">" << tot_ret_evnt  << "</td>"
     << endl;
  
  os << "</table>" << endl;
}
