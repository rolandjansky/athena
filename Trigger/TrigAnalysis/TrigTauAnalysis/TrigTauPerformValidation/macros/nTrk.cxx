/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

int nTrk(const char * fileName)
{
  TFile *fin=TFile::Open(fileName);
  TTree *tree=((TTree*)fin->Get("tauRoI"));

  TH1F *hnTrk=new TH1F("hnTrk","nTrk",7,-0.5,6.5);
  TH1F *hnTrkOL=new TH1F("hnTrkOL","nTrk (offline)",7,-0.5,6.5);
  TH1F *hnTrkEFID=new TH1F("hnTrkEFID","nTrkEFID",7,-0.5,6.5);
  TH1F *hnTrkMy=new TH1F("hnTrkMy","nTrkMy",7,-0.5,6.5);
  tree->Project("hnTrk","nTrk");
  tree->Project("hnTrkOL","tr_nTrk");
  tree->Project("hnTrkEFID","nTrkEFID");
  tree->Project("hnTrkMy","nTrkMy");
  hnTrk->Scale(1./hnTrk->GetEntries());
  hnTrkOL->Scale(1./hnTrkOL->GetEntries());
  hnTrkEFID->Scale(1./hnTrkEFID->GetEntries());
  hnTrkMy->Scale(1./hnTrkMy->GetEntries());
  hnTrk->SetLineWidth(2.);
  hnTrkOL->SetLineWidth(2.);
  hnTrkEFID->SetLineWidth(2.);
  hnTrkMy->SetLineWidth(2.);
  hnTrk->SetLineColor(2);
  hnTrkOL->SetLineColor(6);
  hnTrkEFID->SetLineColor(4);
  hnTrkMy->SetLineColor(3);

  TH1F *hnTrk1=new TH1F("hnTrk1","nTrk (pi)",7,-0.5,6.5);
  TH1F *hnTrkOL1=new TH1F("hnTrkOL1","nTrk (pi)",7,-0.5,6.5);
  TH1F *hnTrkEFID1=new TH1F("hnTrkEFID1","nTrkEFID (pi)",7,-0.5,6.5);
  TH1F *hnTrkMy1=new TH1F("hnTrkMy1","nTrkMy (pi)",7,-0.5,6.5);
  tree->Project("hnTrk1","nTrk","mc_decayType==4");
  tree->Project("hnTrkOL1","tr_nTrk","mc_decayType==4");
  tree->Project("hnTrkEFID1","nTrkEFID","mc_decayType==4");
  tree->Project("hnTrkMy1","nTrkMy","mc_decayType==4");
  hnTrk1->Scale(1./hnTrk1->GetEntries());
  hnTrkOL1->Scale(1./hnTrkOL1->GetEntries());
  hnTrkEFID1->Scale(1./hnTrkEFID1->GetEntries());
  hnTrkMy1->Scale(1./hnTrkMy1->GetEntries());
  hnTrk1->SetLineWidth(2.);
  hnTrkOL1->SetLineWidth(2.);
  hnTrkEFID1->SetLineWidth(2.);
  hnTrkMy1->SetLineWidth(2.);
  hnTrk1->SetLineColor(2);
  hnTrkOL1->SetLineColor(6);
  hnTrkEFID1->SetLineColor(4);
  hnTrkMy1->SetLineColor(3);

  TH1F *hnTrk3=new TH1F("hnTrk3","nTrk (3pi)",7,-0.5,6.5);
  TH1F *hnTrkOL3=new TH1F("hnTrkOL3","nTrk (3pi)",7,-0.5,6.5);
  TH1F *hnTrkEFID3=new TH1F("hnTrkEFID3","nTrkEFID (3pi)",7,-0.5,6.5);
  TH1F *hnTrkMy3=new TH1F("hnTrkMy3","nTrkMy (3pi)",7,-0.5,6.5);
  tree->Project("hnTrk3","nTrk","mc_decayType==5");
  tree->Project("hnTrkOL3","tr_nTrk","mc_decayType==5");
  tree->Project("hnTrkEFID3","nTrkEFID","mc_decayType==5");
  tree->Project("hnTrkMy3","nTrkMy","mc_decayType==5");
  hnTrk3->Scale(1./hnTrk3->GetEntries());
  hnTrkOL3->Scale(1./hnTrkOL3->GetEntries());
  hnTrkEFID3->Scale(1./hnTrkEFID3->GetEntries());
  hnTrkMy3->Scale(1./hnTrkMy3->GetEntries());
  hnTrk3->SetLineWidth(2.);
  hnTrkOL3->SetLineWidth(2.);
  hnTrkEFID3->SetLineWidth(2.);
  hnTrkMy3->SetLineWidth(2.);
  hnTrk3->SetLineColor(2);
  hnTrkOL3->SetLineColor(6);
  hnTrkEFID3->SetLineColor(4);
  hnTrkMy3->SetLineColor(3);

  TCanvas * c1 = (TCanvas *) gROOT->Get("c1");
  if( c1 ) delete c1;
  c1 = new TCanvas("c1","Canvas 1",10,10,700,700);
  c1->Divide(2,2);

  c1->cd(1);
  hnTrk->Draw();
  hnTrkOL->Draw("same");
  hnTrkEFID->Draw("same");
  hnTrkMy->Draw("same");
  TLegend* leg = new TLegend(0.5,0.50,0.89,0.69);
  leg->AddEntry(hnTrk,"nTrack","l");
  leg->AddEntry(hnTrkOL,"offline::nTrack","l");
  leg->AddEntry(hnTrkEFID,"nTrackEFID","l");
  leg->AddEntry(hnTrkMy,"nTrackMy","l");
  leg->Draw();

  c1->cd(3);
  hnTrk1->Draw();
  hnTrkOL1->Draw("same");
  hnTrkEFID1->Draw("same");
  hnTrkMy1->Draw("same");

  c1->cd(4);
  hnTrkOL3->Draw();
  hnTrkMy3->Draw("same");
  hnTrkEFID3->Draw("same");
  hnTrk3->Draw("same");

  TH1F *hnTrkS1=new TH1F("hnTrkS1","nTrk (calo)",7,-0.5,6.5);
  TH1F *hnTrkEFIDS1=new TH1F("hnTrkEFIDS1","nTrkEFID",7,-0.5,6.5);
  tree->Project("hnTrkS1","nTrk","seedType==1");
  tree->Project("hnTrkEFIDS1","nTrkEFID","seedType==1");
  //  hnTrkS1->Scale(1./hnTrkS1->GetEntries());
  //  hnTrkEFIDS1->Scale(1./hnTrkEFIDS1->GetEntries());
  hnTrkS1->SetLineWidth(2.);
  hnTrkEFIDS1->SetLineWidth(2.);
  hnTrkS1->SetLineColor(2);
  hnTrkEFIDS1->SetLineColor(2);

  TH1F *hnTrkS3=new TH1F("hnTrkS3","nTrk (calo)",7,-0.5,6.5);
  TH1F *hnTrkEFIDS3=new TH1F("hnTrkEFIDS3","nTrkEFID",7,-0.5,6.5);
  tree->Project("hnTrkS3","nTrk","seedType==3");
  tree->Project("hnTrkEFIDS3","nTrkEFID","seedType==3");
  //  hnTrkS3->Scale(1./hnTrkS3->GetEntries());
  //  hnTrkEFIDS3->Scale(1./hnTrkEFIDS3->GetEntries());
  hnTrkS3->SetLineWidth(2.);
  hnTrkEFIDS3->SetLineWidth(2.);
  hnTrkS3->SetLineColor(4);
  hnTrkEFIDS3->SetLineColor(4);

  TH2F *hnTrk2d=new TH2F("hnTrk2d","nTrkEFID vs. nTrk;nTrk;nTrkEFID",
			 7,-0.5,6.5,7,-0.5,6.5);
  tree->Project("hnTrk2d","nTrkEFID:nTrk","seedType==3");
  hnTrk2d->SetFillColor(4);

  TCanvas * c2 = (TCanvas *) gROOT->Get("c2");
  if( c2 ) delete c2;
  c2 = new TCanvas("c2","Canvas 2",30,30,700,700);
  c2->Divide(2,2);

  c2->cd(1);
  hnTrkS3->Draw();
  hnTrkS1->Draw("same");
  c2->cd(2);
  hnTrkEFIDS3->Draw();
  hnTrkEFIDS1->Draw("same");
  TLegend* leg2 = new TLegend(0.5,0.55,0.89,0.69);
  leg2->AddEntry(hnTrkS1,"seed: calo","l");
  leg2->AddEntry(hnTrkS3,"seed: calo+trk","l");
  leg2->Draw();
  c2->cd(3);
  hnTrk2d->Draw("box");
}
