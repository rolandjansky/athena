/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

makeplotsSLHC(string mfile="InDetStandardPlots.root"){
  
  gStyle->SetCanvasColor(0);

  Int_t icol=0;
  gStyle->SetFrameBorderMode(icol);
  gStyle->SetCanvasBorderMode(icol);
  gStyle->SetPadBorderMode(icol);
//gStyle->SetGridColor(icol);
  gStyle->SetPadColor(icol);
  gStyle->SetCanvasColor(icol);
  gStyle->SetFrameFillColor(icol);
  gStyle->SetStatColor(icol);
  gStyle->SetHistFillColor(icol);
  //gStyle->SetFillColor(icol);
  
  // set the paper & margin sizes
  gStyle->SetPaperSize(20,26);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadRightMargin(0.20);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.1);

  gStyle->SetPalette(1);

  gStyle->SetPadLeftMargin(0.2);
  gStyle->SetTitleOffset(1.7,"y");

// use large fonts
//Int_t font=72;
  Int_t font=42;
  Double_t tsize=0.05;
  gStyle->SetTextFont(font);
  
  gStyle->SetTextSize(tsize);
  gStyle->SetLabelFont(font,"x");
  gStyle->SetTitleFont(font,"x");
  gStyle->SetLabelFont(font,"y");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetLabelFont(font,"z");
  gStyle->SetTitleFont(font,"z");
  
  gStyle->SetLabelSize(tsize,"x");
  gStyle->SetTitleSize(tsize,"x");
  gStyle->SetLabelSize(tsize,"y");
  gStyle->SetTitleSize(tsize,"y");
  gStyle->SetLabelSize(tsize,"z");
  gStyle->SetTitleSize(tsize,"z");

  //use bold lines and markers
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth(2.);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
  
  //get rid of X error bars and y error bar caps
  //gStyle->SetErrorX(0.001);
  
  //do not display any of the standard histogram decorations
  gStyle->SetOptTitle(0);
  //gStyle->SetOptStat(1111);
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(1111);
  gStyle->SetOptFit(1111);
  
  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  
  //  gROOT->SetStyle("Plain");
  
  //gStyle->SetPadTickX(1);
  //gStyle->SetPadTickY(1);
  
  gROOT->ForceStyle();
  
  // extract the detector paper plots from pre-made files of
  // InDetPerformanceRTT
  
  TFile f1(mfile.c_str());
  //  TFile *f1 = new TFile("InDetStandardPlots.root");
  //  TFile *f1 = new TFile("/afs/cern.ch/user/b/beate/scratch0/reco/testarea/rel_5/InnerDetector/InDetExample/InDetRecExample/run/InDetStandardPlots.root");
  
  // resolutions
  TH1F* h1res[6];
  h1res[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/res_vs_eta_d0");
  h1res[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/res_vs_eta_phi");
  h1res[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/res_vs_eta_qopt");
  h1res[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/res_vs_eta_z0");
  h1res[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/res_vs_eta_theta");
  h1res[5] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/res_vs_eta_z0st");
  TH1F* h1res_ipat[6];
  h1res_ipat[0] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/res_vs_eta_d0");
  h1res_ipat[1] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/res_vs_eta_phi");
  h1res_ipat[2] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/res_vs_eta_qopt");
  h1res_ipat[3] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/res_vs_eta_z0");
  h1res_ipat[4] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/res_vs_eta_theta");
  h1res_ipat[5] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/res_vs_eta_z0st");

  // means 
  TH1F* h1mean[6];
  h1mean[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/mean_vs_eta_d0");
  h1mean[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/mean_vs_eta_phi");
  h1mean[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/mean_vs_eta_qopt");
  h1mean[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/mean_vs_eta_z0");
  h1mean[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/mean_vs_eta_theta");
  h1mean[5] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/mean_vs_eta_z0st");
  TH1F* h1mean_ipat[6];
  h1mean_ipat[0] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/mean_vs_eta_d0");
  h1mean_ipat[1] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/mean_vs_eta_phi");
  h1mean_ipat[2] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/mean_vs_eta_qopt");
  h1mean_ipat[3] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/mean_vs_eta_z0");
  h1mean_ipat[4] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/mean_vs_eta_theta");
  h1mean_ipat[5] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/mean_vs_eta_z0st");

  // pull resolutions
  TH1F* h1pullres[6];
  h1pullres[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullres_vs_eta_d0");
  h1pullres[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullres_vs_eta_phi");
  h1pullres[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullres_vs_eta_qopt");
  h1pullres[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullres_vs_eta_z0");
  h1pullres[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullres_vs_eta_theta");
  h1pullres[5] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullres_vs_eta_z0st");
  TH1F* h1pullres_ipat[6];
  h1pullres_ipat[0] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullres_vs_eta_d0");
  h1pullres_ipat[1] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullres_vs_eta_phi");
  h1pullres_ipat[2] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullres_vs_eta_qopt");
  h1pullres_ipat[3] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullres_vs_eta_z0");
  h1pullres_ipat[4] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullres_vs_eta_theta");
  h1pullres_ipat[5] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullres_vs_eta_z0st");

  // pulls
  TH1F* h1pull[5];
  h1pull[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pulld0all");
  h1pull[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullphiall");
  h1pull[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullqoptall");
  h1pull[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullz0all");
  h1pull[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/pullthetaall");
  TH1F* h1pull_ipat[5];
  h1pull_ipat[0] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pulld0all");
  h1pull_ipat[1] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullphiall");
  h1pull_ipat[2] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullqoptall");
  h1pull_ipat[3] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullz0all");
  h1pull_ipat[4] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/pullthetaall");

//   for (int i=0;i<5;i++) {
//     h1pull[i]->Fit("gaus");
//   }


  TH1F* h1eff[3];
  h1eff[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackeff_vs_eta");
  h1eff[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackeff_vs_eta");
  h1eff[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackeff_vs_eta");
  TH1F* h1eff_ipat;
  h1eff_ipat = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/trackeff_vs_eta");

  // residuals
  TH1F* pixres[4];
  pixres[0]= (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/residualx_pixel_barrel");
  pixres[1]= (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/residualx_pixel_endcap");
  pixres[2]= (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/residualy_pixel_barrel");
  pixres[3]= (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/residualy_pixel_endcap");
  
  TH1F* sctres[2];
  sctres[0]= (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/residualx_sct_barrel");
  sctres[1]= (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/residualx_sct_endcap");
  
  TH1F* trtres[2];
  trtres[0]= (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/residualx_trt_barrel");
  trtres[1]= (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/residualx_trt_endcap");
  
  // hit efficiencies
  TH1F* hiteff[7]; 
  hiteff[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_blay_barrel");
  hiteff[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_pix_barrel");
  hiteff[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_pix_endcap");
  hiteff[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_sct_barrel");
  hiteff[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_sct_endcap");
  hiteff[5] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_trt_barrel");
  hiteff[6] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/eff_hit_vs_eta_trt_endcap");
  TH1F* hiteff_ipat[7]; 
  hiteff_ipat[0] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/eff_hit_vs_eta_blay_barrel");
  hiteff_ipat[1] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/eff_hit_vs_eta_pix_barrel");
  hiteff_ipat[2] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/eff_hit_vs_eta_pix_endcap");
  hiteff_ipat[3] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/eff_hit_vs_eta_sct_barrel");
  hiteff_ipat[4] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/eff_hit_vs_eta_sct_endcap");
  hiteff_ipat[5] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/eff_hit_vs_eta_trt_barrel");
  hiteff_ipat[6] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/eff_hit_vs_eta_trt_endcap");

  TH1F* hitoutl[7]; 
  hitoutl[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/frac_outlier_vs_eta_blay_barrel");
  hitoutl[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/frac_outlier_vs_eta_pix_barrel");
  hitoutl[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/frac_outlier_vs_eta_pix_endcap");
  hitoutl[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/frac_outlier_vs_eta_sct_barrel");
  hitoutl[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/frac_outlier_vs_eta_sct_endcap");
  hitoutl[5] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/frac_outlier_vs_eta_trt_barrel");
  hitoutl[6] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/frac_outlier_vs_eta_trt_endcap");
  TH1F* hitoutl_ipat[7]; 
  hitoutl_ipat[0] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/frac_outlier_vs_eta_blay_barrel");
  hitoutl_ipat[1] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/frac_outlier_vs_eta_pix_barrel");
  hitoutl_ipat[2] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/frac_outlier_vs_eta_pix_endcap");
  hitoutl_ipat[3] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/frac_outlier_vs_eta_sct_barrel");
  hitoutl_ipat[4] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/frac_outlier_vs_eta_sct_endcap");
  hitoutl_ipat[5] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/frac_outlier_vs_eta_trt_barrel");
  hitoutl_ipat[6] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/frac_outlier_vs_eta_trt_endcap");
  
  // track summary information
  TH1F* tracksumpix[9];
  tracksumpix[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NBlayerHits");
  tracksumpix[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NBlayerOutliers");
  tracksumpix[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NBlayerSharedHits");
  tracksumpix[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NPixelHits");
  tracksumpix[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NPixelHoles");
  tracksumpix[5] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NPixelContribLayers");
  tracksumpix[6] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NPixelSharedHits");
  tracksumpix[7] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NPixelGangedHits");
  tracksumpix[8] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NPixelGangedHitsFlaggedFakes");

  TH1F* tracksumsct[4];
  tracksumsct[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NSCTHits");
  tracksumsct[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NSCTHoles");
  tracksumsct[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NSCTDoubleHoles");
  tracksumsct[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NSCTSharedHits");

  TH1F* tracksumtrt[4];
  tracksumtrt[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NTRTHits");
  tracksumtrt[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NTRTOutliers");
  tracksumtrt[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NTRTHighThresholdHits");
  tracksumtrt[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/HitContent_NTRTHighThresholdOutliers");


  // fake rates
  TH1F* fakerate_eta[6];
  fakerate_eta[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/track_badmatchrate_vs_eta");
  fakerate_eta[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/track_fakerate_vs_eta");
  fakerate_eta[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/track_badmatchrate_vs_eta");
  fakerate_eta[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/track_fakerate_vs_eta");
  fakerate_eta[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/track_badmatchrate_vs_eta");
  fakerate_eta[5] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/track_fakerate_vs_eta");
  TH1F* fakerate_eta_ipat[2];
  fakerate_eta_ipat[0] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/track_badmatchrate_vs_eta");
  fakerate_eta_ipat[1] = (TH1F*) f1.Get("IDPerformanceMon/ConvertedIPatTracks/SelectedGoodIPatTracks/track_fakerate_vs_eta");
  
  TH1F* fakerate_pt[6];
  fakerate_pt[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/track_badmatchrate_vs_pt");
  fakerate_pt[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/track_fakerate_vs_pt");
  fakerate_pt[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/track_badmatchrate_vs_pt");
  fakerate_pt[3] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/track_fakerate_vs_pt");
  fakerate_pt[4] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/track_badmatchrate_vs_pt");
  fakerate_pt[5] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/track_fakerate_vs_pt");
  TH1F* fakerate_pt_ipat[2];
  fakerate_pt_ipat[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/track_badmatchrate_vs_pt");
  fakerate_pt_ipat[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/track_fakerate_vs_pt");
  
  TH2F* nHits_pixel_2d = (TH2F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/nHits_pixel_2d"); 
  TH2F* nHits_SCT_2d = (TH2F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/nHits_SCT_2d"); 
  TH2F* nHits_TRT_2d = (TH2F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/nHits_TRT_2d"); 
  TH2F* nHits_ALL_RZ = (TH2F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/nHits_ALL_RZ"); 
  
  TH1F* trackinjeteff[7][3]; 
  TH1F* trackinjetfake[7][3];
  TH1F* trackinjetbad[7][3];
  TH1F* trackinjetbcode[7][3];
  TH1F* trackinjetprob[3];

  // jet properties
  trackinjeteff[0][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjeteff_vs_eta");
  trackinjeteff[1][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjeteff_vs_phi");
  trackinjeteff[2][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjeteff_vs_pt");
  trackinjeteff[3][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjeteff_vs_dr");
  trackinjeteff[4][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjeteff_vs_dr_lt_j50");
  trackinjeteff[5][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjeteff_vs_dr_gr_j100");
  trackinjeteff[6][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjeteff_vs_jetet");
  trackinjetfake[0][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_fakerate_vs_eta");
  trackinjetfake[1][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_fakerate_vs_phi");
  trackinjetfake[2][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_fakerate_vs_pt");
  trackinjetfake[3][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_fakerate_vs_dr");
  trackinjetfake[4][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_fakerate_vs_dr_lt_j50");
  trackinjetfake[5][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_fakerate_vs_dr_gr_j100");
  trackinjetfake[6][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_fakerate_vs_jetet");
  trackinjetbad[0][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_badmatchrate_vs_eta");
  trackinjetbad[1][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_badmatchrate_vs_phi");
  trackinjetbad[2][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_badmatchrate_vs_pt");
  trackinjetbad[3][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_badmatchrate_vs_dr");
  trackinjetbad[4][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_badmatchrate_vs_dr_lt_j50");
  trackinjetbad[5][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_badmatchrate_vs_dr_gr_j100");
  trackinjetbad[6][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_badmatchrate_vs_jetet");
  trackinjetbcode[0][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_bcode0rate_vs_eta");
  trackinjetbcode[1][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_bcode0rate_vs_phi");
  trackinjetbcode[2][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_bcode0rate_vs_pt");
  trackinjetbcode[3][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_bcode0rate_vs_dr");
  trackinjetbcode[4][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_bcode0rate_vs_dr_lt_j50");
  trackinjetbcode[5][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_bcode0rate_vs_dr_gr_j100");
  trackinjetbcode[6][0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/trackinjet_bcode0rate_vs_jetet");
  trackinjetprob[0] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/AllTracks/prob_injet");

  trackinjeteff[0][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjeteff_vs_eta");
  trackinjeteff[1][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjeteff_vs_phi");
  trackinjeteff[2][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjeteff_vs_pt");
  trackinjeteff[3][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjeteff_vs_dr");
  trackinjeteff[4][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjeteff_vs_dr_lt_j50");
  trackinjeteff[5][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjeteff_vs_dr_gr_j100");
  trackinjeteff[6][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjeteff_vs_jetet");
  trackinjetfake[0][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_fakerate_vs_eta");
  trackinjetfake[1][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_fakerate_vs_phi");
  trackinjetfake[2][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_fakerate_vs_pt");
  trackinjetfake[3][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_fakerate_vs_dr");
  trackinjetfake[4][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_fakerate_vs_dr_lt_j50");
  trackinjetfake[5][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_fakerate_vs_dr_gr_j100");
  trackinjetfake[6][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_fakerate_vs_jetet");
  trackinjetbad[0][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_badmatchrate_vs_eta");
  trackinjetbad[1][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_badmatchrate_vs_phi");
  trackinjetbad[2][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_badmatchrate_vs_pt");
  trackinjetbad[3][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_badmatchrate_vs_dr");
  trackinjetbad[4][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_badmatchrate_vs_dr_lt_j50");
  trackinjetbad[5][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_badmatchrate_vs_dr_gr_j100");
  trackinjetbad[6][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_badmatchrate_vs_jetet");
  trackinjetbcode[0][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_bcode0rate_vs_eta");
  trackinjetbcode[1][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_bcode0rate_vs_phi");
  trackinjetbcode[2][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_bcode0rate_vs_pt");
  trackinjetbcode[3][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_bcode0rate_vs_dr");
  trackinjetbcode[4][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_bcode0rate_vs_dr_lt_j50");
  trackinjetbcode[5][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_bcode0rate_vs_dr_gr_j100");
  trackinjetbcode[6][1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/trackinjet_bcode0rate_vs_jetet");
  trackinjetprob[1] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedGoodTracks/prob_injet");

  trackinjeteff[0][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjeteff_vs_eta");
  trackinjeteff[1][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjeteff_vs_phi");
  trackinjeteff[2][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjeteff_vs_pt");
  trackinjeteff[3][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjeteff_vs_dr");
  trackinjeteff[4][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjeteff_vs_dr_lt_j50");
  trackinjeteff[5][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjeteff_vs_dr_gr_j100");
  trackinjeteff[6][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjeteff_vs_jetet");
  trackinjetfake[0][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_fakerate_vs_eta");
  trackinjetfake[1][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_fakerate_vs_phi");
  trackinjetfake[2][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_fakerate_vs_pt");
  trackinjetfake[3][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_fakerate_vs_dr");
  trackinjetfake[4][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_fakerate_vs_dr_lt_j50");
  trackinjetfake[5][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_fakerate_vs_dr_gr_j100");
  trackinjetfake[6][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_fakerate_vs_jetet");
  trackinjetbad[0][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_badmatchrate_vs_eta");
  trackinjetbad[1][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_badmatchrate_vs_phi");
  trackinjetbad[2][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_badmatchrate_vs_pt");
  trackinjetbad[3][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_badmatchrate_vs_dr");
  trackinjetbad[4][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_badmatchrate_vs_dr_lt_j50");
  trackinjetbad[5][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_badmatchrate_vs_dr_gr_j100");
  trackinjetbad[6][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_badmatchrate_vs_jetet");
  trackinjetbcode[0][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_bcode0rate_vs_eta");
  trackinjetbcode[1][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_bcode0rate_vs_phi");
  trackinjetbcode[2][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_bcode0rate_vs_pt");
  trackinjetbcode[3][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_bcode0rate_vs_dr");
  trackinjetbcode[4][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_bcode0rate_vs_dr_lt_j50");
  trackinjetbcode[5][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_bcode0rate_vs_dr_gr_j100");
  trackinjetbcode[6][2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/trackinjet_bcode0rate_vs_jetet");
  trackinjetprob[2] = (TH1F*) f1.Get("IDPerformanceMon/CombinedInDetTracks/SelectedBtagTracks/prob_injet");

  TCanvas *c = new TCanvas("name","Resolutions of track parameters versus eta",400,300);
  c->Divide(1,1);
  for (int i=0;i<6;i++) {
    h1res[i]->GetXaxis()->SetTitle("|#eta|");
    h1pullres[i]->GetXaxis()->SetTitle("|#eta|");
    h1mean[i]->GetXaxis()->SetTitle("|#eta|");
    if (i==0) {
      h1res[i]->GetYaxis()->SetTitle("#sigma(d_{0}) (mm)");
      h1pullres[i]->GetYaxis()->SetTitle("d_{0} pull resolution");
      h1mean[i]->GetYaxis()->SetTitle("d^{rec}_{0}-d_{0}^{gen} (mm)");
    } else if (i==1) {
      h1res[i]->GetYaxis()->SetTitle("#sigma(#phi_{0}) (rad)");
      h1pullres[i]->GetYaxis()->SetTitle("#phi_{0} pull resolution");
      h1mean[i]->GetYaxis()->SetTitle("#phi^{rec}_{0}-#phi_{0}^{gen} (rad)");
    } else if (i==2) {
      h1res[i]->GetYaxis()->SetTitle("#sigma(q/p_{T}) (1/GeV)");
      h1pullres[i]->GetYaxis()->SetTitle("q/p_{T} pull resolution"); 
      h1mean[i]->GetYaxis()->SetTitle("q/p_{T}^{rec}-q/p_{T}^{gen} (1/GeV)");
    } else if (i==3) {
      h1res[i]->GetYaxis()->SetTitle("#sigma(z_{0}) (mm)");
      h1pullres[i]->GetYaxis()->SetTitle("z_{0} pull resolution");
      h1mean[i]->GetYaxis()->SetTitle("z_{0}^{rec}-z_{0}^{gen} (mm)");
    } else if (i==4) {
      h1res[i]->GetYaxis()->SetTitle("#sigma(#theta) (rad)");
      h1pullres[i]->GetYaxis()->SetTitle("#theta pull resolution");
      h1mean[i]->GetYaxis()->SetTitle("#theta^{rec}-#theta^{gen} (rad)");
    } else if (i==5) {
      h1res[i]->GetYaxis()->SetTitle("#sigma(z_{0}sin(#theta)) (mm)");
      h1pullres[i]->GetYaxis()->SetTitle("z_{0}sin(#theta) pull resolution");
      h1mean[i]->GetYaxis()->SetTitle("z_{0}sin(#theta)^{rec}-z_{0}sin(#theta)^{gen} (mm)");
    } 
    c->Clear();
    c->cd(1);
    TLegend *legeff = new TLegend(0.81,0.6,0.99,0.9,NULL,"brNDC");
    if(h1res[i]){
      h1res[i]->Draw("e");
      h1res[i]->SetLineColor(2);
      h1res[i]->SetMarkerColor(2);
      legeff->AddEntry(h1res[i],"NewT","p");
    }
    if(h1res_ipat[i]){
      h1res_ipat[i]->Draw("esame");
      h1res_ipat[i]->SetLineColor(4);
      h1res_ipat[i]->SetMarkerColor(4);
      legeff->AddEntry(h1res_ipat[i],"IPatRec","p");
    }
    legeff->SetFillColor(0);
    legeff->SetBorderSize(0);
    legeff->Draw();
    char name[200];
    sprintf(name,"resplot%i.png",i);
    c->Print(name);
    c->cd(1);
    TLegend *legeff = new TLegend(0.81,0.6,0.99,0.9,NULL,"brNDC");
    if(h1pullres[i]){
      h1pullres[i]->Draw("e");
      h1pullres[i]->SetLineColor(2);
      h1pullres[i]->SetMarkerColor(2);
      legeff->AddEntry(h1pullres[i],"NewT","p");
    }
    if(h1pullres_ipat[i]){
      h1pullres_ipat[i]->Draw("esame");
      h1pullres_ipat[i]->SetLineColor(4);
      h1pullres_ipat[i]->SetMarkerColor(4);
      legeff->AddEntry(h1pullres_ipat[i],"IPatRec","p");
    }
    legeff->SetFillColor(0);
    legeff->SetBorderSize(0);
    legeff->Draw();
    char name[200];
    sprintf(name,"pullresplot%i.png",i);
    c->Print(name);
    c->cd(1);
    TLegend *legeff = new TLegend(0.81,0.6,0.99,0.9,NULL,"brNDC");
    if(h1mean[i]){
      h1mean[i]->Draw("e");
      h1mean[i]->SetLineColor(2);
      h1mean[i]->SetMarkerColor(2);
      legeff->AddEntry(h1mean[i],"NewT","p");
    }
    if(h1mean_ipat[i]){
      h1mean_ipat[i]->Draw("esame");
      h1mean_ipat[i]->SetLineColor(4);
      h1mean_ipat[i]->SetMarkerColor(4);
      legeff->AddEntry(h1mean_ipat[i],"IPatRec","p");
    }
    legeff->SetFillColor(0);
    legeff->SetBorderSize(0);
    legeff->Draw();
    char name[200];
    sprintf(name,"meanplot%i.png",i);
    c->Print(name);
  }

  TLatex l;
  l.SetNDC();
  l.SetTextSize(.035);
  l.SetTextAlign(12); 
  TLatex l2;
  l2.SetNDC();
  l2.SetTextSize(.045);
  l2.SetTextAlign(12);
  gStyle->SetOptFit(0000);
  for (int i=0;i<5;i++) {
    if(h1pull[i]){
      h1pull[i]->GetYaxis()->SetTitle("Number of Entries");
      if (i==0) {
	h1pull[i]->GetXaxis()->SetTitle("d_{0} pull");
      } else if (i==1) {
	h1pull[i]->GetXaxis()->SetTitle("#phi pull");
      } else if (i==2) {
	h1pull[i]->GetXaxis()->SetTitle("q/p_{T} pull");
      } else if (i==3) {
	h1pull[i]->GetXaxis()->SetTitle("z_{0} pull");
      } else if (i==4) {
	h1pull[i]->GetXaxis()->SetTitle("#theta pull");
      } else if (i==5) {
	h1pull[i]->GetXaxis()->SetTitle("z_{0}sin(#theta) pull");
      } 
    }
    c->Clear();
    c->cd(1);
    //gStyle->SetPadRightMargin(0.05);
    if(h1pull[i]){
      h1pull[i]->Draw("e");
      h1pull[i]->SetLineColor(2);
      h1pull[i]->SetMarkerColor(2);
      l2.SetTextColor(2);
      l2.DrawLatex(.59,.85,"NewT");
    //l.SetTextColor(2);
      l.DrawLatex(.59,.79,Form("Entries = %4.0f",h1pull[i]->GetEntries()));
      l.DrawLatex(.59,.74,Form("Mean = %4.2f #pm %4.2f",h1pull[i]->GetMean(),h1pull[i]->GetMeanError()));
      l.DrawLatex(.59,.69,Form("RMS = %4.2f #pm %4.2f",h1pull[i]->GetRMS(),h1pull[i]->GetRMSError()));
    }
    if(h1pull_ipat[i]){
      h1pull_ipat[i]->Draw("esame");
      h1pull_ipat[i]->SetLineColor(4);
      h1pull_ipat[i]->SetMarkerColor(4);
      l2.SetTextColor(4);
      l2.DrawLatex(.59,.63,"IPatRec");
      //l.SetTextColor(4);
      l.DrawLatex(.59,.58,Form("Entries = %4.0f",h1pull_ipat[i]->GetEntries()));
      l.DrawLatex(.59,.53,Form("Mean = %4.2f #pm %4.2f",h1pull_ipat[i]->GetMean(),h1pull_ipat[i]->GetMeanError()));
      l.DrawLatex(.59,.48,Form("RMS = %4.2f #pm %4.2f",h1pull_ipat[i]->GetRMS(),h1pull_ipat[i]->GetRMSError()));
    }
    c->Update();
    char name[200];
    sprintf(name,"pullplot%i.png",i);
    c->Print(name);
  }
   
  gStyle->SetPadRightMargin(0.2);
  for (int i=0;i<3;i++) {
    if(h1eff[i]){
      h1eff[i]->GetXaxis()->SetTitle("|#eta|");
      if (i==0) {
	h1eff[i]->GetYaxis()->SetTitle("Tracking Efficiency: All tracks");
      } else if (i==1) {
	h1eff[i]->GetYaxis()->SetTitle("Tracking Efficiency: Good tracks");
      } else if (i==2) {
	h1eff[i]->GetYaxis()->SetTitle("Tracking Efficiency: B-tagging tracks");
      } 
    }
    c->Clear();
    c->cd(1);
    TLegend *legeff = new TLegend(0.81,0.6,0.99,0.9,NULL,"brNDC");
    if(h1eff[i]){
      h1eff[i]->Draw("e");
      h1eff[i]->SetLineColor(2);
      h1eff[i]->SetMarkerColor(2);
      legeff->AddEntry(h1eff[i],"NewT","pl");
    }
    if(i==1){  
      if(h1eff_ipat){
	h1eff_ipat->Draw("esame");
	h1eff_ipat->SetLineColor(4);
	h1eff_ipat->SetMarkerColor(4);
	legeff->AddEntry(h1eff_ipat,"IPatRec","pl");
      }
    }
    legeff->SetFillColor(0);
    legeff->SetBorderSize(0);
    legeff->Draw();
    char name[200];
    sprintf(name,"effplot%i.png",i);
    c->Print(name);
  }

  //
  for (int i=0;i<4;i++) {
    if(pixres[i]){
      if (i<2) pixres[i]->GetXaxis()->SetTitle("pixel x-residual (mm)");
      else pixres[i]->GetXaxis()->SetTitle("pixel y-residual (mm)");
      c->Clear();
      c->cd(1);
      pixres[i]->Draw("e");
    }
    char name[200];
    sprintf(name,"pix_residual%i.png",i);
    c->Print(name);
  }

  for (int i=0;i<2;i++) {
    c->Clear();
    c->cd(1);
    if(sctres[i]){
      sctres[i]->GetXaxis()->SetTitle("SCT x-residual (mm)");
      sctres[i]->Draw("e");
    }
    char name[200];
    sprintf(name,"sct_residual%i.png",i);
    c->Print(name);
    c->Clear();
    c->cd(1);
    if(trtres[i]){
      trtres[i]->GetXaxis()->SetTitle("TRT residual (mm)");
      trtres[i]->Draw("e");
    }
    sprintf(name,"trt_residual%i.png",i);
    c->Print(name);
  }

  for (int i=0;i<7;i++) {
    if(hiteff[i] && hitoutl[i]){
      hiteff[i]->GetXaxis()->SetTitle("|#eta|");
      hitoutl[i]->GetXaxis()->SetTitle("|#eta|");
      if (i==0) {
	hiteff[i]->GetYaxis()->SetTitle("Hit Efficiency: B-layer");
	hitoutl[i]->GetYaxis()->SetTitle("Fraction of Outliers: B-layer");
      } else if (i==1) {
	hiteff[i]->GetYaxis()->SetTitle("Hit Efficiency");
	hitoutl[i]->GetYaxis()->SetTitle("Fraction of Outliers");
      } else if (i==2) {
	hiteff[i]->GetYaxis()->SetTitle("Hit Efficiency");
	hitoutl[i]->GetYaxis()->SetTitle("Fraction of Outliers");
      } else if (i==3) {
	hiteff[i]->GetYaxis()->SetTitle("Hit Efficiency");
	hitoutl[i]->GetYaxis()->SetTitle("Fraction of Outliers");
      } else if (i==4) {
	hiteff[i]->GetYaxis()->SetTitle("Hit Efficiency");
	hitoutl[i]->GetYaxis()->SetTitle("Fraction of Outliers");
      } 
    }
    c->Clear();
    c->cd(1);
    TLegend *legeff = new TLegend(0.81,0.6,0.99,0.9,NULL,"brNDC");
    if(hiteff[i]){
      hiteff[i]->Draw("e");
      hiteff[i]->SetLineColor(2);
      hiteff[i]->SetMarkerColor(2);
      legeff->AddEntry(hiteff[i],"NewT","p");
    }
    if(hiteff_ipat[i]){
      hiteff_ipat[i]->Draw("esame");
      hiteff_ipat[i]->SetLineColor(4);
      hiteff_ipat[i]->SetMarkerColor(4);
      legeff->AddEntry(hiteff_ipat[i],"IPatRec","p");
    }
    legeff->SetFillColor(0);
    legeff->SetBorderSize(0);
    legeff->Draw();
    char name[200];
    sprintf(name,"hiteff_%i.png",i);
    c->Print(name);
    c->Clear();
    c->cd(1);
    TLegend *legeff = new TLegend(0.81,0.6,0.99,0.9,NULL,"brNDC");
    if(hitoutl[i]){
      hitoutl[i]->Draw("e");
      hitoutl[i]->SetLineColor(2);
      hitoutl[i]->SetMarkerColor(2);
      legeff->AddEntry(hitoutl[i],"NewT","p");
    }
    if(hitoutl_ipat[i]){
      hitoutl_ipat[i]->Draw("esame");
      hitoutl_ipat[i]->SetLineColor(4);
      hitoutl_ipat[i]->SetMarkerColor(4);
      legeff->AddEntry(hitoutl_ipat[i],"IPatRec","p");
    }
    legeff->SetFillColor(0);
    legeff->SetBorderSize(0);
    legeff->Draw();
    sprintf(name,"outlfrac_%i.png",i);
    c->Print(name);
  }
  
  // track summary plots
  for (int i=0;i<9;i++) {
    c->Clear();
    c->Divide(1,1);
    c->cd(1);
    //    TPad *pad = gROOT->GetSelectedPad();
    //    if (i==1 || i==4 || i==8) pad->SetLogy(1);
    //    else pad->SetLogy(0);
    if(tracksumpix[i])tracksumpix[i]->Draw("e");
    char name[200];
    sprintf(name,"tracksumpix_%i.png",i);
    c->Print(name);
  }
  for (int i=0;i<4;i++) {
    c->Clear();
    c->cd(1);
    //    TPad *pad = gROOT->GetSelectedPad();
    //    if (i>0 && i<4) pad->SetLogy();
    //    else pad->SetLogy();
    if(tracksumsct[i])tracksumsct[i]->Draw("e");
    char name[200];
    sprintf(name,"tracksumsct_%i.png",i);
    c->Print(name);
  }
  for (int i=0;i<4;i++) {
    c->Clear();
    c->cd(1);
    //    TPad *pad = gROOT->GetSelectedPad();
    //    if (i>0) pad->SetLogy();
    //    else pad->SetLogy();
    tracksumtrt[i]->Draw("e");
    char name[200];
    sprintf(name,"tracksumtrt_%i.png",i);
    c->Print(name);
  }


  TLegend* leg=new TLegend(0.81,0.3,0.99,0.9,NULL,"brNDC");
  for (int i=0;i<2;i++) {
    if(fakerate_eta[i]){
      fakerate_eta[i]->SetLineColor(4);fakerate_eta[i]->SetMarkerStyle(24);fakerate_eta[i]->SetMarkerColor(4);
    }
    if(fakerate_eta[i+2]){
      fakerate_eta[i+2]->SetMarkerStyle(20);
    }
    if(fakerate_eta[i+4]){
      fakerate_eta[i+4]->SetLineColor(2);fakerate_eta[i+4]->SetMarkerStyle(26);fakerate_eta[i+4]->SetMarkerColor(2);
    }
    if(fakerate_pt[i]){
      fakerate_pt[i]->SetLineColor(4);fakerate_pt[i]->SetMarkerStyle(24);fakerate_pt[i]->SetMarkerColor(4);
    }
    if(fakerate_pt[i+2]){
      fakerate_pt[i+2]->SetMarkerStyle(20);
    }
    if(fakerate_pt[i+4]){
      fakerate_pt[i+4]->SetLineColor(2);fakerate_pt[i+4]->SetMarkerStyle(26);fakerate_pt[i+4]->SetMarkerColor(2);
    }
    if(fakerate_eta_ipat[i]){
      fakerate_eta_ipat[i]->SetLineColor(3);fakerate_eta_ipat[i]->SetMarkerStyle(3);fakerate_eta_ipat[i]->SetMarkerColor(3);
    }
    if(fakerate_pt_ipat[i]){
      fakerate_pt_ipat[i]->SetLineColor(3);fakerate_pt_ipat[i]->SetMarkerStyle(3);fakerate_pt_ipat[i]->SetMarkerColor(3);
    }
  }

  if(fakerate_eta[0])leg->AddEntry(fakerate_eta[0],"All");
  if(fakerate_eta[2])leg->AddEntry(fakerate_eta[2],"Good");
  if(fakerate_eta[4])leg->AddEntry(fakerate_eta[4],"Btag");
  if(fakerate_eta_ipat[0])leg->AddEntry(fakerate_eta_ipat[0],"Good IPat");
  leg->SetFillColor(0);
  leg->SetBorderSize(0);

  // fake rates
  for (int i=0;i<2;i++) {
    c->Clear();
    c->cd(1);
    if(fakerate_eta[i])fakerate_eta[i]->Draw("e");
    if(fakerate_eta[i+2])fakerate_eta[i+2]->Draw("samee");
    if(fakerate_eta[i+4]) fakerate_eta[i+4]->Draw("samee");
    if(fakerate_eta_ipat[i])fakerate_eta_ipat[i]->Draw("esame");
    char name[200];
    if (i==0) {
      sprintf(name,"badmatchrate_eta.png");
    } else {
      sprintf(name,"fakerate_eta.png");
    }
    leg->Draw();
    c->Print(name);
    c->Clear();
    c->cd(1);
    if(fakerate_pt[i])fakerate_pt[i]->Draw("e");
    if(fakerate_pt[i+2])fakerate_pt[i+2]->Draw("samee");
    if(fakerate_pt[i+4])fakerate_pt[i+4]->Draw("samee");
    if(fakerate_pt_ipat[i])fakerate_pt_ipat[i]->Draw("esame");
    if (i==0) {
      sprintf(name,"badmatchrate_pt.png");
    } else {
      sprintf(name,"fakerate_pt.png");
    }
    leg->Draw();
    c->Print(name);
  }
 
  gStyle->SetOptTitle(1);
  gStyle->SetPadLeftMargin(0.08);
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetPadRightMargin(0.12);

  // Hit map plots
  c->Clear();
  c->Divide(1,1);
  c->cd(1);
  if(nHits_pixel_2d){
    nHits_pixel_2d->Draw("colz");
    nHits_pixel_2d->GetXaxis()->SetTitle("#eta");
    nHits_pixel_2d->GetYaxis()->SetTitle("#phi");
  }
  c->Print("nHits_pixel_2d.png");

  // Hit map plots
  c->Clear();
  c->Divide(1,1);
  c->cd(1);
  if(nHits_SCT_2d){
    nHits_SCT_2d->Draw("colz");
    nHits_SCT_2d->GetXaxis()->SetTitle("#eta");
    nHits_SCT_2d->GetYaxis()->SetTitle("#phi");
  }
  c->Print("nHits_SCT_2d.png");

  // Hit map plots
  c->Clear();
  c->Divide(1,1);
  c->cd(1);
  if(nHits_TRT_2d){
    nHits_TRT_2d->Draw("colz");
    nHits_TRT_2d->GetXaxis()->SetTitle("#eta");
    nHits_TRT_2d->GetYaxis()->SetTitle("#phi");
  }
  c->Print("nHits_TRT_2d.png");

  // Hit map plots
  c->Clear();
  c->Divide(1,1);
  c->cd(1);
  if(nHits_ALL_RZ){
    nHits_ALL_RZ->SetMarkerSize(0.5);
    nHits_ALL_RZ->SetMarkerStyle(1);
    nHits_ALL_RZ->Draw("scat");
    nHits_ALL_RZ->GetXaxis()->SetTitle("z");
    nHits_ALL_RZ->GetYaxis()->SetTitle("R");
  }
  c->Print("nHits_ALL_RZ.png");

  string maxis[7] = {"|#eta|", "#phi","p_{T}","dr","dr","dr","jet E_{T}"};
  string myaxismod[3] ={"All tracks", "Good tracks","B-tagging tracks"};
  char name[200];

  TLegend* injetleg=new TLegend(0.81,0.3,0.99,0.9,NULL,"brNDC");
  for (int i=0;i<6;i++) {
    if(trackinjetfake[i][0]){
      trackinjetfake[i][0]->SetLineColor(4);trackinjetfake[i][0]->SetMarkerStyle(24);trackinjetfake[i][0]->SetMarkerColor(4);
    }
    if(trackinjetfake[i][1]){
      trackinjetfake[i][1]->SetMarkerStyle(20);
    }
    if(trackinjetfake[i][2]){
      trackinjetfake[i][2]->SetLineColor(2);trackinjetfake[i][2]->SetMarkerStyle(26);trackinjetfake[i][2]->SetMarkerColor(2);
    }
    if(trackinjetbad[i][0]){
      trackinjetbad[i][0]->SetLineColor(4);trackinjetbad[i][0]->SetMarkerStyle(24);trackinjetbad[i][0]->SetMarkerColor(4);
    }
    if(trackinjetbad[i][1]){
      trackinjetbad[i][1]->SetMarkerStyle(20);
    }
    if(trackinjetbad[i][2]){
      trackinjetbad[i][2]->SetLineColor(2);trackinjetbad[i][2]->SetMarkerStyle(26);trackinjetbad[i][2]->SetMarkerColor(2);
    }
  }

  if(trackinjetfake[0][0])injetleg->AddEntry(trackinjetfake[0][0],"All");
  if(trackinjetfake[0][1])injetleg->AddEntry(trackinjetfake[0][1],"Good");
  if(trackinjetfake[0][2])injetleg->AddEntry(trackinjetfake[0][2],"Btag");
  injetleg->SetFillColor(0);
  injetleg->SetBorderSize(0);

  for(int i=0;i<6;i++){
    for(int j=0;j<3;j++){
       if(trackinjeteff[i][j]){
 	c->Clear();
 	c->cd(1);
 	trackinjeteff[i][j]->GetXaxis()->SetTitle(maxis[i].c_str());
 	sprintf(name,"Tracking Eff in Jet: %s", myaxismod[j].c_str());
 	trackinjeteff[i][j]->GetYaxis()->SetTitle(name);
 	trackinjeteff[i][j]->Draw("e");
 	sprintf(name,"trackinjeteffplot%i_%i.png",i,j);
 	c->Print(name);
       }
      if(trackinjetfake[i][j]){
	c->Clear();
	c->cd(1);
	trackinjetfake[i][j]->GetXaxis()->SetTitle(maxis[i].c_str());
	sprintf(name,"Fake track rate in Jet: %s", myaxismod[j].c_str());
	trackinjetfake[i][j]->GetYaxis()->SetTitle(name);
	trackinjetfake[i][j]->Draw("e");
	sprintf(name,"trackinjetfakeplot%i_%i.png",i,j);
	c->Print(name);
      }
      if(trackinjetbad[i][j]){
	c->Clear();
	c->cd(1);
	trackinjetbad[i][j]->GetXaxis()->SetTitle(maxis[i].c_str());
	sprintf(name,"Bad Jet match rate: %s", myaxismod[j].c_str());
	trackinjetbad[i][j]->GetYaxis()->SetTitle(name);
	trackinjetbad[i][j]->Draw("e");
	sprintf(name,"trackinjetbadplot%i_%i.png",i,j);
	c->Print(name);
      }
      if(trackinjetbcode[i][j]){
	c->Clear();
	c->cd(1);
	trackinjetbcode[i][j]->GetXaxis()->SetTitle(maxis[i].c_str());
	sprintf(name,"Bad Match or Bcode0 Rate: %s", myaxismod[j].c_str());
	trackinjetbcode[i][j]->GetYaxis()->SetTitle(name);
	trackinjetbcode[i][j]->Draw("e");
	sprintf(name,"trackinjetbcodeplot%i_%i.png",i,j);
	c->Print(name);
      }


    }
    if(trackinjetbad[i][0]&&trackinjetbad[i][1]&&trackinjetbad[i][2]){
      trackinjetbad[i][0]->Draw("e");
      trackinjetbad[i][1]->Draw("esame");
      trackinjetbad[i][2]->Draw("esame");
      leg->Draw();	
      sprintf(name,"trackinjetbadplot_%i.png",i,j);
      c->Print(name);
    }
    if(trackinjetfake[i][0]&&trackinjetfake[i][1]&&trackinjetfake[i][2]){
      trackinjetfake[i][0]->Draw("e");
      trackinjetfake[i][1]->Draw("esame");
      trackinjetfake[i][2]->Draw("esame");
      leg->Draw();	
      sprintf(name,"trackinjetfakeplot_%i.png",i,j);
      c->Print(name);
    }
  }
}
