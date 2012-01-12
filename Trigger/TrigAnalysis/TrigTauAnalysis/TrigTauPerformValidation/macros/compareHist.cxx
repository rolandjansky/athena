/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int compareHist()
{
TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","",10,10,1000,500);

  TString plotfile("effic");

  //int i = 1;
  //int i = 0;
  int i = 4;

  compareHist("EmEnergy_1_external_eff_a",i);  c1->Print(plotfile + ".ps(");
  compareHist("TauEnergy_2_external_eff_a",i);  c1->Print(plotfile + ".ps"); 
  compareHist("EmIsol_3_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("HadIsol_4_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("EmCore_5_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("HadCore_6_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("et_1_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("etCalib_2_external_eff_a",i);  c1->Print(plotfile + ".ps");  
  compareHist("nStrips_3_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("stripWidth_4_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("emrad_5_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("isoFrac_6_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("EmEnWidth_7_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("ntrkExist_1_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("ntrk_2_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("leadPt_3_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("nSlow_4_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("nIso_5_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("nMatched_6_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("Q_7_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("sumPtCore_8_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("Et_trkEM_9_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("etCalibEF_1_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("ntrkMinEF_2_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("ntrkMaxEF_3_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("emradEF_4_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("isoFracEF_5_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("emFracEF_6_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("leadPtEF_7_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("emEtEF_8_external_eff_a",i);  c1->Print(plotfile + ".ps");
  compareHist("massEF_9_external_eff_a",i);  c1->Print(plotfile + ".ps)");
  

}



int compareHist(const char * name, int select )
{
 gStyle->SetPadGridX(true);
 gStyle->SetPadGridY(true);
  
  TFile * file1, * file2, *file3;
  if( select == 0 ) {
  file1 = new TFile("align/Et_efficiencies_devval_May28_NONE.root");
  file2 = new TFile("align/Et_efficiencies_TTP06_dR03_NONE.root");
  file3 = new TFile("align/Et_efficiencies_TTP07_NONE.root");
  }
  elseif ( select == 1 ) {
  file1 = new TFile("align/Eta_efficiencies_devval_May28_NONE.root");
  file2 = new TFile("align/Eta_efficiencies_TTP06_dR03_NONE.root");
  file3 = new TFile("align/Eta_efficiencies_TTP07_NONE.root");
  }
  elseif ( select == 2 ) {
  cout << "Print out Phi histograms" << endl;
  file1 = new TFile("align/Phi_efficiencies_devval_May28_NONE.root");
  file2 = new TFile("align/Phi_efficiencies_TTP06_dR03_NONE.root");
  file3 = new TFile("align/Phi_efficiencies_TTP07_NONE.root");
  }
  elseif ( select == 3 ) {
  file1 = new TFile("align/Eta_efficiencies_devval_May28_NONE.root");
  file2 = new TFile("align/Eta_efficiencies_devval_May28_5862_NONE.root");
  file3 = new TFile("align/Eta_efficiencies_TTP07_NONE.root");
  }

  elseif ( select == 4 ) {
  file1 = new TFile("align/Eta_efficiencies_devval_May28_NONE.root");
  file2 = new TFile("align/Eta_efficiencies_devval_May28_5862_NONE.root");
  file3 = new TFile("align/Eta_5107_over40.root");
  }


  gROOT->cd();
  
  TH1* h1 = (TH1*)gROOT->Get("h1");
  if( h1 ) delete h1; 
  h1 = (TH1*)file1->Get(name)->Clone("h1");  

  TH1* h2 = (TH1*)gROOT->Get("h2");
  if( h2 ) delete h2; 
  h2 = (TH1*)file2->Get(name)->Clone("h2");

  TH1* h3 = (TH1*)gROOT->Get("h3");
  if( h3 ) delete h3; 
  h3 = (TH1*)file3->Get(name)->Clone("h3");

  if( h1 ){
  h1->SetLineColor(1);
  h1->Draw();
  }else cout << "No h1 found \n";
  if( h2 ){
  h2->SetLineColor(2);
  h2->Draw("lsame");
  }else cout << "No h2 found \n";
  
  if( h3 ){
  h3->SetLineColor(4);
  h3->Draw("lsame");
  }else cout << "No h3 found \n";
  
  TLatex * text = new TLatex();
  text->DrawTextNDC(0.5,0.3,"black - devval 5107");
  text->SetTextColor(2);
  text->DrawTextNDC(0.5,0.5,"red - devval 5862");
  text->SetTextColor(4);
  text->DrawTextNDC(0.5,0.4,"blue - devval 5107 Pt>40");

  delete file1;
  delete file2;

}
