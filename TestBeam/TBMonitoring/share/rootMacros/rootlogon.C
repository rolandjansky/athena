#include <iostream>

#include "TROOT.h"

using namespace std;

void rootlogon() {

  gROOT->SaveContext();
  gROOT->SetStyle("Plain");

  cout << "Executing rootlogon.C" << endl;
  cout << "current include path = " << gSystem->GetIncludePath() << endl;
  
  gROOT->ProcessLine(".L printCanvases.C");
  gROOT->ProcessLine(".L closeCanvases.C");
  gROOT->ProcessLine(".L dateAndTimeLabel.C");

  //gROOT->ProcessLine(".L LArDigitMon.C");
  //gROOT->ProcessLine(".L LArCalibDigitMon.C");
  gROOT->ProcessLine(".L TBPhase.C");
  gROOT->ProcessLine(".L TBTailCatcher.C");
  gROOT->ProcessLine(".L TBADC.C");
  gROOT->ProcessLine(".L TBBPC.C");
  gROOT->ProcessLine(".L TBTDC.C");
  gROOT->ProcessLine(".L TBScintillator.C");

  //gROOT->ProcessLine(".L LArDigitGlobalPlots.C");
  //gROOT->ProcessLine(".L febPlots.C");
  gROOT->ProcessLine(".L TBPhasePlots.C");
  gROOT->ProcessLine(".L TBTailCatcherPlots.C");
  gROOT->ProcessLine(".L TBADCPlots.C");
  gROOT->ProcessLine(".L TBBPCPlots.C");
  gROOT->ProcessLine(".L TBTDCPlots.C");
  gROOT->ProcessLine(".L TBScintillatorPlots.C");
  gROOT->ProcessLine(".L TBScintillatorPlots_2.C");

  //gROOT->ProcessLine(".L LArCalibDigitMon.C");

  gROOT->Macro("TBEventMoniButtons.C");
}
     
