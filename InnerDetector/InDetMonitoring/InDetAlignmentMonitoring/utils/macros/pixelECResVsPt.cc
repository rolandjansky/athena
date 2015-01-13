/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixelECResidualsVsPt(TString trkCol, bool ec){
	pixelECResidualsVsPt(trkCol, ec, 0, "");
}

void pixelECResidualsVsPt(TString trkCol, bool ec, TString trkCol2){
	pixelECResidualsVsPt(trkCol, ec, 0, trkCol2);
}

void pixelECResidualsVsPt(TString trkCol, bool ec, TCanvas *cPIXECResVsPt){
	pixelECResidualsVsPt(trkCol, ec, cPIXECResVsPt, "");
}

void pixelECResidualsVsPt(TString trkCol, bool ec, TCanvas *cPIXECResVsPt, TString trkCol2) {
	TProfile *hPIXPhiECResVsPt[3];
	TProfile *hPIXEtaECResVsPt[3];
	
	TProfile *hPIXPhiECResVsPt_2[3];
	TProfile *hPIXEtaECResVsPt_2[3];
	
	TH2F *tempHist;
	float ProfEtaMax = 0;
	float max,min;
   	float ProfPhiMax = 0;
   	
	TString histname;
	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cPIXECResVsPt){
		if(!ec)
			cPIXECResVsPt = new TCanvas("cPIXECResVsPt","PIX Endcap A Residuals per Layer",
			      CanvasSizeX6[0],CanvasSizeX6[1]);
		else
			cPIXECResVsPt = new TCanvas("cPIXECResVsPt","PIX Endcap C Residuals per Layer",
			      CanvasSizeX6[0],CanvasSizeX6[1]);
			
	}
	cPIXECResVsPt->Divide(3,2);
	for(int layer=0; layer<3; layer++){
		cPIXECResVsPt->cd(layer+1);
		
		if(!ec)
			histname = "IDAlignMon/"+trkCol+"/Residuals/pix_eca_d";
		else
			histname = "IDAlignMon/"+trkCol+"/Residuals/pix_ecc_d";
		histname += layer;
		histname += "_biased_residualx_pt";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		tempHist = (TH2F*)file->Get(histname);
		hPIXPhiECResVsPt[layer] = tempHist->ProfileX();	
		if(!ec)
			sprintf(name,"Pixel Endcap A D%d #phi Residuals Vs Pt ",layer);
		else
			sprintf(name,"Pixel Endcap C D%d #phi Residuals Vs Pt",layer);
		hPIXPhiECResVsPt[layer]->SetTitle(name);
		hPIXPhiECResVsPt[layer]->GetXaxis()->SetTitle("qPt (GeV)");
		hPIXPhiECResVsPt[layer]->GetYaxis()->SetTitle("#phi Residuals (mm)");
		hPIXPhiECResVsPt[layer]->SetLineColor(Color+2);
		hPIXPhiECResVsPt[layer]->SetFillColor(Color);
		hPIXPhiECResVsPt[layer]->SetLineWidth(1);
		hPIXPhiECResVsPt[layer]->SetFillStyle(3001);
		if (normalizePlots) hPIXPhiECResVsPt[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hPIXPhiECResVsPt[layer]->DrawCopy();
		max = fabs(hPIXPhiECResVsPt[layer]->GetMaximum());
		min = fabs(hPIXPhiECResVsPt[layer]->GetMinimum());
		if(ProfPhiMax<max) ProfPhiMax = max;
		if(ProfPhiMax<min) ProfPhiMax = min;
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			if(!ec)
				histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_eca_d";
			else
				histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_ecc_d";
			histname += layer;
			histname += "_biased_residualx_pt";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			tempHist = (TH2F*)file->Get(histname);
			hPIXPhiECResVsPt_2[layer] = tempHist->ProfileX();	
			hPIXPhiECResVsPt_2[layer]->SetLineColor(ColorForSecond+2);
			hPIXPhiECResVsPt_2[layer]->SetFillColor(ColorForSecond);
			hPIXPhiECResVsPt_2[layer]->SetLineWidth(1);
			hPIXPhiECResVsPt_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hPIXPhiECResVsPt_2[layer]->Scale(1./(float)iEvents);
			hPIXPhiECResVsPt_2[layer]->DrawCopy("same");
			max = fabs(hPIXPhiECResVsPt_2[layer]->GetMaximum());
			min = fabs(hPIXPhiECResVsPt_2[layer]->GetMinimum());
			if(ProfPhiMax<max) ProfPhiMax = max;
			if(ProfPhiMax<min) ProfPhiMax = min;
		}
		
		
		cPIXECResVsPt->cd(layer+4);
		if(!ec)
			histname = "IDAlignMon/"+trkCol+"/Residuals/pix_eca_d";
		else
			histname = "IDAlignMon/"+trkCol+"/Residuals/pix_ecc_d";
		histname += layer;
		histname += "_biased_residualy_pt";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		tempHist = (TH2F*)file->Get(histname);
		hPIXEtaECResVsPt[layer] = tempHist->ProfileX();	
		if(!ec)
			sprintf(name,"Pixel Endcap A D%d #eta Residuals Vs Pt",layer);
		else
			sprintf(name,"Pixel Endcap C D%d #eta Residuals Vs Pt",layer);
		hPIXEtaECResVsPt[layer]->SetTitle(name);
		hPIXEtaECResVsPt[layer]->GetXaxis()->SetTitle("qPt (GeV)");
		hPIXEtaECResVsPt[layer]->GetYaxis()->SetTitle("#eta Residuals (mm)");
		hPIXEtaECResVsPt[layer]->SetLineColor(Color+2);
		hPIXEtaECResVsPt[layer]->SetFillColor(Color);
		hPIXEtaECResVsPt[layer]->SetLineWidth(1);
		hPIXEtaECResVsPt[layer]->SetFillStyle(3001);
		if (normalizePlots) hPIXEtaECResVsPt[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hPIXEtaECResVsPt[layer]->DrawCopy();
		max = fabs(hPIXEtaECResVsPt[layer]->GetMaximum());
		min = fabs(hPIXEtaECResVsPt[layer]->GetMinimum());
		if(ProfEtaMax<max) ProfEtaMax = max;
		if(ProfEtaMax<min) ProfEtaMax = min;
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			if(!ec)
				histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_eca_d";
			else
				histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_ecc_d";
			histname += layer;
			histname += "_biased_residualy_pt";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			tempHist = (TH2F*)file->Get(histname);
			hPIXEtaECResVsPt_2[layer] = tempHist->ProfileX();	
			hPIXEtaECResVsPt_2[layer]->SetLineColor(ColorForSecond+2);
			hPIXEtaECResVsPt_2[layer]->SetFillColor(ColorForSecond);
			hPIXEtaECResVsPt_2[layer]->SetLineWidth(1);
			hPIXEtaECResVsPt_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hPIXEtaECResVsPt_2[layer]->Scale(1./(float)iEvents);
			hPIXEtaECResVsPt_2[layer]->DrawCopy("same");
			max = fabs(hPIXEtaECResVsPt_2[layer]->GetMaximum());
			min = fabs(hPIXEtaECResVsPt_2[layer]->GetMinimum());
			if(ProfEtaMax<max) ProfEtaMax = max;
			if(ProfEtaMax<min) ProfEtaMax = min;
		}
		
	}
	for(int layer=0; layer<3; layer++){
		hPIXPhiECResVsPt[layer]->SetMaximum(ProfPhiMax*1.1);
		hPIXPhiECResVsPt[layer]->SetMinimum(-ProfPhiMax*1.1);
		hPIXEtaECResVsPt[layer]->SetMaximum(ProfEtaMax*1.1);
		hPIXEtaECResVsPt[layer]->SetMinimum(-ProfEtaMax*1.1);
	}

}

