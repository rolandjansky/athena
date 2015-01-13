/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sctECResiduals(TString trkCol, bool ec){
	sctECResiduals(trkCol, ec, 0, "");
}

void sctECResiduals(TString trkCol, bool ec, TString trkCol2){
	sctECResiduals(trkCol, ec, 0, trkCol2);
}

void sctECResiduals(TString trkCol, bool ec, TCanvas *cSCTECRes){
	sctECResiduals(trkCol, ec, cSCTECRes, "");
}

void sctECResiduals(TString trkCol, bool ec, TCanvas *cSCTECRes, TString trkCol2) {
	TH1F *hSCTRes[9];
	TH1F *hSCTRes_2[9];

	
	TString histname;
	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cSCTECRes){
		if(!ec)
			cSCTECRes = new TCanvas("cSCTECRes","SCT Endcap A Residuals per Layer",
			      		CanvasSizeX9[0],CanvasSizeX9[1],
				    	CanvasSizeX9[2], CanvasSizeX9[3]);
		else
			cSCTECRes = new TCanvas("cSCTECRes","SCT Endcap C Residuals per Layer",
			      		CanvasSizeX9[0],CanvasSizeX9[1],
				  		CanvasSizeX9[2], CanvasSizeX9[3]);
			
	}
	cSCTECRes->Divide(3,3);
	for(int layer=0; layer<9; layer++){
		cSCTECRes->cd(layer+1);
		
		if(!ec)
			histname = "IDAlignMon/"+trkCol+"/Residuals/sct_eca_d";
		else
			histname = "IDAlignMon/"+trkCol+"/Residuals/sct_ecc_d";
		histname += layer;
		histname += "_biased_residualx";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hSCTRes[layer] = (TH1F*)file->Get(histname);	
		if(!ec)
			sprintf(name,"SCT Endcap A D%d Residuals (mm)",layer);
		else
			sprintf(name,"SCT Endcap C D%d Residuals (mm)",layer);
		hSCTRes[layer]->SetTitle(name);
		hSCTRes[layer]->GetXaxis()->SetTitle("Residuals (mm)");
		hSCTRes[layer]->GetYaxis()->SetTitle("Entries");
		hSCTRes[layer]->SetLineColor(Color+2);
		hSCTRes[layer]->SetFillColor(Color);
		hSCTRes[layer]->SetLineWidth(1);
		hSCTRes[layer]->SetFillStyle(3001);
		if (normalizePlots) hSCTRes[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hSCTRes[layer]->DrawCopy("hist");
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			if(!ec)
				histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_eca_d";
			else
				histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_ecc_d";
			histname += layer;
			histname += "_biased_residualx";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			hSCTRes_2[layer] = (TH1F*)file->Get(histname);	
			hSCTRes_2[layer]->SetLineColor(ColorForSecond+2);
			hSCTRes_2[layer]->SetFillColor(ColorForSecond);
			hSCTRes_2[layer]->SetLineWidth(1);
			hSCTRes_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hSCTRes_2[layer]->Scale(1./(float)iEvents);
			hSCTRes_2[layer]->DrawCopy("histsame");
		}
	}		
}

