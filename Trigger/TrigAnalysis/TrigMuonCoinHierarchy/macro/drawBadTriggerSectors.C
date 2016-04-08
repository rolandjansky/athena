/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void drawBadTriggerSectors() {
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadTopMargin(0.17);
  gStyle->SetPadGridY(1);
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
  char *hd_name[6] = {"hnSL", "hnSLWithPT2456", "hnHiPtWire", "hnHiPtStrip", "hnTrackletWire", "hnTrackletStrip"};

  TH1F *hn[10];
  int dnum[10] = {0, 0, 1, 1, 2, 3, 2, 3, 4, 5};
  char *hn_name[10] = {"hnSLWithTrackletWire", "hnSLWithTrackletStrip",
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

  TText *txt = new TText();
  txt->SetTextSize(0.1);
  txt->SetTextAlign(22);

  for(int i=0; i<10; i++) {
    TPad *pad = (TPad*)c->cd(i+1);
    pad->SetLogy();
    hn[i] = (TH1F*)fin->Get(hn_name[i]);
    hn[i]->Scale(-1.);
    hn[i]->Add(hd[dnum[i]]);
    hn[i]->Divide(hd[dnum[i]]);
    hn[i]->Draw();
    hn[i]->SetMaximum(1.0);
    hn[i]->SetMinimum(1.0E-06);
    hn[i]->SetLineColor(2);
    hn[i]->SetFillColor(2);
    hn[i]->SetTitle(hn_title[i]);
    hn[i]->GetXaxis()->SetLabelSize(0.1);
    hn[i]->GetYaxis()->SetLabelSize(0.1);
    
    cout << hn_name[i] << endl;
    for(int ix=1; ix<=144; ix++) {
      if(hn[i]->GetBinContent(ix)>1.0E-12) {
	printf("%3d %1s %02d %1s phi%1d %7.3f\%\n", 
	       ix, 
	       (ix<=72 ? "A" : "C"), 
	       ((ix%72<=48) ? (((ix%72+1)%48)/4+1) : ((ix%24)/2+1)), 
	       ((ix%72<=48) ? "E" : "F"), 
	       ((ix%72<=48) ? (((ix%72+1)%48)%4) : (((ix%24)%2)*2)), 
	       hn[i]->GetBinContent(ix)*100.);
      }
    }

    TLine *line = new TLine();
    line->SetLineWidth(2);
    line->SetLineStyle(2);
    line->SetLineColor(4);
    for(int ix=2; ix<6; ix++) {
      if(ix==4) continue;
      line->DrawLine(24*ix+0.5, 1.0E-06, 24*ix+0.5, 1.0);
    }
  }

}
