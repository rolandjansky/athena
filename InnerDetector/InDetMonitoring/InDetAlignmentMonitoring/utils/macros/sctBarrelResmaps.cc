/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void sctBarrelResmaps(TString TrkCol, bool side, bool isRMS, float range){
	sctBarrelResmaps(TrkCol, 0, side, isRMS, range);
}

void sctBarrelResmaps(TString TrkCol, TCanvas *cSCTBaResMap, bool side, bool isRMS, float range){
  if(!cSCTBaResMap)
    cSCTBaResMap = new TCanvas("cSCTBaResMap","SCT Barrel Resmap",
			       CanvasSizeX4[0], CanvasSizeX4[1]);
  
  
  
  TString histname;
  TString name;
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadLeftMargin(0.08);
  gStyle->SetPadTopMargin(0.12);
  gROOT->ForceStyle();
  
  
  TString title;
  
  ////////////////////
  // SCT Barrel     //
  ////////////////////
  
  TH3F *hSCTResBa3D[iNSCT_Layers];
  TH2F *hResMapBa[iNSCT_Layers];
  TGaxis *ATLZaxis[iNSCT_Layers];
  TGaxis *ATLPhiaxis[iNSCT_Layers];
  cSCTBaResMap->Divide(2,2);
  //------------
  // Barrel
  //------------
  cout << "	 - Sct Barrel..." << endl;
  
  for(int layer = 0; layer<iNSCT_Layers; layer++){
    histname = "IDAlignMon/"+TrkCol+"/Residuals/sct_b";
    histname += layer;
    if(side)
      histname += "_s1_biased_xresvsmodetaphi_3d";
    else
      histname += "_s0_biased_xresvsmodetaphi_3d";
    if(PrintLevel>0) cout << "Plotting Histogram: " << histname << endl;
    hSCTResBa3D[layer] = (TH3F*)file->Get(histname); 

    if (hSCTResBa3D[layer] == NULL) {// check if histogram exists
      std::cout << " <sctECHitmap> ** ERROR ** histogram " << histname << " is NOT AVAILABLE " << std::endl;
      return;
    }

    name.Clear();
    name = TrkCol.Data();
    name += ": ";
    name+="SCT Barrel Layer ";
    name+=layer;
    if(side)
      name += "Side 1 ";
    else
      name += "Side 0 ";
    if(isRMS)
      name += "Residual RMS map - ";
    else
      name += "Residual Mean map - ";
    
    hResMapBa[layer] = new TH2F(name,name,hSCTResBa3D[layer]->GetXaxis()->GetNbins()-1, 
				hSCTResBa3D[layer]->GetXaxis()->GetXmin()+1,
				hSCTResBa3D[layer]->GetXaxis()->GetXmax(),
				hSCTResBa3D[layer]->GetYaxis()->GetNbins(), 
				hSCTResBa3D[layer]->GetYaxis()->GetXmin(),
				hSCTResBa3D[layer]->GetYaxis()->GetXmax());
    for(int i = 1; i<=hSCTResBa3D[layer]->GetXaxis()->GetNbins();i++){
      for(int j = 1; j<=hSCTResBa3D[layer]->GetYaxis()->GetNbins();j++){
	if(i<7){
	  if(isRMS)	
	    hResMapBa[layer]->SetBinContent(i,j,1000.*hSCTResBa3D[layer]->ProjectionZ("_z",i,i,j,j)->GetRMS());
	  else
	    hResMapBa[layer]->SetBinContent(i,j,1000.*hSCTResBa3D[layer]->ProjectionZ("_z",i,i,j,j)->GetMean());
	}
	else if (i>7){
	  if(isRMS)	
	    hResMapBa[layer]->SetBinContent(i-1,j,1000.*hSCTResBa3D[layer]->ProjectionZ("_z",i,i,j,j)->GetRMS());
	  else
	    hResMapBa[layer]->SetBinContent(i-1,j,1000.*hSCTResBa3D[layer]->ProjectionZ("_z",i,i,j,j)->GetMean());
	}
	
      }
    }
    cSCTBaResMap->cd(layer+1);
    hResMapBa[layer]->SetStats(kFALSE);
    hResMapBa[layer]->GetXaxis()->SetTitle("eta/ring");
    hResMapBa[layer]->GetYaxis()->SetTitle("phi/stave");
    hResMapBa[layer]->SetMaximum(range);
    hResMapBa[layer]->SetMinimum(-range);
    SCTMaps_SetXaxisLabels(hResMapBa[layer]);
    if(isRMS)
      hResMapBa[layer]->SetMinimum(0);
    hResMapBa[layer]->Draw("colz");
    // Z axis on top (only for histos with 12 bins) 
    
    
		ATLZaxis[layer] = new TGaxis(-5.5,31.5,6.5,31.5,-800,800,510,"-");
		ATLZaxis[layer]->SetName("Z");
		if (layer == 0) {ATLZaxis[layer]->SetY1(31.5);ATLZaxis[layer]->SetY2(31.5);}
		if (layer == 1) {ATLZaxis[layer]->SetY1(39.5);ATLZaxis[layer]->SetY2(39.5);}
		if (layer == 2) {ATLZaxis[layer]->SetY1(47.5);ATLZaxis[layer]->SetY2(47.5);}
		if (layer == 3) {ATLZaxis[layer]->SetY1(55.5);ATLZaxis[layer]->SetY2(55.5);}
		ATLZaxis[layer]->SetLabelSize(hResMapBa[layer]->GetYaxis()->GetLabelSize());
		ATLZaxis[layer]->SetLabelOffset(0.0);
		ATLZaxis[layer]->SetLabelFont(hResMapBa[layer]->GetXaxis()->GetLabelFont());
		ATLZaxis[layer]->SetTitleSize(hResMapBa[layer]->GetXaxis()->GetTitleSize());
		ATLZaxis[layer]->SetTitleFont(hResMapBa[layer]->GetXaxis()->GetTitleFont());
		ATLZaxis[layer]->SetTitle(" z (mm)");
		ATLZaxis[layer]->Draw();
		
		// moving the color scale
		gPad->Update();
		TPaletteAxis *palette = 
	(TPaletteAxis*)hResMapBa[layer]->GetListOfFunctions()->FindObject("palette");
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
		ATLPhiaxis[layer]->SetLabelSize(hResMapBa[layer]->GetYaxis()->GetLabelSize());
		ATLPhiaxis[layer]->SetLabelOffset(0.0);
		ATLPhiaxis[layer]->SetLabelFont(hResMapBa[layer]->GetXaxis()->GetLabelFont());
		ATLPhiaxis[layer]->SetTitleSize(hResMapBa[layer]->GetXaxis()->GetTitleSize());
		ATLPhiaxis[layer]->SetTitleFont(hResMapBa[layer]->GetXaxis()->GetTitleFont());
		ATLPhiaxis[layer]->SetTitle("#phi (deg)");
		ATLPhiaxis[layer]->SetLabelOffset(0.01);
		ATLPhiaxis[layer]->Draw();

		
	}	
	
}
