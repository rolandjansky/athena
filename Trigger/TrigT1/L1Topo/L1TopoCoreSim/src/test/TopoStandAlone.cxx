/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include <iostream>
#include <vector>
#include <memory>
#include <stdint.h>

#include "TrigConfBase/TrigConfMessaging.h"

#include "L1TopoConfig/L1TopoXMLParser.h"
#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoCoreSim/StandaloneL1TopoHistSvc.h"
#include "L1TopoCoreSim/TopoASCIIReader.h"
#include "L1TopoInterfaces/Decision.h"

#include "L1TopoEvent/CompositeTOB.h"
#include "L1TopoCoreSim/Connector.h"
#include "L1TopoCoreSim/DecisionConnector.h"

#include "L1TopoEvent/MetTOB.h"

#include "TFile.h"
#include "TH1.h"

using namespace std;

int run(int argc, const char* argv[]) {

   if(argc<3) {
      cout << "Please specify menu and data file:\n" << argv[0] << " <menu.xml> <data.txt> [INFO|DEBUG|WARNING] [INFO|DEBUG|WARNING]" << endl;
      return 1;
   }

   TrigConf::MSGTC::Level msgLvl = TrigConf::MSGTC::WARNING;
   TrigConf::MSGTC::Level algMsgLvl = TrigConf::MSGTC::WARNING;

   if(argc>=4) {
      string msgInput(argv[3]);
      msgLvl = (msgInput=="DEBUG")?TrigConf::MSGTC::DEBUG:(msgInput=="INFO")?TrigConf::MSGTC::INFO:TrigConf::MSGTC::WARNING;
      algMsgLvl = msgLvl;
   }

   if(argc>=5) {
      string msgInput(argv[4]);
      algMsgLvl = (msgInput=="DEBUG")?TrigConf::MSGTC::DEBUG:(msgInput=="INFO")?TrigConf::MSGTC::INFO:TrigConf::MSGTC::WARNING;
   }

   TrigConf::MsgStreamTC msg("TopoStandalone");
   msg.setLevel( msgLvl );

   // read the menu
   TXC::L1TopoXMLParser XMLParser;
   XMLParser.msg().setLevel( msgLvl );
   XMLParser.readConfiguration(argv[1]);
   XMLParser.parseConfiguration();

   //XMLParser.menu().print();

   string filename = "L1TopoSimulation.root";
   if(argc>=4)
      filename = argv[3];

   //TFile *f = new TFile(argc>=4 ? argv[3] : "L1TopoSimulation.root","RECREATE");
   TH1* h[3];
   h[0] = new TH1F("Decision/DecisionModule1", "L1 Topo Decision (Module 1)", 64, 0, 64);
   h[1] = new TH1F("Decision/DecisionModule2", "L1 Topo Decision (Module 2)", 64, 0, 64);
   h[2] = new TH1F("Decision/DecisionModule3", "L1 Topo Decision (Module 3)", 64, 0, 64);

   const std::vector<TXC::TriggerLine> & topoTriggers = XMLParser.menu().getL1TopoConfigOutputList().getTriggerLines();
   for(const TXC::TriggerLine& tl : topoTriggers) {
      h[tl.module()]->GetXaxis()->SetBinLabel(1+ tl.counter() % 64, tl.name().c_str());
   }
   for(uint i=0; i<3; ++i)
      h[i]->SetLabelSize(0.025);


   // instantiate steering
   TCS::TopoSteering steering;
   steering.setUseBitwise(false);
   steering.setupFromConfiguration(XMLParser.takeMenu());

   steering.setMsgLevel( msgLvl );

   steering.setAlgMsgLevel( algMsgLvl );

   std::shared_ptr<IL1TopoHistSvc> topoHistSvc = std::shared_ptr<IL1TopoHistSvc>( new StandaloneL1TopoHistSvc() );
   topoHistSvc->setBaseDir("L1TopoSimulation.root:");
   for(int i = 0; i < 3; i++ )
      topoHistSvc->registerHist(h[i]);




   steering.setHistSvc(topoHistSvc);

   //steering.printConfiguration(cout);

   //steering.structure().printParameters(cout);

   steering.initializeAlgorithms();

   TCS::TopoASCIIReader reader; // instantiate ascii reader

   reader.setVerbosity(0); // disable print to screen

   // load ascii event file
   reader.loadInput(argv[2]);
   reader.validateInput();
  

   // instantiate input event
   TCS::TopoInputEvent & inputEvent = steering.inputEvent();
   inputEvent.msg().setLevel( msgLvl );
   reader.setInputEvent(&inputEvent);

   //steering.simulationResult().globalDecision().msg().setLevel( TrigConf::MSGTC::INFO );

   // loop over the events
   while(reader.getNextEvent()) {

      msg << TrigConf::MSGTC::INFO << "=======================================================" << TrigConf::endmsgtc;

      steering.executeEvent();

      const TCS::GlobalDecision & globalDec = steering.simulationResult().globalDecision();

      for(unsigned int module=0; module<3; ++module)
         for(unsigned int trigger=0; trigger<64; ++trigger)
            if( globalDec.passed(module, trigger) ) h[module]->Fill(trigger);

      steering.reset();

   }
   msg << TrigConf::MSGTC::INFO << "=======================================================" << TrigConf::endmsgtc;
  
//    f->Write();
//    f->Close();

   steering.saveHist();

   reader.printFileSummary();
  
   return 0;
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

