//
//  main.cpp
//  TestTopo
//
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.
//

#include <iostream>



#include "L1TopoCoreSimulation/ClusterSortingAlg.h"
#include "L1TopoCoreSimulation/DeltaPhiLeading.h"
#include "L1TopoCoreSimulation/ClusterTOB.h"
#include "L1TopoCoreSimulation/ClusterTOBArray.h"
#include "L1TopoCoreSimulation/TOBArray.h"
#include "L1TopoCoreSimulation/ParameterArray.h"
#include "L1TopoCoreSimulation/AlgFactory.h"

#include "L1TopoCoreSimulation/TopoSteering.h"

using namespace std;

int run(int /*argc*/, const char ** /* * argv[]*/) {
   
   TCS::TopoSteering steering;
   
   steering.setupMockConfiguration();

   steering.reset();

   TCS::TopoInputEvent & inputEvent = steering.inputEvent();
   inputEvent.add( TCS::ClusterTOB(5, 1,   3,  12) );
   inputEvent.add( TCS::ClusterTOB(7, 1, -15,   0) );
   inputEvent.add( TCS::ClusterTOB(2, 1,  31, -13) );
   inputEvent.add( TCS::ClusterTOB(9, 1,   0,  -1) );
   inputEvent.add( TCS::ClusterTOB(4, 1,  -2,  -4) );
   inputEvent.add( TCS::JetTOB(4,2,-11,9) );
   inputEvent.add( TCS::JetTOB(13,11,-2,12) );

   cout << steering.inputEvent() << endl;
   
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

