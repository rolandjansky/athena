/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **************************************************************************
// Important shifters histograms
// **************************************************************************

void shifters() {

  gROOT->ProcessLine(".L include/dialogs.cc");

  // Subdetector selection

  // Inner Detector  
  int InDet = GetIntegerDialog("Select Inner Detector (yes: 1 - no: 0)", 1);
  if (InDet == 1) { 
    int Pixels = GetIntegerDialog("Select Pixels subdetector (yes: 1 - no: 0)", 1);
    int SCT = GetIntegerDialog("Select SCT subdetector (yes: 1 - no: 0)", 1);
    int TRT = GetIntegerDialog("Select TRT subdetector (yes: 1 - no: 0)", 1);
  }

  // LAr
  int LAr = GetIntegerDialog("Select LAr subdetector (yes: 1 - no: 0)", 1);

  // Tile
  int Tile = GetIntegerDialog("Select Tile subdetector (yes: 1 - no: 0)", 1);

  // Muons
  int Muons = GetIntegerDialog("Select Muons subdetector (yes: 1 - no: 0)", 1);
  if (Muons == 1) { 
    int RPC = GetIntegerDialog("Select RPC subdetector (yes: 1 - no: 0)", 1);
    int MDT = GetIntegerDialog("Select MDT subdetector (yes: 1 - no: 0)", 1);
  }

  gROOT->ProcessLine(".U include/dialogs.cc");

  // Histograms production

  TCanvas *c666 = new TCanvas("c666","Run Information");
  c666->Range(0,0,80,30);
  TLatex tip;
  tip.SetTextSize(0.05);
  char clatex[80];
  sprintf(clatex,"RUN: %d",run);
  tip.DrawLatex(17,17,clatex);
  sprintf(name,"%s(",psfilename);
  c666->Print(name);

  if (InDet==1) {

    if (Pixels==1) {
      sprintf(command,".x %s/Pixel_hitmaps_tot_dynamic.C(3)",path0);
      gROOT->ProcessLine(command);
      sprintf(command,".x %s/pixel_correlation_all.C(3)",path0);
      gROOT->ProcessLine(command);
    }

    if (SCT==1) {
      sprintf(command,".x %s/hitmaps.cc(3)",path1);
      gROOT->ProcessLine(command);
      // Phi 0
      sprintf(command,".x %s/correlations.cc(3,0)",path1);
      gROOT->ProcessLine(command);
      // Phi 1
      sprintf(command,".x %s/correlations.cc(3,1)",path1);
      gROOT->ProcessLine(command);
    }

    if (TRT==1) {
      sprintf(command,".x %s/global_histos.cc(3)",path2);
      gROOT->ProcessLine(command);
      // Phi 0
      sprintf(command,".x %s/trt_hitmaps.cc(3,0)",path2);
      gROOT->ProcessLine(command);
      // Phi 1
      sprintf(command,".x %s/trt_hitmaps.cc(3,1)",path2);
      gROOT->ProcessLine(command);
    }
	
  }

  if (LAr==1) {
    // Nothing
  }

  if (Tile==1) {
    // Nothing
  }

  if (Muons==1) {
    if (MDT==1) {
      sprintf(command,".x %s/mdt/mdt_barrel_profile.cc(3)",path5);
      gROOT->ProcessLine(command);
      sprintf(command,".x %s/mdt/mdt_endcap_profile.cc(3)",path5);
      gROOT->ProcessLine(command);
    }
  }
  
  // Correlations
  if (InDet==1) {
    
    if (Pixels==1 && SCT==1) {
      sprintf(command,".x %s/row_vs_strip.cc(3,0)",path3);
      gROOT->ProcessLine(command);
      sprintf(command,".x %s/row_vs_strip.cc(3,1)",path3);
      gROOT->ProcessLine(command);
    }
    
    if (Pixels==1 && TRT==1) {
      // only phi=0 for TRT
      //sprintf(command,".x %s/row_vs_TRT_straw_Y.cc(3,0,0)",path3);
      //gROOT->ProcessLine(command);
      //sprintf(command,".x %s/row_vs_TRT_straw_Y.cc(3,0,1)",path3);
      //gROOT->ProcessLine(command);
      sprintf(command,".x %s/pixel_TRT_y.cc(3,1,0)",path3);
      gROOT->ProcessLine(command);
    }
    
    if (SCT==1 && TRT==1) {
      // only phi=0 for TRT
      sprintf(command,".x %s/strip_vs_straw_detail.cc(3,0,0,0,0)",path3);
      gROOT->ProcessLine(command);
    }
    
    if (TRT==1 && Muons==1) {
      //if (MDT==1) {
      //sprintf(command,".x %s/trtHL_MDT_2.cc(3)",path3);
      //gROOT->ProcessLine(command);
      //}
      if (RPC==1) {
	sprintf(command,".x %s/trt_rpc_time.cc(3,2,1,1)",path3);
	gROOT->ProcessLine(command);
      }
    }

    if (Muons==1) {
      sprintf(command,".x %s/InDet_Muon_Track.cc(3)",path3);
      gROOT->ProcessLine(command);
    }
    
  }

  sprintf(name,"%s)",psfilename);
  c666->Print(name);

  cout << "\nfinished!" << endl;  
}
