/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Offline monitor for the Simulation

void sct_shifters(void) {
  sprintf(command,".x %s/hitmaps.cc(0)",path1);
  gROOT->ProcessLine(command);
  gPad->Print("shifters_histograms.ps(");
  sprintf(command,".x %s/tbin.cc(0)",path1);
  gROOT->ProcessLine(command);
  gPad->Print("shifters_histograms.ps)");
}
