TLegend *lgd_sagitta = 0;
TLegend *lgd_d0 = 0;
TLegend *lgd_z0 = 0;

TGraphAsymmErrors *gr_iter_sagitta = 0;
TGraphAsymmErrors *gr_iter_d0 = 0;
TGraphAsymmErrors *gr_iter_z0 = 0;

TGraphAsymmErrors *gr_iter_max_sagitta = 0;
TGraphAsymmErrors *gr_iter_max_d0 = 0;
TGraphAsymmErrors *gr_iter_max_z0 = 0;

const double ymin = 0.65;

const int n_iter = 5;

void core_sagitta(const char* filename="preTSI_Initial.preTSI_initialRefit2_SiOnly.root", const char* title="Initial", const int color = kGray+2, const int marker = 20, const double pos = 0.9) {
  TFile *file = TFile::Open( filename );
  if( !file ) return;
  TH2D *sagittaMap = dynamic_cast<TH2D*>( file->Get("LambdaCorrectionVsEtaPhi") );

  TH1F *sagittaDispersion = new TH1F("sagittaDispersion", ";#deltas [TeV^{-1}];Fraction of (#eta-#phi) cells / 0.1 TeV^{-1}", 40, -2.0, 2.0);
  double max = -9999.;
  double min = 9999.;
  for(int ibin=0; ibin<sagittaMap->GetNbinsX(); ibin++) {
    for(int jbin=0; jbin<sagittaMap->GetNbinsY(); jbin++) {
      double val = sagittaMap->GetBinContent(ibin+1, jbin+1);
      sagittaDispersion->Fill( val );
      if( val > max ) max = val;
      if( val < min ) min = val;
    }
  }
  //sagittaDispersion->Sumw2();
  sagittaDispersion->SetMarkerSize(0.7);
  sagittaDispersion->SetMarkerColor(color);
  sagittaDispersion->SetMarkerStyle( marker );
  sagittaDispersion->SetLineColor(color);
  sagittaDispersion->Scale( 1.0/(sagittaMap->GetNbinsX()*sagittaMap->GetNbinsY()) );
  sagittaDispersion->SetMaximum( sagittaDispersion->GetMaximum()*2.0 );
  sagittaDispersion->Draw("hp,same");

  TF1 *fit = new TF1("fit", "gaus", -2.0, 2.0);
  fit->SetNpx(1000);
  fit->SetLineColor(color);
  sagittaDispersion->Fit(fit->GetName(), "rq0");
  //fit->Draw("same");
  //sagittaDispersion->Draw("same");

  if( !lgd_sagitta ) {
    lgd_sagitta = new TLegend( 0.2, ymin, 0.9, 0.9 );
    lgd_sagitta->SetBorderSize(0);
    lgd_sagitta->SetFillStyle(0);
    lgd_sagitta->SetTextFont(42);
  }
  lgd_sagitta->AddEntry( sagittaDispersion, Form("%s", title), "lp");

  if( !gr_iter_sagitta ) {
    gr_iter_sagitta = new TGraphAsymmErrors();
  }

  gr_iter_sagitta->SetPoint(gr_iter_sagitta->GetN(), gr_iter_sagitta->GetN(), sagittaDispersion->GetMean() );
  gr_iter_sagitta->SetPointError(gr_iter_sagitta->GetN()-1, 0.5, 0.5, sagittaDispersion->GetRMS(), sagittaDispersion->GetRMS() );

  if( !gr_iter_max_sagitta ) {
    gr_iter_max_sagitta = new TGraphAsymmErrors();
  }

  gr_iter_max_sagitta->SetPoint(gr_iter_max_sagitta->GetN(), gr_iter_max_sagitta->GetN(), sagittaDispersion->GetMean() );
  gr_iter_max_sagitta->SetPointError(gr_iter_max_sagitta->GetN()-1, 0.5, 0.5, fabs(max-sagittaDispersion->GetMean()), fabs(min-sagittaDispersion->GetMean()) );

  cout << title << ": " << sagittaDispersion->GetRMS() << endl;

  //TLatex latex;
  //latex.DrawLatexNDC(0.2, pos, Form("Dispersion: %.2f TeV^{-1}", sagittaDispersion->GetRMS()));
}

void core_d0(const char* filename="preTSI_Initial.preTSI_initialRefit2_SiOnly.root", const char* title="Initial", const int color = kGray+2, const int marker = 20, const double pos = 0.9) {
  TFile *file = TFile::Open( filename );
  if( !file ) return;
  TH2D *d0Map = dynamic_cast<TH2D*>( file->Get("d0CorrectionVsEtaPhi") );

  TH1F *d0Dispersion = new TH1F("d0Dispersion", ";#deltad_{0} [#mum];Fraction of (#eta-#phi) cells / 0.5 #mum", 40, -10.0, 10.0);
  double max = -9999.;
  double min = 9999.;
  for(int ibin=0; ibin<d0Map->GetNbinsX(); ibin++) {
    for(int jbin=0; jbin<d0Map->GetNbinsY(); jbin++) {
      double val = d0Map->GetBinContent(ibin+1, jbin+1)*1.e3;
      d0Dispersion->Fill( val );
      if( val > max ) max = val;
      if( val < min ) min = val;
    }
  }
  //d0Dispersion->Sumw2();
  d0Dispersion->SetMarkerSize(0.7);
  d0Dispersion->SetMarkerColor(color);
  d0Dispersion->SetMarkerStyle( marker );
  d0Dispersion->SetLineColor(color);
  d0Dispersion->Scale( 1.0/(d0Map->GetNbinsX()*d0Map->GetNbinsY()) );
  d0Dispersion->SetMaximum( d0Dispersion->GetMaximum()*2.0 );
  d0Dispersion->Draw("hp,same");

  TF1 *fit = new TF1("fit", "gaus", -2.0, 2.0);
  fit->SetNpx(1000);
  fit->SetLineColor(color);
  d0Dispersion->Fit(fit->GetName(), "rq0");
  //fit->Draw("same");
  //d0Dispersion->Draw("same");

  if( !lgd_d0 ) {
    lgd_d0 = new TLegend( 0.2, ymin, 0.9, 0.9 );
    lgd_d0->SetBorderSize(0);
    lgd_d0->SetFillStyle(0);
    lgd_d0->SetTextFont(42);
  }
  lgd_d0->AddEntry( d0Dispersion, Form("%s", title), "lp");
    

  if( !gr_iter_d0 ) {
    gr_iter_d0 = new TGraphAsymmErrors();
  }

  gr_iter_d0->SetPoint(gr_iter_d0->GetN(), gr_iter_d0->GetN(), d0Dispersion->GetMean() );
  gr_iter_d0->SetPointError(gr_iter_d0->GetN()-1, 0.5, 0.5, d0Dispersion->GetRMS(), d0Dispersion->GetRMS() );

  if( !gr_iter_max_d0 ) {
    gr_iter_max_d0 = new TGraphAsymmErrors();
  }

  gr_iter_max_d0->SetPoint(gr_iter_max_d0->GetN(), gr_iter_max_d0->GetN(), d0Dispersion->GetMean() );
  gr_iter_max_d0->SetPointError(gr_iter_max_d0->GetN()-1, 0.5, 0.5, fabs(max-d0Dispersion->GetMean()), fabs(min-d0Dispersion->GetMean()) );

  cout << title << ": " << d0Dispersion->GetRMS() << endl;
  
  //TLatex latex;
  //latex.DrawLatexNDC(0.2, pos, Form("Dispersion: %.2f TeV^{-1}", d0Dispersion->GetRMS()));
}

void core_z0(const char* filename="preTSI_Initial.preTSI_initialRefit2_SiOnly.root", const char* title="Initial", const int color = kGray+2, const int marker = 20, const double pos = 0.9) {
  TFile *file = TFile::Open( filename );
  if( !file ) return;
  TH2D *z0Map = dynamic_cast<TH2D*>( file->Get("z0CorrectionVsEtaPhi") );

  TH1F *z0Dispersion = new TH1F("z0Dispersion", ";#deltaz_{0} [#mum];Fraction of (#eta-#phi) cells / 5 #mum", 40, -100, 100);
  double max = -9999.;
  double min = 9999.;
  for(int ibin=0; ibin<z0Map->GetNbinsX(); ibin++) {
    for(int jbin=0; jbin<z0Map->GetNbinsY(); jbin++) {
      double val = z0Map->GetBinContent(ibin+1, jbin+1)*1.e3;
      z0Dispersion->Fill( val );
      if( val > max ) max = val;
      if( val < min ) min = val;
    }
  }
  //z0Dispersion->Sumw2();
  z0Dispersion->SetMarkerSize(0.7);
  z0Dispersion->SetMarkerColor(color);
  z0Dispersion->SetMarkerStyle( marker );
  z0Dispersion->SetLineColor(color);
  z0Dispersion->Scale( 1.0/(z0Map->GetNbinsX()*z0Map->GetNbinsY()) );
  z0Dispersion->SetMaximum( z0Dispersion->GetMaximum()*3.0 );
  z0Dispersion->Draw("hp,same");

  TF1 *fit = new TF1("fit", "gaus", -2.0, 2.0);
  fit->SetNpx(1000);
  fit->SetLineColor(color);
  z0Dispersion->Fit(fit->GetName(), "rq0");
  //fit->Draw("same");
  //z0Dispersion->Draw("same");

  if( !lgd_z0 ) {
    lgd_z0 = new TLegend( 0.2, ymin, 0.9, 0.9 );
    lgd_z0->SetBorderSize(0);
    lgd_z0->SetFillStyle(0);
    lgd_z0->SetTextFont(42);
  }
  lgd_z0->AddEntry( z0Dispersion, Form("%s", title), "lp");
    

  if( !gr_iter_z0 ) {
    gr_iter_z0 = new TGraphAsymmErrors();
  }

  gr_iter_z0->SetPoint(gr_iter_z0->GetN(), gr_iter_z0->GetN(), z0Dispersion->GetMean() );
  gr_iter_z0->SetPointError(gr_iter_z0->GetN()-1, 0.5, 0.5, z0Dispersion->GetRMS(), z0Dispersion->GetRMS() );

  if( !gr_iter_max_z0 ) {
    gr_iter_max_z0 = new TGraphAsymmErrors();
  }

  gr_iter_max_z0->SetPoint(gr_iter_max_z0->GetN(), gr_iter_max_z0->GetN(), z0Dispersion->GetMean() );
  gr_iter_max_z0->SetPointError(gr_iter_max_z0->GetN()-1, 0.5, 0.5, fabs(max-z0Dispersion->GetMean()), fabs(min-z0Dispersion->GetMean()) );

  cout << title << ": " << z0Dispersion->GetRMS() << endl;
  //TLatex latex;
  //latex.DrawLatexNDC(0.2, pos, Form("Dispersion: %.2f TeV^{-1}", z0Dispersion->GetRMS()));
}


void setBinLabel( TH1 *frame ) {
  frame->GetXaxis()->SetBinLabel(1, "Initial");
  frame->GetXaxis()->SetBinLabel(2, "Iter0 (L2)");
  frame->GetXaxis()->SetBinLabel(3, "Iter1 (L2)");
  frame->GetXaxis()->SetBinLabel(4, "Iter2 (L2)");
  frame->GetXaxis()->SetBinLabel(5, "Iter3 (L3)");
}

void dispersion() {
  SetAtlasStyle();

  //
  new TCanvas("c_sagitta", "sagitta", 0, 0, 800, 600);
  core_sagitta("preTSI_Initial.preTSI_initialRefit2_SiOnly.root", "Initial", kGray+1, 20, 0.8);
  core_sagitta("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter0.preTS1_Iter0Refit2_SiOnly.root", "Iter0 (L2)", kRed-6, 22, 0.9);
  core_sagitta("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter1.preTS1_Iter1Refit2_SiOnly.root", "Iter1 (L2)", kBlue+2, 24, 0.9);
  core_sagitta("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter2.preTS1_Iter2Refit2_SiOnly.root", "Iter2 (L2)", kGreen+3, 21, 0.9);
  core_sagitta("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter3.preTS1_Iter3Refit2_SiOnly.root", "Iter3 (L3)", kOrange+1, 25, 0.9);

  lgd_sagitta->Draw();

  gPad->RedrawAxis();
  gPad->SaveAs("disp_sagitta.pdf");

  //
  new TCanvas("c_d0", "d0", 0, 0, 800, 600);
  core_d0("preTSI_Initial.preTSI_initialRefit2_SiOnly.root", "Initial", kGray+1, 20, 0.8);
  core_d0("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter0.preTS1_Iter0Refit2_SiOnly.root", "Iter0 (L2)", kRed-6, 22, 0.9);
  core_d0("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter1.preTS1_Iter1Refit2_SiOnly.root", "Iter1 (L2)", kBlue+2, 24, 0.9);
  core_d0("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter2.preTS1_Iter2Refit2_SiOnly.root", "Iter2 (L2)", kGreen+3, 21, 0.9);
  core_d0("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter3.preTS1_Iter3Refit2_SiOnly.root", "Iter3 (L3)", kOrange+1, 25, 0.9);

  lgd_d0->Draw();
  
  gPad->RedrawAxis();
  gPad->SaveAs("disp_d0.pdf");
  
  //
  new TCanvas("c_z0", "z0", 0, 0, 800, 600);
  core_z0("preTSI_Initial.preTSI_initialRefit2_SiOnly.root", "Initial", kGray+1, 20, 0.8);
  core_z0("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter0.preTS1_Iter0Refit2_SiOnly.root", "Iter0 (L2)", kRed-6, 22, 0.9);
  core_z0("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter1.preTS1_Iter1Refit2_SiOnly.root", "Iter1 (L2)", kBlue+2, 24, 0.9);
  core_z0("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter2.preTS1_Iter2Refit2_SiOnly.root", "Iter2 (L2)", kGreen+3, 21, 0.9);
  core_z0("correctionmaps_Refit1_SiAndTRTZmumuValidationExampleOutput.Data.preTS1_Iter3.preTS1_Iter3Refit2_SiOnly.root", "Iter3 (L3)", kOrange+1, 25, 0.9);

  lgd_z0->Draw();
  
  gPad->RedrawAxis();
  gPad->SaveAs("disp_d0.pdf");

  TLegend *lgd = new TLegend(0.75, 0.70, 0.9, 0.85);
  lgd->SetBorderSize(0);
  lgd->SetFillStyle(0);
  lgd->SetTextFont(42);
  lgd->AddEntry(gr_iter_sagitta, "Average", "p");
  lgd->AddEntry(gr_iter_sagitta, "RMS", "f");
  lgd->AddEntry(gr_iter_max_sagitta, "MinMax", "f");

  TLatex latex;
  latex.SetTextSize(0.04);
  latex.SetTextAlign(32);

  TLine *line0 = new TLine(-0.5, 0.0, n_iter-0.5, 0.0);
  line0->SetLineWidth(1);
  line0->SetLineStyle(3);

  new TCanvas("c_iter_sagitta", "Iteration Sagitta", 0, 0, 800, 600);
  TH1 *frame_sagitta = new TH1F("frame_sagitta", ";Iteration;#deltaS [TeV^{-1}]", n_iter, -0.5, n_iter-0.5);
  setBinLabel( frame_sagitta );
  frame_sagitta->GetYaxis()->SetRangeUser(-4.0, 4.0);
  frame_sagitta->SetLineWidth(0);
  frame_sagitta->Draw();
  gr_iter_max_sagitta->SetFillColor(kGreen);
  gr_iter_max_sagitta->SetFillStyle(1001);
  gr_iter_max_sagitta->Draw("pe2+");
  gr_iter_sagitta->SetFillColor(kYellow);
  gr_iter_sagitta->SetFillStyle(1001);
  gr_iter_sagitta->Draw("pe2+");
  lgd->Draw();
  latex.DrawLatexNDC(0.92, 0.88, "Data 2016 pre-TS1");
  gPad->RedrawAxis();
  line0->Draw();
  gPad->SaveAs("iter_sagitta.pdf");
  
  new TCanvas("c_iter_d0", "Iteration D0", 0, 0, 800, 600);
  TH1 *frame_d0 = new TH1F("frame_d0", ";Iteration;#deltad_{0} [#mum]", n_iter, -0.5, n_iter-0.5);
  setBinLabel( frame_d0 );
  frame_d0->GetYaxis()->SetRangeUser(-15.0, 15.0);
  frame_d0->Draw();
  gr_iter_max_d0->SetFillColor(kGreen);
  gr_iter_max_d0->SetFillStyle(1001);
  gr_iter_max_d0->Draw("pe2+");
  gr_iter_d0->SetFillColor(kYellow);
  gr_iter_d0->SetFillStyle(1001);
  gr_iter_d0->Draw("pe2+");
  lgd->Draw();
  latex.DrawLatexNDC(0.92, 0.88, "Data 2016 pre-TS1");
  gPad->RedrawAxis();
  line0->Draw();
  gPad->SaveAs("iter_d0.pdf");

  new TCanvas("c_iter_z0", "Iteration Z0", 0, 0, 800, 600);
  TH1 *frame_z0 = new TH1F("frame_z0", ";Iteration;#deltaz_{0} [#mum]", n_iter, -0.5, n_iter-0.5);
  setBinLabel( frame_z0 );
  frame_z0->GetYaxis()->SetRangeUser(-300.0, 300.0);
  frame_z0->Draw();
  gr_iter_max_z0->SetFillColor(kGreen);
  gr_iter_max_z0->SetFillStyle(1001);
  gr_iter_max_z0->Draw("pe2+");
  gr_iter_z0->SetFillColor(kYellow);
  gr_iter_z0->SetFillStyle(1001);
  gr_iter_z0->Draw("pe2+");
  lgd->Draw();
  latex.DrawLatexNDC(0.92, 0.88, "Data 2016 pre-TS1");
  gPad->RedrawAxis();
  line0->Draw();
  gPad->SaveAs("iter_z0.pdf");

}

