/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sctBarrelHitmap(TString TrkCol){
	sctBarrelHitmap(TrkCol, 0, "");
}
void sctBarrelHitmap(TString TrkCol, TString DrawOptions){
	sctBarrelHitmap(TrkCol, 0, DrawOptions);
}

void sctBarrelHitmap(TString TrkCol, TCanvas *cSCTHitMapBa,	TString DrawOptions){
	cout << cSCTHitMapBa << endl;
	TH2F *hTmpHist;
	TH2F *hSctBa[4];
	TString histname;
	bool setCommonMax = true;
	const int iNSCT_Layers = 4;
	Char_t name[100];
	int maxx[2];
	int maxb[4];
	DrawOptions.ToLower();
	bool NoText = DrawOptions.Contains("notext");

	if(!cSCTHitMapBa)
		cSCTHitMapBa = new TCanvas("cSCTHitMapBa","Hit Map of SCT Barrel",
						CanvasSizeX4[0],CanvasSizeX4[1]);

	//------------
	// Barrel
	//------------
	cout << "	 - SCT Barrel..." << endl;
	
	// Module physical axis in Z (to be drawn on top along graphics X)
	TGaxis *ATLZaxis[4]; 
	TGaxis *ATLPhiaxis[4]; 
	
	maxx[0]=0;
	for(int layer = 0; layer<iNSCT_Layers; layer++){
		histname = "IDAlignMon/"+TrkCol+"/HitEfficiencies/measurements_vs_Eta_Phi_sct_b";
		histname +=layer;		
		if(PrintLevel>0) cout << " <sctBarrelHitmap> Plotting Histogram: " << histname << endl;
		hTmpHist = (TH2F*)file->Get(histname); 
		hSctBa[layer] = (TH2F*)hTmpHist->Clone("hSctBa");
		hSctBa[layer]->SetBins(12,-5.5,6.5,hTmpHist->GetNbinsY(),hTmpHist->GetYaxis()->GetXmin(),hTmpHist->GetYaxis()->GetXmax());
		for(int ybin=1; ybin<=hSctBa[layer]->GetNbinsY(); ybin++){
			for(int xbin=1; xbin<=hSctBa[layer]->GetNbinsX(); xbin++){
				if(xbin<7)
					hSctBa[layer]->SetBinContent(xbin, ybin, hTmpHist->GetBinContent(xbin, ybin));
				else
					hSctBa[layer]->SetBinContent(xbin, ybin, hTmpHist->GetBinContent(xbin+1, ybin));
			}
		}
		SCTMaps_SetXaxisLabels(hSctBa[layer]);
		maxb[layer] = (int)hSctBa[layer]->GetBinContent(hSctBa[layer]->GetMaximumBin());
		maxx[0] = maxb[layer]>maxx[0] ? maxb[layer] : maxx[0];
		SetHistoAtt(hSctBa[layer]);
		sprintf(name,"Hitmap %s - SCT Barrel Layer %d", TrkCol.Data(), layer);
	
		hSctBa[layer]->SetTitle(name);
		hSctBa[layer]->GetYaxis()->SetTitleOffset(1.0);
		hSctBa[layer]->GetXaxis()->SetLabelSize(0.04);
		hSctBa[layer]->SetXTitle("ring");
		hSctBa[layer]->SetYTitle("sector");
		
		
	}

	cSCTHitMapBa->Divide(2,2);
	for(int layer=0; layer<iNSCT_Layers; layer++){
		cSCTHitMapBa->cd(layer+1);
		if(setCommonMax) hSctBa[layer]->SetMaximum(maxx[0]);	
		NoText ? hSctBa[layer]->Draw("colz") : hSctBa[layer]->Draw("colztext");

		// Z axis on top (only for histos with 12 bins) 
		
		if (hSctBa[layer]->GetNbinsX()==12) {
			ATLZaxis[layer] = new TGaxis(-5.5,31.5,6.5,31.5,-800,800,510,"-");
			ATLZaxis[layer]->SetName("Z");
			if (layer == 0) {ATLZaxis[layer]->SetY1(31.5);ATLZaxis[layer]->SetY2(31.5);}
			if (layer == 1) {ATLZaxis[layer]->SetY1(39.5);ATLZaxis[layer]->SetY2(39.5);}
			if (layer == 2) {ATLZaxis[layer]->SetY1(47.5);ATLZaxis[layer]->SetY2(47.5);}
			if (layer == 3) {ATLZaxis[layer]->SetY1(55.5);ATLZaxis[layer]->SetY2(55.5);}
			ATLZaxis[layer]->SetLabelSize(hSctBa[layer]->GetYaxis()->GetLabelSize());
			ATLZaxis[layer]->SetLabelOffset(0.0);
			ATLZaxis[layer]->SetLabelFont(hSctBa[layer]->GetXaxis()->GetLabelFont());
			ATLZaxis[layer]->SetTitleSize(hSctBa[layer]->GetXaxis()->GetTitleSize());
			ATLZaxis[layer]->SetTitleFont(hSctBa[layer]->GetXaxis()->GetTitleFont());
			ATLZaxis[layer]->SetTitle(" z (mm)");
			ATLZaxis[layer]->Draw();
		}
		// moving the color scale
		gPad->Update();
		TPaletteAxis *palette = 
	(TPaletteAxis*)hSctBa[layer]->GetListOfFunctions()->FindObject("palette");
		palette->SetX1NDC(0.935);
		palette->SetX2NDC(0.965);
		palette->SetY1NDC(0.1);
		palette->SetY2NDC(0.75);
		gPad->Modified();
		// Phi axis on the right 
		ATLPhiaxis[layer] = new TGaxis(6.5,-0.5,6.5,31.5,0.,360.,510,"+L");
		ATLPhiaxis[layer]->SetName("Z");
		if (layer == 0) {ATLPhiaxis[layer]->SetY2(31.5);}
		if (layer == 1) {ATLPhiaxis[layer]->SetY2(39.5);}
		if (layer == 2) {ATLPhiaxis[layer]->SetY2(47.5);}
		if (layer == 3) {ATLPhiaxis[layer]->SetY2(55.5);}
		ATLPhiaxis[layer]->SetLabelSize(hSctBa[layer]->GetYaxis()->GetLabelSize());
		ATLPhiaxis[layer]->SetLabelOffset(0.0);
		ATLPhiaxis[layer]->SetLabelFont(hSctBa[layer]->GetXaxis()->GetLabelFont());
		ATLPhiaxis[layer]->SetTitleSize(hSctBa[layer]->GetXaxis()->GetTitleSize());
		ATLPhiaxis[layer]->SetTitleFont(hSctBa[layer]->GetXaxis()->GetTitleFont());
		ATLPhiaxis[layer]->SetTitle("#phi (deg)");
		ATLPhiaxis[layer]->SetLabelOffset(0.01);
		ATLPhiaxis[layer]->Draw();
	
	}
		
	
}



