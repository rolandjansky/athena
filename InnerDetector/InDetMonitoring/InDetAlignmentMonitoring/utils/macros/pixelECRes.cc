/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixelECResiduals(TString trkCol, bool ec){
	pixelECResiduals(trkCol, ec, 0, "");
}

void pixelECResiduals(TString trkCol, bool ec, TString trkCol2){
	pixelECResiduals(trkCol, ec, 0, trkCol2);
}

void pixelECResiduals(TString trkCol, bool ec, TCanvas *cPIXECRes){
	pixelECResiduals(trkCol, ec, cPIXECRes, "");
}

void pixelECResiduals(TString trkCol, bool ec, TCanvas *cPIXECRes, TString trkCol2) {
	TH1F *hPIXPhiECRes[3];
	TH1F *hPIXEtaECRes[3];
	
	TH1F *hPIXPhiECRes_2[3];
	TH1F *hPIXEtaECRes_2[3];
	
	TString histname, histtitle;
	Color_t Color;
	Char_t name[100];
	if (trkCol == FirstTrackCollection) Color = ColorForFirst;
	else Color = ColorForSecond;
	
	cout << " Residuals..." << endl;
	if (!cPIXECRes){
		if(!ec)
			cPIXECRes = new TCanvas("cPIXECRes","PIX Endcap A Residuals per Layer",
			      CanvasSizeX6[0],CanvasSizeX6[1]);
		else
			cPIXECRes = new TCanvas("cPIXECRes","PIX Endcap C Residuals per Layer",
			      CanvasSizeX6[0],CanvasSizeX6[1]);
			
	}
	cPIXECRes->Divide(3,2);
	for(int layer=0; layer<3; layer++){
		cPIXECRes->cd(layer+1);
		
		if(!ec)
			histname = "IDAlignMon/"+trkCol+"/Residuals/pix_eca_d";
		else
			histname = "IDAlignMon/"+trkCol+"/Residuals/pix_ecc_d";
		histname += layer;
		histname += "_biased_residualx";
		// histname += "_residualx";
		if(PrintLevel>0) cout << " <pixelECRes> Plotting Histogram: " << histname << endl;
		hPIXPhiECRes[layer] = (TH1F*)file->Get(histname);

		if ( hPIXPhiECRes[layer] == NULL) { // check if histogram exists
		  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
		  return;
		}
	
		histtitle = trkCol.Data();
		if(ec)
		  histtitle += " PIX ECC D";
		else
		  histtitle += " PIX ECA D";
		histtitle += layer;
		histtitle += " local x residuals";

		hPIXPhiECRes[layer]->SetTitle(histtitle.Data());
		hPIXPhiECRes[layer]->GetXaxis()->SetTitle("Local x residuals [mm]");
		hPIXPhiECRes[layer]->GetYaxis()->SetTitle("Entries");
		hPIXPhiECRes[layer]->SetLineColor(Color+2);
		hPIXPhiECRes[layer]->SetFillColor(Color);
		hPIXPhiECRes[layer]->SetLineWidth(1);
		hPIXPhiECRes[layer]->SetFillStyle(3001);
		if (normalizePlots) hPIXPhiECRes[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hPIXPhiECRes[layer]->DrawCopy("hist");
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			if(!ec)
				histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_eca_d";
			else
				histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_ecc_d";
			histname += layer;
			histname += "_biased_residualx";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			hPIXPhiECRes_2[layer] = (TH1F*)file->Get(histname);	
			hPIXPhiECRes_2[layer]->SetLineColor(ColorForSecond+2);
			hPIXPhiECRes_2[layer]->SetFillColor(ColorForSecond);
			hPIXPhiECRes_2[layer]->SetLineWidth(1);
			hPIXPhiECRes_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hPIXPhiECRes_2[layer]->Scale(1./(float)iEvents);
			hPIXPhiECRes_2[layer]->DrawCopy("histsame");
		}
		
		
		cPIXECRes->cd(layer+4);
		if(!ec)
			histname = "IDAlignMon/"+trkCol+"/Residuals/pix_eca_d";
		else
			histname = "IDAlignMon/"+trkCol+"/Residuals/pix_ecc_d";
		histname += layer;
		histname += "_biased_residualy";
		//histname += "_residualy";
		if(PrintLevel>0) cout << " <pixelECRes> Plotting Histogram: " << histname << endl;
		hPIXEtaECRes[layer] = (TH1F*)file->Get(histname);
		histtitle = trkCol.Data();
		if(ec)
		  histtitle += " PIX ECC D";
		else
		  histtitle += " PIX ECA D";
		histtitle += layer;
		histtitle += " local x residuals";

		hPIXEtaECRes[layer]->SetTitle(histtitle.Data());
		hPIXEtaECRes[layer]->GetXaxis()->SetTitle("Local y Residuals (mm)");
		hPIXEtaECRes[layer]->GetYaxis()->SetTitle("Entries");
		hPIXEtaECRes[layer]->SetLineColor(Color+2);
		hPIXEtaECRes[layer]->SetFillColor(Color);
		hPIXEtaECRes[layer]->SetLineWidth(1);
		hPIXEtaECRes[layer]->SetFillStyle(3001);
		if (normalizePlots) hPIXEtaECRes[layer]->Scale(1./(float)iEvents);
		if (!AtlasStyle) {
		 gPad->SetGridy();
		 gPad->SetGridx();
		}
		hPIXEtaECRes[layer]->DrawCopy("hist");
		setTitleFont(font);
		ScalePlots();
		setTitleSize(0.67);
		if(trkCol2 !=""){
			if(!ec)
				histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_eca_d";
			else
				histname = "IDAlignMon/"+trkCol2+"/Residuals/pix_ecc_d";
			histname += layer;
			histname += "_residualy";
	
			if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
			hPIXEtaECRes_2[layer] = (TH1F*)file->Get(histname);	
			hPIXEtaECRes_2[layer]->SetLineColor(ColorForSecond+2);
			hPIXEtaECRes_2[layer]->SetFillColor(ColorForSecond);
			hPIXEtaECRes_2[layer]->SetLineWidth(1);
			hPIXEtaECRes_2[layer]->SetFillStyle(3001);
			if (normalizePlots) hPIXEtaECRes_2[layer]->Scale(1./(float)iEvents);
			hPIXEtaECRes_2[layer]->DrawCopy("histsame");
		}
		
	}


}

