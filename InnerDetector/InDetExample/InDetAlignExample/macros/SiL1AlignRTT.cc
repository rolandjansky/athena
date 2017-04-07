/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// do not rename the next method, RTT will stop working
void SiL1AlignRTT()
{
  gROOT->SetBatch(kTRUE);
  makePlot("Iter0/Iter0_monitor.root", "Iter0_plots");
  makePlot("Iter1/Iter1_monitor.root", "Iter1_plots");
}

void makePlot(Char_t filename[300], TString psname){
  TFile *file;
  TTree *GlobalChi2;
  TH1F *hSCTRes[2];
  TH1F *hPIXPhiRes[2];
  TH1F *hPIXEtaRes[2];

  TCanvas *cRes;
  Char_t nameh[300];
  Char_t titolh[300];
  Int_t font=42;
  Int_t EventNumber;
  Int_t NTracks;
  Int_t VnHits[2000];
  Float_t VRes_RPhi[2000][200];
  Float_t VRes_eta[2000][200];
  Int_t VUsedTrack[2000];
  Int_t VUsedHit[2000][200];
  Int_t VHit_DetType[2000][200];
  
  Int_t SCTHits = 0;
  Int_t PixelHits = 0;
  Int_t SCTUsedHits = 0;
  Int_t PixelUsedHits = 0;
  Int_t TotalTracks = 0;
  Int_t TotalUsedTracks = 0;
  
  Bool_t Debug = false;
  
  file = TFile::Open(filename);
  GlobalChi2 = (TTree*)(file->Get("GlobalChi2/General"));
 
  gStyle->SetOptStat(111110);
  gStyle->SetCanvasColor(kWhite);  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  
  GlobalChi2->SetBranchAddress("event",         &EventNumber);
  GlobalChi2->SetBranchAddress("nTracks",       &NTracks);
  GlobalChi2->SetBranchAddress("nhits",         &VnHits);
  GlobalChi2->SetBranchAddress("TrkRes",        &VRes_RPhi);
  GlobalChi2->SetBranchAddress("TrkEtaRes",     &VRes_eta); // only for PIX
  GlobalChi2->SetBranchAddress("usedHit",       &VUsedHit);
  GlobalChi2->SetBranchAddress("usedTrk",       &VUsedTrack);
  GlobalChi2->SetBranchAddress("Hit_DetType",   &VHit_DetType);
  
  int entries = GlobalChi2->GetEntries();
  cout << entries << " events found" << endl;
  for(int level = 0; level<2; level++){
    sprintf(titolh,"SCT Residuals (Barrel+Endcaps)");
    sprintf(nameh,"hSCTRes_%d",level);
  
    hSCTRes[level] = new TH1F(nameh, titolh, 
		       60,-0.2, 0.2);
    hSCTRes[level]->GetXaxis()->SetTitle("SCT #phi Residuals (mm)");
    hSCTRes[level]->SetLineColor(1);
    hSCTRes[level]->SetFillColor(18);


    sprintf(nameh,"hPIXPhiRes_%d",level);
    sprintf(titolh,"Pixel phi Residuals (Barrel+Endcaps)");
    hPIXPhiRes[level] = new TH1F(nameh, titolh, 
			  60 ,-0.20, 0.2);
    hPIXPhiRes[level]->GetXaxis()->SetTitle("Pixel #phi Residuals (mm)");
    hPIXPhiRes[level]->SetLineColor(1);
    hPIXPhiRes[level]->SetFillColor(18);
  
    sprintf(nameh,"hPIXEtaRes_%d",level);
    sprintf(titolh,"Pixel eta Residuals (Barrel+Endcaps)");
 
    hPIXEtaRes[level] = new TH1F(nameh, titolh, 
			  60 ,-0.20, 0.2);
    hPIXEtaRes[level]->GetXaxis()->SetTitle("Pixel #eta Residuals (mm)");
    hPIXEtaRes[level]->SetLineColor(1);
    hPIXEtaRes[level]->SetFillColor(18);
  }

  
  GlobalChi2->GetEntry(0);
  //cout << "hits " << VnHits[0] << endl;
  
  for(int e=0; e<entries; e++){
    GlobalChi2->GetEntry(e);
    if(Debug){
      cout << "Event " << e << endl;
      cout << "|- " << NTracks << " tracks" << endl;
    }
    for(int track=0; track < NTracks; track++){
      TotalTracks++;
      if(VUsedTrack[track] == 1)
	TotalUsedTracks++;
      if(Debug){
	cout << "|- Track "<< track << endl;
	cout << "   |- " << VnHits[track] << " hits" << endl;
      }
      for(int h=0; h<VnHits[track] ; h++){
	if(Debug){
	  cout << "   |- Hit "<< h << " ,Detector: " << VHit_DetType[track][h] << endl;
	  cout << "   |- used: " << VUsedHit[track][h] << endl;
	}
	if(VHit_DetType[track][h]==2){
	  SCTHits++;
	  hSCTRes[0]->Fill(VRes_RPhi[track][h]);
	  if(VUsedHit[track][h]==1 && VUsedTrack[track] == 1){
	    hSCTRes[1]->Fill(VRes_RPhi[track][h]);
	    SCTUsedHits++;
	  }
	}

	if(VHit_DetType[track][h]==1){
	  PixelHits++;
	  hPIXPhiRes[0]->Fill(VRes_RPhi[track][h]);
	  hPIXEtaRes[0]->Fill(VRes_eta[track][h]);
	  if(VUsedHit[track][h]==1 && VUsedTrack[track] == 1){
	    PixelUsedHits++;
	    hPIXPhiRes[1]->Fill(VRes_RPhi[track][h]);
	    hPIXEtaRes[1]->Fill(VRes_eta[track][h]);
	  }
	}
      }
      
    }
  }
  

  cRes = new TCanvas("GlobalResiduals","Global Residuals",1000,800);
  cRes->Divide(2,2);
   

  // SUMMARY
  cRes->cd(1);
  TLine *TLineBorder;
  TLineBorder = new TLine(0.02,0.98,0.98,0.98);
  TLineBorder->SetLineColor(kRed);
  TLineBorder->Draw();
  TLineBorder = new TLine(0.02,0.02,0.98,0.02);
  TLineBorder->SetLineColor(kRed);
  TLineBorder->Draw();
  TLineBorder = new TLine(0.02,0.98,0.02,0.02);
  TLineBorder->SetLineColor(kRed);
  TLineBorder->Draw();
  TLineBorder = new TLine(0.98,0.98,0.98,0.02);
  TLineBorder->SetLineColor(kRed);
  TLineBorder->Draw();
  
  int iConsts = 0;
  
  TLatex *t = new TLatex();
  t->SetNDC();
  t->SetTextFont(font);
  t->SetTextSize(0.05);
  t->SetTextAlign(12);

  
  // Information
  t->SetTextSize(0.035);
  sprintf(nameh,"Events: %d",entries);
  t->DrawLatex(0.08,0.78,nameh);
  sprintf(nameh,"AlignTrks: %d",TotalTracks);
  t->DrawLatex(0.08,0.74,nameh);
  sprintf(nameh," - Total hits: %d [ Pixel hits: %d, SCT hits: %d ]",PixelHits+SCTHits,PixelHits,SCTHits);
  t->DrawLatex(0.08,0.70,nameh);
  cRes->Update();
  sprintf(nameh,"Processed AlignTrks: %d",TotalUsedTracks);
  t->DrawLatex(0.08,0.66,nameh);
  sprintf(nameh," - Processed hits: %d [ Pixel hits: %d, SCT hits: %d ]",PixelUsedHits+SCTUsedHits,PixelUsedHits,SCTUsedHits);
  t->DrawLatex(0.08,0.62,nameh);
  cRes->Update();
  
  cRes->cd(2);
  hSCTRes[0]->SetLineColor(kGray+2);
  hSCTRes[0]->SetLineWidth(1.5);
  hSCTRes[0]->SetFillColor(kGray);
  hSCTRes[0]->SetFillStyle(3001);
  hSCTRes[0]->Draw();
  
  hSCTRes[1]->SetLineColor(kRed+2);
  hSCTRes[1]->SetLineWidth(1.5);
  hSCTRes[1]->SetFillColor(kRed);
  hSCTRes[1]->SetFillStyle(3001);
  hSCTRes[1]->Draw("SAME");
  
  cRes->cd(3);
  hPIXPhiRes[0]->SetLineColor(kGray+2);
  hPIXPhiRes[0]->SetLineWidth(1.5);
  hPIXPhiRes[0]->SetFillColor(kGray);
  hPIXPhiRes[0]->SetFillStyle(3001);
  hPIXPhiRes[0]->Draw();

  hPIXPhiRes[1]->SetLineColor(kRed+2);
  hPIXPhiRes[1]->SetLineWidth(1.5);
  hPIXPhiRes[1]->SetFillColor(kRed);
  hPIXPhiRes[1]->SetFillStyle(3001);
  hPIXPhiRes[1]->Draw("SAME");

  cRes->cd(4);
  hPIXEtaRes[0]->SetLineColor(kGray+2);
  hPIXEtaRes[0]->SetLineWidth(1.5);
  hPIXEtaRes[0]->SetFillColor(kGray);
  hPIXEtaRes[0]->SetFillStyle(3001);
  hPIXEtaRes[0]->Draw();
   
  hPIXEtaRes[1]->SetLineColor(kRed+2);
  hPIXEtaRes[1]->SetLineWidth(1.5);
  hPIXEtaRes[1]->SetFillColor(kRed);
  hPIXEtaRes[1]->SetFillStyle(3001);
  hPIXEtaRes[1]->Draw("SAME");
  cRes->Print(psname+".eps");
  cRes->Print(psname+".png");
  
  delete cRes;
   
}
