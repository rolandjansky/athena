{
  gROOT->ProcessLine(".x MoMuPlotStyle.C");
  TFile *f1 = new TFile("TrackAnalysis.root");
  TTree *t1 = (TTree*)f1.Get("");

  //MoMu Standalone part
  c0 = new TCanvas("c0","  ",200,10,700,500);
  c0->Divide(1,1);
  c0->cd(1) ;

  segmentsPMatch_Moore->GetXaxis()->SetTitleSize(0.04) ;
  segmentsPMatch_Moore->GetXaxis()->SetTitleColor(4) ;
  segmentsPMatch_Moore->SetXTitle(" MDT+CSC segments hit ");
  segmentsPMatch_Moore->SetLineWidth(4);
  segmentsPMatch_Moore->SetLineColor(4);
  segmentsPMatch_Moore->Draw("");

  segmentsTruth_Moore->SetLineColor(1);
  segmentsTruth_Moore->SetLineWidth(2);
  segmentsTruth_Moore->Draw("same");

  segmentsReco_Moore->SetLineColor(2);
  segmentsReco_Moore->Draw("same");

  segmentsMatch_Moore->SetLineWidth(4);
  segmentsMatch_Moore->SetLineColor(2);
  segmentsMatch_Moore->Draw("same");

  segmentsSMatch_Moore->SetLineWidth(4);
  segmentsSMatch_Moore->SetLineColor(3);
  segmentsSMatch_Moore->Draw("same");

  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(segmentsTruth_Moore," Simulation","L");
  leg->AddEntry(segmentsReco_Moore," MoMu track ","L");
  leg->AddEntry(segmentsSMatch_Moore," MoMu segments ","L");
  leg->AddEntry(segmentsPMatch_Moore," MoMu patterns ","L");
  leg->Draw("same");

  c0->Print("Segments.eps");

  c1 = new TCanvas("c1","  ",200,10,700,500);
  c1->Divide(1,1);
  c1->cd(1) ;
  mdtSegmentsSMatch_Moore->GetXaxis()->SetTitleSize(0.04) ;
  mdtSegmentsSMatch_Moore->GetXaxis()->SetTitleColor(4) ;
  mdtSegmentsSMatch_Moore->SetXTitle(" MDT segments hit ");
  mdtSegmentsSMatch_Moore->SetLineWidth(2);
  mdtSegmentsSMatch_Moore->SetLineColor(3);
  mdtSegmentsSMatch_Moore->Draw("");

  mdtSegmentsTruth_Moore->SetLineColor(1);
  mdtSegmentsTruth_Moore->SetLineWidth(4);
  mdtSegmentsTruth_Moore->Draw("same");

  mdtSegmentsReco_Moore->SetLineColor(2);
  mdtSegmentsReco_Moore->Draw("same");
  mdtSegmentsMatch_Moore->SetLineWidth(4);
  mdtSegmentsMatch_Moore->SetLineColor(2);
  mdtSegmentsMatch_Moore->Draw("same");

  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(mdtSegmentsTruth_Moore," Simulation","L");
  leg->AddEntry(mdtSegmentsReco_Moore," MoMu track ","L");
  leg->AddEntry(mdtSegmentsSMatch_Moore," MoMu segments ","L");
  leg->Draw("same");

  c1->Print("MdtSegments.eps");

  c2 = new TCanvas("c2","  ",200,10,700,500);
  c2->Divide(1,1);
  c2->cd(1) ;

  mdtSegmentsSMatchBarrel_Moore->GetXaxis()->SetTitleSize(0.04) ;
  mdtSegmentsSMatchBarrel_Moore->GetXaxis()->SetTitleColor(4) ;
  mdtSegmentsSMatchBarrel_Moore->SetXTitle(" MDT Barrel segments hit ");
  mdtSegmentsSMatchBarrel_Moore->SetLineWidth(2);
  mdtSegmentsSMatchBarrel_Moore->SetLineColor(3);
  mdtSegmentsSMatchBarrel_Moore->Draw("");

  mdtSegmentsTruthBarrel_Moore->SetLineWidth(4);
  mdtSegmentsTruthBarrel_Moore->SetLineColor(1);
  mdtSegmentsTruthBarrel_Moore->Draw("same");

  mdtSegmentsRecoBarrel_Moore->SetLineColor(2);
  mdtSegmentsRecoBarrel_Moore->Draw("same");
  mdtSegmentsMatchBarrel_Moore->SetLineWidth(4);
  mdtSegmentsMatchBarrel_Moore->SetLineColor(2);
  mdtSegmentsMatchBarrel_Moore->Draw("same");

  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(mdtSegmentsTruthBarrel_Moore," Simulation","L");
  leg->AddEntry(mdtSegmentsRecoBarrel_Moore," MoMu track ","L");
  leg->AddEntry(mdtSegmentsSMatchBarrel_Moore," MoMu segments ","L");
  leg->Draw("same");

  c2->Print("MdtSegmentsBarrel.eps");


  c21 = new TCanvas("c21","  ",200,10,700,500);
  c21->Divide(1,1);
  c21->cd(1) ;

  mdtSegmentHitsTruth_Moore->GetXaxis()->SetTitleSize(0.04) ;
  mdtSegmentHitsTruth_Moore->GetXaxis()->SetTitleColor(4) ;
  mdtSegmentHitsTruth_Moore->SetXTitle(" MDT Matched Hits on segment ");

  mdtSegmentHitsMatch_Moore->SetLineWidth(4);
  mdtSegmentHitsMatch_Moore->SetLineColor(2);

  mdtSegmentHitsTruth_Moore->SetLineWidth(4);
  mdtSegmentHitsTruth_Moore->SetLineColor(1);

  if(mdtSegmentHitsTruth_Moore->GetMaximum() < mdtSegmentHitsMatch_Moore->GetMaximum()){
   mdtSegmentHitsMatch_Moore->Draw("");
   mdtSegmentHitsTruth_Moore->Draw("same");
  } else{
   mdtSegmentHitsTruth_Moore->Draw("");
   mdtSegmentHitsMatch_Moore->Draw("same");
  }

  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(mdtSegmentHitsTruth_Moore," Simulation","L");
  leg->AddEntry(mdtSegmentHitsMatch_Moore," MoMu segments ","L");
  leg->Draw("same");

  c21->Print("MdtSegmentHits.eps");


  bool cosmics = false;

  if (!cosmics) {
  c3 = new TCanvas("c3","  ",200,10,700,500);
  c3->Divide(1,1);
  c3->cd(1) ;
  cscLayersTruth_Moore->GetXaxis()->SetTitleSize(0.04) ;
  cscLayersTruth_Moore->GetXaxis()->SetTitleColor(4) ;
  cscLayersTruth_Moore->SetXTitle(" CSC Eta Layers hit ");
  cscLayersTruth_Moore->SetLineColor(1);
  cscLayersTruth_Moore->SetLineWidth(4);
  cscLayersTruth_Moore->Draw("");

  cscLayersReco_Moore->SetLineColor(2);
  cscLayersReco_Moore->Draw("same");
  cscLayersMatch_Moore->SetLineColor(2);
  cscLayersMatch_Moore->SetLineWidth(4);
  cscLayersMatch_Moore->Draw("same");


  cscLayersSMatch_Moore->SetLineWidth(2);
  cscLayersSMatch_Moore->SetLineColor(3);
  cscLayersSMatch_Moore->Draw("same");
  
  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(cscLayersTruth_Moore," Simulation","L");
  leg->AddEntry(cscLayersReco_Moore," MoMu track ","L");
  leg->AddEntry(cscLayersMatch_Moore," MoMu segments ","L");
  leg->AddEntry(cscLayersSMatch_Moore," MoMu segments, matched ","L");
  leg->Draw("same");

  c3->Print("CscEtaLayers.eps");
 
  c5 = new TCanvas("c5","  ",200,10,700,500);
  c5->Divide(1,1);
  c5->cd(1) ;
  ptrel_Moore->GetXaxis()->SetTitleSize(0.04) ;
  ptrel_Moore->GetXaxis()->SetTitleColor(4) ;
  ptrel_Moore->SetXTitle(" Delta pt / pt ");
  ptrel_Moore->SetLineColor(2);
  ptrel_Moore->SetLineWidth(2);
  Double_t par[6];
  TF1* g1    = new TF1("g1","gaus",-0.05,0.05);
  TF1* g2    = new TF1("g2","gaus",-0.5,0.5);
  TF1* total = new TF1("total","gaus(0)+gaus(3)",-0.5,0.5);
  total->SetLineColor(4);
  total->SetLineWidth(1);
  ptrel_Moore->Fit(g1,"RN");
  ptrel_Moore->Fit(g2,"NR+");
  g1->GetParameters(&par[0]);
  g2->GetParameters(&par[3]);
  total->SetParameters(par);
  ptrel_Moore->Fit(total,"R+");
  ptrel_Moore->Draw("");
  c5->Print("PtRel.eps");

  c51 = new TCanvas("c51","  ",200,10,700,500);
  c51->Divide(1,1);
  c51->cd(1) ;
  ppull_Moore->GetXaxis()->SetTitleSize(0.04) ;
  ppull_Moore->GetXaxis()->SetTitleColor(4) ;
  ppull_Moore->SetXTitle(" delta q/p / error ");
  ppull_Moore->SetLineColor(2);
  ppull_Moore->SetLineWidth(2);
  ppull_Moore->Draw("");
  c51->Print("PPull.eps");
  }
 
  c4 = new TCanvas("c4","  ",200,10,700,500);
  c4->Divide(1,1);
  c4->cd(1) ;
  eta_mis_Moore->GetXaxis()->SetTitleSize(0.04) ;
  eta_mis_Moore->GetXaxis()->SetTitleColor(4) ;
  eta_mis_Moore->SetXTitle(" Eta missed tracks ");
  eta_mis_Moore->SetLineColor(2);
  eta_mis_Moore->SetLineWidth(4);
  eta_mis_Moore->Draw("");
  c4->Print("EtaMissed.eps");

  c40 = new TCanvas("c4","  ",200,10,700,500);
  c40->Divide(1,1);
  c40->cd(1); 
  TH1D *eta_eff_Moore = new TH1D("eta_eff_Moore"," ",100,-5.,5.);
  eta_eff_Moore->Sumw2();
  eta_eff_Moore->Add(eta_sim_Moore,eta_mis_Moore,1.,-1.);
  eta_eff_Moore->Divide(eta_eff_Moore,eta_sim_Moore,1.,1.,"B");
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
  c40->Print("EtaEfficiency.eps");

  c41 = new TCanvas("c41","  ",200,10,700,500);
  c41->Divide(1,1);
  c41->cd(1) ;
  psi_sim_Moore->Rebin(10);
  psi_mis_Moore->Rebin(10);
  TH1D *ratpsi_Moore = new TH1D("ratpsi_Moore"," ",10,0.,3.14);  
  ratpsi_Moore->Sumw2();
  ratpsi_Moore->Add(psi_sim_Moore,psi_mis_Moore,1.,-1.);
  ratpsi_Moore->Divide(ratpsi_Moore,psi_sim_Moore,1.,1.);
  ratpsi_Moore->GetXaxis()->SetTitleSize(0.04) ;
  ratpsi_Moore->GetXaxis()->SetTitleColor(4) ;
  ratpsi_Moore->SetXTitle(" Opening angle ");
  ratpsi_Moore->SetYTitle(" Barrel Efficiency ");
  ratpsi_Moore->SetLineColor(2);
  ratpsi_Moore->SetLineWidth(4);
  ratpsi_Moore->Draw("");
  c41->Print("BarrelEffPsi.eps");

  c42 = new TCanvas("c42","  ",200,10,700,500);
  c42->Divide(1,1);
  c42->cd(1) ;
  psif_sim_Moore->Rebin(10);
  psif_mis_Moore->Rebin(10);
  TH1D *ratpsif_Moore = new TH1D("ratpsif_Moore"," ",10,0.,3.14);  
  ratpsif_Moore->Sumw2();
  ratpsif_Moore->Add(psif_sim_Moore,psif_mis_Moore,1.,-1.);
  ratpsif_Moore->Divide(ratpsif_Moore,psif_sim_Moore,1.,1.);
  ratpsif_Moore->GetXaxis()->SetTitleSize(0.04) ;
  ratpsif_Moore->GetXaxis()->SetTitleColor(4) ;
  ratpsif_Moore->SetXTitle(" Opening angle ");
  ratpsif_Moore->SetYTitle(" Forward Efficiency ");
  ratpsif_Moore->SetLineColor(2);
  ratpsif_Moore->SetLineWidth(4);
  ratpsif_Moore->Draw(""); 
  c42->Print("FwdEffPsi.eps");


  c6 = new TCanvas("c6","  ",200,10,700,500);
  c6->Divide(1,1);
  c6->cd(1) ;
  TVirtualPad* Pad=c6->cd(1) ;
  Pad->SetLogy();
  chi2dof_Moore->GetXaxis()->SetTitleSize(0.04) ;
  chi2dof_Moore->GetXaxis()->SetTitleColor(4) ;
  chi2dof_Moore->GetXaxis()->SetRange(0,100);
  chi2dof_Moore->SetXTitle(" Chi2/dof track ");
  chi2dof_Moore->SetLineColor(2);
  chi2dof_Moore->SetLineWidth(4);
  chi2dof_Moore->Draw("");
  c6->Print("Chi2Dof.eps");

  c61 = new TCanvas("c61","  ",200,10,700,500);
  c61->Divide(1,1);
  c61->cd(1) ;
  TVirtualPad* Pad=c61->cd(1) ;
  Pad->SetLogy();
  tracksFake_Moore->GetXaxis()->SetTitleSize(0.04) ;
  tracksFake_Moore->GetXaxis()->SetTitleColor(4) ;
  tracksFake_Moore->SetXTitle(" number of fake tracks ");
  tracksFake_Moore->SetLineColor(2);
  tracksFake_Moore->SetLineWidth(4);
  tracksFake_Moore->Draw("");
  c61->Print("TrackFake.eps");


  c62 = new TCanvas("c62","  ",200,10,700,500);
  c62->Divide(1,1);
  c62->cd(1) ;
  TVirtualPad* Pad=c62->cd(1) ;
  eta_ptout_Moore->GetXaxis()->SetTitleSize(0.07) ;
  eta_ptout_Moore->GetXaxis()->SetTitleColor(4) ;
  eta_ptout_Moore->SetXTitle(" eta ");
  eta_ptout_Moore->SetLineColor(2);
  eta_ptout_Moore->SetLineWidth(2);
  eta_ptout_Moore->Draw("");
  TLegend* legp = new TLegend(0.60,0.7,0.9,0.9);
  legp->SetFillColor(0);
  legp->SetTextSize(.05);
  legp->AddEntry(eta_ptout_Moore,"|dp/p|>0.2","L");
  legp->Draw("same");

  c62->Print("EtaOutliers.eps");

  c7 = new TCanvas("c7","  ",200,10,700,500);
  c7->Divide(1,1);
  c7->cd(1) ;

  rpcLayersPMatch_Moore->GetXaxis()->SetTitleSize(0.04) ;
  rpcLayersPMatch_Moore->GetXaxis()->SetTitleColor(4) ;
  rpcLayersPMatch_Moore->SetXTitle(" RPC Phi Layers hit ");
  rpcLayersPMatch_Moore->SetLineWidth(4);
  rpcLayersPMatch_Moore->SetLineColor(4);
  rpcLayersPMatch_Moore->Draw("");

  rpcLayersTruth_Moore->SetLineColor(1);
  rpcLayersTruth_Moore->SetLineWidth(2);
  rpcLayersTruth_Moore->Draw("same");

  rpcLayersReco_Moore->SetLineColor(2);
  rpcLayersReco_Moore->Draw("same");

  rpcLayersMatch_Moore->SetLineColor(2);
  rpcLayersMatch_Moore->SetLineWidth(4);
  rpcLayersMatch_Moore->Draw("same");

  rpcLayersSMatch_Moore->SetLineWidth(4);
  rpcLayersSMatch_Moore->SetLineColor(3);
  rpcLayersSMatch_Moore->Draw("same");

  TLegend* leg = new TLegend(0.70,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(rpcLayersTruth_Moore," Simulation","L");
  leg->AddEntry(rpcLayersReco_Moore," MoMu track ","L");
  leg->AddEntry(rpcLayersSMatch_Moore," MoMu segments ","L");
  leg->AddEntry(rpcLayersPMatch_Moore," MoMu patterns ","L");
  leg->Draw("same");

  c7->Print("RpcLayers.eps");

  bool cosmics = false;
  if (!cosmics) {
 
  c8 = new TCanvas("c8","  ",200,10,700,500);
  c8->Divide(1,1);
  c8->cd(1) ;
  tgcLayersPMatch_Moore->GetXaxis()->SetTitleSize(0.04) ;
  tgcLayersPMatch_Moore->GetXaxis()->SetTitleColor(4) ;
  tgcLayersPMatch_Moore->SetXTitle(" TGC Phi Layers hit ");
  tgcLayersPMatch_Moore->SetLineWidth(4);
  tgcLayersPMatch_Moore->SetLineColor(4);
  tgcLayersPMatch_Moore->Draw("");

  tgcLayersTruth_Moore->SetLineColor(1);
  tgcLayersTruth_Moore->SetLineWidth(4);
  tgcLayersTruth_Moore->Draw("same");

  tgcLayersReco_Moore->SetLineColor(2);
  tgcLayersReco_Moore->Draw("same");
  tgcLayersMatch_Moore->SetLineColor(2);
  tgcLayersMatch_Moore->SetLineWidth(4);
  tgcLayersMatch_Moore->Draw("same");

  tgcLayersSMatch_Moore->SetLineColor(3);
  tgcLayersSMatch_Moore->SetLineWidth(4);
  tgcLayersSMatch_Moore->Draw("same");

  TLegend* leg = new TLegend(0.70,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(tgcLayersTruth_Moore," Simulation","L");
  leg->AddEntry(tgcLayersReco_Moore," MoMu track ","L");
  leg->AddEntry(tgcLayersSMatch_Moore," MoMu segments ","L");
  leg->AddEntry(tgcLayersPMatch_Moore," MoMu patterns ","L");
  leg->Draw("same");

  c8->Print("TgcLayers.eps");
 
  c9 = new TCanvas("c9","  ",200,10,700,500);
  c9->Divide(1,1);
  c9->cd(1) ;
  cscPhiLayersSMatch_Moore->GetXaxis()->SetTitleSize(0.04) ;
  cscPhiLayersSMatch_Moore->GetXaxis()->SetTitleColor(4) ;
  cscPhiLayersSMatch_Moore->SetXTitle(" CSC Phi Layers hit ");
  cscPhiLayersSMatch_Moore->SetLineColor(3);
  cscPhiLayersSMatch_Moore->SetLineWidth(4);
  cscPhiLayersSMatch_Moore->Draw("");
  
  cscPhiLayersTruth_Moore->SetLineColor(1);
  cscPhiLayersTruth_Moore->SetLineWidth(4);
  cscPhiLayersTruth_Moore->Draw("same");

  cscPhiLayersReco_Moore->SetLineColor(2);
  cscPhiLayersReco_Moore->Draw("same");
  cscPhiLayersMatch_Moore->SetLineColor(2);
  cscPhiLayersMatch_Moore->SetLineWidth(4);
  cscPhiLayersMatch_Moore->Draw("same");

  TLegend* leg = new TLegend(0.60,0.7,0.9,0.9);
  leg->SetFillColor(0);
  leg->SetTextSize(.05);
  leg->AddEntry(cscLayersTruth_Moore," Simulation","L");
  leg->AddEntry(cscLayersReco_Moore," MoMu track ","L");
  leg->AddEntry(cscLayersMatch_Moore," MoMu segments ","L");
  leg->AddEntry(cscLayersSMatch_Moore," MoMu segments, matched ","L");
  leg->Draw("same");

  c9->Print("CscPhiLayers.eps");


  c10 = new TCanvas("c10","  ",200,10,700,500);
  c10->Divide(1,1);
  c10->cd(1) ;
  mdtSegmentsMissed_Moore->GetXaxis()->SetTitleSize(0.07) ;
  mdtSegmentsMissed_Moore->GetXaxis()->SetTitleColor(4) ;
  mdtSegmentsMissed_Moore->SetXTitle("missed MDT segments/event");
  mdtSegmentsMissed_Moore->SetLineColor(2);
  mdtSegmentsMissed_Moore->SetLineWidth(4);
  mdtSegmentsMissed_Moore->GetXaxis()->SetRange(2,24);
  Double_t max = mdtSegmentsMissed_Moore->GetMaximum();
  mdtSegmentsMissed_Moore->SetMaximum(1.25*max);
  mdtSegmentsMissed_Moore->Draw();
  TText *tt1 = new TText(1.5,1.15*max,"Inner Middle Outer bits 0 1 2");
  TText *tt2 = new TText(8.,1.05*max,"|" );
  TText *tt3 = new TText(16.,1.05*max,"|" );
  TText *tt4 = new TText(1.,1.05*max," Barrel          B-E        Endcap" );
  tt1->Draw("same");
  tt2->Draw("same");
  tt3->Draw("same");
  tt4->Draw("same");

  c10->Print("MdtMissedSegments.eps");

  c11 = new TCanvas("c11","  ",200,10,700,500);
  c11->Divide(1,1);
  c11->cd(1) ;
  trackSegmentsMissed_Moore->GetXaxis()->SetTitleSize(0.07) ;
  trackSegmentsMissed_Moore->GetXaxis()->SetTitleColor(4) ;
  trackSegmentsMissed_Moore->SetXTitle("missed segments track ");
  trackSegmentsMissed_Moore->SetLineColor(2);
  trackSegmentsMissed_Moore->SetLineWidth(4);
  trackSegmentsMissed_Moore->GetXaxis()->SetRange(2,32);
  Double_t max = trackSegmentsMissed_Moore->GetMaximum();
  trackSegmentsMissed_Moore->SetMaximum(1.25*max);
  trackSegmentsMissed_Moore->Draw();
  TText *tt1 = new TText(1.5,1.15*max,"Inner Middle Outer bits 0 1 2");
  TText *tt2 = new TText(8.,1.05*max,"|" );
  TText *tt3 = new TText(16.,1.05*max,"|" );
  TText *tt4 = new TText(24.,1.05*max,"|" );
  TText *tt5 = new TText(1.,1.05*max,"Barrel    B-E       End     CSC" );
  tt1->Draw("same");
  tt2->Draw("same");
  tt3->Draw("same");
  tt4->Draw("same");
  tt5->Draw("same");
  
  c11->Print("MissedSegments.eps");

  }

  gSystem->Exec("./MooRTT_alleps2gif.py");
  gSystem->Exec("./MooRTT_summarizeCPU.py");

}
