/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************
//
//  GlobalPositionVsIter.h
//
//*************************

//Definim les llibreries necesaries

#include <iostream>
#include <fstream>
#include <algorithm>
#include "TROOT.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile2D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TStyle.h"
#include "TBox.h"
#include "TLine.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TGaxis.h"
#include "TSystem.h"
#include "TMarker.h"
#include "TPolyMarker.h"
#include "TNtuple.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TColor.h"

//------------------
//  VARIABLES
//------------------
const Int_t MAX_MODUL = 5832;

Int_t Initial;
Int_t Final;
//variables comunes dels dos files
Int_t MCode;
Int_t Sector,Ring,Side,Layer;
//GlobaPosition
Float_t Tx,Ty,Tz,Rx,Ry,Rz;
//PerfectGlobal_Position
Float_t PTx,PTy,PTz,PRx,PRy,PRz,tmp;

Int_t NumSectors;
Int_t NumRings;
bool doPIXBarrel = false;
bool doPIXEndcaps = false;
bool doSCTBarrel = false;
bool doSCTEndcaps = false;
bool doModule = false;
bool doStave = false;
int userDetType;
int userBec;
int userLayer;
int userRing; 
int userSector;

//--------------
//   NTUPLES
//--------------

TNtuple *ntupleConstantsVsIter;


//--------------
//    CANVAS
//--------------

//PIX
TCanvas *cPIX_Ba[3];
TCanvas *cPIX_EC[2][3];
//SCT
TCanvas *cSCT_Ba[4];
TCanvas *cSCT_EC[2][9];

//Module
TCanvas *cModule;

//Stave
TCanvas *cStave;
// Canvas sizes
const Int_t CanvasSizeX4[2] = {1024,768};
const Int_t CanvasSizeX6[2] = {1150,700};

int mainColor;

//-----------------------
//      TGraph PIXELS
//-----------------------

//PIX

TGraph *hGraphPixBa[3][6][700];

TGraph *hGraphPixEC[2][3][6][50];
//SCT

TGraph *hGraphSCTBa[4][6][700];

TGraph *hGraphSCTEC[2][9][6][160];
// Module
TGraph *hGraphModule[6];

// Stave
TGraph *hGraphStave[6][13];
//----------------
//    Names
//----------------

Int_t NumberOfIters = -1;
const int MaxPathSize = 450;
Char_t extPath[MaxPathSize]="";

Char_t filename[MaxPathSize] = "";
Char_t command[MaxPathSize] = ""; 

Char_t lg[10];
Char_t name[MaxPathSize] = "";
Char_t name2[MaxPathSize] = "";


Char_t canvasname[MaxPathSize] = "";
Char_t namePath[MaxPathSize] = "";
Char_t namePath2[MaxPathSize] = "";

Char_t condition[MaxPathSize] = "";

Char_t path[MaxPathSize] = ""; 
Char_t title[MaxPathSize] = ""; 
Char_t YAxis[MaxPathSize] = ""; 
//------------------------
//   Funcions
//-----------------------


void checkExternals(Char_t*);
int invMcode(int, Char_t*);
void printPS(bool);
void GraphicsStyle();
//void CreatePalette(int);



