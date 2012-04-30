/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/monitor.h>

// Offline monitor for the Simulation

void pixel_monitor(void) {
  printf("\n *************************************\n");
  printf(" *                                   *\n");
  printf(" *     Pixels Offline Monitor        *\n");
  printf(" *                                   *\n");
  printf(" *************************************\n\n");

  MenuMain0();
  
}

// **************************************************************************

void MenuMain0(void){
  barMain = new TControlBar("vertical","Pixels",910,50);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("&Hitmaps","pixelhitmaps()","Pixel Hitmaps");
  barMain->AddButton("Tot Hitmaps","tot()","Time Over Threshold Hitmaps");
  barMain->AddButton("Correlations","corr()","Hit correlations");
  barMain->AddButton("Correlations All","corr2()","Hit correlations");
  barMain->AddButton("Hit histograms col and row","histo()","Hit histograms col and row");
  barMain->AddButton("Cluster histograms LocX and LocY","clusterhisto()","Cluster histograms LocX and LocY");
  barMain->AddButton("BCID histograms per module","BCIDhisto()","BCID histograms per module");
  barMain->AddButton("Track Residuals and Pulls","trackres()","Track Residuals and Pulls");
  barMain->AddButton("Track Residuals and Pulls per module","trackresmod()","Track Residuals and Pulls per module");
  barMain->AddButton("Track and Hit errors","trackerr()","Track and Hit errors");
  barMain->AddButton("Track and Hit errors per module","trackerrmod()","Track and Hit errors per module");
  //barMain->AddButton("&Quit","quit()","Stop and Quit");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************

void quit() {
  gROOT->ProcessLine(".q");
}

// **************************************************************************

// PIXEL_Hits

void pixelhitmaps(void) {
  sprintf(command,".x %s/Pixel_hitmaps_dynamic.C(0)",path0);
  gROOT->ProcessLine(command);
}

void tot() {
  sprintf(command,".x %s/Pixel_hitmaps_tot_dynamic.C(0)",path0);
  gROOT->ProcessLine(command);
}

void histo() {
  sprintf(command,".x %s/pixel_histo_colrow.C(0)",path0);
  gROOT->ProcessLine(command);
}

void clusterhisto() {
  sprintf(command,".x %s/pixel_cluster_histo_LocXY.C(0)",path0);
  gROOT->ProcessLine(command);
}

void corr() {
  sprintf(command,".x %s/pixel_correlation.C(0)",path0);
  gROOT->ProcessLine(command);
}

void corr2() {
  sprintf(command,".x %s/pixel_correlation_all.C(0)",path0);
  gROOT->ProcessLine(command);
}

void BCIDhisto() {
  sprintf(command,".x %s/pixel_BCID.C(0)",path0);
  gROOT->ProcessLine(command);
}

void trackres() {
  sprintf(command,".x %s/track_pixel_res.C(0)",path0);
  gROOT->ProcessLine(command);
}

void trackresmod() {
  sprintf(command,".x %s/track_pixel_res_modules.C(0)",path0);
  gROOT->ProcessLine(command);
}

void trackerr() {
  sprintf(command,".x %s/track_pixel_err.C(0)",path0);
  gROOT->ProcessLine(command);
}

void trackerrmod() {
  sprintf(command,".x %s/track_pixel_err_modules.C(0)",path0);
  gROOT->ProcessLine(command);
}
