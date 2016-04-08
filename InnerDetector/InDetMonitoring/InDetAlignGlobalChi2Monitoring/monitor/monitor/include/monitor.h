/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TSystem.h>
#include <TMath.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <math.h>
#include <map>

// forward declare methods otherwise monitor(...) does not compile
void callMacros();
void printPS();
void runMonitor();
void checkExternals(Char_t *extPath, Int_t extIteration);
Long_t checkIfcompiled(char file[280]);
void checkIfSplittedJob();
bool openFile();
int processNtuple();
void readSummary();
void openResultsFile();
void readResultsSummary();
void ShowStatistics();
void StoreStatistics();
void setStyle();
void checks();
void SystemInfo();

const int MaxPathSize = 450;

Color_t ColorForUsed;
Color_t ColorForAll;
Color_t ColorForGenParticle;

Color_t ColorForBarrel;
Color_t ColorForEndCapC;
Color_t ColorForEndCapA;

Int_t font = 0;
Int_t isize = 0;
Int_t level = 1;

Int_t IterNumber = 0;
Int_t NumberOfCPUs = 1;

Int_t PrintLevel = 3;

Long_t minFileSize = 40; // in Bytes

TFile *file;
TFile *fileResults;

Char_t path[MaxPathSize] = "";
Char_t path2[MaxPathSize] = "";
Char_t filename[MaxPathSize] = "";
Char_t command[MaxPathSize] = "";
Char_t aSetup[MaxPathSize] = "";
Char_t aSim[MaxPathSize] = "";
Char_t psSetup[MaxPathSize] = "";
Char_t psFileName[MaxPathSize] = "";
Char_t name[MaxPathSize] = "";
Char_t name2[MaxPathSize] = "";
Char_t name3[MaxPathSize] = "";
Char_t bigmatrixpath[MaxPathSize] = "";

Bool_t AtlasStyle = false;
Bool_t normalizePlots = false;
Bool_t showFits = false;

Bool_t PixelOnly = false;
Bool_t SCTOnly   = false;
Bool_t PixelBarrelOnly = false;
Bool_t SCTBarrelOnly   = false;

Bool_t TrackMatching =  false;

Bool_t SR1Ntuple = false;

Bool_t doTruth = false;
Bool_t doTrkTrack = false;
Bool_t doSiCluster = false;
Bool_t isNominal = false;

Bool_t firstCompiled = false;
Bool_t reprocessTrkana = false;
Bool_t processResults = false;

Bool_t doVtxCandidate = true;

Bool_t isTRT = false;

bool isGood[6] = {false, false, false, false, false, false};

// Summary Branch
TTree *treeSummary;
Int_t iEvents = 999;
Int_t iTracks = 999;
Int_t iTrkTracks = 999;
Int_t iUsedTracks = 999;
Int_t iHits = 999;
Int_t iPixelHits = 999;
Int_t iSCTHits = 999; 
Int_t iUsedHits = 999;
Int_t iAlignOpt = 999; 
Int_t iSetup = 999; 
Int_t iCosmics = 999;
Int_t iVtxCandidate = 999;
Float_t iBField = 999.0; 
Int_t iSimulation = 999;
Int_t iGlobalTag = 999;
Int_t iMisalignment = 999; // obsolete
Int_t iNoMCSCosmics = 999;
Int_t iNoMCS = 999; // obsolete
Int_t iMinimumHitsPerTrack = 999;
Int_t iMinimumPixelHitsPerTrack = 999; 
Int_t iMinimumSCTHitsPerTrack = 999;
Float_t iNtupleVersion = 999.0;
Int_t iAlignProcessLevel = 999;
Int_t iBeamSpot = 999;
Int_t iTrkCons = 999;

// Int_t iTrkTracks = 999;

struct FitValues {
  double FitMean;
  double FitSigma;
  double FitErrorMean;
  double FitErrorSigma;
} ;

// Canvas sizes
const Int_t CanvasSizeX2[4] = {0,10,700,300};
const Int_t CanvasSizeX3[2] = {1180,400};
const Int_t CanvasSizeX4[2] = {1024,768};
const Int_t CanvasSizeX6[2] = {1150,700};
const Int_t CanvasSizeX9[4] = {0,450,925,700};

//Canvas
TCanvas *cInfoRes;            // Summary and Residuals
TCanvas *cHits;               // Number of Hits on Track
TCanvas *cHitCorr;            // Hit Correlations
TCanvas *cHitsPerLayer;       // Number of Hits per layer
TCanvas *cProcHitsPerLayer;   // Number of Processed Hits per layer
TCanvas *cAllRes;             // Residuals in barrel/endcaps
TCanvas *cResPerLayer;        // Residuals per layer in barrel/endcaps
TCanvas *cProcAllRes;         // Processed residuals in barrel/endcaps
TCanvas *cProcResPerLayer;    // Processed Residuals per layer in barrel/endcap

// SCT Barrel Residuals
TCanvas *cSCTBarrelRes;       // SCT Barrel Residuals per Layer
TCanvas *cSCTBarrelResVspT;   // SCT Barrel Residuals Vs reco pT per Layer
TCanvas *cSCTEndCapAResVspT;   // SCT EndCap Residuals Vs reco PT per Disc

// SCT Endcap Residuals
TCanvas *cSCTEndcapARes;       // SCT EndcapA Residuals per Layer
TCanvas *cSCTEndcapCRes;       // SCT EndcapA Residuals per Layer

// PIX Barrel Residuals
TCanvas *cPIXBarrelRes;       // PIX Barrel Residuals per Layer
TCanvas *cPIXBarrelResVspT;   // PIX Barrel Residuals Vs reco pT per Layer

// PIX Endcap Residuals
TCanvas *cPIXEndcapARes;       // PIX EndcapA Residuals per Layer
TCanvas *cPIXEndcapCRes;       // PIX EndcapA Residuals per Layer

TCanvas *cSCTResErrors;       // SCT Residuals errors
TCanvas *cPixResErrors;       // Pixel Residuals errors

// AlignTrk
TCanvas *cParams;             // Track Parameters
TCanvas *cTruthParams;        // Truth Track Parameters
TCanvas *cTruthDiffParams;    // Difference Reco-Truth Track parameters
TCanvas *cUsedParams;         // Used Track parameters
TCanvas *cCompParams;         // Comparation reconstructed and truth track parameter
TCanvas *cMomentum;           // Momentum details 
TCanvas *cMomentum2;          // Processed tracks Momentum details 
TCanvas *cCompMomentum;       // Comparation Momentum details 
TCanvas *cParCorr;            // Track parameter correlations (d0 and z0 vs phi0)
TCanvas *cD0bsCorr;           // D0 wrt Beam Spot (as used in GX2 Track Constraint) correlations
TCanvas *cd0Error;            // Sigma (d0)
TCanvas *cz0Error;            // Sigma (z0)
TCanvas *cpTvsEta;            // momentum and eta correlations
TCanvas *cParCorr2;           // Momentum and phi0 correlations 
TCanvas *cTrkPixelProp;       // Pixel tracks properties
TCanvas *cUsedTrkPixelProp;   // Used Pixels track properties
TCanvas *cTrkSCTProp;         // SCT tracks properties
TCanvas *cUsedTrkSCTProp;     // Used SCT tracks properties
TCanvas *cBeamSpot;           // Beam spot

// Trk::Track
TCanvas *cTrackhits;           //Trk::Track Hits
TCanvas *cTrackParams;        // Trk::Track Parameters
TCanvas *cTrkMomentum;        // Trk::Track Momentum details
TCanvas *cTruthTrackParams;   // Trk::Track Truth Parameters
TCanvas *cTrkTruthDiffParams; // Trk::Track Reco/Truth Difference
TCanvas *cTruthVtx;           // Trk::Track Vertex Truth Production
TCanvas *cGenParticle;        // Generated Track Particle parameters

// Track Matching
TCanvas *cCompTrackMatchingParams;
TCanvas *cCompTrackMatchingParamsVsD0;
TCanvas *cCompTrackMatchingParamsVsZ;
TCanvas *cCompTrackMatchingParamsVsPhi0;
TCanvas *cCompTrackMatchingParamsVsEta;
TCanvas *cCompTrackMatchingParamsVsQoverPt;
// AlignTrk & Trk::Track
TCanvas *cNTracks;            // Number of Tracks for AlignTrk and Trk::Track

TCanvas *cChi2;               // Chi2 
TCanvas *cVtxCandidatePos;    // Vertex candidate position plots     

// Residual maps (means)
TCanvas *cResMapMeanPix;            // Residual mean maps for Pixels
TCanvas *cEtaResMapMeanPix;          // Eta residual mean maps for Pixels
TCanvas *cResMapMeanSCTSide0;       // Residual mean maps for SCT (Side 0)
TCanvas *cResMapMeanSCTSide1;       // Residual mean maps for SCT (Side 1)
TCanvas *cResMapMeanSCTEcASide0;   // Residual sigma maps for SCT (Side 0) - EndCapA
TCanvas *cResMapMeanSCTEcASide1;   // Residual sigma maps for SCT (Side 1) - EndCapA
TCanvas *cResMapMeanSCTEcCSide0;   // Residual sigma maps for SCT (Side 0) - EndCapC
TCanvas *cResMapMeanSCTEcCSide1;   // Residual sigma maps for SCT (Side 1) - EndCapC

// Residual maps (sigmas)
TCanvas *cResMapSigmaPix;           // Residual sigma maps for Pixels
TCanvas *cEtaResMapSigmaPix;           // Residual sigma maps for Pixels
TCanvas *cResMapSigmaSCTSide0;      // Residual sigma maps for SCT (Side 0)
TCanvas *cResMapSigmaSCTSide1;      // Residual sigma maps for SCT (Side 1)
TCanvas *cResMapSigmaSCTEcASide0;   // Residual sigma maps for SCT (Side 0) - EndCapA
TCanvas *cResMapSigmaSCTEcASide1;   // Residual sigma maps for SCT (Side 1) - EndCapA
TCanvas *cResMapSigmaSCTEcCSide0;   // Residual sigma maps for SCT (Side 0) - EndCapC
TCanvas *cResMapSigmaSCTEcCSide1;   // Residual sigma maps for SCT (Side 1) - EndCapC


TCanvas *cPIXEndCapAResVspT; // Pixel EndCap A Residuals Vs pT
// TCanvas *cPIXBarrelResVspT;  // Pixel Barrel Residuals Vs pT
TCanvas *cPIXEndCapCResVspT; // Pixel EndCap C Residuals Vs pT

// TCanvas *cSCTEndCapAResVspT; // SCT EndCap A Residuals Vs pT
// TCanvas *cSCTBarrelResVspT;  // SCT Barrel Residuals Vs pT
TCanvas *cSCTEndCapCResVspT; // SCT EndCap C Residuals Vs pT

// Module positions
TCanvas *cPixelBarrelPos;     // Pixels Initial Positions
TCanvas *cPixelBarrelMeanPos; // Pixels Initial MeanPositions
TCanvas *cSCTBarrelPos;       // SCT Initial Positions
TCanvas *cSCTBarrelMeanPos;   // SCT Initial Mean Positions

// Hitmaps
TCanvas *cPIXHitMap;
TCanvas *cPIXHitMapEc;
TCanvas *cSCTHitMapBa;
TCanvas *cSCTHitMapEc[2];

TCanvas *cHitmapsXY;

// EigenValues
TCanvas *cEigenModes;
TCanvas *cAlignCorr;

// SiCluster Canvas
TCanvas *cSiClusSize;
TCanvas *cSiClusSizeIncAng;

// bigmatrix Canvas
TCanvas *cBigMatrix;


// --------------------------------------------------------------------

// Summary Branch
Int_t iAlignSolveLevel = 999;
Int_t iAlignPars = 999;
Int_t iDoF = 999;
Int_t iModCut = 999;
Int_t iHitCut = 999;

Int_t iSoftScaled = 999;
Float_t iSoftCut=-1.0;
Float_t iSoftPIXTXerror=-1.0;
Float_t iSoftPIXTYerror=-1.0;
Float_t iSoftPIXTZerror=-1.0;
Float_t iSoftPIXRXerror=-1.0;
Float_t iSoftPIXRYerror=-1.0;
Float_t iSoftPIXRZerror=-1.0;
Float_t iSoftSCTTXerror=-1.0;
Float_t iSoftSCTTYerror=-1.0;
Float_t iSoftSCTTZerror=-1.0;
Float_t iSoftSCTRXerror=-1.0;
Float_t iSoftSCTRYerror=-1.0;
Float_t iSoftSCTRZerror=-1.0;
Float_t iSoftPIXECTXerror=-1.0;
Float_t iSoftPIXECTYerror=-1.0;
Float_t iSoftPIXECTZerror=-1.0;
Float_t iSoftPIXECRXerror=-1.0;
Float_t iSoftPIXECRYerror=-1.0;
Float_t iSoftPIXECRZerror=-1.0;
Float_t iSoftSCTECTXerror=-1.0;
Float_t iSoftSCTECTYerror=-1.0;
Float_t iSoftSCTECTZerror=-1.0;
Float_t iSoftSCTECRXerror=-1.0;
Float_t iSoftSCTECRYerror=-1.0;
Float_t iSoftSCTECRZerror=-1.0;

Int_t alignedModules = -999;
Int_t alignedSCTModules = -999;
Int_t alignedPIXModules = -999;

// TTrees
TTree *treeDia;
TTree *treeConsts;
TTree *treeResultsSummary;

// Alignment Results
TCanvas *cDia;

TCanvas *cPixelConsts;
TCanvas *cSCTConsts;

TCanvas *cPixelErrorConsts;
TCanvas *cSCTErrorConsts;

TCanvas *cErrorsVsPixelHits;
TCanvas *cErrorsVsSCTHits;

TCanvas *cCorrVsPixelHits;
TCanvas *cCorrVsSCTHits;

TCanvas *cPIXConstantsMap[6];
TCanvas *cSCTConstantsMap[6];

typedef Int_t StatusCode;
enum {SUCCESS=0, FAILURE=1};

// prototypes
StatusCode  CompileMacros();
StatusCode  RunEigenAna(Char_t *);
