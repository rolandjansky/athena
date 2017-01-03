/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


{

gROOT->ProcessLine(".L BuildROCLib.C");
TPaveStats *s;
TLegend* l;

TFile f("testOutE.Cl.root");
TFile g("testOutJ.Cl.root");

TH1F* hc1 = (TH1F*)f.Get("rEta");
TH1F* hc2 = (TH1F*)g.Get("rEta");
hc1->Scale(100.0/((float)hc1->GetEntries()) );
hc2->Scale(100.0/((float)hc2->GetEntries()) );

TCanvas c1;
l = new TLegend(0.5,0.5,0.65,0.65);
l->AddEntry(hc1,"electron");
l->AddEntry(hc2,"jet");
hc1->SetXTitle("rEta");
hc1->Draw();
hc2->SetLineColor(2);
hc2->Draw("same");
l->Draw("same");
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetX1NDC(0.2); s->SetX2NDC(0.39);}
c1.Print("rEta.pdf");


TH2F* ROC_rEta = NULL;
char a[]="ROC_rEta";
std::cout << "rEta : " << std::endl;
BuildROC( hc1 , hc2, ROC_rEta, a ) ;
TCanvas c2;
ROC_rEta->SetMarkerStyle(20);
ROC_rEta->SetMarkerSize(0.4);
ROC_rEta->SetTitle("efficiency versus fake rate for different RCore cuts");
ROC_rEta->Draw();
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetY1NDC(0.2); s->SetY2NDC(0.39);}
c2.Print("rEtaOpt.pdf");


TH1F *n3_el = ((TH1F*)f.Get("num3_pt"));
TH1F *n3_je = ((TH1F*)g.Get("num3_pt"));
TH1F *n4_el = ((TH1F*)f.Get("num4_pt"));
TH1F *n4_je = ((TH1F*)g.Get("num4_pt"));
n3_el->Sumw2();
n3_je->Sumw2();
n4_el->Sumw2();
n4_je->Sumw2();
n4_el->Divide(n4_el,n3_el,1.0,1.0,"B");
n4_je->Divide(n4_je,n3_je,1.0,1.0,"B");

TH1F *n3_eta_el = ((TH1F*)f.Get("num3_eta"));
TH1F *n3_eta_je = ((TH1F*)g.Get("num3_eta"));
TH1F *n4_eta_el = ((TH1F*)f.Get("num4_eta"));
TH1F *n4_eta_je = ((TH1F*)g.Get("num4_eta"));
n3_eta_el->Sumw2();
n3_eta_je->Sumw2();
n4_eta_el->Sumw2();
n4_eta_je->Sumw2();
n4_eta_el->Divide(n4_eta_el,n3_eta_el,1.0,1.0,"B");
n4_eta_je->Divide(n4_eta_je,n3_eta_je,1.0,1.0,"B");

TCanvas c2a;
n4_el->SetTitle("efficiency of rEta in electron sample; pt [GeV]");
n4_el->Draw();
c2a.Print("rEtaEff_pt.pdf");

TCanvas c2b;
n4_eta_el->SetTitle("efficiency of rEta in electron sample; #eta");
n4_eta_el->Draw();
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetY1NDC(0.2); s->SetY2NDC(0.39);}
c2b.Print("rEtaEff_eta.pdf");

TCanvas c2c;
n4_je->SetTitle("efficiency of rEta in jet sample; pt [GeV]");
n4_je->Draw();
c2c.Print("rEtaEff_pt_fa.pdf");

TCanvas c2d;
n4_eta_je->SetTitle("efficiency of rEta in jet sample; #eta");
n4_eta_je->Draw();
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetY1NDC(0.2); s->SetY2NDC(0.39);}
c2d.Print("rEtaEff_eta_fa.pdf");

TCanvas c2e;
((TH2F*)f.Get("rEta_vs_eta"))->SetTitle("rEta ; #eta; rEta");
((TH2F*)f.Get("rEta_vs_eta"))->Draw("COLZ");
c2e.Print("rEta_vs_eta.el.pdf");

TCanvas c2f;
((TH2F*)f.Get("f1_vs_eta"))->SetTitle("f1 ; #eta; f1");
((TH2F*)f.Get("f1_vs_eta"))->Draw("COLZ");
c2f.Print("f1_vs_eta.el.pdf");

TCanvas c2g;
((TH2F*)f.Get("f3_vs_eta"))->SetTitle("f3 ; #eta; f3");
((TH2F*)f.Get("f3_vs_eta"))->Draw("COLZ");
c2g.Print("f3_vs_eta.el.pdf");


TH1F* hc1_f3 = (TH1F*)f.Get("f3");
TH1F* hc2_f3 = (TH1F*)g.Get("f3");
hc1_f3->Scale(100.0/((float)hc1_f3->GetEntries()) );
hc2_f3->Scale(100.0/((float)hc2_f3->GetEntries()) );

TCanvas c3;
l = new TLegend(0.5,0.5,0.65,0.65);
l->AddEntry(hc1_f3,"electron");
l->AddEntry(hc2_f3,"jet");
hc1_f3->SetXTitle("f3");;
hc1_f3->Draw();
hc2_f3->SetLineColor(2);
hc2_f3->Draw("same");
l->Draw("same");
gPad->SetLogy(1);
c3.Print("f3.pdf");

TH2F* ROC_f3 = NULL;
char b[]="ROC_f3";
std::cout << "f3 : " << std::endl;
BuildROC( hc1_f3 , hc2_f3, ROC_f3, b, true ) ;
TCanvas c4;
ROC_f3->SetMarkerStyle(20);
ROC_f3->SetMarkerSize(0.4);
ROC_f3->SetTitle("efficiency versus fake rate for different f3 cuts");
ROC_f3->Draw();
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetY1NDC(0.2); s->SetY2NDC(0.39);}
c4.Print("f3Opt.pdf");

TH1F* hc1_f1 = (TH1F*)f.Get("f1");
TH1F* hc2_f1 = (TH1F*)g.Get("f1");
hc1_f1->Scale(100.0/((float)hc1_f1->GetEntries()) );
hc2_f1->Scale(100.0/((float)hc2_f1->GetEntries()) );

TCanvas c5;
l = new TLegend(0.6,0.5,0.75,0.65);
l->AddEntry(hc1_f1,"electron");
l->AddEntry(hc2_f1,"jet");
hc1_f1->Draw();
hc2_f1->SetLineColor(2);
hc2_f1->Draw("same");
l->Draw("same");
c5.Print("f1.pdf");

TH2F* ROC_f1 = NULL;
char c[]="ROC_f1";
std::cout << "f1 : " << std::endl;
BuildROC( hc1_f1 , hc2_f1, ROC_f1, c ) ;
TCanvas c6;
ROC_f1->SetMarkerStyle(20);
ROC_f1->SetMarkerSize(0.4);
ROC_f1->SetTitle("efficiency versus fake rate for different f1 cuts");
ROC_f1->Draw();
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetY1NDC(0.2); s->SetY2NDC(0.39);}
c6.Print("f1Opt.pdf");

TH1F* hc1_had1 = (TH1F*)f.Get("had1");
TH1F* hc2_had1 = (TH1F*)g.Get("had1");
hc1_had1->Scale(100.0/((float)hc1_had1->GetEntries()) );
hc2_had1->Scale(100.0/((float)hc2_had1->GetEntries()) );

TCanvas c7;
l = new TLegend(0.6,0.5,0.75,0.65);
l->AddEntry(hc1_had1,"electron");
l->AddEntry(hc2_had1,"jet");
hc1_had1->Draw();
hc2_had1->SetLineColor(2);
hc2_had1->Draw("same");
l->Draw("same");
gPad->SetLogy(1);
c7.Print("ehad1.pdf");

TH2F* ROC_had1 = NULL;
char d[]="ROC_had1";
std::cout << "ehad : " << std::endl;
BuildROC( hc1_had1 , hc2_had1, ROC_had1, d, true ) ;
TCanvas c8;
ROC_had1->SetMarkerStyle(20);
ROC_had1->SetMarkerSize(0.4);
ROC_had1->SetTitle("efficiency versus fake rate for different had1 cuts");
ROC_had1->Draw();
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetY1NDC(0.2); s->SetY2NDC(0.39);}
c8.Print("ehad1Opt.pdf");

TH1F* hc1_wstot = (TH1F*)f.Get("wstot");
TH1F* hc2_wstot = (TH1F*)g.Get("wstot");
hc1_wstot->Scale(100.0/((float)hc1_wstot->GetEntries()) );
hc2_wstot->Scale(100.0/((float)hc2_wstot->GetEntries()) );

TCanvas c9;
l = new TLegend(0.6,0.5,0.75,0.65);
l->AddEntry(hc1_wstot,"electron");
l->AddEntry(hc2_wstot,"jet");
hc1_wstot->Draw();
hc2_wstot->SetLineColor(2);
hc2_wstot->Draw("same");
l->Draw("same");
gPad->SetLogy(1);
c9.Print("wstot.pdf");

TH2F* ROC_wstot = NULL;
char e[]="ROC_wstot";
std::cout << "wstot : " << std::endl;
BuildROC( hc1_wstot , hc2_wstot, ROC_wstot, e, true ) ;
TCanvas c10;
ROC_wstot->SetMarkerStyle(20);
ROC_wstot->SetMarkerSize(0.4);
ROC_wstot->SetTitle("efficiency versus fake rate for different wstot cuts");
ROC_wstot->Draw();
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetY1NDC(0.2); s->SetY2NDC(0.39);}
c10.Print("wstotOpt.pdf");

TH1F* hc1_eratio = (TH1F*)f.Get("eratio");
TH1F* hc2_eratio = (TH1F*)g.Get("eratio");
hc1_eratio->Scale(100.0/((float)hc1_eratio->GetEntries()) );
hc2_eratio->Scale(100.0/((float)hc2_eratio->GetEntries()) );

TCanvas c11;
l = new TLegend(0.6,0.5,0.75,0.65);
l->AddEntry(hc1_eratio,"electron");
l->AddEntry(hc2_eratio,"jet");
hc1_eratio->Draw();
hc2_eratio->SetLineColor(2);
hc2_eratio->Draw("same");
l->Draw("same");
gPad->SetLogy(1);
c11.Print("eratio.pdf");

TH2F* ROC_eratio = NULL;
char h[]="ROC_eratio";
std::cout << "eratio : " << std::endl;
BuildROC( hc1_eratio , hc2_eratio, ROC_eratio, h ) ;
TCanvas c12;
ROC_eratio->SetMarkerStyle(20);
ROC_eratio->SetMarkerSize(0.4);
ROC_eratio->SetTitle("efficiency versus fake rate for different eratio cuts");
ROC_eratio->Draw();
gPad->Update();
s = (TPaveStats*) gPad->GetPrimitive("stats");
if ( s ) {s->SetY1NDC(0.2); s->SetY2NDC(0.39);}
c12.Print("eratioOpt.pdf");


}
