/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>

void SCTConstantsMap() {

  if (iAlignProcessLevel==3 && iAlignSolveLevel==3 && fileResults!=NULL && !PixelOnly) {
    
    cout << " SCT Alignment Corrections Maps..." << endl;
    
    // check which DoF is available
    isAlignCorrGood_SCT();

    Bool_t setCommonMax = true;
    Bool_t setCommonMaxInBothPerLayer = true;
    Bool_t setCommonMaxInBoth = false;
    
    const Int_t m_alignParams = 6;
    const Int_t m_nSCTBarrels = 4;

    Color_t icolor = 33;

    // Define histograms
    TH2F *hSCTConstMap[m_nSCTBarrels][m_alignParams];
    for (int i=0; i<m_nSCTBarrels; i++) {
      for (int j=0; j<m_alignParams; j++) {
      
	if (j==0) sprintf(name,"Tx");
	else if (j==1) sprintf(name,"Ty");
	else if (j==2) sprintf(name,"Tz");
	else if (j==3) sprintf(name,"Rx");
	else if (j==4) sprintf(name,"Ry");
	else if (j==5) sprintf(name,"Rz");

	sprintf(name2,"hSCTConstMap%sLayer%d",name,i);
	if (i==0) hSCTConstMap[i][j] = new TH2F(name2,name2,12,-5.5,6.5,32,-0.5,31.5);
	else if (i==1) hSCTConstMap[i][j] = new TH2F(name2,name2,12,-5.5,6.5,40,-0.5,39.5);
	else if (i==2) hSCTConstMap[i][j] = new TH2F(name2,name2,12,-5.5,6.5,48,-0.5,47.5);
	else if (i==3) hSCTConstMap[i][j] = new TH2F(name2,name2,12,-5.5,6.5,56,-0.5,55.5);
	sprintf(name2,"Alignment %s Corrections Maps - Layer %d",name,i);
	hSCTConstMap[i][j]->SetTitle(name2);
	hSCTConstMap[i][j]->GetYaxis()->SetTitle("sector");
	hSCTConstMap[i][j]->GetXaxis()->SetTitle("ring");
	hSCTConstMap[i][j]->SetStats(kFALSE);
	
	for (Int_t bin=1; bin<= hSCTConstMap [i][j]->GetNbinsX(); bin++) {
	  if (bin== 1) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-6");    
	  if (bin== 2) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-5");    
	  if (bin== 3) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-4");    
	  if (bin== 4) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-3");    
	  if (bin== 5) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-2");    
	  if (bin== 6) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"-1");    
	  if (bin== 7) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"1");    
	  if (bin== 8) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"2");    
	  if (bin== 9) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"3");    
	  if (bin==10) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"4");    
	  if (bin==11) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"5");    
	  if (bin==12) hSCTConstMap [i][j]->GetXaxis()->SetBinLabel(bin,"6");    
	}


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
    
      if (ndettype==2 && nbec==0) {
	if (nlayer<m_nSCTBarrels) {

	  if (nring<0) {
	    hSCTConstMap[nlayer][0]->SetBinContent(nring+7,nsector+1,Tx);
	    hSCTConstMap[nlayer][1]->SetBinContent(nring+7,nsector+1,Ty);
	    hSCTConstMap[nlayer][2]->SetBinContent(nring+7,nsector+1,Tz);
	    hSCTConstMap[nlayer][3]->SetBinContent(nring+7,nsector+1,Rx);
	    hSCTConstMap[nlayer][4]->SetBinContent(nring+7,nsector+1,Ry);
	    hSCTConstMap[nlayer][5]->SetBinContent(nring+7,nsector+1,Rz);
	  }
 	  if (nring>0){
 	    hSCTConstMap[nlayer][0]->SetBinContent(nring+6,nsector+1,Tx);
 	    hSCTConstMap[nlayer][1]->SetBinContent(nring+6,nsector+1,Ty);
 	    hSCTConstMap[nlayer][2]->SetBinContent(nring+6,nsector+1,Tz);
 	    hSCTConstMap[nlayer][3]->SetBinContent(nring+6,nsector+1,Rx);
 	    hSCTConstMap[nlayer][4]->SetBinContent(nring+6,nsector+1,Ry);
 	    hSCTConstMap[nlayer][5]->SetBinContent(nring+6,nsector+1,Rz);
	  }

	}
      }
    } // end for

    //
    // -----------------------------------------------------------------
    //
      
    if (setCommonMax) {
      
      // Set common maximum in all maps
      float max[m_nSCTBarrels][m_alignParams] = {0.0};
      float min[m_nSCTBarrels][m_alignParams] = {0.0};
      
      for (int i=0; i<m_nSCTBarrels; i++) {
	for (int j=0; j<m_alignParams; j++) {
	  max[i][j] = hSCTConstMap[i][j]->GetBinContent(hSCTConstMap[i][j]->GetMaximumBin());
	  min[i][j] = hSCTConstMap[i][j]->GetBinContent(hSCTConstMap[i][j]->GetMinimumBin());
	  
	  if (PrintLevel<1) {
	    printf("%2.5f\n",max[i][j]);
	    printf("%2.5f\n",min[i][j]);
	  }

	}
      }

      if (setCommonMaxInBothPerLayer) {
	for (int i=0; i<m_nSCTBarrels; i++) {
	  for (int j=0; j<m_alignParams; j++) {
	    float maxBoth = max(max[i][j],TMath::Abs(min[i][j]));
	    hSCTConstMap[i][j]->SetMaximum(maxBoth);
	    hSCTConstMap[i][j]->SetMinimum(-maxBoth);
	  }
	}
      }
      else {
	
	float maxx[m_alignParams] = {0.0}; 
	float minx[m_alignParams] = {0.0};
	
	for (int j=0; j<m_alignParams; j++) {
	  
	  if (m_nSCTBarrels>1) {
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
	
	for (int i=0; i<m_nSCTBarrels; i++) {
	  for (int j=0; j<m_alignParams; j++) {
	    if (setCommonMaxInBoth) {
	      float maxBoth = max(maxx[j],TMath::Abs(minx[j]));
	      hSCTConstMap[i][j]->SetMaximum(maxBoth);
	      hSCTConstMap[i][j]->SetMinimum(-maxBoth);
	    }
	    else {
	      hSCTConstMap[i][j]->SetMaximum(maxx[j]);
	      hSCTConstMap[i][j]->SetMinimum(minx[j]);
	    }
	  }
	}
      } // end setCommonMaxInBothPerLayer
      
    } // end setCommonMax
      

    //
    // -----------------------------------------------------------------
    //

    gStyle->SetPadRightMargin(0.16);
    gStyle->SetPadLeftMargin(0.08);
    gStyle->SetPadTopMargin(0.12);
    gROOT->ForceStyle();

    // Module physical axis in Z (to be drwan on top along graphics X)
    TGaxis *ATLZaxis[m_nSCTBarrels];
    TGaxis *ATLPhiaxis[m_nSCTBarrels];
 
    for (int i=0; i<m_nSCTBarrels; i++) {
      // Z axis on top (only for histos with 12 bins) 
      ATLZaxis[i] = new TGaxis(-5.5,31.5,6.5,31.5,-800,800,510,"-");
      ATLZaxis[i]->SetName("Z");
      if (i == 0) {ATLZaxis[i]->SetY1(31.5); ATLZaxis[i]->SetY2(31.5);}
      if (i == 1) {ATLZaxis[i]->SetY1(39.5); ATLZaxis[i]->SetY2(39.5);}
      if (i == 2) {ATLZaxis[i]->SetY1(47.5); ATLZaxis[i]->SetY2(47.5);}
      if (i == 3) {ATLZaxis[i]->SetY1(55.5); ATLZaxis[i]->SetY2(55.5);}
      ATLZaxis[i]->SetLabelSize(hSCTConstMap[0][0]->GetYaxis()->GetLabelSize());
      ATLZaxis[i]->SetLabelOffset(0.0);
      ATLZaxis[i]->SetLabelFont(hSCTConstMap[0][0]->GetXaxis()->GetLabelFont());
      ATLZaxis[i]->SetTitleSize(hSCTConstMap[0][0]->GetXaxis()->GetTitleSize());
      ATLZaxis[i]->SetTitleFont(hSCTConstMap[0][0]->GetXaxis()->GetTitleFont());
      ATLZaxis[i]->SetTitle(" z (mm)");

      // Phi axis on right
      ATLPhiaxis[i] = new TGaxis(6.5,-0.5,6.5,31.5,0.,360.,510,"+L");
      ATLPhiaxis[i]->SetName("Z");
      if (i == 0) {ATLPhiaxis[i]->SetY2(31.5);}
      if (i == 1) {ATLPhiaxis[i]->SetY2(39.5);}
      if (i == 2) {ATLPhiaxis[i]->SetY2(47.5);}
      if (i == 3) {ATLPhiaxis[i]->SetY2(55.5);}
      ATLPhiaxis[i]->SetLabelSize(hSCTConstMap[0][0]->GetYaxis()->GetLabelSize());
      ATLPhiaxis[i]->SetLabelOffset(0.0);
      ATLPhiaxis[i]->SetLabelFont(hSCTConstMap[0][0]->GetXaxis()->GetLabelFont());
      ATLPhiaxis[i]->SetTitleSize(hSCTConstMap[0][0]->GetXaxis()->GetTitleSize());
      ATLPhiaxis[i]->SetTitleFont(hSCTConstMap[0][0]->GetXaxis()->GetTitleFont());
      ATLPhiaxis[i]->SetTitle("#phi (deg)");
      ATLPhiaxis[i]->SetLabelOffset(0.01);
      
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
	
	sprintf(name2,"cSCTConstantsMap%s",name);
	sprintf(name3,"SCT Constants Map for %s",name);
	cSCTConstantsMap[j] = new TCanvas(name2,name3,CanvasSizeX4[0],CanvasSizeX4[1]);
	cSCTConstantsMap[j]->Divide(2,2);
	
	for (int i=0; i<m_nSCTBarrels; i++) {
	  cSCTConstantsMap[j]->cd(i+1);
	  hSCTConstMap[i][j]->Draw("COLZ");
	  //	cSCTConstantsMap[j]->Update();
	  
	  // moving the color scale
	  gPad->Update();
	  TPaletteAxis *palette = (TPaletteAxis*)hSCTConstMap[i][j]->GetListOfFunctions()->FindObject("palette");
	  palette->SetX1NDC(0.905);
	  palette->SetX2NDC(0.935);
	  palette->SetY1NDC(0.1);
	  palette->SetY2NDC(0.73);
	  gPad->Modified();
	  
	  setTitleFont(font);
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
void isAlignCorrGood_SCT() {

    float m_Tx,m_Ty,m_Tz,m_Rx,m_Ry,m_Rz;
    treeConsts->SetBranchAddress("Tx", &m_Tx);   
    treeConsts->SetBranchAddress("Ty", &m_Ty);   
    treeConsts->SetBranchAddress("Tz", &m_Tz);   
    treeConsts->SetBranchAddress("Rx", &m_Rx);   
    treeConsts->SetBranchAddress("Ry", &m_Ry);   
    treeConsts->SetBranchAddress("Rz", &m_Rz);  

    int iModules = treeConsts->GetEntries();
    // cout << iModules << endl;

    TH1 *htempConstSCT[6];
    for (int i=0; i<6; i++) {
      if (i==0) sprintf(name,"ConstSCT_Tx");
      else if (i==1) sprintf(name,"ConstSCT_Ty");
      else if (i==2) sprintf(name,"ConstSCT_Tz");
      else if (i==3) sprintf(name,"ConstSCT_Rx");
      else if (i==4) sprintf(name,"ConstSCT_Ry");
      else if (i==5) sprintf(name,"ConstSCT_Rz");
      htempConstSCT[i] = new TH1F(name,name,100,-10,10);
    }

    for (int j=0; j<iModules; j++) {
      treeConsts->GetEntry(j);
      htempConstSCT[0]->Fill(m_Tx);
      htempConstSCT[1]->Fill(m_Ty);
      htempConstSCT[2]->Fill(m_Tz);
      htempConstSCT[3]->Fill(m_Rx);
      htempConstSCT[4]->Fill(m_Ry);
      htempConstSCT[5]->Fill(m_Rz);
    }
    
    TCanvas *cSCT = new TCanvas("test_ConstSCT","test_ConstSCT");
    // cSCT->Divide(3,2);
    for (int j=0; j<6; j++) {
      // cSCT->cd(j+1);
      htempConstSCT[j]->Draw("goff");
      // htemp[j]->Draw();
      if (htempConstSCT[j]->GetMean()!=0.0) isGood[j] = true;

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
    cSCT->Close();

  return;
}
