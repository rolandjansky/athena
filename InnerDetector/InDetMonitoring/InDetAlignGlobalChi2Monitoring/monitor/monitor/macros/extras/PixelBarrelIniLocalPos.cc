/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PixelBarrelIniLocalPos() {

  if (iPixelHits>0) {
    Long_t tmp;
    TCut cut1 = "mcode<1300000 && mcode>1000000";

    Char_t inFile[180];
    sprintf(inFile,"%s/Iter%d_LocalPositions.txt",path,IterNumber);
    
    if (gSystem->GetPathInfo(inFile,&tmp,&tmp,&tmp,&tmp))
      sprintf(inFile,"%s/../Iter%d_LocalPositions.txt",path,IterNumber);
    if (gSystem->GetPathInfo(inFile,&tmp,&tmp,&tmp,&tmp))
      cout << "No LocalPositions.txt file found!" << endl;
    else {

      TTree *treePixelPos = new TTree("PixelPositions","Pixel Module Position");

      // milimeters and miliradians corrections
      int IsFile = treePixelPos->ReadFile(inFile,"mcode:Tx:Ty:Tz:Rx:Ry:Rz");

      if (!IsFile) {
	// cout << "   - No " << IterNumber << "_LocalPositions.txt file found!" << endl;
	continue;
      }

      cout << " Initial Local Pixel Module Positions..." << endl;

      // treePixelPos->Scan();

      cPixelBarrelPos = new TCanvas("cPixelBarrelPos","Initial Pixel Module Position",
				    CanvasSizeX6[0],CanvasSizeX6[1]);
      cPixelBarrelPos->Divide(3,2);

      //
      // -----------------------------------------------------------------
      //

      cPixelBarrelPos->cd(1);
      sprintf(name,"Tx>>hPixelBarrelTx(100,%2.3f,%2.3f)",-1.6,1.6);
      treePixelPos->Draw(name,cut1,"goff");
      hPixelBarrelTx->SetTitle("Initial Pixel Position for this iter wrt nominal");
      if (normalizePlots) hPixelBarrelTx->GetYaxis()->SetTitle("Entries (%)");
      else                hPixelBarrelTx->GetYaxis()->SetTitle("Entries");
      hPixelBarrelTx->GetXaxis()->SetTitle("Local Pixel Barrel #DeltaTx (mm)");
      float iPixB = hPixelBarrelTx->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hPixelBarrelTx->Scale(100./iPixB);
      hPixelBarrelTx->SetLineColor(ColorForBarrel+2);
      hPixelBarrelTx->SetLineWidth(1);
      hPixelBarrelTx->SetFillColor(ColorForBarrel);
      hPixelBarrelTx->SetFillStyle(3001);
      doSpace(hPixelBarrelTx,1.2);
      hPixelBarrelTx->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hPixelBarrelTx);
      delete hPixelBarrelTx;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  Pixel Barrel");
      pText->Draw("tr");

      //
      // -----------------------------------------------------------------
      //
      
      cPixelBarrelPos->cd(2);
      sprintf(name,"Ty>>hPixelBarrelTy(100,%2.3f,%2.3f)",-1.6,1.6);
      treePixelPos->Draw(name,cut1,"goff");
      hPixelBarrelTy->SetTitle("Initial Pixel Position for this iter wrt nominal");
      if (normalizePlots) hPixelBarrelTy->GetYaxis()->SetTitle("Entries (%)");
      else                hPixelBarrelTy->GetYaxis()->SetTitle("Entries");
      hPixelBarrelTy->GetXaxis()->SetTitle("Local Pixel Barrel #DeltaTy (mm)");
      float iPixB = hPixelBarrelTy->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hPixelBarrelTy->Scale(100./iPixB);
      hPixelBarrelTy->SetLineColor(ColorForBarrel+2);
      hPixelBarrelTy->SetLineWidth(1);
      hPixelBarrelTy->SetFillColor(ColorForBarrel);
      hPixelBarrelTy->SetFillStyle(3001);
      doSpace(hPixelBarrelTy,1.2);
      hPixelBarrelTy->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hPixelBarrelTy);
      delete hPixelBarrelTy;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  Pixel Barrel");
      pText->Draw("tr");

      //
      // -----------------------------------------------------------------
      //
      
      cPixelBarrelPos->cd(3);
      sprintf(name,"Tz>>hPixelBarrelTz(100,%2.3f,%2.3f)",-1.6,1.6);
      treePixelPos->Draw(name,cut1,"goff");
      hPixelBarrelTz->SetTitle("Initial Pixel Position for this iter wrt nominal");
      if (normalizePlots) hPixelBarrelTz->GetYaxis()->SetTitle("Entries (%)");
      else                hPixelBarrelTz->GetYaxis()->SetTitle("Entries");
      hPixelBarrelTz->GetXaxis()->SetTitle("Local Pixel Barrel #DeltaTz (mm)");
      float iPixB = hPixelBarrelTz->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hPixelBarrelTz->Scale(100./iPixB);
      hPixelBarrelTz->SetLineColor(ColorForBarrel+2);
      hPixelBarrelTz->SetLineWidth(1);
      hPixelBarrelTz->SetFillColor(ColorForBarrel);
      hPixelBarrelTz->SetFillStyle(3001);
      doSpace(hPixelBarrelTz,1.2);
      hPixelBarrelTz->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hPixelBarrelTz);
      delete hPixelBarrelTz;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  Pixel Barrel");
      pText->Draw("tr");

      //
      // -----------------------------------------------------------------
      //
      
      cPixelBarrelPos->cd(4);
      sprintf(name,"Rx>>hPixelBarrelRx(100,%2.3f,%2.3f)",-1.6,1.6);
      treePixelPos->Draw(name,cut1,"goff");
      hPixelBarrelRx->SetTitle("Initial Pixel Position for this iter wrt nominal");
      if (normalizePlots) hPixelBarrelRx->GetYaxis()->SetTitle("Entries (%)");
      else                hPixelBarrelRx->GetYaxis()->SetTitle("Entries");
      hPixelBarrelRx->GetXaxis()->SetTitle("Local Pixel Barrel #DeltaRx (mrad)");
      float iPixB = hPixelBarrelRx->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hPixelBarrelRx->Scale(100./iPixB);
      hPixelBarrelRx->SetLineColor(ColorForBarrel+2);
      hPixelBarrelRx->SetLineWidth(1);
      hPixelBarrelRx->SetFillColor(ColorForBarrel);
      hPixelBarrelRx->SetFillStyle(3001);
      doSpace(hPixelBarrelRx,1.2);
      hPixelBarrelRx->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hPixelBarrelRx);
      delete hPixelBarrelRx;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  Pixel Barrel");
      pText->Draw("tr");

      //
      // -----------------------------------------------------------------
      //
      
      cPixelBarrelPos->cd(5);
      sprintf(name,"Ry>>hPixelBarrelRy(100,%2.3f,%2.3f)",-2.4,2.4);
      treePixelPos->Draw(name,cut1,"goff");
      hPixelBarrelRy->SetTitle("Initial Pixel Position for this iter wrt nominal");
      if (normalizePlots) hPixelBarrelRy->GetYaxis()->SetTitle("Entries (%)");
      else                hPixelBarrelRy->GetYaxis()->SetTitle("Entries");
      hPixelBarrelRy->GetXaxis()->SetTitle("Local Pixel Barrel #DeltaRy (mrad)");
      float iPixB = hPixelBarrelRy->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hPixelBarrelRy->Scale(100./iPixB);
      hPixelBarrelRy->SetLineColor(ColorForBarrel+2);
      hPixelBarrelRy->SetLineWidth(1);
      hPixelBarrelRy->SetFillColor(ColorForBarrel);
      hPixelBarrelRy->SetFillStyle(3001);
      doSpace(hPixelBarrelRy,1.2);
      hPixelBarrelRy->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hPixelBarrelRy);
      delete hPixelBarrelRy;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  Pixel Barrel");
      pText->Draw("tr");


      //
      // -----------------------------------------------------------------
      //
      
      cPixelBarrelPos->cd(6);
      sprintf(name,"Rz>>hPixelBarrelRz(100,%2.3f,%2.3f)",-1.6,1.6);
      treePixelPos->Draw(name,cut1,"goff");
      hPixelBarrelRz->SetTitle("Initial Pixel Position for this iter wrt nominal");
      if (normalizePlots) hPixelBarrelRz->GetYaxis()->SetTitle("Entries (%)");
      else                hPixelBarrelRz->GetYaxis()->SetTitle("Entries");
      hPixelBarrelRz->GetXaxis()->SetTitle("Local Pixel Barrel #DeltaRz (mrad)");
      float iPixB = hPixelBarrelRz->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hPixelBarrelRz->Scale(100./iPixB);
      hPixelBarrelRz->SetLineColor(ColorForBarrel+2);
      hPixelBarrelRz->SetLineWidth(1);
      hPixelBarrelRz->SetFillColor(ColorForBarrel);
      hPixelBarrelRz->SetFillStyle(3001);
      doSpace(hPixelBarrelRz,1.2);
      hPixelBarrelRz->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hPixelBarrelRz);
      delete hPixelBarrelRz;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  Pixel Barrel");
      pText->Draw("tr");

      // -----------------------------------------------
      //
      //  Mean Plots
      //
      // -----------------------------------------------

      Float_t mcode;
      Int_t dettype, layer;
      Float_t Tx,Ty,Tz,Rx,Ry,Rz;

      // Create a ROOT Tree
      TTree *treePixelPos2 = new TTree("Tree","tree");
      treePixelPos2->Branch("dettype",&dettype,"dettype/I");
      treePixelPos2->Branch("layer",&layer,"layer/I");
      treePixelPos2->Branch("Tx",&Tx,"Tx/F");
      treePixelPos2->Branch("Ty",&Ty,"Ty/F");
      treePixelPos2->Branch("Tz",&Tz,"Tz/F");
      treePixelPos2->Branch("Rx",&Rx,"Rx/F");
      treePixelPos2->Branch("Ry",&Ry,"Ry/F");
      treePixelPos2->Branch("Rz",&Rz,"Rz/F");

      // treePixelPos2->Print();

      // Get info from the Summary branch
      treePixelPos->SetBranchAddress("mcode",&mcode);
      treePixelPos->SetBranchAddress("Tx",&Tx);   
      treePixelPos->SetBranchAddress("Ty",&Ty);   
      treePixelPos->SetBranchAddress("Tz",&Tz);   
      treePixelPos->SetBranchAddress("Rx",&Rx);   
      treePixelPos->SetBranchAddress("Ry",&Ry);   
      treePixelPos->SetBranchAddress("Rz",&Rz);   
  
      int events = treePixelPos->GetEntries();
      // cout << events << endl;

      for (int i=0; i<events; i++) {
	treePixelPos->GetEntry(i);
    
	if (mcode<1300000 && mcode>1000000) {

	  // Pixels and Barrels
	  dettype = 1;

	  layer = floor((mcode/100000)-dettype*10);

	  // cout << mcode << " " << " " << dettype << " " << layer << " " 
	  //      << Tx << " " << Ty << " " << Tz << " " << Rx << " " << Ry << " " << Rz << endl;

	  treePixelPos2->Fill();

	}
	else dettype = 2;
      }

      cPixelBarrelMeanPos = new TCanvas("cPixelBarrelMeanPos",
					"Initial Pixel Module Mean Position Vs Layer",
					CanvasSizeX6[0],CanvasSizeX6[1]);
      cPixelBarrelMeanPos->Divide(3,2);

      cPixelBarrelMeanPos->cd(1);
      treePixelPos2->Draw("Tx:layer","dettype==1","profgoff");
      TProfile *h2profPixelBarrelTx = (TProfile*)gDirectory->FindObject("htemp");
      h2profPixelBarrelTx->GetYaxis()->SetRangeUser(-0.4,0.4);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profPixelBarrelTx->GetXaxis()->SetNdivisions(3);
      h2profPixelBarrelTx->SetTitle("Local Pixel Barrel Initial mean Pos Vs Layer");
      h2profPixelBarrelTx->GetYaxis()->SetTitle("mean Tx distribution (mm)");
      h2profPixelBarrelTx->GetXaxis()->SetTitle("Pixel Barrel Layer");
      h2profPixelBarrelTx->DrawCopy();
      drawHorizontalLine(h2profPixelBarrelTx);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cPixelBarrelMeanPos->cd(2);
      treePixelPos2->Draw("Ty:layer","dettype==1","profgoff");
      TProfile *h2profPixelBarrelTy = (TProfile*)gDirectory->FindObject("htemp");
      h2profPixelBarrelTy->GetYaxis()->SetRangeUser(-1.4,1.4);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profPixelBarrelTy->GetXaxis()->SetNdivisions(3);
      h2profPixelBarrelTy->SetTitle("Local Pixel Barrel Initial mean Pos Vs Layer");
      h2profPixelBarrelTy->GetYaxis()->SetTitle("mean Ty distribution (mm)");
      h2profPixelBarrelTy->GetXaxis()->SetTitle("Pixel Barrel Layer");
      h2profPixelBarrelTy->DrawCopy();
      drawHorizontalLine(h2profPixelBarrelTy);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cPixelBarrelMeanPos->cd(3);
      treePixelPos2->Draw("Tz:layer","dettype==1","profgoff");
      TProfile *h2profPixelBarrelTz = (TProfile*)gDirectory->FindObject("htemp");
      h2profPixelBarrelTz->GetYaxis()->SetRangeUser(-0.4,0.4);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profPixelBarrelTz->GetXaxis()->SetNdivisions(3);
      h2profPixelBarrelTz->SetTitle("Local Pixel Barrel Initial mean Pos Vs Layer");
      h2profPixelBarrelTz->GetYaxis()->SetTitle("mean Tz distribution (mm)");
      h2profPixelBarrelTz->GetXaxis()->SetTitle("Pixel Barrel Layer");
      h2profPixelBarrelTz->DrawCopy();
      drawHorizontalLine(h2profPixelBarrelTz);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cPixelBarrelMeanPos->cd(4);
      treePixelPos2->Draw("Rx:layer","dettype==1","profgoff");
      TProfile *h2profPixelBarrelRx = (TProfile*)gDirectory->FindObject("htemp");
      h2profPixelBarrelRx->GetYaxis()->SetRangeUser(-0.2,0.2);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profPixelBarrelRx->GetXaxis()->SetNdivisions(3);
      h2profPixelBarrelRx->SetTitle("Local Pixel Barrel Initial mean Pos Vs Layer");
      h2profPixelBarrelRx->GetYaxis()->SetTitle("mean Rx distribution (mrad)");
      h2profPixelBarrelRx->GetXaxis()->SetTitle("Pixel Barrel Layer");
      h2profPixelBarrelRx->DrawCopy();
      drawHorizontalLine(h2profPixelBarrelRx);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cPixelBarrelMeanPos->cd(5);
      treePixelPos2->Draw("Ry:layer","dettype==1","profgoff");
      TProfile *h2profPixelBarrelRy = (TProfile*)gDirectory->FindObject("htemp");
      h2profPixelBarrelRy->GetYaxis()->SetRangeUser(-1.4,1.4);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profPixelBarrelRy->GetXaxis()->SetNdivisions(3);
      h2profPixelBarrelRy->SetTitle("Local Pixel Barrel Initial mean Pos Vs Layer");
      h2profPixelBarrelRy->GetYaxis()->SetTitle("mean Ry distribution (mrad)");
      h2profPixelBarrelRy->GetXaxis()->SetTitle("Pixel Barrel Layer");
      h2profPixelBarrelRy->DrawCopy();
      drawHorizontalLine(h2profPixelBarrelRy);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cPixelBarrelMeanPos->cd(6);
      treePixelPos2->Draw("Rz:layer","dettype==1","profgoff");
      TProfile *h2profPixelBarrelRz = (TProfile*)gDirectory->FindObject("htemp");
      h2profPixelBarrelRz->GetYaxis()->SetRangeUser(-0.2,0.2);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profPixelBarrelRz->GetXaxis()->SetNdivisions(3);
      h2profPixelBarrelRz->SetTitle("Local Pixel Barrel Initial mean Pos Vs Layer");
      h2profPixelBarrelRz->GetYaxis()->SetTitle("mean Rz distribution (mrad)");
      h2profPixelBarrelRz->GetXaxis()->SetTitle("Pixel Barrel Layer");
      h2profPixelBarrelRz->DrawCopy();
      drawHorizontalLine(h2profPixelBarrelRz);
      setTitleFont(font);
      setTitleSize(0.67);

      // Delete the tree
      delete treePixelPos;
      delete treePixelPos2;
    }
  }
  else cout << " Initial Local Pixel Module Positions... No Pixels!" << endl;
}
