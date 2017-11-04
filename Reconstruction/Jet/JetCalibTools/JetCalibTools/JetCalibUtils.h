/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBUTILS_H
#define JETCALIBUTILS_H

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include <cmath>
#include <vector>

typedef std::vector<TString> StrV;
typedef std::vector<double> VecD;

namespace JetCalibUtils {

  // Vectorizing functions and associated typedefs
  StrV Vectorize (TString str, TString sep=" ");
  VecD VectorizeD(TString str, TString sep=" ");

  TH1 *GetHisto(TFile *file, TString hname);
  TH2 *GetHisto2(TFile *file, TString hname);
  TH3 *GetHisto3(TFile *file, TString hname);

  TTree *setTree(TTree *tree);

}

#endif
