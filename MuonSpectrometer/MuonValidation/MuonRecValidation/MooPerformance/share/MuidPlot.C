//#include "TH1.h"
{

  gROOT->ProcessLine(".x MoMuPlotStyle.C");

  TFile f1("TrackAnalysis.root");
  TTree *t1 = (TTree*)f1.Get("");

  c0 = new TCanvas("c0","  ",200,10,700,500);
  c0->Divide(1,1);
  c0->cd(1) ;
  res3p_Moore->GetXaxis()->SetTitleSize(0.07) ;
  res3p_Moore->GetXaxis()->SetTitleColor(4) ;
  res3p_Moore->SetXTitle(" Delta pt / pt ");
  res3p_Moore->SetLineColor(4);
  res3p_Moore->SetLineWidth(2);
  Double_t pars[6];
  TF1* g1s    = new TF1("g1s","gaus",-0.05,0.05);
  TF1* g2s    = new TF1("g2s","gaus",-0.5,0.5);
  TF1* totals = new TF1("totals","gaus(0)+gaus(3)",-0.5,0.5);
  totals->SetLineColor(2);
  totals->SetLineWidth(1);
  res3p_Moore->Fit(g1s,"RN");
  res3p_Moore->Fit(g2s,"NR+");
  g1s->GetParameters(&pars[0]);
  g2s->GetParameters(&pars[3]);
  totals->SetParameters(pars);
  res3p_Moore->Fit(totals,"R+");
  res3p_Moore->Draw("");
  fres3p_Moore->SetLineColor(1);
  fres3p_Moore->SetLineWidth(2);
  fres3p_Moore->Draw("same");
  c0->Print("plotPtRel_sa.eps");

  c01 = new TCanvas("c01","  ",200,10,700,500);
  c01->Divide(1,1);
  c01->cd(1) ;
  pull3invp_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull3invp_Moore->GetXaxis()->SetTitleColor(4) ;
  pull3invp_Moore->SetXTitle(" delta q/p / error ");
  pull3invp_Moore->SetLineColor(4);
  pull3invp_Moore->SetLineWidth(2);
  pull3invp_Moore->Draw("");
  fpull3invp_Moore->SetLineColor(1);
  fpull3invp_Moore->SetLineWidth(2);
  fpull3invp_Moore->Draw("same");
  TLegend* leg0 = new TLegend(0.6,0.75,0.9,0.9);
  leg0->SetFillColor(0);
  leg0->SetTextSize(.07);
  leg0->AddEntry(pull4invp_Moore," Muid SA ","L");
  leg0->AddEntry(fpull4invp_Moore," forward ","L");
  leg0->Draw("same");
  fpull3invp_Moore->Draw("same");
  c01->Print("plotDeltaQP_sa.eps");


  c02 = new TCanvas("c02","  ",200,10,700,500);
  c02->Divide(1,1);
  c02->cd(1) ;
  etaIP3_mis_Moore->GetXaxis()->SetTitleSize(0.07) ;
  etaIP3_mis_Moore->GetXaxis()->SetTitleColor(4) ;
  etaIP3_mis_Moore->SetXTitle(" Eta missed tracks ");
  etaIP3_mis_Moore->SetLineColor(2);
  etaIP3_mis_Moore->SetLineWidth(4);
  etaIP3_mis_Moore->Draw("");
  c02->Print("plotEtaMiss_sa.eps");


  c021 = new TCanvas("c021","  ",200,10,700,500);
  c021->Divide(1,1);
  c021->cd(1) ;
  TH1D *eta_eff_Moore = new TH1D("eta_eff_Moore"," ",100,-5.,5.);  
  eta_eff_Moore->Sumw2();
  eta_eff_Moore->Add(etaIP_sim_Moore,etaIP3_mis_Moore,1.,-1.);
  eta_eff_Moore->Divide(eta_eff_Moore,etaIP_sim_Moore,1.,1.,"B");
  eta_eff_Moore->GetXaxis()->SetTitleSize(0.07) ;
  eta_eff_Moore->GetXaxis()->SetTitleColor(4) ;
  eta_eff_Moore->SetLineWidth(4);
  eta_eff_Moore->SetLineColor(2);
  eta_eff_Moore->SetXTitle(" eta ");
  eta_eff_Moore->SetYTitle(" Efficiency ");
  eta_eff_Moore->SetMinimum(0.);
  eta_eff_Moore->SetMaximum(1.);
  eta_eff_Moore->SetAxisRange(-3.,3.);
  eta_eff_Moore->Draw("");
  c021->Print("plotEtaEff_sa.eps");


  c03 = new TCanvas("c03","  ",200,10,700,500);
  c03->Divide(2,3);

  c03->cd(1) ;
  pull3d0_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull3d0_Moore->GetXaxis()->SetTitleColor(4) ;
  pull3d0_Moore->SetXTitle(" pull d0 ");
  pull3d0_Moore->SetLineColor(4);
  pull3d0_Moore->SetLineWidth(2);
  pull3d0_Moore->Draw("");
  fpull3d0_Moore->SetLineColor(1);
  fpull3d0_Moore->SetLineWidth(2);
  fpull3d0_Moore->Draw("same");


  c03->cd(2) ;
  pull3phi_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull3phi_Moore->GetXaxis()->SetTitleColor(4) ;
  pull3phi_Moore->SetXTitle(" pull phi ");
  pull3phi_Moore->SetLineColor(4);
  pull3phi_Moore->SetLineWidth(2);
  pull3phi_Moore->Draw("");
  fpull3phi_Moore->SetLineColor(1);
  fpull3phi_Moore->SetLineWidth(2);
  fpull3phi_Moore->Draw("same");

  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.1);
  leg->AddEntry(pull3phi_Moore," Muid SA ","L");
  leg->AddEntry(fpull3phi_Moore," forward ","L");
  leg->Draw("same");


  c03->cd(3) ;
  pull3z0_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull3z0_Moore->GetXaxis()->SetTitleColor(4) ;
  pull3z0_Moore->SetXTitle(" pull z0 ");
  pull3z0_Moore->SetLineColor(4);
  pull3z0_Moore->SetLineWidth(2);
  pull3z0_Moore->Draw("");
  fpull3z0_Moore->SetLineColor(1);
  fpull3z0_Moore->SetLineWidth(2);
  fpull3z0_Moore->Draw("same");

  c03->cd(4) ;
  pull3theta_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull3theta_Moore->GetXaxis()->SetTitleColor(4) ;
  pull3theta_Moore->SetXTitle(" pull theta ");
  pull3theta_Moore->SetLineColor(4);
  pull3theta_Moore->SetLineWidth(2);
  pull3theta_Moore->Draw("");
  fpull3theta_Moore->SetLineColor(1);
  fpull3theta_Moore->SetLineWidth(2);
  fpull3theta_Moore->Draw("same");

  c03->cd(5) ;
  pull3invp_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull3invp_Moore->GetXaxis()->SetTitleColor(4) ;
  pull3invp_Moore->SetXTitle(" pull q/p ");
  pull3invp_Moore->SetLineColor(4);
  pull3invp_Moore->SetLineWidth(2);
  pull3invp_Moore->Draw("");
  fpull3invp_Moore->SetLineColor(1);
  fpull3invp_Moore->SetLineWidth(2);
  fpull3invp_Moore->Draw("same");
  c03->Print("plotPulls_sa.eps");

  c04 = new TCanvas("c04","  ",200,10,700,500);
  c04->Divide(2,2);
  
  TVirtualPad* Pad=c04->cd(1) ;
  Pad->SetLogy();
  c04->cd(1) ;
//  chi20s_Moore->SetStats(1);
  chi23s_Moore->GetXaxis()->SetTitleSize(0.07) ;
  chi23s_Moore->GetXaxis()->SetTitleColor(4) ;
  chi23s_Moore->SetXTitle(" chi2 IP simple ");
  chi23s_Moore->SetLineColor(4);
  chi23s_Moore->SetLineWidth(2);
  chi23s_Moore->Draw("");
  fchi23s_Moore->SetLineColor(1);
  fchi23s_Moore->SetLineWidth(2);
  fchi23s_Moore->Draw("same");

  TVirtualPad* Pad=c04->cd(2) ;
  Pad->SetLogy();
//  chi20f_Moore->SetStats(1);
  c04->cd(2) ;
  chi23r_Moore->GetXaxis()->SetTitleSize(0.07) ;
  chi23r_Moore->GetXaxis()->SetTitleColor(4) ;
  chi23r_Moore->SetXTitle(" chi2 comb ");
  chi23r_Moore->SetLineColor(4);
  chi23r_Moore->SetLineWidth(2);
  chi23r_Moore->Draw("");
  fchi23r_Moore->SetLineColor(1);
  fchi23r_Moore->SetLineWidth(2);
  fchi23r_Moore->Draw("same");
  TLegend* leg = new TLegend(0.50,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.1);
  leg->AddEntry(chi23r_Moore," Muid CB ","L");
  leg->AddEntry(fchi23r_Moore," forward ","L");
  leg->Draw("same");
  
  c04->cd(3) ;
  eta3ts_Moore->SetStats(1);
  eta3ts_Moore->GetXaxis()->SetTitleSize(0.07) ;
  eta3ts_Moore->GetXaxis()->SetTitleColor(4) ;
  eta3ts_Moore->SetXTitle(" eta tail chi2 simple ");
  eta3ts_Moore->SetLineColor(4);
  eta3ts_Moore->SetLineWidth(2);
  eta3ts_Moore->Draw("");

  c04->cd(4) ;
  eta3tr_Moore->SetStats(1);
  eta3tr_Moore->GetXaxis()->SetTitleSize(0.07) ;
  eta3tr_Moore->GetXaxis()->SetTitleColor(4) ;
  eta3tr_Moore->SetXTitle(" eta tail chi2 comb ");
  eta3tr_Moore->SetLineColor(4);
  eta3tr_Moore->SetLineWidth(2);
  eta3tr_Moore->Draw("");
  c04->Print("plotChi2Tail_sa.eps");


  c1 = new TCanvas("c1","  ",200,10,700,500);
  c1->Divide(1,1);
  c1->cd(1) ;
  res4p_Moore->GetXaxis()->SetTitleSize(0.07) ;
  res4p_Moore->GetXaxis()->SetTitleColor(4) ;
  res4p_Moore->SetXTitle(" Delta pt / pt ");
  res4p_Moore->SetLineColor(4);
  res4p_Moore->SetLineWidth(2);
  //Single gaus does not reproduce shape fully, although better than MuidSA case
  //res4p_Moore->Fit("gaus","","",-0.5,0.5);
  Double_t parc[6];
  TF1* g1c    = new TF1("g1c","gaus",-0.05,0.05);
  TF1* g2c    = new TF1("g2c","gaus",-0.5,0.5);
  TF1* totalc = new TF1("totalc","gaus(0)+gaus(3)",-0.5,0.5);
  totalc->SetLineColor(2);
  totalc->SetLineWidth(1);
  res4p_Moore->Fit(g1c,"RN");
  res4p_Moore->Fit(g2c,"NR+");
  g1c->GetParameters(&parc[0]);
  g2c->GetParameters(&parc[3]);
  totalc->SetParameters(parc);
  res4p_Moore->Fit(totalc,"R+");
  res4p_Moore->Draw("");
  fres4p_Moore->SetLineColor(1);
  fres4p_Moore->SetLineWidth(2);
  fres4p_Moore->Draw("same");
  c1->Print("plotPtRel_comb.eps");

  c11 = new TCanvas("c11","  ",200,10,700,500);
  c11->Divide(1,1);
  c11->cd(1) ;
  pull4invp_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull4invp_Moore->GetXaxis()->SetTitleColor(4) ;
  pull4invp_Moore->SetXTitle(" delta q/p / error ");
  pull4invp_Moore->SetLineColor(4);
  pull4invp_Moore->SetLineWidth(2);
  pull4invp_Moore->Draw("");
  fpull4invp_Moore->SetLineColor(1);
  fpull4invp_Moore->SetLineWidth(2);
  fpull4invp_Moore->Draw("same");
  TLegend* leg0 = new TLegend(0.6,0.75,0.9,0.9);
  leg0->SetFillColor(0);
  leg0->SetTextSize(.07);
  leg0->AddEntry(pull4invp_Moore," Muid CB ","L");
  leg0->AddEntry(fpull4invp_Moore," forward ","L");
  leg0->Draw("same");
  fpull4invp_Moore->Draw("same");
  c11->Print("plotDeltaQP_comb.eps");
   
  
  c2 = new TCanvas("c2","  ",200,10,700,500);
  c2->Divide(1,1);
  c2->cd(1) ;
  etaIP4_mis_Moore->GetXaxis()->SetTitleSize(0.07) ;
  etaIP4_mis_Moore->GetXaxis()->SetTitleColor(4) ;
  etaIP4_mis_Moore->SetXTitle(" Eta missed tracks ");
  etaIP4_mis_Moore->SetLineColor(2);
  etaIP4_mis_Moore->SetLineWidth(4);
  etaIP4_mis_Moore->Draw("");
  c2->Print("plotEtaMiss_comb.eps");

  c21 = new TCanvas("c21","  ",200,10,700,500);
  c21->Divide(1,1);
  c21->cd(1) ;
  TH1D *eta_eff_Moore = new TH1D("eta_eff_Moore"," ",100,-5.,5.);  
  eta_eff_Moore->Sumw2();
  eta_eff_Moore->Add(etaIP_sim_Moore,etaIP4_mis_Moore,1.,-1.);
  eta_eff_Moore->Divide(eta_eff_Moore,etaIP_sim_Moore,1.,1.,"B");
  eta_eff_Moore->GetXaxis()->SetTitleSize(0.07) ;
  eta_eff_Moore->GetXaxis()->SetTitleColor(4) ;
  eta_eff_Moore->SetLineWidth(4);
  eta_eff_Moore->SetLineColor(2);
  eta_eff_Moore->SetXTitle(" eta ");
  eta_eff_Moore->SetYTitle(" Efficiency ");
  eta_eff_Moore->SetMinimum(0.);
  eta_eff_Moore->SetMaximum(1.);
  eta_eff_Moore->SetAxisRange(-3.,3.);
  eta_eff_Moore->Draw("");
  c21->Print("plotEtaEff_comb.eps");

  c222 = new TCanvas("c222","  ",200,10,700,500);
  c222->Divide(1,1);
  TVirtualPad* Pad=c222->cd(1) ;
  Pad->SetLogy();
  tracksIPFake_Moore->GetXaxis()->SetTitleSize(0.07) ;
  tracksIPFake_Moore->GetXaxis()->SetTitleColor(4) ;
  tracksIPFake_Moore->SetXTitle(" number of fake tracks ");
  tracksIPFake_Moore->SetLineColor(2);
  tracksIPFake_Moore->SetLineWidth(4);
  tracksIPFake_Moore->Draw("");
  c222->Print("plotNfakes_sa.eps");

  c22 = new TCanvas("c22","  ",200,10,700,500);
  c22->Divide(1,1);
  TVirtualPad* Pad=c22->cd(1) ;
  Pad->SetLogy();
  tracksCombFake_Moore->GetXaxis()->SetTitleSize(0.07) ;
  tracksCombFake_Moore->GetXaxis()->SetTitleColor(4) ;
  tracksCombFake_Moore->SetXTitle(" number of fake tracks ");
  tracksCombFake_Moore->SetLineColor(2);
  tracksCombFake_Moore->SetLineWidth(4);
  tracksCombFake_Moore->Draw("");
  c22->Print("plotNfakes_comb.eps");
  
 
  c10 = new TCanvas("c10","  ",200,10,700,500);
  c10->Divide(2,3);

  c10->cd(1) ;
  pull4d0_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull4d0_Moore->GetXaxis()->SetTitleColor(4) ;
  pull4d0_Moore->SetXTitle(" pull d0 ");
  pull4d0_Moore->SetLineColor(4);
  pull4d0_Moore->SetLineWidth(2);
  pull4d0_Moore->Draw("");
  fpull4d0_Moore->SetLineColor(1);
  fpull4d0_Moore->SetLineWidth(2);
  fpull4d0_Moore->Draw("same");


  c10->cd(2) ;
  pull4phi_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull4phi_Moore->GetXaxis()->SetTitleColor(4) ;
  pull4phi_Moore->SetXTitle(" pull phi ");
  pull4phi_Moore->SetLineColor(4);
  pull4phi_Moore->SetLineWidth(2);
  pull4phi_Moore->Draw("");
  fpull4phi_Moore->SetLineColor(1);
  fpull4phi_Moore->SetLineWidth(2);
  fpull4phi_Moore->Draw("same");

  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.1);
  leg->AddEntry(pull4phi_Moore," Muid CB ","L");
  leg->AddEntry(fpull4phi_Moore," forward ","L");
  leg->Draw("same");


  c10->cd(3) ;
  pull4z0_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull4z0_Moore->GetXaxis()->SetTitleColor(4) ;
  pull4z0_Moore->SetXTitle(" pull z0 ");
  pull4z0_Moore->SetLineColor(4);
  pull4z0_Moore->SetLineWidth(2);
  pull4z0_Moore->Draw("");
  fpull4z0_Moore->SetLineColor(1);
  fpull4z0_Moore->SetLineWidth(2);
  fpull4z0_Moore->Draw("same");

  c10->cd(4) ;
  pull4theta_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull4theta_Moore->GetXaxis()->SetTitleColor(4) ;
  pull4theta_Moore->SetXTitle(" pull theta ");
  pull4theta_Moore->SetLineColor(4);
  pull4theta_Moore->SetLineWidth(2);
  pull4theta_Moore->Draw("");
  fpull4theta_Moore->SetLineColor(1);
  fpull4theta_Moore->SetLineWidth(2);
  fpull4theta_Moore->Draw("same");

  c10->cd(5) ;
  pull4invp_Moore->GetXaxis()->SetTitleSize(0.07) ;
  pull4invp_Moore->GetXaxis()->SetTitleColor(4) ;
  pull4invp_Moore->SetXTitle(" pull q/p ");
  pull4invp_Moore->SetLineColor(4);
  pull4invp_Moore->SetLineWidth(2);
  pull4invp_Moore->Draw("");
  fpull4invp_Moore->SetLineColor(1);
  fpull4invp_Moore->SetLineWidth(2);
  fpull4invp_Moore->Draw("same");
  c10->Print("plotPulls_comb.eps");

  c11 = new TCanvas("c11","  ",200,10,700,500);
  c11->Divide(2,2);
  
  TVirtualPad* Pad=c11->cd(1) ;
  Pad->SetLogy();
  c11->cd(1) ;
//  chi20s_Moore->SetStats(1);
  chi24s_Moore->GetXaxis()->SetTitleSize(0.07) ;
  chi24s_Moore->GetXaxis()->SetTitleColor(4) ;
  chi24s_Moore->SetXTitle(" chi2 IP simple ");
  chi24s_Moore->SetLineColor(4);
  chi24s_Moore->SetLineWidth(2);
  chi24s_Moore->Draw("");
  fchi24s_Moore->SetLineColor(1);
  fchi24s_Moore->SetLineWidth(2);
  fchi24s_Moore->Draw("same");

  TVirtualPad* Pad=c11->cd(2) ;
  Pad->SetLogy();
//  chi20f_Moore->SetStats(1);
  c11->cd(2) ;
  chi24r_Moore->GetXaxis()->SetTitleSize(0.07) ;
  chi24r_Moore->GetXaxis()->SetTitleColor(4) ;
  chi24r_Moore->SetXTitle(" chi2 comb ");
  chi24r_Moore->SetLineColor(4);
  chi24r_Moore->SetLineWidth(2);
  chi24r_Moore->Draw("");
  fchi24r_Moore->SetLineColor(1);
  fchi24r_Moore->SetLineWidth(2);
  fchi24r_Moore->Draw("same");
  TLegend* leg = new TLegend(0.50,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.1);
  leg->AddEntry(chi24r_Moore," Muid CB ","L");
  leg->AddEntry(fchi24r_Moore," forward ","L");
  leg->Draw("same");
  
  c11->cd(3) ;
  eta4ts_Moore->SetStats(1);
  eta4ts_Moore->GetXaxis()->SetTitleSize(0.07) ;
  eta4ts_Moore->GetXaxis()->SetTitleColor(4) ;
  eta4ts_Moore->SetXTitle(" eta tail chi2 simple ");
  eta4ts_Moore->SetLineColor(4);
  eta4ts_Moore->SetLineWidth(2);
  eta4ts_Moore->Draw("");

  c11->cd(4) ;
  eta4tr_Moore->SetStats(1);
  eta4tr_Moore->GetXaxis()->SetTitleSize(0.07) ;
  eta4tr_Moore->GetXaxis()->SetTitleColor(4) ;
  eta4tr_Moore->SetXTitle(" eta tail chi2 comb ");
  eta4tr_Moore->SetLineColor(4);
  eta4tr_Moore->SetLineWidth(2);
  eta4tr_Moore->Draw("");
  c11->Print("plotChi2Tail_comb.eps");

  //mutag
  c12 = new TCanvas("c12","  ",200,10,700,500);
  c12->Divide(2,1);
  
  c12->cd(1) ;
  eta_mis_MuTagMoore->GetXaxis()->SetTitleSize(0.07) ;
  eta_mis_MuTagMoore->GetXaxis()->SetTitleColor(4) ;
  eta_mis_MuTagMoore->SetXTitle(" Eta missed MuTagIMO tracks ");
  eta_mis_MuTagMoore->SetLineColor(2);
  eta_mis_MuTagMoore->SetLineWidth(4);
  eta_mis_MuTagMoore->Draw("");
  
  c12->cd(2) ;
  TH1D *eta_eff_MuTagMoore = new TH1D("eta_eff_MuTagIMO"," ",100,-5.,5.);  
  eta_eff_MuTagMoore->Sumw2();
  eta_eff_MuTagMoore->Add(eta_sim_MuTagMoore,eta_mis_MuTagMoore,1.,-1.);
  eta_eff_MuTagMoore->Divide(eta_eff_MuTagMoore,eta_sim_MuTagMoore,1.,1.,"B");
  eta_eff_MuTagMoore->GetXaxis()->SetTitleSize(0.07) ;
  eta_eff_MuTagMoore->GetXaxis()->SetTitleColor(4) ;
  eta_eff_MuTagMoore->SetLineWidth(4);
  eta_eff_MuTagMoore->SetLineColor(2);
  eta_eff_MuTagMoore->SetMinimum(0);  
  eta_eff_MuTagMoore->SetMaximum(1.1);
  eta_eff_MuTagMoore->SetXTitle(" eta ");
  eta_eff_MuTagMoore->SetYTitle(" Efficiency ");
  eta_eff_MuTagMoore->Draw("");

  c12->Print("plotPerfo_mutag.eps");

  
  c122 = new TCanvas("c122","  ",200,10,700,500);
  c122->cd(1) ;
  TVirtualPad* Pad=c122->cd(1) ;
  Pad->SetLogy();
  tracksFake_MuTagMoore->GetXaxis()->SetTitleSize(0.07) ;
  tracksFake_MuTagMoore->GetXaxis()->SetTitleColor(4) ;
  tracksFake_MuTagMoore->SetXTitle(" number of fake tracks ");
  tracksFake_MuTagMoore->SetLineColor(2);
  tracksFake_MuTagMoore->SetLineWidth(4);
  tracksFake_MuTagMoore->Draw("");
  
  c122->Print("plotFake_mutag.eps");


  c121 = new TCanvas("c121","  ",200,10,700,500);
  c121->Divide(1,1);
  c121->cd(1) ;
  mdtSegmentsReco_MuTagMoore->GetXaxis()->SetTitleSize(0.07) ;
  mdtSegmentsReco_MuTagMoore->GetXaxis()->SetTitleColor(4) ;
  mdtSegmentsReco_MuTagMoore->SetXTitle(" MDT segments hit ");
  mdtSegmentsReco_MuTagMoore->SetLineColor(2);
  mdtSegmentsReco_MuTagMoore->Draw("");
  mdtSegmentsTruth_MuTagMoore->SetLineColor(1);
  mdtSegmentsTruth_MuTagMoore->SetLineWidth(4);
  mdtSegmentsTruth_MuTagMoore->Draw("same");
  mdtSegmentsMatch_MuTagMoore->SetLineWidth(4);
  mdtSegmentsMatch_MuTagMoore->SetLineColor(2);
  mdtSegmentsMatch_MuTagMoore->Draw("same");
  mdtSegmentsSMatch_MuTagMoore->SetLineWidth(2);
  mdtSegmentsSMatch_MuTagMoore->SetLineColor(3);
  mdtSegmentsSMatch_MuTagMoore->Draw("same");
  mdtSegmentsPMatch_MuTagMoore->SetLineWidth(4);
  mdtSegmentsPMatch_MuTagMoore->SetLineColor(4);
  mdtSegmentsPMatch_MuTagMoore->Draw("same");
  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(mdtSegmentsTruth_MuTagMoore," Simulation","L");
  leg->AddEntry(mdtSegmentsReco_MuTagMoore," MuTagMoore track ","L");
  leg->AddEntry(mdtSegmentsSMatch_MuTagMoore," Moore segments ","L");
  leg->AddEntry(mdtSegmentsPMatch_MuTagMoore," Moore patterns ","L");
  leg->Draw("same");

  c121->Print("plotSegments_mutag.eps");

  c120 = new TCanvas("c120","  ",200,10,700,500);
  mdtSegmentsMissed_MuTagMoore->GetXaxis()->SetTitleSize(0.07) ;
  mdtSegmentsMissed_MuTagMoore->GetXaxis()->SetTitleColor(4) ;
  mdtSegmentsMissed_MuTagMoore->SetXTitle("missed MDT segments/event");
  mdtSegmentsMissed_MuTagMoore->SetLineColor(2);
  mdtSegmentsMissed_MuTagMoore->SetLineWidth(4);
  mdtSegmentsMissed_MuTagMoore->GetXaxis()->SetRange(2,24);
  Double_t max = mdtSegmentsMissed_MuTagMoore->GetMaximum();
  mdtSegmentsMissed_MuTagMoore->SetMaximum(1.25*max);
  mdtSegmentsMissed_MuTagMoore->Draw();
  TText *tt1 = new TText(1.5,1.15*max,"Inner Middle Outer bits 0 1 2");
  TText *tt2 = new TText(8.,1.05*max,"|" );
  TText *tt3 = new TText(16.,1.05*max,"|" );
  TText *tt4 = new TText(1.,1.05*max," Barrel          B-E        Endcap" );
  tt1->Draw("same");
  tt2->Draw("same");
  tt3->Draw("same");
  tt4->Draw("same");
  c120->Print("plotSegmentsMissed_mutag.eps");


  //Merged (OR) plots for validation of what's in AOD's
  c13 = new TCanvas("c13","  ",200,10,700,500);
  c13->Divide(2,1);
  
  c13->cd(1) ;
  eta_mis_MergedMoore->GetXaxis()->SetTitleSize(0.07) ;
  eta_mis_MergedMoore->GetXaxis()->SetTitleColor(4) ;
  eta_mis_MergedMoore->SetXTitle(" Eta missed Merged tracks ");
  eta_mis_MergedMoore->SetLineColor(2);
  eta_mis_MergedMoore->SetLineWidth(4);
  eta_mis_MergedMoore->Draw("");
  
  c13->cd(2) ;
  TH1D *eta_eff_MergedMoore = new TH1D("eta_eff_Merged"," ",100,-5.,5.);  
  eta_eff_MergedMoore->Sumw2();
  eta_eff_MergedMoore->Add(eta_sim_MergedMoore,eta_mis_MergedMoore,1.,-1.);
  eta_eff_MergedMoore->Divide(eta_eff_MergedMoore,eta_sim_MergedMoore,1.,1.,"B");
  eta_eff_MergedMoore->GetXaxis()->SetTitleSize(0.07) ;
  eta_eff_MergedMoore->GetXaxis()->SetTitleColor(4) ;
  eta_eff_MergedMoore->SetLineWidth(4);
  eta_eff_MergedMoore->SetLineColor(2);
  eta_eff_MergedMoore->SetMinimum(0);  
  eta_eff_MergedMoore->SetMaximum(1.1);
  eta_eff_MergedMoore->SetXTitle(" eta ");
  eta_eff_MergedMoore->SetYTitle(" Efficiency ");
  eta_eff_MergedMoore->Draw("");
  c13->Print("plotMergedEff.eps");

  c8 = new TCanvas("c8","  ",200,10,700,500);
  c8->cd(1) ;
  TVirtualPad* Pad=c8->cd(1) ;
  Pad->SetLogy();
  tracksFake_MergedMoore->GetXaxis()->SetTitleSize(0.07) ;
  tracksFake_MergedMoore->GetXaxis()->SetTitleColor(4) ;
  tracksFake_MergedMoore->SetXTitle(" number of fake tracks ");
  tracksFake_MergedMoore->SetLineColor(2);
  tracksFake_MergedMoore->SetLineWidth(4);
  tracksFake_MergedMoore->Draw("");
  c8->Print("plotMergedFake.eps");

  //summary table
  int ntruthTracks = eta_sim_Moore->GetEntries();
  int nmissedMoore = eta_mis_Moore->GetEntries();
  int ntruthTracks5 = eta_simpt5_Moore->GetEntries();
  int nmissedMoore5 = eta_mispt5_Moore->GetEntries();

  int ntruthTracksIP = 1;
  int nmissedMooreIP = 1;
  int ntruthTracksIP5 = 1;
  int nmissedMooreIP5 = 1;

  ntruthTracksIP = etaIP_sim_Moore->GetEntries();
  nmissedMooreIP = etaIP3_mis_Moore->GetEntries();
  ntruthTracksIP5 = etaIP_simpt5_Moore->GetEntries();
  nmissedMooreIP5 = etaIP3_mispt5_Moore->GetEntries();
 
  double effMoore = (ntruthTracks-nmissedMoore)/(double)ntruthTracks;
  double effMoore5 = (ntruthTracks5-nmissedMoore5)/(double)ntruthTracks5;

  double effMuid = (ntruthTracksIP-nmissedMooreIP)/(double)ntruthTracksIP;
  double effMuid5 = (ntruthTracksIP5-nmissedMooreIP5)/(double)ntruthTracksIP5;

  double effCB = 0;
  double effMuonCB = 0;
  double effCB5 = 0;
  double effMuonCB5 = 0;

  double effTag = 0;
  double effTag5 = 0;
  double effAll = 0;
  double effAll5 = 0;
  double effMerged = 0;
  double effMerged5 = 0;
  double effSAMerged = 0;
  double effCBMerged = 0;
  double effTagMerged = 0;
  double effSAMerged5 = 0;
  double effCBMerged5 = 0;
  double effTagMerged5 = 0;
  int ntruthTracksNoMuoncut = eta_allsim_Moore->GetEntries();
  int ntruthTracks5NoMuoncut = eta_allsimpt5_Moore->GetEntries();
  double corMS = ntruthTracks5/ (double) ntruthTracks5NoMuoncut;
  double corGen = 1.;

  int ntruthTracksIPa = eta_sim_AllMoore->GetEntries();
  int ntruthTracksIPa5 = eta_simpt5_AllMoore->GetEntries();
  int ntruthTracksIPCB = eta_sim_CBMoore->GetEntries();
  int ntruthTracksIPMC = eta_sim_MCMoore->GetEntries();
  int nmissedCBMoore = eta_mis_CBMoore->GetEntries();
  int nmissedMuonCBMoore = eta_mis_MCMoore->GetEntries();
  int nmissedCBMoore5 = eta_mispt5_CBMoore->GetEntries();
  int nmissedMuonCBMoore5 = eta_mispt5_MCMoore->GetEntries();
  int ntruthTracksIPCB5 = eta_simpt5_CBMoore->GetEntries();
  int ntruthTracksIPMC5 = eta_simpt5_MCMoore->GetEntries();
  effCB = (-nmissedCBMoore+ntruthTracksIPCB )/(double)ntruthTracksIPa;
  effMuonCB = (-nmissedMuonCBMoore+ntruthTracksIPMC )/(double)ntruthTracksIPa;
  effCB5 = (-nmissedCBMoore5+ntruthTracksIPCB5)/(double)ntruthTracksIPa5;
  effMuonCB5 = (-nmissedMuonCBMoore5+ntruthTracksIPMC5)/(double)ntruthTracksIPa5;
  
  int ntruthTracksIPTag = eta_sim_MuTagMoore->GetEntries();
  int nmissedTagMoore = eta_mis_MuTagMoore->GetEntries();
  int nmissedTagMoore5 = eta_mispt5_MuTagMoore->GetEntries();
  int ntruthTracksIPTag5 = eta_simpt5_MuTagMoore->GetEntries();
  effTag = (-nmissedTagMoore+ntruthTracksIPTag)/(double)ntruthTracksIPa;
  effTag5 = (-nmissedTagMoore5+ntruthTracksIPTag5)/(double)ntruthTracksIPa5;
  
  int nmissedAllMoore = eta_mis_AllMoore->GetEntries();
  int nmissedAllMoore5 = 0; 
  
  effAll = (ntruthTracksIPa-nmissedAllMoore-0.001)/(double)ntruthTracksIPa;
  effAll5 = (ntruthTracksIPa5-nmissedAllMoore5-0.001)/(double)ntruthTracksIPa5;
  
  int nmissedMergedMoore = eta_mis_MergedMoore->GetEntries();
  int nmissedMergedMoore5 = 0;
  
  nmissedMergedMoore5 = eta_mispt5_MergedMoore->GetEntries();
  
  effMerged = (ntruthTracksIPa-nmissedMergedMoore)/(double)ntruthTracksIPa;
  effMerged5 = (ntruthTracksIPa5-nmissedMergedMoore5)/(double)ntruthTracksIPa5;
  effCBMerged = eta_recoCB_MergedMoore->GetEntries()/(double)ntruthTracksIPa;
  effSAMerged = eta_recoSA_MergedMoore->GetEntries()/(double)ntruthTracksIPa;
  effTagMerged = eta_recoTag_MergedMoore->GetEntries()/(double)ntruthTracksIPa;
  effCBMerged5 = eta_recopt5CB_MergedMoore->GetEntries()/(double)ntruthTracksIPa5;
  effSAMerged5 = eta_recopt5SA_MergedMoore->GetEntries()/(double)ntruthTracksIPa5;
  effTagMerged5 = eta_recopt5Tag_MergedMoore->GetEntries()/(double)ntruthTracksIPa5;
 
  int ntruthTracksIPa5NoMuoncut = eta_allsimpt5_AllMoore->GetEntries();
  corGen = ntruthTracksIPa5/ (double) ntruthTracksIPa5NoMuoncut;
  corGen = ntruthTracks5/ (double) ntruthTracks5NoMuoncut;  
  int nevts= tracksFake_Moore->GetEntries();

  std::cout<< "                                  " <<std::endl; 
  std::cout<< " -----> PERFORMANCE SUMMARY TABLE " << std::endl;

  std::cout<< " events " << nevts << " all muons  " << ntruthTracksNoMuoncut << " above pt 5 GeV "<<  ntruthTracks5NoMuoncut << " in MS " << ntruthTracksIPa << " in MS above pt 5 GeV " << ntruthTracksIPa5 << std::endl; 

  std::cout<< "                                  " <<std::endl; 
  std::cout<< " ***** EFFICIENCY ***** " << std::endl;
  std::cout << "        eff Moore   MuidSA    MuidCB    MutagIMO    MuonCB     all      Merged    CBMerged    SAMerged     TagMerged"  << std::endl; 
  std::cout << "          MS            IP                                  "  << std::endl; 
  std::cout << " I      " << effMoore << "  " << effMuid << "  " << effCB << "   " << effTag << "   " << effMuonCB << "   " << effAll  << "   " << effMerged << "   " << effCBMerged << "   " << effSAMerged << "   " << effTagMerged << std::endl; 
  std::cout << " II     " << effMoore5 << "  " << effMuid5 << "  " << effCB5 << "   " << effTag5 << "   " << effMuonCB5 << "   " << effAll5  << "   " << effMerged5 <<  "   " << effCBMerged5 << "   " << effSAMerged5 << "   " << effTagMerged5  << std::endl; 
  std::cout << " III    " << corMS*effMoore5 << "  " << corGen*effMuid5 << "  " << corGen*effCB5 << "   " << corGen*effTag5 << "   " << corGen*effMuonCB5 << "   " << corGen*effAll5  << "   " << corGen*effMerged5 << std::endl; 
  
  std::cout<< "                                  " <<std::endl; 
  std::cout <<  " MS hits = mdt+csc truth hits > 0; simulation selection abs(eta) < 2.8 " << std::endl;
  std::cout << " cut I:    (MS no pt cut) and MS hits " << std::endl;  
  std::cout << " cut II:  (MS pt  > 5 GeV  IP pt  > 5 GeV) and MS hits " << std::endl; 
  std::cout << " cut III: (MS pt  > 5 GeV  IP pt  > 5 GeV)  ==  Physics efficiency including eta = 0 hole " << std::endl;  

  int nfakeMoore  =  eta_fake_Moore->GetEntries();
  int nfakeMoore5  =  0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_Moore");
  if (tobj) {
    nfakeMoore5  =  eta_fakept5_Moore->GetEntries();
  }
  double rfakeMoore = nfakeMoore/(double) nevts;
  double rfakeMoore5 = nfakeMoore5/(double) nevts;
  int nfakeMuid = 0;
  int nfakeMuid5 = 0;
  tobj =  (TH1F*)f1.FindObjectAny("CtracksIPFake_Moore");
  if (tobj) {
    nfakeMuid = CtracksIPFake_Moore->GetEntries();
  }
  tobj =  (TH1F*)f1.FindObjectAny("CtracksIP5Fake_Moore");
  if (tobj) {
    nfakeMuid5 = CtracksIP5Fake_Moore->GetEntries();
  }
  double rfakeMuid = nfakeMuid/(double) nevts; 
  double rfakeMuid5 = nfakeMuid5/(double) nevts; 

  double rfakeCB = 0.;
  double rfakeMuonCB = 0.;
  double rfakeCB5 = 0.;
  double rfakeMuonCB5 = 0.;

  double rfakeTag = 0.;
  double rfakeTag5 = 0.;
  double rfakeAll = 0.;
  double rfakeAll5 = 0.;
  double rfakeMerged = 0.;
  double rfakeMerged5 = 0.;
  double rfakeTagMerged = 0.;
  double rfakeTagMerged5 = 0.;
  double rfakeSAMerged = 0.;
  double rfakeSAMerged5 = 0.;
  double rfakeCBMerged = 0.;
  double rfakeCBMerged5 = 0.;

  int nfakeCBMoore  = 0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fake_CBMoore");
  if (tobj) {
   nfakeCBMoore = eta_fake_CBMoore->GetEntries();
  }
  int nfakeCBMoore5  = 0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_CBMoore");
  if (tobj) {
    nfakeCBMoore5  =  eta_fakept5_CBMoore->GetEntries();
  }
  rfakeCB = nfakeCBMoore/(double) nevts;
  rfakeCB5 = nfakeCBMoore5/(double) nevts;


  int nfakeMuonCBMoore  = 0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fake_MCMoore");
  if (tobj) {
   nfakeMuonCBMoore = eta_fake_MCMoore->GetEntries();
  }
  int nfakeMuonCBMoore5  = 0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_MCMoore");
  if (tobj) {
    nfakeMuonCBMoore5  =  eta_fakept5_MCMoore->GetEntries();
  }
  rfakeMuonCB = nfakeMuonCBMoore/(double) nevts;
  rfakeMuonCB5 = nfakeMuonCBMoore5/(double) nevts;
 
  
  int nfakeMuTagMoore  =  eta_fake_MuTagMoore->GetEntries();
  int nfakeMuTagMoore5  =  0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_MuTagMoore");
  if(tobj) nfakeMuTagMoore5  =  eta_fakept5_MuTagMoore->GetEntries();
  rfakeTag = nfakeMuTagMoore/(double) nevts;
  rfakeTag5 = nfakeMuTagMoore5/(double) nevts;
  
  int nfakeAllMoore  = 0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fake_AllMoore");
  if (tobj) {
    nfakeAllMoore  =  eta_fake_AllMoore->GetEntries();
  }
  int nfakeAllMoore5  = 0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_AllMoore");
  if (tobj) {
    nfakeAllMoore5  =  eta_fakept5_AllMoore->GetEntries();
  }
  rfakeAll = nfakeAllMoore/(double) nevts;
  rfakeAll5 = nfakeAllMoore5/(double) nevts;
  
  int nfakeMerged  = 0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fake_MergedMoore");
  if (tobj) {
    nfakeMerged  =  eta_fake_MergedMoore->GetEntries();
  }  
  int nfakeMerged5  = 0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_MergedMoore");
  if (tobj) nfakeMerged5  =  eta_fakept5_MergedMoore->GetEntries();
  rfakeMerged = nfakeMerged/(double) nevts;
  rfakeMerged5 = nfakeMerged5/(double) nevts;
  int nfakeTagMerged  =  0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fake_TagMergedMoore");
  if(tobj) nfakeTagMerged  =  eta_fake_TagMergedMoore->GetEntries();

  int nfakeTagMerged5  =  0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_TagMergedMoore");
  if(tobj) nfakeTagMerged5  =  eta_fakept5_TagMergedMoore->GetEntries();
  
  rfakeTagMerged = nfakeTagMerged/(double) nevts;
  rfakeTagMerged5 = nfakeTagMerged5/(double) nevts;
  int nfakeSAMerged  =  eta_fake_SAMergedMoore->GetEntries();
  int nfakeSAMerged5  =  0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_SAMergedMoore");
  if(tobj) nfakeSAMerged5  =  eta_fakept5_SAMergedMoore->GetEntries();
  rfakeSAMerged = nfakeSAMerged/(double) nevts;
  rfakeSAMerged5 = nfakeSAMerged5/(double) nevts;
  int nfakeCBMerged  =  eta_fake_CBMergedMoore->GetEntries();
  int nfakeCBMerged5  =  0;
  tobj =  (TH1F*)f1.FindObjectAny("eta_fakept5_CBMergedMoore");
  if (tobj) nfakeCBMerged5  =  eta_fakept5_CBMergedMoore->GetEntries();
  rfakeCBMerged = nfakeCBMerged/(double) nevts;
  rfakeCBMerged5 = nfakeCBMerged5/(double) nevts;
  
  std::cout<< "                                  " <<std::endl; 
  std::cout<< " ***** FAKE RATE ***** " << std::endl;
  std::cout << "       fakes Moore   MuidSA    MuidCB    MutagIMO    MuonCB    all      Merged     CBMerged    SAMerged   TagMerged"  << std::endl; 
  
  std::cout << " I       " << rfakeMoore << "      " << rfakeMuid << "      " << rfakeCB << "       " << rfakeTag << "       " << rfakeMuonCB << "       " << rfakeAll  << "       " << rfakeMerged <<  "     " << rfakeCBMerged << "     " << rfakeSAMerged << "     " << rfakeTagMerged << std::endl; 
  std::cout << " II      " << rfakeMoore5 << "      " << rfakeMuid5 << "      " << rfakeCB5 << "       " << rfakeTag5 << "       " << rfakeMuonCB5 << "       " << rfakeAll5  << "       " << rfakeMerged5 << "     "  << rfakeCBMerged5 << "     " <<rfakeSAMerged5 << "     " << rfakeTagMerged5 << std::endl; 
  


  //Graphical representation of summary tables
  
  TCanvas c77("c77");
  TPaveText pt(.1,.1,.9,.9);
  char gen_txt[500];
  sprintf(gen_txt, "N events: %i   all muons: %i   above Pt 5 GeV: %i   in MS: %i   in MS above pt 5 GeV: %i", nevts, ntruthTracksNoMuoncut, ntruthTracks5NoMuoncut, ntruthTracksIPa, ntruthTracksIPa5);
  TString text_line = TString(gen_txt); 
  pt->AddText(text_line.Data());

  pt->AddText(" MS hits = mdt+csc truth hits > 0; simulation selection abs(eta) < 2.8 ");
  pt->AddText(" cut I:    (MS no pt cut) and MS hits ");
  pt->AddText(" cut II:  (MS pt  > 5 GeV  IP pt  > 5 GeV) and MS hits" );
  pt->AddText(" cut III: (MS pt  > 5 GeV  IP pt  > 5 GeV)  ==  Physics efficiency including eta = 0 hole");

  pt.SetLabel("GENERAL EVENT INFO Table");
  pt.Draw();
  c77->Print("generalTable.eps");  
  
  TCanvas c99("c99");
  TPaveText pt(.1,.1,.9,.9);
  char eff_txt[500];
  sprintf(eff_txt, "I --> Moore: %3.6f   MuidSA: %3.6f   MuidCB: %3.6f   MutagIMO: %3.6f   MuonCB: %3.6f ", effMoore, effMuid, effCB, effTag, effMuonCB);
  TString text_line = TString(eff_txt); 
  pt->AddText(text_line.Data());
  
  sprintf(eff_txt, "I --> all: %3.6f   Merged: %3.6f   CBMerged: %3.6f   SAMerged: %3.6f   TagMerged: %3.6f ", effAll, effMerged, effCBMerged, effSAMerged, effTagMerged);
  TString text_line = TString(eff_txt); 
  pt->AddText(text_line.Data());

  sprintf(eff_txt, "II --> Moore: %3.6f   MuidSA: %3.6f   MuidCB: %3.6f   MutagIMO: %3.6f   MuonCB: %3.6f ", effMoore5, effMuid5, effCB5, effTag5, effMuonCB5);
  TString text_line = TString(eff_txt); 
  pt->AddText(text_line.Data());

  sprintf(eff_txt, "II --> all: %3.6f Merged: %3.6f   CBMerged: %3.6f   SAMerged: %3.6f   TagMerged: %3.6f ", effAll5, effMerged5, effCBMerged5, effSAMerged5, effTagMerged5);
  TString text_line = TString(eff_txt); 
  pt->AddText(text_line.Data());
  
  sprintf(eff_txt, "III --> Moore: %3.6f   MuidSA: %3.6f   MuidCB: %3.6f   MutagIMO: %3.6f   MuonCB: %3.6f ", corMS*effMoore5, corGen*effMuid5, corGen*effCB5, corGen*effTag5, corGen*effMuonCB5);
  TString text_line = TString(eff_txt); 
  pt->AddText(text_line.Data());

  sprintf(eff_txt, "III --> all: %3.6f Merged: %3.6f   CBMerged: %3.6f   SAMerged: %3.6f   TagMerged: %3.6f ", effAll5, effMerged5, effCBMerged5, effSAMerged5, effTagMerged5);
  TString text_line = TString(eff_txt); 

  //pt->SetTextSize(0.02);

  pt->AddText(text_line.Data());
  pt.SetLabel("EFFICIENCY Summary Table");
  pt.Draw();
  c99->Print("efficiencyTable.eps");  

  TCanvas c88("c88");
  TPaveText pt(.1,.1,.9,.9);
  char fake_txt[500];

  sprintf(fake_txt, "I --> Moore: %3.3f   MuidSA: %3.3f   MuidCB: %3.3f   MutagIMO: %3.3f   MuonCB: %3.3f ", rfakeMoore, rfakeMuid, rfakeCB, rfakeTag, rfakeMuonCB);
  TString text_line = TString(fake_txt); 
  pt->AddText(text_line.Data());
  
  sprintf(fake_txt, "I --> all: %3.3f   Merged: %3.3f   CBMerged: %3.3f   SAMerged: %3.3f   TagMerged: %3.3f ", rfakeAll, rfakeMerged, rfakeCBMerged, rfakeSAMerged, rfakeTagMerged);
  TString text_line = TString(fake_txt); 
  pt->AddText(text_line.Data());

  sprintf(fake_txt, "II --> Moore: %3.3f   MuidSA: %3.3f   MuidCB: %3.3f   MutagIMO: %3.3f ", rfakeMoore5, rfakeMuid5, rfakeCB5, rfakeTag5);
  TString text_line = TString(fake_txt); 
  pt->AddText(text_line.Data());

  sprintf(fake_txt, "II --> all: %3.3f Merged: %3.3f   CBMerged: %3.3f   SAMerged: %3.3f   TagMerged: %3.3f ", rfakeAll5, rfakeMerged5, rfakeCBMerged5, rfakeSAMerged5, rfakeTagMerged5);
  TString text_line = TString(fake_txt); 
  //pt->SetTextSize(0.02);

  pt->AddText(text_line.Data());
  pt.SetLabel("FAKES Summary Table");
  pt.Draw();
  c88->Print("fakesTable.eps");  

  gSystem->Exec("./MooRTT_alleps2gif.py");
  
  gSystem->Exec("./MooRTT_summarizeCPU.py");

}
