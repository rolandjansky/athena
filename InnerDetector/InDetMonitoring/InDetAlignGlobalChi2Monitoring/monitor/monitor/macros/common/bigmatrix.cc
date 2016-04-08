/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "lib/libbigmatrix.so"

void bigmatrix() {

  cout << " bigmatrix..." << endl;

  bigmatrix *matrix = new bigmatrix(bigmatrixpath);

  // Check Status
  bool status = matrix->Status();

  if (status) {
    // Draw bigmatrix
    TH2F* hBigMatrix = matrix->GetPlot();
    cBigMatrix = new TCanvas("cBigMatrix","cBigMatrix",1000,750);
    hBigMatrix->Draw("COLZ");
    matrix->DrawLines(cBigMatrix);
  }

  return;  
}
