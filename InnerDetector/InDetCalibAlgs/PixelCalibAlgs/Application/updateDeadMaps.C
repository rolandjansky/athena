/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<vector>
#include<cmath>
#include<map>
#include<sstream>
#include<string>
#include<iostream>
#include<fstream>
#include<utility>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TKey.h"
#include "Riostream.h"

#include "PixelCalibAlgs/PixelConvert.h"
#include "PixelConditionsData/SpecialPixelMap.h"

std::vector< std::pair< std::string, std::vector<int> > > pixelMapping;
std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta);
std::vector<int> getPositionFromDCSID (std::string module_name);

std::vector<std::string> &splitter(const std::string &s, char delim, std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

std::vector<std::string> splitter(const std::string &s, char delim) {
  std::vector<std::string> elems;
  splitter(s, delim, elems);
  return elems;
}

bool is_file_exist(const char *fileName)
{
  std::ifstream infile;
  infile.open(fileName);
  return infile.good();
}

using namespace std;

int main(int argc, char* argv[]){
  bool isIBL = true;

  int minEventNumber = 2000000;
  int minAveragePixelHitNumber = 13;

  int nPixelsPerModule = 46080;
  double nModules = 1744.;
  //double nPixels = nModules*nPixelsPerModule;

  if (isIBL) {
    //nModules = 2048.;
    nModules = 2024.; // ignore DBM
    //nPixels = 2880. * 16. * 1744. // Pixel
    //  + 26880. * 112. + 53760. * 168.  // IBL 3D + Planar
    //  + 26880. * 24; // DBM
    //nPixels = 2880. * 16. * 1744. // Pixel
    //  + 26880. * 112. + 53760. * 168.;  // IBL 3D + Planar
  }
  //-----------------------------------
  //Usage configuration of the program
  //------------------------------------
  std::string option;
  std::string finname;
  std::string foutname;

  bool optionOnline = false;

  if( argc == 3 ){
    finname=argv[argc - 2];
    foutname=argv[argc - 1];
  }
  else if( argc == 4 ){
    finname=argv[argc - 2];
    foutname=argv[argc - 1];
    option = argv[1];
    if( option == "--online" ) optionOnline = true;
  }
  else{
    std::cout << "Usage: updateDeadMaps [--online] <inputfile> <outputfile> " << std::endl;
    return 0;
  }

  //-----------------------------
  //Open input and output files
  //------------------------------
  TFile* hitMapFile = new TFile(finname.c_str(), "READ");
  if( !hitMapFile || !hitMapFile->IsOpen() ){
    std::cerr << "Error: Unable to open input file." << std::endl;
    return 1;
  }

  ofstream logfile;
  logfile.open("DeadPixels.txt");

  TH1D* nEventsHistogram = 0;
  hitMapFile->GetObject("NEvents", nEventsHistogram);
  double nEvents = nEventsHistogram->GetBinContent(1);

  if(nEvents < minEventNumber) {
    std::cerr << "WARNING!" << std::endl;
    std::cerr << "Number of events in the file is "<< nEvents <<std::endl;
    std::cerr << "Minimum required event number to have an accurate estimate of dead pixels is "<< minEventNumber <<std::endl;
  }


  TFile* deadMapFile = new TFile(foutname.c_str(), "RECREATE");
  if( !deadMapFile || !deadMapFile->IsOpen() ){
    std::cerr << "Error: Unable to open output file." << std::endl;
    return 1;
  }

  //-------------------------------
  //Read configuration file
  //-------------------------------

  std::string text;
  std::ifstream cutConfigFile;
  cutConfigFile.open("updateDeadMapsConfig.txt");

  if( cutConfigFile.is_open() ){

    std::cout << std::endl
      << "Configuration read from file updateDeadMapsConfig.txt:"
      << std::endl << std::endl;

    cutConfigFile >> text >> minEventNumber;
    cutConfigFile >> text >> minAveragePixelHitNumber;

  }
  else{
    std::cout << std::endl
      << "File updateDeadMapsConfig.txt not in current directory, using default occupancy cuts:"
      << std::endl << std::endl;

  }
  std::cout<<"Minimum event number required: "<<minEventNumber<<std::endl;
  std::cout<<"Minimum average hits/pixl required per module: "
    <<minAveragePixelHitNumber<<std::endl;
  std::cout<<std::endl<<std::endl;

  //-------------------------------
  // Initialization
  //-------------------------------
  std::vector<std::string> components;
  components.push_back("Disk1A");
  components.push_back("Disk2A");
  components.push_back("Disk3A");
  components.push_back("Disk1C");
  components.push_back("Disk2C");
  components.push_back("Disk3C");
  if (isIBL) components.push_back("IBL");     // kazuki
  components.push_back("B-layer");
  components.push_back("Layer1");
  components.push_back("Layer2");

  //std::map<unsigned int, TH2D*> hitMaps;
  //std::map<unsigned int, TH2C*> deadMaps;
  std::map<std::string, TH2D*> hitMaps;
  std::map<std::string, TH2C*> deadMaps;
  std::map<unsigned int, TH2C*> deadMapsOnline;
  //std::map<unsigned int, TH2C*> deadRefMaps;
  std::map<unsigned int, TH2C*> deadRefMapsOnline;

  TH1D* disabledModules = 0;
  TH1D* deadPlot = 0;

  TH1D* deadPixel_layer = 0;
  TH1D* deadPixel_layer_all = 0;

  std::vector< TH1D*> h_deadPixel;

  //double ndeadPixel[9]={0.};
  double ndeadPixel[10]={0.}; // add IBL
  double ndeadPixel_total=0;
  //double ndeadPixel_noDisabled[9]={0.};
  double ndeadPixel_noDisabled[10]={0.}; // add IBL

  hitMapFile->GetObject("DisabledModules", disabledModules);

  if( !optionOnline ){

    nEventsHistogram->SetDirectory(deadMapFile);

    disabledModules->SetDirectory(deadMapFile);

    deadPlot = new TH1D("deadPlot","Zero hit pixels per module",100,-0.5,99.5);
    deadPlot->SetDirectory(deadMapFile);

    deadPixel_layer = new TH1D("deadPixel_layer","Zero hit pixels in each layer (No include dead module)",10,0,10);
    deadPixel_layer->SetDirectory(deadMapFile);
    deadPixel_layer->SetLabelSize  ( 0.05,"x");
    deadPixel_layer->GetXaxis()->SetBinLabel(1,"Disk1A");
    deadPixel_layer->GetXaxis()->SetBinLabel(2,"Disk2A");
    deadPixel_layer->GetXaxis()->SetBinLabel(3,"Disk3A");
    deadPixel_layer->GetXaxis()->SetBinLabel(4,"Disk1C");
    deadPixel_layer->GetXaxis()->SetBinLabel(5,"Disk2C");
    deadPixel_layer->GetXaxis()->SetBinLabel(6,"Disk3C");
    //deadPixel_layer->GetXaxis()->SetBinLabel(7,"B-Layer");
    //deadPixel_layer->GetXaxis()->SetBinLabel(8,"Layer1");
    //deadPixel_layer->GetXaxis()->SetBinLabel(9,"Layer2");
    deadPixel_layer->GetXaxis()->SetBinLabel(7,"IBL");
    deadPixel_layer->GetXaxis()->SetBinLabel(8,"B-Layer");
    deadPixel_layer->GetXaxis()->SetBinLabel(9,"Layer1");
    deadPixel_layer->GetXaxis()->SetBinLabel(10,"Layer2");

    deadPixel_layer_all = new TH1D("deadPixel_layer_all","Zero hit pixels in each layer (include dead module)",10,0,10);
    deadPixel_layer_all->SetDirectory(deadMapFile);
    deadPixel_layer_all->SetLabelSize  ( 0.05,"x");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(1,"Disk1A");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(2,"Disk2A");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(3,"Disk3A");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(4,"Disk1C");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(5,"Disk2C");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(6,"Disk3C");
    //deadPixel_layer_all->GetXaxis()->SetBinLabel(7,"B-Layer");
    //deadPixel_layer_all->GetXaxis()->SetBinLabel(8,"Layer1");
    //deadPixel_layer_all->GetXaxis()->SetBinLabel(9,"Layer2");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(7,"IBL");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(8,"B-Layer");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(9,"Layer1");
    deadPixel_layer_all->GetXaxis()->SetBinLabel(10,"Layer2");


    h_deadPixel.resize( components.size() );
    for( std::vector<std::string>::const_iterator component = components.begin();
        component != components.end(); ++component){

      h_deadPixel[ component - components.begin() ] =
        new TH1D(("deadPixel" + (*component)).c_str(), ("Zero hit pixel per module " + (*component)).c_str(),
            100,-0.5,99.5);

      h_deadPixel[ component - components.begin() ]->SetDirectory(deadMapFile);
    }

  }

  // Open file to save python fragment with disabled modules
  ofstream pyFile("DisabledModulesAndFE.py");

  //------------------------------------
  // read hit maps from input file
  //
  // endcaps
  // 48 modules each disk
  // phi_index [0,327]
  // eta_index [0,143]
  //------------------------------------
  std::vector<std::string> hemispheres;
  hemispheres.push_back("A");
  hemispheres.push_back("C");

  for(std::vector<std::string>::const_iterator hemisphere = hemispheres.begin();
      hemisphere != hemispheres.end(); ++hemisphere){

    std::string hitMapsDirName = std::string("hitMaps_endcap") + (*hemisphere);
    std::string deadMapsDirName = std::string("deadMaps_endcap") + (*hemisphere);

    TDirectory* hitMapDir = 0;
    TDirectory* deadmapDir = 0;

    if( !optionOnline ){
      hitMapDir = deadMapFile->mkdir(hitMapsDirName.c_str());
      deadmapDir = deadMapFile->mkdir(deadMapsDirName.c_str());
    }

    for(int k = 1; k <= 3; k ++){

      std::ostringstream component, hitMapsPath;

      component << "Disk" << k << (*hemisphere);
      hitMapsPath << "hitMaps_endcap" << (*hemisphere) << "/Disk" << k;

      TDirectory* hitMapSubDir = 0;
      TDirectory* deadmapSubDir = 0;

      if( !optionOnline ){
        hitMapSubDir = hitMapDir->mkdir(component.str().substr(0,5).c_str());
        deadmapSubDir = deadmapDir->mkdir(component.str().substr(0,5).c_str());
      }

      for(int phi = 0; phi < 48; phi++){
        std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
                ->GetListOfKeys()->At(phi)))->GetName());

        int i = PixelConvert::HashID(PixelConvert::OnlineIDfromDCSID(name));
        //int i = PixelConvert::HashID((name));

        //hitMaps[i] =
        //  static_cast<TH2D*>((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
        //          ->GetListOfKeys()->At(phi)))->ReadObj());
        hitMaps[name] =
          static_cast<TH2D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
                  ->GetListOfKeys())->At(phi)))->ReadObj());

        if( optionOnline ){

          std::string DCSID = PixelConvert::DCSID(PixelConvert::OnlineID(i));
          deadMapsOnline[i] = new TH2C(DCSID.c_str(), ("Dead: " + DCSID).c_str(), 144, 0., 144., 320, 0., 320.);

          deadmapDir = deadMapFile->mkdir(DCSID.c_str());
          deadMapsOnline[i]->SetDirectory( deadmapDir );


        }
        else{
          //hitMaps[i]->SetName(name.c_str());
          //hitMaps[i]->SetDirectory(hitMapSubDir);
          hitMaps[name]->SetName(name.c_str());
          hitMaps[name]->SetDirectory(hitMapSubDir);

          //deadMaps[i] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
          //deadMaps[i]->SetDirectory(deadmapSubDir);
          deadMaps[name] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
          deadMaps[name]->SetDirectory(deadmapSubDir);

        }
      }
    }
  }


  //-----------------------------------------------------------
  // read hit maps from input file
  //
  // barrel
  // 22, 38, 52 slaves for layer 0, 1 and 2
  // 13 modules per stave
  // phi_index [0,319] for layer 0, [0,327] for layer 1 & 2
  // eta_index [0,143] for layer 0, 1 & 2
  //-----------------------------------------------------------

  std::vector<int> staves;
  if(isIBL) staves.push_back(14);
  staves.push_back(22);
  staves.push_back(38);
  staves.push_back(52);

  std::vector<std::string> layers;
  if(isIBL) layers.push_back("IBL");
  layers.push_back("B-layer");
  layers.push_back("Layer1");
  layers.push_back("Layer2");


  std::string hitMapsDirName("hitMaps_barrel");
  std::string deadMapsDirName("deadMaps_barrel");

  TDirectory* hitMapDir = 0;
  TDirectory* deadmapDir = 0;

  if( !optionOnline ){
    hitMapDir = deadMapFile->mkdir(hitMapsDirName.c_str());
    deadmapDir = deadMapFile->mkdir(deadMapsDirName.c_str());
  }

  for(unsigned int layer = 0; layer < staves.size(); layer++){

    std::ostringstream hitMapsPath;

    hitMapsPath << "hitMaps_barrel/" << layers[layer];

    TDirectory* hitMapSubDir = 0;
    TDirectory* deadmapSubDir = 0;

    if( !optionOnline ){
      hitMapSubDir = hitMapDir->mkdir(layers[layer].c_str());
      deadmapSubDir = deadmapDir->mkdir(layers[layer].c_str());
    }

    int nModulesPerStave = 13;
    if (isIBL && layer == 0) nModulesPerStave = 20; // --- IBL --- //
    for(int module = 0; module < staves[layer] * nModulesPerStave; module++) // loop on modules
    //for(int j = 0; j < (staves[layer] * 13); j++)
    {

      std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
              ->GetListOfKeys()->At(module)))->GetName());

      int i = PixelConvert::HashID(PixelConvert::OnlineIDfromDCSID(name));
      //int i = PixelConvert::HashID((name));

      //hitMaps[i] =
      //  static_cast<TH2D*>((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
      //          ->GetListOfKeys()->At(j)))->ReadObj());
      hitMaps[name] =
        static_cast<TH2D*>
        (
         (static_cast<TKey*>
          (
           (
            (
             static_cast<TDirectory*>
             (
              hitMapFile->Get( hitMapsPath.str().c_str() )
             )
            )->GetListOfKeys()
           )->At(module)
          )
         )->ReadObj()
        );

      if( optionOnline ){
        std::string DCSID = PixelConvert::DCSID(PixelConvert::OnlineID(i));

        deadMapsOnline[i] = new TH2C(DCSID.c_str(), ("Dead: " + DCSID).c_str(), 144, 0., 144., 320, 0., 320.);
        TDirectory* deadmapDir = deadMapFile->mkdir(DCSID.c_str());
        deadMapsOnline[i]->SetDirectory( deadmapDir );

      }
      else{
        //hitMaps[i]->SetName(name.c_str());
        //hitMaps[i]->SetDirectory(hitMapSubDir);
        hitMaps[name]->SetName(name.c_str());
        hitMaps[name]->SetDirectory(hitMapSubDir);

        //deadMaps[i] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
        //deadMaps[i]->SetDirectory(deadmapSubDir);
        if(isIBL && layer == 0) deadMaps[name] = new TH2C(name.c_str(), name.c_str(), 160, 0., 160., 336, 0., 336.);
        else deadMaps[name] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
        deadMaps[name]->SetDirectory(deadmapSubDir);

      }
    }
  }

  //std::string testarea = std::getenv("TestArea");
  //ifstream ifs;
  char* tmppath = std::getenv("DATAPATH");
  if(tmppath == NULL){
      std::cout << "FATAL: Unable to retrieve environmental DATAPATH" << std::endl;
      exit(EXIT_FAILURE);
  }
  std::string cmtpath(tmppath);
  std::vector<std::string> paths = splitter(cmtpath, ':');
  std::ifstream ifs;
  for (const auto& x : paths){
    if(is_file_exist((x + "/PixelMapping_Run2.dat").c_str())){
      if(isIBL){
        ifs.open(x + "/PixelMapping_Run2.dat");
      } else {
        ifs.open(x + "/PixelMapping_May08.dat");
      }
  //if(isIBL){
  //  ifs.open(testarea + "/InstallArea/share/PixelMapping_Run2.dat");
  //} else {
  //  ifs.open(testarea + "/InstallArea/share/PixelMapping_May08.dat");
  //}
      int tmp_barrel_ec; int tmp_layer; int tmp_module_phi; int tmp_module_eta; std::string tmp_module_name;
      std::vector<int> tmp_position;
      tmp_position.resize(4);
      //int counter = 0; // debug
      while(ifs >> tmp_barrel_ec >> tmp_layer >> tmp_module_phi >> tmp_module_eta >> tmp_module_name) {
        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_module_phi;
        tmp_position[3] = tmp_module_eta;
        pixelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
      }
      break;
    }
  }

  //// convert moduleID to phi/eta
  //std::map<std::string, int> phi2M_ECA;
  //phi2M_ECA[std::string("1")] = 0;
  //phi2M_ECA[std::string("6")] = 1;
  //phi2M_ECA[std::string("2")] = 2;
  //phi2M_ECA[std::string("5")] = 3;
  //phi2M_ECA[std::string("3")] = 4;
  //phi2M_ECA[std::string("4")] = 5;
  //std::map<std::string, int> phi2M_ECC;
  //phi2M_ECC[std::string("4")] = 0;
  //phi2M_ECC[std::string("3")] = 1;
  //phi2M_ECC[std::string("5")] = 2;
  //phi2M_ECC[std::string("2")] = 3;
  //phi2M_ECC[std::string("6")] = 4;
  //phi2M_ECC[std::string("1")] = 5;
  //std::map<std::string, int> eta2moduleID_IBL;
  //eta2moduleID_IBL[std::string("A_M4_A8_2")] = 9;
  //eta2moduleID_IBL[std::string("A_M4_A8_1")] = 8;
  //eta2moduleID_IBL[std::string("A_M4_A7_2")] = 7;
  //eta2moduleID_IBL[std::string("A_M4_A7_1")] = 6;
  //eta2moduleID_IBL[std::string("A_M3_A6")] = 5;
  //eta2moduleID_IBL[std::string("A_M3_A5")] = 4;
  //eta2moduleID_IBL[std::string("A_M2_A4")] = 3;
  //eta2moduleID_IBL[std::string("A_M2_A3")] = 2;
  //eta2moduleID_IBL[std::string("A_M1_A2")] = 1;
  //eta2moduleID_IBL[std::string("A_M1_A1")] = 0;
  //eta2moduleID_IBL[std::string("C_M1_C1")] = -1;
  //eta2moduleID_IBL[std::string("C_M1_C2")] = -2;
  //eta2moduleID_IBL[std::string("C_M2_C3")] = -3;
  //eta2moduleID_IBL[std::string("C_M2_C4")] = -4;
  //eta2moduleID_IBL[std::string("C_M3_C5")] = -5;
  //eta2moduleID_IBL[std::string("C_M3_C6")] = -6;
  //eta2moduleID_IBL[std::string("C_M4_C7_1")] = -7;
  //eta2moduleID_IBL[std::string("C_M4_C7_2")] = -8;
  //eta2moduleID_IBL[std::string("C_M4_C8_1")] = -9;
  //eta2moduleID_IBL[std::string("C_M4_C8_2")] = -10;
  ////
  //std::map<std::string, int> eta2moduleID_PixelBarrel;
  //eta2moduleID_PixelBarrel[std::string("M6A")] = 6;
  //eta2moduleID_PixelBarrel[std::string("M5A")] = 5;
  //eta2moduleID_PixelBarrel[std::string("M4A")] = 4;
  //eta2moduleID_PixelBarrel[std::string("M3A")] = 3;
  //eta2moduleID_PixelBarrel[std::string("M2A")] = 2;
  //eta2moduleID_PixelBarrel[std::string("M1A")] = 1;
  //eta2moduleID_PixelBarrel[std::string("_M0")] = 0;
  //eta2moduleID_PixelBarrel[std::string("M1C")] = -1;
  //eta2moduleID_PixelBarrel[std::string("M2C")] = -2;
  //eta2moduleID_PixelBarrel[std::string("M3C")] = -3;
  //eta2moduleID_PixelBarrel[std::string("M4C")] = -4;
  //eta2moduleID_PixelBarrel[std::string("M5C")] = -5;
  //eta2moduleID_PixelBarrel[std::string("M6C")] = -6;


  //----------------------------------
  // calculate dead maps
  //----------------------------------

  int alternativeNDisabledModules = 0;

  //for(std::map<unsigned int, TH2D*>::const_iterator module = hitMaps.begin(); module != hitMaps.end(); ++module)
  for(std::map<std::string, TH2D*>::const_iterator module = hitMaps.begin(); module != hitMaps.end(); ++module)
  {

    //unsigned int moduleID = (*module).first;
    std::string moduleID = (*module).first;

    //std::string onlineID = PixelConvert::OnlineID(module->first);
    //std::string offlineID = PixelConvert::OfflineID(module->first);
    //std::string DCSID = PixelConvert::DCSID(onlineID);

    TH2C* deadmap = 0;

    if( optionOnline ){
      //deadmap = deadMapsOnline[moduleID];
      std::cout << "!!!!!!!!!!!!!!!!!" << std::endl;
    }
    else{
      deadmap = deadMaps[moduleID];
    }

    std::vector<int> position = getPositionFromDCSID(moduleID);
    int barrel = position[0];
    int layer = position[1];
    int module_phi = position[2];
    int module_eta = position[3];
    //if (moduleID.substr(0,1) == "D") { // Endcap
    //  int B = std::stoi(moduleID.substr(5,2)); // e.g.) D3A_B02_S2_M4, phi = 17
    //  int S = std::stoi(moduleID.substr(moduleID.size() - 4, 1));
    //  if (moduleID.substr(2,1) == "A") {
    //    barrel = 2;
    //    int M = phi2M_ECA[moduleID.substr(moduleID.size() - 1, 1)];
    //    if(B == 1 && S == 1) module_phi = 42 + M;
    //    else module_phi = (B - 1) * 12 + (S - 1) * 6 + M - 6;
    //  } else if (moduleID.substr(2,1) == "C") {
    //    barrel = -2;
    //    int M = phi2M_ECC[moduleID.substr(moduleID.size() - 1, 1)];
    //    if(B == 1 && S == 1) module_phi = 42 + M;
    //    else module_phi = (B - 1) * 12 + (S - 1) * 6 + M - 6;
    //  }
    //  layer = std::stoi( moduleID.substr(1,1) ) - 1;
    //  module_eta = barrel;
    //} else if (moduleID.substr(0,1) == "L") { // Barrel
    //  barrel = 0;
    //  if (moduleID.substr(1,1) == "I") { // IBL
    //    layer = 0;
    //    module_phi = std::stoi(moduleID.substr(4,2)) - 1; // e.g.) LI_S12_A_M2_A4
    //    module_eta = eta2moduleID_IBL[moduleID.substr(7,std::string::npos)];
    //  } else { // Pixel
    //    layer = std::stoi( moduleID.substr(1,1) ) + 1; // Pixel
    //    int B = std::stoi(moduleID.substr(4,2)); // e.g.) L0_B07_S1_M1A, phi = 13, eta = 1
    //    int S = std::stoi(moduleID.substr(8,1));
    //    module_eta = eta2moduleID_PixelBarrel[moduleID.substr(moduleID.size() - 3, std::string::npos)];
    //    if (layer == 1) { // BLayer
    //      if (moduleID.substr(0,9) == "L0_B11_S2") module_phi = 0;
    //      else module_phi = (B - 1) * 2 + S;
    //    } else if (layer == 2) { // Layer-1
    //      module_phi = (B - 1) * 2 + (S - 1);
    //    } else if (layer == 3) { // Layer-2
    //      if (moduleID.substr(0,9) == "L2_B01_S1") module_phi = 51;
    //      else module_phi = (B - 1) * 2 + (S - 1) - 1;
    //    }
    //  }
    //} // end if barrel

    //int barrel = ((moduleID >> 25) & 3);
    //barrel = 2*(barrel-1);
    //int layer = ((moduleID >> 23) & 3);
    //int module_phi = ((moduleID >> 17) & 63);
    //int module_eta = ((moduleID >> 13) & 15);
    //module_eta -= 6;

    unsigned int component_index = 0;

    if( barrel == 2 ){
      component_index = layer;
    }
    else if ( barrel == -2 ){
      component_index = layer + 3;
    }
    else if ( barrel == 0 ){
      component_index = layer + 6;
    }

    double averageHitNumber=module->second->GetEntries()/nPixelsPerModule;
    if(averageHitNumber < minAveragePixelHitNumber && averageHitNumber > 0.001){
      std::cerr<<"WARNING!" << std::endl;
      std::cerr<<"Average Occ/pix= "<<averageHitNumber
        <<" in module " << module->second->GetTitle() << std::endl;
    }

    int thisModuleCut=0;
    bool isModuleActive = false;
    bool isIBL3D = ( isIBL && barrel == 0 && layer == 0 && (module_eta <= -7 || module_eta >= 6) ) ? true : false;
    //int pixel_eta_max = (barrel == 0 && layer == 0) ? 160 : 144;
    int pixel_phi_max = (barrel == 0 && layer == 0) ? 336 : 328;
    int nchip = 16;
    if (isIBL && barrel == 0 && layer == 0) {
      nchip = (isIBL3D) ? 1 : 2;
    }
    const int nrow = (isIBL && barrel == 0 && layer == 0) ? 336 : 160;
    const int ncol = (isIBL && barrel == 0 && layer == 0) ? 80 : 18;
    bool isFEActive[16];

    for (int chip = 0; chip < nchip; chip++) {
      isFEActive[chip] = false;
      int rowmin = 0, colmin = 0;
      if (chip < (nchip/2)) {
        rowmin = nrow + 8;
        colmin = (nchip/2-chip-1)*ncol;
      } else {
        colmin = (chip-nchip/2)*ncol;
      }
      int rowmax = rowmin + nrow;
      int colmax = colmin + ncol;
      if (isIBL && barrel == 0 && layer == 0) {
        rowmin = 0;
        rowmax = pixel_phi_max;
        colmin = 0;
        colmax = (isIBL3D) ? 80 : 160;
      }
      for(int pixel_eta = colmin; pixel_eta < colmax; pixel_eta++){
        for(int pixel_phi = rowmin; pixel_phi < rowmax; pixel_phi++){
          double nHits = module->second->GetBinContent( pixel_eta + 1, pixel_phi + 1 );

          //unsigned int pixelType = ModuleSpecialPixelMap::
          //  pixelType( pixel_eta % 18, (pixel_phi <= 163) ? pixel_phi : 327 - pixel_phi );

          // check pixel type
          //unsigned int type = 0;
          int pixel_eta_on_chip = (isIBL && barrel == 0 && layer == 0) ? pixel_eta % 80 : pixel_eta % 18; // column
          int pixel_phi_on_chip = (pixel_phi <= 163) ? pixel_phi : 327 - pixel_phi; // eta
          if (isIBL && barrel == 0 && layer == 0) pixel_phi_on_chip = pixel_phi;
          int pixelType = 0;

          if (isIBL && barrel == 0 && layer == 0) { // ----- IBL ----- //
            if( !isIBL3D && (pixel_eta_on_chip == 0 || pixel_eta_on_chip == 80 - 1) ){
              pixelType = 1; // long
            }
            //else if(pixel_eta_on_chip > 0 && pixel_eta_on_chip < 80 - 1) // pixel size = 50x250 um2
            else { // pixel size = 50x250 um2
              pixelType = 0; // normal
            }
          } else { // Pixel
            if(pixel_eta_on_chip > 0 && pixel_eta_on_chip < 18 - 1){ // pixel size = 50x400 um2
              pixelType = 0; // normal
              for(int kk = 0; kk < 3; kk++){
                // row 154,156,158                       = inter-ganged
                // row 153,155,157,159,  160,161,162,163 = ganged
                if(pixel_phi_on_chip == (153 + 2 * kk + 1)){
                  pixelType = 5; // inter-ganged (dealt as normal)
                  break;
                }
                if(pixel_phi_on_chip == (153 + 2 * kk) || pixel_phi_on_chip >= 159){
                  pixelType = 2; // ganged
                  break;
                }
              }
            }
            else if(pixel_eta_on_chip == 0 || pixel_eta_on_chip == 18 - 1){
              pixelType =  1; //long
              for(int kk = 0; kk < 3; kk++){
                if(pixel_phi_on_chip == (153 + 2 * kk + 1)){
                  pixelType = 6; // long inter-ganged (dealt as long)
                  break;
                }
                if(pixel_phi_on_chip == (153 + 2 * kk) || pixel_phi_on_chip >= 159){
                  pixelType = 3; // long ganged
                  break;
                }
              }
            }
            else
              pixelType =  8; //invalid pixel_phi/pixel_eta pair
          }

          if(pixelType != 0 && pixelType !=1 && pixelType !=2 &&
              pixelType != 3 && pixelType !=5 && pixelType !=6)  continue;
          //pixelType:
          //0: normal, 1: long, 2: ganged, 3: long ganged, 5: inter ganged,
          //6: long inter ganged, 8: invalid

          if(nHits < 0.5){

            if( optionOnline ){

              unsigned int column = 0, row = 0;

              if(barrel == 0){                            //barrel

                if( pixel_phi > 167 ){
                  row = 327 - pixel_phi;
                }
                else{
                  row = 319 - pixel_phi;
                }

                column = 143 - pixel_eta;
              }
              else if(barrel == -2  || barrel == 2){      //disks
                if(module_phi % 2 == 0){                  //even modules
                  row = pixel_phi;

                  if( pixel_phi > 167 ){
                    row -= 8;
                  }

                  column = 143 - pixel_eta;
                }
                else if(module_phi % 2 == 1){             //odd modules

                  if( pixel_phi > 167 ){
                    row = 327 - pixel_phi;
                  }
                  else{
                    row = 319 - pixel_phi;
                  }

                  column = 143 - pixel_eta;
                }
              }


              deadmap->Fill(column, row);

            } // end if optionOnline
            else{

              deadmap->Fill(pixel_eta, pixel_phi);

            }

            thisModuleCut++; // #dead pixels on module
          } else {
            isModuleActive=true;
            isFEActive[chip]=true;
          }

        } // end for loop over pixel_phi
      } // end for loop over pixel_eta
    } // end for loop over nchip

    if (!isModuleActive) {
      std::cout << "Dead module: "  << module->second->GetTitle() << std::endl;
      pyFile << "job.PixMapOverlayWriter.ModuleIDFragments += [ \""
        <<  module->second->GetTitle() << "\" ]" << std::endl;
    } else {
      bool hasDeadFE = false;
      for (int chip=0; chip<nchip; chip++) {
        if (!isFEActive[chip]) { hasDeadFE=true; break; }
      }
      if (hasDeadFE) {
        std::cout << "Dead FE in module: "  << module->second->GetTitle();
        pyFile << "job.PixMapOverlayWriter.Frontends += { \""
          <<  module->second->GetTitle() << "\": ";
        char sep='[';
        for (int chip=0; chip<nchip; chip++) {
          if (!isFEActive[chip]) {
            std::cout << ", " << chip;
            pyFile << sep << chip;
            sep = ',';
          }
        }
        std::cout << std::endl;
        pyFile << "] }" << std::endl;
      }
    }


    if( !optionOnline ){

      ndeadPixel[component_index]+=thisModuleCut;
      ndeadPixel_total+=thisModuleCut;

      if (isIBL && layer == 0 && barrel == 0) {
        nPixelsPerModule = (isIBL3D) ? 80 * 336 : 160 * 336;
      } else {
        nPixelsPerModule = 144 * 320;
      }
      if(thisModuleCut < nPixelsPerModule){
        ndeadPixel_noDisabled[component_index]+=thisModuleCut;
      } else {
        deadmap->Reset();

        alternativeNDisabledModules++;
        //std::cout << "Check moduli" << onlineID << std::endl;
      }

      h_deadPixel[component_index]->Fill(thisModuleCut);
      deadPlot->Fill(thisModuleCut);
    } // end if !optionOnline

    //std::cout << "Module " << onlineID << " " << offlineID << " " << DCSID << " dead pixels " << thisModuleCut << std::endl;
    //logfile << "Module " << DCSID << " dead pixels " << thisModuleCut <<std::endl;
    logfile << "Module " << (*module).first << " dead pixels " << thisModuleCut <<std::endl;
  } // end loop over all modules

  if( !optionOnline ){
    std::cout<<std::endl;
    std::cout<<"---------------------------------------------------------"<<std::endl;
    std::cout<<"Summary of the pixel with zero hit:"<<std::endl;
    std::cout<<"---------------------------------------------------------"<<std::endl;

    double nDisabledModules = 0.;
    int nFullDisabledModules = 0;
    double nPixelsOnDisabledModules = 0.;

    for( int ii = 1; ii < nModules + 1; ii++ ){
      //cout << disabledModules->GetBinContent(ii) << endl;
      nDisabledModules += disabledModules->GetBinContent(ii);
      if ( disabledModules->GetBinContent(ii) == nEvents ) {
        nFullDisabledModules++;
        //if(ii <= 280) {
        if(false) { // always nPixelsPerModule = 144*320
          if (ii % 20 <= 4 || ii % 20 > 16) {
            nPixelsPerModule = 80 * 336;
          } else {
            nPixelsPerModule = 160 * 336;
          }
        } else {
          nPixelsPerModule = 144 * 320;
        }
        nPixelsOnDisabledModules += nPixelsPerModule;
      }

    }

    nDisabledModules /= nEvents;

    // double nPixelsOnDisabledModules = nDisabledModules * nPixels / nModules;
    //double nPixelsOnDisabledModules = nFullDisabledModules * nPixelsPerModule ;
    double alternativeNPixelsNoDisabledModules = 0 ;
    double alternativeNDeadPixelTotal=0;

    for( std::vector<std::string>::const_iterator component = components.begin();
        component != components.end(); ++component) {
      alternativeNDeadPixelTotal += ndeadPixel[ component - components.begin() ];
      alternativeNPixelsNoDisabledModules += ndeadPixel_noDisabled[ component - components.begin() ];
    }


    std::cout << std::endl;
    std::cout << "Number of events in this run "
      << nEvents << std::endl;

    std::cout << "Number of disabled modules "
      << nFullDisabledModules << std::endl;

    std::cout << "Alternative disabled modules "
      << alternativeNDisabledModules << std::endl;


    std::cout << "Number of pixel from the disabled modules "
      << nPixelsOnDisabledModules << std::endl;

    std::cout << "Alternative number of pixel from the disabled modules  "
      << alternativeNDeadPixelTotal - alternativeNPixelsNoDisabledModules << std::endl;


    std::cout << "Number of pixel with zero hit (total) "
      << ndeadPixel_total << std::endl;

    std::cout << "Number of pixel with zero hit (not including disabled modules) "
      << ndeadPixel_total - nPixelsOnDisabledModules << std::endl;

    std::cout << "Alternative number of pixel with zero hit (not including disabled modules) "
      << alternativeNPixelsNoDisabledModules << std::endl;

    std::cout << std::endl;
    std::cout << "Number of pixel with zero hit in each layer: (not including disabled module)"
      << std::endl;
    std::cout << std::endl;

    for( std::vector<std::string>::const_iterator component = components.begin();
        component != components.end(); ++component){

      std::cout <<component->c_str()<<" : "
        <<ndeadPixel[ component - components.begin() ]<< " ("
        <<ndeadPixel_noDisabled[ component - components.begin() ]
        <<")"<<std::endl;

      deadPixel_layer_all->SetBinContent(component - components.begin() + 1,
          ndeadPixel[ component - components.begin() ]);
      deadPixel_layer->SetBinContent(component - components.begin() + 1,
          ndeadPixel_noDisabled[ component - components.begin() ]);
    }


  }

  hitMapFile->Close();
  delete hitMapFile;

  deadMapFile->Write();
  deadMapFile->Close();
  delete deadMapFile;


  pyFile.close();

  return 0;
}

std::string getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta){
  for(unsigned int ii = 0; ii < pixelMapping.size(); ii++) {
    if (pixelMapping[ii].second.size() != 4) {
      std::cout << "getDCSIDFromPosition: Vector size is not 4!" << std::endl;
      return std::string("Error!");
    }
    if (pixelMapping[ii].second[0] != barrel_ec) continue;
    if (pixelMapping[ii].second[1] != layer) continue;
    if (pixelMapping[ii].second[2] != module_phi) continue;
    if (pixelMapping[ii].second[3] != module_eta) continue;
    return pixelMapping[ii].first;
  }
  std::cout << "Not found!" << std::endl;
  return std::string("Error!");
}

std::vector<int> getPositionFromDCSID (std::string module_name){
  for(unsigned int ii = 0; ii < pixelMapping.size(); ii++) {
    if (pixelMapping[ii].first == module_name)
    return pixelMapping[ii].second;
  }
  std::cout << "Not found!" << std::endl;
  return pixelMapping[0].second;
}
