/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/monitor.h>

// Offline monitor for the Simulation

void corr_monitor(void) {
  printf("\n *************************************\n");
  printf(" *                                   *\n");
  printf(" *   Correlations Offline Monitor    *\n");
  printf(" *                                   *\n");
  printf(" *************************************\n\n");

  MenuMain3();
  
}

// **************************************************************************

void MenuMain3(void){
  barMain = new TControlBar("vertical","Monitor Menu",820,240);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("Pixels-SCT (Row vs Strip) (Y>0)","pixelsct0()","Correlations between Pixels and SCT");
  barMain->AddButton("Pixels-SCT (Column vs Strip) (Y>0)","pixelsct1()","Correlations between Pixels and SCT");
  barMain->AddButton("Pixels-SCT (Row vs Strip) (Y<0)","pixelsct2()","Correlations between Pixels and SCT");
  barMain->AddButton("Pixels-SCT (Column vs Strip) (Y<0)","pixelsct3()","Correlations between Pixels and SCT");
  //barMain->AddButton("&Quit","quit()","Stop and Quit");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************

void quit() {
  gROOT->ProcessLine(".q");
}

// **************************************************************************

// Hits

void pixelsct0(void) {
  sprintf(command,".x %s/row_vs_strip.cc(0)",path3);
  gROOT->ProcessLine(command);
}

void pixelsct1(void) {
  sprintf(command,".x %s/col_vs_strip.cc(0)",path3);
  gROOT->ProcessLine(command);
}

void pixelsct2(void) {
  sprintf(command,".x %s/row_vs_strip.cc(1)",path3);
  gROOT->ProcessLine(command);
}

void pixelsct3(void) {
  sprintf(command,".x %s/col_vs_strip.cc(1)",path3);
  gROOT->ProcessLine(command);
}

