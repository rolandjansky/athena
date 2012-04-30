/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <include/monitor.h>

// InnerDetector Offline Monitor

void monitor() {

  printf("\n *************************************\n");
  printf(" *                                   *\n");
  printf(" *         Offline Monitor           *\n");
  printf(" *                                   *\n");
  printf(" *************************************\n\n");


  // Read the ntuple
  file = new TFile("../run/ntuple.root");
  //file = TFile::Open("rfio:/castor/cern.ch/grid/atlas/datafiles/ctb/realdata/10.0.2.v1/ntp_2102009_10.0.2.root");
  if (file->IsZombie()) {
    cout << "Error opening file!" << endl;
    run = 0;
    file();
    // exit(-1);
  }
  
  tree = (TTree*)file->Get("TB/tree");
  
  // Load the menu  
  MenuMain();
    
}

// **************************************************************************

void MenuMain(){
  barMain = new TControlBar("vertical","Monitor",820,50);
  barMain->SetNumberOfColumns(2);
  barMain->AddButton("&Pixels","pixel_menu()","Pixels Offline Monitor");
  barMain->AddButton("&SCT","sct_menu()","SCT Offline Monitor");
  barMain->AddButton("&TRT","trt_menu()","TRT Offline Monitor");
  barMain->AddButton("&LAr","lar_menu()","LAr Offline Monitor");
  barMain->AddButton("Ti&le","tile_menu()","Tile Offline Monitor");
  barMain->AddButton("&Muons","muons_menu()","Muons Offline Monitor");
  barMain->AddButton("&Correlations","corr_menu()","Correlations between detectors");
  barMain->AddButton("Shi&fters","shifters()","Important histograms for shifters");
  barMain->AddButton("Select a new &file","file()","Select a data file");
  barMain->AddButton("&Quit","quit()","Stop and Quit");
  barMain->Show();
  gROOT->SaveContext();
   
}

// **************************************************************************

void quit() {
  gROOT->ProcessLine(".q");
}

void file() {
  
  gROOT->ProcessLine(".L include/dialogs.cc");
  
  // Getting the OpenDialog
  const char *newfile = OpenFileDialog();
  
  // Openning the new file
  file = new TFile(newfile);
  if (file->IsZombie()) {
    cout << "Error opening file!" << endl;
    run = 0;
    fi le();
    exit(-1);
  }
  else {
    tree = (TTree*)file->Get("TB/tree");
    
    cout << "\n New selected file: " << newfile << endl;
    
    gROOT->ProcessLine(".U include/dialogs.cc");
    
    // Get the run number
    tree->SetBranchAddress("Run",&run);
    
    for (Int_t i=0;i<1;i++) {
      tree->GetEntry(i);
      cout<<"\nRun number: "<<run<<"\n"<<endl;
    }
  }
}

// **************************************************************************

void pixel_menu(){
  sprintf(command,".x %s/pixel_monitor.cc",path0);
  gROOT->ProcessLine(command);
}

void sct_menu() {
  sprintf(command,".x %s/sct_monitor.cc",path1);
  gROOT->ProcessLine(command);
}

void trt_menu() {
  sprintf(command,".x %s/trt_monitor.cc",path2);
  gROOT->ProcessLine(command);
}

void corr_menu() {
  sprintf(command,".x %s/corr_monitor.cc",path3);
  gROOT->ProcessLine(command);
}

void lar_menu() {
  sprintf(command,".x %s/lar_monitor.cc",path6);
  gROOT->ProcessLine(command);
}

void tile_menu() {
  sprintf(command,".x %s/tile_monitor.cc",path4);
  gROOT->ProcessLine(command);
}

void muons_menu() {
  sprintf(command,".x %s/muon_monitor.cc",path5);
  gROOT->ProcessLine(command);
}


// **************************************************************************
// Important shifters histograms
// **************************************************************************

void shifters() {
  sprintf(command,".x include/shifters.cc");
  gROOT->ProcessLine(command);
}
