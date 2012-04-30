/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


void lar_monitor(void) {
  printf("\n *************************************\n");
  printf(" *                                   *\n");
  printf(" *       LAr Offline Monitor        *\n");
  printf(" *                                   *\n");
  printf(" *************************************\n\n");

  MenuMainLAr();
  
}

// **************************************************************************

void MenuMainLAr(void){
  barMain = new TControlBar("vertical","LAr",650,350);
  barMain->SetNumberOfColumns(2);

  barMain->AddButton("&Energy in LAr Calo",".x LAr_macros/lar_energy_resolution.cc","Energy in LAr Calo");
  barMain->AddButton("&Position in LAr Calo",".x LAr_macros/lar_position.cc","Energy in LAr Calo");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************

void quit() {
  gROOT->ProcessLine(".q");
}

// **************************************************************************

