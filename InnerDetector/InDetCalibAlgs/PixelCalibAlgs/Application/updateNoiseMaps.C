/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<vector>
#include<cmath>
#include<map>
#include<sstream>
#include<string>
#include<fstream>
#include<iostream>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TKey.h"
#include "Riostream.h"
#include "TDirectory.h"
#include "TString.h"
#include "Math/ProbFuncMathCore.h"

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
  std::ifstream infile(fileName, std::ifstream::in);
  return infile.good();
}

//double ComputeMuChip(TH2D* modhisto,int chip)
double ComputeMuChip(TH2D* modhisto, int chip)
{
  static const int nchip = 16;
  static const int ncol  = 18;
  static const int nrow  = 160; // not encoded in ModuleSpecialPixelMap
  static const int nlongcol=2;
  static const int ngangedrow=4;
  static const int totalpixels = nchip*ncol*nrow;
  static const double fr10 = (ncol-nlongcol)*(nrow-ngangedrow)/double(totalpixels); // fraction of normal pixels
  static const double fr15 = nlongcol*(nrow-ngangedrow)/double(totalpixels);        // fraction of long pixels
  static const double fr20 = (ncol-nlongcol)*ngangedrow/double(totalpixels);        // fraction of ganged pixels
  static const double fr30 = nlongcol*ngangedrow/double(totalpixels);               // fraction of long-ganged pixels
  double totalhits=0.;
  int rowmin=0, colmin=0;
  if (chip<(nchip/2)) {
    rowmin=nrow+8;
    colmin = (nchip/2-chip-1)*ncol;
  } else {
    colmin = (chip-nchip/2)*ncol;
  }
  double rowmax=rowmin+nrow;
  double colmax=colmin+ncol;
  for (int col=colmin; col<colmax; col++) {
    for (int row=rowmin; row<rowmax; row++) {
      totalhits+=(modhisto->GetBinContent(col+1,row+1));
    }
  }
  double mu = totalhits/double(totalpixels);
  return mu/(fr10+1.5*fr15+2.*fr20+3.0*fr30);
}

double ComputeMuChipIBL(TH2D* modhisto, int chip)
{
  static const int nchip = 2;   // chip is 1 for 3D but it is taken in account in ComputeMuAve
  static const int ncol  = 80;
  static const int nrow  = 336; // not encoded in ModuleSpecialPixelMap
  static const int nlongcol=2;
  //static const int ngangedrow=0;
  static const int totalpixels = nchip*ncol*nrow;
  static const double fr10 = (ncol-nlongcol)*nrow/double(totalpixels); // fraction of normal pixels
  static const double fr15 = nlongcol*nrow/double(totalpixels);   // fraction of long pixels
  //static const double fr20 = (ncol-nlongcol)*ngangedrow/double(totalpixels);        // fraction of ganged pixels
  //static const double fr30 = nlongcol*ngangedrow/double(totalpixels);               // fraction of long-ganged pixels
  double totalhits=0.;
  int colmin = 0;
  if (chip == 1) colmin = 80;
  int colmax = colmin + ncol;
  for (int col = colmin; col < colmax; col++) {
    for (int row = 0; row < nrow; row++) {
      totalhits+=(modhisto->GetBinContent(col+1,row+1));
    }
  }
  double mu = totalhits/double(totalpixels);
  return mu/(fr10+1.5*fr15);
}

//double ComputeMuAve(TH2D* modhisto)
double ComputeMuAve(TH2D* modhisto, int barrel_ec, int layer)
{
  double muave = 0.;
  int ngoodchip=0;
  int nchip = 16;
  if (barrel_ec == 0 && layer == 0) nchip = 2;
  for (int i = 0; i < nchip; i++) {
    double muchip = 0.;
    if (barrel_ec == 0 && layer == 0) muchip = ComputeMuChipIBL(modhisto, i);
    else muchip = ComputeMuChip(modhisto, i);
    //double muchip = ComputeMuChip(modhisto,i);
    if ( muchip > 0. ) {
      muave += muchip;
      ngoodchip++;
    }
  }
  if ( ngoodchip>0 ) muave /= ngoodchip;
  return muave;
}

int ComputePoisson(double probcut, double mu) {
  // Given a Poisson distrubution with average mu,
  // compute the smallest value of n such that
  // Probability(x>=n)<probcut;

  //double term = exp(-mu);
  if (mu < 1) mu = 1;
  int nhits = static_cast<int>(mu);
  double prob = ROOT::Math::poisson_cdf_c(nhits, mu); // set starting point
  while (prob > probcut) {
    nhits++;
    prob = ROOT::Math::poisson_cdf_c(nhits, mu);
  }
  return nhits;
}


int main(int argc, char* argv[]){

  std::string option;

  if( argc >= 2 ){
    option = argv[1];
  }

  if( !(argc == 3 ||
        (argc == 4 && option == "--online")) ){

    std::cout << "Usage: updateNoiseMaps [--online] <inputfile> <outputfile>" << std::endl;
    return 0;
  }

  bool isIBL = true; /////

  bool optionOnline = false;

  if( option == "--online" ){
    optionOnline = true;
  }

  TFile* hitMapFile = new TFile(argv[argc - 2], "READ");

  if( !hitMapFile || !hitMapFile->IsOpen() ){
    std::cerr << "Error: Unable to open input file." << std::endl;
    return 1;
  }

  TFile* noiseMapFile = new TFile(argv[argc - 1], "RECREATE");

  if( !noiseMapFile || !noiseMapFile->IsOpen() ){
    std::cerr << "Error: Unable to open output file." << std::endl;
    return 1;
  }

  std::vector<std::string> components;
  components.push_back("Disk1A");
  components.push_back("Disk2A");
  components.push_back("Disk3A");
  components.push_back("Disk1C");
  components.push_back("Disk2C");
  components.push_back("Disk3C");
  if(isIBL) components.push_back("IBL");     // kazuki
  components.push_back("B-layer");
  components.push_back("Layer1");
  components.push_back("Layer2");
  if(isIBL) components.push_back("DBMA");     // kazuki
  if(isIBL) components.push_back("DBMC");     // kazuki

  std::vector<std::string> types;
  types.push_back("Normal");
  types.push_back("Long");
  types.push_back("Ganged");
  types.push_back("LongGanged");

  //std::map<unsigned int, TH2D*> hitMaps;
  //std::map<unsigned int, TH2C*> noiseMaps;
  //std::map<unsigned int, TH1D*> lumiMaps;
  //std::map<unsigned int, TH2C*> noiseMapsOnline;
  std::map<std::string, TH2D*> hitMaps;
  std::map<std::string, TH2C*> noiseMaps;
  std::map<std::string, TH1D*> lumiMaps;
  std::map<std::string, TH2C*> noiseMapsOnline;

  TH1D* nEventsHistogram = 0;

  hitMapFile->GetObject("NEvents", nEventsHistogram);

  double nEvents = nEventsHistogram->GetBinContent(1);

  TH2F* disablePlotBI = 0; // kazuki
  TH2F* disablePlotB0 = 0;
  TH2F* disablePlotB1 = 0;
  TH2F* disablePlotB2 = 0;
  TH2F* disablePlotEC = 0;
  TH2F* disablePlotDBM = 0; // kazuki

  TH1D* globalOccupancy = 0;

  TH1D* disabledModules = 0;

  TH1D* maskedPlot = 0;

  std::vector< TH1D*> h_occupancy;

  double minOccupancy = 0;

  double nHitsBeforeMask = 0.;
  double nHitsAfterMask = 0.;

  if(isIBL) hitMapFile->GetObject("disablePlotBI", disablePlotBI);  // kazuki
  hitMapFile->GetObject("disablePlotB0", disablePlotB0);
  hitMapFile->GetObject("disablePlotB1", disablePlotB1);
  hitMapFile->GetObject("disablePlotB2", disablePlotB2);
  hitMapFile->GetObject("disablePlotEC", disablePlotEC);
  if(isIBL) hitMapFile->GetObject("disablePlotDBM", disablePlotDBM);  // kazuki

  hitMapFile->GetObject("DisabledModules", disabledModules);

  if( !optionOnline ){

    nEventsHistogram->SetDirectory(noiseMapFile);

    if(isIBL) disablePlotBI->SetDirectory(noiseMapFile); // kazuki
    disablePlotB0->SetDirectory(noiseMapFile);
    disablePlotB1->SetDirectory(noiseMapFile);
    disablePlotB2->SetDirectory(noiseMapFile);
    disablePlotEC->SetDirectory(noiseMapFile);
    if(isIBL) disablePlotDBM->SetDirectory(noiseMapFile); // kazuki

    disabledModules->SetDirectory(noiseMapFile);

    int minLogOccupancy = 8;
    minOccupancy = std::pow(10.,-minLogOccupancy);

    globalOccupancy = new TH1D("occupancy","Pixel occupancy",minLogOccupancy*10,-minLogOccupancy,0.);
    globalOccupancy->SetDirectory(noiseMapFile);

    h_occupancy.resize( components.size() + types.size() );

    for( std::vector<std::string>::const_iterator component = components.begin();
        component != components.end(); ++component){

      h_occupancy[ component - components.begin() ] =
        new TH1D(("occupancy" + (*component)).c_str(), ("Pixel occupancy " + (*component)).c_str(),
            minLogOccupancy*10,-minLogOccupancy,0.);

      h_occupancy[ component - components.begin() ]->SetDirectory(noiseMapFile);
    }

    for( std::vector<std::string>::const_iterator type = types.begin();
        type != types.end(); ++type){

      h_occupancy[ type - types.begin() + components.size() ] =
        new TH1D(("occupancy" + (*type)).c_str(), ("Pixel occupancy " + (*type)).c_str(),
            minLogOccupancy*10,-minLogOccupancy,0.);

      h_occupancy[ type - types.begin() + components.size() ]->SetDirectory(noiseMapFile);
    }

    maskedPlot = new TH1D("maskedPlot","Disabled pixels per module",50,0.5,50.5);
    maskedPlot->SetDirectory(noiseMapFile);
  }

  // read hit maps from input file

  std::cout << "reading hit maps of endcaps from input file" << std::endl;

  // endcaps

  std::vector<std::string> hemispheres;
  hemispheres.push_back("A");
  hemispheres.push_back("C");

  for(std::vector<std::string>::const_iterator hemisphere = hemispheres.begin();
      hemisphere != hemispheres.end(); ++hemisphere){

    std::string hitMapsDirName = std::string("hitMaps_endcap") + (*hemisphere);
    std::string noiseMapsDirName = std::string("noiseMaps_endcap") + (*hemisphere);
    std::string lumiMapsDirName = std::string("LBdep_endcap") + (*hemisphere);

    TDirectory* hitMapDir = 0;
    TDirectory* noiseMapDir = 0;
    TDirectory* lumiMapDir = 0;

    if( !optionOnline ){
      hitMapDir = noiseMapFile->mkdir(hitMapsDirName.c_str());
      noiseMapDir = noiseMapFile->mkdir(noiseMapsDirName.c_str());
      lumiMapDir = noiseMapFile->mkdir(lumiMapsDirName.c_str());
    }

    for(int num = 1; num <= 3; num++){

      std::ostringstream component, hitMapsPath, lumiMapsPath;

      component << "Disk" << num << (*hemisphere);
      hitMapsPath << "hitMaps_endcap" << (*hemisphere) << "/Disk" << num;
      lumiMapsPath << "LBdep_endcap" << (*hemisphere) << "/Disk" << num;

      TDirectory* hitMapSubDir = 0;
      TDirectory* noiseMapSubDir = 0;
      TDirectory* lumiMapSubDir = 0;

      if( !optionOnline ){
        hitMapSubDir = hitMapDir->mkdir(component.str().substr(0,5).c_str());
        noiseMapSubDir = noiseMapDir->mkdir(component.str().substr(0,5).c_str());
        lumiMapSubDir = lumiMapDir->mkdir(component.str().substr(0,5).c_str());
      }

      for(int phi = 0; phi < 48; phi++){
        std::string name((static_cast<TKey*>((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
                ->GetListOfKeys()->At(phi)))->GetName());

        //int i = PixelConvert::HashID(PixelConvert::OnlineIDfromDCSID(name));
        //hitMaps[i] =
        hitMaps[name] =
          static_cast<TH2D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get(hitMapsPath.str().c_str())))
                  ->GetListOfKeys())->At(phi)))->ReadObj());
        //hitMaps[name]->SetName(("hMap_" + name).c_str());
        //lumiMaps[i] =
        lumiMaps[name] =
          static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get(lumiMapsPath.str().c_str())))
                  ->GetListOfKeys())->At(phi)))->ReadObj());
        //lumiMaps[name]->SetName(("lMap_" + name).c_str());

        if( optionOnline ){

          //std::string DCSID = PixelConvert::DCSID(PixelConvert::OnlineID(i));

          //noiseMapsOnline[i] = new TH2C(DCSID.c_str(), ("occupancy: " + DCSID).c_str(), 144, 0., 144., 320, 0., 320.);
          noiseMapsOnline[name] = new TH2C(name.c_str(), ("occupancy: " + name).c_str(), 144, 0., 144., 320, 0., 320.);
          TDirectory* noiseMapDir = noiseMapFile->mkdir(name.c_str());
          //noiseMapsOnline[i]->SetDirectory( noiseMapDir );
          noiseMapsOnline[name]->SetDirectory( noiseMapDir );
        }
        else{
          //hitMaps[i]->SetName(name.c_str());
          //hitMaps[i]->SetDirectory(hitMapSubDir);
          //lumiMaps[i]->SetName(name.c_str());
          //lumiMaps[i]->SetDirectory(lumiMapSubDir);
          lumiMaps[name]->SetDirectory(lumiMapSubDir);
          //hitMaps[name]->SetName(name.c_str());
          hitMaps[name]->SetDirectory(hitMapSubDir);

          //noiseMaps[i] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
          //noiseMaps[i]->SetDirectory(noiseMapSubDir);
          noiseMaps[name] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
          noiseMaps[name]->SetDirectory(noiseMapSubDir);
        }
      } // for loop on phi
    } // for loop on disk num
  } // for loop on hemispheres


  // barrel

  std::vector<int> staves;
  if(isIBL) staves.push_back(14); // kazuki
  staves.push_back(22);
  staves.push_back(38);
  staves.push_back(52);

  std::vector<std::string> layers;
  if(isIBL) layers.push_back("IBL"); // kazuki
  layers.push_back("B-layer");
  layers.push_back("Layer1");
  layers.push_back("Layer2");


  std::string hitMapsDirName("hitMaps_barrel");
  std::string noiseMapsDirName("noiseMaps_barrel");
  std::string lumiMapsDirName("LBdep_barrel");

  TDirectory* hitMapDir = 0;
  TDirectory* noiseMapDir = 0;
  TDirectory* lumiMapDir = 0;

  if( !optionOnline ){
    hitMapDir = noiseMapFile->mkdir(hitMapsDirName.c_str());
    noiseMapDir = noiseMapFile->mkdir(noiseMapsDirName.c_str());
    lumiMapDir = noiseMapFile->mkdir(lumiMapsDirName.c_str());
  }

  std::cout << "reading hit maps of barrels from input file" << std::endl;

  for(unsigned int layer = 0; layer < layers.size(); layer++)
  //for(unsigned int k = 0; k < staves.size(); k++)
  {

    std::ostringstream hitMapsPath;

    hitMapsPath << "hitMaps_barrel/" << layers[layer];
    //std::cout << "DEBUG: " << "hitMapsPath = " << hitMapsPath.str() << std::endl;

    std::ostringstream lumiMapsPath;

    lumiMapsPath << "LBdep_barrel/" << layers[layer];
    //std::cout << "DEBUG: " << "lumiMapsPath = " << lumiMapsPath.str() << std::endl;

    TDirectory* hitMapSubDir = 0;
    TDirectory* noiseMapSubDir = 0;
    TDirectory* lumiMapSubDir = 0;

    if( !optionOnline ){
      hitMapSubDir = hitMapDir->mkdir(layers[layer].c_str());
      noiseMapSubDir = noiseMapDir->mkdir(layers[layer].c_str());
      lumiMapSubDir = lumiMapDir->mkdir(layers[layer].c_str());
    }

    int nModulesPerStave = 13;
    if (isIBL && layer == 0) nModulesPerStave = 20; // --- IBL --- //
    for(int module = 0; module < staves[layer] * nModulesPerStave; module++) // loop on modules
    //for(int j = 0; j < (staves[layer] * 13); j++)
    {

      std::string name(
          (static_cast<TKey*> ( (
                                 static_cast<TDirectory*>
             ( hitMapFile->Get( hitMapsPath.str().c_str() ) )
             )->GetListOfKeys()->At(module) )
          )->GetName());

      //std::cout << "DEBUG: " << "module = " << module << ", name = " << name << std::endl;
      //int i = PixelConvert::HashID(PixelConvert::OnlineIDfromDCSID(name));

      //std::cout << "DEBUG: " << "hitMapsPath = " << hitMapsPath.str() << std::endl;
      //hitMaps[i] =
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
      //hitMaps[name]->SetName((std::string("hMap_") + name).c_str());
      //std::cout << "DEBUG: " << "hitMap of " << name << "was successfully read" << std::endl;
      lumiMaps[name] =
        static_cast<TH1D*>((static_cast<TKey*>(((static_cast<TDirectory*>(hitMapFile->Get(lumiMapsPath.str().c_str())))
                ->GetListOfKeys())->At(module)))->ReadObj());
      //lumiMaps[name]->SetName((std::string("lMap_") + name).c_str());
      //std::cout << "DEBUG: " << "lumiMap of " << name << "was successfully read" << std::endl;
      if( optionOnline ){
        //std::string DCSID = PixelConvert::DCSID(PixelConvert::OnlineID(i));

        //noiseMapsOnline[i] = new TH2C(DCSID.c_str(), ("occupancy: " + DCSID).c_str(), 144, 0., 144., 320, 0., 320.);
        if(isIBL && layer == 0) noiseMapsOnline[name] = new TH2C(name.c_str(), ("occupancy: " + name).c_str(), 160, 0., 160., 336, 0., 336.);
        else noiseMapsOnline[name] = new TH2C(name.c_str(), ("occupancy: " + name).c_str(), 144, 0., 144., 320, 0., 320.);
        TDirectory* noiseMapDir = noiseMapFile->mkdir(name.c_str());
        noiseMapsOnline[name]->SetDirectory( noiseMapDir );

      }
      else{
        //hitMaps[i]->SetName(name.c_str());
        //hitMaps[i]->SetDirectory(hitMapSubDir);
        hitMaps[name]->SetDirectory(hitMapSubDir);
        //lumiMaps[i]->SetName(name.c_str());
        //lumiMaps[i]->SetDirectory(lumiMapSubDir);
        lumiMaps[name]->SetDirectory(lumiMapSubDir);

        //noiseMaps[i] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
        //noiseMaps[i]->SetDirectory(noiseMapSubDir);
        //std::cout << "DEBUG: " << "initializing noiseMap of " << name << std::endl;
        if(isIBL && layer == 0) noiseMaps[name] = new TH2C(name.c_str(), name.c_str(), 160, 0., 160., 336, 0., 336.);
        else noiseMaps[name] = new TH2C(name.c_str(), name.c_str(), 144, 0., 144., 328, 0., 328.);
        noiseMaps[name]->SetDirectory(noiseMapSubDir);
        //std::cout << "DEBUG: " << "noiseMap of " << name << "initialized" << std::endl;
      }
    }
  }


  // print cut configuration

  std::map<std::string, double> cuts;

  double gangedPixelMultiplier = 2.;
  double longPixelMultiplier   = 1.5;
  double longgangedPixelMultiplier = 3.;
  std::string text;

  std::string occupancyPerBC( "true" );
  bool optionOccupancyPerBC = false;

  unsigned int nBCReadout = 1;

  double occucut = 1.E-7;
  //double occucut = 1.E-4;
  //std::cout << std::endl
  //  << "Using probability for a normal pixel to be flagged as noisy: "
  //  << std::endl;

  cuts["Disk1A"] = occucut;
  cuts["Disk2A"] = occucut;
  cuts["Disk3A"] = occucut;
  cuts["Disk1C"] = occucut;
  cuts["Disk2C"] = occucut;
  cuts["Disk3C"] = occucut;
  if(isIBL) cuts["IBL"]= occucut; // IBL
  cuts["B-layer"]= occucut;
  cuts["Layer1"] = occucut;
  cuts["Layer2"] = occucut;


  std::cout << "Disk1A " << cuts["Disk1A"] << std::endl;
  std::cout << "Disk2A " << cuts["Disk2A"] << std::endl;
  std::cout << "Disk3A " << cuts["Disk3A"] << std::endl << std::endl;

  std::cout << "Disk1C " << cuts["Disk1C"] << std::endl;
  std::cout << "Disk2C " << cuts["Disk2C"] << std::endl;
  std::cout << "Disk3C " << cuts["Disk3C"] << std::endl << std::endl;

  std::cout << "IBL " << cuts["IBL"] << std::endl;
  std::cout << "B-layer " << cuts["B-layer"] << std::endl;
  std::cout << "Layer1 " << cuts["Layer1"] << std::endl;
  std::cout << "Layer2 " << cuts["Layer2"] << std::endl << std::endl;

  std::cout << "Multiplier for ganged pixels " << gangedPixelMultiplier << std::endl;
  std::cout << "Multiplier for long pixels " << longPixelMultiplier << std::endl << std::endl;
  std::cout << "Multiplier for long-ganged pixels " << longgangedPixelMultiplier << std::endl << std::endl;

  if( occupancyPerBC == "true" || occupancyPerBC == "True" || occupancyPerBC == "TRUE" ){
    optionOccupancyPerBC = true;
    std::cout << "Occupancy calculated per BC with " << nBCReadout << " BC readout"
      << std::endl << std::endl;
  }
  else{
    std::cout << "Occupancy calculated per event" << std::endl << std::endl;
  }

  //std::string testarea = std::getenv("TestArea");
  char* tmppath = std::getenv("DATAPATH");
  if(tmppath == nullptr){
      std::cout << "FATAL: Unable to retrieve environmental DATAPATH" << std::endl;
      exit(EXIT_FAILURE);    
  }
  std::string cmtpath(tmppath);
  std::vector<std::string> paths = splitter(cmtpath, ':');
  std::ifstream ifs;
  for (const auto& x : paths){
    if(is_file_exist((x + "/PixelMapping_Run2.dat").c_str())){
      if(isIBL){
      //  ifs.open(testarea + "/InstallArea/share/PixelMapping_Run2.dat");
      // else
      //  ifs.open(testarea + "/InstallArea/share/PixelMapping_May08.dat");
        ifs.open(x + "/PixelMapping_Run2.dat");
      } else {
        ifs.open(x + "/PixelMapping_May08.dat");
      }
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

  //------------------------------------
  // calculate noise maps
  //-----------------------------------

  //for(std::map<unsigned int, TH2D*>::const_iterator module = hitMaps.begin(); module != hitMaps.end(); ++module)
  for(std::map<std::string, TH2D*>::const_iterator module = hitMaps.begin(); module != hitMaps.end(); ++module)
  {

    std::string moduleID = (*module).first;
    //unsigned int moduleID = (*module).first;

    TH2C* noiseMap = 0;

    if( optionOnline ){
      noiseMap = noiseMapsOnline[moduleID];
    }
    else{
      noiseMap = noiseMaps[moduleID];
    }

    std::vector<int> position = getPositionFromDCSID(moduleID);
    int barrel = position[0];
    int layer = position[1];
    int module_phi = position[2];
    int module_eta = position[3];
    //int barrel = -999;
    //int layer = -999;
    //int module_phi = -999;
    //int module_eta = -999;
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

    /**
    int barrel = ((moduleID >> 25) & 3);
    barrel = 2*(barrel-1);
    int layer = ((moduleID >> 23) & 3);
    int module_phi = ((moduleID >> 17) & 63);
    int module_eta = ((moduleID >> 13) & 15);
    module_eta -= 6;
    */

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

    TH2F* disablePlot = 0;

    if( !optionOnline ){

      if( barrel !=0 ) disablePlot = disablePlotEC;
      else{
        /**
        if(layer == 0) disablePlot = disablePlotB0;
        else if(layer == 1) disablePlot = disablePlotB1;
        else if(layer == 2) disablePlot = disablePlotB2;
        */
        if(layer == 0) disablePlot = disablePlotBI;
        else if(layer == 1) disablePlot = disablePlotB0;
        else if(layer == 2) disablePlot = disablePlotB1;
        else if(layer == 3) disablePlot = disablePlotB2;
      }
    }

    int thisModuleCut=0;

    double nHitsAfterMaskThisModule = 0.;

    double cut = cuts[components[ component_index ] ];
    double thisPixelCut = 0.;

    double muave = ComputeMuAve(module->second, barrel, layer);
    double normalpixelcut = ComputePoisson(cut,muave);
    //double normalpixelcut = cut;
    double longpixelcut = ComputePoisson(cut,muave*longPixelMultiplier);
    //double longpixelcut = cut*longPixelMultiplier;
    double gangedpixelcut = ComputePoisson(cut,muave*gangedPixelMultiplier);
    double longgangedpixelcut = ComputePoisson(cut,muave*longgangedPixelMultiplier);
    //double gangedpixelcut = cut*gangedPixelMultiplier;
    //double longgangedpixelcut = cut*longgangedPixelMultiplier;
    bool isIBL3D = ( isIBL && barrel == 0 && layer == 0 && (module_eta <= -7 || module_eta >= 6) ) ? true : false;
    // std::cout << module->second->GetTitle() << "  mu=" << muave << " cut at: " << normalpixelcut << std::endl;
    int pixel_eta_max = (barrel == 0 && layer == 0) ? 160 : 144;
    int pixel_phi_max = (barrel == 0 && layer == 0) ? 336 : 328;
    //for(int pixel_eta = 0; pixel_eta < 144; pixel_eta++)
    for(int pixel_eta = 0; pixel_eta < pixel_eta_max; pixel_eta++)
    {
      //for(int pixel_phi = 0; pixel_phi < 328; pixel_phi++)
      for(int pixel_phi = 0; pixel_phi < pixel_phi_max; pixel_phi++)
      {

        double nHits = module->second->GetBinContent( pixel_eta + 1, pixel_phi + 1 );


        unsigned int type = 0;
        //unsigned int pixelType = ModuleSpecialPixelMap::
        //  pixelType( pixel_eta % 18, (pixel_phi <= 163) ? pixel_phi : 327 - pixel_phi );
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

        switch(pixelType) {
          case 0:
          case 5:
            type = 0; // normal pixel
            thisPixelCut = normalpixelcut;
            break;
          case 1:
          case 6:
            type = 1; // long pixel
            thisPixelCut = longpixelcut;
            break;
          case 2:
            type = 2; // ganged pixel
            thisPixelCut = gangedpixelcut;
            break;
          case 3:
            type = 3; // long-ganged pixel
            thisPixelCut = longgangedpixelcut;
            break;
          case 8:
          default:
            type = 4; // Invalid
            break;
        }

        //std::cout << "Debug: thisPixelCut = " << thisPixelCut << std::endl;

        if( type != 4 ){ // valid

          double occupancy = 0.;

          if( optionOccupancyPerBC ){
            occupancy = nHits / nEvents / nBCReadout;
          }
          else{
            occupancy = nHits / nEvents;
          }

          if( nHits >= thisPixelCut && occupancy > 1e-3)
          //if( nHits >= thisPixelCut )
          //if( occupancy >= thisPixelCut ) /////////////////////////////
          {

            if( optionOnline ) {

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
              noiseMap->Fill(column, row);
            } // end if optionOnline
            else {
              noiseMap->Fill(pixel_eta, pixel_phi);
            }

            thisModuleCut++;
          } // end if high ocupancy

          if( !optionOnline ){
            if ( occupancy < minOccupancy ) occupancy = minOccupancy;

            double logOccupancy = std::log10(occupancy);

            globalOccupancy->Fill( logOccupancy );
            h_occupancy[ component_index ]->Fill( logOccupancy );
            h_occupancy[ type + components.size() ]->Fill( logOccupancy );
          }
        }

        if( !optionOnline ) {
          if( noiseMap->GetBinContent(pixel_eta + 1, pixel_phi + 1) == 0 ){

            nHitsAfterMaskThisModule += nHits;
          }
        }
      } // end loop on pixel_phi
    } // end loop on pixel_eta

    nHitsBeforeMask += module->second->GetEntries();
    nHitsAfterMask += nHitsAfterMaskThisModule;

    if( !optionOnline ){

      if(thisModuleCut > 0.) maskedPlot->Fill(thisModuleCut);

      if( barrel == 0 ){
        if (layer == 0) { // IBL
          if( disablePlot->GetBinContent(module_eta+11, module_phi+1) < 0. )
          {
            disablePlot->SetBinContent(module_eta+11, module_phi+1, -1.);
          }
          else
          {
            disablePlot->SetBinContent(module_eta+11, module_phi+1, thisModuleCut);
          }
        } else if (layer > 0) { // Pixel
          if( disablePlot->GetBinContent(module_eta+7, module_phi+1) < 0. )
            //if( disablePlot->GetBinContent(module_eta, module_phi) < 0. )
          {
            disablePlot->SetBinContent(module_eta+7, module_phi+1, -1.);
            //disablePlot->SetBinContent(module_eta, module_phi, -1.);
          }
          else
          {
            disablePlot->SetBinContent(module_eta+7, module_phi+1, thisModuleCut);
            //disablePlot->SetBinContent(module_eta, module_phi, thisModuleCut);
          }
        }
      } // end if barrel
      //else if( barrel > 0 )
      else if (barrel == 2)
      {
        if( disablePlot->GetBinContent(layer+5, module_phi+1) < 0. )
        //if( disablePlot->GetBinContent(layer, module_phi) < 0. )
        {
          disablePlot->SetBinContent(layer+5, module_phi+1, -1.);
          //disablePlot->SetBinContent(layer, module_phi, -1.);
        }
        else
        {
          disablePlot->SetBinContent(layer+5, module_phi+1, thisModuleCut);
          //disablePlot->SetBinContent(layer, module_phi, thisModuleCut);
        }
      } // end if eca
      else if (barrel == -2)
      {
        if( disablePlot->GetBinContent(3-layer, module_phi+1) < 0. )
        //if( disablePlot->GetBinContent(-layer, module_phi) < 0. )
        {
          disablePlot->SetBinContent(3-layer, module_phi+1, -1.);
          //disablePlot->SetBinContent(-layer, module_phi, -1.);
        }
        else
        {
          disablePlot->SetBinContent(3-layer, module_phi+1, thisModuleCut);
          //disablePlot->SetBinContent(-layer, module_phi, thisModuleCut);
        }
      } // end if ecc
    }
  }

  if( !optionOnline ){
    //double nPixels = 80363520.;
    double nPixels = 80363520.;
    double nModules = 1744.;
    if (isIBL) {
      nPixels = 2880. * 16. * 1744. // Pixel
        + 26880. * 112. + 53760. * 168.  // IBL 3D + Planar
        + 26880. * 24; // DBM
      nModules = 2048.;
    }

    double nDisabledModules = 0.;

    for( int i = 1; i < nModules + 1; i++ ){
      nDisabledModules += disabledModules->GetBinContent(i);
    }

    nDisabledModules /= nEvents;

    double nPixelsOnDisabledModules = nDisabledModules * nPixels / nModules;

    std::cout << std::endl;

    std::cout << "Number of Events is " << nEvents << std::endl;
    std::cout << "Taking into account an average of "
      << nDisabledModules << " disabled modules." << std::endl;
    std::cout << "Occupancy before offline noise mask "
      << nHitsBeforeMask / (nPixels - nPixelsOnDisabledModules) /
      (nEvents * nBCReadout) << " +- "
      << std::sqrt(nHitsBeforeMask) / (nPixels - nPixelsOnDisabledModules) /
      (nEvents * nBCReadout) << std::endl;
    std::cout << "Occupancy after offline noise mask "
      << nHitsAfterMask / (nPixels - nPixelsOnDisabledModules) /
      (nEvents * nBCReadout) << " +- "
      << std::sqrt(nHitsAfterMask) / (nPixels - nPixelsOnDisabledModules) /
      (nEvents * nBCReadout) << std::endl << std::endl;
  }

  hitMapFile->Close();
  delete hitMapFile;

  noiseMapFile->Write();
  noiseMapFile->Close();
  delete noiseMapFile;

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
