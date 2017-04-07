/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void pixelResmaps(TString TrkCol, bool isXres, bool isRMS, float range){
	pixelResmaps(TrkCol, 0, isXres, isRMS, range);
}

void pixelResmaps(TString TrkCol, TCanvas *cPIXResMap, bool isXres, bool isRMS, float range){
	if(!cPIXResMap)
		cPIXResMap = new TCanvas("cPIXResMap","Pixel Resmap",
						 CanvasSizeX9[0], CanvasSizeX9[1],
						 CanvasSizeX9[2], CanvasSizeX9[3]);
						 
	
	
	TString histname;
	TString name;
	gStyle->SetPadRightMargin(0.15);
	gStyle->SetPadLeftMargin(0.08);
	gStyle->SetPadTopMargin(0.12);
	gROOT->ForceStyle();


	TString title;
	
	////////////////////
	// PIX			//
	////////////////////
	
	TH3F *hPixResBa3D[iNPIX_Layers];
	TH2F *hResMapBa[iNPIX_Layers];
	
	cPIXResMap->Divide(3,3);
	//------------
	// Barrel
	//------------
	cout << "	 - Pixel Barrel..." << endl;

	for(int layer = 0; layer<iNPIX_Layers; layer++){
		histname = "IDAlignMon/"+TrkCol+"/Residuals/pix_b";
		histname += layer;
		if(isXres)
			histname += "_biased_xresvsmodetaphi_3d";
		else
			histname += "_biased_yresvsmodetaphi_3d";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixResBa3D[layer] = (TH3F*)file->Get(histname); 

		if ( hPixResBa3D[layer] == NULL) { // check if histogram exists
		  std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
		  return;
		}

		if(TrkCol == FirstTrackCollection) {
		  name = TrkCol.Data();
		  name +=": "; 
		}
		else {
		  name = SecondTrackCollection.Data();
		}
		if(isXres)
			name += "local x ";
		else
			name += "local y ";
		if(isRMS)
			name += "Residual RMS map - ";
		else
			name += "Residual Mean map - ";
		
		name+="Pixel Barrel Layer ";
		name+=layer;
		hResMapBa[layer] = new TH2F(name,name,hPixResBa3D[layer]->GetXaxis()->GetNbins(), 
										 hPixResBa3D[layer]->GetXaxis()->GetXmin(),
										 hPixResBa3D[layer]->GetXaxis()->GetXmax(),
										 hPixResBa3D[layer]->GetYaxis()->GetNbins(), 
										 hPixResBa3D[layer]->GetYaxis()->GetXmin(),
										 hPixResBa3D[layer]->GetYaxis()->GetXmax());
		for(int i = 1; i<=hPixResBa3D[layer]->GetXaxis()->GetNbins();i++){
			for(int j = 1; j<=hPixResBa3D[layer]->GetYaxis()->GetNbins();j++){
				if(isRMS)	
					hResMapBa[layer]->SetBinContent(i,j,1000.*hPixResBa3D[layer]->ProjectionZ(histname+"_zrms",i,i,j,j)->GetRMS());
				else
					hResMapBa[layer]->SetBinContent(i,j,1000.*hPixResBa3D[layer]->ProjectionZ(histname+"_zmean",i,i,j,j)->GetMean());
			}
		}
		cPIXResMap->cd(layer+4);
		hResMapBa[layer]->SetStats(kFALSE);
      	hResMapBa[layer]->GetXaxis()->SetTitle("eta/ring");
      	hResMapBa[layer]->GetYaxis()->SetTitle("phi/stave");
      	hResMapBa[layer]->SetMaximum(range);
     	hResMapBa[layer]->SetMinimum(-range);
     	if(isRMS)
     		hResMapBa[layer]->SetMinimum(0);
		hResMapBa[layer]->Draw("colz");
	}	
	
	// Endcap A
	
	TH2F *hPixResECA2D[iNPIX_Discs];
	TH1F *hResMapECA[iNPIX_Discs];

	
	for (int disc = 0; disc < iNPIX_Discs; disc++){
		histname = "IDAlignMon/"+TrkCol+"/Residuals/pix_eca_d";
		histname += disc;
		if(isXres)
			histname += "_biased_xresvsmodphi_2d";
		else
			histname += "_biased_yresvsmodphi_2d";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixResECA2D[disc] = (TH2F*)file->Get(histname);
		name = TrkCol.Data();
		name += ": ";
		if(isXres)
			name += "local x ";
		else
			name += "local y ";
		if(isRMS)
			name += "Residual RMS map - ";
		else
			name += "Residual Mean map - ";
		
		name+="Pixel ECA Disk ";
		name+=disc;
		hResMapECA[disc] = new TH1F(name+"Plot",name,hPixResECA2D[disc]->GetXaxis()->GetNbins(), 
										 hPixResECA2D[disc]->GetXaxis()->GetXmin(),
										 hPixResECA2D[disc]->GetXaxis()->GetXmax());
								
		for(int i = 1; i<=hPixResECA2D[disc]->GetXaxis()->GetNbins();i++){
			if(isRMS)	
				hResMapECA[disc]->SetBinContent(i,1000.*hPixResECA2D[disc]->ProjectionY("_py",i,i)->GetRMS());
			else
				hResMapECA[disc]->SetBinContent(i,1000.*hPixResECA2D[disc]->ProjectionY("_py",i,i)->GetMean());
		}
		cPIXResMap->cd(disc+1);
		hResMapECA[disc]->SetStats(kFALSE);
      	hResMapECA[disc]->SetMaximum(range);
     	hResMapECA[disc]->SetMinimum(-range);
     	if(isRMS)
     		hResMapECA[disc]->SetMinimum(0);
     	TBox *blank = new TBox(-10,-10, 10, 9.85);
	  	blank->SetFillColor(kWhite);
		if(isRMS)
			draw_wheel_frame(name,0,range);
		else
			draw_wheel_frame(name,-range,range);
		blank->Draw();
		gPad->SetRightMargin(0.20);
		gPad->SetLeftMargin(0.15);
		gPad->RedrawAxis();
		gPad->Update();
		setTitleFont(font);
		gPad->RedrawAxis();
		draw_pixel_ec_modules(hResMapECA[disc]);
     }		
     
     
     // Endcap C
	
	TH2F *hPixResECC2D[iNPIX_Discs];
	TH1F *hResMapECC[iNPIX_Discs];

	
	for (int disc = 0; disc < iNPIX_Discs; disc++){
		histname = "IDAlignMon/"+TrkCol+"/Residuals/pix_ecc_d";
		histname += disc;
		if(isXres)
			histname += "_biased_xresvsmodphi_2d";
		else
			histname += "_biased_yresvsmodphi_2d";
		if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
		hPixResECC2D[disc] = (TH2F*)file->Get(histname); 
		name = TrkCol.Data();
		name += ": ";
		if(isXres)
			name += "#local x ";
		else
			name += "#local y ";
		if(isRMS)
			name += "Residual RMS map - ";
		else
			name += "Residual Mean map - ";
		
		name+="Pixel ECC Disk ";
		name+=disc;
		hResMapECC[disc] = new TH1F(name+"Plot",name,hPixResECC2D[disc]->GetXaxis()->GetNbins(), 
										 hPixResECC2D[disc]->GetXaxis()->GetXmin(),
										 hPixResECC2D[disc]->GetXaxis()->GetXmax());
								
		for(int i = 1; i<=hPixResECC2D[disc]->GetXaxis()->GetNbins();i++){
			if(isRMS)	
				hResMapECC[disc]->SetBinContent(i,1000.*hPixResECC2D[disc]->ProjectionY("_py",i,i)->GetRMS());
			else
				hResMapECC[disc]->SetBinContent(i,1000.*hPixResECC2D[disc]->ProjectionY("_py",i,i)->GetMean());
		}
		cPIXResMap->cd(disc+7);
		hResMapECC[disc]->SetStats(kFALSE);
      	hResMapECC[disc]->SetMaximum(range);
     	hResMapECC[disc]->SetMinimum(-range);
     	if(isRMS)
     		hResMapECC[disc]->SetMinimum(0);
     	TBox *blank = new TBox(-10,-10, 10, 9.85);
	  	blank->SetFillColor(kWhite);
		if(isRMS)
			draw_wheel_frame(name,0,range);
		else
			draw_wheel_frame(name,-range,range);
		blank->Draw();
		gPad->SetRightMargin(0.20);
		gPad->SetLeftMargin(0.15);
		gPad->RedrawAxis();
		gPad->Update();
		setTitleFont(font);
		gPad->RedrawAxis();
		draw_pixel_ec_modules(hResMapECC[disc]);
     }	
}

//=====================================================================
// draw_wheel_frame()
//=====================================================================
void draw_wheel_frame(TString name, float minrange, float maxrange) {
  TH2F *PixFrame = new TH2F(name, name, 100, -10, 10, 100, -10, 10);
  PixFrame->SetStats(kFALSE);
  PixFrame->Fill(-5,5,maxrange);
  PixFrame->Fill(5,-5,minrange);
  // PixFrame->GetXaxis()->SetAxisColor(kWhite);
  // PixFrame->GetYaxis()->SetAxisColor(kWhite);
  PixFrame->GetZaxis()->SetLabelSize(0.035);  
  PixFrame->GetXaxis()->SetLabelColor(kWhite);
  PixFrame->GetYaxis()->SetLabelColor(kWhite);
  PixFrame->GetYaxis()->SetTitle("Y axis");
  PixFrame->GetXaxis()->SetTitle("X axis");
  PixFrame->GetYaxis()->SetTitleOffset(0.8);
  PixFrame->SetContour(NContLevels);
  PixFrame->SetMaximum(maxrange);
  PixFrame->SetMinimum(minrange);
  PixFrame->Draw("colz");
  SetZAxisTitle(PixFrame);

  gPad->Update();
}
