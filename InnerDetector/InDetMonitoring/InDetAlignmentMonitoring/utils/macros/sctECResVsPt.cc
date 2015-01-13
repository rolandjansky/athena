/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sctECResidualsVsPt(TString trkCol, bool ec){
	sctECResidualsVsPt(trkCol, ec, 0, "");
}

void sctECResidualsVsPt(TString trkCol, bool ec, TString trkCol2){
	sctECResidualsVsPt(trkCol, ec, 0, trkCol2);
}

void sctECResidualsVsPt(TString trkCol, bool ec, TCanvas *cSCTECResVsPt){
	sctECResidualsVsPt(trkCol, ec, cSCTECResVsPt, "");
}

void sctECResidualsVsPt(TString trkCol, bool ec, TCanvas *cSCTECResVsPt, TString trkCol2) {
	TProfile *hSCTECResVsPt[9];
	TProfile *hSCTECResVsPt_2[9];
	TH2F *tempHist;
	float max,min;
   	float ProfMax = 0;
   	
	TString histname;
	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cSCTECResVsPt){
		if(!ec)
			cSCTECResVsPt = new TCanvas("cSCTECResVsPt","SCT Endcap A Residuals per Layer",
			      				CanvasSizeX9[0],CanvasSizeX9[1],
				    			CanvasSizeX9[2], CanvasSizeX9[3]);
		else
			cSCTECResVsPt = new TCanvas("cSCTECResVsPt","SCT Endcap C Residuals per Layer",
			     			 	CanvasSizeX9[0],CanvasSizeX9[1],
				    			CanvasSizeX9[2], CanvasSizeX9[3]);
			
	}
	cSCTECResVsPt->Divide(3,3);
	for(int layer=0; layer<9; layer++){
		cSCTECResVsPt->cd(layer+1);
		
		if(!ec)
			histname = "IDAlignMon/"+trkCol+"/Residuals/sct_eca_d";
		else
			histname = "IDAlignMon/"+trkCol+"/Residuals/sct_ecc_d";
		histname += layer;
		histname += "_biased_residualx_pt";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		tempHist = (TH2F*)file->Get(histname);
		hSCTECResVsPt[layer] = tempHist->ProfileX();	
		if(!ec)
			sprintf(name,"SCT Endcap A D%d Residuals Vs Pt ",layer);
		else
			sprintf(name,"SCT Endcap C D%d Residuals Vs Pt",layer);
		hSCTECResVsPt[layer]->SetTitle(name);
		hSCTECResVsPt[layer]->GetXaxis()->SetTitle("qPt (GeV)");
		hSCTECResVsPt[layer]->GetYaxis()->SetTitle("Residuals (mm)");
		hSCTECResVsPt[layer]->SetLineColor(Color+2);
		hSCTECResVsPt[layer]->SetFillColor(Color);
		hSCTECResVsPt[layer]->SetLineWidth(1);
		hSCTECResVsPt[layer]->SetFillStyle(3001);
		if (normalizePlots) hSCTECResVsPt[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hSCTECResVsPt[layer]->DrawCopy();
		max = fabs(hSCTECResVsPt[layer]->GetMaximum());
		min = fabs(hSCTECResVsPt[layer]->GetMinimum());
		if(ProfMax<max) ProfMax = max;
		if(ProfMax<min) ProfMax = min;
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			if(!ec)
				histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_eca_d";
			else
				histname = "IDAlignMon/"+trkCol2+"/Residuals/sct_ecc_d";
			histname += layer;
			histname += "_biased_residualx_pt";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			tempHist = (TH2F*)file->Get(histname);
			hSCTECResVsPt_2[layer] = tempHist->ProfileX();	
			hSCTECResVsPt_2[layer]->SetLineColor(ColorForSecond+2);
			hSCTECResVsPt_2[layer]->SetFillColor(ColorForSecond);
			hSCTECResVsPt_2[layer]->SetLineWidth(1);
			hSCTECResVsPt_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hSCTECResVsPt_2[layer]->Scale(1./(float)iEvents);
			hSCTECResVsPt_2[layer]->DrawCopy("same");
			max = fabs(hSCTECResVsPt_2[layer]->GetMaximum());
			min = fabs(hSCTECResVsPt_2[layer]->GetMinimum());
			if(ProfMax<max) ProfMax = max;
			if(ProfMax<min) ProfMax = min;
		}
	
	}
	
	for(int layer=0; layer<9; layer++){
		hSCTECResVsPt[layer]->SetMaximum(ProfMax*1.1);
		hSCTECResVsPt[layer]->SetMinimum(-ProfMax*1.1);
	}

}

