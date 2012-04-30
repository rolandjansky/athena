/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/monitor.h>

// Offline monitor for the Simulation

void tile_monitor(void) {
  printf("\n *************************************\n");
  printf(" *                                   *\n");
  printf(" *       Tile Offline Monitor        *\n");
  printf(" *                                   *\n");
  printf(" *************************************\n\n");

  MenuMain4();
  
}

// **************************************************************************

void MenuMain4(void){
  barMain = new TControlBar("vertical","Tile",600,350);
  barMain->SetNumberOfColumns(2);

  barMain->AddButton("&Mean energy in drawers",".x Tile_macros/drawers.cc","each module is made of 2 drawers");
  barMain->AddButton("&Energy vs eta",".x Tile_macros/towers.cc","energy in each eta tower");
  barMain->AddButton("&Total energy",".x Tile_macros/energy_in_tile.cc","Total energy in TileCal");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************

void quit() {
  gROOT->ProcessLine(".q");
}

// **************************************************************************

