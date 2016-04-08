/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __jacobana__
#define __jacobana__
////////////////////////////////////////////////////
// includes
/////////////
#include <iostream>
#include <fstream>
#include <iomanip>
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TVectorD.h"
#include "TStopwatch.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGaxis.h"
#include "TCanvas.h"
#include "TMatrixDEigen.h"
#include "TFile.h"
#include "TDirectoryFile.h"

#ifndef ROOT_TString
#include "TString.h"
#endif

//
const Double_t ZeroEquivalent = 1.E-20;
const Double_t InfEquivalent = 1.E+20;
//
////////////////////////////////////////////////////
// variables
/////////////
TString *DirName;
TString *BigVectorFile;
TString *BigMatrixFile;
TString *EigenValuesFile;
TString *EigenVectorsFile;
//
// big vector and matrix at input
TVectorD inputBigVector;
TMatrixD inputBigMatrix;
//
TVectorD BigVector;
TMatrixD BigMatrix;
TMatrixD BigMatrixInv;
TMatrixD SoftModeCutMatrix;
//
TVectorD BigVector_b;
TMatrixD BigMatrix_b;
//
TMatrixD ChangeOfBaseMatrix; 
//
TVectorD EigenVal;        // vector of eigen values
TVectorD EigenIma;        // imgainary part of the eigenvalues
//
TVectorD da_AlignCorrRaw; // vector of align corrections in the modules base using all the modes
TVectorD db_AlignCorrRaw; // vector of align corrections in the diagonal base 
TVectorD Eb_AlignCorr;    // vector of error of the alignment corrections in the diagonal base
TVectorD Ea_AlignCorr;    // vector of error of the alignment corrections in the module base
TVectorD db_Pulls;        // vector of pulls of the alignment corrections in the diagonal base
TVectorD da_AlignCorr;    // vector of align corrections in the modules base after weak mode removal
TVectorD db_AlignCorr;    // vector of align corrections in the diagonal base after weak mode removal
//
short PrintLevel;
//
//const Int_t NMaxStruct = 31; // Level 2
const Int_t NMaxStruct = 3568; // Level 3: modules in the SCT Barrel
//
// indicate which degree of freedom is in use
Bool_t UseTx, UseTy, UseTz, UseRx, UseRy, UseRz;
short  NdofPerStruct;
Bool_t DofIsTranslation[6];  // 6 is the maximum Dof per structure
Bool_t DofIsRotation[6];
Bool_t DofIsTx[6];  
Bool_t DofIsTy[6];  
Bool_t DofIsTz[6];  
Bool_t DofIsRx[6];  
Bool_t DofIsRy[6];  
Bool_t DofIsRz[6];  
Int_t  NofAlignableStructures;
Bool_t DofHasGlobalShift[6] = {0};
Bool_t UsedEigenMode[NMaxStruct*6];
TString *DofName[6];
TString *StructureName[NMaxStruct];
//
//
// weak modes
Int_t NModesToRemove;    // number of modes to remove in case of fixed number of modes removal 
Int_t WeakModeStrategy;  // which weak mode strategy to use
Float_t PullLimit;       // Cut in the pull of eigenvalues
//
// scale (number of hits: due to the normalization)
Int_t ScaleFactor;
//
// solving options
TString *SolveOptions;
Bool_t SolveWholeID;
Bool_t SolveSCTBarrel;
Bool_t SolveBarrelOnly;
Bool_t QuietMode;
Bool_t PlotEigenVecs;
Bool_t InputIsRaw;      // If true, input is the raw big matrix and vector -> needs diagonalization
Bool_t InputIsEigen;    // If true, input is the eigen values and vectors -> does not need diagonalization
Bool_t UseSoftModeCut;  // If true a soft mode cut is add to the big matrix
Bool_t UseECConstraint;  // If true a EC constraint (on Tz) is added to the big matrix
Bool_t SolvingLevel1;   // If true change labels to Level 1
Bool_t DoSolving;       // If true solve the matrices, if false just read and do the preconditioning
Bool_t SaveMatrices;    // If true the BigMatrix and BigVector are saved
/////////////////////
// histograming
/////////////////////
TFile *JAHistFile;
TDirectoryFile *JAHistDir;
//
TH1F *hEigenVal;
TH1F *hdbAliCorr;
TH1F *hdbAliCorrRaw;
TH1F *hdbAliCorrErr;
TH1F *hdbPulls;
TH1F *hdbPullsH;
TH1F *hdaAliCorr;
TH1F *hdaAliCorrRaw;
TH1F *hBigV_b;
TH2F *hBigMatrix;
//
TH1F *hTxAliCorr;
TH1F *hTyAliCorr;
TH1F *hTzAliCorr;
TH1F *hRxAliCorr;
TH1F *hRyAliCorr;
TH1F *hRzAliCorr;
TH1F *hTxAliCorrProj;
TH1F *hTyAliCorrProj;
TH1F *hTzAliCorrProj;
TH1F *hRxAliCorrProj;
TH1F *hRyAliCorrProj;
TH1F *hRzAliCorrProj;
//
TH1F *hTxAliErr;
TH1F *hTyAliErr;
TH1F *hTzAliErr;
TH1F *hRxAliErr;
TH1F *hRyAliErr;
TH1F *hRzAliErr;
TH1F *hTxAliErrProj;
TH1F *hTyAliErrProj;
TH1F *hTzAliErrProj;
TH1F *hRxAliErrProj;
TH1F *hRyAliErrProj;
TH1F *hRzAliErrProj;
//
//
TH1F *hModCorr[NMaxStruct*6];
//
// Canvas
TCanvas *CanDiagonalBase;
TCanvas *CanAlignCorr;
TCanvas *CanBigMatrix;

TCanvas *CCXX[6][186]; // maximum number dofs per structure is 6
//
typedef Int_t StatusCode;
enum {SUCCESS=0, FAILURE=1};
//
// constants
//const Float_t Rad2mRad = 1000.;
const Float_t Rad2mRad = 1.;
//Double_t AngleUnitFactor = 1.; // use radians
Double_t AngleUnitFactor = 0.001.; // use mradians
float    FormatVersion;
//
////////////////////////////////////////////////////
// prototypes
/////////////
StatusCode  jacobana_init(const char*);
void        jacobana_operate();
void        jacobana_end();
//
void        AliCorrBookHistos();
void        AliCorrErrBookHistos();
void        BookBigMatrixHisto();
void        BookEigenVectors();
void        BookModeCorrections();
TMatrixD    BuildProjector();
void        ChangeAngleUnits();
Bool_t      CheckEigenVal();
Bool_t      CheckForNegative();
StatusCode  CheckAverageShiftByDof();
void        ComputeAlignCorr();
void        ComputeAlignCorrErrors();
void        DivideEigenCanvas(TCanvas *);
TVectorD    Flip(const TVectorD &);
TMatrixD    Flip(const TMatrixD &);
StatusCode  FitToPol0(TH1F *);
Int_t       GetFittingNdof();
Int_t       GetNEigenCanvas();
Int_t       GetNEigenPerCanvas();
Int_t       GetNdofPerStruct();
Int_t       GetNMaxStructures();
Int_t       GetNModesToRemove();
Int_t       GetNumberOfStruct();
Int_t       GetPrintLevel();
Float_t     GetPullLimit();
Int_t       GetScaleFactor();
Int_t       GetWeakModeRemovalStrategy();
void        GlobalModeFindManager();
void        HandleSolveOptions();
Bool_t      HasDofGotGlobalShift(Int_t);
void        InitGraphicsStyle();
void        InitOutputFile();
void        InitUsedEigenModesFlag();
Bool_t      IsDofUsed(int);
Bool_t      IsEigenModeUsed(int);
Bool_t      IsRotation(int);
Bool_t      IsRx(int);
Bool_t      IsRy(int);
Bool_t      IsRz(int);
Bool_t      IsTranslation(int);
Bool_t      IsTx(int);
Bool_t      IsTy(int);
Bool_t      IsTz(int);
void        LookForModesWithGlobalShift(Int_t);
void        MatrixAndVectorPreConditioning();
Bool_t      NeedToProjectDofs();
void        OpenCanvas();
void        PrintAlignmentCorrections(const TVectorD&);
void        PrepareSoftModeCut();
void        PrepareECConstraint();
void        Project2BarrelOnly();
void        Project2SCTBarrel();
void        ProjectSelectedDofs();
StatusCode  ReadBigMatrix();
StatusCode  ReadBigVector();
StatusCode  ReadEigen();
StatusCode  ReadEigenValues();
StatusCode  ReadEigenVectors();
void        SetActiveNdof(Bool_t, Bool_t, Bool_t, Bool_t, Bool_t, Bool_t);
void        SetDirectory(const char*);
void        SetDofHasGlobalShift(Int_t, Bool_t);
void        SetFlagForUsedEigenMode(Int_t, Bool_t);
void        SetNModesToRemove(int);
void        SetNofStructures(int);
void        SetStructureNames();
void        SetPrintLevel(Int_t);
void        SetPullLimit(Float_t);
void        SetScaleFactor(Int_t);
void        SetXAxisLabels(TH1F*);
void        SetWeakModeRemovalStrategy(int);
void        SpyDoF(Int_t);
void        SolveInModulesBase();
void        SolveInDiagonalBase();
void        SolveWithWeakModeRemoval();
void        SwapEigen(const Int_t, const Int_t);
StatusCode  WriteBigVector();
StatusCode  WriteBigMatrix();
StatusCode  WriteMatrixAndVector();
//
#endif
