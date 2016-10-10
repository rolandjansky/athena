/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKRoadStream.h"
#include "TrigFTKSim/FTKTrackStream.h"
#include "TCanvas.h"
#include <iostream>
#include "TChain.h"
#include "TBranch.h"

#define MAXTOWER 64
#define MAXL 8

using namespace std;

ofstream myfile;
ofstream myfileTeX;

// [tower]
float nRoad[MAXTOWER], nFit[MAXTOWER], nFitI[MAXTOWER], nTrack[MAXTOWER], nTrackI[MAXTOWER], nTrackBeforeHW[MAXTOWER], nFitRecovery[MAXTOWER], nFitRecoveryI[MAXTOWER];
float nFitMajority[MAXTOWER], nFitMajorityI[MAXTOWER], nFitMajorityPix[MAXTOWER], nFitMajorityPixI[MAXTOWER],  nFitMajoritySCT[MAXTOWER], nFitMajoritySCTI[MAXTOWER] ;
float nCluster[MAXL][MAXTOWER], nSSID[8][MAXTOWER];
float nConn[MAXTOWER], nExtrapAUX[MAXTOWER];
FTKRoadStream *stream[MAXTOWER];
FTKTrackStream *trackstream[MAXTOWER];

// events to run over
Int_t events;
unsigned int nloop;
double divide;

// output location in txt
std::string output;

// output location in tex
std::string outputTeX;

std::string description;

// number of towers
Int_t ntower;

// List of input files
std::vector<std::string> files;

// Input files
TChain *ch;

void Init();
void Process(unsigned int ientry);
void Terminate();
void printinfo(float towers[MAXTOWER], TString text);
void AddBreak(int n = 1);
