/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void ExtrapolationValidation(const TString& fileName)
{
//================ Trying out the style ===================/


   TStyle *plain  = new TStyle("Plain","Plain Style (no colors/fill areas)");
   // Canvas
   plain->SetCanvasBorderMode(0);
   plain->SetCanvasColor(0);
   // Pad
   plain->SetPadBorderMode(0);
   plain->SetPadColor(0);
   // Title
   plain->SetTitleColor(0);
   plain->SetTitleBorderSize(0);
   plain->SetTitleX(0.25);
   plain->SetTitleY(0.98);
   // Stat
   //plain->SetStatColor(1);
   //plain->SetOptStat(0);
   // Labelling
   plain->SetLabelFont(132, "XY");
   plain->SetLabelSize(0.05, "XY");
   // Lines
   //plain->SetLineWidth(0.5);
   gROOT->SetStyle("Plain");

   // =======================================================/

   // get the file
   TFile* file = TFile::Open(fileName, "READ");

   // get the tree
   TTree* tree = file ? file->Get("ExtrapolationValidation") : 0;

   if (tree){
     // draw the canvs
     TCanvas* parameterCanvas = new TCanvas("parametersCanvas", "Parameter differences", 100, 100, 600, 920);
     parameterCanvas->Divide(2,3);
     
     // local 1
     parameterCanvas->cd(1);
       tree->Draw("(ParametersLoc1[0]-ParametersLoc1[3])>>loc1(100,-0.01,0.01)","Parameters==4");
       TH1F* loc1 = (TH1F*)gDirectory->Get("loc1");
             loc1->Draw();
       parameterCanvas->GetPad(1)->SetLogy();
       parameterCanvas->GetPad(1)->SetGridy();
 
     // local 2
     parameterCanvas->cd(2);
       tree->Draw("(ParametersLoc2[0]-ParametersLoc2[3])>>loc2(100,-0.01,0.01)","Parameters==4");
       TH1F* loc2 = (TH1F*)gDirectory->Get("loc2");
             loc2->Draw();
       parameterCanvas->GetPad(2)->SetLogy();
       parameterCanvas->GetPad(2)->SetGridy();
     // phi
     parameterCanvas->cd(3);
       tree->Draw("(ParametersPhi[0]-ParametersPhi[3])/ParametersPhi[0]>>phi(100,-0.001,0.001)","Parameters==4");
       TH1F* phi = (TH1F*)gDirectory->Get("phi");
             phi->Draw();
       parameterCanvas->GetPad(3)->SetLogy();
       parameterCanvas->GetPad(3)->SetGridy();
     // theta
     parameterCanvas->cd(4);
       tree->Draw("(ParametersTheta[0]-ParametersTheta[3])/ParametersTheta[0]>>theta(100,-0.001,0.001)","Parameters==4");
       TH1F* theta = (TH1F*)gDirectory->Get("theta");
             theta->Draw();
       parameterCanvas->GetPad(4)->SetLogy();
       parameterCanvas->GetPad(4)->SetGridy();
     // q/p
     parameterCanvas->cd(5);
       tree->Draw("(ParametersQoverP[0]-ParametersQoverP[3])/ParametersQoverP[0]>>qop(100,-0.001,0.001)","Parameters==4");
       TH1F* qop = (TH1F*)gDirectory->Get("qop");
             qop->Draw();
       parameterCanvas->GetPad(5)->SetLogy();
       parameterCanvas->GetPad(5)->SetGridy();

     // draw the canvs
     TCanvas* statCanvas = new TCanvas("statCanvas", "Track statistics", 400, 400, 600, 920);
     statCanvas->Divide(1,3);
     // momentum distribution
     statCanvas->cd(1);
       tree->Draw("StartMomentum","Parameters==4");
     statCanvas->cd(2);
       tree->Draw("sqrt(DestinationSurfaceR*DestinationSurfaceR+DestinationSurfaceZ*DestinationSurfaceZ)","Parameters==4");
     statCanvas->cd(3);
       tree->Draw("abs(sqrt(DestinationSurfaceR*DestinationSurfaceR+DestinationSurfaceZ*DestinationSurfaceZ)-sqrt(StartSurfaceR*StartSurfaceR+StartSurfaceZ*StartSurfaceZ))","Parameters==4");


   }



}