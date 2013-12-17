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
#include "TrigCostBase/HistMan.h"

// Local
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/ExecTableFarm.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableFarm)

using namespace std;

//--------------------------------------------------------------------------------------      
void Anp::ExecTableFarm::LumiTime::AddEvent(const Exec::Event &event)
{
  timer_steer += event.timer_steer;
  timer_alg   += event.timer_alg;
  timer_rob   += event.timer_rob;
  
  ++nCost_evt;
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableFarm::ExecTableFarm()
  :fDir(0),
   fCanvas(0),
   fDebug(false),
   fLevel(0),
   fMakeHist(false),
   fTimeout(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableFarm::~ExecTableFarm()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableFarm::SetReg(const Registry &reg)
{
  //
  // Copy configuration
  //
  fReg = reg;
  
  //
  // Read configuration
  //
  reg.Get("Debug",      fDebug);
  reg.Get("Level",      fLevel);
  reg.Get("Timeout",    fTimeout);
  reg.Get("HistOpt",    fHistOpt);
  reg.Get("DirName",    fDirName);
  reg.Get("MakeHist",   fMakeHist);
  reg.Get("ClientLink", fClientLink);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableFarm::SetDir(TDirectory *dir)
{
  //
  // Get directory and create histograms
  //
  if(fDebug) cout << "ExecTableFarm::SetDir - fDirName: " << fDirName << endl; 
  if(!fMakeHist) return;

  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableFarm::ProcessAlg(const Exec::AlgCall &/*alg*/)
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableFarm::FinishEvent(const Exec::Event &event)
{
  //
  // Find the client pointer for run and lb; then process AlgCalls
  //
  LumiTime *ltime = GetLumiTime(event);
  ltime->AddEvent(event);
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::ExecTableFarm::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableFarm::LumiTime* Anp::ExecTableFarm::GetLumiTime(const Exec::Event &event)
{
  //
  // Find run 
  //
  const unsigned run = event.run;
  RunMap::iterator irun = fRun.find(run);
  if(irun == fRun.end()){
    LumiMap lbMap;
    irun = fRun.insert(RunMap::value_type(run,lbMap)).first;
  }

  //
  // Make LumiTime for lumi block
  //
  const unsigned lb = event.lb;
  LumiMap &lumiMap = irun->second;

  LumiMap::iterator ilb = lumiMap.find(lb);
  if(ilb == lumiMap.end()) {
    
    LumiTime lt;
    lt.run = run;
    lt.lb  = lb;

    ilb = lumiMap.insert(LumiMap::value_type(lb, lt)).first;
  }

  return &(ilb->second);
}

//-------------------------------------------------------------------------------------- 
void Anp::ExecTableFarm::WriteExec(HtmlNode &anode, HtmlMenu &/*hmenu*/, const Exec::Summary &sum)
{
  //
  // Write out results
  //
  ostream &os = anode.GetCont();

  os << "<script src=\"sorttable.js\"></script>" << endl
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>"
     << "<th> Run # </th>"
     << "<th> Lumi Block # </th>"
     << "<th> Lumi Block Start Time </th>";
  if(fLevel == 2){
    os << "<th> L2 input rate (kHz) </th>"
       << "<th title='Number of L2 events processed by StudyFarm'> # L2 Cost Events </th>"
       << "<th> Rob Request Time (s) </th>";
  }
  if(fLevel == 3){
    os << "<th> EF input rate (kHz) </th>" 
       << "<th title='Number of EF events processed by StudyFarm'> # EF Cost Events </th>";
  }
  os << "<th title='the actual time for a lumi block'> lb Time (s) </th>"
     << "<th title='the sum of wall time for all algorithms run during the event'> Internal Alg Time (s) </th>"
     << "<th> Steering Time (s) </th>";
  
  if(fLevel == 2){ 
    //
    // Need to multiply the L2 and EF by 10 because alg time is only stored for every tenth event
    //
    os  << "<th title='Estimate of farm usage by lb: calculated by (L1 accept evts)/(L2Evts)*(10*alg wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used (alg estimate) </th>";
    os << "<th title='Estimate of L2 Hlt Farm usage by lb: calculated by (L1 accept evts)/(L2Evts)*(steering wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used (steering estimate) </th>" << endl;
  }
  else if(fLevel == 3){
    os << "<th title='Estimate of Hlt farm usage by lb: calculated by (L2 accept evts)/(EF Cost Evts)*(10*alg wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used (alg estimate) </th>";
    os << "<th title='Estimate of EF Hlt Farm usage by lb: calculated by (L2 accept evts)/(EF Cost Evts)*(steering wall time)/(lbtime*NumL2PUs)'> (%) Hlt Farm Used (steering estimate) </th>" << endl;
  }
  os << "</tr>" << endl;

  const vector<Exec::SummaryLB> &sumLBVec = sum.sum_lbs;
  float nNodes = 0.0;
  if(sum.farm_node){
    nNodes = static_cast<float>(sum.farm_node->size());
  }
  //cout << "nNodes: " << nNodes << endl;
  //
  // looping over runs
  //
  for(RunMap::iterator irun = fRun.begin(); irun != fRun.end(); ++irun){

    const unsigned run = irun->first;
    LumiMap &lumimap = irun->second;
    vector<LumiTime*> lvec;

    //
    // Find LB range
    //
    unsigned minlb = 100000000;
    unsigned maxlb = 0;

    for(LumiMap::iterator ilumi = lumimap.begin(); ilumi != lumimap.end(); ++ilumi){

      const unsigned lb = ilumi->first;
      if(lb >= lvec.size()){
	lvec.insert(lvec.end(),1+lb-lvec.size(), NULL);
      }

      lvec.at(lb) = &(ilumi->second);
      if(lb<minlb) minlb = lb;
      if(lb>maxlb) maxlb = lb;
    }
    //
    // Define histograms
    //
    Exec::FarmHist hFarm;
    if(minlb<maxlb){
      hFarm.hUseVsLBSteer = MakeHist("farm_use_vs_lb_steer","HLT Farm Usage (Steering estimate)",minlb,maxlb);
      hFarm.hUseVsLBAlg   = MakeHist("farm_use_vs_lb_alg","HLT Farm Usage (Alg estimate)",minlb,maxlb);
    }
    if(hFarm.hUseVsLBSteer){ 
      if(fDebug) cout << "ExecTableFarm::WriteExec - steer hlt est valid" << endl;
    }
    else cout << "ExecTableFarm::WriteExec - steer hlt est NOT valid" << endl;

    for(unsigned lb = 0; lb<lvec.size();++lb){
      LumiTime *lumiInfo = lvec.at(lb);
      if(!lumiInfo) continue;

      //
      // Find summary for lb
      //
      Exec::SummaryLB sumLB;
      for(unsigned s=0;s<sumLBVec.size();++s){
	if(sumLBVec.at(s).run == run && sumLBVec.at(s).lb == lb){ 
	  sumLB = sumLBVec.at(s);
	  break;
	}
      }

      //
      // Calculate some variables
      //
      const double sc = sumLB.scale_corr;

      const double trig_rate   = .001*sc*static_cast<float>(lumiInfo->nCost_evt);
      const double timer_lb    = sumLB.lb_time;
      const double timer_rob   = timer_lb*sc*lumiInfo->timer_rob/1000.0;
      const double timer_alg   = timer_lb*sc*lumiInfo->timer_alg/1000.0;
      const double timer_steer = timer_lb*sc*lumiInfo->timer_steer/1000.0;
      const double cpu_avail   = timer_lb*nNodes;

      const std::string trig_rateS    = Anp::Round2Pair(trig_rate, 0.1).first;
      const std::string timer_robS    = Anp::Round2Pair(timer_rob, 0.1).first;
      const std::string timer_lbS     = Anp::Round2Pair(timer_lb, 0.1).first;
      const std::string timer_algS    = Anp::Round2Pair(timer_alg, 1.0).first;
      const std::string timer_steerS  = Anp::Round2Pair(timer_steer, 1.0).first;
      
      std::string cpu_consumS;
      std::string cpu_consum_steerS;

      if(cpu_avail==0.0){
	cpu_consumS       = "N/A";
	cpu_consum_steerS = "N/A";
      }
      else{ 
	double cpu_consum          = 100.0*timer_alg/cpu_avail;
	double cpu_consum_steer    = 100.0*timer_steer/cpu_avail;
	cpu_consumS                = Anp::Round2Pair(cpu_consum, 0.01).first;
	cpu_consum_steerS          = Anp::Round2Pair(cpu_consum_steer, 0.01).first;
	if(hFarm.hUseVsLBSteer) hFarm.hUseVsLBSteer->Fill(lb,cpu_consum_steer);
	if(hFarm.hUseVsLBAlg)   hFarm.hUseVsLBAlg  ->Fill(lb,cpu_consum);
      }

      //
      // Write HTML table entries
      //
      os << "<tr>" 
	 << "<td align=\"left\"> "   << run                                  << " </td>"
	 << "<td align=\"center\"> " << lb                                   << " </td>"
	 << "<td align=\"center\"> " << sumLB.lb_start                       << " </td>"
	 << "<td align=\"center\"> " << trig_rate                            << " </td>";
      
      os  << "<td align=\"center\"> " << lumiInfo->nCost_evt                        << " </td>";
      
      if(fLevel==2) os << "<td align=\"center\"> " << timer_robS                    << " </td>";
      
      os << "<td align=\"center\"> " << timer_lbS                                   << " </td>"
	 << "<td align=\"center\"> " << timer_algS                                  << " </td>"
	 << "<td align=\"center\"> " << timer_steerS                                << " </td>"
	 << "<td align=\"center\"> " << cpu_consumS                                 << " </td>"
	 << "<td align=\"center\"> " << cpu_consum_steerS                           << " </td>"
	 << "</tr>" << endl;
      

    }
    //
    // Clear the lumi blocks for the next run
    //
    lvec.clear();
    if(fMakeHist) hFarmMap.insert(FarmHistMap::value_type(run,hFarm));
  }
  
  os << "</table>" << endl;

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

  //
  // Writing histograms for chain
  //
  if(fCanvas) {
    fCanvas->cd();
  }

  os << "<pre>" << endl 
     << endl 
     << "Histograms for HLT Farm Usage:" 
     << endl; 
  for(FarmHistMap::iterator hist = hFarmMap.begin(); hist != hFarmMap.end(); ++hist){
    stringstream runstr;
    runstr << hist->first;
    Anp::PrintHist(anode, hist->second.hUseVsLBAlg, fCanvas, base+runstr.str()+"alg_est_farm_use"+suff,
		   "  Algorithm Estimate of HLT Farm Usage for run "+runstr.str(), fHistOpt);
    Anp::PrintHist(anode, hist->second.hUseVsLBSteer, fCanvas, base+runstr.str()+"steer_est_farm_use"+suff,
		   "  Steering Estimate of HLT Farm Usage for run "+runstr.str(), fHistOpt);
  }
  os << "</pre>" << endl;
}

//--------------------------------------------------------------------------------------      
TH1* Anp::ExecTableFarm::MakeHist(const std::string &hname, 
				  const std::string &yaxis, 
				  const unsigned min_lb, 
				  const unsigned max_lb)
{
  //
  // Create histogram
  //
  if(!(min_lb > 0 && min_lb < max_lb)) {
    return 0;
  }

  TH1 *h = new TH1F(hname.c_str(), "", max_lb-min_lb+1, min_lb, max_lb+1);
  h->SetDirectory(fDir);
  h->SetStats(false);
  h->GetXaxis()->CenterTitle();
  h->GetXaxis()->SetTitle("luminosity block");
  h->GetYaxis()->CenterTitle();
  h->GetYaxis()->SetTitle(yaxis.c_str());  

  return h;
}
