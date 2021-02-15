/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// test the loading of the topo menu xml and configuration of the topo steering

#include <iostream>

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoConfig/L1TopoXMLParser.h"

#include "TrigConfIO/JsonFileLoader.h"
#include "TrigConfData/L1Menu.h"

using namespace std;

int run(int argc, const char * argv[]) {

   if(argc<2) {
      cout << "Please specify topo menu input JSON file:\n" << argv[0] << " -v <menu.json>" << endl;
      return 1;
   }

   bool verbose = (string(argv[1])=="-v");

   TrigConf::L1Menu l1menu;
   TrigConf::JsonFileLoader fileLoader;

   try {
      fileLoader.loadFile( argv[0], l1menu);
   }
   catch(std::exception & e) {
      cout << "TopoTestSteeringConfig: Caught exception from the topo menu loader, no topo menu will be available! Exception message: " << e.what() << endl;
      return 1;
   }

   if (verbose)
      l1menu.printMenu(true);

   TCS::TopoSteering steering;
   try {
      steering.setupFromConfiguration(l1menu);
   } catch(exception & e) {
      cerr << "TopoTestSteeringConfig: Caught exception when configuring topo steering from menu: " << endl << e.what() << endl;
      return 1;
   }

   steering.printConfiguration(cout);

   try {
      steering.initializeAlgorithms();
   } catch(exception & e) {
      cerr << "TopoTestSteeringConfig: Caught exception when initializing algorithms " << endl << e.what() << endl;
      return 1;
   }

   steering.reset();

   return 0;
}




int main(int argc, const char * argv[]) {
   try {
      return run(argc, argv);
   }
   catch(std::exception & e) {
      cerr << "Caught exception: " << e.what() << endl;
   }
   return 1;
}

