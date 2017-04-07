/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// StaveDraw.C
//
#include "TCanvas.h"
#include "TFile.h"
#include "TGaxis.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TTree.h"
#include "TString.h"
#include "TStyle.h"

#include "iostream"
enum {PIX=1, SCT=2};

float ComputeTz(int detec, int layer, double ring);

void StaveDraw(TString nname,Int_t DetType= 1, Int_t layer= 0, Int_t sector= 0, float MaxT=-1)
{
  int PrintLevel = 3;
  gStyle->SetCanvasColor(kWhite);  
  gStyle->SetPadColor(kWhite);
  gStyle->SetFrameFillColor(kWhite);
  gStyle->SetFrameBorderMode(0);

  int rings = 0; //  number of rings


  char tname[80];
  enum {TX=0, TY=1, TZ=2, RX=3, RY=4, RZ=5};

  if (PrintLevel>=3) std::cout << " -- StaveDraw -- opening input file " << nname << std::endl;
  //sprintf(nname,"ComparePositions.root");

  TFile* f0 = new TFile(nname.Data());
  if (PrintLevel>=3) std::cout << " -- StaveDraw -- input file opened " << nname << std::endl;

  Float_t TransMax = 0.30;
  if (MaxT>0) TransMax = MaxT;
  Float_t TransMin = -TransMax;

 
  Float_t TransScaleFactor = 1.;
  Float_t RotScaleFactor = 0.1;

  Float_t RotMax = TransMax/RotScaleFactor;
  Float_t RotMin = -RotMax;

  // sort out the detector type
  if (DetType<=0) {std::cout << " -- StaveDraw -- unknown detector type: " << DetType << std::endl; return;}

  // sort out the layer
  if (layer<0) layer = 0;
  if (DetType == PIX && layer>3) layer = 3;
  if (DetType == SCT && layer>3) layer = 3;
 
  // sort out the stave for pixels
  if (sector<0) sector = 0;
  if (DetType == PIX && layer==0 && sector>13) sector = 13; // IBL
  if (DetType == PIX && layer==1 && sector>21) sector = 21;
  if (DetType == PIX && layer==2 && sector>37) sector = 37;
  if (DetType == PIX && layer==3 && sector>51) sector = 51;
  
  // sort out the stave for the SCT
  if (DetType == SCT && layer==0 && sector>31) sector = 31;
  if (DetType == SCT && layer==1 && sector>39) sector = 39;
  if (DetType == SCT && layer==2 && sector>47) sector = 47;
  if (DetType == SCT && layer==3 && sector>51) sector = 51;

  if (DetType == PIX && layer ==0) rings = 20;
  if (DetType == PIX && layer > 0) rings = 13;
  if (DetType == SCT) rings = 12;

  // colorins
  Int_t RzColor = kAzure-1;
  Int_t TxColor = kRed+1;
  Int_t TyColor = kRed-2;
  Int_t TzColor = kRed-1;

  std::cout << " -- StaveDraw -- Drawing modules type " << DetType 
	    << " in layer " << layer 
	    << " and sector " << sector  
	    << std::endl;

  // 
  TString OutputName;
  if (DetType == PIX && layer==0) OutputName.Append("IBL");
  if (DetType == PIX && layer >0) OutputName.Append("PIX_Layer_");
  if (DetType == SCT) OutputName.Append("SCT_Layer_");

  if (!(DetType == PIX && layer==0)) OutputName += layer;

  OutputName.Append("_Stave_");
  OutputName += sector;

  OutputName.Append(".png");
  if (PrintLevel>=3) std::cout << " -- StaveDraw -- output file name:" <<OutputName.Data() << std::endl;
  //

  // 
  TH1F* hAliCorr;
  TTree* Corrections = (TTree *) f0->Get("Corrections"); 
  TCanvas *cAliCorr = new TCanvas ("cAliCorr","Alignment corrections per stave", 1);
  
  if (DetType == PIX && layer == 0 ) { // IBL 
    sprintf(tname,"IBL corrections stave %d", sector);
    hAliCorr = new TH1F ("hAliCorr",tname,13,-340,340);
  } 
  if (DetType == PIX && layer >0 ) { // pixel 
    sprintf(tname,"PIX corrections layer %d stave %d",layer, sector);
    hAliCorr = new TH1F ("hAliCorr",tname,13,-440,440);
  } 
  if (DetType == SCT) {
    sprintf(tname,"SCT corrections layer %d sector %d",layer, sector);
    hAliCorr = new TH1F ("hAliCorr",tname,12,-800,800);
    // change the range
    TransMax = 0.30;
    TransMin = -TransMax;
  }
  hAliCorr->SetStats(0);
  hAliCorr->GetYaxis()->SetLabelColor(TxColor);
  hAliCorr->GetYaxis()->SetTitleColor(TxColor);
  hAliCorr->GetYaxis()->SetAxisColor(TxColor);
  hAliCorr->GetYaxis()->SetRangeUser(TransMin,TransMax);
  hAliCorr->SetXTitle("Global Z (mm)");
  hAliCorr->SetYTitle("Local Translations (mm)");

  hAliCorr->Draw();

  //
  TGaxis *axis = new TGaxis(hAliCorr->GetXaxis()->GetXmax(), TransMin,
			    hAliCorr->GetXaxis()->GetXmax(), TransMax,
			    RotMin, RotMax,510,"+L");
  
  axis->SetTitle("Local rotations (mrad)");
  axis->SetLabelSize(hAliCorr->GetYaxis()->GetLabelSize());
  axis->SetTitleFont(hAliCorr->GetYaxis()->GetTitleFont());
  axis->SetTitleSize(hAliCorr->GetYaxis()->GetTitleSize());
  axis->SetTitleColor(RzColor);
  axis->SetLineColor(RzColor);
  axis->SetLabelColor(RzColor);
  axis->Draw();

  char SelectionName[80];
  char VariablesName[80];
  sprintf(SelectionName,"type==%d && bec==0 && layer==%d && sector==%d",DetType, layer, sector);
  Bool_t doPlot = true;

  // legend
  TLegend *Rotllo = new TLegend(0.30, 0.75, 0.41, 0.87);
  Rotllo->SetFillStyle(1001);
  Rotllo->SetFillColor(10);
  char LegendText[80];

  // Points with errors
  TGraphErrors* dofcor[6];


  for (Int_t idof=0; idof<6; idof++){
    if (idof == TX) {
      sprintf(VariablesName,"%f*tx:ComputeTz(%d,%d,ring):etx",TransScaleFactor,DetType,layer);
      doPlot = false;
      Corrections->SetMarkerColor(TxColor);
      Corrections->SetMarkerStyle(20);
      sprintf(LegendText,"Tx");
    }
    if (idof == TY) {
      sprintf(VariablesName,"%f*ty:ComputeTz(%d,%d,ring):ety",TransScaleFactor,DetType,layer);
      doPlot = false;
      Corrections->SetMarkerColor(TyColor);
      Corrections->SetMarkerStyle(28);
      sprintf(LegendText,"Ty");
    }
    if (idof == TZ) {
      sprintf(VariablesName,"%f*tz:ComputeTz(%d,%d,ring):etz",TransScaleFactor,DetType,layer);
      doPlot = true;
      Corrections->SetMarkerColor(TzColor);
      Corrections->SetMarkerStyle(26);
      sprintf(LegendText,"Tz");
    }
    if (idof == RX) {
      sprintf(VariablesName,"%f*rx:ring",RotScaleFactor);
      doPlot = false;
      Corrections->SetMarkerColor(TzColor);
      sprintf(LegendText,"Rx");
    }
    if (idof == RY) {
      sprintf(VariablesName,"%f*ry:ComputeTz(%d,%d,ring)",RotScaleFactor,DetType,layer);
      doPlot = false;
      Corrections->SetMarkerStyle(kOpenCircle);
      Corrections->SetMarkerColor(RzColor);
      sprintf(LegendText,"Ry");
    }
    if (idof == RZ) {
      sprintf(VariablesName,"%f*rz:ComputeTz(%d,%d,ring):erz/10",RotScaleFactor,DetType,layer);
      doPlot = false;
      Corrections->SetMarkerColor(RzColor);
      Corrections->SetMarkerStyle(20);
      sprintf(LegendText,"Rz (E/10)");
    }
    //
    if (doPlot) {
      Corrections->Draw(VariablesName,SelectionName,"goff");
      Rotllo->AddEntry(Corrections->Clone(),LegendText,"p");
      //static TGraphErrors dofcor(rings,Corrections->GetV2(),Corrections->GetV1(),0,Corrections->GetV3());
      dofcor[idof] = new TGraphErrors(rings,Corrections->GetV2(),Corrections->GetV1(),0,Corrections->GetV3());
      if (true) {
	dofcor[idof]->SetMarkerStyle(Corrections->GetMarkerStyle());
	dofcor[idof]->SetMarkerColor(Corrections->GetMarkerColor());
	dofcor[idof]->SetLineColor(Corrections->GetMarkerColor());
	dofcor[idof]->Draw("p same");
	dofcor[idof]->Print();
      }
    }
  }

  Rotllo->Draw();    
  gPad->SetGridy(1);
  cAliCorr->Print(OutputName.Data());
}

float ComputeTz(int detec, int layer, double ring)
{
  bool debug = false;
  if (debug) std::cout << " -- ComputeTz -- for ring " << ring << " -- START -- " << std::endl; 
  float zpos = ring*20;
  if (detec == PIX) { 
    if (layer == 0) { // IBL modules 
      // loop on ring till the desired one:
      zpos = -330; // far left edge
      float gap = 40; // 40 mm is the length of a planar module, 20 of a 3D one
      for (int i=-10; i <= ring; i++) {
	gap = 40; // refresh
	if ((-10 <= i) && (i <= -7)) gap = 20; // 3D modules 
	if ((6  <= i) && (i <=  9)) gap = 20; // 3D modules 
	zpos += gap;
	if (debug) std::cout << " ring " << i << " gap = " << gap << "  new zpos: " << zpos << std::endl;
      }
    }
    if (layer > 0) { // Pixel modules 
      // loop on ring till the desired one:
      zpos = -441; // far left edge
      float gap = 63; // 63 mm is the length of a pixel module
      for (int i=-6; i <= ring; i++) {
	zpos += gap;
	if (debug) std::cout << " ring " << i << " gap = " << gap << "  new zpos: " << zpos << std::endl;
      }
    }
  }
  if (debug) std::cout << " -- ComputeTz -- ring: " << ring << "  Tz= " << zpos << std::endl;
 
  return zpos;
}
