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

// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilTrig.h"

// Local
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/SurveyLBs.h"
#include "TrigCostExec/Util.h"

REGISTER_ANP_OBJECT(ExecClient,SurveyLBs)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::SurveyLBs::SurveyLBs()
  :fDir(0),
   fCanvas(0),
   fDebug(true),
   fMakeHist(false),
   fLevel(0),
   fDirName()
{
}

//--------------------------------------------------------------------------------------      
Anp::SurveyLBs::~SurveyLBs()
{
}

//--------------------------------------------------------------------------------------      
void Anp::SurveyLBs::SetReg(const Registry &reg)
{
  //
  // Read configuration
  //
  reg.Get("Debug",      fDebug);
  reg.Get("HistOpt",    fHistOpt);
  reg.Get("MakeHist",   fMakeHist);
  reg.Get("DirName",    fDirName);
  reg.Get("ClientLink", fClientLink);
  reg.Get("Level",      fLevel);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }
}

//--------------------------------------------------------------------------------------      
void Anp::SurveyLBs::SetDir(TDirectory *dir)
{
  //
  // Set directory for histograms
  //
  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::SurveyLBs::ProcessAlg(const Exec::AlgCall &)
{
  //
  // Count one algorithm execution
  //
}

//--------------------------------------------------------------------------------------      
void Anp::SurveyLBs::FinishEvent(const Exec::Event &)
{
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::SurveyLBs::GetCount() const
{
  static Exec::Count count;
  return count;
}

//--------------------------------------------------------------------------------------
void Anp::SurveyLBs::WriteExec(HtmlNode &anode, HtmlMenu &, const Exec::Summary &sum)
{
  if(fDebug) log() << "WriteExec - starting writeexec" << endl;
  //
  // Write out results versus lumi block
  //

  //
  // Find LB range
  //
  pair<unsigned,unsigned> exlbs = GetMaxLB(sum);

  //
  // Create histograms
  //
  TH1 *htime_alg = MakeLBHist("htime_alg_vs_lb", "Mean event time (ms)",           exlbs.first, exlbs.second,fDir);
  TH1 *htime_cpu = MakeLBHist("htime_cpu_vs_lb", "Mean algorithm CPU time (ms)",   exlbs.first, exlbs.second,fDir);
  TH1 *htime_rob = MakeLBHist("htime_rob_vs_lb", "Mean algorithm ROB time (ms)",   exlbs.first, exlbs.second,fDir);
  TH1 *hnRoI     = MakeLBHist("hn_roi_vs_lb",    "Mean RoIs per Event",            exlbs.first, exlbs.second,fDir);
  if(fDebug) log() << "WriteExec - defined histos" << endl;
  //
  // Iterate over lumi blocks
  //
  for(unsigned i = 0; i < sum.sum_lbs.size(); ++i) {
    const Exec::SummaryLB &s = sum.sum_lbs.at(i);
    if(fDebug) log() << "WriteExec - getting execCount" << endl;
    const ExecCount *exec = dynamic_cast<const ExecCount *>(s.client.get());
    if(!exec) {
      log() << "WriteLBs - missing ExecCount algorithm at lb=" << s.lb << endl;
      continue;
    }
    if(fDebug) log() << "WriteExec - GetCount" << endl;
    const Exec::Count &c = exec->GetCount();
    if(fDebug) log() << "WriteExec - Filling histos" << endl;

    //
    // Fill histograms
    //
    if(htime_alg) htime_alg->Fill(s.lb, c.GetMeanAlgTimePerEvent());
    if(htime_cpu) htime_cpu->Fill(s.lb, c.GetMeanCPUTimePerEvent());
    if(htime_rob) htime_rob->Fill(s.lb, c.GetMeanROBTimePerEvent());
    if(hnRoI)     hnRoI    ->Fill(s.lb, c.GetMeanNumRoIsPerEvent());
  }
  if(fDebug) log() << "WriteExec - about to make plots" << endl;
  string suff;  
  if     (fHistOpt.find("png") != string::npos) suff = ".png";
  else if(fHistOpt.find("eps") != string::npos) suff = ".eps";
  
  //
  // Print histograms and insert links
  //
  anode.GetCont() << endl << "Mean event times versus lumi block number:" << endl;

  if(fCanvas) {
    fCanvas->cd();
  }

  Anp::PrintHist(anode, htime_alg, fCanvas, anode.GetBase()+"_time_alg"+suff,
		 "  Mean event time", fHistOpt);

  Anp::PrintHist(anode, htime_cpu, fCanvas, anode.GetBase()+"_time_cpu"+suff,
		 "  Mean algorithm CPU time", fHistOpt);

  Anp::PrintHist(anode, hnRoI, fCanvas, anode.GetBase()+"_nRoI"+suff,
		 "  Mean Number of RoIs", fHistOpt);

  if(fLevel ==0 || fLevel==2) Anp::PrintHist(anode, htime_rob, fCanvas, anode.GetBase()+"_time_rob"+suff,
					     "  Mean ROB request time", fHistOpt);
}

////--------------------------------------------------------------------------------------      
//TH1* Anp::SurveyLBs::MakeHist(const std::string &hname, 
//				  const std::string &yaxis, 
//				  unsigned min_lb, 
//				  unsigned max_lb)
//{
//  //
//  // Create histogram
//  //
//  if(!(min_lb > 0 && min_lb < max_lb)) {
//    return 0;
//  }
//
//  TH1 *h = new TH1F(hname.c_str(), "", max_lb-min_lb+1, min_lb, max_lb+1);
//  h->SetDirectory(fDir);
//  h->SetStats(false);
//  h->GetXaxis()->CenterTitle();
//  h->GetXaxis()->SetTitle("luminosity block");
//  h->GetYaxis()->CenterTitle();
//  h->GetYaxis()->SetTitle(yaxis.c_str());  
//
//  return h;
//}

//--------------------------------------------------------------------------------------      
std::ostream& Anp::SurveyLBs::log() const
{
  std::cout << GetClientName() << "::";
  return std::cout;
}
