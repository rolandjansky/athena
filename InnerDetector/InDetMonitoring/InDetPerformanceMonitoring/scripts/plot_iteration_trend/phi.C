TLegend *lgd = 0;

void core_sagitta(const char* filename="../outputmaps_initial/correctionmaps_Refit1_SiAndTRTpreTSI_Initial.preTSI_initialRefit2_SiOnly.root", const char* title="", const int marker=20, const int color=kGray+1) {
  TFile *file = new TFile(filename);
  file->ls();
  TH2D *h2 = (TH2D*)(file->Get("LambdaCorrectionVsEtaPhi"));

  TProfile *prof = new TProfile("prof", "", h2->GetNbinsY(), -TMath::Pi(), TMath::Pi(), "s");
  for(int ibin=0; ibin<h2->GetNbinsX(); ibin++) {
    for(int jbin=0; jbin<h2->GetNbinsY(); jbin++) {
      prof->Fill( h2->GetYaxis()->GetBinCenter(jbin+1), h2->GetBinContent(ibin+1, jbin+1) );
    }
  }
  prof->SetMarkerStyle(marker);
  prof->SetMarkerColor(color);
  prof->SetMarkerSize(1.0);
  prof->SetLineColor(color);
  prof->GetYaxis()->SetRangeUser(-1.0, 1.0);
  prof->SetTitle(";#phi;#deltaS [TeV^{-1}];");
  prof->Draw("pe,x0,same");

  if( !lgd ) {
    lgd = new TLegend(0.7, 0.7, 0.9, 0.9);
    lgd->SetBorderSize(0);
    lgd->SetFillStyle(0);
    lgd->SetTextFont(42);
  }

  lgd->AddEntry(prof, title, "pe");
}

void core_d0(const char* filename="../outputmaps_initial/correctionmaps_Refit1_SiAndTRTpreTSI_Initial.preTSI_initialRefit2_SiOnly.root", const char* title="", const int marker=20, const int color=kGray+1) {
  TFile *file = new TFile(filename);
  file->ls();
  TH2D *h2 = (TH2D*)(file->Get("d0CorrectionVsEtaPhi"));

  TProfile *prof = new TProfile("prof", "", h2->GetNbinsY(), -TMath::Pi(), TMath::Pi(), "s");
  for(int ibin=0; ibin<h2->GetNbinsX(); ibin++) {
    for(int jbin=0; jbin<h2->GetNbinsY(); jbin++) {
      prof->Fill( h2->GetYaxis()->GetBinCenter(jbin+1), h2->GetBinContent(ibin+1, jbin+1)*1.e3 );
    }
  }
  prof->SetMarkerStyle(marker);
  prof->SetMarkerColor(color);
  prof->SetMarkerSize(1.0);
  prof->SetLineColor(color);
  prof->GetYaxis()->SetRangeUser(-5.0, 5.0);
  prof->SetTitle(";#phi;#deltad_{0} [#mum];");
  prof->Draw("pe,x0,same");

  if( !lgd ) {
    lgd = new TLegend(0.7, 0.7, 0.9, 0.9);
    lgd->SetBorderSize(0);
    lgd->SetFillStyle(0);
    lgd->SetTextFont(42);
  }

}

void core_z0(const char* filename="../outputmaps_initial/correctionmaps_Refit1_SiAndTRTpreTSI_Initial.preTSI_initialRefit2_SiOnly.root", const char* title="", const int marker=20, const int color=kGray+1) {
  TFile *file = new TFile(filename);
  file->ls();
  TH2D *h2 = (TH2D*)(file->Get("z0CorrectionVsEtaPhi"));

  TProfile *prof = new TProfile("prof", "", h2->GetNbinsY(), -TMath::Pi(), TMath::Pi(), "s");
  for(int ibin=0; ibin<h2->GetNbinsX(); ibin++) {
    for(int jbin=0; jbin<h2->GetNbinsY(); jbin++) {
      prof->Fill( h2->GetYaxis()->GetBinCenter(jbin+1), h2->GetBinContent(ibin+1, jbin+1)*1.e3 );
    }
  }
  prof->SetMarkerStyle(marker);
  prof->SetMarkerColor(color);
  prof->SetMarkerSize(1.0);
  prof->SetLineColor(color);
  prof->GetYaxis()->SetRangeUser(-50.0, 50.0);
  prof->SetTitle(";#phi;#deltaz_{0} [#mum];");
  prof->Draw("pe,x0,same");

  if( !lgd ) {
    lgd = new TLegend(0.7, 0.7, 0.9, 0.9);
    lgd->SetBorderSize(0);
    lgd->SetFillStyle(0);
    lgd->SetTextFont(42);
  }

}

void phi() {
  SetAtlasStyle();

  new TCanvas();
  //core_sagitta("../outputmaps_initial/correctionmaps_Refit1_SiAndTRTpreTSI_Initial.preTSI_initialRefit2_SiOnly.root", "Initial", 20, kGray+2);
  core_sagitta("../outputmaps_Iter3/correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter3.preTS1_Iter3Refit2_SiOnly.root", "Iter3 (L3)", 21, kBlack);
  lgd->Draw();
  gPad->SaveAs("phi_sagitta.pdf");
  
  new TCanvas();
  //core_d0("../outputmaps_initial/correctionmaps_Refit1_SiAndTRTpreTSI_Initial.preTSI_initialRefit2_SiOnly.root", "Initial", 20, kGray+2);
  core_d0("../outputmaps_Iter3/correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter3.preTS1_Iter3Refit2_SiOnly.root", "Iter3 (L3)", 21, kBlack);
  lgd->Draw();
  gPad->SaveAs("phi_d0.pdf");

  new TCanvas();
  //core_z0("../outputmaps_initial/correctionmaps_Refit1_SiAndTRTpreTSI_Initial.preTSI_initialRefit2_SiOnly.root", "Initial", 20, kGray+2);
  core_z0("../outputmaps_Iter3/correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter3.preTS1_Iter3Refit2_SiOnly.root", "Iter3 (L3)", 21, kBlack);
  lgd->Draw();
  gPad->SaveAs("phi_z0.pdf");
}

