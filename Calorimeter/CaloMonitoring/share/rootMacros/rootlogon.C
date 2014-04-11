////////////////////////////////////////////////////////////
// rootlogon.C
// 
// last modified 2004/06/27
//
// Macro that gets run when root is started.  It loads
// the necessary macros and then launches Monibuttons.C
////////////////////////////////////////////////////////////

#include <iostream>

#include "TROOT.h"

using namespace std;

void rootlogon() {

  gROOT->SaveContext();
  gROOT->SetStyle("Plain");

  cout << "Executing rootlogon.C" << endl;
  cout << "current include path = " << gSystem->GetIncludePath() << endl;
  
  // Load all required macros for monitoring
  gROOT->ProcessLine(".L closeCanvases.C");
  gROOT->ProcessLine(".L dateAndTimeLabel.C");
  gROOT->ProcessLine(".L OKButton.C");
  gROOT->ProcessLine(".L printCanvases.C");
  gROOT->ProcessLine(".L printSummary.C");
  gROOT->ProcessLine(".L runNumberLabel.C");

  gROOT->ProcessLine(".L plotHisto.C");
  gROOT->ProcessLine(".L plotList.C");
  gROOT->ProcessLine(".L plotPattern.C");

  gROOT->ProcessLine(".L CaloCellMon.C");
  gROOT->ProcessLine(".L CaloCellBPCMon.C");
  gROOT->ProcessLine(".L LArCalibDigitMon.C");
  gROOT->ProcessLine(".L LArDigitMon.C");
  gROOT->ProcessLine(".L LArWaveFebMon.C");
  gROOT->ProcessLine(".L LArWaveMon.C");
  gROOT->ProcessLine(".L TBADCMon.C");
  gROOT->ProcessLine(".L TBBPCMon.C");
  gROOT->ProcessLine(".L TBEventMon.C");
  gROOT->ProcessLine(".L TBScintillatorMon.C");
  gROOT->ProcessLine(".L TBTailCatcherMon.C");
  gROOT->ProcessLine(".L TBTDCMon.C");

  gROOT->ProcessLine(".L AllWaveSumPlot.C");
  gROOT->ProcessLine(".L CaloCellMonPlots.C");
  gROOT->ProcessLine(".L LArDigitGlobalPlots.C");
  gROOT->ProcessLine(".L TBInfo.C");
  gROOT->ProcessLine(".L TBPhasePlots.C");

  gROOT->ProcessLine(".L TrigPattern.C");
  
  gROOT->Macro("MoniButtons.C");
}
     
