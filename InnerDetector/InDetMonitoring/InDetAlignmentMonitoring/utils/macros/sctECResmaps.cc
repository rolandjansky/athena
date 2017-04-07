/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sctECResmaps(TString TrkCol, bool ec, bool side, bool isRMS, float range){
	sctECResmaps(TrkCol, 0, ec, side, isRMS, range);
}

void sctECResmaps(TString TrkCol, TCanvas *cSCTResMapEC, bool ec, bool side, bool isRMS, float range){
	
	if(!cSCTResMapEC){
    	if(!ec)
			cSCTResMapEC = new TCanvas("cSCTResMapECA","SCT Endcap A Residual map",CanvasSizeX9[0],CanvasSizeX9[1],
				       CanvasSizeX9[2], CanvasSizeX9[3]);
		else
			cSCTResMapEC = new TCanvas("cSCTResMapECC","SCT Endcap C Residual map",CanvasSizeX9[0],CanvasSizeX9[1],
				       CanvasSizeX9[2], CanvasSizeX9[3]);
	}
	
	TString histname;
	TString name;
	gStyle->SetPadRightMargin(0.15);
	gStyle->SetPadLeftMargin(0.08);
	gStyle->SetPadTopMargin(0.12);
	gROOT->ForceStyle();


	TString title;
	
	////////////////////
	// SCT EC     //
	////////////////////
	
	TH3F *hSCTResEC3D[iNSCT_Discs];
	TH2F *hSCTResEC2D[iNSCT_Discs];
	
	cSCTResMapEC->Divide(3,3);
	
	
	cout << " <sctECResmaps>  - Sct Endcap";
	if(ec) cout << " C" << endl;
	else cout << " A" << endl;

	for(int disc = 0; disc<iNSCT_Discs; disc++){
		histname = "IDAlignMon/"+TrkCol+"/Residuals/sct_";
		if(ec) histname+="ecc_d";
		else histname+="eca_d";
		histname += disc;
		if(side)
			histname += "_s1_biased_xresvsmodetaphi_3d";
		else
			histname += "_s0_biased_xresvsmodetaphi_3d";
		if(PrintLevel>0) cout << " <sctECResmaps> Plotting Histogram: " << histname << endl;
		hSCTResEC3D[disc] = (TH3F*)file->Get(histname); 

		if ( hSCTResEC3D[disc] == NULL) { // check if histogram exists
		  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
		  return;
		}

		name = TrkCol.Data();
		name += ": ";

		if(ec) name+="SCT ECC Disc ";
		else name+="SCT ECA Disc ";
		name+=disc;

		if(side)
			name += "Side 1 ";
		else
			name += "Side 0 ";
		if(isRMS)
			name += "Residual RMS map - ";
		else
			name += "Residual Mean map - ";
		

		hSCTResEC2D[disc] = new TH2F(name+"Plot",name,hSCTResEC3D[disc]->GetXaxis()->GetNbins(), 
										 hSCTResEC3D[disc]->GetXaxis()->GetXmin(),
										 hSCTResEC3D[disc]->GetXaxis()->GetXmax(),
										 hSCTResEC3D[disc]->GetYaxis()->GetNbins(), 
										 hSCTResEC3D[disc]->GetYaxis()->GetXmin(),
										 hSCTResEC3D[disc]->GetYaxis()->GetXmax());
		for(int i = 1; i<=hSCTResEC3D[disc]->GetXaxis()->GetNbins();i++){
			for(int j = 1; j<=hSCTResEC3D[disc]->GetYaxis()->GetNbins();j++){
				if(isRMS)	
					hSCTResEC2D[disc]->SetBinContent(i,j,1000.*hSCTResEC3D[disc]->ProjectionZ("_z",i,i,j,j)->GetRMS());
				else
					hSCTResEC2D[disc]->SetBinContent(i,j,1000.*hSCTResEC3D[disc]->ProjectionZ("_z",i,i,j,j)->GetMean());
			}		
		}
		
		hSCTResEC2D[disc]->SetContour(NContLevels);
	    hSCTResEC2D[disc]->SetMaximum(range);
	    if(isRMS)
     		hSCTResEC2D[disc]->SetMinimum(0);
	    else 
	    	hSCTResEC2D[disc]->SetMinimum(-range);
	    hSCTResEC2D[disc]->SetStats(kFALSE);
		cSCTResMapEC->cd(disc+1);
		TBox *blank = new TBox(-10,-10, 10, 9.85);
	    blank->SetFillColor(kWhite);
		draw_wheel_frame(name,-range,range);

	    blank->Draw();
	    gPad->SetRightMargin(0.20);
	    gPad->SetLeftMargin(0.15);
	    gPad->RedrawAxis();
	    gPad->Update();
	    setTitleFont(font);
	    draw_sct_ec_modules(hSCTResEC2D[disc], disc);
	    
	}	
	
}
