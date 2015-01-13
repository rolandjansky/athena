/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixelHitmaps(TString TrkCol, TString DrawOptions){
	pixelHitmaps(TrkCol, 0, DrawOptions);
}

void pixelHitmaps(TString TrkCol, TCanvas *cPIXHitMap, TString DrawOptions){
	if(!cPIXHitMap)
		cPIXHitMap = new TCanvas("cPIXHitMap","Pixel Hitmap",
						 CanvasSizeX9[0], CanvasSizeX9[1],
						 CanvasSizeX9[2], CanvasSizeX9[3]);

	const int iNPIX_Layers = 3;
	const int iNPIX_Discs	= 3;
	
	bool setCommonMax = true;
	TString histname;
	Char_t name[100];
	gStyle->SetPadRightMargin(0.15);
	gStyle->SetPadLeftMargin(0.08);
	gStyle->SetPadTopMargin(0.12);
	gROOT->ForceStyle();

	//---------------------------------------------------- 
	// option handling
	//----------------------------------------------------
	DrawOptions.ToLower();
	bool NoText = DrawOptions.Contains("notext");
	NoText ? cout << " <pixelHitmaps> Hitmaps..." << endl : cout << " Hitmaps (with entries)..." << endl;
	
	TString title;
	int maxx;
	int maxb[3];
	
	////////////////////
	// PIX			//
	////////////////////
	
	TH2F *hPixBa[3];
	TH1F *hPixEc[2][3];
	TH2F *tempHist;
	cPIXHitMap->Divide(3,3);

	//------------
	// Barrel
	//------------
	cout << "	 - Pixel Barrel..." << endl;

	maxx=0;
	for(int layer = 0; layer<iNPIX_Layers; layer++){
		histname = "IDAlignMon/"+TrkCol+"/HitEfficiencies/measurements_vs_Eta_Phi_pix_b";
		histname += layer;
		hPixBa[layer] = (TH2F*)file->Get(histname); 
		maxb[layer] = (int) hPixBa[layer]->GetBinContent(hPixBa[layer]->GetMaximumBin());
		maxx = maxb[layer]>maxx ? maxb[layer] : maxx;
		SetHistoAtt(hPixBa[layer]);
	}

	for(int layer=0; layer<iNPIX_Layers; layer++){
		cPIXHitMap->cd(layer+1+3);
		if(setCommonMax) hPixBa[layer]->SetMaximum(maxx);
		sprintf(name,"Hitmap %s - Pixel Barrel Layer %d", TrkCol.Data(), layer);
		hPixBa[layer]->SetTitle(name);
		hPixBa[layer]->GetYaxis()->SetTitleOffset(1.0);
		NoText ? hPixBa[layer]->Draw("colz") : hPixBa[layer]->Draw("colztext");
		gPad->Update();
		setTitleFont(font);
	}
	
	//------------
	// Endcaps
	//------------
	cout << "	 - Pixel EndCaps..." << endl;
	for(int ec=0; ec<2; ec++){
		maxx=0;
		for(int disc=0; disc<iNPIX_Discs; disc++){
			if(ec==0) histname = "IDAlignMon/"+TrkCol+"/HitEfficiencies/measurements_vs_Eta_Phi_pix_eca";
			else histname = "IDAlignMon/"+TrkCol+"/HitEfficiencies/measurements_vs_Eta_Phi_pix_ecc";
			tempHist = (TH2F*)file->Get(histname); 
			hPixEc[ec][disc] =(TH1F*)tempHist->ProjectionY("_py",disc+1,disc+1);
	
			maxb[disc] = (int) hPixEc[ec][disc]->GetBinContent(hPixEc[ec][disc]->GetMaximumBin());
			maxx = maxb[disc]>maxx ? maxb[disc] : maxx;
		}
	}

	/* Box filled in white to hide histogram. Could not get rif off this since 
		 a TH2F needs to be drawn afterwards with chopt_z option for the 
		 color palette to be created. Tried to draw in a pad and then 
		 superimpose the "box" histo but then any pointer to the original 
		 TPaletteAxis gets of course deleted. Changing the range in x-axis to make 
		 "disappear" the visible entries ends with the same feature.
	*/

	TBox *blank = new TBox(-10,-10, 10,-8);
	blank->SetFillColor(kWhite);
	
	for(int ec=0; ec<2; ec++){
		for(int disc=0; disc<iNPIX_Discs; disc++){
	if(!ec) cPIXHitMap->cd(iNPIX_Discs*ec+disc+1);
	else cPIXHitMap->cd(iNPIX_Discs*ec+disc+1+3);
	
	// get histograms
	if(ec==0) histname = "IDAlignMon/"+TrkCol+"/HitEfficiencies/measurements_vs_Eta_Phi_pix_eca";
	else histname = "IDAlignMon/"+TrkCol+"/HitEfficiencies/measurements_vs_Eta_Phi_pix_ecc";

	tempHist = (TH2F*)file->Get(histname); 
	hPixEc[ec][disc] =(TH1F*)tempHist->ProjectionY("_py",disc+1,disc+1);

	// create fake histogram just for axes and color palette
	sprintf(name,"Hitmap %s - Pixel EndCapC disk %d", TrkCol.Data(), disc);
	if(!ec) sprintf(name,"Hitmap %s - Pixel EndCapA disk %d", TrkCol.Data(), disc);
	TH2F *box = new TH2F(name, name, 100, -10, 10, 100, -10, 10);
	box->SetStats(kFALSE);
	for(int bin=1; bin<=hPixEc[ec][disc]->GetNbinsX(); bin++)
		box->SetBinContent(bin, 1, hPixEc[ec][disc]->GetBinContent(bin));
	if(setCommonMax) {box->SetMaximum(maxx);	
		box->SetMinimum(0); 
	}
	box->GetXaxis()->SetLabelColor(kWhite);
	box->GetYaxis()->SetLabelColor(kWhite);
	box->GetYaxis()->SetTitle("Y axis");
	box->GetXaxis()->SetTitle("X axis");
	box->GetYaxis()->SetTitleOffset(0.8);
	box->Draw("colz");
	blank->Draw();
	gPad->RedrawAxis();
	hPixEc[ec][disc]->SetMaximum(maxx);
	hPixEc[ec][disc]->SetMinimum(0); 
	draw_pixel_ec_modules(hPixEc[ec][disc]);

	setTitleFont(font);
	gPad->Update();
		}
	}
}

