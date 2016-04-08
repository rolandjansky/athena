/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void SCTBarrelIniLocalPos() {

  if (iSCTHits>0) {
    Long_t tmp;
    TCut cut1 = "mcode<2400000 && mcode>2000000";

    Char_t inFile[180];
    sprintf(inFile,"%s/Iter%d_LocalPositions.txt",path,IterNumber);
    if (gSystem->GetPathInfo(inFile,&tmp,&tmp,&tmp,&tmp))
      sprintf(inFile,"%s/../Iter%d_LocalPositions.txt",path,IterNumber);
    if (gSystem->GetPathInfo(inFile,&tmp,&tmp,&tmp,&tmp))
      cout << "No LocalPositions.txt file found!" << endl;
    else {

      TTree *treeSCTPos = new TTree("SCTPositions","SCT Module Position");

      // milimeters and miliradians corrections
      int IsFile = treeSCTPos->ReadFile(inFile,"mcode:Tx:Ty:Tz:Rx:Ry:Rz");

      if (!IsFile) {
	// cout << "   - No Iter" << IterNumber << "_LocalPositions.txt file found!" << endl;
	continue;
      }

      cout << " Initial Local SCT Module Positions..." << endl;

      // treeSCTPos->Scan();

      cSCTBarrelPos = new TCanvas("cSCTBarrelPos","Initial SCT Module Position",
				  CanvasSizeX6[0],CanvasSizeX6[1]);
      cSCTBarrelPos->Divide(3,2);

      //
      // -----------------------------------------------------------------
      //

      cSCTBarrelPos->cd(1);
      sprintf(name,"Tx>>hSCTBarrelTx(100,%2.3f,%2.3f)",-2.7,2.7);
      treeSCTPos->Draw(name,cut1,"goff");
      hSCTBarrelTx->SetTitle("Initial SCT Position for this iter wrt nominal");
      if (normalizePlots) hSCTBarrelTx->GetYaxis()->SetTitle("Entries (%)");
      else                hSCTBarrelTx->GetYaxis()->SetTitle("Entries");
      hSCTBarrelTx->GetXaxis()->SetTitle("Local SCT Barrel #DeltaTx (mm)");
      float iPixB = hSCTBarrelTx->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hSCTBarrelTx->Scale(100./iPixB);
      hSCTBarrelTx->SetLineColor(ColorForBarrel+2);
      hSCTBarrelTx->SetLineWidth(1);
      hSCTBarrelTx->SetFillColor(ColorForBarrel);
      hSCTBarrelTx->SetFillStyle(3001);
      doSpace(hSCTBarrelTx,1.2);
      hSCTBarrelTx->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hSCTBarrelTx);
      delete hSCTBarrelTx;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  SCT Barrel");
      pText->Draw("tr");

      //
      // -----------------------------------------------------------------
      //
      
      cSCTBarrelPos->cd(2);
      sprintf(name,"Ty>>hSCTBarrelTy(100,%2.3f,%2.3f)",-1.8,1.8);
      treeSCTPos->Draw(name,cut1,"goff");
      hSCTBarrelTy->SetTitle("Initial SCT Position for this iter wrt nominal");
      if (normalizePlots) hSCTBarrelTy->GetYaxis()->SetTitle("Entries (%)");
      else                hSCTBarrelTy->GetYaxis()->SetTitle("Entries");
      hSCTBarrelTy->GetXaxis()->SetTitle("Local SCT Barrel #DeltaTy (mm)");
      float iPixB = hSCTBarrelTy->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hSCTBarrelTy->Scale(100./iPixB);
      hSCTBarrelTy->SetLineColor(ColorForBarrel+2);
      hSCTBarrelTy->SetLineWidth(1);
      hSCTBarrelTy->SetFillColor(ColorForBarrel);
      hSCTBarrelTy->SetFillStyle(3001);
      doSpace(hSCTBarrelTy,1.2);
      hSCTBarrelTy->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hSCTBarrelTy);
      delete hSCTBarrelTy;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  SCT Barrel");
      pText->Draw("tr");

      //
      // -----------------------------------------------------------------
      //
      
      cSCTBarrelPos->cd(3);
      sprintf(name,"Tz>>hSCTBarrelTz(100,%2.3f,%2.3f)",-2.2,2.2);
      treeSCTPos->Draw(name,cut1,"goff");
      hSCTBarrelTz->SetTitle("Initial SCT Position for this iter wrt nominal");
      if (normalizePlots) hSCTBarrelTz->GetYaxis()->SetTitle("Entries (%)");
      else                hSCTBarrelTz->GetYaxis()->SetTitle("Entries");
      hSCTBarrelTz->GetXaxis()->SetTitle("Local SCT Barrel #DeltaTz (mm)");
      float iPixB = hSCTBarrelTz->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hSCTBarrelTz->Scale(100./iPixB);
      hSCTBarrelTz->SetLineColor(ColorForBarrel+2);
      hSCTBarrelTz->SetLineWidth(1);
      hSCTBarrelTz->SetFillColor(ColorForBarrel);
      hSCTBarrelTz->SetFillStyle(3001);
      doSpace(hSCTBarrelTz,1.2);
      hSCTBarrelTz->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hSCTBarrelTz);
      delete hSCTBarrelTz;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  SCT Barrel");
      pText->Draw("tr");

      //
      // -----------------------------------------------------------------
      //
      
      cSCTBarrelPos->cd(4);
      sprintf(name,"Rx>>hSCTBarrelRx(100,%2.3f,%2.3f)",-1.4,1.4);
      treeSCTPos->Draw(name,cut1,"goff");
      hSCTBarrelRx->SetTitle("Initial SCT Position for this iter wrt nominal");
      if (normalizePlots) hSCTBarrelRx->GetYaxis()->SetTitle("Entries (%)");
      else                hSCTBarrelRx->GetYaxis()->SetTitle("Entries");
      hSCTBarrelRx->GetXaxis()->SetTitle("Local SCT Barrel #DeltaRx (mrad)");
      float iPixB = hSCTBarrelRx->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hSCTBarrelRx->Scale(100./iPixB);
      hSCTBarrelRx->SetLineColor(ColorForBarrel+2);
      hSCTBarrelRx->SetLineWidth(1);
      hSCTBarrelRx->SetFillColor(ColorForBarrel);
      hSCTBarrelRx->SetFillStyle(3001);
      doSpace(hSCTBarrelRx,1.2);
      hSCTBarrelRx->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hSCTBarrelRx);
      delete hSCTBarrelRx;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  SCT Barrel");
      pText->Draw("tr");

      //
      // -----------------------------------------------------------------
      //
      
      cSCTBarrelPos->cd(5);
      sprintf(name,"Ry>>hSCTBarrelRy(100,%2.3f,%2.3f)",-3.0,3.0);
      treeSCTPos->Draw(name,cut1,"goff");
      hSCTBarrelRy->SetTitle("Initial SCT Position for this iter wrt nominal");
      if (normalizePlots) hSCTBarrelRy->GetYaxis()->SetTitle("Entries (%)");
      else                hSCTBarrelRy->GetYaxis()->SetTitle("Entries");
      hSCTBarrelRy->GetXaxis()->SetTitle("Local SCT Barrel #DeltaRy (mrad)");
      float iPixB = hSCTBarrelRy->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hSCTBarrelRy->Scale(100./iPixB);
      hSCTBarrelRy->SetLineColor(ColorForBarrel+2);
      hSCTBarrelRy->SetLineWidth(1);
      hSCTBarrelRy->SetFillColor(ColorForBarrel);
      hSCTBarrelRy->SetFillStyle(3001);
      doSpace(hSCTBarrelRy,1.2);
      hSCTBarrelRy->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hSCTBarrelRy);
      delete hSCTBarrelRy;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  SCT Barrel");
      pText->Draw("tr");


      //
      // -----------------------------------------------------------------
      //
      
      cSCTBarrelPos->cd(6);
      sprintf(name,"Rz>>hSCTBarrelRz(100,%2.3f,%2.3f)",-1.4,1.4);
      treeSCTPos->Draw(name,cut1,"goff");
      hSCTBarrelRz->SetTitle("Initial SCT Position for this iter wrt nominal");
      if (normalizePlots) hSCTBarrelRz->GetYaxis()->SetTitle("Entries (%)");
      else                hSCTBarrelRz->GetYaxis()->SetTitle("Entries");
      hSCTBarrelRz->GetXaxis()->SetTitle("Local SCT Barrel #DeltaRz (mrad)");
      float iPixB = hSCTBarrelRz->GetEntries();
      if (!AtlasStyle) {
	gPad->SetGridx();
	gPad->SetGridy();
      }
      if (normalizePlots) hSCTBarrelRz->Scale(100./iPixB);
      hSCTBarrelRz->SetLineColor(ColorForBarrel+2);
      hSCTBarrelRz->SetLineWidth(1);
      hSCTBarrelRz->SetFillColor(ColorForBarrel);
      hSCTBarrelRz->SetFillStyle(3001);
      doSpace(hSCTBarrelRz,1.2);
      hSCTBarrelRz->DrawCopy();
      setTitleFont(font);
      setTitleSize(0.67);
      ScalePlots();
      drawVerticalLine(hSCTBarrelRz);
      delete hSCTBarrelRz;

      TPaveText *pText = new TPaveText(0.37,0.82,0.60,0.92,"NDC");
      pText->SetFillColor(kWhite);
      pText->SetShadowColor(kWhite);
      pText->SetTextAlign(12);
      pText->SetTextSize(0.04);
      pText->AddText("  SCT Barrel");
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
      TTree *treeSCTPos2 = new TTree("Tree","tree");
      treeSCTPos2->Branch("dettype",&dettype,"dettype/I");
      treeSCTPos2->Branch("layer",&layer,"layer/I");
      treeSCTPos2->Branch("Tx",&Tx,"Tx/F");
      treeSCTPos2->Branch("Ty",&Ty,"Ty/F");
      treeSCTPos2->Branch("Tz",&Tz,"Tz/F");
      treeSCTPos2->Branch("Rx",&Rx,"Rx/F");
      treeSCTPos2->Branch("Ry",&Ry,"Ry/F");
      treeSCTPos2->Branch("Rz",&Rz,"Rz/F");

      // treeSCTPos2->Print();

      // Get info from the Summary branch
      treeSCTPos->SetBranchAddress("mcode",&mcode);
      treeSCTPos->SetBranchAddress("Tx",&Tx);   
      treeSCTPos->SetBranchAddress("Ty",&Ty);   
      treeSCTPos->SetBranchAddress("Tz",&Tz);   
      treeSCTPos->SetBranchAddress("Rx",&Rx);   
      treeSCTPos->SetBranchAddress("Ry",&Ry);   
      treeSCTPos->SetBranchAddress("Rz",&Rz);   
  
      int events = treeSCTPos->GetEntries();
      // cout << events << endl;

      for (int i=0; i<events; i++) {
	treeSCTPos->GetEntry(i);
    
	if (mcode<2400000 && mcode>2000000) {

	  // SCTs and Barrels
	  dettype = 2;

	  layer = floor((mcode/100000)-dettype*10);

	  // cout << mcode << " " << " " << dettype << " " << layer << " " 
	  //      << Tx << " " << Ty << " " << Tz << " " << Rx << " " << Ry << " " << Rz << endl;
	  
	  treeSCTPos2->Fill();

	}
	else dettype = 1;
      }

      cSCTBarrelMeanPos = new TCanvas("cSCTBarrelMeanPos",
					"Initial SCT Module Mean Position Vs Layer",
				      CanvasSizeX6[0],CanvasSizeX6[1]);
      cSCTBarrelMeanPos->Divide(3,2);

      cSCTBarrelMeanPos->cd(1);
      treeSCTPos2->Draw("Tx:layer","dettype==2","profgoff");
      TProfile *h2profSCTBarrelTx = (TProfile*)gDirectory->FindObject("htemp");
      h2profSCTBarrelTx->GetYaxis()->SetRangeUser(-1.6,1.6);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profSCTBarrelTx->GetXaxis()->SetNdivisions(4);
      h2profSCTBarrelTx->SetTitle("Local SCT Barrel Initial mean Pos Vs Layer");
      h2profSCTBarrelTx->GetYaxis()->SetTitle("mean Tx distribution (mm)");
      h2profSCTBarrelTx->GetXaxis()->SetTitle("SCT Barrel Layer");
      h2profSCTBarrelTx->DrawCopy();
      drawHorizontalLine(h2profSCTBarrelTx);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cSCTBarrelMeanPos->cd(2);
      treeSCTPos2->Draw("Ty:layer","dettype==2","profgoff");
      TProfile *h2profSCTBarrelTy = (TProfile*)gDirectory->FindObject("htemp");
      h2profSCTBarrelTy->GetYaxis()->SetRangeUser(-1.5,1.5);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profSCTBarrelTy->GetXaxis()->SetNdivisions(4);
      h2profSCTBarrelTy->SetTitle("Local SCT Barrel Initial mean Pos Vs Layer");
      h2profSCTBarrelTy->GetYaxis()->SetTitle("mean Ty distribution (mm)");
      h2profSCTBarrelTy->GetXaxis()->SetTitle("SCT Barrel Layer");
      h2profSCTBarrelTy->DrawCopy();
      drawHorizontalLine(h2profSCTBarrelTy);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cSCTBarrelMeanPos->cd(3);
      treeSCTPos2->Draw("Tz:layer","dettype==2","profgoff");
      TProfile *h2profSCTBarrelTz = (TProfile*)gDirectory->FindObject("htemp");
      h2profSCTBarrelTz->GetYaxis()->SetRangeUser(-0.6,0.6);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profSCTBarrelTz->GetXaxis()->SetNdivisions(4);
      h2profSCTBarrelTz->SetTitle("Local SCT Barrel Initial mean Pos Vs Layer");
      h2profSCTBarrelTz->GetYaxis()->SetTitle("mean Tz distribution (mm)");
      h2profSCTBarrelTz->GetXaxis()->SetTitle("SCT Barrel Layer");
      h2profSCTBarrelTz->DrawCopy();
      drawHorizontalLine(h2profSCTBarrelTz);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cSCTBarrelMeanPos->cd(4);
      treeSCTPos2->Draw("Rx:layer","dettype==2","profgoff");
      TProfile *h2profSCTBarrelRx = (TProfile*)gDirectory->FindObject("htemp");
      h2profSCTBarrelRx->GetYaxis()->SetRangeUser(-0.2,0.2);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profSCTBarrelRx->GetXaxis()->SetNdivisions(4);
      h2profSCTBarrelRx->SetTitle("Local SCT Barrel Initial mean Pos Vs Layer");
      h2profSCTBarrelRx->GetYaxis()->SetTitle("mean Rx distribution (mrad)");
      h2profSCTBarrelRx->GetXaxis()->SetTitle("SCT Barrel Layer");
      h2profSCTBarrelRx->DrawCopy();
      drawHorizontalLine(h2profSCTBarrelRx);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cSCTBarrelMeanPos->cd(5);
      treeSCTPos2->Draw("Ry:layer","dettype==2","profgoff");
      TProfile *h2profSCTBarrelRy = (TProfile*)gDirectory->FindObject("htemp");
      h2profSCTBarrelRy->GetYaxis()->SetRangeUser(-2.4,2.4);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profSCTBarrelRy->GetXaxis()->SetNdivisions(4);
      h2profSCTBarrelRy->SetTitle("Local SCT Barrel Initial mean Pos Vs Layer");
      h2profSCTBarrelRy->GetYaxis()->SetTitle("mean Ry distribution (mrad)");
      h2profSCTBarrelRy->GetXaxis()->SetTitle("SCT Barrel Layer");
      h2profSCTBarrelRy->DrawCopy();
      drawHorizontalLine(h2profSCTBarrelRy);
      setTitleFont(font);
      setTitleSize(0.67);

      //
      // -----------------------------------------------------------------
      //

      cSCTBarrelMeanPos->cd(6);
      treeSCTPos2->Draw("Rz:layer","dettype==2","profgoff");
      TProfile *h2profSCTBarrelRz = (TProfile*)gDirectory->FindObject("htemp");
      h2profSCTBarrelRz->GetYaxis()->SetRangeUser(-0.4,0.4);
      if (!AtlasStyle) {
	gPad->SetGridy();
	gPad->SetGridx();
      }
      h2profSCTBarrelRz->GetXaxis()->SetNdivisions(4);
      h2profSCTBarrelRz->SetTitle("Local SCT Barrel Initial mean Pos Vs Layer");
      h2profSCTBarrelRz->GetYaxis()->SetTitle("mean Rz distribution (mrad)");
      h2profSCTBarrelRz->GetXaxis()->SetTitle("SCT Barrel Layer");
      h2profSCTBarrelRz->DrawCopy();
      drawHorizontalLine(h2profSCTBarrelRz);
      setTitleFont(font);
      setTitleSize(0.67);

      // Delete the tree
      delete treeSCTPos;
      delete treeSCTPos2;
    }
  }
  else cout << "Initial Local SCT Module Positions... No SCTs!" << endl;
}
