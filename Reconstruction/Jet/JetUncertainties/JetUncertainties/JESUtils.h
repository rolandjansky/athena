/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JESUTILS_H
#define JESUTILS_H

#include "TString.h"
#include "TObjString.h"
#include "TH2D.h"
#include <cmath>
#include <vector>

typedef std::vector<TString> StrV;
typedef std::vector<double> VecD;

namespace JESUtils {

  // Vectorizing functions and associated typedefs
  StrV Vectorize (TString str, TString sep);
  VecD VectorizeD(TString str, TString sep);
  
  // Empty histogram creation functions
  VecD GetUniformBins(int numBins, double lowBound, double highBound);
  VecD GetLogBins(int numBins, double lowBound, double highBound);
  VecD GetDefaultPtBins();
  VecD GetDefaultEtaBins();
  TH2D* MakeEmptyPtEtaHisto(TString hname, VecD ptBins, VecD etaBins);
  TH2D* MakeEmptyPtEtaHisto(TString hname, VecD ptBins);
  TH2D* MakeEmptyPtEtaHisto(TString hname);
  
  
  // Histogram interpolation functions
  void CheckPtEtaRanges(TH2D* histo1, TH2D* histo2);
  TH2D* MakeInterpolatedPtEtaHisto(TH2D* oldHisto, TString newName, VecD newPtBins, VecD newEtaBins);
  TH2D* MakeInterpolatedPtEtaHisto(TH2D* oldHisto, TString newName);
  
}

#endif
