/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void hitCorrelations(TString TrkCol_1,TString TrkCol_2) {
	TH2F *hPixelVSSCT;
	TH2F *hPixelVSSCT2;
	TH2F *hPixelEta;
	TH2F *hPixelEta2;
	TH2F *hSCTEta;
	TH2F *hSCTEta2;
	TString histname, histtitle;
	bool somethingwasplotted = false;
	cout << " <hitCorrelations> plotting more hits information..." << endl;

	cHitCorr = new TCanvas("TracksHits2","More Hits information",CanvasSizeX6[0],CanvasSizeX6[1]);
	cHitCorr->Divide(3,2);
	
	cHitCorr->cd(1);
	histname = "IDAlignMon/"+TrkCol_1+"/GenericTracks/PIXvSCTHits";
	if(PrintLevel>0) cout << " <hitCorrelations> Plotting Histogram: " << histname << endl;
	hPixelVSSCT = (TH2F*)file->Get(histname);
	if (hPixelVSSCT != NULL) {
	  histtitle = TrkCol_1.Data();
	  histtitle += ": Hits on track";
	  hPixelVSSCT->SetTitle(histtitle.Data());
	  hPixelVSSCT->GetXaxis()->SetTitle("SCT Hits on track");
	  hPixelVSSCT->GetYaxis()->SetTitle("Pixel Hits on track");
	  hPixelVSSCT->Draw("COLZ");
	  if (!AtlasStyle) {
	    gPad->SetGridx();
	    gPad->SetGridy();
	  }
	  gPad->SetLogz();
	  cHitCorr->Update();
	  moveTPaveStats(hPixelVSSCT);
	  setTitleFont(font);
	  ScalePlots(1);
	  if (normalizePlots) hPixelVSSCT->Scale(1./(float)iEvents);
	  somethingwasplotted = true;
	}

	//
	// -----------------------------------------------------------------
	//
	
    cHitCorr->cd(4);
    if (TrkCol_2 !="") {
        histname = "IDAlignMon/"+TrkCol_2+"/GenericTracks/PIXvSCTHits";
	    if(PrintLevel>0) cout << " <hitCorrelations> Plotting Histogram: " << histname << endl;
	    hPixelVSSCT2 = (TH2F*)file->Get(histname);
	    if (hPixelVSSCT2 != NULL) {
          histtitle = TrkCol_2.Data();
	      histtitle += ": Hits on track";
	      hPixelVSSCT2->SetTitle(histtitle.Data());
	      hPixelVSSCT2->GetXaxis()->SetTitle("SCT Hits on track");
	      hPixelVSSCT2->GetYaxis()->SetTitle("Pixel Hits on track");
	      hPixelVSSCT2 ->Draw("COLZ");
	      if (!AtlasStyle) {
	        gPad->SetGridx();
	        gPad->SetGridy();
	      }
	      gPad->SetLogz();
	      cHitCorr->Update();
	      moveTPaveStats(hPixelVSSCT2);
	      setTitleFont(font);
	      ScalePlots(1);
	      if (normalizePlots) hPixelVSSCT2->Scale(1./(float)iEvents);
	      somethingwasplotted = true;
	    }
    }

	//
	// -----------------------------------------------------------------
	//

	cHitCorr->cd(2);
	histname = "IDAlignMon/"+TrkCol_1+"/GenericTracks/PIXHitsvEta";
	if(PrintLevel>0) cout << " <hitCorrelations> Plotting Histogram: " << histname << endl;
	hPixelEta = (TH2F*)file->Get(histname);
	if (hPixelEta != NULL) {
	  histtitle = TrkCol_1.Data();
	  histtitle += ": Pixel hits vs Track #eta";
	  hPixelEta->SetTitle(histtitle.Data());
	  hPixelEta->GetXaxis()->SetTitle("#eta");
	  hPixelEta->GetYaxis()->SetTitle("Pixel Hits on track");
	  hPixelEta->Draw("COLZ");
	  if (!AtlasStyle) {
	    gPad->SetGridx();
	    gPad->SetGridy();
	  }
	  gPad->SetLogz();
	  cHitCorr->Update();
	  moveTPaveStats(hPixelEta);
	  setTitleFont(font);
	  ScalePlots(1);
	  if (normalizePlots) hPixelEta->Scale(1./(float)iEvents);
	  somethingwasplotted = true;
	}

	//
	// -----------------------------------------------------------------
	//
	
    cHitCorr->cd(5);
    if (TrkCol_2 !="") {
	   histname = "IDAlignMon/"+TrkCol_2+"/GenericTracks/PIXHitsvEta";
	   if(PrintLevel>0) cout << " <hitsCorrelation> Plotting Histogram: " << histname << endl;
	   hPixelEta2 = (TH2F*)file->Get(histname);
	   if (hPixelEta2 != NULL) {
	     histtitle = TrkCol_2.Data();
         histtitle += ": Pixel hits vs Track #eta";
         hPixelEta2->SetTitle(histtitle.Data());
	     hPixelEta2->GetXaxis()->SetTitle("#eta");
         hPixelEta2->GetYaxis()->SetTitle("Pixel Hits on track");
	     hPixelEta2->Draw("COLZ");
	     if (!AtlasStyle) {
	       gPad->SetGridx();
	       gPad->SetGridy();
	     }
	     gPad->SetLogz();
	     cHitCorr->Update();
	     moveTPaveStats(hPixelEta2);
	     setTitleFont(font);
	     ScalePlots(1);
	     if (normalizePlots) hPixelEta2->Scale(1./(float)iEvents);
           somethingwasplotted = true;
	   }
    }

	cHitCorr->cd(3);
	histname = "IDAlignMon/"+TrkCol_1+"/GenericTracks/SCTHitsvEta";
	if(PrintLevel>0) cout << " <hitsCorrelation> Plotting Histogram: " << histname << endl;
	hSCTEta = (TH2F*)file->Get(histname);
	if (hSCTEta != NULL) {
	  histtitle = TrkCol_1.Data();
	  histtitle += ": SCT hits vs Track #eta";
	  hSCTEta->SetTitle(histtitle.Data());
	  hSCTEta->GetXaxis()->SetTitle("#eta");
	  hSCTEta->GetYaxis()->SetTitle("SCT Hits on track");
	  hSCTEta->Draw("COLZ");
	  if (!AtlasStyle) {
	    gPad->SetGridx();
	    gPad->SetGridy();
	  }
	  gPad->SetLogz();
	  cHitCorr->Update();
	  moveTPaveStats(hSCTEta);
	  setTitleFont(font);
	  ScalePlots(1);
	  if (normalizePlots) hSCTEta->Scale(1./(float)iEvents);
	  somethingwasplotted = true;
	}

	//
	// -----------------------------------------------------------------
	//

    cHitCorr->cd(6);
    if (TrkCol_2 !="") {
	  histname = "IDAlignMon/"+TrkCol_2+"/GenericTracks/SCTHitsvEta";
	  if(PrintLevel>0) cout << " <hitsCorrelation> Plotting Histogram: " << histname << endl;
	    hSCTEta2 = (TH2F*)file->Get(histname);
	    if (hSCTEta2 != NULL) {
	      histtitle = TrkCol_2.Data();
	      histtitle += ": SCT hits vs Track #eta";
	      hSCTEta2->SetTitle(histtitle.Data());
	      hSCTEta2->GetXaxis()->SetTitle("#eta");
	      hSCTEta2->GetYaxis()->SetTitle("SCT Hits on track");
	      hSCTEta2->Draw("COLZ");
	      if (!AtlasStyle) {
	        gPad->SetGridx();
	        gPad->SetGridy();
	      }
	      gPad->SetLogz();
	      cHitCorr->Update();
	      moveTPaveStats(hSCTEta2);
	      setTitleFont(font);
	      ScalePlots(1);
	      if (normalizePlots) hSCTEta2->Scale(1./(float)iEvents);
	        somethingwasplotted = true;
	      }
    }

	// if no histogram was found draw a blank
	if (!somethingwasplotted) {
	  TH1F* hBlank = new TH1F ("hBlank"," ",1, 0., 1.);
	  cHitCorr->cd(1);
	  hBlank->SetMaximum(1.);
	  hBlank->SetMinimum(-1.);
	  hBlank->Draw();
	  TText myWarning;
	  myWarning.DrawText(0., 0., " hitsCorrelation.cc NOTHING DRAWN. PLEASE CHECK ");
	  
	}
	
	return;
}
