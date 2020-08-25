// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#include <iostream>

#include "L1TopoCoreSim/ClusterSortingAlg.h"
#include "L1TopoCoreSim/DeltaPhiLeading.h"
#include "L1TopoCoreSim/ClusterTOB.h"
#include "L1TopoCoreSim/ClusterTOBArray.h"
#include "L1TopoCoreSim/TOBArray.h"
#include "L1TopoCoreSim/AlgFactory.h"
#include "L1TopoCoreSim/TopoSteering.h"

#include "TrigConfL1TopoData/L1TopoConfigReader.h"

using namespace std;

int run(int argc/*argc*/, const char * argv[]/* * argv[]*/) {
   
   TCS::TopoSteering steering;

   steering.setupMockConfiguration();
   /*
   L1TopoConfigReader xmlReader;
   xmlReader.setVerbosity(1);
   xmlReader.readConfigFile(argv[1]);
   
   xmlReader.print();

   steering.configureFromXML(xmlReader.getAlgos());
   */
   cout << endl << "         *** TOPO CONFIGURATION ***" << endl << endl;

   steering.printConfiguration(cout);

   steering.reset();

   TCS::TopoInputEvent & inputEvent = steering.inputEvent();
   inputEvent.add( TCS::ClusterTOB(5, 1,   3,  -12) );
   inputEvent.add( TCS::ClusterTOB(7, 1, -15,   0) );
   inputEvent.add( TCS::ClusterTOB(2, 1,  31, -10) );
   inputEvent.add( TCS::ClusterTOB(9, 1,   0,  12) );
   inputEvent.add( TCS::ClusterTOB(4, 1,  -2,  6) );

   inputEvent.add( TCS::JetTOB(5, 4,   3,  12) );
   inputEvent.add( TCS::JetTOB(7, 6, -15,   0) );
   inputEvent.add( TCS::JetTOB(2, 1,  31, -13) );
   inputEvent.add( TCS::JetTOB(9, 8,   0,  -1) );
   inputEvent.add( TCS::JetTOB(4, 3,  -2,  -4) );


   cout << endl << "         *** INPUT EVENT ***" << endl << endl << steering.inputEvent() << endl;

   cout << endl << "         *** EXECUTION ***" << endl << endl;



   //   cout << "Going to execute" << endl;
   steering.executeEvent();

   //   cout << "Reporting simulation result" << endl;
   cout << steering.simulationResult() << endl;

   steering.printDebugInfo();

   //   cout << "Resetting the steering" << endl;
   steering.reset();

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

