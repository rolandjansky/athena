/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TPaveText.h>
#include <TGaxis.h>
#include <TSystem.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <math.h>
#include <map>

const int MaxPathSize = 200;

Int_t PrintLevel = 5;

TNtuple *ntuple;
TNtuple *ntuple2;
TCanvas *cConstsBarrel;
TCanvas *cConstsEndCapA;
TCanvas *cConstsEndCapC;

TCanvas *cHistsCorrVsIter;

Bool_t AtlasStyle = false;  
Bool_t graphics = false;
Bool_t first = true;
Bool_t good = false;
Bool_t problem = false;

Bool_t doLevel2ForRef = true;

Bool_t BarrelOnly = false;
Bool_t customComp = true;
Bool_t showNominal = true;

Int_t isReference = 0;

Char_t path[MaxPathSize];
Char_t filename[MaxPathSize];
Char_t command[MaxPathSize];
Char_t username[MaxPathSize];

TLegend *leg;

Int_t Iteration = -2;
Int_t NumberOfIters = 0;
Int_t NumberOfCPUs = 1;

Int_t counter = 0;
Int_t Geometry = 0;

Int_t *mcode;
Int_t *modsLy;
Float_t *radiusLy;
TVector3 *cog;

// for style.cc
Color_t ColorForUsed = kRed;
Color_t ColorForAll  = kGray;
Color_t ColorForBarrel;
Color_t ColorForEndCapC;
Color_t ColorForEndCapA;
Color_t ColorForGenParticle;
Color_t iColor[] = {kGray+2,kRed-4,kOrange-3,kYellow+1,kGreen-3,kCyan-6,kAzure,kBlue,kBlue+4};

///////////////////////////////////////
// declaration
///////////////////////////////////////
void init(Char_t*);
void checkExternals(Char_t*);
void checkNumberOfIters();
Long_t checkFile(Char_t*);
Long_t checkDir(Char_t*);
void initGraphics(Int_t);
void readRadius(Char_t*);
Bool_t read(Char_t*);
void PlotConstants(Int_t,Float_t,Color_t,Char_t*);
void PrintLegend();
void PrintCanvas();
Float_t GetRadius(Int_t,Int_t,Int_t);
void SetStyle();
Long_t checkFile(Char_t*);
//void checkNCPUs();

void PlotHists();

// from utils...
Int_t font = 42;
void setTitleFont();
void drawHorizontalLine(TGraph*);
void drawHorizontalLine(TGraph*, Double_t);
void SetXAxisLabels(TGraph*, Int_t);
Int_t GetNumberOfBinsX(TH2F*);

#endif
