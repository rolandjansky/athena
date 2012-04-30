/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/monitor.h>

// Offline monitor for the Simulation

void trt_monitor(void) {
  printf("\n *************************************\n");
  printf(" *                                   *\n");
  printf(" *       TRT Offline Monitor         *\n");
  printf(" *                                   *\n");
  printf(" *************************************\n\n");

  MenuMain2();
  
}

// **************************************************************************

void MenuMain2(void){
  barMain = new TControlBar("vertical","TRT",1000,50);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("&Global Parameters","global()","Show global histograms");
  barMain->AddButton("Hitmaps [straw]","hitmaps()","TRT Hitmaps with straw position");
  barMain->AddButton("Hitmaps [y-coordinate]","hitmaps_y()","TRT Hitmaps with straw y position");
  barMain->AddButton("StrawLayer hitmap","strawlayer_hitmap()","Beam profile for the given straw layer");
  barMain->AddButton("StrawLayer hitmaps (All)","strawlayer_hitmap_all()","Beam profile for all straw layer");
  barMain->AddButton("Correlations [straw]","correlat()","TRT correlations between straw layers");
  barMain->AddButton("Correlations [y-coordinate]","correlat_y()","TRT correlations between straw layers with straw y position");
  barMain->AddButton("R-t TRT relation","R_t_menu()","R-t TRT relation");
  barMain->AddButton("Track residuals and pulls for TRT","trk_res()","Track residuals and pulls for TRT");
  barMain->AddButton("Track errors for TRT","trk_err()","Track errors for TRT");

  //barMain->AddButton("&Quit","quit()","Stop and Quit");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************

void quit() {
  gROOT->ProcessLine(".q");
}

// **************************************************************************

void global(void) {
  sprintf(command,".x %s/global_histos.cc(0)",path2);
  gROOT->ProcessLine(command);
}

void strawlayer_hitmap(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int TRT_OPT = GetIntegerDialog("Select TRT straw layer (Options: 0 - 72)", 0);
  if (TRT_OPT==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}
  sprintf(command,".x %s/TRT_straw_layer_hit_map.C(TRT_OPT)",path2);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

void strawlayer_hitmap_all(void) {
  sprintf(command,".x %s/TRT_straw_layer_hit_map_all.C()",path2);
  gROOT->ProcessLine(command);
}



// **************************************************************************
// TRT_Hits
// **************************************************************************

void hitmaps(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");
  
  int PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  if(PHI==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}

  sprintf(command,".x %s/trt_hitmaps.cc(0,PHI)",path2);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

void hitmaps_y(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  if(PHI==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}

  sprintf(command,".x %s/trt_hitmaps_y.cc(0,PHI)",path2);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}


// **************************************************************************
// Correlations
// **************************************************************************

void correlat(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int DCLAYER = GetIntegerDialog("Select TRT layer (Options: 0, 1, 2)", 0);
  if(DCLAYER==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}

  int DCPHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  if(DCPHI==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}

  sprintf(command,".x %s/straw_correls.cc(0,DCLAYER,DCPHI)",path2);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

void correlat_y(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int DCLAYER = GetIntegerDialog("Select TRT layer (Options: 0, 1, 2)", 0);
  if(DCLAYER==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}

  int DCPHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  if(DCPHI==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}

  sprintf(command,".x %s/straw_y_correls.cc(0,DCLAYER,DCPHI)",path2);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

// **************************************************************************
// R_t relation
// **************************************************************************

void R_t_menu(void){
  barMain = new TControlBar("vertical","R-t relation",1000,50);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("R-t TRT relation (global)","R_t1()","R-t TRT relation");
  barMain->AddButton("R-t TRT relation (straw-layer selection)","R_t2()","R-t TRT relation");
  barMain->AddButton("R-t TRT relation (straw selection)","R_t_perstraw()","R-t TRT relation");
  barMain->AddButton("R-t TRT relation (layer selection)","R_t_allstrawlayer()","R-t TRT relation");
  barMain->Show();
  gROOT->SaveContext();

}

void R_t1(void) {
  sprintf(command,".x %s/t_vs_R.cc(0)",path2);
  gROOT->ProcessLine(command);
}

void R_t2(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int LAYER = GetIntegerDialog("Select TRT layer (Options: 0, 1, 2)", 0);
  if(LAYER==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}
  int PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  if(PHI==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}
  int STRAWLAYER = GetIntegerDialog("Select TRT straw-layer", 0);
  if(STRAWLAYER==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}
 

  sprintf(command,".x %s/t_R.cc(PHI,LAYER,STRAWLAYER)",path2);
  gROOT->ProcessLine(command);
 
  gROOT->ProcessLine(".U include/dialogs.cc");
}

void R_t_perstraw(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int LAYER = GetIntegerDialog("Select TRT layer (Options: 0, 1, 2)", 0);
  if(LAYER==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}
  int PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  if(PHI==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}
  int STRAWLAYER = GetIntegerDialog("Select TRT straw-layer", 0);
  if(STRAWLAYER==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}
  int STRAW = GetIntegerDialog("Select TRT straw", 10);
  if(STRAW==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}

  sprintf(command,".x %s/t_R_perstraw.cc(0,PHI,LAYER,STRAWLAYER,STRAW)",path2);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}

void R_t_allstrawlayer(void) {
  gROOT->ProcessLine(".L include/dialogs.cc");

  int LAYER = GetIntegerDialog("Select TRT layer (Options: 0, 1, 2)", 0);
  if(LAYER==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}
  int PHI = GetIntegerDialog("Select TRT phi (Options: 0, 1)", 0);
  if(PHI==-999) {gROOT->ProcessLine(".U include/dialogs.cc"); return;}


  sprintf(command,".x %s/t_vs_R_strawlayer.cc(PHI,LAYER)",path2);
  gROOT->ProcessLine(command);

  gROOT->ProcessLine(".U include/dialogs.cc");
}
// **************************************************************************
// Tracking
// **************************************************************************

void trk_res(void) {
  sprintf(command,".x %s/track_trt_res.C(0)",path2);
  gROOT->ProcessLine(command);
}

void trk_err(void) {
  sprintf(command,".x %s/track_trt_err.C(0)",path2);
  gROOT->ProcessLine(command);
}

