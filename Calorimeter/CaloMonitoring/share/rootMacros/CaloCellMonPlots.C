/////////////////////////////////////////////////////////////////////////////
// CaloCellMonPlots.C
// 
// last modified 2004/06/28
//
// This function is called from the CaloCellMon.C macro.
// It takes as arguments the path to the histograms to be plotted,the
// title to give the canvas, and the canvasNameBase for display persistancy.
// Histograms are plotted using plotHisto.C
//
// This code is hardwired for specific path values.
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "TCanvas.h"
#include "TLatex.h"
#include "TString.h"
#include "TROOT.h"

using namespace std;

void CaloCellMonPlots(TString path, TString canvasTitle, TString canvasNameBase) { 
  // TString path           directory path
  // TString canvasTitle    each canvas has a title composed of the canvas name followed by canvasTitle
  // TString canvasNameBase each canvas name is composed of the canvasNameBase followed by an index

  float canvasWidthX = 600;
  float canvasWidthY = 462;
  float canvasX      = 400;
  float canvasY      =  20;
  float canvasStepX  =  20;
  float canvasStepY  =  20;

  if (!gDirectory->cd(path)) {
    TString message = "Path " + path + " not found";
    OKButton(message);
    return;
  }

  //close any open canvases with the same canvasNameBase
  TString stringToExecute = "closeCanvases(\"" + canvasNameBase + "\")";
  gROOT->ProcessLine(stringToExecute);

  //TLatex* labelEta = new TLatex(-0.95,-0.65,"#eta=3.2");
  //TLatex* labelPhi = new TLatex(-0.50,-1.05,"#eta=2.5");

  // viewing the EMEC cell histograms
  // unfortunately, the MonTool labels the EMEC layers the wrong way: EMEC2 is the "first layer" in 2004, not EMEC1
  //  if (path == "/LArCellEMEC") {
  if (canvasNameBase == "cBaseEMEC") {
    TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);
    oneCanvas->Divide(2, 2);
    oneCanvas->cd(1); 
    if (plotHisto("EMECMiddleEnergy", "Energy (MeV)", "", "", "EMEC Sample2 Energy")) oneCanvas->Update();
    oneCanvas->cd(2); 
    if (plotHisto("EMECBackEnergy", "Energy (MeV)", "", "", "EMEC Sample3 Energy")) oneCanvas->Update();
    oneCanvas->cd(3); 
    if (plotHisto("EMECMiddlePosition", "eta", "phi", "", "EMEC Sample2 Phi vs Eta", "LEGO")) {
      //labelEta->Draw(); labelPhi->Draw();
      oneCanvas->Update();
    }
    oneCanvas->cd(4); 
    if (plotHisto("EMECBackPosition", "eta", "phi", "", "EMEC Sample3 Phi vs Eta", "LEGO")) {
      //labelEta->Draw(); labelPhi->Draw();
      oneCanvas->Update();
    }
  }

  canvasX += canvasStepX;
  canvasY += canvasStepY;
  //viewing the HEC cell histograms
  //  if (path=="/LArCellHEC"){
  if ( canvasNameBase == "cBaseHEC"){
    TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);
    oneCanvas->Divide(3, 2);
    oneCanvas->cd(1); 
    if (plotHisto("HEC0energy", "Energy (MeV)", "", "", "HEC Sample0 Energy")) oneCanvas->Update();
    oneCanvas->cd(2); 
    if (plotHisto("HEC1energy", "Energy (MeV)", "", "", "HEC Sample1 Energy")) oneCanvas->Update();
    oneCanvas->cd(3); 
    if (plotHisto("HEC2energy", "Energy (MeV)", "", "", "HEC Sample2 Energy")) oneCanvas->Update();
    oneCanvas->cd(4); 
    if (plotHisto("HEC0Position", "eta", "phi", "", "HEC Sample0 Phi vs Eta", "LEGO")) {
      //labelEta->Draw(); labelPhi->Draw();
      oneCanvas->Update();
    }
    oneCanvas->cd(5); 
    if (plotHisto("HEC1Position", "eta", "phi", "", "HEC Sample1 Phi vs Eta", "LEGO")) {
      //labelEta->Draw(); labelPhi->Draw();
      oneCanvas->Update();
    }
    oneCanvas->cd(6); 
    if (plotHisto("HEC2Position", "eta", "phi", "", "HEC Sample2 Phi vs Eta", "LEGO")) {
      //labelEta->Draw(); labelPhi->Draw();
      oneCanvas->Update();
    }
  }

  canvasX += canvasStepX;
  canvasY += canvasStepY;
  //viewing the FCAL cell histograms
  // if ((path=="/LArCellFCAL") && (canvasTitle=="LAr FCal calorimeter cell")) {
  if (canvasNameBase == "cBaseFCAL" ) {
    TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);
    oneCanvas->Divide(2, 2);
    oneCanvas->cd(1); 
    if (plotHisto("FCAL1energy", "Energy (MeV)", "", "", "FCAL Module1 Energy")) oneCanvas->Update();
    oneCanvas->cd(2); 
    if (plotHisto("FCAL2energy", "Energy (MeV)", "", "", "FCAL Module2 Energy")) oneCanvas->Update();
    oneCanvas->cd(3); 
    if (plotHisto("FCAL1Position", "eta", "phi", "", "FCAL Module1 Phi vs Eta", "LEGO")) {
      //labelEta->Draw(); labelPhi->Draw();
      oneCanvas->Update();
    }
    oneCanvas->cd(4); 
    if (plotHisto("FCAL2Position", "eta", "phi", "", "FCAL Module2 Phi vs Eta", "LEGO")) {
      //labelEta->Draw(); labelPhi->Draw();
      oneCanvas->Update();
    }
  }

  canvasX += canvasStepX;
  canvasY += canvasStepY;
  //viewing the Cold TailCatcher cell histograms
  // if ((path=="/LArCellFCAL") && (canvasTitle=="Cold TailCatcher cell")) {
  if ( canvasNameBase == "cBaseCTC" ) {
    TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);
    oneCanvas->Divide(2, 2);
    oneCanvas->cd(1); 
    if (plotHisto("FCAL3energy", "Energy (MeV)", "", "", "Cold TC Sample3 Energy")) oneCanvas->Update();
    oneCanvas->cd(3); 
    if (plotHisto("FCAL3Position", "iphi", "ieta", "", "Cold TC Sample3 Eta plane", "LEGO")) oneCanvas->Update();
    // oneCanvas->cd(4); 
    // if (plotHisto("TCphi", "iphi", "ieta", "", "Cold TC Sample3 Phi plane", "LEGO")) oneCanvas->Update();
  }

  canvasX += canvasStepX;
  canvasY += canvasStepY;
  //viewing the Cold TailCatcher cell histograms
  // if ((path=="/LArCellFCAL") && (canvasTitle=="Cold TailCatcher cell")) {
  if ( canvasNameBase == "cBaseCorr" ) {
    TCanvas* oneCanvas = new TCanvas(canvasNameBase, canvasTitle, canvasX, canvasY, canvasWidthX, canvasWidthY);
    oneCanvas->Divide(2, 2);
    oneCanvas->cd(1); 
    if (plotHisto("EMECVsHEC", "E EMEC (MeV)", "E HEC (MeV)", "", "HEC vs EMEC", "BOX")) oneCanvas->Update();
    oneCanvas->cd(2); 
    if (plotHisto("EMECVsFCAL", "E EMEC (MeV)", "E FCAL (MeV)", "", "FCAL vs EMEC", "BOX")) oneCanvas->Update();
    oneCanvas->cd(3); 
    if (plotHisto("HECVsFCAL", "E HEC (MeV)", "E FCAL (MeV)", "", "FCAL vs HEC", "BOX")) oneCanvas->Update();
  }
  
  //return to the root file top directory
  gDirectory->cd();
}
