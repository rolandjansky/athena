/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/monitor.h>

void muon_monitor(void) {
  printf("\n *************************************\n");
  printf(" *                                   *\n");
  printf(" *       Muon Offline Monitor        *\n");
  printf(" *                                   *\n");
  printf(" *************************************\n\n");

  MenuMain5();
  
}

// **************************************************************************

void MenuMain5(void){
  barMain = new TControlBar("vertical","Muons",600,50);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("MDT Barrel Beam Profiles","mdt_barrel_profile()","Barrel Beam Profiles");
  barMain->AddButton("MDT Endcap Beam Profiles","mdt_endcap_profile()","Endcap Beam Profiles");
  barMain->AddButton("MDT Barrel TDC Spectra","mdt_barrel_tdc()","Barrel TDC Spectra");
  barMain->AddButton("MDT Endcap TDC Spectra","mdt_endcap_tdc()","Endcap TDC Spectra");
  barMain->AddButton("RPC LowPt Eta","rpc_lowpt_eta()","RPC LowPt Eta");
  barMain->AddButton("RPC LowPt Phi","rpc_lowpt_phi()","RPC LowPt Phi");
  barMain->AddButton("RPC Pivot Eta","rpc_pivot_eta()","RPC Pivot Eta");
  barMain->AddButton("RPC Pivot Phi","rpc_pivot_phi()","RPC Pivot Phi");
  barMain->AddButton("RPC HighPt Eta","rpc_highpt_eta()","RPC HigPt Eta");
  barMain->AddButton("RPC HighPt Phi","rpc_highpt_phi()","RPC HigPt Phi");
  barMain->AddButton("TGC Wire hit" ,"tgc_wire_hit()" ,"TGC WIRE Hit" );
  barMain->AddButton("TGC Strip hit","tgc_strip_hit()","TGC STRIP Hit");
  barMain->AddButton("MUCTPI PT"    ,"muctpi_pt()"    ,"MUCTPI PT"    );
  
  //barMain->AddButton("&Quit","quit()","Stop and Quit");
  barMain->Show();
  gROOT->SaveContext();
}

// **************************************************************************
void rpc_highpt_phi() {
  sprintf(command,".x %s/rpc/rpc_highpt_phi.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void quit() {
  gROOT->ProcessLine(".q");
}

// **************************************************************************

void mdt_barrel_profile() {
  sprintf(command,".x %s/mdt/mdt_barrel_profile.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void mdt_endcap_profile() {
  sprintf(command,".x %s/mdt/mdt_endcap_profile.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void mdt_barrel_tdc() {
  sprintf(command,".x %s/mdt/mdt_barrel_tdc.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void mdt_endcap_tdc() {
  sprintf(command,".x %s/mdt/mdt_endcap_tdc.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void rpc_lowpt_eta() {
  sprintf(command,".x %s/rpc/rpc_lowpt_eta.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void rpc_lowpt_phi() {
  sprintf(command,".x %s/rpc/rpc_lowpt_phi.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void rpc_pivot_eta() {
  sprintf(command,".x %s/rpc/rpc_pivot_eta.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void rpc_pivot_phi() {
  sprintf(command,".x %s/rpc/rpc_pivot_phi.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void rpc_highpt_eta() {
  sprintf(command,".x %s/rpc/rpc_highpt_eta.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void rpc_highpt_phi() {
  sprintf(command,".x %s/rpc/rpc_highpt_phi.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void tgc_wire_hit() {
  sprintf(command,".x %s/tgc/tgc_wire_hit.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void tgc_strip_hit() {
  sprintf(command,".x %s/tgc/tgc_strip_hit.cc(0)",path5);
  gROOT->ProcessLine(command);
}

void muctpi_pt() {
  sprintf(command,".x %s/muctpi/muctpi_pt.cc(0)",path5);
  gROOT->ProcessLine(command);
}

