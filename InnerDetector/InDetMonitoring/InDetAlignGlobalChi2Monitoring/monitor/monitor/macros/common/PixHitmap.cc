/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PixHitmap() {

  if (!SCTOnly && iPixelhits>0) {
    if(iDoF>200) {
      cout << " Pixel Hitmap..." << endl;

      int hitcut = 0; // cut by hand
    
      int ihitmap = treeHitmap->GetEntries();
      cPixHitmap = new TCanvas("CosmicPixHitmapPixel","Pixel Hitmap",1200,350);
      cPixHitmap->Divide(3,1);
    
      // Pixels Hitmaps
      TH2F *align_Pixlayer_2 = new TH2F("align_Pixlayer_2","Pixels - Layer 2",13,-6.5,6.5,52,-0.5,51.5);
      align_Pixlayer_2->GetYaxis()->SetTitle("Sector (#phi)");
      align_Pixlayer_2->GetXaxis()->SetTitle("Ring (#eta)");
      align_Pixlayer_2->GetZaxis()->SetLabelSize(0.02);
      align_Pixlayer_2->SetStats(kFALSE);
      align_Pixlayer_2->SetLineColor(1);
      align_Pixlayer_2->SetContour(45);
    
      TH2F *align_Pixlayer_1 = new TH2F("align_Pixlayer_1","Pixels - Layer 1",13,-6.5,6.5,38,-0.5,37.5);
      align_Pixlayer_1->GetYaxis()->SetTitle("Sector (#phi)");
      align_Pixlayer_1->GetXaxis()->SetTitle("Ring (#eta)");
      align_Pixlayer_1->GetZaxis()->SetLabelSize(0.02);
      align_Pixlayer_1->SetStats(kFALSE);
      align_Pixlayer_1->SetLineColor(1);
      align_Pixlayer_1->SetContour(45);
    
      TH2F *align_Pixlayer_0 = new TH2F("align_Pixlayer_0","Pixels - Layer 0",13,-6.5,6.5,22,-0.5,21.5);
      align_Pixlayer_0->GetYaxis()->SetTitle("Sector (#phi)");
      align_Pixlayer_0->GetXaxis()->SetTitle("Ring (#eta)");
      align_Pixlayer_0->GetZaxis()->SetLabelSize(0.02);
      align_Pixlayer_0->SetStats(kFALSE);
      align_Pixlayer_0->SetLineColor(1);
      align_Pixlayer_0->SetContour(45);
    
      int nhits, nlayer, nsector, nring, ndettype;
      treeHitmap->SetBranchAddress("dettype",&ndettype);  
      treeHitmap->SetBranchAddress("hits",&nhits);   
      treeHitmap->SetBranchAddress("layer",&nlayer);   
      treeHitmap->SetBranchAddress("ring",&nring);   
      treeHitmap->SetBranchAddress("sector",&nsector); 
    
      int activePixMods[3]={0};
    
      for (int i=0; i<ihitmap; i++) {
	treeHitmap->GetEntry(i);
      
	if (nhits>=hitcut && ndettype==1) {
	  if (nlayer==0) {
	    //     cout << "[" << nlayer << "," << nring << "," << nsector 
	    //      	 << "] - " << nhits << " hits" << endl;
	    align_Pixlayer_0->SetBinContent(nring+7,nsector+1,nhits);
	    activePixMods[0]++;
	  }
	  else if (nlayer==1) {
	    // 	cout << "[" << nlayer << "," << nring << "," << nsector 
	    // 	     << "] - " << nhits << " hits" << endl;
	    align_Pixlayer_1->SetBinContent(nring+7,nsector+1,nhits);
	    activePixMods[1]++;
	  }
	  else if (nlayer==2) {
	    // 	cout << "[" << nlayer << "," << nring << "," << nsector 
	    // 	     << "] - " << nhits << " hits" << endl;
	    align_Pixlayer_2->SetBinContent(nring+7,nsector+1,nhits);
	    activePixMods[2]++;
	  }
	}
      } 
  

      // Set common maximum in all hitmaps
      int max0 = align_Pixlayer_0->GetBinContent(align_Pixlayer_0->GetMaximumBin());
      int max1 = align_Pixlayer_1->GetBinContent(align_Pixlayer_1->GetMaximumBin());
      int max2 = align_Pixlayer_2->GetBinContent(align_Pixlayer_2->GetMaximumBin());
      int maxx = max(max0,max1); maxx = max(maxx,max2);
      align_Pixlayer_0->SetMaximum(maxx);
      align_Pixlayer_1->SetMaximum(maxx);
      align_Pixlayer_2->SetMaximum(maxx);

      int hitPixMods[3]={0};

      for (int ii=1;ii<14;ii++) {

	// Layer 0
	for (int jj=1;jj<30;jj++) {
	  int bin0 = align_Pixlayer_0->GetBinContent(ii,jj);
	  if (bin0!=0) hitPixMods[0]++;
	}
    
	// Layer 1
	for (int jj=1;jj<40;jj++) {
	  int bin1 = align_Pixlayer_1->GetBinContent(ii,jj);
	  if (bin1!=0) hitPixMods[1]++;
	}
    
	// Layer 2
	for (int jj=1;jj<55;jj++) {
	  int bin2 = align_Pixlayer_2->GetBinContent(ii,jj);
	  if (bin2!=0) hitPixMods[2]++;
	}
      }

      int totalPix=0;
      for (int i=0;i<3;i++) {
	cout << "- Layer: " << i << " :: Nbr. of Pixel modules with hits = " << hitPixMods[i] << endl;
	totalPix += hitPixMods[i];
	cPixHitmap->cd(i+1);
	gPad->SetRightMargin(2);

	if (i==0) align_Pixlayer_0->DrawCopy("COLZTEXT");
	else if (i==1) align_Pixlayer_1->DrawCopy("COLZTEXT");
	else if (i==2) align_Pixlayer_2->DrawCopy("COLZTEXT");

	TLatex *t = new TLatex();
	t->SetNDC();
	t->SetTextFont(62);
	t->SetTextColor(36);
	t->SetTextSize(0.04);
	t->SetTextAlign(12);
	sprintf(name,"Modules: %d",activePixMods[i]);
	t->DrawLatex(0.37,0.98,name);
	t->SetTextSize(0.03);
	sprintf(name,"(Modules with hits: %d)",hitPixMods[i]);
	t->DrawLatex(0.63,0.975,name);
      }

      // --------------------------------------
      // Show active modules
      // --------------------------------------
      for (int i=0; i<ihitmap; i++) {
	treeHitmap->GetEntry(i);

	if (ndettype==1) {
	  //     cout << "[" << nlayer << "," << nring << "," << nsector 
	  // 	 << "] - " << nhits << " hits" << endl;
      
	  if (nlayer==0) align_Pixlayer_0->SetBinContent(nring+7,nsector+1,maxx);
	  else if (nlayer==1) align_Pixlayer_1->SetBinContent(nring+7,nsector+1,maxx);
	  else if (nlayer==2) align_Pixlayer_2->SetBinContent(nring+7,nsector+1,maxx);
	} 
      }

      for (int i=0;i<3;i++) {
	cPixHitmap->cd(i+1);
	if (i==0) align_Pixlayer_0->DrawCopy("BOXSAME");
	else if (i==1) align_Pixlayer_1->DrawCopy("BOXSAME");
	else if (i==2) align_Pixlayer_2->DrawCopy("BOXSAME");
      }

      cout << " * Nbr. of active modules: " << ihitmap << endl;
      cout << " * Nbr. of Pixel modules with hits: " << totalPix << endl;

      align_Pixlayer_0->Reset();
      align_Pixlayer_1->Reset();
      align_Pixlayer_2->Reset();

      if (good) {
	// -----------------------------------------
	// Show modules than not pass the HitCut
	// -----------------------------------------
	int mlayer, msector, mring, mdettype;
	treeConsts->SetBranchAddress("layer",&mlayer);   
	treeConsts->SetBranchAddress("ring",&mring);   
	treeConsts->SetBranchAddress("sector",&msector); 
	treeConsts->SetBranchAddress("dettype",&mdettype); 
	// treeConsts->Scan();
	// treeConsts->Show(0);

	int goodMods[3]={0};

	for (int ii=0; ii<iDoF/iAlignPars; ii++) {
	  treeConsts->GetEntry(ii);
    
	  if (mdettype==1) {
	
	    // cout << "[" << mlayer << "," << mring << "," << msector << "]" << endl;
	
	    if (mlayer==0) {
	      align_Pixlayer_0->SetBinContent(mring+7,msector+1,maxx);
	      goodMods[mlayer]++;
	    }
	    else if (mlayer==1) {
	      align_Pixlayer_1->SetBinContent(mring+7,msector+1,maxx);
	      goodMods[mlayer]++;
	    }
	    else if (mlayer==2) {
	      align_Pixlayer_2->SetBinContent(mring+7,msector+1,maxx);
	      goodMods[mlayer]++;
	    }
	  }
	}

	for (int i=0;i<3;i++) {
	  cPixHitmap->cd(i+1);
    
	  if (i==0) {
	    align_Pixlayer_0->SetLineColor(kRed);
	    align_Pixlayer_0->DrawCopy("BOXSAME");
	  }
	  else if (i==1) {
	    align_Pixlayer_1->SetLineColor(kRed);
	    align_Pixlayer_1->DrawCopy("BOXSAME");
	  }
	  else {
	    align_Pixlayer_2->SetLineColor(kRed);
	    align_Pixlayer_2->DrawCopy("BOXSAME");
	  }
	}
	cout << " * Nbr. of modules with more that " << iNhitsCut << " hits: " << iDoF/iAlignPars << endl;
	cout << " * Nbr. of modules excluded from the alignment: " << ihitmap-treeConsts->GetEntries() << endl;
    
	for (int i=0;i<3;i++) {
	  cPixHitmap->cd(i+1);
	  sprintf(name,"Modules with more than %d hits: %d",iNhitsCut,goodMods[i]);
	  t->DrawLatex(0.37,0.94,name);
	  cPixHitmap->Update();
	}

      }
    }
    else cout << " --> No Pixel hitmap because it is not Level 3" << endl;
  }
  else cout << " --> No Pixel hitmap because No Pixels (SCTOnly sample)" << endl;
}
