/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


const int NPIX_Layers = 3;
const int NPIX_Discs  = 3;
const int NSCT_Layers = 4;
const int NSCT_Sides=2; // el modul del SCT te dos cares
const int NSCT_Discs = 9;

float MaxMeanRange;
float MinMeanRange;
const Int_t NContLevels = 50;
Int_t RM_ColorIndex[NContLevels];
float RM_Levels[NContLevels];
//
void ResidusMap(char nname[80], int side=0, float MeanRange=5, 
		float SigmaRangeMax=50, float SigmaRangeMin=10.) {
  int level = 1;
  
  bool  fent_SCT = 1;
  bool  fent_PIX = 1;

  MaxMeanRange = MeanRange;
  MinMeanRange = -MeanRange;

  Set_style();
  Set_ColorScale();
  // Print_ColorScale();

  TFile *f0 = new TFile(nname);
  TH2F *HD[NSCT_Discs];

  char nameh[80];
  char xCommand[80];

  ////////////////////
  // SCT           //
  ////////////////////
  if (fent_SCT) {
    /////////////////////
    // Barrel
    /////////////////////
    // mitjana
    TCanvas *CMapResMean = new TCanvas("CMapResMean","Map of SCT residuals mean",0,100,1200,800); 
    CMapResMean->Divide(2,2);
    
    for (int layer = 0; layer < NSCT_Layers; layer++){
      sprintf(nameh,"trkana/SCTRes/hSCTResMeanMapBaLay%d_s%d_%d",layer,side,level);
      HD[layer] = (TH2F*)f0->Get(nameh); 
      HD[layer]->Scale(1000.);
      HD[layer]->SetContour(NContLevels);
      HD[layer]->SetMaximum(MaxMeanRange);
      HD[layer]->SetMinimum(MinMeanRange);
      HD[layer]->SetStats(kFALSE);
      CMapResMean->cd(layer+1);
      HD[layer]->Draw("colz");
    }
    
    // sigma
    TCanvas *CMapResSigma = new TCanvas("CMapResSigma","Map of SCT residuals sigma",0,425,1400,300); 
    CMapResSigma->Divide(4,1);
    
    for (int layer = 0; layer < NSCT_Layers; layer++){
      //    for (int side=0; side<NSCT_Sides; side++) {      
      sprintf(nameh,"trkana/SCTRes/hSCTResSigmaMapBaLay%d_s%d_%d",layer,side,level);
      HD[layer] = (TH2F*)f0->Get(nameh); 
      HD[layer]->Scale(1000.);
      HD[layer]->SetContour(SigmaRangeMax);
      HD[layer]->SetMaximum(SigmaRangeMax);
      HD[layer]->SetMinimum(SigmaRangeMin);
      HD[layer]->SetStats(kFALSE);
      CMapResSigma->cd(layer+1);
      HD[layer]->Draw("colz");
    }

    /////////////////////
    // SCT Endcap A
    /////////////////////
    char namecan[80];
    sprintf(namecan,"Map of SCT residuals mean End Cap A side %d",side);

    TCanvas *CMapSCTResMeanEA 
      = new TCanvas("CMapSCTResMeanEA",namecan,0,200,900,900); 

    CMapSCTResMeanEA->Divide(3,3);
    for (int disc = 0; disc < NSCT_Discs; disc++){
      sprintf(nameh,"trkana/SCTRes/hSCTResMeanMapEaLay%d_s%d_%d",disc,side,level);
      HD[disc] = (TH2F*)f0->Get(nameh); 
      HD[disc]->Scale(1000.);
      HD[disc]->SetContour(NContLevels);
      HD[disc]->SetMaximum(MaxMeanRange);
      HD[disc]->SetMinimum(MinMeanRange);
      HD[disc]->SetStats(kFALSE);
      CMapSCTResMeanEA->cd(disc+1);
      HD[disc]->Draw("colztext");
    }

    // pintar discs (a la Sergio)
    TCanvas *CSCTEaDisk = new TCanvas("CSCTEaDisk",namecan,100,100,900,900); 
    CSCTEaDisk->Divide(3,3);
    
    TBox *blank = new TBox(-10,-10, 10,10);
    blank->SetFillColor(kWhite);

    Int_t PadIndex=0;

    for(int disc=0; disc<NSCT_Discs; disc++){
      if (1==0) cout << "  << EndCap A >> disc " << disc << endl;
      PadIndex++;
      CSCTEaDisk->cd(PadIndex);            
      draw_wheel_frame("SCT","A",disc,level);
      blank->Draw();
      //gPad->RedrawAxis();
      sprintf(xCommand,".x DrawSCTECModules.C(HD[%d],%d)",disc,disc);
      gROOT->ProcessLine(xCommand);
      //gPad->Update();
    }

    /////////////////////
    // SCT Endcap C
    /////////////////////
    char namecan[80];
    sprintf(namecan,"Map of SCT residuals mean End Cap C side %d",side);

    TCanvas *CMapSCTResMeanEC 
      = new TCanvas("CMapSCTResMeanEC",namecan,200,100,900,900); 

    CMapSCTResMeanEC->Divide(3,3);
    for (int disc = NSCT_Discs-1; disc>=0; disc--){
      sprintf(nameh,"trkana/SCTRes/hSCTResMeanMapEcLay%d_s%d_%d",disc,side,level);
      HD[disc] = (TH2F*)f0->Get(nameh); 
      HD[disc]->Scale(1000.);
      HD[disc]->SetContour(NContLevels);
      HD[disc]->SetMaximum(MaxMeanRange);
      HD[disc]->SetMinimum(MinMeanRange);
      HD[disc]->SetStats(kFALSE);
      CMapSCTResMeanEC->cd(disc+1);
      HD[disc]->Draw("colz");
    }

    // pintar discs (a la Sergio)
    TCanvas *CSCTEcDisk = new TCanvas("CSCTEcDisk",namecan,100,100,900,900); 
    CSCTEcDisk->Divide(3,3);
    
    TBox *blank = new TBox(-10,-10, 10,10);
    blank->SetFillColor(kWhite);

    PadIndex=0;
    for(int disc=NSCT_Discs-1; disc>=0; disc--){
      if (1==0) cout << "  << EndCap C >> disc " << disc << endl;
      PadIndex++;
      CSCTEcDisk->cd(PadIndex);            
      draw_wheel_frame("SCT","C",disc,level);
      blank->Draw();
      //gPad->RedrawAxis();
      sprintf(xCommand,".x DrawSCTECModules.C(HD[%d],%d)",disc,disc);
      gROOT->ProcessLine(xCommand);
      //gPad->Update();
    }
  }

  ////////////////////
  // PIX           //
  ////////////////////
  if (fent_PIX) {
    /////////////////////
    // Barrel
    /////////////////////    

    // PHI residuals

    // mitjana
    TCanvas *CMapPIXResMean = new TCanvas("CMapPIXResMean","Map of PIX residuals mean Barrel",0,300,1000,300); 
    CMapPIXResMean->Divide(3,1);
    
    for (int layer = 0; layer < NPIX_Layers; layer++){
      sprintf(nameh,"trkana/PIXRes/hPIXResMeanMapBaLay%d_%d",layer,level);
      HD[layer] = (TH2F*)f0->Get(nameh); 
      HD[layer]->Scale(1000.);
      HD[layer]->SetContour(NContLevels);
      HD[layer]->SetMaximum(MaxMeanRange);
      HD[layer]->SetMinimum(MinMeanRange);
      HD[layer]->SetStats(kFALSE);
      CMapPIXResMean->cd(layer+1);
      HD[layer]->Draw("colz");
    }
    
    // sigma
    TCanvas *CMapPIXResSigma = new TCanvas("CMapPIXResSigma","Map of PIX residuals sigma Barrel",0,700,1000,300); 
    CMapPIXResSigma->Divide(3,1);
    
    for (int layer = 0; layer < NPIX_Layers; layer++){
      //    for (int side=0; side<NSCT_Sides; side++) {      
      sprintf(nameh,"trkana/PIXRes/hPIXResSigmaMapBaLay%d_%d",layer,level);
      HD[layer] = (TH2F*)f0->Get(nameh); 
      HD[layer]->Scale(1000.);
      HD[layer]->SetContour(SigmaRangeMax);
      HD[layer]->SetMaximum(SigmaRangeMax);
      HD[layer]->SetMinimum(SigmaRangeMin);
      HD[layer]->SetStats(kFALSE);
      CMapPIXResSigma->cd(layer+1);
      HD[layer]->Draw("colz");
      //    }
    }


    // ETA residuals
    if (f0->FindObjectAny("hPIXEtaResMeanMapBaLay0_0")) {  // First one should chek the histograms exist

      // mitjana
      TCanvas *CMapPIXEtaResMean = new TCanvas("CMapPIXEtaResMean","Map of PIX Eta residuals mean Barrel",0,300,1000,300); 
      CMapPIXEtaResMean->Divide(3,1);
      
      for (int layer = 0; layer < NPIX_Layers; layer++){
	sprintf(nameh,"trkana/PIXRes/hPIXEtaResMeanMapBaLay%d_%d",layer,level);
	HD[layer] = (TH2F*)f0->Get(nameh); 
	HD[layer]->Scale(1000.);
	HD[layer]->SetContour(NContLevels);
	HD[layer]->SetMaximum(MaxMeanRange);
	HD[layer]->SetMinimum(MinMeanRange);
	HD[layer]->SetStats(kFALSE);
	CMapPIXEtaResMean->cd(layer+1);
	HD[layer]->Draw("colz");
      }
      
      // sigma
      TCanvas *CMapPIXEtaResSigma = new TCanvas("CMapPIXEtaResSigma","Map of PIX Eta residuals sigma Barrel",0,700,1000,300); 
      CMapPIXEtaResSigma->Divide(3,1);
      
      for (int layer = 0; layer < NPIX_Layers; layer++){
	//    for (int side=0; side<NSCT_Sides; side++) {      
	sprintf(nameh,"trkana/PIXRes/hPIXEtaResSigmaMapBaLay%d_%d",layer,level);
	HD[layer] = (TH2F*)f0->Get(nameh); 
	HD[layer]->Scale(1000.);
	HD[layer]->SetContour(SigmaRangeMax);
	HD[layer]->SetMaximum(SigmaRangeMax);
	HD[layer]->SetMinimum(SigmaRangeMin);
	HD[layer]->SetStats(kFALSE);
	CMapPIXEtaResSigma->cd(layer+1);
	HD[layer]->Draw("colz");
	//    }
      }
    } // End of Eta residuals

    /////////////////////
    // Pixel Endcap A
    /////////////////////
    TCanvas *CMapPIXResMeanEA = new TCanvas("CMapPIXResMeanEA","Map of PIX residuals mean End Cap A",0,300,1000,300); 
    CMapPIXResMeanEA->Divide(3,1);
    int PadIndex=0;
    for (int layer = 0; layer < NPIX_Layers; layer++){
      sprintf(nameh,"trkana/PIXRes/hPIXResMeanMapEaLay%d_%d",layer,level);
      HD[layer] = (TH2F*)f0->Get(nameh); 
      HD[layer]->Scale(1000.);
      HD[layer]->SetContour(NContLevels);
      HD[layer]->SetMaximum(MaxMeanRange);
      HD[layer]->SetMinimum(MinMeanRange);
      HD[layer]->SetStats(kFALSE);
      CMapPIXResMeanEA->cd(layer+1);
      HD[layer]->Draw("colztext");
    }

    // pintar discs (a la Sergio)
    TCanvas *CPIXEaDisk = new TCanvas("CPIXEaDisk","Pixel Endcap A Residual Map",300,600,900,300); 
    CPIXEaDisk->Divide(3,1);
    
    TBox *blank = new TBox(-10,-10, 10,10);
    blank->SetFillColor(kWhite);

    PadIndex=0;

    for(int disc=0; disc<NPIX_Discs; disc++){
      if (1==0) cout << "  << EndCap A >> disc " << disc << endl;
      PadIndex++;
      CPIXEaDisk->cd(PadIndex);            
      draw_wheel_frame("PIX","A",disc,level);
      blank->Draw();
      //gPad->RedrawAxis();
      sprintf(xCommand,".x DrawPIXECModules.C(HD[%d])",disc);
      gROOT->ProcessLine(xCommand);
      //gPad->Update();
    }

    /////////////////////
    // Pixel Endcap C
    /////////////////////
    TCanvas *CMapPIXResMeanEC = 
      new TCanvas("CMapPIXResMeanEC","Map of PIX residuals mean End Cap C",100,400,1000,300); 
    CMapPIXResMeanEC->Divide(3,1);

    PadIndex=0;
    for (int layer = NPIX_Discs-1; layer >= 0; layer--){
      sprintf(nameh,"trkana/PIXRes/hPIXResMeanMapEcLay%d_%d",layer,level);
      HD[layer] = (TH2F*)f0->Get(nameh); 
      HD[layer]->Scale(1000.);
      HD[layer]->SetContour(NContLevels);
      HD[layer]->SetMaximum(MaxMeanRange);
      HD[layer]->SetMinimum(MinMeanRange);
      HD[layer]->SetStats(kFALSE);
      PadIndex++;
      CMapPIXResMeanEC->cd(PadIndex);
      HD[layer]->Draw("colz");
    }

    // pintar discs (a la Sergio)
    TCanvas *CPIXEcDisk = new TCanvas("CPIXEcDisk","Pixel Endcap C Residual Map",200,500,900,300); 
    CPIXEcDisk->Divide(3,1);

    TBox *blank = new TBox(-10,-10, 10,10);
    blank->SetFillColor(kWhite);

    PadIndex=0;

    for(int disc=NPIX_Discs-1; disc>=0; disc--){
      if (1==0) cout << "  << EndCap C >> disc " << disc << endl;
      PadIndex++;
      CPIXEcDisk->cd(PadIndex);            
      draw_wheel_frame("PIX","C",disc,level);
      blank->Draw();
      //gPad->RedrawAxis();
      sprintf(xCommand,".x DrawPIXECModules.C(HD[%d])",disc);
      gROOT->ProcessLine(xCommand);
      //gPad->Update();
    }    
  }// End of PIX
}
//////////////////////////////////////////////////////
void draw_wheel_frame(const char detec[3], const char endcap[1], Int_t disc, Int_t level)
//////////////////////////////////////////////////////
{
  char nameh[80];

  sprintf(nameh,"%s EndCap %c Disk %d Residual Map (level %d)",detec, endcap[0], disc,level);

  TH2F *PixFrame = new TH2F(nameh, nameh, 100, -10, 10, 100, -10, 10);
  PixFrame->SetStats(kFALSE);
  PixFrame->Fill(-5,5,MaxMeanRange);
  PixFrame->Fill(5,-5,MinMeanRange);
  PixFrame->GetXaxis()->SetAxisColor(kWhite);
  PixFrame->GetYaxis()->SetAxisColor(kWhite);
  PixFrame->GetXaxis()->SetLabelColor(kWhite);
  PixFrame->GetYaxis()->SetLabelColor(kWhite);
  PixFrame->GetZaxis()->SetLabelSize(0.035);      
  PixFrame->SetContour(NContLevels);
  PixFrame->SetMaximum(MaxMeanRange);
  PixFrame->SetMinimum(MinMeanRange);
  PixFrame->Draw("colz");
}

/////////////////////////////////////////////////////////////////////
int locateColor(const int ncols, float min, float max, float cont){
/////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////
void Set_style()
{
  // style
  gStyle->SetPalette(1,0);
  gStyle->SetCanvasColor(0);
  //gStyle->SetOptFit(10);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(10);
  //  gStyle->SetStatFontSize(0);
  gStyle->SetStatColor(0);
  gStyle->SetStatW(0.22);
  //gStyle->SetStatH(0.22);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderMode(0);
}
//////////////////////////////////////////////////
void Set_ColorScale()
{
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
//////////////////////////////////////////////////
void Print_ColorScale()
{

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


}
