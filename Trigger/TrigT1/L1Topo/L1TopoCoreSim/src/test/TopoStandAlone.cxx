#include <iostream>
#include <vector>
#include <stdint.h>

#include "L1TopoConfig/L1TopoXMLParser.h"
#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoCoreSim/TopoASCIIReader.h"
#include "L1TopoInterfaces/Decision.h"

#include "L1TopoEvent/CompositeTOB.h"
#include "L1TopoCoreSim/Connector.h"
#include "L1TopoCoreSim/DecisionConnector.h"

#include "TFile.h"
#include "TH1.h"

using namespace std;

int run(int argc, const char* argv[]) {


   if(argc<3) {
      cout << "Please specify menu and data file:\n" << argv[0] << " <menu.xml> <data.txt>" << endl;
      return 1;
   }

   // read the menu
   TXC::L1TopoXMLParser XMLParser;
   XMLParser.readConfiguration(argv[1]);
   XMLParser.parseConfiguration();

   string filename = "L1TopoSimulation.root";
   if(argc>=4)
      filename = argv[3];

   TFile *f = new TFile(argc>=4 ? argv[3] : "L1TopoSimulation.root","RECREATE");
   TH1* h[3];
   h[0] = new TH1F("DecisionModule1", "L1 Topo Decision (Module 1)", 64, 0, 64);
   h[1] = new TH1F("DecisionModule2", "L1 Topo Decision (Module 2)", 64, 0, 64);
   h[2] = new TH1F("DecisionModule3", "L1 Topo Decision (Module 3)", 64, 0, 64);

   // instantiate steering
   TCS::TopoSteering steering;
   steering.setupFromConfiguration(XMLParser.takeMenu());
   steering.printConfiguration(cout);
   // steering.structure().printParameters(cout);

   steering.initializeAlgorithms();

   TCS::TopoASCIIReader reader; // instantiate ascii reader

   reader.setVerbosity(0); // disable print to screen


   // load ascii event file
   reader.loadInput(argv[2]);
   reader.validateInput();
  

   // instantiate input event
   TCS::TopoInputEvent & inputEvent = steering.inputEvent();
   reader.setInputEvent(&inputEvent);


   // loop over the events
   while(reader.getNextEvent()){

      cout << inputEvent << endl;
    
      steering.executeEvent();

      const TCS::GlobalDecision & globalDec = steering.simulationResult().globalDecision();

      cout << globalDec << endl;

      for(unsigned int module=0; module<3; ++module)
         for(unsigned int trigger=0; trigger<64; ++trigger)
            if( globalDec.passed(module, trigger) ) h[module]->Fill(trigger);

      steering.reset();

   }
  
   f->Write();
   f->Close();

   reader.printFileSummary();
  
   return 1;
}


int main(int argc, const char * argv[]) {
  try {
    return run(argc, argv);
  }
  catch(std::exception & e) {
    cerr << "Caught exception: " << e.what() << endl;
    return 1;
  }
  return 0;
}

