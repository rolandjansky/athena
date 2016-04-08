/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

void PIXConstantsMap() {

  if (iAlignProcessLevel==3 && iAlignSolveLevel==3 && fileResults!=NULL && !SCTOnly) {
    
    cout << " Pixel Alignment Corrections Maps..." << endl;
    
    // check which DoF is available
    isAlignCorrGood_PIX();

    Bool_t setCommonMax = true;
    Bool_t setCommonMaxInBothPerLayer = true;
    Bool_t setCommonMaxInBoth = false;

    const Int_t m_alignParams = 6;
    const Int_t m_nPIXBarrels = 3;

    Color_t icolor = 33;

    // Define histograms
    TH2F *hPIXConstMap[m_nPIXBarrels][m_alignParams];
    for (int i=0; i<m_nPIXBarrels; i++) {
      for (int j=0; j<m_alignParams; j++) {
      
	if (j==0) sprintf(name,"Tx");
	else if (j==1) sprintf(name,"Ty");
	else if (j==2) sprintf(name,"Tz");
	else if (j==3) sprintf(name,"Rx");
	else if (j==4) sprintf(name,"Ry");
	else if (j==5) sprintf(name,"Rz");

	sprintf(name2,"hPIXConstMap%sLayer%d",name,i);
	if (i==0) hPIXConstMap[i][j] = new TH2F(name2,name2,13,-6.5,6.5,22,-0.5,21.5);
	else if (i==1) hPIXConstMap[i][j] = new TH2F(name2,name2,13,-6.5,6.5,38,-0.5,37.5);
	else if (i==2) hPIXConstMap[i][j] = new TH2F(name2,name2,13,-6.5,6.5,52,-0.5,51.5);
	sprintf(name2,"Alignment %s Corrections Maps - Layer %d",name,i);
	hPIXConstMap[i][j]->SetTitle(name2);
	hPIXConstMap[i][j]->GetYaxis()->SetTitle("sector");
	hPIXConstMap[i][j]->GetXaxis()->SetTitle("ring");
	hPIXConstMap[i][j]->SetStats(kFALSE);
	
	/*
	  for (Int_t bin=1; bin<= hPIXConstMap [i][j]->GetNbinsX(); bin++) {
	  if (bin== 1) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-6");    
	  if (bin== 2) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-5");    
	  if (bin== 3) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-4");    
	  if (bin== 4) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-3");    
	  if (bin== 5) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-2");    
	  if (bin== 6) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-1");    
	  if (bin== 7) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"1");    
	  if (bin== 8) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"2");    
	  if (bin== 9) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"3");    
	  if (bin==10) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"4");    
	  if (bin==11) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"5");    
	  if (bin==12) hPIXConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"6");    
	  }
	*/

      }
    }

    int iModules = treeConsts->GetEntries();

    int nlayer, nsector, nring, ndettype, nbec;
    treeConsts->SetBranchAddress("dettype", &ndettype);  
    treeConsts->SetBranchAddress("bec", &nbec);   
    treeConsts->SetBranchAddress("layer", &nlayer);   
    treeConsts->SetBranchAddress("ring", &nring);   
    treeConsts->SetBranchAddress("sector", &nsector); 

    float Tx,Ty,Tz,Rx,Ry,Rz;
    treeConsts->SetBranchAddress("Tx", &Tx);   
    treeConsts->SetBranchAddress("Ty", &Ty);   
    treeConsts->SetBranchAddress("Tz", &Tz);   
    treeConsts->SetBranchAddress("Rx", &Rx);   
    treeConsts->SetBranchAddress("Ry", &Ry);   
    treeConsts->SetBranchAddress("Rz", &Rz);   

    for (int i=0; i<iModules; i++) {
      treeConsts->GetEntry(i);
    
      if (ndettype==1 && nbec==0) {
	if (nlayer<m_nPIXBarrels) {

	  hPIXConstMap[nlayer][0]->SetBinContent(nring+7,nsector+1,Tx);
	  hPIXConstMap[nlayer][1]->SetBinContent(nring+7,nsector+1,Ty);
	  hPIXConstMap[nlayer][2]->SetBinContent(nring+7,nsector+1,Tz);
	  hPIXConstMap[nlayer][3]->SetBinContent(nring+7,nsector+1,Rx);
	  hPIXConstMap[nlayer][4]->SetBinContent(nring+7,nsector+1,Ry);
	  hPIXConstMap[nlayer][5]->SetBinContent(nring+7,nsector+1,Rz);

	}
      }
    } // end for

    //
    // -----------------------------------------------------------------
    //
      
    if (setCommonMax) {

      // Set common maximum in all maps
      float max[m_nPIXBarrels][m_alignParams] = {0.0};
      float min[m_nPIXBarrels][m_alignParams] = {0.0};

      for (int i=0; i<m_nPIXBarrels; i++) {
	for (int j=0; j<m_alignParams; j++) {
	  max[i][j] = hPIXConstMap[i][j]->GetBinContent(hPIXConstMap[i][j]->GetMaximumBin());
	  min[i][j] = hPIXConstMap[i][j]->GetBinContent(hPIXConstMap[i][j]->GetMinimumBin());

	  if (PrintLevel<1) {
	    printf("%2.5f\n",max[i][j]);
	    printf("%2.5f\n",min[i][j]);
	  }

	}
      }
   
      if (setCommonMaxInBothPerLayer) {
	for (int i=0; i<m_nPIXBarrels; i++) {
	  for (int j=0; j<m_alignParams; j++) {
	    float maxBoth = max(max[i][j],TMath::Abs(min[i][j]));
	    hPIXConstMap[i][j]->SetMaximum(maxBoth);
	    hPIXConstMap[i][j]->SetMinimum(-maxBoth);
	  }
	}
      }

      else {
	float maxx[m_alignParams] = {0.0}; 
	float minx[m_alignParams] = {0.0};

	for (int j=0; j<m_alignParams; j++) {

	  if (m_nPIXBarrels>1) {
	    maxx[j] = max(max[0][j],max[1][j]); maxx[j] = max(maxx[j],max[2][j]); maxx[j] = max(maxx[j],max[3][j]);
	    minx[j] = min(min[0][j],min[1][j]); minx[j] = min(minx[j],min[2][j]);minx[j] = min(minx[j],min[3][j]);
	  }
	  else {
	    maxx[j] = max[0][j];
	    minx[j] = min[0][j];
	  }

	  if (PrintLevel<1) {
	    printf("-> %d. max: %2.5f\n",j,maxx[j]);
	    printf("-> %d. min: %2.5f\n",j,minx[j]);
	  }
	
	}
    
	for (int i=0; i<m_nPIXBarrels; i++) {
	  for (int j=0; j<m_alignParams; j++) {
	    if (setCommonMaxInBoth) {
	      float maxBoth = max(maxx[j],TMath::Abs(minx[j]));
	      hPIXConstMap[i][j]->SetMaximum(maxBoth);
	      hPIXConstMap[i][j]->SetMinimum(-maxBoth);
	    }
	    else {
	      hPIXConstMap[i][j]->SetMaximum(maxx[j]);
	      hPIXConstMap[i][j]->SetMinimum(minx[j]);
	    }
	  }
	}

      } // end setCommonMax
    }

    //
    // -----------------------------------------------------------------
    //

    gStyle->SetPadRightMargin(0.20);
    gStyle->SetPadLeftMargin(0.08);
    gStyle->SetPadTopMargin(0.15);
    gROOT->ForceStyle();

    // Module physical axis in Z (to be drwan on top along graphics X)
    TGaxis *ATLZaxis[m_nPIXBarrels];
    TGaxis *ATLPhiaxis[m_nPIXBarrels];
    
    for (int i=0; i<m_nPIXBarrels; i++) {
      // Z axis on top (only for histos with 12 bins) 
      ATLZaxis[i] = new TGaxis(-6.5,31.5,6.5,31.5,-405,405,510,"-");
      ATLZaxis[i]->SetName("Z");
      if (i == 0) {ATLZaxis[i]->SetY1(21.5); ATLZaxis[i]->SetY2(21.5);}
      if (i == 1) {ATLZaxis[i]->SetY1(37.5); ATLZaxis[i]->SetY2(37.5);}
      if (i == 2) {ATLZaxis[i]->SetY1(51.5); ATLZaxis[i]->SetY2(51.5);}
      ATLZaxis[i]->SetLabelSize(hPIXConstMap[0][0]->GetYaxis()->GetLabelSize());
      ATLZaxis[i]->SetLabelOffset(0.0);
      ATLZaxis[i]->SetLabelFont(hPIXConstMap[0][0]->GetXaxis()->GetLabelFont());
      ATLZaxis[i]->SetTitleSize(hPIXConstMap[0][0]->GetXaxis()->GetTitleSize());
      ATLZaxis[i]->SetTitleFont(hPIXConstMap[0][0]->GetXaxis()->GetTitleFont());
      ATLZaxis[i]->SetTitle(" z (mm)");

      // Phi axis on right
      ATLPhiaxis[i] = new TGaxis(6.5,-0.5,6.5,31.5,0.,360.,510,"+L");
      ATLPhiaxis[i]->SetName("Z");
      if (i == 0) {ATLPhiaxis[i]->SetY2(21.5);}
      if (i == 1) {ATLPhiaxis[i]->SetY2(37.5);}
      if (i == 2) {ATLPhiaxis[i]->SetY2(51.5);}
      ATLPhiaxis[i]->SetLabelSize(hPIXConstMap[0][0]->GetYaxis()->GetLabelSize());
      ATLPhiaxis[i]->SetLabelOffset(0.0);
      ATLPhiaxis[i]->SetLabelFont(hPIXConstMap[0][0]->GetXaxis()->GetLabelFont());
      ATLPhiaxis[i]->SetTitleSize(hPIXConstMap[0][0]->GetXaxis()->GetTitleSize());
      ATLPhiaxis[i]->SetTitleFont(hPIXConstMap[0][0]->GetXaxis()->GetTitleFont());
      ATLPhiaxis[i]->SetTitle("#phi (deg)");
      ATLPhiaxis[i]->SetLabelOffset(0.01);
      ATLPhiaxis[i]->SetTitleOffset(1.5);
      
    }

    // show plots
    for (int j=0; j<m_alignParams; j++) {

      if (isGood[j]) {
	if (j==0) sprintf(name,"Tx");
	else if (j==1) sprintf(name,"Ty");
	else if (j==2) sprintf(name,"Tz");
	else if (j==3) sprintf(name,"Rx");
	else if (j==4) sprintf(name,"Ry");
	else if (j==5) sprintf(name,"Rz");
	
	sprintf(name2,"cPIXConstantsMap%s",name);
	sprintf(name3,"Pixel Constants Map for %s",name);
	cPIXConstantsMap[j] = new TCanvas(name2,name3,CanvasSizeX3[0],CanvasSizeX3[1]);
	cPIXConstantsMap[j]->Divide(3,1);
	
	for (int i=0; i<m_nPIXBarrels; i++) {
	  cPIXConstantsMap[j]->cd(i+1);
	  hPIXConstMap[i][j]->Draw("COLZ");
	  //	cPIXConstantsMap[j]->Update();
	  
	  // moving the color scale
	  gPad->Update();
	  TPaletteAxis *palette = (TPaletteAxis*)hPIXConstMap[i][j]->GetListOfFunctions()->FindObject("palette");
	  palette->SetX1NDC(0.900);
	  palette->SetX2NDC(0.935);
	  palette->SetY1NDC(0.1);
	  palette->SetY2NDC(0.70);
	  gPad->Modified();
	  
	  setTitleFont(font);
	  setTitleSize(0.85);

	  ATLZaxis[i]->Draw();
	  ATLPhiaxis[i]->Draw();
	}
      }
      
    } // end j   
    
  }
  
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadTopMargin(0.12);
  gROOT->ForceStyle();
  
  return;
}


//=====================================================================
// isAlignCorrGood()
//=====================================================================
void isAlignCorrGood_PIX() {

  float m_Tx,m_Ty,m_Tz,m_Rx,m_Ry,m_Rz;
  treeConsts->SetBranchAddress("Tx", &m_Tx);   
  treeConsts->SetBranchAddress("Ty", &m_Ty);   
  treeConsts->SetBranchAddress("Tz", &m_Tz);   
  treeConsts->SetBranchAddress("Rx", &m_Rx);   
  treeConsts->SetBranchAddress("Ry", &m_Ry);   
  treeConsts->SetBranchAddress("Rz", &m_Rz);  

  int iModules = treeConsts->GetEntries();
  // cout << iModules << endl;

  TH1 *htempConstPIX[6];
  for (int i=0; i<6; i++) {
    if (i==0) sprintf(name,"ConstPIX_Tx");
    else if (i==1) sprintf(name,"ConstPIX_Ty");
    else if (i==2) sprintf(name,"ConstPIX_Tz");
    else if (i==3) sprintf(name,"ConstPIX_Rx");
    else if (i==4) sprintf(name,"ConstPIX_Ry");
    else if (i==5) sprintf(name,"ConstPIX_Rz");
    htempConstPIX[i] = new TH1F(name,name,100,-10,10);
  }

  for (int j=0; j<iModules; j++) {
    treeConsts->GetEntry(j);
    htempConstPIX[0]->Fill(m_Tx);
    htempConstPIX[1]->Fill(m_Ty);
    htempConstPIX[2]->Fill(m_Tz);
    htempConstPIX[3]->Fill(m_Rx);
    htempConstPIX[4]->Fill(m_Ry);
    htempConstPIX[5]->Fill(m_Rz);
  }
    
  TCanvas *cPIX = new TCanvas("test_ConstPIX","test_ConstPIX");
  // cPIX->Divide(3,2);
  for (int j=0; j<6; j++) {
    // cPIX->cd(j+1);
    htempConstPIX[j]->Draw("goff");
    // htemp[j]->Draw();
    if (htempConstPIX[j]->GetMean()!=0.0) isGood[j] = true;

    /*
      if (i==0) sprintf(name,"Tx");
      else if (i==1) sprintf(name,"Ty");
      else if (i==2) sprintf(name,"Tz");
      else if (i==3) sprintf(name,"Rx");
      else if (i==4) sprintf(name,"Ry");
      else if (i==5) sprintf(name,"Rz");
      cout << name << " is " << isGood[j] << endl;
    */
  }
  cPIX->Close();

  return;
}
