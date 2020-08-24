// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

//
//  main.cpp
//  TestTopo
//
//  Created by Joerg Stelzer on 11/10/12.
//

#include <iostream>

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoCoreSim/TopoASCIIReader.h"
#include "L1TopoCoreSim/TopoInputEvent.h"
#include "TrigConfL1TopoData/L1TopoConfigReader.h"

using namespace std;

int run(int argc/*argc*/, const char * argv[]/* * argv[]*/) {

  // instantiate XML reader class
  L1TopoConfigReader xmlReader;

  // turn of xml reader print to screen comments
  xmlReader.setVerbosity(0);
  
  // load in xml file
  xmlReader.readConfigFile(argv[1]);

  // print parsed result
  //xmlReader.print();

  // instantiate steering
  TCS::TopoSteering steering;

  // get parsed algos from reader and load them into the steering
  steering.configureFromXML(xmlReader.getAlgos());
  
  //cout << endl << "         *** TOPO CONFIGURATION ***" << endl << endl;
  //steering.printConfiguration(cout);
  steering.reset();
  
  // instantiate ascii reader class
  TCS::TopoASCIIReader asciiReader;

  // disable print to screen
  //asciiReader.setVerbosity(0);

  // load ascii event file
  asciiReader.inputFile(argv[2]);
  
  // check to see if file loaded properly
  asciiReader.validateFile();
  
  TCS::TopoInputEvent & inputEvent = steering.inputEvent();
  
  asciiReader.setInputEvent(&inputEvent);
  
  // event loop: loop through events in ascii file
  while(asciiReader.getNextEvent()){
    //cout << endl << "         *** INPUT EVENT ***" << endl << endl << steering.inputEvent() << endl;
    steering.executeEvent();
    asciiReader.printEvent();
    cout << steering.simulationResult() << endl;
    steering.reset();
  }
  
  asciiReader.printFileSummary();
  
  return 1;
}




int main(int argc, const char * argv[]) {
  try {
    return run(argc, argv);
  }
  catch(std::exception & e) {
    cerr << "Caught exception: " << e.what() << endl;
  }
  return 1;
};

