/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

Int_t NModInLayer = 32;
//////////////////////////////////////////////////
void SiCluster(const char *chop="nofit") 
  //////////////////////////////////////////////////
{
  if (!PixelOnly && doSiCluster) {

    const Int_t NSCT_Layers = 4;
    Bool_t DoFit = true;

    Int_t MarkColor = kBlack;
    if (level == 3) MarkColor = kCyan;
    if (level == 4) MarkColor = kBlue+2;
    if (level == 5) MarkColor = kRed-3;

    // fit function
    TF1 *Ajust = new TF1("CLUSIN",sinx_fitfun, 0.,0.,3);
    Ajust->SetParName(0,"Average");
    Ajust->SetParName(1,"Amplitude");
    Ajust->SetParName(2,"Phase");
    //------------
    // SCT Barrel
    //------------
    cout << "   - SiCluster - SCT Barrel..." << endl;
  
    TProfile *pSCTClusSize[NSCT_Layers];
    TProfile *pSCTClusSizeIncAngle;
    Float_t MaxCluSize = -99999.; // start with a funny value
    Float_t MinCluSize = 0.90; // lower limit of the Y scale

    // import histograms and get absolute maximum to plot them later with the same scale
    for(int layer = 0; layer<NSCT_Layers; layer++){
      sprintf(name,"trkana/SiCluster/pSCTClusSizeLayer%d_%d",layer,level);
      pSCTClusSize[layer] = (TProfile*)file->Get(name);
      if (pSCTClusSize[layer]==NULL) cout << " -> xungo. No pilla pSCTClusSize["<<layer<<"] = " << name << endl;
      Float_t thisMax =  pSCTClusSize[layer]->GetBinContent(pSCTClusSize[layer]->GetMaximumBin());
      if (thisMax > MaxCluSize) MaxCluSize = 1.02*thisMax;
      pSCTClusSize[layer]->SetYTitle("Average cluster size");
    }

    MaxCluSize = 3.5;

    // open a new canvas
    cSiClusSize = new TCanvas("SiClusterSize","SiClusterSize",
			      CanvasSizeX2[0],CanvasSizeX2[1],
			      CanvasSizeX2[2],CanvasSizeX2[3]);
    cSiClusSize->Divide(2,2);
  
    // define axis
    TGaxis *SectorAxis[4]; 
  
    // lines
    TLine *Line090[NSCT_Layers];
    TLine *Line180[NSCT_Layers];
    TLine *Line270[NSCT_Layers];

    // draw the profiles
    for(int layer = 0; layer<NSCT_Layers; layer++){

      cSiClusSize->cd(layer+1);

      // fit 
      if (DoFit && layer==3) {
	if (layer==0) NModInLayer=32;
	if (layer==1) NModInLayer=40;
	if (layer==2) NModInLayer=48;
	if (layer==3) NModInLayer=56; 
	Ajust->SetParameters(2., 0.2, 0.0);
	Ajust->SetLineStyle(2);
	Ajust->SetLineWidth(1);
	Ajust->SetLineColor(MarkColor);
	pSCTClusSize[layer]->Fit(Ajust,"Qsame");
      }

      pSCTClusSize[layer]->SetStats(kFALSE);
      pSCTClusSize[layer]->SetMaximum(MaxCluSize);
      pSCTClusSize[layer]->SetMinimum(MinCluSize);
      pSCTClusSize[layer]->SetMarkerStyle(20);
      pSCTClusSize[layer]->SetMarkerSize(1.0);
      pSCTClusSize[layer]->SetMarkerColor(MarkColor);
      pSCTClusSize[layer]->SetLineColor(MarkColor);
      pSCTClusSize[layer]->Draw("X+");
      gPad->Update();
      setTitleFont(font);

      // 
      // Degrees frame
      Float_t x090 = 0.25*(pSCTClusSize[layer]->GetXaxis()->GetXmax()-pSCTClusSize[layer]->GetXaxis()->GetXmin());
      Float_t x180 = 0.50*(pSCTClusSize[layer]->GetXaxis()->GetXmax()-pSCTClusSize[layer]->GetXaxis()->GetXmin());
      Float_t x270 = 0.75*(pSCTClusSize[layer]->GetXaxis()->GetXmax()-pSCTClusSize[layer]->GetXaxis()->GetXmin());
      Line090[layer] = new TLine( x090, MinCluSize,  x090, MaxCluSize); Line090[layer]->SetLineStyle(3);
      Line090[layer]->Draw();
      Line180[layer] = new TLine( x180, MinCluSize,  x180, MaxCluSize); Line180[layer]->SetLineStyle(3);
      Line180[layer]->Draw();
      Line270[layer] = new TLine( x270, MinCluSize,  x270, MaxCluSize); Line270[layer]->SetLineStyle(3);
      Line270[layer]->Draw();
      // draw an axis
      SectorAxis[layer] = new TGaxis(-0.5,MinCluSize,31.5,MinCluSize,0.,360.,510,"+");
      SectorAxis[layer]->SetName("#phi (deg)");
      if (layer == 0) {SectorAxis[layer]->SetX1(-0.5);SectorAxis[layer]->SetX2(31.5);}
      if (layer == 1) {SectorAxis[layer]->SetX1(-0.5);SectorAxis[layer]->SetX2(39.5);}
      if (layer == 2) {SectorAxis[layer]->SetX1(-0.5);SectorAxis[layer]->SetX2(47.5);}
      if (layer == 3) {SectorAxis[layer]->SetX1(-0.5);SectorAxis[layer]->SetX2(55.5);}
      SectorAxis[layer]->SetLabelSize(pSCTClusSize[layer]->GetYaxis()->GetLabelSize());
      SectorAxis[layer]->SetLabelOffset(0.0);
      SectorAxis[layer]->SetLabelFont(pSCTClusSize[layer]->GetXaxis()->GetLabelFont());
      SectorAxis[layer]->SetTitleSize(pSCTClusSize[layer]->GetXaxis()->GetTitleSize());
      SectorAxis[layer]->SetTitleFont(pSCTClusSize[layer]->GetXaxis()->GetTitleFont());
      SectorAxis[layer]->SetTitle("#phi (deg)");
      SectorAxis[layer]->Draw();
    }  

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Cluster size vs incident angle
    ///////////////////////////////////////////////////////////////////////////////////////////////
    sprintf(name,"trkana/SiCluster/pSCTClusSizeIncAngle_%d",level);
    pSCTClusSizeIncAngle = (TProfile*)file->Get(name);
    pSCTClusSizeIncAngle->SetYTitle("Average cluster size");
    
    cSiClusSizeIncAng = new TCanvas("SiClusterSizeIncAng","SiClusterSizeIngAng",
				    CanvasSizeX2[0],CanvasSizeX2[1],
				    CanvasSizeX2[2],CanvasSizeX2[3]);
    cSiClusSizeIncAng->Divide(1,1);
    pSCTClusSizeIncAngle->SetStats(kFALSE);
    pSCTClusSizeIncAngle->SetMaximum(MaxCluSize);
    pSCTClusSizeIncAngle->SetMinimum(MinCluSize);
    pSCTClusSizeIncAngle->SetMarkerStyle(20);
    pSCTClusSizeIncAngle->SetMarkerSize(1.0);
    pSCTClusSizeIncAngle->SetMarkerColor(MarkColor);
    pSCTClusSizeIncAngle->SetLineColor(MarkColor);
    pSCTClusSizeIncAngle->Draw();
    setTitleFont(font);
  }

  return;
}

//////////////////////////////////////////////////
double sinx_fitfun(double *x, double *par)
{
  double Valor;
  const double Mod2Rad = 6.283185307/NModInLayer;

  double Average =  par[0];
  double Amplitude = par[1];
  double Phase = par[2];

  Valor = Average + Amplitude+sin(2*Mod2Rad*x[0]+Phase);

  return Valor;
}
