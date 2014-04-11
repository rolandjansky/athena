////////////////////////////////////////////////////////////
// TrigPattern.C
// 
// last modified 2004/07/02
//
// Plotting macro histograms of trigger pattern units
////////////////////////////////////////////////////////////

#include <iostream>

#include "TCanvas.h"
#include "TROOT.h"
#include "TString.h"

using namespace std;

void TrigPattern(TString path, TString canvasTitle, TString canvasNameBase) {
  // TString path           directory path
  // TString canvasTitle    each canvas has a title composed of the canvas name followed by canvasTitle
  // TString canvasNameBase each canvas name is composed of the canvasNameBase followed by an index

  if (!gDirectory->cd(path)) {
    TString message = "Path " + path + " not found";
    OKButton(message);
    return;
  }

  TString stringToExecute = "closeCanvases(\"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);

  TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, 400, 10, 600, 462);
  oneCanvas->Divide(1, 2);
  oneCanvas->cd(2);
  gPad->Divide(2, 1);

  oneCanvas->cd(1);
  if (plotHisto("triggpat2", "Trigger Bit Correlations","","","","TEXT")) oneCanvas->Update();
  
  oneCanvas->cd(2);
  gPad->cd(1);
  if (plotHisto("triggpat1", "Trigger Bits")) oneCanvas->Update();

  oneCanvas->cd(2);
  gPad->cd(2);
  TPaveText* pave = new TPaveText(0.01, 0.01, 0.99, 0.99, "NDC");

  pave->AddText("Entry  0 = 0x00000001 :  S1 accept");
  pave->AddText("Entry  1 = 0x00000002 :  S2 accept");
  pave->AddText("Entry  2 = 0x00000004 :  S3 accept");
  pave->AddText("Entry  3 = 0x00000008 :  B2 accept");
  pave->AddText("Entry  4 = 0x00000010 :  VetoLeft");
  pave->AddText("Entry  5 = 0x00000020 :  VetoRight");
  pave->AddText("Entry  6 = 0x00000040 :  VetoUp");             
  pave->AddText("Entry  7 = 0x00000080 :  VetoDown");          
  pave->AddText("Entry  8 = 0x00000100 :  Mu 1 Left");          
  pave->AddText("Entry  9 = 0x00000200 :  Mu 1 Righ");          
  pave->AddText("Entry 10 = 0x00000400 :  Mu 2 Left");         
  pave->AddText("Entry 11 = 0x00000800 :  Early PileUp");       
  pave->AddText("Entry 12 = 0x00001000 :  Late PileUp");        
  pave->AddText("Entry 13 = 0x00002000 :  In Spill Tag");       
  pave->AddText("Entry 14 = 0x00004000 :  Off Spill Tag");      
  pave->AddText("Entry 15 = 0x00008000 :  Cedar 7 of 8 segments");
  pave->AddText("Entry 16 = 0x00010000 :  Mu 2 Right");        
  pave->AddText("Entry 17 = 0x00020000 :  Hole Counter");       
  pave->AddText("Entry 18 = 0x00040000 :  W1*W2 coincidence");  
  pave->AddText("Entry 19 = 0x00080000 :  B1 accept");       
  pave->AddText("Entry 20 = 0x00100000 :  Scaler Reset Tag");   
  pave->AddText("Entry 21 = 0x00200000 :  Cedar 8 of 8 segments");
  
  pave->SetTextSize(0.04);
  pave->SetTextAlign(11);  // bottom left align
  pave->SetBorderSize(0);
  pave->SetMargin(0.04);
  pave->SetFillColor(0);
  pave->Draw();
    
  gDirectory->cd();
}
