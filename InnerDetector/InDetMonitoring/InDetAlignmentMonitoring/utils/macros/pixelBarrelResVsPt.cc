/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixelBarrelResidualsVsPt(TString trkCol){
	pixelBarrelResidualsVsPt(trkCol, 0, "");
}

void pixelBarrelResidualsVsPt(TString trkCol, TString trkCol2){
	pixelBarrelResidualsVsPt(trkCol, 0, trkCol2);
}

void pixelBarrelResidualsVsPt(TString trkCol, TCanvas *cPIXBarrelResVsPt){
	pixelBarrelResidualsVsPt(trkCol, cPIXBarrelResVsPt, "");
}

void pixelBarrelResidualsVsPt(TString trkCol, TCanvas *cPIXBarrelResVsPt, TString trkCol2) {
	TProfile *hPIXPhiBarrelResVsPt[3];
	TProfile *hPIXEtaBarrelResVsPt[3];
	
	TProfile *hPIXPhiBarrelResVsPt_2[3];
	TProfile *hPIXEtaBarrelResVsPt_2[3];
	
	float ProfEtaMax = 0;
	float max,min;
   	float ProfPhiMax = 0;
   	
	TH2F *tempHist;
	TString histname, histtitle;
	Color_t Color;
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " <pixelBarrelResVsPt> Residuals..." << endl;
	if (!cPIXBarrelResVsPt){
		cPIXBarrelResVsPt = new TCanvas("cPIXBarrelResVsPt","PIX Barrel Residuals per Layer",
			      CanvasSizeX6[0],CanvasSizeX6[1]);
	}
	cPIXBarrelResVsPt->Divide(3,2);
	for(int layer=0; layer<3; layer++){
		cPIXBarrelResVsPt->cd(layer+1);
		
		histname = "IDAlignMon/"+trkCol+"/Residuals/pix_b";
		histname += layer;
		histname += "_biased_residualx_pt";
		// histname += "_unbiased_residualx_pt";
		if(PrintLevel>0) cout << " <pixelBarrelResVsPt> Plotting Histogram: " << histname << endl;
		tempHist = (TH2F*)file->Get(histname);

		if (tempHist == NULL) { // check if histogram exists
		  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
		  return;
		}

		hPIXPhiBarrelResVsPt[layer] = tempHist->ProfileX();	
		histtitle.Clear();
		histtitle = trkCol.Data();
		histtitle += ": Pixel barrel L";
		histtitle += layer;
		histtitle += " Local x residuals Vs p_{T}";
		hPIXPhiBarrelResVsPt[layer]->SetTitle(histtitle.Data());
		hPIXPhiBarrelResVsPt[layer]->GetXaxis()->SetTitle("Track qp_{T} [GeV]");
		hPIXPhiBarrelResVsPt[layer]->GetYaxis()->SetTitle("Local x residuals [mm]");
		hPIXPhiBarrelResVsPt[layer]->SetLineColor(Color+2);
		hPIXPhiBarrelResVsPt[layer]->SetFillColor(Color);
		hPIXPhiBarrelResVsPt[layer]->SetLineWidth(1);
		hPIXPhiBarrelResVsPt[layer]->SetFillStyle(3001);
		if (normalizePlots) hPIXPhiBarrelResVsPt[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hPIXPhiBarrelResVsPt[layer]->SetStats(false);
		hPIXPhiBarrelResVsPt[layer]->DrawCopy();
		max = fabs(hPIXPhiBarrelResVsPt[layer]->GetMaximum());
		min = fabs(hPIXPhiBarrelResVsPt[layer]->GetMinimum());
		if(ProfPhiMax<max) ProfPhiMax = max;
		if(ProfPhiMax<min) ProfPhiMax = min;
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_b";
			histname += layer;
			histname += "_biased_residualx_pt";
			// histname += "_residualx_pt";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			tempHist = (TH2F*)file->Get(histname);
		    hPIXPhiBarrelResVsPt_2[layer] = tempHist->ProfileX();	
			hPIXPhiBarrelResVsPt_2[layer]->SetLineColor(ColorForSecond+2);
			hPIXPhiBarrelResVsPt_2[layer]->SetFillColor(ColorForSecond);
			hPIXPhiBarrelResVsPt_2[layer]->SetLineWidth(1);
			hPIXPhiBarrelResVsPt_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hPIXPhiBarrelResVsPt_2[layer]->Scale(1./(float)iEvents);
			hPIXPhiBarrelResVsPt_2[layer]->DrawCopy("same");
			max = fabs(hPIXPhiBarrelResVsPt_2[layer]->GetMaximum());
			min = fabs(hPIXPhiBarrelResVsPt_2[layer]->GetMinimum());
			if(ProfPhiMax<max) ProfPhiMax = max;
			if(ProfPhiMax<min) ProfPhiMax = min;
		}
		
		
		cPIXBarrelResVsPt->cd(layer+4);
		
		histname = "IDAlignMon/"+trkCol+"/Residuals/pix_b";
		histname += layer;
		histname += "_biased_residualy_pt";
		//histname += "_residualy_pt";
		if(PrintLevel>0) cout << " <pixelBarrelResVsPt> Plotting Histogram: " << histname << endl;
		tempHist = (TH2F*)file->Get(histname);
		hPIXEtaBarrelResVsPt[layer] = tempHist->ProfileX();
		histtitle.Clear();
		histtitle = trkCol.Data();
		histtitle += ": Pixel barrel L";
		histtitle += layer;
		histtitle += " Local y residuals Vs p_{T}";
		hPIXEtaBarrelResVsPt[layer]->SetTitle(histtitle.Data());
		hPIXEtaBarrelResVsPt[layer]->GetXaxis()->SetTitle("Track qp_{T} [GeV]");
		hPIXEtaBarrelResVsPt[layer]->GetYaxis()->SetTitle("Local y residuals [mm]");
		hPIXEtaBarrelResVsPt[layer]->SetLineColor(Color+2);
		hPIXEtaBarrelResVsPt[layer]->SetFillColor(Color);
		hPIXEtaBarrelResVsPt[layer]->SetLineWidth(1);
		hPIXEtaBarrelResVsPt[layer]->SetFillStyle(3001);
		if (normalizePlots) hPIXEtaBarrelResVsPt[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hPIXEtaBarrelResVsPt[layer]->DrawCopy();
		max = fabs(hPIXEtaBarrelResVsPt[layer]->GetMaximum());
		min = fabs(hPIXEtaBarrelResVsPt[layer]->GetMinimum());
		if(ProfEtaMax<max) ProfEtaMax = max;
		if(ProfEtaMax<min) ProfEtaMax = min;
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_b";
			histname += layer;
			histname += "_biased_residualy_pt";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			tempHist = (TH2F*)file->Get(histname);
			hPIXEtaBarrelResVsPt_2[layer] = tempHist->ProfileX();	
			hPIXEtaBarrelResVsPt_2[layer]->SetLineColor(ColorForSecond+2);
			hPIXEtaBarrelResVsPt_2[layer]->SetFillColor(ColorForSecond);
			hPIXEtaBarrelResVsPt_2[layer]->SetLineWidth(1);
			hPIXEtaBarrelResVsPt_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hPIXEtaBarrelResVsPt_2[layer]->Scale(1./(float)iEvents);
			hPIXEtaBarrelResVsPt_2[layer]->DrawCopy("same");
			max = fabs(hPIXEtaBarrelResVsPt_2[layer]->GetMaximum());
			min = fabs(hPIXEtaBarrelResVsPt_2[layer]->GetMinimum());
			if(ProfEtaMax<max) ProfEtaMax = max;
			if(ProfEtaMax<min) ProfEtaMax = min;
		}
		
	}
	for(int layer=0; layer<3; layer++){
		hPIXPhiBarrelResVsPt[layer]->SetMaximum(ProfPhiMax*1.1);
		hPIXPhiBarrelResVsPt[layer]->SetMinimum(-ProfPhiMax*1.1);
		hPIXEtaBarrelResVsPt[layer]->SetMaximum(ProfEtaMax*1.1);
		hPIXEtaBarrelResVsPt[layer]->SetMinimum(-ProfEtaMax*1.1);

	}
}
