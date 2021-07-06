/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sctBarrelResiduals(TString trkCol){
	sctBarrelResiduals(trkCol, 0, "");
}

void sctBarrelResiduals(TString trkCol, TString trkCol2){
	sctBarrelResiduals(trkCol, 0, trkCol2);
}

void sctBarrelResiduals(TString trkCol, TCanvas *cSCTBarrelRes){
	sctBarrelResiduals(trkCol, cSCTBarrelRes, "");
}

void sctBarrelResiduals(TString trkCol, TCanvas *cSCTBarrelRes, TString trkCol2) {
  constexpr size_t nLayers{4};
	TH1F *hSCTBarrelRes[nLayers];
	TH1F *hSCTBarrelRes_2[nLayers];
	
	TString histname, histtitle;
	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cSCTBarrelRes){
		cSCTBarrelRes = new TCanvas("cSCTBarrelRes","SCT Barrel Residuals per Layer",
			      CanvasSizeX4[0],CanvasSizeX4[1]);
	}
	cSCTBarrelRes->Divide(2,2);
	for(int layer=0; layer<nLayers; layer++){
		cSCTBarrelRes->cd(layer+1);
		
		histname = "IDAlignMon/"+trkCol+"/Residuals/sct_b";
		histname += layer;
		histname += "_biased_residualx";
		if(PrintLevel>0) cout << " <sctBarrelRes> Plotting Histogram: " << histname << endl;
		hSCTBarrelRes[layer] = (TH1F*)file->Get(histname);

		if ( hSCTBarrelRes[layer] == NULL) { // check if histogram exists
		  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
		  return;
		}
	
	
		histtitle.Clear();
		histtitle += trkCol.Data();
		histtitle += ": SCT Barrel L";
		histtitle += layer;
		histtitle += " residuals";
		hSCTBarrelRes[layer]->SetTitle(histtitle.Data());
		hSCTBarrelRes[layer]->GetXaxis()->SetTitle("Residuals [mm]");
		hSCTBarrelRes[layer]->GetYaxis()->SetTitle("Entries");
		hSCTBarrelRes[layer]->SetLineColor(Color+2);
		hSCTBarrelRes[layer]->SetFillColor(Color);
		hSCTBarrelRes[layer]->SetLineWidth(1);
		hSCTBarrelRes[layer]->SetFillStyle(3001);
		if (normalizePlots) hSCTBarrelRes[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hSCTBarrelRes[layer]->DrawCopy("hist");
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_b";
			histname += layer;
			histname += "_biased_residualx";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			hSCTBarrelRes_2[layer] = (TH1F*)file->Get(histname);	
			hSCTBarrelRes_2[layer]->SetLineColor(ColorForSecond+2);
			hSCTBarrelRes_2[layer]->SetFillColor(ColorForSecond);
			hSCTBarrelRes_2[layer]->SetLineWidth(1);
			hSCTBarrelRes_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hSCTBarrelRes_2[layer]->Scale(1./(float)iEvents);
			hSCTBarrelRes_2[layer]->DrawCopy("histsame");
		}
			
	}


}
