/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



//
void plotTightWPMaps()
{

  // Global styles 
  gStyle->SetOptStat(0);

  // 
  TString inputFile = "../data/muonSelection_tightWPHisto_2016_03_15.root";

  //
  TFile* file = TFile::Open( inputFile.Data(), "READ" );

  if( !file->IsOpen() ){
    abort();
  }

  
  TH2D* tightWP_lowPt_rhoCuts = dynamic_cast<TH2D*>( file->Get("tightWP_lowPt_rhoCuts") );
  TH2D* tightWP_lowPt_qOverPCuts = dynamic_cast<TH2D*>( file->Get("tightWP_lowPt_qOverPCuts") );
  TH2D* tightWP_mediumPt_rhoCuts = dynamic_cast<TH2D*>( file->Get("tightWP_mediumPt_rhoCuts") );
  TH2D* tightWP_highPt_rhoCuts = dynamic_cast<TH2D*>( file->Get("tightWP_highPt_rhoCuts") );

  TCanvas* c1 = new TCanvas("c1","c1",800,600);
  c1->SetRightMargin(0.19);
  c1->SetBottomMargin(0.15);

  tightWP_lowPt_rhoCuts->GetYaxis()->SetTitle("Muon |#eta|");
  tightWP_lowPt_rhoCuts->GetXaxis()->SetTitle("Muon p_{T} [GeV]");
  tightWP_lowPt_rhoCuts->GetZaxis()->SetTitle("Best #rho cut value");
  tightWP_lowPt_rhoCuts->SetTitle("");
  tightWP_lowPt_rhoCuts->GetXaxis()->SetTitleSize(0.05);
  tightWP_lowPt_rhoCuts->GetYaxis()->SetTitleSize(0.05);
  tightWP_lowPt_rhoCuts->GetZaxis()->SetTitleSize(0.05);
  tightWP_lowPt_rhoCuts->GetXaxis()->SetLabelSize(0.05);
  tightWP_lowPt_rhoCuts->GetYaxis()->SetLabelSize(0.05);
  tightWP_lowPt_rhoCuts->GetZaxis()->SetLabelSize(0.05);
  tightWP_lowPt_rhoCuts->GetXaxis()->SetTitleOffset(1.08);
  tightWP_lowPt_rhoCuts->GetYaxis()->SetTitleOffset(1.00);
  tightWP_lowPt_rhoCuts->GetZaxis()->SetTitleOffset(1.08);
  tightWP_lowPt_rhoCuts->Draw("colz text");

  c1->SaveAs("tightWP_lowPt_rhoCuts.eps");
  c1->SaveAs("tightWP_lowPt_rhoCuts.pdf");
  
  // -------------------------------------------- //

  TCanvas* c2 = new TCanvas("c2","c2",800,600);
  c2->SetRightMargin(0.19);
  c2->SetBottomMargin(0.15);

  tightWP_lowPt_qOverPCuts->GetYaxis()->SetTitle("Muon |#eta|");
  tightWP_lowPt_qOverPCuts->GetXaxis()->SetTitle("Muon p_{T} [GeV]");
  tightWP_lowPt_qOverPCuts->GetZaxis()->SetTitle("Best |(q/p)^{ID}-(q/p)^{ME}|/#sqrt{#sigma^{ID}+#sigma^{ME}} cut value");
  tightWP_lowPt_qOverPCuts->SetTitle("");
  tightWP_lowPt_qOverPCuts->GetXaxis()->SetTitleSize(0.05);
  tightWP_lowPt_qOverPCuts->GetYaxis()->SetTitleSize(0.05);
  tightWP_lowPt_qOverPCuts->GetZaxis()->SetTitleSize(0.04);
  tightWP_lowPt_qOverPCuts->GetXaxis()->SetLabelSize(0.05);
  tightWP_lowPt_qOverPCuts->GetYaxis()->SetLabelSize(0.05);
  tightWP_lowPt_qOverPCuts->GetZaxis()->SetLabelSize(0.05);
  tightWP_lowPt_qOverPCuts->GetXaxis()->SetTitleOffset(1.08);
  tightWP_lowPt_qOverPCuts->GetYaxis()->SetTitleOffset(1.00);
  tightWP_lowPt_qOverPCuts->GetZaxis()->SetTitleOffset(1.25);
  tightWP_lowPt_qOverPCuts->Draw("colz text");

  c2->SaveAs("tightWP_lowPt_qOverPCuts.eps");
  c2->SaveAs("tightWP_lowPt_qOverPCuts.pdf");					 

  // -------------------------------------------- //

  TCanvas* c3 = new TCanvas("c3","c3",800,600);
  c3->SetRightMargin(0.19);
  c3->SetBottomMargin(0.15);

  tightWP_mediumPt_rhoCuts->GetYaxis()->SetTitle("Muon |#eta|");
  tightWP_mediumPt_rhoCuts->GetXaxis()->SetTitle("Muon p_{T} [GeV]");
  tightWP_mediumPt_rhoCuts->GetZaxis()->SetTitle("Best #rho cut value");
  tightWP_mediumPt_rhoCuts->SetTitle("");
  tightWP_mediumPt_rhoCuts->GetXaxis()->SetTitleSize(0.05);
  tightWP_mediumPt_rhoCuts->GetYaxis()->SetTitleSize(0.05);
  tightWP_mediumPt_rhoCuts->GetZaxis()->SetTitleSize(0.05);
  tightWP_mediumPt_rhoCuts->GetXaxis()->SetLabelSize(0.05);
  tightWP_mediumPt_rhoCuts->GetYaxis()->SetLabelSize(0.05);
  tightWP_mediumPt_rhoCuts->GetZaxis()->SetLabelSize(0.05);
  tightWP_mediumPt_rhoCuts->GetXaxis()->SetTitleOffset(1.08);
  tightWP_mediumPt_rhoCuts->GetYaxis()->SetTitleOffset(1.00);
  tightWP_mediumPt_rhoCuts->GetZaxis()->SetTitleOffset(1.08);
  tightWP_mediumPt_rhoCuts->Draw("colz text");

  c3->SaveAs("tightWP_mediumPt_rhoCuts.eps");
  c3->SaveAs("tightWP_mediumPt_rhoCuts.pdf");					 

  // -------------------------------------------- //

  TCanvas* c4 = new TCanvas("c4","c4",800,600);
  c4->SetRightMargin(0.19);
  c4->SetBottomMargin(0.15);

  tightWP_highPt_rhoCuts->GetYaxis()->SetTitle("Muon |#eta|");
  tightWP_highPt_rhoCuts->GetXaxis()->SetTitle("Muon p_{T} [GeV]");
  tightWP_highPt_rhoCuts->GetZaxis()->SetTitle("Best #rho cut value");
  tightWP_highPt_rhoCuts->SetTitle("");
  tightWP_highPt_rhoCuts->GetXaxis()->SetTitleSize(0.05);
  tightWP_highPt_rhoCuts->GetYaxis()->SetTitleSize(0.05);
  tightWP_highPt_rhoCuts->GetZaxis()->SetTitleSize(0.05);
  tightWP_highPt_rhoCuts->GetXaxis()->SetLabelSize(0.05);
  tightWP_highPt_rhoCuts->GetYaxis()->SetLabelSize(0.05);
  tightWP_highPt_rhoCuts->GetZaxis()->SetLabelSize(0.05);
  tightWP_highPt_rhoCuts->GetXaxis()->SetTitleOffset(1.08);
  tightWP_highPt_rhoCuts->GetYaxis()->SetTitleOffset(1.00);
  tightWP_highPt_rhoCuts->GetZaxis()->SetTitleOffset(1.08);
  tightWP_highPt_rhoCuts->Draw("colz text");

  c3->SaveAs("tightWP_highPt_rhoCuts.eps");
  c3->SaveAs("tightWP_highPt_rhoCuts.pdf");

}
