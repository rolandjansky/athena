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
#include "TrigCostExec/ExecTableROBIN.h"
#include "TrigCostExec/ExecWrite.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableROBIN)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecTableROBIN::ExecTableROBIN()
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
Anp::ExecTableROBIN::~ExecTableROBIN()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROBIN::SetReg(const Registry &reg)
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
void Anp::ExecTableROBIN::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROBIN::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Count one algorithm execution
  //

  for(set<uint32_t>::const_iterator rit = alg.robin_ids.begin(); rit != alg.robin_ids.end(); ++rit) {
    const uint32_t robin_id = *rit;
    
    ExecClient *client = GetROBINClient(robin_id, alg);
    client->ProcessAlg(alg);
    
    fActiveClients.push_back(client);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableROBIN::FinishEvent(const Exec::Event &event)
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
const Anp::Exec::Count& Anp::ExecTableROBIN::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------
Anp::ExecClient* Anp::ExecTableROBIN::GetROBINClient(unsigned robin_id, const Exec::AlgCall &alg)
{
  //
  // Make client for algorithm
  //
  ROBINMap::iterator irobin = fRobin.find(robin_id);
  if(irobin == fRobin.end()) {

    Registry reg(fReg);

    //
    // Only create alg, roi and robin maps when this is a full summary
    //
    if(!fShortClient){
      reg.Set("DoAlg",   true);
      reg.Set("DoDet",   false);
      reg.Set("DoROS",   false);
      reg.Set("DoRoi",   true);
      reg.Set("DoChn",   true);
      reg.Set("DoROBIN", true);
    }

    reg.Set("ROBINId", robin_id);
    
    if(fDebug) {
      log() << "GetROBINClient - " << alg.GetROBINName(robin_id) << endl;
    }

    Handle<ExecClient> client(new ExecCount());
    irobin = fRobin.insert(ROBINMap::value_type(robin_id, client)).first;
    
    client->SetClientName(alg.GetROBINName(robin_id));
    client->SetClientType("");
    client->SetReg(reg);
    client->SetDir(Anp::GetDir(fDir, alg.GetROBINName(robin_id)));
  }

  return irobin->second.get();
}

//--------------------------------------------------------------------------------------
void Anp::ExecTableROBIN::WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum)
{
  //
  // Write out results for entire run (regardless of lumi block)
  //
  if(fDebug) log() << "WriteExec - begin" << endl;
  ostream &os = anode.GetCont();
  
  os << endl << "Table shows summary for data requests and retrievals per ROBIN:";

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> ROBIN </th>"
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
     << "<th> Time per ROBIN retrieval (ms) </th>"
     << "<th> Time per event (ms) </th>"
     << "<th> ROBs/retrieval </th>"
     << "</tr>" << endl;
  
  ClientVec robines;

  for(ROBINMap::iterator it = fRobin.begin(); it != fRobin.end(); ++it) {
    robines.push_back(it->second.get());
  }

  //
  // Sort algorithms by total execution time
  //
  std::sort(robines.begin(), robines.end(), SortExecClient("rob_nreq_retrv"));    

  for(unsigned i = 0; i < robines.size(); ++i) {

    ExecClient *robin = robines.at(i);

    if(!robin){
      log() << "WriteTableROBIN - invalid pointer for robin card" << endl;
      continue;
    }
    
    const Exec::Count &count = robin->GetCount();
    const string &robin_name = robin->GetClientName();
    const string  robin_base = sum.html_base+"_"+robin_name;

    //
    // Create web page for current ROBIN
    //
    string robin_link = robin_name;

    if(sum.write_full) {      
      Handle<HtmlNode> nodeR(new HtmlNode(robin_base+".html", HtmlNode::kHTML));
      nodeR->SetBase(robin_base);
      anode.AddChild(nodeR);

      WriteROBIN(nodeR.ref(), *robin, sum);
      robin_link = nodeR->GetLink(robin_name);
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
       << "<td align=\"left\"> "   << robin_link                                    << " </td>"
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
void Anp::ExecTableROBIN::WriteROBIN(HtmlNode &rnode, const ExecClient &robin, const Exec::Summary &sum)
{
  if(fDebug) log() << "WriteROBIN - begin" << endl;
  //
  // Write execution summary for single algorithm
  //
  ostream &os = rnode.GetCont();
  os << "<pre>";
  //
  // Write algorithm summary
  //
  const Exec::Count &c = robin.GetCount();

  const string size_total_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size),       1.0).first;
  const string size_retrv_str = Anp::Round2Pair(Exec::ConvertWordsToKB(c.rob_size_retrv), 1.0).first;

  os << "ROBIN:                          " << robin.GetClientName() << endl
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
     << "<th> ROBIN requests</th>"
     << "<th> ROBIN retrievals</th>"
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

  const std::map<uint32_t, Exec::Count> &alg_map = robin.GetAlgCount();
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
      req_evnt = ac.robin_nreq      /double(ac.nevent);
      ret_evnt = ac.robin_nreq_retrv/double(ac.nevent);
    }
    if(ac.alg_ncalls > 0) {
      req_call = ac.robin_nreq      /double(ac.alg_ncalls);
      ret_call = ac.robin_nreq_retrv/double(ac.alg_ncalls);
    }
    if(ac.rob_nreq_retrv > 0) {
      rob_pret = ac.rob_nids_retrv/double(ac.rob_nreq_retrv);
    }
      
    const string str_req_evnt = Anp::Round2Pair(req_evnt, 0.1).first;
    const string str_ret_evnt = Anp::Round2Pair(ret_evnt, 0.1).first;

    const string str_req_call = Anp::Round2Pair(req_call, 0.1).first;
    const string str_ret_call = Anp::Round2Pair(ret_call, 0.1).first;  

    const string str_req_rate = Anp::Round2Pair(sum.scale_corr*double(ac.robin_nreq),       0.1).first;  
    const string str_ret_rate = Anp::Round2Pair(sum.scale_corr*double(ac.robin_nreq_retrv), 0.1).first; 
    const string str_cache_rate = Anp::Round2Pair(sum.scale_corr*double(ac.robin_nreq_cache), 0.1).first;

    const string str_req_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(ac.rob_size), 0.1).first;  
    const string str_ret_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(ac.rob_size_retrv), 0.1).first;  
    const string str_cache_band = Anp::Round2Pair(sum.scale_corr*Exec::ConvertWordsToKB(ac.rob_size_cache), 0.1).first;
    
    const string str_rob_pret = Anp::Round2Pair(rob_pret, 1.0).first;

    os << "<tr>" 
       << "<td align=\"left\">"  << sum.GetAlgName(ac.alg_id) << "</td>"
       << "<td align=\"right\">" << ac.robin_nreq               << "</td>"
       << "<td align=\"right\">" << ac.robin_nreq_retrv         << "</td>"
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

    total_nreq     += ac.robin_nreq;
    total_nret     += ac.robin_nreq_retrv;
    total_ncache   += ac.robin_nreq_cache;
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

  //Exec::WriteCHNBYROBINTable(rnode,robin,sum,sum.scale_corr,"Chain");

  if(!fMakeHist){
    return;
  }
    //
    // Plot ROBs in ROBIN request...scaled rates
    //    - one plot for each ROBIN
    //    - normalized to total request rates
    //
    
    //string suff;  
    //stringstream robin_id;
    //robin_id << c.robin_id;
    //if     (fHistOpt.find("png") != string::npos) suff = ".png";
    //else if(fHistOpt.find("eps") != string::npos) suff = ".eps";
    //
    //Exec::ROBINHist rhe = robin.GetROBINHistEvent();
    //Exec::ROBINHist rhc = robin.GetROBINHistCalls();
    //Exec::ROBINHist rhr = robin.GetROBINHistROI();  
    ////
    //// scale histograms to the correct ROBIN rate
    ////
    //if(rhc.hROBINNrobidsRetrv) rhc.hROBINNrobidsRetrv->Scale(sum.scale_corr);
    //if(rhc.hROBINNrobids)      rhc.hROBINNrobids     ->Scale(sum.scale_corr);
    ////
    //// Print histograms and insert links
    ////
    //if(fCanvas) {
    //  fCanvas->cd();
    //}
    //
    ////
    //// Plot ROBs in ROBIN request...scaled rates
    ////    - one plot for each ROBIN
    ////    - normalized to total request rates
    ////    - plot by roi
    ////    - plot by event
    ////    - plot by alg call
    ////
    //
    //os << "Request Rate Histograms by ROBIN Call: " << endl;
    //if(rhc.hROBINNrobids) Anp::PrintHist(rnode, rhc.hROBINNrobids, fCanvas, rnode.GetBase()+robin_id.str()+"_hret_rate_vs_nROBs_req"+suff,
    //		   "  ROBIN Retrieval Rate Binned by Number of ROBs Requested", fHistOpt);
    //if(rhc.hROBINNrobidsRetrv) Anp::PrintHist(rnode, rhc.hROBINNrobidsRetrv, fCanvas, rnode.GetBase()+robin_id.str()+"_hret_rate_vs_nROBs_ret"+suff,
    //		   "  ROBIN Retrieval Rate Binned by Number of ROBs Retrieved", fHistOpt);
    //
    //os << "Histograms by Events: " << endl;
    //if(rhe.hROBINNreq) Anp::PrintHist(rnode, rhe.hROBINNreq, fCanvas, rnode.GetBase()+robin_id.str()+"_evt_hreq_count"+suff,
    //		   "  ROBIN Number of Requests", fHistOpt);
    //if(rhe.hROBINNreqRetrv) Anp::PrintHist(rnode, rhe.hROBINNreqRetrv, fCanvas, rnode.GetBase()+robin_id.str()+"_evt_hret_count"+suff,
    //		   "  ROBIN Number of Retrievals", fHistOpt);
    //if(rhe.hROBINSize) Anp::PrintHist(rnode, rhe.hROBINSize, fCanvas, rnode.GetBase()+robin_id.str()+"_evt_hreq_size"+suff,
    //		   "  Size Requested (kB) from ROBIN", fHistOpt);
    //if(rhe.hROBINSizeRetrv) Anp::PrintHist(rnode, rhe.hROBINSizeRetrv, fCanvas, rnode.GetBase()+robin_id.str()+"_evt_hret_size"+suff,
    //		   "  Size Retrieved (kB) from ROBIN", fHistOpt);
    //
    //if(fPlots){
    //  os << "Histograms by Algorithm Call: " << endl;
    //  if(rhc.hROBINNreq) Anp::PrintHist(rnode, rhc.hROBINNreq, fCanvas, rnode.GetBase()+robin_id.str()+"_call_hreq_count"+suff,
    //				      "  ROBIN Number of Requests", fHistOpt);
    //  if(rhc.hROBINNreqRetrv) Anp::PrintHist(rnode, rhc.hROBINNreqRetrv, fCanvas, rnode.GetBase()+robin_id.str()+"_call_hret_count"+suff,
    //					   "  ROBIN Number of Retrievals", fHistOpt);
    //  if(rhc.hROBINSize) Anp::PrintHist(rnode, rhc.hROBINSize, fCanvas, rnode.GetBase()+robin_id.str()+"_call_hreq_size"+suff,
    //				      "  Size Requested (kB) from ROBIN", fHistOpt);
    //  if(rhc.hROBINSizeRetrv) Anp::PrintHist(rnode, rhc.hROBINSizeRetrv, fCanvas, rnode.GetBase()+robin_id.str()+"_call_hret_size"+suff,
    //					   "  Size Retrieved (kB) from ROBIN", fHistOpt);
    //  
    //  os << "Histograms by ROI: " << endl;
    //  if(rhr.hROBINNreq) Anp::PrintHist(rnode, rhr.hROBINNreq, fCanvas, rnode.GetBase()+robin_id.str()+"_roi_hreq_count"+suff,
    //				      "  ROBIN Number of Requests", fHistOpt);
    //  if(rhr.hROBINNreqRetrv) Anp::PrintHist(rnode, rhr.hROBINNreqRetrv, fCanvas, rnode.GetBase()+robin_id.str()+"_roi_hret_count"+suff,
    //					   "  ROBIN Number of Retrievals", fHistOpt);
    //  if(rhr.hROBINSize) Anp::PrintHist(rnode, rhr.hROBINSize, fCanvas, rnode.GetBase()+robin_id.str()+"_roi_hreq_size"+suff,
    //				      "  Size Requested (kB) from ROBIN", fHistOpt);
    //  if(rhr.hROBINSizeRetrv) Anp::PrintHist(rnode, rhr.hROBINSizeRetrv, fCanvas, rnode.GetBase()+robin_id.str()+"_roi_hret_size"+suff,
    //					   "  Size Retrieved (kB) from ROBIN", fHistOpt);
    //}
}

//--------------------------------------------------------------------------------------      
std::ostream& Anp::ExecTableROBIN::log() const
{
  std::cout << GetClientName() << "::";
  return std::cout;
}
