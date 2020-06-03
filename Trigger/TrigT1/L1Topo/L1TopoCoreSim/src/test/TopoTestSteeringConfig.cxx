/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// test the loading of the topo menu xml and configuration of the topo steering

#include <iostream>

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoConfig/L1TopoXMLParser.h"

using namespace std;

int run(int argc, const char * argv[]) {

   if(argc<2) {
      cout << "Please specify topo menu input XML file:\n" << argv[0] << " -v <menu.xml>" << endl;
      return 1;
   }

   bool verbose = (string(argv[1])=="-v");


   TXC::L1TopoXMLParser parser;
   parser.readConfiguration(argv[argc-1]);
   try {
      parser.parseConfiguration();
   }
   catch(std::exception & e) {
      cout << "TopoTestSteeringConfig: Caught exception from the topo menu parser, no topo menu will be available! Exception message: " << e.what() << endl;
      return 1;
   }

   TXC::L1TopoMenu menu = parser.takeMenu();  // since parser goes out of scope, we take the menu

   if (verbose)
      menu.print();

   TCS::TopoSteering steering;
   try {
      steering.setupFromConfiguration( menu );
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

