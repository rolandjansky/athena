/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  main.cpp
//  TestTopo
//
//  Created by Joerg Stelzer on 11/10/12.
//

#include <iostream>

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoAlgorithms/ClusterSortingAlg.h"
#include "L1TopoAlgorithms/DeltaPhiLeading.h"
#include "L1TopoEvent/ClusterTOB.h"


using namespace std;

int run(int /*argc*/, const char ** /* * argv[]*/) {
   
   TCS::TopoSteering steering;

   steering.setupMockConfiguration();

   steering.printConfiguration(cout);

   steering.reset();

   TCS::TopoInputEvent & inputEvent = steering.inputEvent();
   inputEvent.add( TCS::ClusterTOB(5, 1,   3,  12) );
   inputEvent.add( TCS::ClusterTOB(7, 1, -15,   0) );
   inputEvent.add( TCS::ClusterTOB(2, 1,  31, -13) );
   inputEvent.add( TCS::ClusterTOB(9, 1,   0,  -1) );
   inputEvent.add( TCS::ClusterTOB(4, 1,  -2,  -4) );
   inputEvent.add( TCS::JetTOB(4,2,-11,9) );
   inputEvent.add( TCS::JetTOB(13,11,-2,12) );

   cout << endl << "         *** INPUT EVENT ***" << endl << endl << steering.inputEvent() << endl;

   cout << endl << "         *** EXECUTION ***" << endl << endl;

   steering.executeEvent();

   cout << steering.simulationResult() << endl;

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

