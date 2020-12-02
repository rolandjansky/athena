/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <strstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <cstdio>
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TControlBar.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPave.h"
#include "TPaveText.h"

// Paths
char path0[20]="InDet_macros/pixels";
char path1[20]="InDet_macros/sct";
char path2[20]="InDet_macros/trt";
char path3[20]="Corr_macros";
char path4[20]="Tile_macros";
char path5[20]="Muons_macros";
char path6[20]="LAr_macros";

// General
TFile *file;
TTree *tree;
int run;
char psfilename[40];

// Monitor Menu
TControlBar *barMain; // main
TControlBar *barMain0; // Pixels
TControlBar *barMain1; // SCT
TControlBar *barMain2; // TRT
TControlBar *barMain3; // Correlations
TControlBar *barMain4; // Tile
TControlBar *barMain5; // Muons
TControlBar *barMain6; // LAr

// Graphics
TCanvas *c11;

// Common variables
const int MAX =150000;
char name[200],name2[200];
int layer,phi,side,module,plane,row,col,tot;
char title[200], options[300];
int hit;

// SCT variables
const int MODS = 8;
const int MODPLANES = 16;
char command[250], filename[50],option[50];
float strips,clusHit;
float spHit0,spHit1;
int entries;
char sname[50];
float event,tbin;
float rdo_strips[MAX],rdo_layer[MAX],rdo_phi[MAX],rdo_side[MAX];
int ievent=0,rdo_nhits=0;
int istrips[8];
int counter=0;
char axisX[35], axisY[35], axisZ[35];
int ievent, ntracks=0;
const int LAYERS = 4;
const int PLANES_IN_LAYER = 4;

// Pixels variables
const int LAYERS = 3;
const int MODULES_IN_PHI = 2;
const int CHIPS_PER_MODULE_IN_COL=8;
const int CHIPS_PER_MODULE_IN_ROW=2;
const int ROW_RESOLUTION=1;		// no of pixels in a row per bin
const int COL_RESOLUTION=1;		// no of pixels in a col per bin  
float treevariable1;
float treevariable2;
float treevariable3;
double x[2], y[2];
const int MAX_HITS = 1000;
float pixel_layer[MAX_HITS], pixel_phi[MAX_HITS], pixel_col[MAX_HITS], pixel_row[MAX_HITS], pixel_tot[MAX_HITS];

// TRT variables
const int TRT_LAYER = 3;
const int STRAW_LAYER[TRT_LAYER] = {19,24,30};

int istraw;
float ystraw;
TCanvas *c41;
TCanvas *c42;
TCanvas *c43;
int n_rdo,trtlayer[MAX],straw_layer[MAX],straw[MAX],trtphi[MAX];
int nDC,nmdtdig;
float straw_y[MAX];
float drift_t[MAX],trttot[MAX],hl[MAX];

// Tracks
const int MAX_TRT_HITS = 100;           //max no. of trt hits per track
const int MAX_TRACKS = 60;            //max no. of tracks per event
const int MAX_SCT_HITS = 32;           //max no. of sct hits per track
int trk_sct_nhits[MAX_TRACKS];
float sct_errtrk[MAX_TRACKS][MAX_SCT_HITS];
float sct_errhit[MAX_TRACKS][MAX_SCT_HITS];
float sct_res[MAX_TRACKS][MAX_SCT_HITS];
float sct_pull[MAX_TRACKS][MAX_SCT_HITS];
int trk_sct_layer[MAX_TRACKS][MAX_SCT_HITS], trk_sct_phi[MAX_TRACKS][MAX_SCT_HITS],trk_sct_side[MAX_TRACKS][MAX_SCT_HITS];
int n_tracks,n_hits[MAX_TRACKS],trk_hl[MAX_TRACKS][MAX_TRT_HITS];	
float t[MAX_TRACKS][MAX_TRT_HITS],R[MAX_TRACKS][MAX_TRT_HITS];
int trk_phi[MAX_TRACKS][MAX_TRT_HITS],trk_straw_layer[MAX_TRACKS][MAX_TRT_HITS],trk_layer[MAX_TRACKS][MAX_TRT_HITS],trk_straw[MAX_TRACKS][MAX_TRT_HITS];

// Correlations
TCanvas *c20, *c21;
int sct_nhits=0,pixel_nhits=0;
float sct_strips[MAX],sct_layer[MAX],sct_phi[MAX],sct_side[MAX];
//float pixel_layer[MAX],pixel_phi[MAX],pixel_row[MAX],pixel_col[MAX];
int PixelsPhi;
TH2F *corrhisto[12];
