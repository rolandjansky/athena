/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./L1TopoSimulation.h"

#include "TH1F.h"

#include "AthenaMonitoring/IMonitorToolBase.h"

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoEvent/TopoInputEvent.h"

#include "L1TopoSimulation/IInputTOBConverter.h"
#include "TrigConfInterfaces/IL1TopoConfigSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"

using namespace std;
using namespace LVL1;


namespace {
   // needed for monitoring
   class TopoResultBit : public IMonitoredAlgo::IGetter {
   public:
    
      //! constructor
      TopoResultBit(const TCS::GlobalDecision & decision, unsigned int module) :
         m_decision(decision),
         m_module(module)
      {}

      //! return size of data
      virtual unsigned int size() const { return 64; }

      //! indexed access to data
      virtual double get(unsigned pos) const { return m_decision.passed(m_module, pos) ? pos : -1.; }

   private:
      const TCS::GlobalDecision & m_decision;
      unsigned int m_module;
   };
}



L1TopoSimulation::L1TopoSimulation(const std::string &name, ISvcLocator *pSvcLocator) :
   AthAlgorithm(name, pSvcLocator),
   m_l1topoConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_monitors(this),
   m_emtauInputProvider("LVL1::EMTauInputProvider/EMTauInputProvider", this),
   m_jetInputProvider("LVL1::JetInputProvider/JetInputProvider", this),
   m_energyInputProvider("LVL1::EnergyInputProvider/EnergyInputProvider", this),
   m_muonInputProvider("LVL1::MuonInputProvider/MuonInputProvider", this),
   m_topoSteering( unique_ptr<TCS::TopoSteering>(new TCS::TopoSteering()) )
{
   declareProperty( "TrigConfigSvc", m_l1topoConfigSvc, "Service to provide the L1Topo menu");
   declareProperty( "EMTAUInputProvider", m_emtauInputProvider, "Tool to fill the EMTAU TOBs of the topo input event");
   declareProperty( "JetInputProvider", m_jetInputProvider, "Tool to fill the Jet TOBs of the topo input event");
   declareProperty( "EnergyInputProvider", m_energyInputProvider, "Tool to fill the energy and MET TOBs of the topo input event");
   declareProperty( "MuonInputProvider", m_muonInputProvider, "Tool to fill the muon TOBs of the topo input event");
   declareProperty( "AthenaMonTools",  m_monitors, "List of monitoring tools to be run with this instance, if incorrect then tool is silently skipped.");
   declareProperty( "EnableInputDump", m_enableInputDump, "Boolean to enable writing of input data for standalone running");
   declareProperty( "InputDumpFile", m_inputDumpFile, "File name for dumping input data");
   declareProperty( "TopoCTPLocation", m_topoCTPLocation = LVL1::DEFAULT_L1TopoCTPLocation, "StoreGate key of topo decision output for CTP" );
   declareProperty( "TopoOutputLevel", m_topoOutputLevel, "OutputLevel for L1Topo algorithms" );
   declareProperty( "TopoSteeringOutputLevel", m_topoSteeringOutputLevel, "OutputLevel for L1Topo steering" );



   const TCS::GlobalDecision & dec = m_topoSteering->simulationResult().globalDecision();
   declareMonitoredCustomVariable("DecisionModule1", new TopoResultBit(dec, 0));
   declareMonitoredCustomVariable("DecisionModule2", new TopoResultBit(dec, 1));
   declareMonitoredCustomVariable("DecisionModule3", new TopoResultBit(dec, 2));

}


L1TopoSimulation::~L1TopoSimulation()
{}


StatusCode
L1TopoSimulation::initialize() {
   ATH_MSG_DEBUG("initialize");

   m_topoSteering->setMsgLevel( TrigConf::MSGTC::Level(m_topoSteeringOutputLevel) );

   CHECK( m_monitors.retrieve() );

   CHECK( m_l1topoConfigSvc.retrieve() );

   return StatusCode::SUCCESS;
}



StatusCode
L1TopoSimulation::start() {
   ATH_MSG_DEBUG("start");

   // monitoring : book histogram
   for (auto mt : m_monitors )
      CHECK( mt->bookHists() );

   const TXC::L1TopoMenu* menu = m_l1topoConfigSvc->menu();
   
   if(menu == nullptr) {
      ATH_MSG_FATAL("No L1 Topo menu from " << m_l1topoConfigSvc->name());
      return StatusCode::FAILURE;
   }


   try {
      m_topoSteering->setupFromConfiguration(*menu);
   }
   catch(exception & e) {
      ATH_MSG_FATAL("Caught exception when configuring topo steering from menu: " << e.what() );
      return StatusCode::FAILURE;
   }

   m_topoSteering->setAlgMsgLevel( TrigConf::MSGTC::Level(m_topoOutputLevel) );

   try {
      m_topoSteering->initializeAlgorithms();
   }
   catch(exception & e) {
      ATH_MSG_FATAL("Caught exception when initializing topo algorithms" << e.what() );
      return StatusCode::FAILURE;
   }

   if( m_enableInputDump ) {
      m_topoSteering->inputEvent().enableInputDump( m_inputDumpFile );
      m_topoSteering->inputEvent().dumpStart();
   }

   return StatusCode::SUCCESS;
}



StatusCode
L1TopoSimulation::execute() {
  
   // reset input and internal state
   m_topoSteering->reset();

   // fill the L1Topo Input Event
   TCS::TopoInputEvent & inputEvent = m_topoSteering->inputEvent();

   // EM TAU
   CHECK(m_emtauInputProvider->fillTopoInputEvent(inputEvent));

   // JET
   CHECK(m_jetInputProvider->fillTopoInputEvent(inputEvent));

   // ET sum, ET miss
   CHECK(m_energyInputProvider->fillTopoInputEvent(inputEvent));

   // Muon
   CHECK(m_muonInputProvider->fillTopoInputEvent(inputEvent));

   ATH_MSG_DEBUG("" << inputEvent);

   inputEvent.dump();

   // execute the toposteering
   m_topoSteering->executeEvent();

   ATH_MSG_DEBUG("" << m_topoSteering->simulationResult().globalDecision());
   

   // get the decision output and put it into storegate for the CTP
   const TCS::GlobalDecision & dec = m_topoSteering->simulationResult().globalDecision();
   LVL1::FrontPanelCTP * topo2CTP = new LVL1::FrontPanelCTP();
   for(unsigned int clock=0; clock<2; ++clock) {
      topo2CTP->setCableWord0( clock, dec.decision( 0, clock) );
      topo2CTP->setCableWord1( clock, dec.decision( 1, clock) );
      topo2CTP->setCableWord2( clock, dec.decision( 2, clock) );
   } 
   CHECK(evtStore()->record( topo2CTP, m_topoCTPLocation ));


   // TODO: get the output combination data and put into SG

   // fill histograms
   for (auto mt : m_monitors )
      if ( ! mt->preSelector() ) 
         mt->fillHists().ignore();

   return StatusCode::SUCCESS;
}




StatusCode
L1TopoSimulation::finalize() {
   m_topoSteering->inputEvent().dumpFinish();

   // monitoring
   for (auto mt : m_monitors )
      mt->finalHists().ignore();

   return StatusCode::SUCCESS;
}
