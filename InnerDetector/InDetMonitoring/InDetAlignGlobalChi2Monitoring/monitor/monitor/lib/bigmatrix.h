/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __BIGMATRIX_H__
#define __BIGMATRIX_H__

#include "TROOT.h"
#include "TStyle.h"
#include "TMatrixD.h"
#include "TH2F.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TGaxis.h"
#include "TLine.h"

#include <string>

using namespace std;

class bigmatrix {
 public:
  bigmatrix(string);
  ~bigmatrix();
  
  inline int Size() { return ncols*nrows; };
  inline int NRows() { return nrows; };
  inline int NCols() { return ncols; };
  inline float Version() { return version; };
  inline float Level() { return level; };
  inline float Occupancy() { return 100*occupancy/Size(); };
  inline bool Status() { return status; };

  inline TH2F *GetPlot() { return hBigMat; };

  void GetSummary();
  void DrawLines(TCanvas*);

 private:
  string filename;

  int ncols;
  int nrows;
  int DoFs;

  bool newMat;
  bool reducePlot;
  bool createPlot;
  bool isTriangular;
  bool status;

  float occupancy;
  float version;
  float level;

  TH2F* hBigMat;

  int Initialize();
  void Logo();
  int CheckVersion(TMatrixD&);
  int ReadSymMatrix(TMatrixD&);
};

#endif
