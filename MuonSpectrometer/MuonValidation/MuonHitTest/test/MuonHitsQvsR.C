/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
   gStyle->SetTitleX(0.13);
   gStyle->SetTitleY(0.98);
   gStyle->SetStatX(0.90);
   gStyle->SetStatY(0.95);
   gStyle->SetStatW(0.15);
   gStyle->SetStatH(0.10);
   
   
   TCanvas *c1 = new TCanvas("c1","MuonHits_MDT_1200_R_vs_1201_R",0,0,800,650);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
   
   
   c1->Divide(2,2);   
   c1->cd(1);
  TFile tmp12("./mdthitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled12 = (TH1F*) tmp12.Get("mdtdet_barrel");   
   scaled12->Scale(1.);   
   scaled12->SetMarkerColor(4);   
   scaled12->SetTitle("MDT transverse 1200 R vs 1201 R");   
   scaled12->Draw();     
   TFile tmp11("./mdthitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled11 = (TH1F*) tmp11.Get("mdtdet_barrel");   
   scaled11->Scale(1.);
   scaled11->SetMarkerColor(2);   
   scaled11->Draw("same");   
 
   c1->cd(2);
   TFile tmp22("./mdthitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled22 = (TH1F*) tmp22.Get("mdtlong_view");   
   scaled22->Scale(1.);   
   scaled22->SetMarkerColor(4);   
   scaled22->SetTitle("MDT longitudinal 1200 R vs 1201 R");
   scaled22->Draw();     
   TFile tmp21("./mdthitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled21 = (TH1F*) tmp21.Get("mdtlong_view");   
   scaled21->Scale(1.);
   scaled21->SetMarkerColor(2);   
   scaled21->Draw("same");   
    
   c1->cd(3);
   TFile tmp32("./mdthitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled32 = (TH1F*) tmp32.Get("mdthitpos_phi");   
   scaled32->Scale(1.);   
   scaled32->SetLineColor(4);   
   scaled32->SetTitle("MDT phi 1200 R vs 1201 R");
   scaled32->DrawNormalized();     
   TFile tmp31("./mdthitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled31 = (TH1F*) tmp31.Get("mdthitpos_phi");   
   scaled31->Scale(1.);
   scaled31->SetLineColor(2);   
   scaled31->DrawNormalized("same");   
      
   c1->cd(4);
   TFile tmp42("./mdthitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled42 = (TH1F*) tmp42.Get("mdthitpos_eta");   
   scaled42->Scale(1.);   
   scaled42->SetLineColor(4);   
   scaled42->SetTitle("MDT eta 1200 R vs 1201 R");
   scaled42->DrawNormalized();     
   TFile tmp41("./mdthitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled41 = (TH1F*) tmp41.Get("mdthitpos_eta");   
   scaled41->Scale(1.);
   scaled41->SetLineColor(2);   
   scaled41->DrawNormalized("same");
   

   TCanvas *c2 = new TCanvas("c2","MuonHits RPC 1200 R vs 1201 R",0,0,800,650);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
   
   
   c2->Divide(2,2);   
   c2->cd(1);
   TFile tmp52("./rpchitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled52 = (TH1F*) tmp52.Get("rpcdet_barrel");   
   scaled52->Scale(1.);   
   scaled52->SetMarkerColor(4);   
   scaled52->SetTitle("RPC transverse 1200 R vs 1201 R");   
   scaled52->Draw();     
   TFile tmp51("./rpchitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled51 = (TH1F*) tmp51.Get("rpcdet_barrel");   
   scaled51->Scale(1.);
   scaled51->SetMarkerColor(2);   
   scaled51->Draw("same");   
 
   c2->cd(2);
   TFile tmp62("./rpchitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled62 = (TH1F*) tmp62.Get("rpclong_view");   
   scaled62->Scale(1.);   
   scaled62->SetMarkerColor(4);   
   scaled62->SetTitle("RPC longitudinal 1200 R vs 1201 R");
   scaled62->Draw();     
   TFile tmp61("./rpchitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled61 = (TH1F*) tmp61.Get("rpclong_view");   
   scaled61->Scale(1.);
   scaled61->SetMarkerColor(2);   
   scaled61->Draw("same");   
    
   c2->cd(3);
   TFile tmp72("./rpchitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled72 = (TH1F*) tmp72.Get("rpchitpos_phi");   
   scaled72->Scale(1.);   
   scaled72->SetLineColor(4);   
   scaled72->SetTitle("RPC phi 1200 R vs 1201 R");
   scaled72->DrawNormalized();     
   TFile tmp71("./rpchitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled71 = (TH1F*) tmp71.Get("rpchitpos_phi");   
   scaled71->Scale(1.);
   scaled71->SetLineColor(2);   
   scaled71->DrawNormalized("same");   
      
   c2->cd(4);
   TFile tmp82("./rpchitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled82 = (TH1F*) tmp82.Get("rpchitpos_eta");   
   scaled82->Scale(1.);   
   scaled82->SetLineColor(4);   
   scaled82->SetTitle("RPC eta 1200 R vs 1201 R");
   scaled82->DrawNormalized();     
   TFile tmp81("./rpchitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled81 = (TH1F*) tmp81.Get("rpchitpos_eta");   
   scaled81->Scale(1.);
   scaled81->SetLineColor(2);   
   scaled81->DrawNormalized("same");


   TCanvas *c3 = new TCanvas("c3","MuonHits CSC 1200 R vs 1201 R",0,0,800,650);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
   
   
   c3->Divide(2,2);   
   c3->cd(1);
   TFile tmp92("./cschitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled92 = (TH1F*) tmp92.Get("cscdet_barrel");   
   scaled92->Scale(1.);   
   scaled92->SetMarkerColor(4);   
   scaled92->SetTitle("CSC transverse 1200 R vs 1201 R");   
   scaled92->Draw();     
   TFile tmp91("./cschitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled91 = (TH1F*) tmp91.Get("cscdet_barrel");   
   scaled91->Scale(1.);
   scaled91->SetMarkerColor(2);   
   scaled91->Draw("same");   
 
   c3->cd(2);
   TFile tmp102("./cschitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled102 = (TH1F*) tmp102.Get("csclong_view");   
   scaled102->Scale(1.);   
   scaled102->SetMarkerColor(4);   
   scaled102->SetTitle("CSC longitudinal 1200 R vs 1201 R");
   scaled102->Draw();     
   TFile tmp101("./cschitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled101 = (TH1F*) tmp101.Get("csclong_view");   
   scaled101->Scale(1.);
   scaled101->SetMarkerColor(2);   
   scaled101->Draw("same");   
    
   c3->cd(3);
   TFile tmp112("./cschitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled112 = (TH1F*) tmp112.Get("cschitpos_phi");   
   scaled112->Scale(1.);   
   scaled112->SetLineColor(4);   
   scaled112->SetTitle("CSC phi 1200 R vs 1201 R");
   scaled112->DrawNormalized();     
   TFile tmp111("./cschitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled111 = (TH1F*) tmp111.Get("cschitpos_phi");   
   scaled111->Scale(1.);
   scaled111->SetLineColor(2);   
   scaled111->DrawNormalized("same");   
      
   c3->cd(4);
   TFile tmp122("./cschitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled122 = (TH1F*) tmp122.Get("cschitpos_eta");   
   scaled122->Scale(1.);   
   scaled122->SetLineColor(4);   
   scaled122->SetTitle("CSC eta 1200 R vs 1201 R");
   scaled122->DrawNormalized();     
   TFile tmp121("./cschitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled121 = (TH1F*) tmp121.Get("cschitpos_eta");   
   scaled121->Scale(1.);
   scaled121->SetLineColor(2);   
   scaled121->DrawNormalized("same");




   TCanvas *c4 = new TCanvas("c4","MuonHits TGC 1200 R vs 1201 R",0,0,800,650);
   gPad->SetBottomMargin(0.14);
   gPad->SetTopMargin(0.17);
   gPad->SetLeftMargin(0.16);
   
   
   c4->Divide(2,2);   
   c4->cd(1);
   TFile tmp132("./tgchitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled132 = (TH1F*) tmp132.Get("tgcdet_barrel");   
   scaled132->Scale(1.);   
   scaled132->SetMarkerColor(4);   
   scaled132->SetTitle("TGC transverse 1200 R vs 1201 R");   
   scaled132->Draw();     
   TFile tmp131("./tgchitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled131 = (TH1F*) tmp131.Get("tgcdet_barrel");   
   scaled131->Scale(1.);
   scaled131->SetMarkerColor(2);   
   scaled131->Draw("same");   
 
   c4->cd(2);
   TFile tmp142("./tgchitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled142 = (TH1F*) tmp142.Get("tgclong_view");   
   scaled142->Scale(1.);   
   scaled142->SetMarkerColor(4);   
   scaled142->SetTitle("TGC longitudinal 1200 R vs 1201 R");
   scaled142->Draw();     
   TFile tmp141("./tgchitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled141 = (TH1F*) tmp141.Get("tgclong_view");   
   scaled141->Scale(1.);
   scaled141->SetMarkerColor(2);   
   scaled141->Draw("same");   
    
   c4->cd(3);
   TFile tmp152("./tgchitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled152 = (TH1F*) tmp152.Get("tgchitpos_phi");   
   scaled152->Scale(1.);   
   scaled152->SetLineColor(4);   
   scaled152->SetTitle("TGC phi 1200 R vs 1201 R");
   scaled152->DrawNormalized();     
   TFile tmp151("./tgchitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled151 = (TH1F*) tmp151.Get("tgchitpos_phi");   
   scaled151->Scale(1.);
   scaled151->SetLineColor(2);   
   scaled151->DrawNormalized("same");   
      
   c4->cd(4);
   TFile tmp162("./tgchitpos_1200_R_50GeV.root", "READ");   
   TH1F *scaled162 = (TH1F*) tmp162.Get("tgchitpos_eta");   
   scaled162->Scale(1.);   
   scaled162->SetLineColor(4);   
   scaled162->SetTitle("TGC eta 1200 R vs 1201 R");
   scaled162->DrawNormalized();     
   TFile tmp161("./tgchitpos_1201_R_50GeV.root", "READ");   
   TH1F *scaled161 = (TH1F*) tmp161.Get("tgchitpos_eta");   
   scaled161->Scale(1.);
   scaled161->SetLineColor(2);   
   scaled161->DrawNormalized("same");
   




}




