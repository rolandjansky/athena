/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/evolution.h>

const int MaxPathSize = 450;

Char_t path[MaxPathSize] = "";
Char_t path2[MaxPathSize] = "";
Char_t pathTemp[MaxPathSize] = "";
Char_t inputpath[MaxPathSize] = "";

Char_t filename[MaxPathSize] = "";
Char_t command[MaxPathSize] = "";

Char_t name[MaxPathSize] = "";
Char_t name2[MaxPathSize] = "";
Char_t name3[MaxPathSize] = "";


//ConstantsVsIters
Char_t nameIter[MaxPathSize] = "";
Char_t nameFile[MaxPathSize] = "";
Char_t namePath[MaxPathSize] = "";
Char_t namePIXBarrel[MaxPathSize] = "";
Char_t namePIXECA[MaxPathSize] = "";
Char_t namePIXECC[MaxPathSize] = "";



Char_t name_eta[MaxPathSize] = "";
Char_t name_phi[MaxPathSize] = "";

TFile *file;

Bool_t AtlasStyle = false;
Int_t PrintLevel = 0;
Int_t NumberOfIters = 1;
Int_t IterCounter = 0;

Bool_t PlotNtuple = false;
Bool_t isMixed = false;

TNtuple *ntupleSCTResVsIter;
TNtuple *ntuplePIXPhiResVsIter;
TNtuple *ntuplePIXEtaResVsIter;

//position
TNtuple *ntupleConstantsVsIter;


struct FitValues {
  double FitMean;
  double FitSigma;
  double FitErrorMean;
  double FitErrorSigma;
} ;

Color_t ColorForSCTBarrel[4] = {kRed-4,kOrange-3,kGreen-3,kMagenta};

// Canvas sizes
const Int_t CanvasSizeX2[4] = {0,10,700,300};
const Int_t CanvasSizeX4[2] = {1024,768};
const Int_t CanvasSizeX6[2] = {1150,700};
const Int_t CanvasSizeX9[4] = {0,450,880,700};

// Canvas
TCanvas *cBarrelResVsIters;
//Position
TCanvas *cPIXPositionVsIter;




// to be compatible with styel.cc
Color_t ColorForUsed;
Color_t ColorForAll;
Color_t ColorForGenParticle;

Color_t ColorForBarrel;
Color_t ColorForEndCapC;
Color_t ColorForEndCapA;

Int_t  font = 42;
