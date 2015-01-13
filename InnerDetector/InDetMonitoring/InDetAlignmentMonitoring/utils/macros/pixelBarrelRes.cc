/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixelBarrelResiduals(TString trkCol){
	pixelBarrelResiduals(trkCol, 0, "");
}

void pixelBarrelResiduals(TString trkCol, TString trkCol2){
	pixelBarrelResiduals(trkCol, 0, trkCol2);
}

void pixelBarrelResiduals(TString trkCol, TCanvas *cPIXBarrelRes){
	pixelBarrelResiduals(trkCol, cPIXBarrelRes, "");
}

void pixelBarrelResiduals(TString trkCol, TCanvas *cPIXBarrelRes, TString trkCol2) {
	TH1F *hPIXPhiBarrelRes[3];
	TH1F *hPIXEtaBarrelRes[3];
	
	TH1F *hPIXPhiBarrelRes_2[3];
	TH1F *hPIXEtaBarrelRes_2[3];
	
	TString histname, histtitle;
	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cPIXBarrelRes){
		cPIXBarrelRes = new TCanvas("cPIXBarrelRes","PIX Barrel Residuals per Layer",
			      CanvasSizeX6[0],CanvasSizeX6[1]);
	}
	cPIXBarrelRes->Divide(3,2);
	for(int layer=0; layer<3; layer++){
		cPIXBarrelRes->cd(layer+1);
		
		histname = "IDAlignMon/"+trkCol+"/Residuals/pix_b";
		histname += layer;
		histname += "_biased_residualx";
		if(PrintLevel>0) cout << " <pixelBarrelRes> Plotting Histogram: " << histname << endl;
		hPIXPhiBarrelRes[layer] = (TH1F*)file->Get(histname);	

		if (hPIXPhiBarrelRes[layer] == NULL) { // check if histogram exists
		  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
		  return;
		}

		histtitle.Clear();
		histtitle += trkCol.Data();
		histtitle += ": Pixel Barrel L";
		histtitle += layer;
		histtitle += " local x residuals";
		hPIXPhiBarrelRes[layer]->SetTitle(histtitle.Data());
		hPIXPhiBarrelRes[layer]->GetXaxis()->SetTitle("Residuals [mm]");
		hPIXPhiBarrelRes[layer]->GetYaxis()->SetTitle("Entries");
		hPIXPhiBarrelRes[layer]->SetLineColor(Color+2);
		hPIXPhiBarrelRes[layer]->SetFillColor(Color);
		hPIXPhiBarrelRes[layer]->SetLineWidth(1);
		hPIXPhiBarrelRes[layer]->SetFillStyle(3001);
		if (normalizePlots) hPIXPhiBarrelRes[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hPIXPhiBarrelRes[layer]->DrawCopy("hist");
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_b";
			histname += layer;
			histname += "_biased_residualx";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			hPIXPhiBarrelRes_2[layer] = (TH1F*)file->Get(histname);	
			hPIXPhiBarrelRes_2[layer]->SetLineColor(ColorForSecond+2);
			hPIXPhiBarrelRes_2[layer]->SetFillColor(ColorForSecond);
			hPIXPhiBarrelRes_2[layer]->SetLineWidth(1);
			hPIXPhiBarrelRes_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hPIXPhiBarrelRes_2[layer]->Scale(1./(float)iEvents);
			hPIXPhiBarrelRes_2[layer]->DrawCopy("histsame");
		}
		
		
		cPIXBarrelRes->cd(layer+4);
		
		histname = "IDAlignMon/"+trkCol+"/Residuals/pix_b";
		histname += layer;
		histname += "_biased_residualy";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPIXEtaBarrelRes[layer] = (TH1F*)file->Get(histname);
		histtitle.Clear();
		histtitle += trkCol.Data();
		histtitle += ": Pixel Barrel L";
		histtitle += layer;
		histtitle += " local y residuals";
		hPIXEtaBarrelRes[layer]->SetTitle(histtitle.Data());
		hPIXEtaBarrelRes[layer]->GetXaxis()->SetTitle("Residuals [mm]");
		hPIXEtaBarrelRes[layer]->GetYaxis()->SetTitle("Entries");
		hPIXEtaBarrelRes[layer]->SetLineColor(Color+2);
		hPIXEtaBarrelRes[layer]->SetFillColor(Color);
		hPIXEtaBarrelRes[layer]->SetLineWidth(1);
		hPIXEtaBarrelRes[layer]->SetFillStyle(3001);
		if (normalizePlots) hPIXEtaBarrelRes[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hPIXEtaBarrelRes[layer]->DrawCopy("hist");
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_b";
			histname += layer;
			histname += "_biased_residualy";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			hPIXEtaBarrelRes_2[layer] = (TH1F*)file->Get(histname);	
			hPIXEtaBarrelRes_2[layer]->SetLineColor(ColorForSecond+2);
			hPIXEtaBarrelRes_2[layer]->SetFillColor(ColorForSecond);
			hPIXEtaBarrelRes_2[layer]->SetLineWidth(1);
			hPIXEtaBarrelRes_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hPIXEtaBarrelRes_2[layer]->Scale(1./(float)iEvents);
			hPIXEtaBarrelRes_2[layer]->DrawCopy("histsame");
		}
		
	}


}
