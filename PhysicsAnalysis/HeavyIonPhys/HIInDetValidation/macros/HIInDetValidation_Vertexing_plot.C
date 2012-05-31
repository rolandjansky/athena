/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "vector"
#include "stdio.h"
#include "TFile.h"
#include "TChain.h"
#include "TH2F"
#include "TH1F.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TLatex.h"


#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"

#ifdef __MAKECINT__
#pragma link C++ class vector<float>+;
#endif


   
   
void HIInDetValidation_Vertexing_plot(){

/////////////////ATLAS STYLE//////////////////////////////////////////
TStyle *atlasStyle = new TStyle("ATLAS","Atlas style");

  // use plain black on white colors
  Int_t icol=0; // WHITE
  atlasStyle->SetFrameBorderMode(icol);
  atlasStyle->SetFrameFillColor(icol);
  atlasStyle->SetCanvasBorderMode(icol);
  atlasStyle->SetCanvasColor(icol);
  atlasStyle->SetPadBorderMode(icol);
  atlasStyle->SetPadColor(icol);
  atlasStyle->SetStatColor(icol);
  //atlasStyle->SetFillColor(icol); // don't use: white fill color for *all* objects

  // set the paper & margin sizes
  atlasStyle->SetPaperSize(20,26);

  // set margin sizes
  atlasStyle->SetPadTopMargin(0.);
  atlasStyle->SetPadRightMargin(0.0005);
  atlasStyle->SetPadBottomMargin(0.12);
  atlasStyle->SetPadLeftMargin(0.13);
  


  // set title offsets (for axis label)
  atlasStyle->SetTitleXOffset(0.75);
 // atlasStyle->SetTitleYOffset(1.1);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.04;
  atlasStyle->SetTextFont(font);
  atlasStyle->SetStatFontSize(0.033);


  atlasStyle->SetTextSize(tsize);
  atlasStyle->SetLabelFont(font,"x");
  atlasStyle->SetTitleFont(font,"x");
  atlasStyle->SetLabelFont(font,"y");
  atlasStyle->SetTitleFont(font,"y");
  atlasStyle->SetLabelFont(font,"z");
  atlasStyle->SetTitleFont(font,"z");
  
  atlasStyle->SetLabelSize(tsize,"x");
  atlasStyle->SetTitleSize(tsize,"x");
  atlasStyle->SetLabelSize(tsize,"y");
  atlasStyle->SetTitleSize(tsize,"y");
  atlasStyle->SetLabelSize(tsize,"z");
  atlasStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  atlasStyle->SetMarkerStyle(21);
 // atlasStyle->SetMarkerSize(0.9);
 // atlasStyle->SetMarkerColor(4);
  atlasStyle->SetHistLineWidth(1.);
  atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //atlasStyle->SetErrorX(0.001);
  // get rid of error bar caps
  atlasStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  atlasStyle->SetOptTitle();
  atlasStyle->SetOptStat(0);
  //atlasStyle->SetOptStat(0);
  //atlasStyle->SetOptFit(1111);
  atlasStyle->SetOptFit(011);

 // put tick marks on top and RHS of plots
  //atlasStyle->SetPadTickX(1);
 // atlasStyle->SetPadTickY(1);
 
 atlasStyle->SetStatY(0.99);                
// Set y-position (fraction of pad size)
 atlasStyle->SetStatX(0.99);                
// Set x-position (fraction of pad size)
 atlasStyle->SetStatW(0.25);                
// Set width of stat-box (fraction of pad size)
 atlasStyle->SetStatH(0.15);                
 // Set height of stat-box (fraction of pad size)
 atlasStyle->SetLabelOffset(0.008,"X");
 atlasStyle->SetLabelOffset(0.008,"Y");
  
  
 atlasStyle->SetOptStat(0);
 atlasStyle->SetOptFit(0011);

  
   atlasStyle->SetFrameBorderMode(0);
 gROOT->SetStyle("ATLAS");
 gROOT->ForceStyle();
///////////////////////////////////////////////////////////////////////////////////////





/*TCanvas* c1=new TCanvas();
c1->SetWindowSize(700,1000);
c1->Resize();*/

   TCanvas *c1 = new TCanvas("c1", "Tracking Performance",10,32,500,667);
   gStyle->SetOptStat(0);
   c1->Range(0,0,1,1);
   c1->SetFillColor(10);
   c1->SetBorderMode(0);
   c1->SetBorderSize(2);
   c1->SetFrameFillColor(0);
   c1->SetFrameBorderMode(0);


  TFile f1("Vertexing-SetupA.root");

 
 c1->cd();
 c1->Clear();c1->Divide(2,3);


 c1->cd(1);


 h_res_vx_x->Fit("gaus", "", "", -0.04, 0.04);
  h_res_vx_x->Draw("h");
 h_res_vx_x->GetXaxis()->CenterTitle(true);
 h_res_vx_x->GetYaxis()->CenterTitle(true);
 h_res_vx_x->GetYaxis()->SetTitleFont(62); 
 h_res_vx_x->GetXaxis()->SetTitleFont(62); 
 h_res_vx_x->GetYaxis()->SetTitleSize(0.065); 
 h_res_vx_x->GetXaxis()->SetTitleSize(0.065); 
 h_res_vx_x->GetYaxis()->SetTitleOffset(0.7); 
 h_res_vx_x->GetXaxis()->SetTitle("v_{x}^{reco}-v_{x}^{gen} [mm]");
 h_res_vx_x->GetYaxis()->SetTitle("Events");
 
    tex = new TLatex(0.8,0.73,"   (c)");
tex->SetNDC();
   tex->SetTextSize(0.05401814);
   tex->SetLineWidth(2);
   tex->Draw();
 c1_1->Modified();

 c1->cd(2);
 h_res_vx_y->Fit("gaus", "", "", -0.04, 0.04);
  h_res_vx_y->Draw("h");
 h_res_vx_y->GetXaxis()->CenterTitle(true);
 h_res_vx_y->GetYaxis()->CenterTitle(true);
 h_res_vx_y->GetYaxis()->SetTitleFont(62); 
 h_res_vx_y->GetXaxis()->SetTitleFont(62); 
 h_res_vx_y->GetYaxis()->SetTitleSize(0.065); 
 h_res_vx_y->GetXaxis()->SetTitleSize(0.065); 
 h_res_vx_y->GetYaxis()->SetTitleOffset(0.7); 
 h_res_vx_y->GetXaxis()->SetTitle("v_{y}^{reco}-v_{y}^{gen} [mm]");
 h_res_vx_y->GetYaxis()->SetTitle("Events");
   tex = new TLatex(0.8,0.73,"   (d)");
tex->SetNDC();
   tex->SetTextSize(0.05401814);
   tex->SetLineWidth(2);
   tex->Draw();
 c1_2->Modified();

    c1->cd(3);

 h_vx_z->Draw("h");
 h_vx_z->GetXaxis()->CenterTitle(true);
 h_vx_z->GetYaxis()->CenterTitle(true);
 h_vx_z->GetYaxis()->SetTitleFont(62); 
 h_vx_z->GetXaxis()->SetTitleFont(62); 
 h_vx_z->GetYaxis()->SetTitleSize(0.065); 
 h_vx_z->GetXaxis()->SetTitleSize(0.065); 
 h_vx_z->GetYaxis()->SetTitleOffset(0.7); 
 h_vx_z->GetXaxis()->SetTitle("v_{z}^{reco} [mm]");
 h_vx_z->GetYaxis()->SetTitle("Events");
    tex = new TLatex(0.8,0.73,"   (b)");
tex->SetNDC();
   tex->SetTextSize(0.05401814);
   tex->SetLineWidth(2);
   tex->Draw();
 c1_3->Modified();

  
 c1->cd(4);
 //c1->SetLiny();
 h_res_vx_z->Draw("h");
 h_res_vx_z->Fit("gaus", "", "", -0.1, 0.1);
 h_res_vx_z->GetXaxis()->CenterTitle(true);
 h_res_vx_z->GetYaxis()->CenterTitle(true);
 h_res_vx_z->GetYaxis()->SetTitleFont(62); 
 h_res_vx_z->GetXaxis()->SetTitleFont(62); 
 h_res_vx_z->GetYaxis()->SetTitleSize(0.065); 
 h_res_vx_z->GetXaxis()->SetTitleSize(0.065); 
 h_res_vx_z->GetYaxis()->SetTitleOffset(0.7); 
 h_res_vx_z->GetXaxis()->SetTitle("v_{z}^{reco}-v_{z}^{gen} [mm]");
 h_res_vx_z->GetYaxis()->SetTitle("Events");
 
    tex = new TLatex(0.8,0.73,"   (e)");
tex->SetNDC();
   tex->SetTextSize(0.05401814);
   tex->SetLineWidth(2);
   tex->Draw();
 c1_4->Modified();
 
 c1->cd(5);

 h_vx_corr->SetMarkerSize(0.15);
 h_vx_corr->Draw("h");
 h_vx_corr->GetXaxis()->CenterTitle(true);
 h_vx_corr->GetYaxis()->CenterTitle(true);
 h_vx_corr->GetYaxis()->SetTitleFont(62); 
 h_vx_corr->GetXaxis()->SetTitleFont(62); 
 h_vx_corr->GetYaxis()->SetTitleSize(0.065); 
 h_vx_corr->GetXaxis()->SetTitleSize(0.065); 
 h_vx_corr->GetYaxis()->SetTitleOffset(1.); 
 h_vx_corr->GetXaxis()->SetTitle("Tracks in event");
 h_vx_corr->GetYaxis()->SetTitle("Tracks in vertex");
 
   tex = new TLatex(0.8,0.73,"   (a)");
tex->SetNDC();
   tex->SetTextSize(0.05401814);
   tex->SetLineWidth(2);
   tex->Draw();
 c1_5->Modified();
 
 c1->cd(6);

 h_vx_res_nTracks->SetMarkerSize(0.15);
 h_vx_res_nTracks->Draw("h");
 h_vx_res_nTracks->GetYaxis()->SetTitleFont(62); 
 h_vx_res_nTracks->GetXaxis()->SetTitleFont(62); 
 h_vx_res_nTracks->GetYaxis()->SetTitleSize(0.065); 
 h_vx_res_nTracks->GetXaxis()->SetTitleSize(0.065); 
 h_vx_res_nTracks->GetYaxis()->SetTitleOffset(1.); 
 h_vx_res_nTracks->GetXaxis()->CenterTitle(true);
 h_vx_res_nTracks->GetYaxis()->CenterTitle(true);
 h_vx_res_nTracks->GetXaxis()->SetTitle("Tracks in vertex");
 h_vx_res_nTracks->GetYaxis()->SetTitle("v_{x}^{reco}-v_{x}^{gen} [mm]");

 
   tex = new TLatex(0.8,0.73,"   (f)");
tex->SetNDC();
   tex->SetTextSize(0.05401814);
   tex->SetLineWidth(2);
   tex->Draw();
 c1_6->Modified();


c1_4->SetLogy();
c1_2->SetLogy();
c1_1->SetLogy();

c1->SaveAs("Vertex_plot_C.pdf");


}
