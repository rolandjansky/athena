/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sctBarrelResidualsVsPt(TString trkCol){
	sctBarrelResidualsVsPt(trkCol, 0, "");
}

void sctBarrelResidualsVsPt(TString trkCol, TString trkCol2){
	sctBarrelResidualsVsPt(trkCol, 0, trkCol2);
}

void sctBarrelResidualsVsPt(TString trkCol, TCanvas *cSCTBarrelResVsPt){
	sctBarrelResidualsVsPt(trkCol, cSCTBarrelResVsPt, "");
}

void sctBarrelResidualsVsPt(TString trkCol, TCanvas *cSCTBarrelResVsPt, TString trkCol2) {
  constexpr size_t nLayers{4};
	TProfile *hSCTBarrelResVsPt[nLayers];
	TProfile *hSCTBarrelResVsPt_2[nLayers];
	
	float ProfMax = 0;
	float max,min;
   	
	TH2F *tempHist;
	TString histname;
	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cSCTBarrelResVsPt){
		cSCTBarrelResVsPt = new TCanvas("cSCTBarrelResVsPt","SCT Barrel Residuals per Layer",
			      CanvasSizeX4[0],CanvasSizeX4[1]);
	}
	cSCTBarrelResVsPt->Divide(2,2);
	for(int layer=0; layer<nLayers; layer++){
		cSCTBarrelResVsPt->cd(layer+1);
		
		histname = "IDAlignMon/"+trkCol+"/Residuals/sct_b";
		histname += layer;
		histname += "_biased_residualx_pt";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		tempHist = (TH2F*)file->Get(histname);
		hSCTBarrelResVsPt[layer] = tempHist->ProfileX();	
		sprintf(name,"SCT Barrel L%d Residuals Vs Pt",layer);
		hSCTBarrelResVsPt[layer]->SetTitle(name);
		hSCTBarrelResVsPt[layer]->GetXaxis()->SetTitle("Track qPt(GeV)");
		hSCTBarrelResVsPt[layer]->GetYaxis()->SetTitle("Residuals (mm)");
		hSCTBarrelResVsPt[layer]->SetLineColor(Color+2);
		hSCTBarrelResVsPt[layer]->SetFillColor(Color);
		hSCTBarrelResVsPt[layer]->SetLineWidth(1);
		hSCTBarrelResVsPt[layer]->SetFillStyle(3001);
		if (normalizePlots) hSCTBarrelResVsPt[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hSCTBarrelResVsPt[layer]->DrawCopy();
		max = fabs(hSCTBarrelResVsPt[layer]->GetMaximum());
		min = fabs(hSCTBarrelResVsPt[layer]->GetMinimum());
		if(ProfMax<max) ProfMax = max;
		if(ProfMax<min) ProfMax = min;
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_b";
			histname += layer;
			histname += "_biased_residualx_pt";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			tempHist = (TH2F*)file->Get(histname);
		    hSCTBarrelResVsPt_2[layer] = tempHist->ProfileX();	
			hSCTBarrelResVsPt_2[layer]->SetLineColor(ColorForSecond+2);
			hSCTBarrelResVsPt_2[layer]->SetFillColor(ColorForSecond);
			hSCTBarrelResVsPt_2[layer]->SetLineWidth(1);
			hSCTBarrelResVsPt_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hSCTBarrelResVsPt_2[layer]->Scale(1./(float)iEvents);
			hSCTBarrelResVsPt_2[layer]->DrawCopy("same");
			max = fabs(hSCTBarrelResVsPt_2[layer]->GetMaximum());
			min = fabs(hSCTBarrelResVsPt_2[layer]->GetMinimum());
			if(ProfMax<max) ProfMax = max;
			if(ProfMax<min) ProfMax = min;
		}
		
	}
	for(int layer=0; layer<4; layer++){
		hSCTBarrelResVsPt[layer]->SetMaximum(ProfMax*1.1);
		hSCTBarrelResVsPt[layer]->SetMinimum(-ProfMax*1.1);
	}
}
