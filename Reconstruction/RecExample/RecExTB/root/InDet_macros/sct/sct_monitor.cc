/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Offline monitor for the Simulation

void sct_monitor(void) {
  printf("\n *************************************\n");
  printf(" *                                   *\n");
  printf(" *       SCT Offline Monitor         *\n");
  printf(" *                                   *\n");
  printf(" *************************************\n\n");

  MenuMain1();
  
}

// **************************************************************************

void MenuMain1(void){
  barMain = new TControlBar("vertical","SCT",600,50);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("&Hitmaps","hitmaps()","SCT Hitmaps for plane");
  barMain->AddButton("&Multi","multi()","SCT Multiplicities per plane");
  barMain->AddButton("Time&Bin histograms","tbin()","TimeBin information");
  barMain->AddButton("&Correlations","correlations()","Phi Correlations between planes");
  barMain->AddButton("Cluster: Hits vs Y","clusHits_vs_y()","Hits vs Y with Clusters");
  barMain->AddButton("Cluster: Hits vs Z","clusHits_vs_z()","Hits vs Z with Clusters");
  barMain->AddButton("SpacePoint: Hits vs Y","spHits_vs_y()","Hits vs Y with SpacePoints");
  barMain->AddButton("SpacePoint: Hits vs Z","spHits_vs_z()","Hits vs Z with SpacePoints");
  barMain->AddButton("SpacePoint: &Frontview [Y vs Z]","frontview_sp()","Frontview [Y vs Z] (Global coordinates)");
  barMain->AddButton("SpacePoint: &3D Frontview [Y vs Z]","frontview_sp3D()","3D Frontview [Y vs Z] (Global coordinates)");
  barMain->AddButton("Bad channels search","find()","Find the BAD channels");
  barMain->AddButton("Track residuals for SCT","trk_res()","Track residuals for SCT (all modules together)");
  barMain->AddButton("Track residuals for SCT (module)","trk_res_modules()","Track residuals for SCT (per module)");
  barMain->AddButton("Track errors for SCT","trk_err()","Track errors for SCT (all modules together)");
  barMain->AddButton("&Run all","RunAll()","Run all macros");
  barMain->AddButton("&Save all","SaveAll()","Run all macros and store the results");
  //barMain->AddButton("&Quit","quit()","Stop and Quit");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************

void quit() {
  gROOT->ProcessLine(".q");
}

// **************************************************************************

void SaveAll(void){
  barMain = new TControlBar("vertical","Save All",840,70);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("PS","SaveAll_PS()","Save the histograms in GIF format");
  barMain->AddButton("GIF","SaveAll_GIF()","Save the histograms in GIF format");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************

void SaveAll_PS(void) {

  gROOT->ProcessLine(".x InDet_macros/sct/hitmaps.cc(1)");
  gROOT->ProcessLine(".x InDet_macros/sct/tbin.cc(1)");
  gROOT->ProcessLine(".x InDet_macros/sct/correlations.cc(1,0)");
  gROOT->ProcessLine(".x InDet_macros/sct/correlations.cc(1,1)");
  gROOT->ProcessLine(".x InDet_macros/sct/clusHits_vs_y.cc(1)");
  gROOT->ProcessLine(".x InDet_macros/sct/clusHits_vs_z.cc(1)");
  gROOT->ProcessLine(".x InDet_macros/sct/spHits_vs_y.cc(1)");
  gROOT->ProcessLine(".x InDet_macros/sct/spHits_vs_z.cc(1)");
  gROOT->ProcessLine(".x InDet_macros/sct/frontview_sp.cc(1)");
  sprintf(option,"COLZ");
  gROOT->ProcessLine(".x InDet_macros/sct/frontview_sp3D.cc(1,option)");
  // gROOT->ProcessLine(".q");
}

void SaveAll_GIF(void) {
  gROOT->ProcessLine(".x InDet_macros/sct/hitmaps.cc(2)");
  gROOT->ProcessLine(".x InDet_macros/sct/tbin.cc(2)");
  gROOT->ProcessLine(".x InDet_macros/sct/correlations.cc(2,0)");
  gROOT->ProcessLine(".x InDet_macros/sct/correlations.cc(2,1)");
  gROOT->ProcessLine(".x InDet_macros/sct/clusHits_vs_y.cc(2)");
  gROOT->ProcessLine(".x InDet_macros/sct/clusHits_vs_z.cc(2)");
  gROOT->ProcessLine(".x InDet_macros/sct/spHits_vs_y.cc(2)");
  gROOT->ProcessLine(".x InDet_macros/sct/spHits_vs_z.cc(2)");
  gROOT->ProcessLine(".x InDet_macros/sct/frontview_sp.cc(2)");
  sprintf(option,"COLZ");
  gROOT->ProcessLine(".x InDet_macros/sct/frontview_sp3D.cc(2,option)");
  // gROOT->ProcessLine(".q");
}

// **************************************************************************

void RunAll(void) {
  gROOT->ProcessLine(".x InDet_macros/sct/hitmaps.cc(0)");
  gROOT->ProcessLine(".x InDet_macros/sct/tbin.cc(0)");
  gROOT->ProcessLine(".x InDet_macros/sct/correlations.cc(0,0)");
  gROOT->ProcessLine(".x InDet_macros/sct/correlations.cc(0,1)");
  gROOT->ProcessLine(".x InDet_macros/sct/clusHits_vs_y.cc(0)");
  gROOT->ProcessLine(".x InDet_macros/sct/clusHits_vs_z.cc(0)");
  gROOT->ProcessLine(".x InDet_macros/sct/spHits_vs_y.cc(0)");
  gROOT->ProcessLine(".x InDet_macros/sct/spHits_vs_z.cc(0)");
  gROOT->ProcessLine(".x InDet_macros/sct/frontview_sp.cc(0)");
  sprintf(option,"COLZ");
  gROOT->ProcessLine(".x InDet_macros/sct/frontview_sp3D.cc(0,option)");
  // gROOT->ProcessLine(".q");
}

// **************************************************************************

// SCT_Hits

void hitmaps(void) {
  sprintf(command,".x %s/hitmaps.cc(0)",path1);
  gROOT->ProcessLine(command);
}

void multi(void) {
  sprintf(command,".x %s/multi.C(0)",path1);
  gROOT->ProcessLine(command);
}

void tbin() {
  sprintf(command,".x %s/tbin.cc(0)",path1);
  gROOT->ProcessLine(command);
}

// **************************************************************************

void correlations(void){
  gROOT->ProcessLine(".L include/dialogs.cc");
  
  int SCT_PHI = GetIntegerDialog("Select SCT phi (Options: 0, 1)", 0);
  if(SCT_PHI==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}

  sprintf(command,".x %s/correlations.cc(0,SCT_PHI)",path1);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");

}

// **************************************************************************

// SCT_Clusters

void clusHits_vs_y() {
  sprintf(command,".x %s/clusHits_vs_y.cc(0)",path1);
  gROOT->ProcessLine(command);
}

void clusHits_vs_z() {
  sprintf(command,".x %s/clusHits_vs_z.cc(0)",path1);
  gROOT->ProcessLine(command);
}

// SCT_SpacePoins

void spHits_vs_y() {
  sprintf(command,".x %s/spHits_vs_y.cc(0)",path1);
  gROOT->ProcessLine(command);
}

void spHits_vs_z() {
  sprintf(command,".x %s/spHits_vs_z.cc(0)",path1);
  gROOT->ProcessLine(command);
}

void frontview_sp() {
  sprintf(command,".x %s/frontview_sp.cc(0)",path1);
  gROOT->ProcessLine(command);
}

// **************************************************************************

void frontview_sp3D(void){
  sprintf(option,"COLZ");
  gROOT->ProcessLine(".x InDet_macros/sct/frontview_sp3D.cc(0,option)");
}

// **************************************************************************
// Tracking
// **************************************************************************

void trk_res(void) {
  sprintf(command,".x %s/track_sct_res.C(0)",path1);
  gROOT->ProcessLine(command);
}

void trk_res_modules(void) {
  sprintf(command,".x %s/track_sct_res_modules.C(0)",path1);
  gROOT->ProcessLine(command);
}

void trk_err(void) {
  sprintf(command,".x %s/track_sct_err.C(0)",path1);
  gROOT->ProcessLine(command);
}

// **************************************************************************
// Extras
// **************************************************************************

void find(void) {
  sprintf(command,".x %s/findbad.c",path1);
  gROOT->ProcessLine(command);
}

