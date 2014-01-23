/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"

TH1* extractHist( TVirtualPad* pad, TString hname )
{
  
  TList* l = pad->GetListOfPrimitives();
  if( !l ) return 0;
  unsigned int entries = l->GetEntries();
  for( unsigned int i=0;i<entries; ++i ){
    TObject* object = l->At(i);
    if( object->GetName() == hname){
      return (TH1*)object;
    }    
  }
  return 0;
}

void drawSag(){


  TCanvas* sag_resi = new TCanvas("sag_resi","sag_resi",900,600);
  sag_resi->Divide(2,2);
  TVirtualPad* pad = sag_resi->cd(1);
  pad->SetLogy();
  MDTTestHits->Draw("m_Validation_MDT_ExNomWireR-m_Validation_MDT_SdoRadius");
  TH1* hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SagResi");
    hist->GetXaxis()->SetTitle("#Delta R_{sdo-ex nom}");
  }
  pad = sag_resi->cd(2);
  MDTTestHits->Draw("m_Validation_MDT_ExSagWireR-m_Validation_MDT_SdoRadius");
  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SagR_SdoR");
    hist->GetXaxis()->SetTitle("#Delta R_{sdo-ex sag}");
  }
  pad = sag_resi->cd(3);
  MDTTestHits->Draw("m_Validation_MDT_LocalZ/m_Validation_MDT_WireLen",
		    "abs(m_Validation_MDT_ExSagWireR-m_Validation_MDT_SdoRadius) > 0.1" );
  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SagResi_wrong");
    hist->GetXaxis()->SetTitle("Sim pos z/wire length");
  }
  pad = sag_resi->cd(4);
  MDTTestHits->Draw("m_Validation_MDT_SimRadius-abs(m_Validation_MDT_SdoRadius):m_Validation_MDT_LocalZ",
		    "" );

  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SagResi_wrong_wrong");
    hist->GetXaxis()->SetTitle("Sim pos z");
    hist->GetYaxis()->SetTitle("Projective sag");
  }

  sag_resi->Print("sag_resi.eps","eps");
  sag_resi->Print("sag_resi.gif","gif");
}



void drawSim(){

  TCanvas* sim_resi = new TCanvas("sim_resi","sim_resi",900,600);
  sim_resi->Divide(2,2);
  TVirtualPad* pad = sim_resi->cd(1);
  pad->SetLogy();
  MDTTestHits->Draw("abs(m_Validation_MDT_ExNomWireR)-m_Validation_MDT_SimRadius");
  TH1* hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SimResi");
    hist->GetXaxis()->SetTitle("#Delta R_{sim-ex}");
  }
  pad = sim_resi->cd(2);
  MDTTestHits->Draw("m_Validation_MDT_ExNomWireZ-m_Validation_MDT_LocalZ");
  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SimZ_ExZ");
    hist->GetXaxis()->SetTitle("SimHit Z");
    hist->GetYaxis()->SetTitle("Extrapolated Z");
  }
  pad = sim_resi->cd(3);
  MDTTestHits->Draw("m_Validation_MDT_LocalZ/m_Validation_MDT_WireLen",
		    "abs(abs(m_Validation_MDT_ExNomWireR)-m_Validation_MDT_SimRadius) > 0.1" );
  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SimResi_wrong");
    hist->GetXaxis()->SetTitle("Sim pos z/wire length");
  }
  pad = sim_resi->cd(4);
  MDTTestHits->Draw("m_Validation_MDT_SimRadius:abs(m_Validation_MDT_ExNomWireR)",
		    "abs(m_Validation_MDT_LocalZ/m_Validation_MDT_WireLen) < 0.45 && abs(abs(m_Validation_MDT_ExNomWireR)-m_Validation_MDT_SimRadius) > 0.1" );

  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SimResi_wrong_wrong");
    hist->GetXaxis()->SetTitle("R_{ex}");
    hist->GetYaxis()->SetTitle("R_{sim}");
  }

  sim_resi->Print("sim_resi.eps","eps");
  sim_resi->Print("sim_resi.gif","gif");
}


void drawROT(){

  TCanvas* rot_resi = new TCanvas("rot_resi","rot_resi",900,600);
  rot_resi->Divide(2,2);
  TVirtualPad* pad = rot_resi->cd(1);
  pad->SetLogy();
  MDTTestHits->Draw("m_Validation_MDT_ExSagRotR-m_Validation_MDT_SdoRadius");
  TH1* hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("ROTResi");
    hist->GetXaxis()->SetTitle("#Delta R_{sdo-rot}");
  }
  pad = rot_resi->cd(2);
  MDTTestHits->Draw("m_Validation_MDT_ExSagRotR:m_Validation_MDT_SdoRadius");
  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("ROT_ExRSDO");
    hist->GetXaxis()->SetTitle("Extr. ROT R");
    hist->GetYaxis()->SetTitle("Sdo R");
  }
  pad = rot_resi->cd(3);
  MDTTestHits->Draw("abs(m_Validation_MDT_ExSagRotR)-m_Validation_MDT_RotRadius",
		    "abs( abs(m_Validation_MDT_ExSagRotR)-m_Validation_MDT_RotRadius ) < 0.5");
  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("RotResi_true");
    hist->GetXaxis()->SetTitle("R_{ROT}-R_{extr.} (single tube res)");
    gStyle->SetFitFormat("3.4f");
    hist->Fit("gaus");
  }

  pad = rot_resi->cd(4);
  MDTTestHits->Draw("abs(m_Validation_MDT_ExSagRotR)-m_Validation_MDT_RotRadius:m_Validation_MDT_LocalZ",
		    "abs( abs(m_Validation_MDT_ExSagRotR)-m_Validation_MDT_RotRadius ) < 0.5");

  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("RotResi_true_z");
    hist->GetYaxis()->SetTitle("R_{ROT}-R_{extr.}");
    hist->GetXaxis()->SetTitle("Z_{sim}");
  }

  rot_resi->Print("rot_resi.eps","eps");
  rot_resi->Print("rot_resi.gif","gif");
}

 
void drawProp(TString chname){
  
  TString cname = "prop_resi_";
  cname += chname;
  TCanvas* prop_resi_BO = new TCanvas(cname,cname,900,600);
  prop_resi_BO->Divide(2,2);

  TString radialResString("abs(m_Validation_MDT_ExNomWireR)-m_Validation_MDT_SimRadius");
  TString zResString("m_Validation_MDT_ExNomWireZ-m_Validation_MDT_LocalZ");

  // define cuts
  TString radialCut("abs(abs(m_Validation_MDT_ExNomWireR)-m_Validation_MDT_SimRadius)<0.5");
  TString chamberCutS("m_Validation_MDT_StationName==\"");
  chamberCutS += chname + "S\""; 
  TString chamberCutL("m_Validation_MDT_StationName==\"");
  chamberCutL += chname + "L\""; 
  TString cutS = radialCut + "&&" + chamberCutS;
  TString cutL = radialCut + "&&" + chamberCutL;

  pad = prop_resi_BO->cd(1);
  MDTTestHits->Draw(radialResString,cutS);
  hist = extractHist(pad,"htemp");
  if( hist ){
    TString hname = "PropResi_R_";
    hname += chname + "S";
    hist->SetName(hname);
    hist->GetXaxis()->SetTitle("#Delta R");
  }

  pad = prop_resi_BO->cd(2);

  MDTTestHits->Draw(zResString,chamberCutS);
  hist = extractHist(pad,"htemp");
  if( hist ){
    TString hname = "PropResi_Z_";
    hname += chname + "S";
    hist->SetName(hname);
    hist->GetXaxis()->SetTitle("#Delta Z");
  }
 
  pad = prop_resi_BO->cd(3);
  MDTTestHits->Draw(radialResString,cutL);
  hist = extractHist(pad,"htemp");
  if( hist ){
    TString hname = "PropResi_R_";
    hname += chname + "L";
    hist->SetName(hname);
    hist->GetXaxis()->SetTitle("#Delta R");
  }

  pad = prop_resi_BO->cd(4);

  MDTTestHits->Draw(zResString,chamberCutL);
  hist = extractHist(pad,"htemp");
  if( hist ){
    TString hname = "PropResi_Z_";
    hname += chname + "L";
    hist->SetName(hname);
    hist->GetXaxis()->SetTitle("#Delta Z");
  }
}


void drawProp(){

  TCanvas* prop_resi = new TCanvas("prop_resi","prop_resi",900,600);
  prop_resi->Divide(2,2);
  TVirtualPad* pad = prop_resi->cd(1);
  MDTTestHits->Draw("abs(m_Validation_MDT_ExNomWireR)-m_Validation_MDT_SimRadius",
		    "abs(abs(m_Validation_MDT_ExNomWireR)-m_Validation_MDT_SimRadius)<0.5");
  TH1* hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SimResi");
    hist->GetXaxis()->SetTitle("#Delta R_{sim-ex}");
  }
  pad = prop_resi->cd(2);
  MDTTestHits->Draw("m_Validation_MDT_ExNomWireZ-m_Validation_MDT_LocalZ",
		    "abs(m_Validation_MDT_ExNomWireZ-m_Validation_MDT_LocalZ)<0.5");
  hist = extractHist(pad,"htemp");
  if( hist ){
    hist->SetName("SimZ_ExZ");
    hist->GetXaxis()->SetTitle("SimHit Z");
    hist->GetYaxis()->SetTitle("Extrapolated Z");
  }

  drawProp("BO");
}

void draw()
{
 TFile* f = new TFile("MDTTest.aan.root","READ");
  f->cd("MDTHitsValidation");
  drawSim();
  drawSag();
  drawROT();
}
