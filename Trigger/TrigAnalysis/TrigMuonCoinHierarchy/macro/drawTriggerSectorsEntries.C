/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>

void drawTriggerSectorsEntries() {
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadTopMargin(0.17);
  //  gStyle->SetPadGridY(1);
  gStyle->SetPadTickY(1);
  gStyle->SetOptTitle(1);
  gStyle->SetTitleH(0.15);
  gStyle->SetTitleY(0.99);
  gStyle->SetTitleW(0.80);
  gStyle->SetTitleX(0.10);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  TCanvas *c = new TCanvas("c", "c", 800, 800);
  c->Divide(2, 5);

  TFile *fin = new TFile("TgcCoinHierarchyTestAlg.root");

  TH1F *hd[6];
  TH1F *hd_[10];
  char *hd_name[6] = {"hnSL", "hnSLWithPT2456", "hnHiPtWire", "hnHiPtStrip", "hnTrackletWire", "hnTrackletStrip"};

  TH1F *hn[10];
  int dnum[10] = {0, 0, 1, 1, 2, 3, 2, 3, 4, 5};
  std::string hn_name[10] = {"hnSLWithTrackletWire", "hnSLWithTrackletStrip",
			     "hnSLWithPT2456WithHiPtWire", "hnSLWithPT2456WithHiPtStrip", 
			     "hnHiPtWireWithTrackletWire", "hnHiPtStripWithTrackletStrip",
			     "hnHiPtWireWithTGC1WireHit", "hnHiPtStripWithTGC1StripHit", 
			     "hnTrackletWireWithTGC23WireHit", "hnTrackletStripWithTGC23StripHit"
  }; 
  char *hn_title[10] = {"SL - Tracklet Wire", "SL - Tracklet Strip", 
			"SL (PT=2, 4, 5 or 6) - HiPt Wire", "SL (PT=2, 4, 5 or 6) - HiPt Strip", 
			"HiPt Wire - Tracklet Wire", "HiPt Strip - Tracklet Strip",
			"HiPt Wire - TGC1 Hits Wire", "HiPt Strip - TGC1 Hit Strip", 
			"Tracklet Wire - TGC2 and 3 Hits Wire", "Tracklet Strip - TGC2 and 3 Hits Strip"
  };

  for(int i=0; i<6; i++) {
    hd[i] = (TH1F*)fin->Get(hd_name[i]);
  }

  for(int i=0; i<10; i++) {
    hn[i] = (TH1F*)fin->Get(hn_name[i].c_str());
  }

  double nSL = hd[0]->GetEntries();
  double nSLWithTrackletWire = hn[0]->GetEntries();
  double nSLWithTrackletStrip = hn[1]->GetEntries();
  double nSLWithPT2456 = hd[1]->GetEntries();
  double nSLWithPT2456WithHiPtWire = hn[2]->GetEntries();
  double nSLWithPT2456WithHiPtStrip = hn[3]->GetEntries();
  double nHiPtWire = hd[2]->GetEntries();
  double nHiPtWireWithTrackletWire = hn[4]->GetEntries();
  double nHiPtWireWithTGC1WireHit = hn[6]->GetEntries();
  double nHiPtStrip = hd[3]->GetEntries();
  double nHiPtStripWithTrackletStrip = hn[5]->GetEntries();
  double nHiPtStripWithTGC1StripHit = hn[7]->GetEntries();
  double nTrackletWire = hd[4]->GetEntries();
  double nTrackletWireWithTGC23WireHit = hn[8]->GetEntries();
  double nTrackletStrip = hd[5]->GetEntries();
  double nTrackletStripWithTGC23StripHit = hn[9]->GetEntries();
  printf("*********************************************************************\n");
  if(nSL>0.1) {
    printf("SL->Tracklet Wire                     : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nSLWithTrackletWire, nSL, 100.*nSLWithTrackletWire/nSL);
    printf("SL->Tracklet Strip                    : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nSLWithTrackletStrip, nSL, 100.*nSLWithTrackletStrip/nSL);
  }
  if(nSLWithPT2456>0.1) {
    printf("SL (PT=2, 4, 5 or 6)->HiPt Wire       : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nSLWithPT2456WithHiPtWire, nSLWithPT2456, 100.*nSLWithPT2456WithHiPtWire/nSLWithPT2456);
    printf("SL (PT=2, 4, 5 or 6)->HiPt Strip      : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nSLWithPT2456WithHiPtStrip, nSLWithPT2456, 100.*nSLWithPT2456WithHiPtStrip/nSLWithPT2456);
  }
  if(nHiPtWire>0.1) {
    printf("HiPt Wire->Tracklet Wire              : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nHiPtWireWithTrackletWire, nHiPtWire, 100.*nHiPtWireWithTrackletWire/nHiPtWire);
    printf("HiPt Wire->TGC1 Wire Hits             : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nHiPtWireWithTGC1WireHit, nHiPtWire, 100.*nHiPtWireWithTGC1WireHit/nHiPtWire);
  }
  if(nHiPtStrip>0.1) {
    printf("HiPt Strip->Tracklet Strip            : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nHiPtStripWithTrackletStrip, nHiPtStrip, 100.*nHiPtStripWithTrackletStrip/nHiPtStrip);
    printf("HiPt Strip->TGC1 Strip Hits           : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nHiPtStripWithTGC1StripHit, nHiPtStrip, 100.*nHiPtStripWithTGC1StripHit/nHiPtStrip);
  }
  if(nTrackletWire>0.1) {
    printf("Tracklet Wire->TGC2 and 3 Wire Hits   : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nTrackletWireWithTGC23WireHit, nTrackletWire, 100.*nTrackletWireWithTGC23WireHit/nTrackletWire);
  }
  if(nTrackletStrip>0.1) {
    printf("Tracklet Strip->TGC2 and 3 Strip Hits : %7.0lf / %7.0lf = %8.4lf\%\n", 
	   nTrackletStripWithTGC23StripHit, nTrackletStrip, 100.*nTrackletStripWithTGC23StripHit/nTrackletStrip);
  }
  printf("*********************************************************************\n");

  TText *txt = new TText();
  txt->SetTextSize(0.1);
  txt->SetTextAlign(22);

  for(int i=0; i<10; i++) {
    TPad *pad = (TPad*)c->cd(i+1);
    pad->SetLogy();
    hd_[i] = (TH1F*)hd[dnum[i]]->Clone();
    std::string tmp = hn_name[i]+"_";
    hd_[i]->SetName(tmp.c_str());

    hd_[i]->SetMaximum(sqrt(10.)*hd_[i]->GetMaximum());
    hd_[i]->SetMinimum(sqrt(0.1));
    hd_[i]->Draw();

    hn[i]->Scale(-1.);
    hn[i]->Add(hd_[i]);
    hn[i]->Draw("same");
    hn[i]->SetLineColor(2);
    hn[i]->SetFillColor(2);
    hd_[i]->SetTitle(hn_title[i]);
    hd_[i]->GetXaxis()->SetLabelSize(0.1);
    hd_[i]->GetYaxis()->SetLabelSize(0.1);

    hd_[i]->Draw("same");
    
    TLine *line = new TLine();
    line->SetLineWidth(2);
    line->SetLineStyle(2);
    line->SetLineColor(4);
    for(int ix=2; ix<6; ix++) {
      if(ix==4) continue;
      line->DrawLine(24*ix+0.5, sqrt(0.1), 24*ix+0.5, hd_[i]->GetMaximum());
    }
  }
}
