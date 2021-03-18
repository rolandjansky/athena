/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include<include/tile_monitor.h>

void selectFile() {
  
  gROOT->ProcessLine(".L ./include/dialogs.cc");
  
  // Getting the OpenDialog
  const char *newfile = OpenFileDialog();
  
  // Opening the new file
  file = new TFile(newfile);
  if (file->IsZombie()) {
    cout << "Error opening file!" << endl;
    run = 0;
    fi le();
    exit(-1);
  }
  else {
    cout << "\n New selected file: " << newfile << endl;
    
    gROOT->ProcessLine(".U ./include/dialogs.cc");
  }
}

void tile_monitor(void){
  
  std::cout<<" *************************************"<<std::endl;
  std::cout<<" *                                   *"<<std::endl;
  std::cout<<" *         Tile Monitoring           *"<<std::endl;
  std::cout<<" *                                   *"<<std::endl;
  std::cout<<" *************************************"<<std::endl;

  // include default tilemon.root file

  file=new TFile("../run/tilemon.root");
  if (file->IsZombie()) {
    cout << "Error opening file!" << endl;
    cout << "Select a input file!" << endl;
    selectFile();
    if(!file)exit(-1);
  }
  
  MenuMain();

}

void MenuMain(){
  barMain = new TControlBar("vertical","Tile",600,50);
  barMain->AddButton("Select a new &file","file()","Select a data file");

  barMain->AddButton("Tile Cell Histograms","tile_cell()","Tile Cell histograms");
  barMain->AddButton("Tile Tower Histograms","tile_tower()","Tile Tower histograms");
  barMain->AddButton("Tile Cluster Histograms","tile_cluster()","Tile Cluster histograms");
  barMain->AddButton("Tile MuId Histograms","tile_muid()","Tile MuId histograms");

  barMain->AddButton("&Quit","quit()","Stop and Quit");

  barMain->Show();
  gROOT->SaveContext();
}


void tile_cell(){
  gROOT->ProcessLine(".x ./tile_cell.cc");
}

void tile_tower(){
  gROOT->ProcessLine(".x ./tile_tower.cc");
}

void tile_cluster(){
  gROOT->ProcessLine(".x ./tile_cluster.cc");
}

void tile_muid(){
  gROOT->ProcessLine(".x ./tile_muid.cc");
}

void quit() {
  gROOT->ProcessLine(".q");
}
