/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./L1TopoSimulation.h"
#include "./AthenaL1TopoHistSvc.h"
//#include "./getAthenaTopoHistSvc.h"

#include "TH1F.h"

#include "AthenaMonitoring/IMonitorToolBase.h"

#include "L1TopoCoreSim/TopoSteering.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoEvent/TopoInputEvent.h"

#include "L1TopoSimulation/IInputTOBConverter.h"
#include "TrigConfInterfaces/IL1TopoConfigSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "L1TopoInterfaces/IL1TopoHistSvc.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"

#include "L1TopoRDO/BlockTypes.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"


// #include "TrigSteering/Scaler.h"
#include "./PeriodicScaler.h"


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
   m_histSvc( "THistSvc/THistSvc", name),
   m_monitors(this),
   m_emtauInputProvider("LVL1::EMTauInputProvider/EMTauInputProvider", this),
   m_jetInputProvider("LVL1::JetInputProvider/JetInputProvider", this),
   m_energyInputProvider("LVL1::EnergyInputProvider/EnergyInputProvider", this),
   m_muonInputProvider("LVL1::MuonInputProvider/MuonInputProvider", this),
   m_topoSteering( unique_ptr<TCS::TopoSteering>(new TCS::TopoSteering()) )
{
   declareProperty( "TrigConfigSvc", m_l1topoConfigSvc, "Service to provide the L1Topo menu");
   declareProperty( "HistSvc", m_histSvc, "Histogramming service for L1Topo algorithms");
   declareProperty( "EMTAUInputProvider", m_emtauInputProvider, "Tool to fill the EMTAU TOBs of the topo input event");
   declareProperty( "JetInputProvider", m_jetInputProvider, "Tool to fill the Jet TOBs of the topo input event");
   declareProperty( "EnergyInputProvider", m_energyInputProvider, "Tool to fill the energy and MET TOBs of the topo input event");
   declareProperty( "MuonInputProvider", m_muonInputProvider, "Tool to fill the muon TOBs of the topo input event");
   declareProperty( "AthenaMonTools", m_monitors, "List of monitoring tools to be run with this instance, if incorrect then tool is silently skipped.");
   declareProperty( "MonHistBaseDir", m_histBaseDir = "L1TopoAlgorithms", "Base directory for monitoring histograms will be /EXPERT/<MonHistBaseDir>" );
   declareProperty( "EnableInputDump", m_enableInputDump, "Boolean to enable writing of input data for standalone running");
   declareProperty( "UseBitwise", m_enableBitwise, "Boolean to enable the bitwise version of software algorithms");
   declareProperty("FillHistoBasedOnHardware", m_fillHistogramsBasedOnHardwareDecision=true,
                   "Boolean to fill accept/reject histograms based on hdw; default based on sim");
   declareProperty( "InputDumpFile", m_inputDumpFile, "File name for dumping input data");
   declareProperty( "TopoCTPLocation", m_topoCTPLocation = LVL1::DEFAULT_L1TopoCTPLocation, "StoreGate key of topo decision output for CTP" );
   declareProperty( "TopoOverflowCTPLocation", m_topoOverflowCTPLocation = LVL1::DEFAULT_L1TopoOverflowCTPLocation, "StoreGate key of topo overflow output for CTP" );
   declareProperty( "TopoOutputLevel", m_topoOutputLevel, "OutputLevel for L1Topo algorithms" );
   declareProperty( "TopoSteeringOutputLevel", m_topoSteeringOutputLevel, "OutputLevel for L1Topo steering" );
   declareProperty("Prescale", m_prescale = 1, "Internal prescale factor for this algorithm, implemented with a periodic scaler: so 1 means run every time, N means run every 1 in N times it is called; the other times it will exit without doing anything");
   declareProperty("PrescaleDAQROBAccess", m_prescaleForDAQROBAccess = 4, "Prescale factor for requests for DAQ ROBs: can be used to avoid overloading ROS. Zero means disabled, 1 means always, N means sample only 1 in N events");


   const TCS::GlobalDecision & dec = m_topoSteering->simulationResult().globalDecision();
   declareMonitoredCustomVariable("DecisionModule1", new TopoResultBit(dec, 0));
   declareMonitoredCustomVariable("DecisionModule2", new TopoResultBit(dec, 1));
   declareMonitoredCustomVariable("DecisionModule3", new TopoResultBit(dec, 2));
   m_scaler = new LVL1::PeriodicScaler();
   
}


L1TopoSimulation::~L1TopoSimulation()
{}


StatusCode
L1TopoSimulation::initialize() {
   ATH_MSG_INFO("initialize");

   m_topoSteering->setMsgLevel( TrigConf::MSGTC::Level(m_topoSteeringOutputLevel) );

   ATH_MSG_DEBUG("retrieving " << m_monitors);
   CHECK( m_monitors.retrieve() );

   ATH_MSG_DEBUG("retrieving " << m_l1topoConfigSvc);
   CHECK( m_l1topoConfigSvc.retrieve() );

   ATH_MSG_DEBUG("retrieving " << m_histSvc);
   CHECK( m_histSvc.retrieve() );

   ATH_MSG_DEBUG("retrieving " << m_emtauInputProvider);
   CHECK( m_emtauInputProvider.retrieve() );

   ATH_MSG_DEBUG("retrieving " << m_jetInputProvider);
   CHECK( m_jetInputProvider.retrieve() );

   ATH_MSG_DEBUG("retrieving " << m_energyInputProvider);
   CHECK( m_energyInputProvider.retrieve() );

   ATH_MSG_DEBUG("retrieving " << m_muonInputProvider);
   CHECK( m_muonInputProvider.retrieve() );

   ATH_MSG_DEBUG("Prescale factor set to " << m_prescale);
   ATH_MSG_DEBUG("PrescaleDAQROBAccess factor set to " << m_prescaleForDAQROBAccess);
   ATH_MSG_DEBUG("FillHistoBasedOnHardware " << m_fillHistogramsBasedOnHardwareDecision);
   if(m_fillHistogramsBasedOnHardwareDecision and
      (m_prescaleForDAQROBAccess % m_prescale)) {
      ATH_MSG_FATAL("PrescaleDAQROBAccess must be a multiple of Prescale"
                    <<" : current values :"
                    <<" "<<m_prescaleForDAQROBAccess
                    <<", "<<m_prescale);
      return StatusCode::FAILURE;
   }
   ATH_MSG_DEBUG("Output trigger key property " << m_topoCTPLocation);
   ATH_MSG_DEBUG("Output overflow key property " << m_topoOverflowCTPLocation);

   const TXC::L1TopoMenu* menu = m_l1topoConfigSvc->menu();
   if(menu == nullptr) {
      ATH_MSG_FATAL("No L1 Topo menu from " << m_l1topoConfigSvc->name());
      return StatusCode::FAILURE;
   }

   m_topoSteering->setUseBitwise(m_enableBitwise);
   std::cout << "Calling m_topoSteering->setupFromConfiguration(*menu)" << endl;
   try {
      m_topoSteering->setupFromConfiguration(*menu);
   }
   catch(exception & e) {
      ATH_MSG_FATAL("Caught exception when configuring topo steering from menu: " << e.what() );
      return StatusCode::FAILURE;
   }

   m_topoSteering->setAlgMsgLevel( TrigConf::MSGTC::Level(m_topoOutputLevel) );
   m_topoSteering->setOutputAlgosFillBasedOnHardware(m_fillHistogramsBasedOnHardwareDecision);


   std::shared_ptr<IL1TopoHistSvc> topoHistSvc = std::shared_ptr<IL1TopoHistSvc>( new AthenaL1TopoHistSvc(m_histSvc) );
   topoHistSvc->setBaseDir("/EXPERT/" + m_histBaseDir.value());

   m_topoSteering->setHistSvc(topoHistSvc);

   return StatusCode::SUCCESS;
}

// Exectued at every run-number change both online and offline
StatusCode 
L1TopoSimulation::beginRun() {
   m_scaler->reset();
   return StatusCode::SUCCESS;
}

// Exectued at every run-number change both online and offline
StatusCode 
L1TopoSimulation::endRun() {
   return StatusCode::SUCCESS;
}

// Exectued once per offline job and for every new run online
StatusCode
L1TopoSimulation::stop() {
   ATH_MSG_DEBUG("stop");

   // monitoring
   for (auto mt : m_monitors )
      mt->finalHists().ignore();


   return StatusCode::SUCCESS;
}
                           

// Exectued once per offline job and for every new run online
StatusCode
L1TopoSimulation::start() {
   ATH_MSG_DEBUG("start");

   // monitoring : book histogram
   for (auto mt : m_monitors )
      CHECK( mt->bookHists() );


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
  

   if (m_prescale>1 && not m_scaler->decision(m_prescale)){
      ATH_MSG_DEBUG( "This event not processed due to prescale");
      return StatusCode::SUCCESS;
      // do not record dummy output: 
      // LVL1::FrontPanelCTP is initialised with all 6 32-bit words set to 0 which is valid data
      // LVL1::FrontPanelCTP * topo2CTP = new LVL1::FrontPanelCTP();
      // CHECK(evtStore()->record( topo2CTP, m_topoCTPLocation ));
   } 
   else {
     ATH_MSG_DEBUG( "This event is processed - not prescaled");
   }

   // reset input and internal state
   m_topoSteering->reset();

   // fill the L1Topo Input Event
   TCS::TopoInputEvent & inputEvent = m_topoSteering->inputEvent();

   // Event Info
   const EventInfo *evt;
   CHECK(evtStore()->retrieve(evt));
   inputEvent.setEventInfo(evt->event_ID()->run_number(),evt->event_ID()->event_number(),evt->event_ID()->lumi_block(),evt->event_ID()->bunch_crossing_id());

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
   
   if(m_fillHistogramsBasedOnHardwareDecision){
       if(m_scaler->decision(m_prescaleForDAQROBAccess) and
          retrieveHardwareDecision()){
           m_topoSteering->propagateHardwareBitsToAlgos();
           m_topoSteering->setOutputAlgosSkipHistograms(false);
       } else {
           m_topoSteering->setOutputAlgosSkipHistograms(true);
       }
   }

   // execute the toposteering
   m_topoSteering->executeEvent();

   ATH_MSG_DEBUG("" << m_topoSteering->simulationResult().globalDecision());
   

   /**
    * Get the decision output and store for the CTP simulation
    *
    * note the the topo simulation currently uses modules 0 and 1
    * while the CTP front panel expects the topo on cables 1 and 2
    * (cable 0 is coming from ALFA)
    *
    */

   const TCS::GlobalDecision & dec = m_topoSteering->simulationResult().globalDecision();
   LVL1::FrontPanelCTP * topoDecision2CTP = new LVL1::FrontPanelCTP();
   LVL1::FrontPanelCTP * topoOverflow2CTP = new LVL1::FrontPanelCTP();
   for(unsigned int clock=0; clock<2; ++clock) {
      topoDecision2CTP->setCableWord0( clock, 0 ); // ALFA
      topoDecision2CTP->setCableWord1( clock, dec.decision( 0, clock) );  // TOPO 0
      topoDecision2CTP->setCableWord2( clock, dec.decision( 1, clock) );  // TOPO 1
      topoOverflow2CTP->setCableWord0( clock, 0 ); // ALFA
      topoOverflow2CTP->setCableWord1( clock, dec.overflow( 0, clock) );  // TOPO 0
      topoOverflow2CTP->setCableWord2( clock, dec.overflow( 1, clock) );  // TOPO 1
   } 
   CHECK(evtStore()->record( topoDecision2CTP, m_topoCTPLocation ));
   CHECK(evtStore()->record( topoOverflow2CTP, m_topoOverflowCTPLocation ));

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

   delete m_scaler;
   m_scaler=0;

   return StatusCode::SUCCESS;
}

StatusCode L1TopoSimulation::retrieveHardwareDecision()
{
    // some duplication with L1TopoRDO::Helpers
    // getDecisionAndOverflowBits() ?
    StatusCode sc = StatusCode::SUCCESS;
    std::bitset<TCS::TopoSteering::numberOfL1TopoBits> hardwareDaqRobTriggerBits;
    std::bitset<TCS::TopoSteering::numberOfL1TopoBits> hardwareDaqRobOvrflowBits;
    bool prescalForDAQROBAccess = true; // DG-2017-06-30 decide what to do when the hdw dec is not to be retrieved
    if (prescalForDAQROBAccess){
        std::vector<L1Topo::L1TopoTOB> daqTobsBC0;
        std::vector<uint32_t> tobsbc0SourceIds; // to compute bit indices
        const DataHandle<L1TopoRDOCollection> rdos = 0;
        sc = evtStore()->retrieve(rdos);
        if (sc.isFailure() or 0 == rdos) {
            ATH_MSG_INFO ( "Could not retrieve L1Topo DAQ RDO collection from StoreGate" );
        } else if (rdos->empty()) {
            ATH_MSG_INFO ( "L1Topo DAQ RDO collection is empty" );
        } else {
            for (auto & rdo : *rdos){
                const std::vector<uint32_t> cDataWords = rdo->getDataWords();
                // initialise header: set version 15, BCN -7, which is unlikely
                L1Topo::Header header(0xf,0,0,0,0,1,0x7);
                for (auto word : cDataWords){
                    switch (L1Topo::blockType(word)){
                    case L1Topo::BlockTypes::HEADER: {
                        header = L1Topo::Header(word);
                        break;
                    }
                    case L1Topo::BlockTypes::L1TOPO_TOB: {
                        auto tob = L1Topo::L1TopoTOB(word);
                        if (header.bcn_offset()==0){
                            daqTobsBC0.push_back(tob);
                            tobsbc0SourceIds.push_back(rdo->getSourceID());
                        }
                        break;
                    }
                    default: break;
                    }
                } // for(word)
            } // for(rdo)
        }
        for(uint32_t iTob=0; iTob<daqTobsBC0.size(); ++iTob){
            const L1Topo::L1TopoTOB &tob = daqTobsBC0[iTob];
            const uint32_t &sourceId = tobsbc0SourceIds[iTob];
            for(unsigned int i=0; i<8; ++i){
                unsigned int index = L1Topo::triggerBitIndexNew(sourceId, tob, i);
                hardwareDaqRobTriggerBits[index] = (tob.trigger_bits()>>i)&1;
                hardwareDaqRobOvrflowBits[index] = (tob.overflow_bits()>>i)&1;
            }
        }
        m_topoSteering->setHardwareBits(hardwareDaqRobTriggerBits,
                                        hardwareDaqRobOvrflowBits);
    }
    return sc;
}
