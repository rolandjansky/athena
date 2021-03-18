/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoSimulation.h"
#include "AthenaL1TopoHistSvc.h"

#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoEvent/TopoInputEvent.h"

#include "L1TopoInterfaces/IL1TopoHistSvc.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/EmTauROI_ClassDEF.h"

#include "L1TopoRDO/BlockTypes.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"

// using namespace std;
using namespace LVL1;


L1TopoSimulation::L1TopoSimulation(const std::string &name, ISvcLocator *pSvcLocator) :
   AthAlgorithm(name, pSvcLocator),
   m_topoSteering( std::make_unique<TCS::TopoSteering>() ),
   m_scaler( std::make_unique<LVL1::PeriodicScaler>() )
{
}


bool
L1TopoSimulation::isClonable() const
{
   return true;
}

StatusCode
L1TopoSimulation::initialize() {
   ATH_MSG_INFO("initialize");

   m_topoSteering->setMsgLevel( TrigConf::MSGTC::Level((int)m_topoSteeringOutputLevel) );

   m_topoSteering->setLegacyMode(m_isLegacyTopo);
   
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

   CHECK(m_topoCTPLocation.initialize());
   CHECK(m_topoOverflowCTPLocation.initialize());

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

   const TrigConf::L1Menu * l1menu = nullptr;
   ATH_CHECK( detStore()->retrieve(l1menu) ); 
   ATH_MSG_INFO( "initialize(): retrieving new-style L1 trigger menu from Detector Store" );

   m_topoSteering->setUseBitwise(m_enableBitwise);
   try {
      m_topoSteering->setupFromConfiguration(*l1menu);
   }
   catch(std::exception & e) {
      ATH_MSG_FATAL("Caught exception when configuring topo steering from menu: " << e.what() );
      return StatusCode::FAILURE;
   }

   m_topoSteering->setAlgMsgLevel( TrigConf::MSGTC::Level((int)m_topoOutputLevel) );
   m_topoSteering->setOutputAlgosFillBasedOnHardware(m_fillHistogramsBasedOnHardwareDecision);

   std::shared_ptr<IL1TopoHistSvc> topoHistSvc = std::shared_ptr<IL1TopoHistSvc>( new AthenaL1TopoHistSvc(m_histSvc) );
   topoHistSvc->setBaseDir("/EXPERT/" + m_histBaseDir.value());

   m_topoSteering->setHistSvc(topoHistSvc);

   return StatusCode::SUCCESS;
}


// Exectued once per offline job and for every new run online
StatusCode
L1TopoSimulation::start() {
   ATH_MSG_DEBUG("start");

   m_scaler->reset();

   try {
      m_topoSteering->initializeAlgorithms();
   }
   catch(std::exception & e) {
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
   const EventContext& ctx = Gaudi::Hive::currentContext();

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

   inputEvent.setEventInfo(ctx.eventID().run_number(),
                           ctx.eventID().event_number(),
                           ctx.eventID().lumi_block(),
                           ctx.eventID().bunch_crossing_id());

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

   ATH_MSG_DEBUG("Global Decision:\n" << m_topoSteering->simulationResult().globalOutput());
   

   /**
    * Get the decision output and store for the CTP simulation
    *
    * note the the topo simulation currently uses modules 0 and 1
    * while the CTP front panel expects the topo on cables 1 and 2
    * (cable 0 is coming from ALFA)
    *
    */

   // Format for CTP still undecided

   const TCS::GlobalOutput & globalOutput = m_topoSteering->simulationResult().globalOutput();
   auto topoOutput2CTP = std::make_unique< LVL1::FrontPanelCTP >();
   auto topoOverflow2CTP = std::make_unique< LVL1::FrontPanelCTP >();

   const TrigConf::L1Menu * l1menu = nullptr;
   ATH_CHECK( detStore()->retrieve(l1menu) );

   if( m_isLegacyTopo ) {
      // to be implemented
   } else {
      // set electrical connectors 
      std::string conn1 = l1menu->board("Topo2").connectorNames()[0];
      std::string conn2 = l1menu->board("Topo3").connectorNames()[0];
      for(unsigned int clock=0; clock<2; ++clock) {
         topoOutput2CTP->setCableWord0( clock, 0 ); // ALFA
         ATH_MSG_DEBUG("Word 1 " << conn1 << " clock " << clock << "  " << globalOutput.decision_field( conn1, clock) );
         topoOutput2CTP->setCableWord1( clock, globalOutput.decision_field( conn1, clock) );  // TOPO 0
         ATH_MSG_DEBUG("Word 2 " << conn2 << " clock " << clock << "  " << globalOutput.decision_field( conn2, clock) );
         topoOutput2CTP->setCableWord2( clock, globalOutput.decision_field( conn2, clock) );  // TOPO 1
         // topoOverflow2CTP->setCableWord0( clock, 0 ); // ALFA
         // topoOverflow2CTP->setCableWord1( clock, dec.overflow( 0, clock) );  // TOPO 0
         // topoOverflow2CTP->setCableWord2( clock, dec.overflow( 1, clock) );  // TOPO 1
      }    

      // set optical connectors
      
      for(int optcable=0; optcable<4; ++optcable) {
         std::string connOpt = l1menu->board("Topo1").connectorNames()[optcable];
         topoOutput2CTP->setOptCableWord( optcable, globalOutput.count_field(connOpt) );
      }
      
   }

   
   CHECK(SG::makeHandle(m_topoCTPLocation)        .record(std::move(topoOutput2CTP)));
   CHECK(SG::makeHandle(m_topoOverflowCTPLocation).record(std::move(topoOverflow2CTP)));

   return StatusCode::SUCCESS;
}


StatusCode
L1TopoSimulation::finalize() {
   m_topoSteering->inputEvent().dumpFinish();
   return StatusCode::SUCCESS;
}


StatusCode
L1TopoSimulation::retrieveHardwareDecision()
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
        const L1TopoRDOCollection* rdos = 0;
        sc = evtStore()->retrieve(rdos);
        if (sc.isFailure() or 0 == rdos) {
            ATH_MSG_INFO ( "Could not retrieve L1Topo DAQ RDO collection from StoreGate" );
        } else if (rdos->empty()) {
            ATH_MSG_INFO ( "L1Topo DAQ RDO collection is empty" );
        } else {
            for (const L1TopoRDO* rdo : *rdos){
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
