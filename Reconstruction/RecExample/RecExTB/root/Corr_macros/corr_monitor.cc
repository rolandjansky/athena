/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
  barMain = new TControlBar("vertical","Correlations",820,275);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("Pixels-SCT (Row vs Strip)","pixelsct0()","Correlations between Pixels and SCT [SCT Phi=0,1 means Pixels Phi=1,0]");
  barMain->AddButton("Pixels-SCT (SP vs SP)","pixelsctSP0()","Correlations between Pixels and SCT with SpacePoints [SCT Phi=0,1 means Pixels Phi=1,0]");
  barMain->AddButton("Pixels-TRT (Row vs Straw)","pixeltrt0()","Correlations between Pixels and TRT");
  barMain->AddButton("Pixels-TRT (Row vs Straw_y)","pixeltrt1()","Correlations between Pixels and TRT");
  barMain->AddButton("Pixels-TRT (Pixels_y vs Straw_y)","pixeltrt2()","Correlations between Pixels and TRT (y coordinate)");
  barMain->AddButton("SCT-TRT (Strip vs Straw)","scttrt0()","Correlations between SCT and TRT");
  barMain->AddButton("SCT-TRT (Strip_y vs Straw_y)","scttrt1()","Correlations between SCT and TRT (y coordinate)");
  barMain->AddButton("RPC-TRT (rtime vs timepll)","trtrpc_time()","Correlations between TRT and RPC");
  barMain->AddButton("RPC-TRT (rstrip vs straw)","trtrpc_strip()","Correlations between TRT and RPC");
  //barMain->AddButton("TRT-MDT (TRT HL with Mouns) in %","trtmdt_HL()","Correlations between TRT and MDT");
  //barMain->AddButton("TRT-MDT (TRT RDOs with and without Mouns)","trtmdt_HL2()","Correlations between TRT and MDT");
  barMain->AddButton("ID-Muons Track","ID_muons()","Correlations between ID-Muon Track");
  barMain->AddButton("ID Tracks","ID_tracks()","Correlations between ID Track");
  barMain->AddButton("LAr-Muons","ELAr_muons()","Correlations between LAr-Muons (Energy)");
  barMain->AddButton("LAr-Muons-TRT","ELAr_muons_trt()","Correlations between LAr-Muons-TRT (Energy) (for soft electrons)");
  barMain->AddButton("LAr-ID (phi and eta)","LAr_ID_phi_eta()","Correlations between LAr and ID in phi and eta");
  //barMain->AddButton("&Quit","quit()","Stop and Quit");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************

void quit() {
  gROOT->ProcessLine(".q");
}

// **************************************************************************
// Pixels-SCT
// **************************************************************************

void pixelsct0(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int Y = GetIntegerDialog("Select y axis (Options: 0, 1)", 0);

  sprintf(command,".x %s/row_vs_strip.cc(0,Y)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

void pixelsctSP0(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int Y = GetIntegerDialog("Select y axis (Options: 0, 1)", 0);

  sprintf(command,".x %s/pixelSP_vs_sctSP.cc(0,Y)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

// **************************************************************************
// Pixels-TRT
// **************************************************************************

void pixeltrt0(void) {

  gROOT->ProcessLine(".L include/dialogs.cc");

  int TRT_LAYER = GetIntegerDialog("Select TRT layer (Options: 0, 1, 2)", 0);
  int TRT_PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  int PIXELS_LAYER = GetIntegerDialog("Select Pixels layer (Options: 0, 1, 2)", 0);
  int PIXELS_PHI = GetIntegerDialog("Select Pixels phi (Options: 0, 1)", 0);

  sprintf(command,".x %s/row_vs_straw_detail.cc(0,TRT_LAYER,TRT_PHI,PIXELS_LAYER,PIXELS_PHI)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");

}

void pixeltrt1(void) {

  gROOT->ProcessLine(".L include/dialogs.cc");

  int TRT_PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 1);
  int PIXELS_PHI = GetIntegerDialog("Select Pixels phi (Options: 0, 1)", 0);

  sprintf(command,".x %s/row_vs_TRT_straw_Y.cc(0,TRT_PHI,PIXELS_PHI)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");

}

void pixeltrt2(void) {

  gROOT->ProcessLine(".L include/dialogs.cc");

  int TRT_PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 1);
  int PIXELS_PHI = GetIntegerDialog("Select Pixels phi (Options: 0, 1)", 0);

  sprintf(command,".x %s/pixel_TRT_y.cc(0,TRT_PHI,PIXELS_PHI)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");

}


// **************************************************************************
// SCT-TRT
// **************************************************************************

void scttrt0(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");
  
  int TRT_LAYER = GetIntegerDialog("Select TRT layer (Options: 0, 1, 2)", 0);
  int TRT_PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  int SCT_LAYER = GetIntegerDialog("Select SCT layer (Options: 0, 1, 2, 3)", 0);
  int SCT_PHI = GetIntegerDialog("Select SCT phi (Options: 0, 1)", 0);

  sprintf(command,".x %s/strip_vs_straw_detail.cc(0,TRT_LAYER,TRT_PHI,SCT_LAYER,SCT_PHI)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

void scttrt1(void) {

  gROOT->ProcessLine(".L include/dialogs.cc");

  int TRT_PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 1);
  int SCT_PHI = GetIntegerDialog("Select SCT phi (Options: 0, 1)", 0);

  sprintf(command,".x %s/SCT_TRT_y.cc(0,TRT_PHI,SCT_PHI)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");

}


// **************************************************************************
// TRT-RPC
// **************************************************************************

void trtrpc_time(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int STATION = GetIntegerDialog("Select RPC STATION (Options: 2, 4)", 2);
  int DOUBLR = GetIntegerDialog("Select RPC DOUBLR (Options: 1, 2)", 1);
  int GASGAP = GetIntegerDialog("Select RPC GASGAP (Options: 1, 2)", 1);

  sprintf(command,".x %s/trt_rpc_time.cc(0,STATION,DOUBLR,GASGAP)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

void trtrpc_strip(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int STATION = GetIntegerDialog("Select RPC STATION (Options: 2, 4)", 2);
  int DOUBLR = GetIntegerDialog("Select RPC DOUBLR (Options: 1, 2)", 1);
  int GASGAP = GetIntegerDialog("Select RPC GASGAP (Options: 1, 2)", 1);

  sprintf(command,".x %s/trt_rpc_strip.cc(0,STATION,DOUBLR,GASGAP)",path3);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

// **************************************************************************
// TRT-RPC
// **************************************************************************

//void trtmdt_HL(void) {
//  sprintf(command,".x %s/trtHL_MDT.cc(0)",path3);
//  gROOT->ProcessLine(command);
//}

//void trtmdt_HL2(void) {
//  sprintf(command,".x %s/trtHL_MDT_2.cc(0)",path3);
//  gROOT->ProcessLine(command);
//}

// **************************************************************************
// ID-Muons Track
// **************************************************************************
void ID_muons(void) {
  sprintf(command,".x %s/InDet_Muon_Track.cc(0)",path3);
  gROOT->ProcessLine(command);
}

// **************************************************************************
// ID Tracks correlation in phi
// **************************************************************************
void ID_tracks(void) {
  sprintf(command,".x %s/tracks_ID.cc(0)",path3);
  gROOT->ProcessLine(command);
}

// **************************************************************************
// LAr-Muons (Energy)
// **************************************************************************
void ELAr_muons(void) {
  sprintf(command,".x %s/ELAr_Muons.cc(0)",path3);
  gROOT->ProcessLine(command);
}

// **************************************************************************
// LAr-Muons-TRT (Energy)
// **************************************************************************
void ELAr_muons_trt(void) {
  sprintf(command,".x %s/ELAr_Muons_TRT.cc(0)",path3);
  gROOT->ProcessLine(command);
}

// **************************************************************************
// LAr-ID (phi and eta)
// **************************************************************************
void LAr_ID_phi_eta()(void) {
  sprintf(command,".x %s/InDet_LAr_phi_eta.cc(0)",path3);
  gROOT->ProcessLine(command);
}


