/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __jacobana__
#define __jacobana__
////////////////////////////////////////////////////
// includes
/////////////
//
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
#include "THStack.h"
#include "TGaxis.h"
#include "TCanvas.h"
#include "TMatrixDEigen.h"
#include "TMatrixTSparse.h"
#include "TFile.h"
#include "TDirectoryFile.h"
#include "TPaletteAxis.h"
#include "TLine.h"
#include "TText.h"

#ifndef ROOT_TString
#include "TString.h"
#endif

////////////////////////////////////////////////////
// variables
/////////////
//
TString *DirName;
TString *InputTFileName;
//
TFile *InputTFile; // The actual TFile where the matrix, vector, ... are stored.

// 
Double_t m_ScaleFactor;

TMatrixDSym BigMatrix;
TMatrixD BigMatrix_Inv;  // its inverse for error calculation
TVectorD BigVector;   // in the modules base
TVectorD BigVector_b;  // in the diagonal base

TMatrixD BigMatrix_b;   // big matrix in diagonal base (diagonal matrix)
TVectorD EigenVal;        // vector of eigen values

TVectorD RowsUsed;
TVectorD Identifiers;
TVectorD DoFs;
TMatrixD ChangeOfBaseMatrix; 

TVectorD AlignCorrRaw; // Raw align corrections in the modules base (all modes included, i.e. without mode cut)
TVectorD AlignCorrRaw_b; // Raw align corrections in the diagonal base (all modes included, i.e. without mode cut)

TVectorD AlignCorr; // Align corrections in the modules base (after mode cut)
TVectorD AlignCorr_b; // Align corrections in the diagonal base (after mode cut)

TVectorD AlignCorrError;    // vector of error of the alignment corrections in the modules base
TVectorD AlignCorrError_b;    // vector of error of the alignment corrections in the diagonal base

TVectorD DoFType; 
//
// structures
const Int_t NMaxStruct = 3568; // Level 3: modules in the SCT Barrel
Bool_t UsedEigenMode[NMaxStruct*6];
Int_t  NofAlignableStructures;
short  NdofPerStruct=6;

//
const Double_t ZeroEquivalent = 1.E-20;
const Double_t InfEquivalent = 1.E+20;
//
typedef Int_t StatusCode;
enum {SUCCESS=0, FAILURE=1};
enum {DoFTx=0, DoFTy=1, DoFTz=2, DoFRx=3, DoFRy=4, DoFRz=5};
enum {PIX=0, SCT=1, TRT=2};

//
Double_t AngleUnitFactor = 0.001; // use mradians
//
short PrintLevel;
//
// solving options
TString *SolveOptions;
Bool_t SolveWholeID;
Bool_t SolveLocalX2Style = false; // matrix conditioning to make it like local X2
Bool_t QuietMode = false ;
Bool_t PlotEigenVecs = false;
Bool_t UseSoftModeCut = false;  // If true a soft mode cut is add to the big matrix
Bool_t DoSolving = true;       // If true solve the matrices, if false just read and do the preconditioning
Bool_t NewAlign = false;
Bool_t UseTFileInput = true;

Float_t HitCut;
// weak modes
Int_t NModesToRemove;    // number of modes to remove in case of fixed number of modes removal 
Int_t WeakModeStrategy;  // which weak mode strategy to use
Float_t PullLimit;       // Cut in the pull of eigenvalues

// indicate which degree of freedom is in use
Bool_t UseTx, UseTy, UseTz, UseRx, UseRy, UseRz;
// basic dof names (for printing and human readable)
TString DofName[6] = {"Tx", "Ty", "Tz", "Rx", "Ry", "Rz"};
TString *StructureName[NMaxStruct];

Int_t StructDetec[NMaxStruct];
Int_t DofBelongsToStruct[NMaxStruct*6];
Int_t NumberOfPIXStruct=0;
Int_t NumberOfSCTStruct=0;
Int_t NumberOfTRTStruct=0;


// Canvas
TCanvas *CanDiagonalBase;
TCanvas *CanAlignCorr;
TCanvas *CanAlignCorrErr;
TCanvas *CanBigMatrix;
TCanvas *CanBigMatrixInv;
TCanvas *CanCorrelMatrix;
TCanvas *CAliCorrDof;


// soft mode cut variables
float sct_ec_Rz_smc = 1.;
/////////////////////
// histograming
/////////////////////
TFile *EAHistFile;
TDirectoryFile *EAHistDir;
//
TH1F *hEigenVal;  // histogram of eigen values
TH1F *hBigVector;   // histogram of the big vector in the modules base
TH1F *hBigVector_b;    // histogram of the big vector in the diagonal base
TH1F *hAliCorrRaw; // histogram of the alignment correction in the modules base and including all mode
TH1F *hAliCorrRaw_b;  // same as above but in diagonal base 

TH1F *hAliCorr;  // histogram of the alignment correction in the modules base after the mode cut
TH1F *hAliCorr_b; // same as above but in diagonal base 

TH1F *hAliCorrErr;  // histogram of the alignment correction error in the modules base after the mode cut
TH1F *hAliCorrErr_b; // same as above but in diagonal base 

TH1F *hAliCorrPull_b; // pull of the alignment corrections in the diagonal base

TH1F * hTxAliCorr;
TH1F * hTyAliCorr;
TH1F * hTzAliCorr;
TH1F * hRxAliCorr;
TH1F * hRyAliCorr;
TH1F * hRzAliCorr;

TH1F * hTxAliCorrProj;
TH1F * hTyAliCorrProj;
TH1F * hTzAliCorrProj;
TH1F * hRxAliCorrProj;
TH1F * hRyAliCorrProj;
TH1F * hRzAliCorrProj;

TH1F *hModCorr[NMaxStruct*6];
TH1F *hDofCorr[NMaxStruct*6];
TH1F *hDofIntegral[NMaxStruct*6];

TH2F* hBigMatrixInv; 

struct HistoBins{
      Float_t min;
      Float_t max;
      Int_t bins;
};

TLine *line;
////////////////////////////////////////////////////
// prototypes
/////////////
//
StatusCode  EigenAna_init(const char*);
void        EigenAna_operate();
void        EigenAna_end();
//
void        BookAliCorrHistos();
void        BookAliCorrErrHistos();
void        BookAliCorrPullHistos();
void        BookBigMatrixHisto();
void        BookBigMatrixInvHisto();
void        BookCorrelMatrixHisto();
void        BookDofCorrections();
void        BookEigenValues();
void        BookBigVector();
void        BookModeCorrections();
void        BookRawAlignCorr();
void        ChangeAngleUnits();
void        ComputeAlignCorr();
void        ComputeAlignCorrErrors();
void        DetectNullEigenValues();
void        DivideEigenCanvas(TCanvas *);
void	    DrawStructLines(TH1F *hist, TCanvas *can, int c);
void        DrawStructLinesOnHMatrix (TH2F *hist);
void	    ExtractName(unsigned long long, int);
void        FindDetectorType(unsigned long long, int, int);
TVectorD    Flip(const TVectorD &);
TMatrixD    Flip(const TMatrixD &);
HistoBins   GetHistoLimits(TH1F*);
HistoBins   GetHistoLimits(TH1F* hist1, TH1F* hist2);
float	    GetHitCut ();
Int_t       GetNdofPerStruct ();
Int_t       GetNdof ();
Int_t       GetNEigenCanvas ();
Int_t       GetNEigenPerCanvas();
Int_t       GetNMaxStructures();
Int_t       GetNModesToRemove();
Int_t       GetNStruct();
Int_t       GetNUsedDof();
Int_t       GetPrintLevel();
Float_t     GetPullLimit();
Int_t       GetStructID(Int_t);
Int_t       GetWeakModeRemovalStrategy();
void        HandleSolveOptions();
void        InitUsedEigenModesFlag();
void        InitGraphicsStyle();
void        InitOutputFile();
Bool_t      IsEigenModeUsed(int);
Bool_t      IsTranslation (int);
void        MatrixAndVectorPreConditioning();
void        OpenCanvas ();
void        PrepareLocalX2 ();
void        PrepareSoftModeCut ();
void        PrintAlignmentCorrections (const TVectorD&);
StatusCode  ReadTFile ();
void	    RemoveRows ();
void	    RemoveMatrixRows (int, int, TMatrixDSym&);
void	    RemoveVectorRows (int, int, TVectorD&);
void        SetActiveNdof(Bool_t, Bool_t, Bool_t, Bool_t, Bool_t, Bool_t);
void        SetColorsOfModeCorrHists ();
void        SetScaleFactor(double myscale);
void        SetDirectory(const char*);
void        SetDoFTypes();
void        SetFlagForUsedEigenMode(Int_t, Bool_t);
void	    SetHitCut(float);
void        SetNModesToRemove(int);
void        SetNofStructures(int);
void        SetNofStructuresPerDetec();
void        SetPrintLevel(Int_t);
void        SetPullLimit(Float_t);
void        SetWeakModeRemovalStrategy(int);
void        SetXAxisLabels(TH1F*);
void        SetXAxisLabels(TH2F *histo, Bool_t nameonlyfirstofeachstruct = false);
void        SetXAxisStructures(TH1F*);
void        SetHMatrixLabels (TH2F*);
void        SolveInDiagonalBase();
void        SolveWithWeakModeRemoval();
//
#endif
