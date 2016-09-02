/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Maximiliano.Sioli@CERN.CH
// 

#include <TChain.h>

void printH(TH1F *h);

//#include "AtlasUtils.h"

// dataSample1/2 = dataC_50ns, dataD_25ns, mc_50ns, mc_25ns
// probeType1/2 = IDProbes, CaloProbes, MSProbes, TruthProbes_Muon
// muType = MediumMuons, TightMuons

bool effCorrID = true;

void ScaleFactors(TString dataSample1 = "data", TString probeType1 = "IDProbes", TString dataSample2 = "mc-p", TString probeType2 = "IDProbes", TString muType = "MediumMuons") {

  //  gROOT->LoadMacro("AtlasUtils.C");
  
  TString dataBase = "/gpfs_data/local/atlas/sioli/NTuples/v009/";
  
  TFile *f1; TFile *f2;
  TFile *f1_id; TFile *f2_id; // files containing corrections for ID efficiency
  TChain *treeTruth1 = new TChain("JPsiTPReco/Trees/"+probeType1+"_JPsi_OC/TPTree_"+probeType1+"_JPsi_OC");
  TChain *treeTruth2 = new TChain("JPsiTPReco/Trees/"+probeType2+"_JPsi_OC/TPTree_"+probeType2+"_JPsi_OC");  
  cout << "JPsiTPReco/Trees/"+probeType2+"_JPsi_OC/TPTree_"+probeType2+"_JPsi_OC" << endl;

  cout << "Comparison of " << muType << " efficiencies using:" << endl;
  cout << dataSample1 << "/" << probeType1 << " VS " << dataSample2 << "/" << probeType2 << endl;

  if ( probeType1 == "TruthProbes_Muon" ) {
    if (dataSample1 == "mc_50ns") treeTruth1->Add(dataBase+"Jpsimu2mu2-prompt-50ns/*");
    if (dataSample1 == "mc_25ns") treeTruth1->Add(dataBase+"Jpsimu4mu4-prompt-25ns/*");
    /* if (dataSample1 == "mc50np") treeTruth1->Add(dataBase+"Jpsimu2p5mu2p5-nonprompt-50ns/\*"); */
  } else {
    f1 = TFile::Open("EffResults_"+dataSample1+"/"+probeType1+"_"+muType+"/recoeff_etapt_gp2.root");
    f1_id = TFile::Open("EffResults_"+dataSample1+"/MSProbes_ID_IDTracks/recoeff_etapt_gp2.root");
  }

  if ( probeType2 == "TruthProbes_Muon" ) {
    if (dataSample2 == "mc_50ns") treeTruth2->Add(dataBase+"Jpsimu2mu2-prompt-50ns/*");
    if (dataSample2 == "mc_25ns") treeTruth2->Add(dataBase+"Jpsimu4mu4-prompt-25ns/*");
    /* if (dataSample2 == "mc50np") treeTruth2->Add(dataBase+"Jpsimu2p5mu2p5-nonprompt-50ns/\*"); */
  } else {
    f2 = TFile::Open("EffResults_"+dataSample2+"/"+probeType2+"_"+muType+"/recoeff_etapt_gp2.root");
    f2_id = TFile::Open("EffResults_"+dataSample2+"/MSProbes_ID_IDTracks/recoeff_etapt_gp2.root");
  }

  // Binning as from MuonTPTool
  /* double bins_p[] = {0., 0.5, 1., 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 5.5, 6., 6.5, 7., 7.5, 8., 9., 10., 12., 14., 16., 18., 20.}; */
  /* double bins_e[] = {-2.5, -2.25, -2., -1.75, -1.5, -1.25, -1., -0.75, -0.5, -0.25, 0., 0.25, 0.5, 0.75, 1., 1.25, 1.5, 1.75, 2., 2.25, 2.5}; */

  // Temporary binning for week-1
  /* double bins_p[] = {2.5, 3.5, 4.5, 8.5}; */
  /* double bins_e[] = {-2.5, -1.5, -0.5, 0., 0.5, 1.5, 2.5}; */

  // Temporary binning for period C
  /* double bins_p[] = {3.5, 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 15.}; */
  /* double bins_e[] = {-2.5, -2., -1.5, -1., -0.5, -0.25, 0., 0.25, 0.5, 1., 1.5, 2., 2.5}; */

  // Binning to match Z analysis
  /* double bins_p[] = {3.5, 4., 5., 6., 7., 8., 9., 10., 12., 14., 16., 18., 20.}; */
  /* double bins_e[] = {-2.5, -2.0, -1.5, -1.2, -0.6, -0.15, 0.0, 0.15, 0.6, 1.2, 1.5, 2.0, 2.5}; */

  // Binning for Z+Jpsi matching studies
  float bins_p[] = {3.5, 4., 5., 6., 7., 8., 10., 12., 15., 20.};
  /* float bins_e[] = {0.1, 2.5}; */
  float bins_e[] = {-2.5, -2.0, -1.3, -1.05, -0.1, 0.0, 0.1, 1.05, 1.3, 2.0, 2.5};

  int nbins_p = sizeof (bins_p) / sizeof (float) - 1;
  int nbins_e = sizeof (bins_e) / sizeof (float) - 1;

  // Sample1 histograms

  TH1F *h1Ptnum = new TH1F("h1Ptnum","h1Ptnum",nbins_p,bins_p);
  TH1F *h1Ptden = new TH1F("h1Ptden","h1Ptden",nbins_p,bins_p);
  TH1F *h1Pt = new TH1F("h1Pt","h1Pt",nbins_p,bins_p);
  TH1F *h1Pt_id = new TH1F("h1Pt_id","h1Pt_id",nbins_p,bins_p);
  h1Ptnum->Sumw2();
  h1Ptden->Sumw2();
  h1Pt->Sumw2();
  h1Pt_id->Sumw2();

  TH1F *h1Etanum = new TH1F("h1Etanum","h1Etanum",nbins_e,bins_e);
  TH1F *h1Etaden = new TH1F("h1Etaden","h1Etaden",nbins_e,bins_e);
  TH1F *h1Eta = new TH1F("h1Eta","h1Eta",nbins_e,bins_e);
  TH1F *h1Eta_id = new TH1F("h1Eta_id","h1Eta_id",nbins_e,bins_e);
  h1Etanum->Sumw2();
  h1Etaden->Sumw2();
  h1Eta->Sumw2();
  h1Eta_id->Sumw2();

  TH2F *h1EtaPtnum = new TH2F("h1EtaPtnum","h1EtaPtnum",nbins_e,bins_e,nbins_p,bins_p);
  TH2F *h1EtaPtden = new TH2F("h1EtaPtden","h1EtaPtden",nbins_e,bins_e,nbins_p,bins_p);
  TH2F *h1EtaPt = new TH2F("h1EtaPt","h1EtaPt;#eta;p_{T}",nbins_e,bins_e,nbins_p,bins_p);
  TH2F *h1EtaPt_id = new TH2F("h1EtaPt_id","h1EtaPt_id;#eta;p_{T}",nbins_e,bins_e,nbins_p,bins_p);
  h1EtaPtnum->Sumw2();
  h1EtaPtden->Sumw2();
  h1EtaPt->Sumw2();
  h1EtaPt_id->Sumw2();

  TH1F *h1ErrSystPt; TH1F *h1ErrSystEta; TH2F *h1EtaPtUp; TH2F *h1EtaPtDown;

  // Sample2 histograms

  TH1F *h2Ptnum = new TH1F("h2Ptnum","h2Ptnum",nbins_p,bins_p);
  TH1F *h2Ptden = new TH1F("h2Ptden","h2Ptden",nbins_p,bins_p);
  TH1F *h2Pt = new TH1F("h2Pt","h2Pt",nbins_p,bins_p);
  TH1F *h2Pt_id = new TH1F("h2Pt_id","h2Pt_id",nbins_p,bins_p);
  h2Ptnum->Sumw2();
  h2Ptden->Sumw2();
  h2Pt->Sumw2();
  h2Pt_id->Sumw2();

  TH1F *h2Etanum = new TH1F("h2Etanum","h2Etanum",nbins_e,bins_e);
  TH1F *h2Etaden = new TH1F("h2Etaden","h2Etaden",nbins_e,bins_e);
  TH1F *h2Eta = new TH1F("h2Eta","h2Eta",nbins_e,bins_e);
  TH1F *h2Eta_id = new TH1F("h2Eta_id","h2Eta_id",nbins_e,bins_e);
  h2Etanum->Sumw2();
  h2Etaden->Sumw2();
  h2Eta->Sumw2();
  h2Eta_id->Sumw2();

  TH2F *h2EtaPtnum = new TH2F("h2EtaPtnum","h2EtaPtnum",nbins_e,bins_e,nbins_p,bins_p);
  TH2F *h2EtaPtden = new TH2F("h2EtaPtden","h2EtaPtden",nbins_e,bins_e,nbins_p,bins_p);
  TH2F *h2EtaPt = new TH2F("h2EtaPt","h2EtaPt",nbins_e,bins_e,nbins_p,bins_p);
  TH2F *h2EtaPt_id = new TH2F("h2EtaPt_id","h2EtaPt_id",nbins_e,bins_e,nbins_p,bins_p);
  h2EtaPtnum->Sumw2();
  h2EtaPtden->Sumw2();
  h2EtaPt->Sumw2();
  h2EtaPt_id->Sumw2();

  TH1F *h2ErrSystPt; TH1F *h2ErrSystEta; TH2F *h2EtaPtUp; TH2F *h2EtaPtDown;

  // TCuts:

  //  TCut commonCut1 = "probe_pt>3.5&&probe_pt<20&&abs(probe_eta)>0.0&&abs(probe_eta)<2.5&&abs(tag_z0-probe_z0)<5";
  TCut commonCut1 = "abs(tag_z0-probe_z0)<5";
  TCut commonCut2 = "sqrt(dilep_dphi_exTP*dilep_dphi_exTP+dilep_deta_exTP*dilep_deta_exTP)>0.2";
  TCut commonCut = commonCut1&&commonCut2;

  TCut numCut = commonCut&&"probe_matched_MediumMuons&&probe_dRMatch_MediumMuons<0.005";
  TCut denCut = commonCut;

  if ( probeType1 == "TruthProbes_Muon" ) {

    treeTruth1->Draw("probe_pt>>h1Ptnum",numCut);
    treeTruth1->Draw("probe_pt>>h1Ptden",denCut);
    h1Pt->Divide(h1Ptnum,h1Ptden,1,1,"B");

    treeTruth1->Draw("probe_eta>>h1Etanum",numCut);
    treeTruth1->Draw("probe_eta>>h1Etaden",denCut);
    h1Eta->Divide(h1Etanum,h1Etaden,1,1,"B");

    treeTruth1->Draw("probe_pt:probe_eta>>h1EtaPtnum",numCut);
    treeTruth1->Draw("probe_pt:probe_eta>>h1EtaPtden",denCut);
    h1EtaPt->Divide(h1EtaPtnum,h1EtaPtden,1,1,"B");

  } else {


    f1->GetObject("hEta",h1Eta);
    f1->GetObject("hErrSystEta",h1ErrSystEta);
    f1->GetObject("hPt",h1Pt);
    f1->GetObject("hErrSystPt",h1ErrSystPt);
    f1->GetObject("recoeff_centralvalue_gp2",h1EtaPt);
    f1->GetObject("recoeff_uptotalerror_etapt_gp2",h1EtaPtUp);
    f1->GetObject("recoeff_downtotalerror_etapt_gp2",h1EtaPtDown);

 // ID efficiency corrections
    if ( effCorrID ) {
      f1_id->GetObject("hPt",h1Pt_id);
      f1_id->GetObject("hEta",h1Eta_id);
      f1_id->GetObject("recoeff_centralvalue_gp2",h1EtaPt_id);

      h1Pt->Multiply(h1Pt,h1Pt_id);
      h1Eta->Multiply(h1Eta,h1Eta_id);
      h1EtaPt->Multiply(h1EtaPt,h1EtaPt_id);
    }

  }

  if ( probeType2 == "TruthProbes_Muon" ) {

    treeTruth2->Draw("probe_pt>>h2Ptnum",numCut);
    treeTruth2->Draw("probe_pt>>h2Ptden",denCut);
    h2Pt->Divide(h2Ptnum,h2Ptden,1,1,"B");

    treeTruth2->Draw("abs(probe_eta)>>h2Etanum",numCut);
    treeTruth2->Draw("abs(probe_eta)>>h2Etaden",denCut);
    h2Eta->Divide(h2Etanum,h2Etaden,1,1,"B");

    treeTruth2->Draw("probe_pt:probe_eta>>h2EtaPtnum",numCut);
    treeTruth2->Draw("probe_pt:probe_eta>>h2EtaPtden",denCut);
    /* treeTruth2->Draw("probe_pt:abs(probe_eta)>>h2EtaPtnum",numCut); */
    /* treeTruth2->Draw("probe_pt:abs(probe_eta)>>h2EtaPtden",denCut); */
    h2EtaPt->Divide(h2EtaPtnum,h2EtaPtden,1,1,"B");

  } else {

    f2->GetObject("hPt",h2Pt);
    f2->GetObject("hErrSystPt",h2ErrSystPt);
    f2->GetObject("hEta",h2Eta);
    f2->GetObject("hErrSystEta",h2ErrSystEta);
    f2->GetObject("recoeff_centralvalue_gp2",h2EtaPt);
    f2->GetObject("recoeff_uptotalerror_etapt_gp2",h2EtaPtUp);
    f2->GetObject("recoeff_downtotalerror_etapt_gp2",h2EtaPtDown);

 // ID efficiency corrections
    if ( effCorrID ) {
      f2_id->GetObject("hPt",h2Pt_id);
      f2_id->GetObject("hEta",h2Eta_id);
      f2_id->GetObject("recoeff_centralvalue_gp2",h2EtaPt_id);

      h2Pt->Multiply(h2Pt,h2Pt_id);
      h2Eta->Multiply(h2Eta,h2Eta_id);
      h2EtaPt->Multiply(h2EtaPt,h2EtaPt_id);
    }

  }

  /**************
    [1] pT plot
  ***************/

  float ptmin = 4.;
  float ptmax = 20.;

  TCanvas *can_pt = new TCanvas("can_pt","pT",0,1100,600,700);

  TPad *pad0_pt = new TPad("pad0_pt","pad0_pt",0,0.3,1,1);
  pad0_pt->SetBottomMargin(0);
  pad0_pt->Draw();
  pad0_pt->cd();

  TH1F *frame0_pt = pad0_pt->DrawFrame(ptmin,0.8,ptmax,1.1);

  frame0_pt->GetXaxis()->SetNdivisions(505); frame0_pt->GetYaxis()->SetNdivisions(505);

  frame0_pt->GetYaxis()->SetTitle("Reco Efficiency");
  frame0_pt->GetYaxis()->SetLabelSize(0.05);
  frame0_pt->GetYaxis()->SetTitleSize(0.05);
  frame0_pt->GetYaxis()->SetTitleOffset(1.0);

  int etaSlice = -1; // -1: integral; 0 = average; >0 = slices

  if (etaSlice > 0) {
    h1Pt->Reset();
    h2Pt->Reset();
    for (int binp=1; binp<=nbins_p; binp++) {
      h1Pt->SetBinContent(binp,h1EtaPt->GetBinContent(etaSlice,binp));
      h1Pt->SetBinError(binp,h1EtaPt->GetBinError(etaSlice,binp));
      h2Pt->SetBinContent(binp,h2EtaPt->GetBinContent(etaSlice,binp));
      h2Pt->SetBinError(binp,h2EtaPt->GetBinError(etaSlice,binp));
    }
  }

  if (etaSlice == 0) {
    for (int binx=1; binx<=nbins_p; binx++) {
      double avay = 0.;
      double eavay = 0.;
      double sumw = 0.;
      for (int biny=1; biny<=nbins_e; biny++) {
	double y = h1EtaPt->GetBinContent(biny,binx);
	double ey = h1EtaPt->GetBinError(biny,binx);
	double w = 1./ey/ey;
	avay += w*y;
	sumw += w;
      }
      avay /= sumw;
      eavay = 1./sqrt(sumw);
      h1Pt->SetBinContent(binx,avay);
      h1Pt->SetBinError(binx,eavay);

      avay = 0.;
      eavay = 0.;
      sumw = 0.;
      for (int biny=1; biny<=nbins_e; biny++) {
	double y = h2EtaPt->GetBinContent(biny,binx);
	double ey = h2EtaPt->GetBinError(biny,binx);
	double w = 1./ey/ey;
	avay += w*y;
	sumw += w;
      }
      avay /= sumw;
      eavay = 1./sqrt(sumw);
      h2Pt->SetBinContent(binx,avay);
      h2Pt->SetBinError(binx,eavay);
    }
  }

  // Making a weighted average with asymmetric errors (a la Barlow) */
  /* for (int binx=1; binx<=nbins_p; binx++) { */
  /* 	double avay = 0.; */
  /* 	double eavay = 0.; */
  /* 	double sumw = 0.; */
  /* 	for (int biny=1; biny<=nbins_e; biny++) { */
  /* 	  double y = h2EtaPt->GetBinContent(biny,binx); */
  /* 	  double ey = h2EtaPt->GetBinError(biny,binx); */
  /* 	  double sigman = ey; */
  /* 	  double sigmap = ey; */
  /* 	  if (y+ey>1.) sigmap = 1.-y; // assume parabolic LLH */
  /* 	  double sigma = (sigmap+sigman)/2.; */
  /* 	  double alpha = (sigmap-sigman)/2.; */
  /* 	  double b = alpha; */
  /* 	  double v = sigma*sigma+2.*alpha*alpha; */
  /* 	  double w = 1./v; */
  /* 	  avay += w*(y-b); */
  /* 	  sumw += w; */
  /* 	} */
  /* 	avay /= sumw; */
  /* 	eavay = 1./sqrt(sumw); */
  /* 	h2Pt->SetBinContent(binx,avay); */
  /* 	h2Pt->SetBinError(binx,eavay); */
  /* } */

  h1Pt->SetAxisRange(ptmin,ptmax,"X");
  h2Pt->SetAxisRange(ptmin,ptmax,"X");

  h1Pt->SetMarkerStyle(20);
  h1Pt->SetMarkerColor(1);
  h1Pt->SetLineColor(1);
  h2Pt->SetMarkerStyle(24);
  h2Pt->SetMarkerColor(2);
  h2Pt->SetLineColor(2);

  h1Pt->Draw("E same");
  h2Pt->Draw("same");

  /* printH(h1Pt); */

  /* for (int i=1; i<=h1Pt->GetNbinsX(); i++) { */
  /*   if ( i <= 2 ) { */
  /*     printf("%g %g %g %g %g \n", */
  /* 	     h1Pt->GetBinLowEdge(i)+h1Pt->GetBinWidth(i)/2,  */
  /* 	     h1Pt->GetBinWidth(i), */
  /* 	     h1Pt->GetBinContent(i), */
  /* 	     h1Pt->GetBinError(i), */
  /* 	     sqrt(h1ErrSystPt->GetBinError(i)*h1ErrSystPt->GetBinError(i)-h1Pt->GetBinError(i)*h1Pt->GetBinError(i))); */
  /*   } else { */
  /*     printf("%g %g %g %g %g \n", */
  /* 	     h2Pt->GetBinLowEdge(i)+h2Pt->GetBinWidth(i)/2,  */
  /* 	     h2Pt->GetBinWidth(i), */
  /* 	     h2Pt->GetBinContent(i), */
  /* 	     h2Pt->GetBinError(i), */
  /* 	     sqrt(h2ErrSystPt->GetBinError(i)*h2ErrSystPt->GetBinError(i)-h2Pt->GetBinError(i)*h2Pt->GetBinError(i))); */
  /*   } */
  /* } */

  TLine * lin_pt = new TLine(ptmin,1,ptmax,1);
  lin_pt->SetLineWidth(2);
  lin_pt->SetLineStyle(2);
  lin_pt->Draw();

  /* TLatex lat_pt; */
  /* lat_pt.SetNDC(); */
  /* lat_pt.SetTextColor(1); // l.SetTextAlign(12); */
  /* lat_pt.SetTextSize(0.04); */
  /* lat_pt.SetTextFont(72); lat_pt.DrawLatex(0.12,0.17,"ATLAS"); lat_pt.SetTextFont(42); lat_pt.DrawLatex(0.24,0.17,"Internal"); */
  /* lat_pt.DrawLatex(0.12,0.10,"#sqrt{s} = 13 TeV, #int L dt = 72 pb^{-1}"); */
  /* lat_pt.DrawLatex(0.12,0.03,"Medium Muons, |#eta| < 2.5 (ID probes)"); */

  TLegend *leg_pt = new TLegend(0.50,0.20,0.80,0.30);
  if ( dataSample1 == "dataC_50ns" ) leg_pt->AddEntry(h1Pt, "Period C 50 ns", "pl");
  if ( dataSample1 == "dataD_25ns" ) leg_pt->AddEntry(h1Pt, "Period D 25 ns", "pl");
  if ( dataSample1 == "mc_50ns" ) leg_pt->AddEntry(h1Pt, "J/#psi#rightarrow#mu#mu MC 50 ns", "pl");
  if ( dataSample1 == "mc_25ns" ) leg_pt->AddEntry(h1Pt, "J/#psi#rightarrow#mu#mu MC 25 ns", "pl");
  if ( dataSample2 == "dataC_50ns" ) leg_pt->AddEntry(h2Pt, "Period C 50 ns", "pl");
  if ( dataSample2 == "dataD_25ns" ) leg_pt->AddEntry(h2Pt, "Period D 25 ns", "pl");
  if ( dataSample2 == "mc_50ns" ) leg_pt->AddEntry(h2Pt, "J/#psi#rightarrow#mu#mu MC 50 ns", "pl");
  if ( dataSample2 == "mc_25ns" ) leg_pt->AddEntry(h2Pt, "J/#psi#rightarrow#mu#mu MC 25 ns", "pl");
  leg_pt->SetTextSize(0.04);
  leg_pt->SetFillColor(10);leg_pt->SetLineColor(10);
  leg_pt->Draw("same");

  can_pt->cd();

  TPad *pad_pt = new TPad("pad_pt","pad_pt",0,0,1,0.3);
  pad_pt->SetTopMargin(0);
  pad_pt->Draw();
  pad_pt->cd();

  TH1F *frame_pt  = pad_pt->DrawFrame(ptmin,0.95,ptmax,1.05);

  frame_pt->GetXaxis()->SetNdivisions(505); frame_pt->GetYaxis()->SetNdivisions(505);

  frame_pt->GetXaxis()->SetLabelSize(0.11);
  frame_pt->GetYaxis()->SetTitle("Data/MC");
  frame_pt->GetYaxis()->SetLabelSize(0.11);
  frame_pt->GetYaxis()->SetTitleSize(0.1);
  frame_pt->GetYaxis()->SetTitleOffset(0.6);

  frame_pt->GetXaxis()->SetTitle("p_{T}");
  frame_pt->GetXaxis()->SetTitleSize(0.1);
  frame_pt->GetXaxis()->SetTitleOffset(0.65);

  TH1F *hsf_pt = (TH1F*)h1Pt->Clone("hsf_pt");
  TH1F *hSys_pt = (TH1F*)h1Pt->Clone("hSys_pt");

  hsf_pt->Divide(h1Pt,h2Pt);

  /* if ( dataSample2 != "data" && probeType2 != "TruthProbes_Muon" ) { */
  /*   hSys_pt->Divide(h1Pt,h2ErrSystPt); */
  /*   hSys_pt->SetFillColor(kGreen-8); */
  /*   hSys_pt->Draw("E2 same"); */
  /* } */

  hsf_pt->Draw("ep same");

  for (int i=1; i<=hsf_pt->GetNbinsX(); i++) {
    printf("%g %g %g \n",
	   hsf_pt->GetBinLowEdge(i)+hsf_pt->GetBinWidth(i)/2,
	   hsf_pt->GetBinWidth(i),
	   fabs(1.-hsf_pt->GetBinContent(i)));
  }
  
  lin_pt->Draw();

  can_pt->cd();

  /****************
    [2] Eta plot
  ****************/

  float etamin = -2.5;
  float etamax =  2.5;

  TCanvas *can_eta = new TCanvas("can_eta","eta",500,1100,600,700);

  TPad *pad0_eta = new TPad("pad0_eta","pad0_eta",0,0.3,1,1);
  pad0_eta->SetBottomMargin(0);
  pad0_eta->Draw();
  pad0_eta->cd();

  TH1F *frame0_eta = pad0_eta->DrawFrame(etamin,0.3,etamax,1.1);

  frame0_eta->GetXaxis()->SetNdivisions(505); frame0_eta->GetYaxis()->SetNdivisions(505);

  frame0_eta->GetYaxis()->SetTitle("Reco Efficiency");
  frame0_eta->GetYaxis()->SetLabelSize(0.05);
  frame0_eta->GetYaxis()->SetTitleSize(0.05);
  frame0_eta->GetYaxis()->SetTitleOffset(1.0);

  int ptSlice = -1; // -1: integral; 0 = average; >0 = slices

  if (ptSlice > 0) {
    h1Eta->Reset();
    h2Eta->Reset();
    for (int bine=1; bine<=nbins_e; bine++) {
      h1Eta->SetBinContent(bine,h1EtaPt->GetBinContent(ptSlice,bine));
      h1Eta->SetBinError(bine,h1EtaPt->GetBinError(ptSlice,bine));
      h2Eta->SetBinContent(bine,h2EtaPt->GetBinContent(ptSlice,bine));
      h2Eta->SetBinError(bine,h2EtaPt->GetBinError(ptSlice,bine));
    }
  }

  if (ptSlice == 0) {
    for (int binx=1; binx<=nbins_e; binx++) {
      double avay = 0.;
      double eavay = 0.;
      double sumw = 0.;
      for (int biny=2; biny<=nbins_p; biny++) { // note that starts from 2
	double y = h1EtaPt->GetBinContent(binx,biny);
	double ey = h1EtaPt->GetBinError(binx,biny);
	double w = 1./ey/ey;
	avay += w*y;
	sumw += w;
      }
      avay /= sumw;
      eavay = 1./sqrt(sumw);
      h1Eta->SetBinContent(binx,avay);
      h1Eta->SetBinError(binx,eavay);

      avay = 0.;
      eavay = 0.;
      sumw = 0.;
      for (int biny=2; biny<=nbins_p; biny++) { // note that starts from 2
	double y = h2EtaPt->GetBinContent(binx,biny);
	double ey = h2EtaPt->GetBinError(binx,biny);
	double w = 1./ey/ey;
	avay += w*y;
	sumw += w;
      }
      avay /= sumw;
      eavay = 1./sqrt(sumw);
      h2Eta->SetBinContent(binx,avay);
      h2Eta->SetBinError(binx,eavay);
    }
  }

  h1Eta->SetAxisRange(etamin,etamax,"X");
  h2Eta->SetAxisRange(etamin,etamax,"X");

  h1Eta->SetMarkerStyle(20);
  h1Eta->SetMarkerColor(1);
  h1Eta->SetLineColor(1);
  h2Eta->SetMarkerStyle(24);
  h2Eta->SetMarkerColor(2);
  h2Eta->SetLineColor(2);

  h1Eta->Draw("E same");
  h2Eta->Draw("same");

  TLine * lin_eta = new TLine(etamin,1,etamax,1);
  lin_eta->SetLineWidth(2);
  lin_eta->SetLineStyle(2);
  lin_eta->Draw();

  /* TLatex lat_eta; */
  /* lat_eta.SetNDC(); */
  /* lat_eta.SetTextColor(1); // lat_etaSetTextAlign(12); */
  /* lat_eta.SetTextSize(0.04); */
  /* lat_eta.SetTextFont(72); lat_eta.DrawLatex(0.12,0.17,"ATLAS"); lat_eta.SetTextFont(42); lat_eta.DrawLatex(0.24,0.17,"Internal"); */
  /* lat_eta.DrawLatex(0.12,0.10,"#sqrt{s} = 13 TeV, #int L dt = 6.8 pb^{-1}"); */
  /* lat_eta.DrawLatex(0.12,0.03,"Medium Muons, |#eta| < 2.5 (ID probes)"); */

  TLegend *leg_eta = new TLegend(0.40,0.20,0.65,0.30);
  if ( dataSample1 == "dataC_50ns" ) leg_eta->AddEntry(h1Eta, "Period C 50 ns", "pl");
  if ( dataSample1 == "dataD_25ns" ) leg_eta->AddEntry(h1Eta, "Period D 25 ns", "pl");
  if ( dataSample1 == "mc_50ns" ) leg_eta->AddEntry(h1Eta, "J/#psi#rightarrow#mu#mu MC 50 ns", "pl");
  if ( dataSample1 == "mc_25ns" ) leg_eta->AddEntry(h1Eta, "J/#psi#rightarrow#mu#mu MC 25 ns", "pl");
  if ( dataSample2 == "dataC_50ns" ) leg_eta->AddEntry(h2Eta, "Period C 50 ns", "pl");
  if ( dataSample2 == "dataD_25ns" ) leg_eta->AddEntry(h2Eta, "Period D 25 ns", "pl");
  if ( dataSample2 == "mc_50ns" ) leg_eta->AddEntry(h2Eta, "J/#psi#rightarrow#mu#mu MC 50 ns", "pl");
  if ( dataSample2 == "mc_25ns" ) leg_eta->AddEntry(h2Eta, "J/#psi#rightarrow#mu#mu MC 25 ns", "pl");
  leg_eta->SetTextSize(0.04);
  leg_eta->SetFillColor(10);leg_eta->SetLineColor(10);
  leg_eta->Draw("same");

  can_eta->cd();

  TPad *pad_eta = new TPad("pad_eta","pad_eta",0,0,1,0.3);
  pad_eta->SetTopMargin(0);
  pad_eta->Draw();
  pad_eta->cd();

  TH1F *frame_eta  = pad_eta->DrawFrame(etamin,0.95,etamax,1.05);

  frame_eta->GetXaxis()->SetNdivisions(505); frame_eta->GetYaxis()->SetNdivisions(505);

  frame_eta->GetXaxis()->SetLabelSize(0.11);
  frame_eta->GetYaxis()->SetTitle("Data/MC");
  frame_eta->GetYaxis()->SetLabelSize(0.11);
  frame_eta->GetYaxis()->SetTitleSize(0.1);
  frame_eta->GetYaxis()->SetTitleOffset(0.6);

  frame_eta->GetXaxis()->SetTitle("#eta");
  frame_eta->GetXaxis()->SetTitleSize(0.1);
  frame_eta->GetXaxis()->SetTitleOffset(0.65);

  TH1F *hsf_eta = (TH1F*)h1Eta->Clone("hsf_eta");
  TH1F *hSys_eta = (TH1F*)h1Eta->Clone("hSys_eta");

  hsf_eta->Divide(h1Eta,h2Eta);

  /* if ( dataSample2 != "data" && probeType2 != "TruthProbes_Muon" ) { */
  /*   hSys_eta->Divide(h1Eta,h2ErrSystEta); */
  /*   hSys_eta->SetFillColor(kGreen-8); */
  /*   hSys_eta->Draw("E2 same"); */
  /* } */

  hsf_eta->Draw("ep same");

  lin_eta->Draw();

  can_eta->cd();

  /***********************
    [3] Eta-pT plot: eff
  ***********************/

  TCanvas *can_etapt_eff = new TCanvas("can_etapt_eff","can_etapt_eff",1000,1100,600,700);

  gStyle->SetPalette(54);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  h1EtaPt->GetZaxis()->SetRangeUser(0.0,1.1);
  h1EtaPt->Draw("COLZ");

  gStyle->SetPaintTextFormat("4.2f");
  h1EtaPt->SetMarkerSize(0.5);
  h1EtaPt->Draw("TEXTE SAME");


  /**********************
    [4] Eta-pT plot: SF
  **********************/

  TCanvas *can_etapt = new TCanvas("can_etapt","can_etapt",1000,1100,600,700);

  TH2F *hsf_etapt = (TH2F*)h1EtaPt->Clone("hsf_etapt");
  TH2F *hSys_etapt = (TH2F*)h1EtaPt->Clone("hSys_etapt");

  for (int bine=1; bine<=nbins_e; bine++) {
    h1EtaPt->SetBinContent(bine,1,0);
    h2EtaPt->SetBinContent(bine,1,0);
  }
  hsf_etapt->Divide(h1EtaPt,h2EtaPt);

  gStyle->SetPalette(54);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  //  hsf_etapt->GetZaxis()->SetRangeUser(0.98,1.02); // for closure
  hsf_etapt->GetZaxis()->SetRangeUser(0.85,1.15);
  hsf_etapt->Draw("COLZ");

  gStyle->SetPaintTextFormat("5.3f");
  hsf_etapt->SetMarkerSize(0.5);
  hsf_etapt->Draw("TEXTE SAME");

  TFile *outputFile = new TFile("finalHistos_noCrack.root", "RECREATE");

  TH2F *hEtaPt_CentralValue = (TH2F*)h1EtaPt->Clone("hEtaPt_CentralValue");
  for (int bine=1; bine<=nbins_e; bine++) {
    for (int binp=1; binp<=nbins_p; binp++) {
      hEtaPt_CentralValue->SetBinContent(bine,binp,h1EtaPt->GetBinContent(bine,binp)*h1EtaPt_id->GetBinContent(bine,binp));
    }
  }

  hEtaPt_CentralValue->Write("hEtaPt_CentralValue");

  if ( probeType2 == "TruthProbes_Muon" ) {

    TH2F *hEtaPt_SysModelUp = (TH2F*)h1EtaPt->Clone("hEtaPt_SysModelUp");
    TH2F *hEtaPt_SysModelDown = (TH2F*)h1EtaPt->Clone("hEtaPt_SysModelDown");
    TH2F *hEtaPt_SysClosure = (TH2F*)h1EtaPt->Clone("hEtaPt_SysClosure");
    for (int bine=1; bine<=nbins_e; bine++) {
      for (int binp=1; binp<=nbins_p; binp++) {
	float estat = h1EtaPt->GetBinError(bine,binp);
	float etotup = h1EtaPtUp->GetBinContent(bine,binp);
	float etotdown = h1EtaPtDown->GetBinContent(bine,binp);
	float emodelup = sqrt(etotup*etotup-estat*estat);
	float emodeldown = sqrt(etotdown*etotdown-estat*estat);
	hEtaPt_SysModelUp->SetBinContent(bine,binp,emodelup);
	hEtaPt_SysModelDown->SetBinContent(bine,binp,emodeldown);
	hEtaPt_SysClosure->SetBinContent(bine,binp,1.-hsf_etapt->GetBinContent(bine,binp));
      }
    }

    hEtaPt_SysModelUp->Write("hEtaPt_SysModelUp");
    hEtaPt_SysModelDown->Write("hEtaPt_SysModelDown");
    hEtaPt_SysClosure->Write("hEtaPt_SysClosure");

  }

  outputFile->Close();

}

void printH(TH1F *h) {
  for (int i=1; i<=h->GetNbinsX(); i++) {
    printf("%g %g %g \n",
	   h->GetBinLowEdge(i)+h->GetBinWidth(i)/2,
	   h->GetBinContent(i),
	   h->GetBinError(i));
  }
}
