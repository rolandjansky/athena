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
#include "TGraph.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPave.h"
#include "TPaveText.h"
#include "TChain.h"

// General
TFile *file;
TChain *tree;

// Monitor Menu
TControlBar *barMain;
