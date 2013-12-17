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
#include "TrigCostExec/ExecTableDet.h"
#include "TrigCostExec/ExecWrite.h"
#include "TrigCostExec/RobService.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableDet)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecTableDet::ExecTableDet()
  :fDir(0),
   fCanvas(0),
   fDebug(true),
   fFillNode(false),
   fMakeHist(false),
   fDirName(),
   fEvents(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableDet::~ExecTableDet()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableDet::SetReg(const Registry &reg)
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
  reg.Get("HistOpt",     fHistOpt);
  reg.Get("MakeHist",    fMakeHist);
  reg.Get("ShortClient", fShortClient);
  reg.Get("DirName",     fDirName);
  reg.Get("FillNode",    fFillNode);
  reg.Get("ClientLink",  fClientLink);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }



  //
  // Intializing ExecClient object
  //
  Registry new_reg(fReg);
  new_reg.Set("DoAlg", false);
  new_reg.Set("DoDet", true);
  new_reg.Set("DoROS", false);
  new_reg.Set("DoRoi", false);
  new_reg.Set("DoChn", false);

  fExecDet = Handle<ExecClient>(new ExecCount());
  
  fExecDet->SetClientName("subdetectors");
  fExecDet->SetClientType("");
  fExecDet->SetReg(new_reg);

}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableDet::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableDet::ProcessAlg(const Exec::AlgCall &alg)
{
  //
  // Count one algorithm execution
  //
  fExecDet->ProcessAlg(alg);
    
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableDet::FinishEvent(const Exec::Event &event)
{
  //
  // Count one event
  //
  fExecDet->FinishEvent(event);
  ++fEvents;
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::ExecTableDet::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------
void Anp::ExecTableDet::WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum)
{
  //
  // Write out results for entire run (regardless of lumi block)
  //
  if(fDebug) log() << "WriteExec - begin" << endl;
  ostream &os = anode.GetCont();
  os << "Total Number of events used is " << fEvents << endl
     << endl << "Table shows summary for data requests and retrievals per Subdetector:";

  Exec::WriteDETTable(anode, *fExecDet.get(), sum, 0.0);
  
  //
  // Write execution summary for single algorithm
  //
  os << endl << "Table shows summary for data requests and retrievals per Subdetector:" << endl;
  const group_map &det_map = fExecDet->GetDetCount();
  WriteDet(anode,det_map,sum,"subdet");

  os << endl << "Table shows summary for data requests and retrievals per Subdetector group:" << endl;
  const group_map &grp_map = fExecDet->GetGrpCount();
  for(group_map::const_iterator grp = grp_map.begin(); grp!=grp_map.end(); ++grp){
    group_map::iterator new_grp = fGroupMap.find(grp->first);
    
    if(new_grp == grp_map.end()) continue;
    //new_grp->second.AddCount(grp->second);
    new_grp->second.nevent = grp->second.nevent;
  }
  WriteDet(anode,fGroupMap,sum,"group");

  if(!fMakeHist){
    return;
  }
  //
  // Make subdetector plots
  //
  string suff;  
  if     (fHistOpt.find("png") != string::npos) suff = ".png";
  else if(fHistOpt.find("eps") != string::npos) suff = ".eps";

  //
  // Print histograms and insert links
  //
  if(fCanvas) {
    fCanvas->cd();
  }

  const unsigned nSubDet = det_map.size();
  const unsigned nGroup  = grp_map.size();

  TH1 *hret_perc_sd = MakeHist("hret_percent_vs_subdet",    "ROBs Retrieved (%)",                1, nSubDet, fDir);
  TH1 *hret_pevt_sd = MakeHist("hret_pevt_vs_subdet",       "ROB Requests/event",                1, nSubDet, fDir);
  TH1 *hret_size_sd = MakeHist("hret_size_vs_subdet",       "Request size (kB/event)",           1, nSubDet, fDir);
  TH1 *hret_perc_gp = MakeHist("hret_percent_vs_subdetgrp", "ROBs Retrieved (%)",                1, nGroup,  fDir);
  TH1 *hret_pevt_gp = MakeHist("hret_pevt_vs_subdetgrp",    "ROB Requests/event",                1, nGroup,  fDir);
  TH1 *hret_size_gp = MakeHist("hret_size_vs_subdetgrp",    "Request size (kB/event)",           1, nGroup,  fDir);

  // Subdetector plots
  unsigned i=1;
  double   nEvts = 0.0, nrobs = 0.0;
  const RobService &rob_svc = RobService::Instance();
  for(group_map::const_iterator dit = det_map.begin(); dit != det_map.end(); ++dit) {
    const uint32_t det_id = dit->first;
    const Exec::Count &dc = dit->second;

    nEvts = double(dc.nevent);
    nrobs = double(rob_svc.GetNumberROBsInDet(det_id));    
    const char *detname = (char*)sum.GetDetName(det_id).c_str();
    if(nEvts==0.0) nEvts=1.0;
    if(nrobs==0.0) nrobs=1.0;

    if(hret_perc_sd){
      hret_perc_sd->SetBinContent(i,100.0*double(dc.rob_nids_retrv)/nEvts/nrobs);
      hret_perc_sd->GetXaxis()->SetBinLabel(i,detname);
    }
    if(hret_pevt_sd){
      hret_pevt_sd->SetBinContent(i,double(dc.rob_nids_retrv)/nEvts);
      hret_pevt_sd->GetXaxis()->SetBinLabel(i,detname);
    }
    if(hret_size_sd){
      hret_size_sd->SetBinContent(i,Exec::ConvertWordsToKB(dc.rob_size_retrv)/nEvts);
      hret_size_sd->GetXaxis()->SetBinLabel(i,detname);
    }
    ++i;
  }

  // Subdetector Group plots
  i=1;
  nEvts = 0.0; nrobs = 0.0;
  for(group_map::const_iterator dit = fGroupMap.begin(); dit != fGroupMap.end(); ++dit) {
    const uint32_t det_id = dit->first;
    const Exec::Count &dc = dit->second;
    
    nEvts = double(dc.nevent);
    nrobs = double(rob_svc.GetNumberROBsInGroup(det_id));  

    //std::string to char conversion
    const string str_detname = rob_svc.GetGroupFromId(det_id);
    char *detname=new char[str_detname.size()+1];
    detname[str_detname.size()]=0;
    memcpy(detname,str_detname.c_str(),str_detname.size());

    if(nEvts==0.0) nEvts=1.0;
    if(nrobs==0.0) nrobs=1.0;

    if(hret_perc_gp){
      hret_perc_gp->SetBinContent(i,100.0*double(dc.rob_nids_retrv)/nrobs/nEvts);
      hret_perc_gp->GetXaxis()->SetBinLabel(i,detname);
    }
    if(hret_pevt_gp){
      hret_pevt_gp->SetBinContent(i,double(dc.rob_nids_retrv)/nEvts);
      hret_pevt_gp->GetXaxis()->SetBinLabel(i,detname);
    }
    if(hret_size_gp){
      hret_size_gp->SetBinContent(i,Exec::ConvertWordsToKB(dc.rob_size_retrv)/nEvts);
      hret_size_gp->GetXaxis()->SetBinLabel(i,detname);
    }
    ++i;
  }
    
  os << "Request Size Histograms by Subdetector: " << endl;
  if(hret_perc_sd) Anp::PrintHist(anode, hret_perc_sd, fCanvas, anode.GetBase()+"_hret_percent_vs_subdet"+suff,
				  "  Percentage of ROBs Retrieved/event Binned by Sub-Detector", fHistOpt);
  if(hret_pevt_sd) Anp::PrintHist(anode, hret_pevt_sd, fCanvas, anode.GetBase()+"_hret_pevt_vs_subdet"+suff,
				  "  ROBs Retrieved/event Binned by Sub-Detector", fHistOpt);
  if(hret_size_sd) Anp::PrintHist(anode, hret_size_sd, fCanvas, anode.GetBase()+"_hret_size_vs_subdet"+suff,
				  "  ROB Size Retrieved/event Binned by Sub-Detector", fHistOpt);

  os << "Request Size Histograms by Subdetector Group: " << endl;
  if(hret_perc_gp) Anp::PrintHist(anode, hret_perc_gp, fCanvas, anode.GetBase()+"_hret_percent_vs_subdetgrp"+suff,
				  "  Percentage of ROBs Retrieved/event Binned by Sub-Detector Group", fHistOpt);
  if(hret_pevt_gp) Anp::PrintHist(anode, hret_pevt_gp, fCanvas, anode.GetBase()+"_hret_pevt_vs_subdetgrp"+suff,
				  "  ROBs Retrieved/event Binned by Sub-Detector Group", fHistOpt);
  if(hret_size_gp) Anp::PrintHist(anode, hret_size_gp, fCanvas, anode.GetBase()+"_hret_size_vs_subdetgrp"+suff,
				  "  ROB Size Retrieved/event Binned by Sub-Detector Group", fHistOpt);

}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableDet::WriteDet(HtmlNode &anode, const group_map &det_map, const Exec::Summary &sum,const std::string &KEY)
{

  ostream &os = anode.GetCont();
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
     << "<th> Sub-detector " << KEY << " </th>"
     << "<th> Events </th>"
     << "<th> Total # ROB_IDs </th>"
     << "<th> ROB_IDs requested/Event </th>"
     << "<th> ROB_IDs Retrieved/Event </th>"
     << "<th> Percentage ROB_IDs Retrieved/(Retrieved Event) </th>"
     << "<th> Percentage ROB_IDs Retrieved/(L2 Event) </th>"
     << "<th> Requested data (kB)/Event </th>"
     << "<th> Retrieved data (kB)/Event </th>"
     << "<th> Cached data (kB)/Event </th>"
     << "</tr>" 
     << endl;

  const RobService &rob_svc = RobService::Instance();
  
  double total_evts = 0.0, nEvts = 0.0, nrobs = 0.0;

  for(map<uint32_t, Exec::Count>::const_iterator dit = det_map.begin(); dit != det_map.end(); ++dit) {
    const uint32_t det_id = dit->first;
    const Exec::Count &dc = dit->second;
    string detname = "N/A";

    if(KEY=="subdet"){
      AddGroup(rob_svc.GetGroupIdFromSubDetId(det_id),dc);
      nrobs = double(rob_svc.GetNumberROBsInDet(det_id));    
      detname = sum.GetDetName(det_id);
    }
    if(KEY=="group"){
      nrobs = double(rob_svc.GetNumberROBsInGroup(det_id));    
      detname = rob_svc.GetGroupFromId(det_id);
    }

    nEvts = double(dc.nevent);

    if(nEvts==0.0)   nEvts  =1.0;
    if(nrobs==0.0)   nrobs  =1.0;
    if(fEvents==0.0) fEvents=1.0;
    total_evts += dc.nevent;
    
    const string str_nrq = Anp::Round2Pair(double(dc.rob_nids)                      /nEvts, 1.0).first;
    const string str_nrt = Anp::Round2Pair(double(dc.rob_nids_retrv)                /nEvts, 1.0).first;
    const string str_per = Anp::Round2Pair(100.0*double(dc.rob_nids_retrv)/nrobs    /nEvts, 1.0).first;
    const string str_ppe = Anp::Round2Pair(100.0*double(dc.rob_nids_retrv)/nrobs  /fEvents, 1.0).first;
    const string str_req = Anp::Round2Pair(Exec::ConvertWordsToKB(dc.rob_size)      /nEvts, 1.0).first;
    const string str_ret = Anp::Round2Pair(Exec::ConvertWordsToKB(dc.rob_size_retrv)/nEvts, 1.0).first;
    const string str_chd = Anp::Round2Pair(Exec::ConvertWordsToKB(dc.rob_size_cache)/nEvts, 1.0).first;
    
    os << "<tr>" 
       << "<td align=\"left\">"  << detname    << "</td>"
       << "<td align=\"left\">"  << dc.nevent  << "</td>"
       << "<td align=\"left\">"  << int(nrobs) << "</td>"
       << "<td align=\"right\">" << str_nrq    << "</td>"
       << "<td align=\"right\">" << str_nrt    << "</td>"
       << "<td align=\"right\">" << str_per    << "</td>"
       << "<td align=\"right\">" << str_ppe    << "</td>"
       << "<td align=\"right\">" << str_req    << "</td>"
       << "<td align=\"right\">" << str_ret    << "</td>"
       << "<td align=\"right\">" << str_chd    << "</td>"
       << endl;

  }
  
  os << "<tr>" 
     << "<td align=\"left\">Total</td>"
     << "<td align=\"right\">" << total_evts << "</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "<td align=\"right\">N/A</td>"
     << "</table>"
     << endl;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableDet::AddGroup(uint32_t group_id,const Exec::Count &count)
{
  //
  // Add subdetector counts to the correct group
  //
  group_map::iterator gc = fGroupMap.find(group_id);

  if(gc==fGroupMap.end()){
    fGroupMap.insert(group_map::value_type(group_id, count));
    gc = fGroupMap.find(group_id);
  }

  gc->second.AddCount(count);
}

//--------------------------------------------------------------------------------------      
TH1* Anp::ExecTableDet::MakeHist(const std::string &hname, 
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
  h->GetXaxis()->SetTitle("Sub-Detector Readout");
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(yaxis.c_str());  

  return h;
}

//--------------------------------------------------------------------------------------      
std::ostream& Anp::ExecTableDet::log() const
{
  std::cout << GetClientName() << "::";
  return std::cout;
}
