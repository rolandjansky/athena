/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TString.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TColor.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPolyLine.h>
#include <TPaletteAxis.h>
#include <TMath.h>
#include <TH1.h>
#include <TF1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>

// Global variables
bool AtlasStyle;
bool normalizePlots;
bool showFits;
int PrintLevel;
TString FirstTrackCollection ="";
TString SecondTrackCollection ="";
TString CosmicSegmentsCollection = "";
TString FileName ="";
TFile *file;
TDirectory *IDAlignMonDir;
TDirectory *FirstTrkSelDir;
TDirectory *SecondTrkSelDir;

Int_t font = 42;
// Canvas sizes
const Int_t CanvasSizeX2[4] = {0,10,700,300};
const Int_t CanvasSizeX3[2] = {1180,400};
const Int_t CanvasSizeX4[2] = {1024,768};
const Int_t CanvasSizeX6[2] = {1150,700};
const Int_t CanvasSizeX9[4] = {0,450,925,700};

// Colors
Color_t ColorForFirst;
Color_t ColorForSecond;

// Summary
TString aSetup;
bool iCosmics;
Float_t iBField;
Int_t iEvents;
Int_t iTracks_1;
Int_t iTracks_2;
Int_t IterNumber;
Int_t iHits_1,iPixelHits_1,iSCTHits_1,iTRTHits_1;
Int_t iHits_2,iPixelHits_2,iSCTHits_2,iTRTHits_2;
// Canvas
TCanvas *cInfoRes;
TCanvas *cHits;
TCanvas *cHitCorr;
TCanvas *cHitsPerLayer;
TCanvas *cPIXHitMap;
TCanvas *cPIXHitMap1;
TCanvas *cPIXHitMap2;
TCanvas *cPixRes;
TCanvas *cPixRes_1;
TCanvas *cPixRes_2;
TCanvas *cPixRes_3;
TCanvas *cSctTRTRes;
TCanvas *cSctTRTRes_1;
TCanvas *cSctTRTRes_2;
TCanvas *cSctTRTRes_3;
TCanvas *cSCTHitMapBa;
TCanvas *cSCTHitMapBa1;
TCanvas *cSCTHitMapBa2;
TCanvas *cSCTHitMapEC[2];
TCanvas *cSCTHitMapEC1[2];
TCanvas *cSCTHitMapEC2[2];
TCanvas *cPIXBarrelRes;
TCanvas *cPIXBarrelRes_1;
TCanvas *cPIXBarrelRes_2;
TCanvas *cPIXBarrelRes_3;
TCanvas *cPIXBarrelResVsPt;
TCanvas *cPIXBarrelResVsPt_1;
TCanvas *cPIXBarrelResVsPt_2;
TCanvas *cPIXBarrelResVsPt_3;
TCanvas *cPIXECRes;
TCanvas *cPIXECARes_1;
TCanvas *cPIXECARes_2;
TCanvas *cPIXECARes_3;
TCanvas *cPIXECCRes_1;
TCanvas *cPIXECCRes_2;
TCanvas *cPIXECCRes_3;
TCanvas *cPIXECResVsPt;
TCanvas *cPIXECAResVsPt_1;
TCanvas *cPIXECAResVsPt_2;
TCanvas *cPIXECAResVsPt_3;
TCanvas *cPIXECCResVsPt_1;
TCanvas *cPIXECCResVsPt_2;
TCanvas *cPIXECCResVsPt_3;
TCanvas *cSCTBarrelRes;
TCanvas *cSCTBarrelRes_1;
TCanvas *cSCTBarrelRes_2;
TCanvas *cSCTBarrelRes_3;
TCanvas *cSCTBarrelResVsPt;
TCanvas *cSCTBarrelResVsPt_1;
TCanvas *cSCTBarrelResVsPt_2;
TCanvas *cSCTBarrelResVsPt_3;
TCanvas *cSCTECRes;
TCanvas *cSCTECARes_1;
TCanvas *cSCTECARes_2;
TCanvas *cSCTECARes_3;
TCanvas *cSCTECCRes_1;
TCanvas *cSCTECCRes_2;
TCanvas *cSCTECCRes_3;
TCanvas *cSCTECResVsPt;
TCanvas *cSCTECAResVsPt_1;
TCanvas *cSCTECAResVsPt_2;
TCanvas *cSCTECAResVsPt_3;
TCanvas *cSCTECCResVsPt_1;
TCanvas *cSCTECCResVsPt_2;
TCanvas *cSCTECCResVsPt_3;
TCanvas *cPIXResMap;
TCanvas *cPIXRMSPhiResMap_1;
TCanvas *cPIXMeanPhiResMap_1; 
TCanvas *cPIXRMSEtaResMap_1;
TCanvas *cPIXMeanEtaResMap_1; 
TCanvas *cPIXRMSPhiResMap_2;
TCanvas *cPIXMeanPhiResMap_2;
TCanvas *cPIXRMSEtaResMap_2;
TCanvas *cPIXMeanEtaResMap_2;
TCanvas *cSCTBaResMap;
TCanvas *cSCTBaRMSSide0ResMap_1;
TCanvas *cSCTBaMeanSide0ResMap_1; 
TCanvas *cSCTBaRMSSide1ResMap_1;
TCanvas *cSCTBaMeanSide1ResMap_1; 
TCanvas *cSCTBaRMSSide0ResMap_2;
TCanvas *cSCTBaMeanSide0ResMap_2;
TCanvas *cSCTBaRMSSide1ResMap_2;
TCanvas *cSCTBaMeanSide1ResMap_2;

TCanvas *cSCTResMapEC;
TCanvas *cSCTECCRMSSide0ResMap_1;
TCanvas *cSCTECCRMSSide1ResMap_1;
TCanvas *cSCTECCMeanSide0ResMap_1;
TCanvas *cSCTECCMeanSide1ResMap_1;
TCanvas *cSCTECARMSSide0ResMap_1;
TCanvas *cSCTECARMSSide1ResMap_1;
TCanvas *cSCTECAMeanSide0ResMap_1;
TCanvas *cSCTECAMeanSide1ResMap_1;
TCanvas *cSCTECCRMSSide0ResMap_2;
TCanvas *cSCTECCRMSSide1ResMap_2;
TCanvas *cSCTECCMeanSide0ResMap_2;
TCanvas *cSCTECCMeanSide1ResMap_2;
TCanvas *cSCTECARMSSide0ResMap_2;
TCanvas *cSCTECARMSSide1ResMap_2;
TCanvas *cSCTECAMeanSide0ResMap_2;
TCanvas *cSCTECAMeanSide1ResMap_2;

TCanvas *cTrkParameters;
TCanvas *cTrkParameters_1;
TCanvas *cTrkParameters_2;
TCanvas *cTrkParameters_3;

TCanvas *cTrkMomentum;
TCanvas *cTrkMomentum_1;
TCanvas *cTrkMomentum_2;
TCanvas *cTrkMomentum_3;

TCanvas *cErrD0;
TCanvas *cErrD0_1;
TCanvas *cErrD0_2;
TCanvas *cErrD0_3;

TCanvas *cErrD0VsPhi0;
TCanvas *cErrD0VsPhi0_1;
TCanvas *cErrD0VsPhi0_2;
TCanvas *cErrD0VsPhi0_3;

TCanvas *cErrPt;
TCanvas *cErrPt_1;
TCanvas *cErrPt_2;
TCanvas *cErrPt_3;

TCanvas *cErrPhi0;
TCanvas *cErrPhi0_1;
TCanvas *cErrPhi0_2;
TCanvas *cErrPhi0_3;

TCanvas *cErrCotTheta;
TCanvas *cErrCotTheta_1;
TCanvas *cErrCotTheta_2;
TCanvas *cErrCotTheta_3;

TCanvas *cErrZ0;
TCanvas *cErrZ0_1;
TCanvas *cErrZ0_2;
TCanvas *cErrZ0_3;

TCanvas *cCosmicSegments[5];

//void Monitor(TString inputFileName="",TString inputFirstTrackCollection="ExtendedTracks_all",TString inputSecondTrackCollection="AlignTracks_all");
//void Monitor(TString inputFileName="",TString inputFirstTrackCollection="ExtendedTracks_alignSelection",TString inputSecondTrackCollection="AlignTracks_all");
void Monitor(TString inputFileName="",TString inputFirstTrackCollection="AlignTracks_all",TString inputSecondTrackCollection="");
bool OpenFile();
void setStyle();
void ScalePlots(Int_t position = 0);
void setTitleFont(Int_t font=62);
void setTitleSize(double x2Size = 0.90);
void ForceStyle(TH1F* histoIn1D);
void SetHistoAtt(TH1 *histo);
void SetHistoAtt(TH2 *histo);
int locateColor(const int ncols, float min, float max, float cont);
void draw_sct_ec_modules(TH2F *histo, int disc);
void draw_pixel_ec_modules(TH1F *histo);
void draw_sct_ec_modules(TH2F *histo, int disc);
void draw_wheel_frame(TString name, float minrange, float maxrange);
void SetZAxisTitle(TH2F *histo2D,
	                 Float_t X1=0.82, Float_t X2=0.89,
	                 Float_t Y1=0.1, Float_t Y2=0.87);
	                 
void moveTPaveStats(TH2F* histoIn2D);
void fillSummary();
void drawPlots();
void residuals(TString trkCol);
void hits(TString trkCol_1, TString trkCol_2);
void hitCorrelations(TString trkCol_1, TString trkCol_2);
void pixelHitmaps(TString TrkCol, TString DrawOptions="");
void pixelHitmaps(TString TrkCol, TCanvas *cPIXHitMap, TString DrawOptions="");
void sctBarrelHitmap(TString TrkCol, TCanvas *cSCTHitMapBa,	TString DrawOptions ="");
void sctBarrelHitmap(TString TrkCol, TString DrawOptions ="");
void sctBarrelHitmap(TString TrkCol);
void sctECHitmap(TString TrkCol, int ec, TCanvas *cSCTHitMapEC);
void sctECHitmap(TString TrkCol, int ec);
void SCTMaps_SetXaxisLabels(TH2F *histoMap);

void draw_pixel_ec_modules(TH1F *histo);
void hitsPerLayer(TString trkCol_1, TString trkCol_2); 
void pixelResiduals(TString trkCol);
void pixelResiduals(TString trkCol, TString trkCol2);
void pixelResiduals(TString trkCol, TCanvas *cPixRes);
void pixelResiduals(TString trkCol, TCanvas *cPixRes, TString trkCol2);
void sct_trt_Residuals(TString trkCol);
void sct_trt_Residuals(TString trkCol, TString trkCol2);
void sct_trt_Residuals(TString trkCol, TCanvas *cPixRes);
void sct_trt_Residuals(TString trkCol, TCanvas *cPixRes, TString trkCol2);
void pixelBarrelResiduals(TString trkCol);
void pixelBarrelResiduals(TString trkCol, TString trkCol2);
void pixelBarrelResiduals(TString trkCol, TCanvas *cPixRes);
void pixelBarrelResiduals(TString trkCol, TCanvas *cPixRes, TString trkCol2);
void pixelBarrelResiduals(TString trkCol);
void pixelBarrelResidualsVsPt(TString trkCol, TString trkCol2);
void pixelBarrelResidualsVsPt(TString trkCol, TCanvas *cPIXBarrelRes);
void pixelBarrelResidualsVsPt(TString trkCol, TCanvas *cPIXBarrelRes, TString trkCol2);
void pixelBarrelResidualsVsPt(TString trkCol);
void pixelECResiduals(TString trkCol, bool ec);
void pixelECResiduals(TString trkCol, bool ec, TString trkCol2);
void pixelECResiduals(TString trkCol, bool ec, TCanvas *cPIXECRes);
void pixelECResiduals(TString trkCol, bool ec, TCanvas *cPIXECRes, TString trkCol2);
void pixelECResidualsVsPt(TString trkCol, bool ec);
void pixelECResidualsVsPt(TString trkCol, bool ec, TString trkCol2);
void pixelECResidualsVsPt(TString trkCol, bool ec, TCanvas *cPIXECResVsPt);
void pixelECResidualsVsPt(TString trkCol, bool ec, TCanvas *cPIXECResVsPt, TString trkCol2);
void sctBarrelResiduals(TString trkCol);
void sctBarrelResiduals(TString trkCol, TString trkCol2);
void sctBarrelResiduals(TString trkCol, TCanvas *cPixRes);
void sctBarrelResiduals(TString trkCol, TCanvas *cPixRes, TString trkCol2);
void sctBarrelResiduals(TString trkCol);
void sctBarrelResidualsVsPt(TString trkCol, TString trkCol2);
void sctBarrelResidualsVsPt(TString trkCol, TCanvas *cPIXBarrelRes);
void sctBarrelResidualsVsPt(TString trkCol, TCanvas *cPIXBarrelRes, TString trkCol2);
void sctBarrelResidualsVsPt(TString trkCol);
void sctECResiduals(TString trkCol, bool ec);
void sctECResiduals(TString trkCol, bool ec, TString trkCol2);
void sctECResiduals(TString trkCol, bool ec, TCanvas *cSCTECRes);
void sctECResiduals(TString trkCol, bool ec, TCanvas *cSCTECRes, TString trkCol2);
void sctECResidualsVsPt(TString trkCol, bool ec);
void sctECResidualsVsPt(TString trkCol, bool ec, TString trkCol2);
void sctECResidualsVsPt(TString trkCol, bool ec, TCanvas *cSCTECResVsPt);
void sctECResidualsVsPt(TString trkCol, bool ec, TCanvas *cSCTECResVsPt, TString trkCol2);
void pixelResmaps(TString TrkCol, bool isXres, bool isRMS, float range = 50);
void pixelResmaps(TString TrkCol, TCanvas *cPIXResMap, bool isXres, bool isRMS, float range = 50);
void sctBarrelResmaps(TString TrkCol, bool side, bool isRMS, float range = 50);
void sctBarrelResmaps(TString TrkCol, TCanvas *cSCTBaResMap, bool side, bool isRMS, float range = 50);
void sctECResmaps(TString TrkCol, bool ec, bool side, bool isRMS, float range);
void sctECResmaps(TString TrkCol, TCanvas *cSCTResMapEC, bool ec, bool side, bool isRMS, float range);
void trkParameters(TString trkCol);
void trkParameters(TString trkCol, TString trkCol2);
void trkParameters(TString trkCol, TCanvas *cTrkParameters);
void trkParameters(TString trkCol, TCanvas *cTrkParameters, TString trkCol2);

void trkMomentum(TString trkCol);
void trkMomentum(TString trkCol, TString trkCol2);
void trkMomentum(TString trkCol, TCanvas *cTrkMomentum);
void trkMomentum(TString trkCol, TCanvas *cTrkMomentum, TString trkCol2);

void errD0(TString trkCol);
void errD0(TString trkCol, TString trkCol2);
void errD0(TString trkCol, TCanvas *cErrD0);
void errD0(TString trkCol, TCanvas *cErrD0, TString trkCol2);

void errD0VsPhi0(TString trkCol);
void errD0VsPhi0(TString trkCol, TString trkCol2);
void errD0VsPhi0(TString trkCol, TCanvas *cErrD0VsPhi0);
void errD0VsPhi0(TString trkCol, TCanvas *cErrD0VsPhi0, TString trkCol2);

void errPt(TString trkCol);
void errPt(TString trkCol, TString trkCol2);
void errPt(TString trkCol, TCanvas *cErrPt);
void errPt(TString trkCol, TCanvas *cErrPt, TString trkCol2);

void errPhi0(TString trkCol);
void errPhi0(TString trkCol, TString trkCol2);
void errPhi0(TString trkCol, TCanvas *cErrPhi0);
void errPhi0(TString trkCol, TCanvas *cErrPhi0, TString trkCol2);

void errCotTheta(TString trkCol);
void errCotTheta(TString trkCol, TString trkCol2);
void errCotTheta(TString trkCol, TCanvas *cErrCotTheta);
void errCotTheta(TString trkCol, TCanvas *cErrCotTheta, TString trkCol2);

void errZ0(TString trkCol);
void errZ0(TString trkCol, TString trkCol2);
void errZ0(TString trkCol, TCanvas *cErrZ0);
void errZ0(TString trkCol, TCanvas *cErrZ0, TString trkCol2);

void CosmicSegments(TString trkCol);
void CosmicSegments(TString trkCol, TCanvas *myCanvas[]);
void CosmicSegments(TString trkCol_1, TCanvas *myCanvas[], TString trkCol_2);

void SymmetricYaxis(TProfile *myHist, bool IncludingErrors, bool AroundZero);

void printPS();
