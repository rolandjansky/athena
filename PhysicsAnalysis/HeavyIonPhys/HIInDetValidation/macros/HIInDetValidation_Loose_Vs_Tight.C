/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
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
  atlasStyle->SetPadTopMargin(0.10);
  atlasStyle->SetPadRightMargin(0.05);
  atlasStyle->SetPadBottomMargin(0.10);
  atlasStyle->SetPadLeftMargin(0.10);

  // set title offsets (for axis label)
  atlasStyle->SetTitleXOffset(1.1);
  atlasStyle->SetTitleYOffset(1.4);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.04;
  atlasStyle->SetTextFont(font);

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
  atlasStyle->SetMarkerStyle(20);
  atlasStyle->SetMarkerSize(0.9);
  atlasStyle->SetMarkerColor(4);
  atlasStyle->SetHistLineWidth(1.);
  atlasStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //atlasStyle->SetErrorX(0.001);
  // get rid of error bar caps
  atlasStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  atlasStyle->SetOptTitle();
  atlasStyle->SetOptStat();
  //atlasStyle->SetOptStat(0);
  //atlasStyle->SetOptFit(1111);
  atlasStyle->SetOptFit(0);

 // put tick marks on top and RHS of plots
  //atlasStyle->SetPadTickX(1);
 // atlasStyle->SetPadTickY(1);
 
atlasStyle->SetStatY(0.99);                
// Set y-position (fraction of pad size)
atlasStyle->SetStatX(0.99);                
// Set x-position (fraction of pad size)
atlasStyle->SetStatW(0.2);                
// Set width of stat-box (fraction of pad size)
atlasStyle->SetStatH(0.1);                
// Set height of stat-box (fraction of pad size)
atlasStyle->SetLabelOffset(0.008,"X");
atlasStyle->SetLabelOffset(0.008,"Y");
  
  gROOT->SetStyle("ATLAS");
  gROOT->ForceStyle();
///////////////////////////////////////////////////////////////////////////////////////



TCanvas *c1 = new TCanvas("c1","title",800,600); 
//c1->Divide(3,2);

  TFile f1("Histos-Tracking-MCprob-Loose.root");
 TFile f2("Histos-Tracking-MCprob-Tight.root");

  f1.cd();
     leg_hist = new TLegend(0.92,0.13,0.72,0.23);
  //leg_hist->SetHeader("Some histograms");
  leg_hist->AddEntry(h_pt_eff,"loose cuts","lep");

     //h_pt_eff->Scale(100);
  h_pt_eff->Draw();
   f2.cd();
   h_pt_eff->SetMarkerColor(2);

   h_pt_eff->Draw("same");
  leg_hist->AddEntry(h_pt_eff,"tight cuts","lep");

  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Eff_pt_loose_vs_tight.png");
   
  f1.cd(); 
 // h_eta_eff->Scale(100);
  h_eta_eff->Draw();
   f2.cd();

   h_eta_eff->SetMarkerColor(2);
   h_eta_eff->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Eff_eta_loose_vs_tight.png");
   
   f1.cd(); 
   //h_phi_eff->Scale(100);
  h_phi_eff->Draw();
   f2.cd();
  
   h_phi_eff->SetMarkerColor(2);
   h_phi_eff->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Eff_phi_loose_vs_tight.png");  
  
    f1.cd(); 
    // h_D0_eff->Scale(100);
  h_D0_eff->Draw();
   f2.cd();

   h_D0_eff->SetMarkerColor(2);
   h_D0_eff->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Eff_d0_loose_vs_tight.png");
  
    f1.cd(); 
    // h_Z0_eff->Scale(100);
  h_Z0_eff->SetMinimum(50.);
  h_Z0_eff->Draw();
   f2.cd();
 

   h_Z0_eff->SetMarkerColor(2);
   h_Z0_eff->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Eff_z0_loose_vs_tight.png");
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  TFile f3("Fakes-C-Loose.root");
 TFile f4("Fakes-C-Tight.root");

   f3.cd(); 
   //h_pt_fake->Scale(100);
  h_pt_fake->Draw();
   f4.cd();
 
   h_pt_fake->SetMarkerColor(2);
   h_pt_fake->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Fake_pt_loose_vs_tight.png");

   f3.cd(); 
  // h_eta_fake->Scale(100);
  h_eta_fake->SetMinimum(0.);
  h_eta_fake->Draw();
   f4.cd();


   h_eta_fake->SetMarkerColor(2);
   h_eta_fake->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Fake_eta_loose_vs_tight.png");

   f3.cd(); 
   //h_phi_fake->Scale(100);
  h_phi_fake->Draw();
   f4.cd();

   h_phi_fake->SetMarkerColor(2);
   h_phi_fake->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Fake_phi_loose_vs_tight.png");
  
     f3.cd(); 
    // h_D0_fake->Scale(100);
  h_D0_fake->Draw();
   f4.cd();
 
   h_D0_fake->SetMarkerColor(2);
   h_D0_fake->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Fake_d0_loose_vs_tight.png");
  
     f3.cd(); 
  //    h_Z0_fake->Scale(100);
  h_Z0_fake->Draw();
   f4.cd();

   h_Z0_fake->SetMarkerColor(2);
   h_Z0_fake->Draw("same");
  leg_hist->Draw();
  c1->cd();
  c1->SaveAs("Fake_z0_loose_vs_tight.png");


}
