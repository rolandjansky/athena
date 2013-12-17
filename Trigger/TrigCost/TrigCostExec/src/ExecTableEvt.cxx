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
#include "TrigCostExec/ExecTableEvt.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableEvt)

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ExecTableEvt::ExecTableEvt()
  :fDir(0),
   fCanvas(0),
   fDebug(false),
   fLevel(0),
   fMakeHist(false),
   fTimeout(0),
   fSteerTime(0),
   fAlgTime(0),
   fROBTime(0),
   fCPUTime(0),
   fROBRet(0),
   fAlgCall(0),
   fAlgCach(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableEvt::~ExecTableEvt()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableEvt::SetReg(const Registry &reg)
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
  reg.Get("HistKey",    fHistKey);
  reg.Get("ClientLink", fClientLink);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }

  //
  // Collect Scale Algorithm
  //
  string scale_name;
  if(fReg.Get("ScaleBG", scale_name)) {
    fScaleBG = Factory<ScaleABC>::Instance().Get(scale_name);
  }
  else {
    if(fDebug) cout << "ExecTableEvt::" << "Done - scaleBG algorithm name is not set" << endl;
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableEvt::SetDir(TDirectory *dir)
{
  //
  // Get directory and create histograms
  //
  if(fDebug) cout << "ExecTableEvt::SetDir - fDirName: " << fDirName << endl; 
  if(!fMakeHist) return;

  fDir = Anp::GetDir(dir, fDirName);

  fSteerTime = GetTH1("steer_evt_time");
  fAlgTime   = GetTH1("alg_evt_time"  );
  fROBTime   = GetTH1("rob_evt_time"  );
  fCPUTime   = GetTH1("cpu_evt_time"  );
  fROBRet    = GetTH1("rob_evt_ret"   );

  fAlgCall    = GetTH1("alg_evt_call" );
  fAlgCach    = GetTH1("alg_evt_cach" );
  
  if(fROBRet)  fROBRet ->Sumw2();
  if(fAlgCall) fAlgCall->Sumw2();
  if(fAlgCach) fAlgCach->Sumw2();
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableEvt::ProcessAlg(const Exec::AlgCall &alg)
{
  fCount.AddCount(alg);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableEvt::FinishEvent(const Exec::Event &event)
{
  //
  // Collect Event Scale Factor
  //
  double scale_bg = 1.0;
  if(fScaleBG.valid()){
    scale_bg = fScaleBG->GetScaleBCID().scale_lumi;
    if(fDebug) cout << "ExecTableEvt::" << "Done - scaleBG algorithm valid with value: " << scale_bg << endl;
  }
  else{ 
    if(fDebug) cout << "ExecTableEvt::" << "Done - invalid scaleBG! Using default scale: " << scale_bg << endl;
  }

  //
  // Find the client pointer for run and lb; then process AlgCalls
  //
  if(fSteerTime) fSteerTime->Fill(event.timer_steer);
  if(fAlgTime)   fAlgTime  ->Fill(event.timer_alg);
  if(fROBTime)   fROBTime  ->Fill(event.timer_rob);
  if(fCPUTime)   fCPUTime  ->Fill(event.timer_alg-event.timer_rob);
  if(fROBRet)    fROBRet   ->Fill(fCount.rob_nreq_retrv,scale_bg);
  if(fAlgCall)   fAlgCall  ->Fill(fCount.alg_ncalls,scale_bg);
  if(fAlgCach)   fAlgCach  ->Fill(fCount.alg_ncache,scale_bg);

  fCount.Clear();
}

//--------------------------------------------------------------------------------------      
const Anp::Exec::Count& Anp::ExecTableEvt::GetCount() const
{
  return fCount;
}

//-------------------------------------------------------------------------------------- 
void Anp::ExecTableEvt::WriteExec(HtmlNode &anode, HtmlMenu &/*hmenu*/, const Exec::Summary &/*sum*/)
{
  //
  // Write out results
  //
  ostream &os = anode.GetCont();

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
     << "Histograms for Event:" 
     << endl; 

    if(fSteerTime) Anp::PrintHist(anode, fSteerTime, fCanvas, base+"steer_evt_time"+suff,
		   "  Steering Event Time", fHistOpt);
    if(fAlgTime) Anp::PrintHist(anode, fAlgTime, fCanvas, base+"alg_evt_time"+suff,
		   "  Algorithm Event Time", fHistOpt);
    if(fROBTime) Anp::PrintHist(anode, fROBTime, fCanvas, base+"rob_evt_time"+suff,
		   "  ROB Event Time", fHistOpt);
    if(fCPUTime) Anp::PrintHist(anode, fCPUTime, fCanvas, base+"cpu_evt_time"+suff,
		   "  CPU Event Time", fHistOpt);
    if(fROBRet)  Anp::PrintHist(anode, fROBRet,  fCanvas, base+"rob_evt_ret"+suff,
		   "  Event ROS Retrievals", fHistOpt);
    if(fAlgCall) Anp::PrintHist(anode, fAlgCall,  fCanvas, base+"alg_evt_call"+suff,
		   "  Event Algorithm Calls", fHistOpt);
    if(fAlgCach) Anp::PrintHist(anode, fAlgCach,  fCanvas, base+"alg_evt_cach"+suff,
				"  Event Algorithm Caches", fHistOpt);
}

//---------------------------------------------------------------------------------------
TH1* Anp::ExecTableEvt::GetTH1(const std::string &key, const std::string &name) const
{
  //
  // Histograms are owned by TDirectory
  //
  TH1 *h = Anp::HistMan::Instance().CreateTH1(key, fHistKey);

  if(h && !name.empty()) {
    h->SetName(name.c_str());
  }

  Anp::SetDir(h, fDir);

  return h;
}
