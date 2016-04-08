/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

const int iResNPIX_Layers = 3;
const int iResNPIX_Discs  = 3;
const int iResNSCT_Layers = 4;
const int iResNSCT_Sides  = 2; // el modul del SCT te dos cares
const int iResNSCT_Discs  = 9;

float MaxMeanRange;
float MinMeanRange;
const Int_t NContLevels = 50;
Int_t RM_ColorIndex[NContLevels];
float RM_Levels[NContLevels];


//=====================================================================
// resmaps()
//=====================================================================
void resmaps(float MeanRange=50,float SigmaRangeMax=50,float SigmaRangeMin=10) {
  
  cout << " Residual maps" << endl;

  MaxMeanRange = MeanRange;
  MinMeanRange = -MeanRange;

  Set_ColorScale();
  // Print_ColorScale();

  TH2F *HD[iResNSCT_Discs];

  char nameh[80];
  char xCommand[80];

  Int_t PadIndex = 0;

  ////////////////////
  // SCT           //
  ////////////////////

  if (!PixelOnly && iSCTHits>0) {

    // Loop over the SCT sides
    for (Int_t side=0; side<2; side++) {
      
      /////////////////////
      // Barrel
      /////////////////////
      cout << "  - SCT Barrel (Both) - Side" << side << "..." << endl;

      // mitjana
      if (side==0) {
	cResMapMeanSCTSide0 = new TCanvas("cResMapMeanSCTSide0","Residuals Mean maps for SCT (Side 0)",
					  CanvasSizeX4[0],CanvasSizeX4[1]);
	cResMapMeanSCTSide0->Divide(2,2);
      }
      else {
	cResMapMeanSCTSide1 = new TCanvas("cResMapMeanSCTSide1","Residuals Mean maps for SCT (Side 1)",
					  CanvasSizeX4[0],CanvasSizeX4[1]);
	cResMapMeanSCTSide1->Divide(2,2);
      }
      
      for (int layer = 0; layer < iResNSCT_Layers; layer++){
	sprintf(nameh,"trkana/SCTRes/hSCTResMeanMapBaLay%d_s%d_%d",layer,side,level);
	HD[layer] = (TH2F*)file->Get(nameh); 
	HD[layer]->Scale(1000.);
	HD[layer]->SetContour(NContLevels);
	HD[layer]->SetMaximum(MaxMeanRange);
	HD[layer]->SetMinimum(MinMeanRange);
	HD[layer]->SetStats(kFALSE);
	HD[layer]->GetXaxis()->SetTitle("eta/ring");
	HD[layer]->GetYaxis()->SetTitle("phi/stave");
	sprintf(name,"Residual map (Mean) - SCT Barrel Layer %d (Side %d)",layer,side);
	HD[layer]->SetTitle(name);
	if (side==0) cResMapMeanSCTSide0->cd(layer+1);
	else cResMapMeanSCTSide1->cd(layer+1); 
	HD[layer]->Draw("colz");
	gPad->SetRightMargin(0.13);
	SetZAxisTitle(HD[layer],"res (#mum)",0.89,0.95);
	gPad->Update();
	setTitleFont(font);
      }
      
      // sigma
      if (side==0) {
	cResMapSigmaSCTSide0 = new TCanvas("cResMapSigmaSCTSide0","Residuals Sigma maps for SCT (Side 0)",
					  CanvasSizeX4[0],CanvasSizeX4[1]);
	cResMapSigmaSCTSide0->Divide(2,2);
      }
      else {
	cResMapSigmaSCTSide1 = new TCanvas("cResMapSigmaSCTSide1","Residuals Sigma maps for SCT (Side 1)",
					   CanvasSizeX4[0],CanvasSizeX4[1]);
	cResMapSigmaSCTSide1->Divide(2,2);
      }

      for (int layer = 0; layer < iResNSCT_Layers; layer++){
	sprintf(nameh,"trkana/SCTRes/hSCTResSigmaMapBaLay%d_s%d_%d",layer,side,level);
	HD[layer] = (TH2F*)file->Get(nameh); 
	HD[layer]->Scale(1000.);
	HD[layer]->SetContour(SigmaRangeMax);
	HD[layer]->SetMaximum(SigmaRangeMax);
	HD[layer]->SetMinimum(SigmaRangeMin);
	HD[layer]->SetStats(kFALSE);
	HD[layer]->GetXaxis()->SetTitle("eta/ring");
	HD[layer]->GetYaxis()->SetTitle("phi/stave");
	HD[layer]->SetMinimum(0.0);
	sprintf(name,"Residual map (Sigma) - SCT Barrel Layer %d (Side %d)",layer,side);
	HD[layer]->SetTitle(name);
	if (side==0) cResMapSigmaSCTSide0->cd(layer+1);
	else cResMapSigmaSCTSide1->cd(layer+1);
	HD[layer]->Draw("colz");
	gPad->SetRightMargin(0.13);
	SetZAxisTitle(HD[layer],"res (#mum)",0.89,0.95);
	gPad->Update();
	setTitleFont(font);
      }
      
      if (!SCTBarrelOnly) {

	// drawing disks (a la Sergio)
	for (Int_t isMean=0; isMean<2; isMean++) {

	  /////////////////////
	  // SCT Endcap A
	  /////////////////////
	  if (isMean==0) cout << "  - SCT EndCapA (Mean) - Side" << side << "..." << endl;
	  else cout << "  - SCT EndCapA (Sigma) - Side" << side << "..." << endl;

	  char namecan[80];

	  if (isMean==0) {
	    sprintf(namecan,"Residuals map (Mean) - SCT EndCap A (Side %d)",side);
	    if (side==0) {
	      cResMapMeanSCTEcASide0 = new TCanvas("cResMapMeanSCTEcASide0",namecan,
						   CanvasSizeX9[0],CanvasSizeX9[1],
						   CanvasSizeX9[2],CanvasSizeX9[3]);
	      cResMapMeanSCTEcASide0->Divide(3,3);
	    }
	    else {
	      cResMapMeanSCTEcASide1 = new TCanvas("cResMapMeanSCTEcASide1",namecan,
						   CanvasSizeX9[0],CanvasSizeX9[1],
						   CanvasSizeX9[2],CanvasSizeX9[3]);
	      cResMapMeanSCTEcASide1->Divide(3,3);
	    }
	  }
	  else {
	    sprintf(namecan,"Residuals map (Sigma) - SCT EndCap A (Side %d)",side);
	    if (side==0) {
	      cResMapSigmaSCTEcASide0 = new TCanvas("cResMapSigmaSCTEcASide0",namecan,
						    CanvasSizeX9[0],CanvasSizeX9[1],
						    CanvasSizeX9[2],CanvasSizeX9[3]);
	      cResMapSigmaSCTEcASide0->Divide(3,3);
	    }
	    else {
	      cResMapSigmaSCTEcASide1 = new TCanvas("cResMapSigmaSCTEcASide1",namecan,
						    CanvasSizeX9[0],CanvasSizeX9[1],
						    CanvasSizeX9[2],CanvasSizeX9[3]);
	      cResMapSigmaSCTEcASide1->Divide(3,3);
	    }
	  }

	  for (int disc = 0; disc < iResNSCT_Discs; disc++){
	    if (isMean==0) sprintf(nameh,"trkana/SCTRes/hSCTResMeanMapEaLay%d_s%d_%d",disc,side,level);
	    else sprintf(nameh,"trkana/SCTRes/hSCTResSigmaMapEaLay%d_s%d_%d",disc,side,level);
	    HD[disc] = (TH2F*)file->Get(nameh);
	    HD[disc]->Scale(1000.);
	    HD[disc]->SetContour(NContLevels);
	    HD[disc]->SetMaximum(MaxMeanRange);
	    if (isMean==0) HD[disc]->SetMinimum(MinMeanRange);
	    else HD[disc]->SetMinimum(0.0);
	    // HD[disc]->SetStats(kFALSE);
	    // HD[disc]->Draw("colztext");
	  }
  
	  TBox *blank = new TBox(-10,-10, 10, 9.85);
	  blank->SetFillColor(kWhite);

	  PadIndex=0;

	  for(int disc=0; disc<iResNSCT_Discs; disc++){
	    if (1==0) cout << "  << EndCap A >> disc " << disc << endl;
	    PadIndex++;
	    if (isMean==0) {
	      if (side==0) cResMapMeanSCTEcASide0->cd(PadIndex);
	      else cResMapMeanSCTEcASide1->cd(PadIndex);
	      draw_wheel_frame("Residual map (Mean) SCT","A",disc,level,isMean,side);
	    }
	    else {
	      if (side==0) cResMapSigmaSCTEcASide0->cd(PadIndex);
	      else cResMapSigmaSCTEcASide1->cd(PadIndex);
	      draw_wheel_frame("Residual map (Sigma) SCT","A",disc,level,isMean,side);
	    }
	    blank->Draw();
	    gPad->SetRightMargin(0.20);
	    gPad->SetLeftMargin(0.15);
	    gPad->RedrawAxis();
 	    gPad->Update();
	    setTitleFont(font);
	    draw_sct_ec_modules(HD[disc], disc);
	  }

	  /////////////////////
	  // SCT Endcap C
	  /////////////////////
	  if (isMean==0) cout << "  - SCT EndCapC (Mean) - Side" << side << "..." << endl;
	  else cout << "  - SCT EndCapC (Sigma) - Side" << side << "..." << endl;

	  char namecan[80];

	  if (isMean==0) {
	    sprintf(namecan,"Residual map (Mean) - SCT EndCap C (Side %d)",side);
	    if (side==0) {
	      cResMapMeanSCTEcCSide0 = new TCanvas("cResMapmeanSCTEcCSide0",namecan,
						   CanvasSizeX9[0],CanvasSizeX9[1],
						   CanvasSizeX9[2],CanvasSizeX9[3]);
	      cResMapMeanSCTEcCSide0->Divide(3,3);
	    }
	    else {
	      cResMapMeanSCTEcCSide1 = new TCanvas("cResMapMeanSCTEcCSide1",namecan,
						   CanvasSizeX9[0],CanvasSizeX9[1],
						   CanvasSizeX9[2],CanvasSizeX9[3]);
	      cResMapMeanSCTEcCSide1->Divide(3,3);
	    }
	  }
	  else {
	    sprintf(namecan,"Residual map (Sigma) - SCT EndCap C (Side %d)",side);
	    if (side==0) {
	      cResMapSigmaSCTEcCSide0 = new TCanvas("cResMapSigmaSCTEcCSide0",namecan,
						   CanvasSizeX9[0],CanvasSizeX9[1],
						   CanvasSizeX9[2],CanvasSizeX9[3]);
	      cResMapSigmaSCTEcCSide0->Divide(3,3);
	    }
	    else {
	      cResMapSigmaSCTEcCSide1 = new TCanvas("cResMapSigmaSCTEcCSide1",namecan,
						   CanvasSizeX9[0],CanvasSizeX9[1],
						   CanvasSizeX9[2],CanvasSizeX9[3]);
	      cResMapSigmaSCTEcCSide1->Divide(3,3);
	    }
	  }

	  for (int disc = iResNSCT_Discs-1; disc>=0; disc--){
	    if (isMean==0) sprintf(nameh,"trkana/SCTRes/hSCTResMeanMapEcLay%d_s%d_%d",disc,side,level);
	    else sprintf(nameh,"trkana/SCTRes/hSCTResSigmaMapEcLay%d_s%d_%d",disc,side,level);
	    HD[disc] = (TH2F*)file->Get(nameh); 
	    HD[disc]->Scale(1000.);
	    HD[disc]->SetContour(NContLevels);
	    HD[disc]->SetMaximum(MaxMeanRange);
	    if (isMean==0) HD[disc]->SetMinimum(MinMeanRange);
	    else HD[disc]->SetMinimum(0.0);
	    // HD[disc]->SetStats(kFALSE);
	    // HD[disc]->Draw("colz");
	  }

	  TBox *blank = new TBox(-10,-10, 10, 9.85);
	  blank->SetFillColor(kWhite);

	  PadIndex=0;

	  for(int disc=iResNSCT_Discs-1; disc>=0; disc--){
	    if (1==0) cout << "  << EndCap C >> disc " << disc << endl;
	    PadIndex++;
	    if (isMean==0) {
	      if (side==0) cResMapMeanSCTEcCSide0->cd(PadIndex);            
	      else cResMapMeanSCTEcCSide1->cd(PadIndex);
	      draw_wheel_frame("Residual map (Mean) - SCT","C",disc,level,isMean,side);
	    }
	    else {
	      if (side==0) cResMapSigmaSCTEcCSide0->cd(PadIndex);            
	      else cResMapSigmaSCTEcCSide1->cd(PadIndex);
	      draw_wheel_frame("Residual map (Sigma) - SCT","C",disc,level,isMean,side);
	    }
	    blank->Draw();
	    gPad->SetRightMargin(0.20);
	    gPad->SetLeftMargin(0.15);
	    gPad->RedrawAxis();
 	    gPad->Update();
	    draw_sct_ec_modules(HD[disc], disc);
	  }
	} // end: is Mean or Sigma?
    
      } //  barrel only

    } // loop over SCT sides end

  }
  
  ////////////////////
  // PIX           //
  ////////////////////
  if (!SCTOnly && iPixelHits>0) {
    /////////////////////
    // Barrel
    /////////////////////    
    cout << "  - Pixel Barrel: mean..."; 

    // mitjana
    cResMapMeanPix = new TCanvas("cResMeanPix","Residual Mean maps for Pixels",
				 CanvasSizeX9[0],CanvasSizeX9[1],
				 CanvasSizeX9[2],CanvasSizeX9[3]);
    cResMapMeanPix->Divide(3,3);
    
    for (int layer = 0; layer < iResNPIX_Layers; layer++){
      sprintf(nameh,"trkana/PIXRes/hPIXResMeanMapBaLay%d_%d",layer,level);
      HD[layer] = (TH2F*)file->Get(nameh); 
      HD[layer]->Scale(1000.);
      HD[layer]->SetContour(NContLevels);
      HD[layer]->SetMaximum(MaxMeanRange);
      HD[layer]->SetMinimum(MinMeanRange);
      HD[layer]->SetStats(kFALSE);
      HD[layer]->GetXaxis()->SetTitle("eta/ring");
      HD[layer]->GetYaxis()->SetTitle("phi/stave");
      sprintf(name,"Residual map (Mean) - Pixel Barrel Layer %d",layer);
      HD[layer]->SetTitle(name);
      HD[layer]->GetYaxis()->SetTitleOffset(1.0);
      cResMapMeanPix->cd(layer+1+3);
      HD[layer]->Draw("colz");
      gPad->SetRightMargin(0.13);
      SetZAxisTitle(HD[layer],"res (#mum)",0.89,0.95);
      gPad->Update();
      setTitleFont(font);
    }
    cout << " done" 
	 << endl 
	 << "  - Pixel Barrel: sigma..."; 
    
    // sigma
    cResMapSigmaPix = new TCanvas("cResMapSigmaPix","Residual Sigma maps for Pixels",
				  CanvasSizeX9[0],CanvasSizeX9[1],
				  CanvasSizeX9[2],CanvasSizeX9[3]);
    cResMapSigmaPix->Divide(3,3);
    
    for (int layer = 0; layer < iResNPIX_Layers; layer++){
      sprintf(nameh,"trkana/PIXRes/hPIXResSigmaMapBaLay%d_%d",layer,level);
      HD[layer] = (TH2F*)file->Get(nameh); 
      HD[layer]->Scale(1000.);
      HD[layer]->SetContour(SigmaRangeMax);
      HD[layer]->SetMaximum(SigmaRangeMax);
      HD[layer]->SetMinimum(0.0);
      // HD[layer]->SetMinimum(SigmaRangeMin);
      HD[layer]->SetStats(kFALSE);
      HD[layer]->GetXaxis()->SetTitle("eta/ring");
      HD[layer]->GetYaxis()->SetTitle("phi/stave");
      sprintf(name,"Residual map (Sigma) - Pixel Barrel Layer %d",layer);
      HD[layer]->SetTitle(name);
      HD[layer]->GetYaxis()->SetTitleOffset(1.0);
      cResMapSigmaPix->cd(layer+1+3);
      HD[layer]->Draw("colz");
      gPad->SetRightMargin(0.13);
      SetZAxisTitle(HD[layer],"res (#mum)",0.89,0.95);

      gPad->Update();
      setTitleFont(font);
    }
    cout << " done" << endl; 

    if (!PixelBarrelOnly) {
      // drawing disks (a la Sergio)

      for (Int_t isMean=0; isMean<2; isMean++) {

	/////////////////////
	// Pixel Endcap A
	/////////////////////
	if (isMean==0) cout << "  - Pixel EndCapA (Mean)..." << endl;
	else cout << "  - Pixel EndCapA (Sigma)..." << endl;

	for (int layer = 0; layer < iResNPIX_Layers; layer++){
	  if (isMean==0) sprintf(nameh,"trkana/PIXRes/hPIXResMeanMapEaLay%d_%d",layer,level);
	  else sprintf(nameh,"trkana/PIXRes/hPIXResSigmaMapEaLay%d_%d",layer,level);
	  HD[layer] = (TH2F*)file->Get(nameh); 
	  HD[layer]->Scale(1000.);
	  HD[layer]->SetContour(NContLevels);
	  HD[layer]->SetMaximum(MaxMeanRange);
	  if (isMean==0) HD[layer]->SetMinimum(MinMeanRange);
	  else HD[layer]->SetMinimum(0.0);
	  // HD[layer]->SetStats(kFALSE);
	  // cResMapMeanPix->cd(disc+1);
	  // HD[layer]->Draw("colztext");
	}

	TBox *blank = new TBox(-10,-10, 10, 9.85);
	blank->SetFillColor(kWhite);

	PadIndex=0;

	for(int disc=0; disc<iResNPIX_Discs; disc++){
	  if (1==0) cout << "  << EndCap A >> disc " << disc << endl;
	  PadIndex++;
	  if (isMean==0) {
	    cResMapMeanPix->cd(disc+1);
	    draw_wheel_frame("Residual map (Mean) - Pixel","A",disc,level,isMean,-1);
	  }
	  else {
	    cResMapSigmaPix->cd(disc+1);
	    draw_wheel_frame("Residual map (Sigma) - Pixel","A",disc,level,isMean,-1);
	  }
	  blank->Draw();
	  gPad->SetRightMargin(0.20);
	  gPad->SetLeftMargin(0.15);
	  gPad->RedrawAxis();
	  gPad->Update();
	  setTitleFont(font);
	  gPad->RedrawAxis();
	  draw_pixel_ec_modules(HD[disc]);
	}

	/////////////////////
	// Pixel Endcap C
	/////////////////////
	if (isMean==0) cout << "  - Pixel EndCapC (Mean)..." << endl;
	else cout << "  - Pixel EndCapC (Sigma)..." << endl;

	for (int layer = iResNPIX_Discs-1; layer >= 0; layer--){
	  if (isMean==0) sprintf(nameh,"trkana/PIXRes/hPIXResMeanMapEcLay%d_%d",layer,level);
	  else sprintf(nameh,"trkana/PIXRes/hPIXResSigmaMapEcLay%d_%d",layer,level);
	  HD[layer] = (TH2F*)file->Get(nameh); 
	  HD[layer]->Scale(1000.);
	  HD[layer]->SetContour(NContLevels);
	  HD[layer]->SetMaximum(MaxMeanRange);
	  if (isMean==0) HD[layer]->SetMinimum(MinMeanRange);
	  else HD[layer]->SetMinimum(0.0);
	  // HD[layer]->SetStats(kFALSE);
	  // cResMapMeanPix->cd(disc+1+6);
	  // HD[layer]->Draw("colztext");
	}

	TBox *blank = new TBox(-10,-10, 10, 9.85);
	blank->SetFillColor(kWhite);

	PadIndex=0;

	for(int disc=iResNPIX_Discs-1; disc>=0; disc--){
	  if (1==0) cout << "  << EndCap C >> disc " << disc << endl;
	  PadIndex++;
	  if (isMean==0) {
	    cResMapMeanPix->cd(disc+1+6);
	    draw_wheel_frame("Residual map (Mean) - Pixel","C",disc,level,isMean,-1);
	  }
	  else {
	    cResMapSigmaPix->cd(disc+1+6);
	    draw_wheel_frame("Residual map (Sigma) - Pixel ","C",disc,level,isMean,-1);
	  }
	  blank->Draw();
	  gPad->SetRightMargin(0.20);
	  gPad->SetLeftMargin(0.15);
	  gPad->RedrawAxis();
	  gPad->Update();
	  setTitleFont(font);
	  gPad->RedrawAxis();
	  draw_pixel_ec_modules(HD[disc]);
	}    
      }

    } // end if barrel only

    return;

    //
    // ====================================================================================
    //

    // Pixel Eta residuals
    
    // First one should chek the histograms exist
    if (file->FindObjectAny("hPIXEtaResMeanMapBaLay0_0")) {
      cout << "  - PIX eta residual map: average... ";
      
      /////////////////////
      // Pixel Barrel
      /////////////////////
      // mitjana
      cEtaResMapMeanPix = new TCanvas("cEtaResMeanPix","Eta Residual Mean maps for Pixels",
				      CanvasSizeX9[0],CanvasSizeX9[1],
				      CanvasSizeX9[2],CanvasSizeX9[3]);
      cEtaResMapMeanPix->Divide(3,3);
      
      for (int layer = 0; layer < iResNPIX_Layers; layer++){
	sprintf(nameh,"trkana/PIXRes/hPIXEtaResMeanMapBaLay%d_%d",layer,level);
	HD[layer] = (TH2F*)file->Get(nameh); 
	HD[layer]->Scale(1000.);
	HD[layer]->SetContour(NContLevels);
	HD[layer]->SetMaximum(MaxMeanRange);
	HD[layer]->SetMinimum(MinMeanRange);
	HD[layer]->SetStats(kFALSE);
	HD[layer]->GetXaxis()->SetTitle("eta/ring");
	HD[layer]->GetYaxis()->SetTitle("phi/stave");
	sprintf(name,"Eta Residual map (Mean) - Pixel Barrel Layer %d",layer);
	HD[layer]->SetTitle(name);
	cEtaResMapMeanPix->cd(layer+1+3);
	HD[layer]->Draw("colz");
	gPad->SetRightMargin(0.13);
	SetZAxisTitle(HD[layer],"res (#mum)",0.89,0.95);
	gPad->Update();
	setTitleFont(font);
      }
      
      cout << " done " 
	   << endl
	   << "  - PIX eta residual map: sigma... ";
      
      // sigma
      cEtaResMapSigmaPix = new TCanvas("cEtaResMapSigmaPix","Eta Residual Sigma maps for Pixels",
				       CanvasSizeX9[0],CanvasSizeX9[1],
				       CanvasSizeX9[2],CanvasSizeX9[3]);
      cEtaResMapSigmaPix->Divide(3,3);
      
      for (int layer = 0; layer < iResNPIX_Layers; layer++){
	sprintf(nameh,"trkana/PIXRes/hPIXEtaResSigmaMapBaLay%d_%d",layer,level);
	HD[layer] = (TH2F*)file->Get(nameh); 
	HD[layer]->Scale(1000.);
	HD[layer]->SetContour(SigmaRangeMax);
	HD[layer]->SetMaximum(SigmaRangeMax);
	HD[layer]->SetMinimum(SigmaRangeMin);
	HD[layer]->SetStats(kFALSE);
	HD[layer]->GetXaxis()->SetTitle("eta/ring");
	HD[layer]->GetYaxis()->SetTitle("phi/stave");
	sprintf(name,"Eta Residual map (Sigma) - Pixel Barrel Layer %d",layer);
	HD[layer]->SetTitle(name);
	cEtaResMapSigmaPix->cd(layer+1+3);
	HD[layer]->Draw("colz");
	gPad->SetRightMargin(0.13);
	SetZAxisTitle(HD[layer],"res (#mum)",0.89,0.95);
	gPad->Update();
	setTitleFont(font);
      }
      cout << " done " 
	   << endl;
      
      if (!PixelBarrelOnly) {
	for (Int_t isMean=0; isMean<2; isMean++) {  // isMean switches between mean and sigma histos
	  /////////////////////
	  // Pixel Endcap A
	  /////////////////////
	  if (isMean==0) cout << "  - Pixel EndCapA (Mean)..." << endl;
	  else cout << "  - Pixel EndCapA (Sigma)..." << endl;
	  
	  for (int layer = 0; layer < iResNPIX_Layers; layer++){
	    if (isMean==0) sprintf(nameh,"trkana/PIXRes/hPIXEtaResMeanMapEaLay%d_%d",layer,level);
	    else sprintf(nameh,"trkana/PIXRes/hPIXEtaResSigmaMapEaLay%d_%d",layer,level);
	    HD[layer] = (TH2F*)file->Get(nameh); 
	    HD[layer]->Scale(1000.);
	    HD[layer]->SetContour(NContLevels);
	    HD[layer]->SetMaximum(MaxMeanRange);
	    if (isMean==0) HD[layer]->SetMinimum(MinMeanRange);
	    else HD[layer]->SetMinimum(0.0);
	  }
	  
	  TBox *blank = new TBox(-10,-10, 10, 9.85);
	  blank->SetFillColor(kWhite);
	  
	  PadIndex=0;
	  
	  for(int disc=0; disc<iResNPIX_Discs; disc++){
	    if (1==0) cout << "  << EndCap A >> disc " << disc << endl;
	    PadIndex++;
	    if (isMean==0) {
	      cEtaResMapMeanPix->cd(disc+1);
	      draw_wheel_frame("Eta Residual map (Mean) - Pixel","A",disc,level,isMean,-1);
	    }
	    else {
	      cEtaResMapSigmaPix->cd(disc+1);
	      draw_wheel_frame("Eta Residual map (Sigma) - Pixel","A",disc,level,isMean,-1);
	    }
	    blank->Draw();
	    gPad->SetRightMargin(0.20);
	    gPad->SetLeftMargin(0.15);
	    gPad->RedrawAxis();
	    gPad->Update();
	    setTitleFont(font);
	    gPad->RedrawAxis();
	    sprintf(xCommand,".x ../trkana/macros/DrawPIXECModules.C(HD[%d])",disc);
	    gROOT->ProcessLine(xCommand);
	  }

	  /////////////////////
	  // Pixel Endcap C
	  /////////////////////
	  if (isMean==0) cout << "  - Pixel EndCapC (Mean)..." << endl;
	  else cout << "  - Pixel EndCapC (Sigma)..." << endl;
	  
	  for (int layer = iResNPIX_Discs-1; layer >= 0; layer--){
	    if (isMean==0) sprintf(nameh,"trkana/PIXRes/hPIXEtaResMeanMapEcLay%d_%d",layer,level);
	    else sprintf(nameh,"trkana/PIXRes/hPIXEtaResSigmaMapEcLay%d_%d",layer,level);
	    HD[layer] = (TH2F*)file->Get(nameh); 
	    HD[layer]->Scale(1000.);
	    HD[layer]->SetContour(NContLevels);
	    HD[layer]->SetMaximum(MaxMeanRange);
	    if (isMean==0) HD[layer]->SetMinimum(MinMeanRange);
	    else HD[layer]->SetMinimum(0.0);
	  }
	  
	  TBox *blank = new TBox(-10,-10, 10, 9.85);
	  blank->SetFillColor(kWhite);
	  
	  PadIndex=0;
	  
	  for(int disc=iResNPIX_Discs-1; disc>=0; disc--){
	    if (1==0) cout << "  << EndCap C >> disc " << disc << endl;
	    PadIndex++;
	    if (isMean==0) {
	      cEtaResMapMeanPix->cd(disc+1+6);
	      draw_wheel_frame("Eta Residual map (Mean) - Pixel","C",disc,level,isMean,-1);
	    }
	    else {
	      cEtaResMapSigmaPix->cd(disc+1+6);
	      draw_wheel_frame("Eta Residual map (Sigma) - Pixel ","C",disc,level,isMean,-1);
	    }
	    blank->Draw();
	    gPad->SetRightMargin(0.20);
	    gPad->SetLeftMargin(0.15);
	    gPad->RedrawAxis();
	    gPad->Update();
	    setTitleFont(font);
	    gPad->RedrawAxis();
	    sprintf(xCommand,".x ../trkana/macros/DrawPIXECModules.C(HD[%d])",disc);
	    gROOT->ProcessLine(xCommand);
	  }    
	} // End of loops in mean and sigma
	
      } // End of drawing EndCaps
      
    } // End of Eta residuals
    
  }// End of PIX
  
  return;
}


//=====================================================================
// draw_wheel_frame()
//=====================================================================
void draw_wheel_frame(const char detec[3], const char endcap[1], 
		      Int_t disc, Int_t level, Int_t isMean, Int_t side) {
  char nameh[80];
  
  if (side==-1)   sprintf(nameh,"%s EndCap%c Disk %d",detec,endcap[0],disc);
  else sprintf(nameh,"%s EndCap%c Disk %d - Side %d",detec,endcap[0],disc,side);


  TH2F *PixFrame = new TH2F(nameh, nameh, 100, -10, 10, 100, -10, 10);
  PixFrame->SetStats(kFALSE);
  PixFrame->Fill(-5,5,MaxMeanRange);
  PixFrame->Fill(5,-5,MinMeanRange);
  // PixFrame->GetXaxis()->SetAxisColor(kWhite);
  // PixFrame->GetYaxis()->SetAxisColor(kWhite);
  PixFrame->GetZaxis()->SetLabelSize(0.035);  
  PixFrame->GetXaxis()->SetLabelColor(kWhite);
  PixFrame->GetYaxis()->SetLabelColor(kWhite);
  PixFrame->GetYaxis()->SetTitle("Y axis");
  PixFrame->GetXaxis()->SetTitle("X axis");
  PixFrame->GetYaxis()->SetTitleOffset(0.8);
  PixFrame->SetContour(NContLevels);
  PixFrame->SetMaximum(MaxMeanRange);
  if (isMean==0)  PixFrame->SetMinimum(MinMeanRange);
  else PixFrame->SetMinimum(0.0);
  PixFrame->Draw("colz");
  SetZAxisTitle(PixFrame,"res (#mum)");

  gPad->Update();
}


//=====================================================================
// locateColor()
//=====================================================================
int locateColor(const int ncols, float min, float max, float cont) {
  int res;
  // First we should find the Z bin:
  for (int zbin=10; zbin<NContLevels; zbin++) {
    if (cont>RM_Levels[zbin]) {
      res = RM_ColorIndex[zbin];
    }
  }
  // cout << "--> final color for " << cont << " is " << res << endl;

  // out ouf range
  if (cont<min) {
    if (1==0) cout << " --> Value " << cont << " is out of range: " << cont << " < " << min << endl;
    res = -10;  
  }
  if (cont>max) {
    if (1==0) cout << " --> Value " << cont << " is out of range: " << cont << " > " << max << endl;
    res = kWhite;  
  }

  return res;
}


//=====================================================================
// Set_ColorScale()
//=====================================================================
void Set_ColorScale() {
  for (int i=0; i<NContLevels;i++) {
    RM_ColorIndex[i] = i;
    if (1==0)cout << " RM_ColorIndex["<<i<<"] = " << RM_ColorIndex[i] << endl;
  }

  // compute the Z levels
  float step = (MaxMeanRange-MinMeanRange)/NContLevels;
  RM_Levels[0] = MinMeanRange;
  for (int i=1; i<NContLevels; i++) {
    RM_Levels[i]=RM_Levels[i-1]+step;
  }
  if (1==0) {
    cout << "  -- Levels -- " << endl;
    for (int i=0; i<NContLevels; i++) {
      cout << " L " << i << " -- " << RM_Levels[i] << endl;
    }
  }
}


//=====================================================================
// Print_ColorScale()
//=====================================================================
void Print_ColorScale() {

  cout << " --- Printing Color Scale ---- " << endl
       << "  Ncolors = " << NContLevels << endl;
    
  for (int i=0; i<NContLevels; i++) {
    float Lmin, Lmax;
    Lmin = RM_Levels[i];
    if (i==(NContLevels-1)){
      Lmax = MaxMeanRange;
    } 
    else {
      Lmax = RM_Levels[i+1];
    }

    cout << "  Level " << i << "  Range " << Lmin <<" -> " << Lmax << " color " << RM_ColorIndex[i] << endl;
  }

  return;
}


//////////////////////////////////////////////////////
void draw_pixel_ec_modules(TH2F *histo){
  //////////////////////////////////////////////////////
  const int npixmods=48;  
  const float min = histo->GetMinimum();
  const float max = histo->GetMaximum();

  TPolyLine *p[npixmods];
  double x[5], y[5];
  double u[5], v[5];

  const float hxl = 1;
  const float hxh = 3;
  const float hy  = 4;
  const float phistep = 2*TMath::Pi()/npixmods;
  const float inr =  5; 
  const float our = 10; 

  for(int i=0; i<npixmods; i++){
    for(int j=0; j<5; j++){
      x[j]=0;
      y[j]=0;
    }

    float phi = -i*phistep;

    x[0] = inr*cos(phi-phistep/2);
    x[1] = inr*cos(phi+phistep/2);
    x[2] = our*cos(phi+phistep/2);
    x[3] = our*cos(phi-phistep/2);
    x[4] = inr*cos(phi-phistep/2);

    y[0] = inr*sin(phi-phistep/2);
    y[1] = inr*sin(phi+phistep/2);
    y[2] = our*sin(phi+phistep/2);
    y[3] = our*sin(phi-phistep/2);
    y[4] = inr*sin(phi-phistep/2);

    int col_index = locateColor(gStyle->GetNumberOfColors(), 
				min, max, 
				histo->GetBinContent(1,i+1));
    p[i] = new TPolyLine(5,x,y);
    p[i]->SetFillColor(gStyle->GetColorPalette(col_index));
  }

  for(int i=0; i<npixmods; i++)
    p[i]->Draw("Fsame");
  setTitleFont(font);

}


/////////////////////////////////////////////////////////////////////
int locateColor(const int ncols, float min, float max, float cont){
  /////////////////////////////////////////////////////////////////////
  float val[100];
  float step = (max-min)/ncols;
  int res;

  val[0] = min + step/2;
  for(int i=1; i<ncols; i++)
    val[i] = val[i-1] + step;

  if(cont<=val[0])
    res = 0;
  else if(cont>=val[ncols-1])
    res = ncols-1;
  else{
    for(int i=1; i<ncols; i++){
      if(cont>=val[i-1] && cont<val[i]){
	res = i;
	break;
      }    
    }
  }
  return res;
}

void draw_sct_ec_modules(TH2F *histo, int disc) {
//////////////////////////////////////////////////////
  /*  Number of rings (i.e, module type) in each disk. 
      Values mean:
      1 : outers
      2 : outers + middles
      3 : outers + middles + inners       
  */

  const int nrings[9]={2,3,3,3,3,3,2,2,1};
  
  // number of modules in each ring
  const int nmods[3]={52,40,40};

  float inner_radius[3]={ 7.5, 4.9, 3.4};
  float outer_radius[3]={10.0, 7.4, 4.8};
  // the one before the last wheel has short middle modules
  if (disc==7) {
    inner_radius[1]=6.0;
  }

  TPolyLine *p[52];
  double x[5], y[5];
  double u[5], v[5];

  const float hxl = 1;
  const float hxh = 3;
  const float hy  = 4;

  const float min = histo->GetMinimum();
  const float max = histo->GetMaximum();

  // loop in rings within disk
  for(int ring=0; ring<nrings[disc]; ring++){    
    
    const float phistep = 2*TMath::Pi()/nmods[ring];
    const float inr = inner_radius[ring]; 
    const float our = outer_radius[ring]; 

    // loop in modules within ring
    for(int mod=0; mod<nmods[ring]; mod++){

      for(int j=0; j<5; j++){
	x[j]=0;
	y[j]=0;
      }

      // clockwise if looking z-axis into the page
      float phi = -mod*phistep; 
      
      x[0] = inr*cos(phi-phistep/2);
      x[1] = inr*cos(phi+phistep/2);
      x[2] = our*cos(phi+phistep/2);
      x[3] = our*cos(phi-phistep/2);
      x[4] = inr*cos(phi-phistep/2);
      
      y[0] = inr*sin(phi-phistep/2);
      y[1] = inr*sin(phi+phistep/2);
      y[2] = our*sin(phi+phistep/2);
      y[3] = our*sin(phi-phistep/2);
      y[4] = inr*sin(phi-phistep/2);

      int col_index = locateColor(gStyle->GetNumberOfColors(), 
      				  min, max, 
				  histo->GetBinContent(ring+1,mod+1));
      p[mod] = new TPolyLine(5,x,y);
      if( histo->GetBinContent(ring+1,mod+1)> -1000000) p[mod]->SetFillColor(gStyle->GetColorPalette(col_index));
      else  p[mod]->SetFillColor(kBlack);
     
      

    } // end loop in modules

    for(int mod=0; mod<nmods[ring]; mod++)
      p[mod]->Draw("Fsame");
      
  } // end loop in rings

  return;
}
