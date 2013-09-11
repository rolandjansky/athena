/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes:
#include <vector>
#include <iomanip>

// Athena/Gaudi includes:
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "PathResolver/PathResolver.h"

// Trigger configuration interface includes:
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfL1Data/PrescaledClock.h"
#include "TrigConfL1Data/DeadTime.h"
#include "TrigConfL1Data/Random.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/TriggerThreshold.h"

// TrigConfStorage includes
#include "TrigConfStorage/TrigConfCoolL1PayloadConverters.h"

// Common interface includes:
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/TrigT1CTPDefs.h"
#include "TrigT1Interfaces/CTPSLink.h"

// Inputs to the CTP:
#include "TrigT1Interfaces/MuCTPICTP.h"
#include "TrigT1Interfaces/EmTauCTP.h"
#include "TrigT1Interfaces/JetCTP.h"
#include "TrigT1Interfaces/EnergyCTP.h"
#include "TrigT1Interfaces/MbtsCTP.h"
#include "TrigT1Interfaces/BcmCTP.h"
#include "TrigT1Interfaces/LucidCTP.h"
#include "TrigT1Interfaces/ZdcCTP.h"
#include "TrigT1Interfaces/BptxCTP.h"
#include "TrigT1Interfaces/NimCTP.h"

// CTP DAQ output:
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTPResult.h"
#include "TrigT1Result/CTP_Decoder.h"

// Include for the configuration service:
#include "TrigConfigSvc/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"

// Local includes:
#include "TrigT1CTP/CTPSimulation.h"
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/CTPTriggerThreshold.h"
#include "TrigT1CTP/CTPTriggerItemNode.h"
#include "TrigT1CTP/CTPUtil.h"
#include "TrigT1CTP/ResultBuilder.h"
#include "TrigT1CTP/EventInfo.h"
#include "TrigT1CTP/CTPConstants.h"
#include "TrigT1CTP/CTPTriggerItem.h"
#include "TrigT1CTP/ItemMap.h"
#include "TrigT1CTP/MsgWriter.h"
#include "TrigT1CTP/ISpecialTrigger.h"
#include "TrigT1CTP/RandomTrigger.h"
#include "TrigT1CTP/PrescaledClockTrigger.h"
#include "TrigT1CTP/BunchGroupTrigger.h"
#include "TrigT1CTP/CustomBit.h"

// Facilitate reading from COOL
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "boost/foreach.hpp"

using namespace std;
using namespace TrigConf;

namespace LVL1CTP {
   //! enumeration for prescale mode
   enum PrescaleMode { DEFAULT = 0, OFFSET, RANDOM };
}

LVL1CTP::CTPSimulation::CTPSimulation( const std::string& name, ISvcLocator* pSvcLocator ) :
   AthAlgorithm( name, pSvcLocator ), 
   m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
   m_rndmSvc("AtRndmGenSvc",name), 
   m_rndmEngineName("CTPSimulation"),
   m_monitors(this),
   m_decisionMap(0),
   m_itemMap(0),
   m_resultBuilder(0)
{

   // services
   declareProperty( "TrigConfigSvc", m_configSvc, "Trig Config Service");
   declareProperty( "RndmSvc", m_rndmSvc, "Random Number Service used in CTP simulation" );
   declareProperty( "RndmEngine", m_rndmEngineName, "Random engine name");
   // Property setting general behaviour:
   declareProperty( "DoCalo",  m_doCalo  = true, "Use inputs (multiplicities) from Calo system" );
   declareProperty( "DoMBTS",  m_doMBTS  = true, "Use inputs from MBTS system" );
   declareProperty( "DoMBTSSI",  m_doMBTSSI  = true, "Use single inputs from MBTS system" );
   declareProperty( "DoMuon",  m_doMuon  = true, "Use inputs (multiplicities) from Muon system" );
   declareProperty( "DoBCM",   m_doBCM   = true, "Use inputs from BCM system" );
   declareProperty( "DoLUCID", m_doLUCID = true, "Use inputs from LUCID system" );
   declareProperty( "DoZDC",   m_doZDC   = true, "Use inputs from ZDC system" );
   declareProperty( "DoNIM",   m_doNIM   = false, "Use inputs from NIM system" );
   declareProperty( "DoBPTX",  m_doBPTX = false, "Use inputs from BPTX system" );
   declareProperty( "DoRNDM",  m_doRNDM = true, "Simulate internal random trigger" );
   declareProperty( "DoPSCL",  m_doPSCL = true, "Simulate internal prescaled clock trigger" );
   declareProperty( "PrescaleMode", m_prescaleMode = DEFAULT, 
                    "Mode for applying prescale: 0 - as hardware, 1 - random offset, 2 - random prescales"); 
   m_prescaleMode.verifier().setLower(DEFAULT); m_prescaleMode.verifier().setUpper(RANDOM);
   declareProperty( "IsData", m_IsData = false, "Rerun simulation on data" );  
   declareProperty( "UseDeadTime", m_applyDeadTime = false, "Simulate DeadTime" );    
   declareProperty( "UseBunchGroup", m_applyBunchGroup = true, "Simulate BunchGroup trigger" );    
   declareProperty( "BunchGroupLocation", m_BunchGroupLoc="/TRIGGER/LVL1/BunchGroupContent", "Detector store location of bunch groups");

   // Properties setting up the location of the data sent to the CTP:
   declareProperty( "MuonCTPLocation", m_muonCTPLoc = LVL1MUCTPI::DEFAULT_MuonCTPLocation, 
                    "StoreGate location of Muon inputs" );
   declareProperty( "EmTauCTPLocation", m_emTauCTPLoc = LVL1::TrigT1CaloDefs::EmTauCTPLocation, 
                    "StoreGate location of EmTau inputs" );
   declareProperty( "JetCTPLocation", m_jetCTPLoc = LVL1::TrigT1CaloDefs::JetCTPLocation, 
                    "StoreGate location of Jet inputs" );
   declareProperty( "EnergyCTPLocation", m_energyCTPLoc = LVL1::TrigT1CaloDefs::EnergyCTPLocation, 
                    "StoreGate location of Energy inputs" );
   declareProperty( "MbtsACTPLocation", m_mbtsACTPLoc = LVL1::DEFAULT_MbtsACTPLocation, 
                    "StoreGate location of MBTSA inputs" );
   declareProperty( "MbtsCCTPLocation", m_mbtsCCTPLoc = LVL1::DEFAULT_MbtsCCTPLocation, 
                    "StoreGate location of MBTSC inputs" );
   declareProperty( "BcmCTPLocation", m_bcmCTPLoc = LVL1::DEFAULT_BcmCTPLocation, 
                    "StoreGate location of BCM inputs" );
   declareProperty( "LucidCTPLocation", m_lucidCTPLoc = LVL1::DEFAULT_LucidCTPLocation, 
                    "StoreGate location of LUCID inputs" );
   declareProperty( "ZdcCTPLocation", m_zdcCTPLoc = LVL1::DEFAULT_ZdcCTPLocation, 
                    "StoreGate location of ZDC inputs" );
   declareProperty( "BptxCTPLocation", m_bptxCTPLoc = LVL1::DEFAULT_BptxCTPLocation, 
                    "StoreGate location of BPTX inputs" );
   declareProperty( "NimCTPLocation", m_nimCTPLoc = LVL1::DEFAULT_NimCTPLocation, 
                    "StoreGate location of NIM inputs" );

   // Properties setting up the locations of the output objects:
   declareProperty( "RoIOutputLocation", m_roiOutputLoc = LVL1CTP::DEFAULT_CTPSLinkLocation, "StoreGate location of CTP RoI");
   declareProperty( "RoIOutputLocation_Rerun", m_roiOutputLoc_Rerun = LVL1CTP::DEFAULT_CTPSLinkLocation_Rerun, "StoreGate location of rerun CTP RoI");
   declareProperty( "RDOOutputLocation", m_rdoOutputLoc = LVL1CTP::DEFAULT_RDOOutputLocation, "StoreGate location of CTP RDO" );
   declareProperty( "RDOOutputRerunLocation", m_rdoOutputLoc_Rerun = LVL1CTP::DEFAULT_RDOOutputLocation_Rerun, "StoreGate location of rerun CTP RDO" );

   // Properties for adding readout window and trigger offsets (as in data)
   declareProperty("ReadoutWindow", m_readoutWindow = 1, 
                   "Size of readout window in BCs to be introduced into the bytestream data.");
   declareProperty("IntroduceArtificialTriggerOffsets", m_introduceArtificialTriggerOffsets = false, 
                   "(De-)Activate artificial trigger offsets");
   declareProperty("OffsetConfigFile", m_offsetConfigFile = "TriggerOffsets.cfg", 
                   "Configuration file for trigger offsets");

   // declare monitoring tools
   declareProperty( "AthenaMonTools",  m_monitors, "List of monitoring tools to be run withi this instance, if incorrect then tool is silently skipped.");

   // declare monitoring variables
   declareMonitoredCustomVariable("PIT", new CustomBit(this, &ResultBuilder::pit)); // custom monitoring: PIT
   declareMonitoredCustomVariable("TBP", new CustomBit(this, &ResultBuilder::tbp)); // custom monitoring: TBP
   declareMonitoredCustomVariable("TAP", new CustomBit(this, &ResultBuilder::tap)); // custom monitoring: TAP
   declareMonitoredCustomVariable("TAV", new CustomBit(this, &ResultBuilder::tav)); // custom monitoring: TAV
}

LVL1CTP::CTPSimulation::~CTPSimulation()
{}

StatusCode
LVL1CTP::CTPSimulation::initialize() {

   ATH_MSG_INFO("Initializing " << name()
                << " - package version " << PACKAGE_VERSION);

   //
   // Set up the logger object:
   //
   MsgWriter::instance()->setName( this->name() );

   //
   // Print system info
   //
   if (m_doCalo == false) {
      ATH_MSG_INFO("Inputs from LVL1 Calo systems switched off");
   }
   if (m_doMBTS == false) {
      ATH_MSG_INFO("Inputs from LVL1 MBTS systems switched off");
   } 
   if (m_doMuon == false) {
      ATH_MSG_INFO("Inputs from LVL1 Muon systems switched off");
   }
   if (m_doBCM == false) {
      ATH_MSG_INFO("Inputs from LVL1 BCM systems switched off");
   }
   if (m_doLUCID == false) {
      ATH_MSG_INFO("Inputs from LVL1 LUCID systems switched off");
   }
   if (m_doZDC == false) {
      ATH_MSG_INFO("Inputs from LVL1 ZDC systems switched off");
   }
   if (m_doNIM == false) {
      ATH_MSG_INFO("Inputs from LVL1 NIM systems switched off");
   }
   if (m_doBPTX == false) {
      ATH_MSG_INFO("Inputs from LVL1 BPTX systems switched off");
   }
   if (m_doPSCL == false) {
      ATH_MSG_INFO("Inputs from LVL1 PSCL systems switched off");
   }
   if (m_doRNDM == false) {
      ATH_MSG_INFO("Inputs from LVL1 RNDM systems switched off");
   }

   m_extractFunction = &CTPSimulation::extractMultiplicities;

   //
   // Connect to the TrigConfigSvc for the trigger configuration:
   //
   StatusCode sc = m_configSvc.retrieve();
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR("Couldn't connect to " << m_configSvc.typeAndName());
      return sc;
   } else {
      ATH_MSG_DEBUG("Connected to " << m_configSvc.typeAndName());
   }

   // Connect to Random Number Service
   if(m_doRNDM == true){
      sc = m_rndmSvc.retrieve();
      if ( sc.isFailure()) {
         ATH_MSG_ERROR("Couldn't connect to " << m_rndmSvc.typeAndName());
         return sc;
      } else {
         ATH_MSG_DEBUG("Connected to " << m_rndmSvc.typeAndName());
      }
   }

   // Connect to Monitoring Service
   sc = m_monitors.retrieve();
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR("Monitoring tools not initialized: " << m_monitors);
      return sc;
   } else {
      ATH_MSG_DEBUG("Connected to " << m_monitors.typesAndNames());
   }





   // 
   // Find configuration file with trigger offsets
   // 
   if(m_introduceArtificialTriggerOffsets) {
      if( m_offsetConfigFile.find("/")==std::string::npos ){
         const std::string packagePrefix("TrigT1CTP/");
         m_offsetConfigFile.insert(0,packagePrefix);
      }
      m_offsetConfigFile = PathResolverFindDataFile(m_offsetConfigFile);
      if (m_offsetConfigFile.empty()){
         ATH_MSG_WARNING("Trigger offset configuration file not found!\n"
                         << "Size of readout window will be 1");
      }
   } else {
      m_offsetConfigFile = "";
   }
    
   if (m_applyBunchGroup == true) { 
      // registering callback for bunch group settings
      const DataHandle<AthenaAttributeList>  m_dataHandle;
      if(detStore()->contains<AthenaAttributeList>(m_BunchGroupLoc)){
         ATH_MSG_DEBUG("Registering callback");
         if (StatusCode::SUCCESS!=detStore()->regFcn(&CTPSimulation::callback, this,
                                                     m_dataHandle, m_BunchGroupLoc))
            ATH_MSG_DEBUG("Cannot register callback for bunchgroup content");
      }
      else{
         if (StatusCode::SUCCESS!=LoadBunchGroups())
            ATH_MSG_DEBUG("Unable to correctly load bunch groups");
      }
   }
   return StatusCode::SUCCESS;
}


    
StatusCode
LVL1CTP::CTPSimulation::callback(IOVSVC_CALLBACK_ARGS_P(/*idx*/,/*keys*/)) {
   ATH_MSG_DEBUG("Overriding bunch group settings with new bunch group");
   if (StatusCode::SUCCESS!=LoadBunchGroups())
      ATH_MSG_DEBUG("Unable to correctly load bunch groups in callback");
   return StatusCode::SUCCESS;
}


  
StatusCode
LVL1CTP::CTPSimulation::finalize() {
    
   ATH_MSG_INFO("Finalizing " << name()
                << " - package version " << PACKAGE_VERSION);
    
   //
   // monitoring
   //
   ToolHandleArray<IMonitorToolBase>::iterator it;
   for ( it = m_monitors.begin(); it != m_monitors.end(); ++it ) {
      (*it)->finalHists().ignore();
   }

    //
    // clean up
    //
    if (m_decisionMap) { delete m_decisionMap; m_decisionMap = 0; }
    if (m_itemMap) { delete m_itemMap; m_itemMap = 0; }

   for (InternalTriggerMap::iterator iter = m_internalTrigger.begin(); iter != m_internalTrigger.end(); ++iter) {
      delete iter->second;
   }

   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::CTPSimulation::start() {

   ATH_MSG_DEBUG("Start");

   //
   // monitoring
   //
   ToolHandleArray<IMonitorToolBase>::iterator it;
   for ( it = m_monitors.begin(); it != m_monitors.end(); ++it ) {
      if ( (*it)->bookHists().isFailure() ) {
         ATH_MSG_WARNING("Monitoring tool: " <<  (*it)
                         << " in Algo: " << name()
                         << " can't book histograms successfully, remove it or fix booking problem");
         return StatusCode::FAILURE;
      } else {
         ATH_MSG_DEBUG("Monitoring tool: " <<  (*it) 
                       << " in Algo: " << name() << " bookHists successful");
      }
   }
    
   return StatusCode::SUCCESS;
}


StatusCode
LVL1CTP::CTPSimulation::beginRun() {
   ATH_MSG_INFO("beginRun()");

   // get random engine
   CLHEP::HepRandomEngine* rndmEngine=0;

   if(m_doRNDM == true){
      rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
      if (rndmEngine == 0) {
         ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
         return StatusCode::FAILURE;
      }
      
      const TrigConf::Random random(m_configSvc->ctpConfig()->random());
      
      ATH_MSG_DEBUG("Random trigger definition: " << random.name() 
                    << std::setw(8) << random.rate1() << std::setw(8) << random.rate2());
      
      if (random.rate1() < 0) {
         ATH_MSG_INFO("Rate factor for random trigger RNDM0 below zero (" << random.rate1() << "): only possible in simulation");
      }
      
      unsigned int rate1 = (0x1 << (8+random.rate1())) - 1;
      ATH_MSG_DEBUG("REGTEST - Rate for random trigger RNDM0: " << rate1 << " / " << 40080./rate1 << " Hz");
      m_internalTrigger[ make_pair(TrigConf::L1DataDef::RNDM,0)] = new RandomTrigger(0, rate1, rndmEngine);
      
      if (random.rate2() < 0) {
         ATH_MSG_INFO("Rate factor for random trigger RNDM1 below zero (" << random.rate2() << "): only possible in simulation");
      }
      
      unsigned int rate2 = (0x1 << (8+random.rate2())) - 1;
      ATH_MSG_DEBUG("REGTEST - Rate for random trigger RNDM1: " << rate2 << " / " << 40080./rate2 << " Hz");
      m_internalTrigger[ make_pair(TrigConf::L1DataDef::RNDM,1)] = new RandomTrigger(1, rate2, rndmEngine);
   }
    
   if(m_doPSCL == true){
      const TrigConf::PrescaledClock prescaledClock(m_configSvc->ctpConfig()->prescaledClock());
      
      ATH_MSG_DEBUG("REGTEST - Prescaled clock trigger definition: " << prescaledClock.name() 
                    << std::setw(8) << prescaledClock.clock1() << std::setw(8) << prescaledClock.clock2());	
      if (prescaledClock.clock1() > 0) {
         m_internalTrigger[ make_pair(TrigConf::L1DataDef::PCLK,0)] = new PrescaledClockTrigger(0, prescaledClock.clock1());
      } else {
         ATH_MSG_WARNING("No prescaled clock trigger PCLK0 defined.");
      }
      if (prescaledClock.clock2() > 0) {
         m_internalTrigger[ make_pair(TrigConf::L1DataDef::PCLK,1)] = new PrescaledClockTrigger(1, prescaledClock.clock2());
      } else {
         ATH_MSG_WARNING("No prescaled clock trigger PCLK0 defined.");
      }
   }

   if (m_applyDeadTime == true) { // dead time simulation
      ATH_MSG_WARNING("Deadtime not implemented!");
      
      const TrigConf::DeadTime deadTime(m_configSvc->ctpConfig()->deadTime());
      ATH_MSG_DEBUG("Config:" << " " << deadTime.simple()
                    << " " << deadTime.complex1Level() << " " << deadTime.complex1Rate()
                    << " " << deadTime.complex2Level() << " " << deadTime.complex2Rate());
   }

   if (m_doMBTS) {
      const TrigConf::ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
      if (thresholdConfig == 0) {
         m_doMBTS = false;
         ATH_MSG_WARNING("Can't get threshold configuration from Trig Config Service");
      }
      else {
         const std::vector<TrigConf::TriggerThreshold*>& thresholds = thresholdConfig->getThresholdVector(L1DataDef::MBTS);
         unsigned int threshold_a(0), threshold_c(0);
         BOOST_FOREACH( TrigConf::TriggerThreshold* thr, thresholds) {
            ATH_MSG_DEBUG("MBTS Threshold name=" << thr->name());
            if ( thr->name() == "MBTS_A") ++threshold_a;
            if ( thr->name() == "MBTS_C") ++threshold_c;
         }
	
         if (threshold_a == 0 || threshold_c == 0) {
            m_doMBTS = false;
            ATH_MSG_INFO("MBTS thresholds not set properly:"
                         << " #a = " << threshold_a << " #c = " << threshold_c);
         }
	
         if (m_doMBTSSI){
            std::vector<TrigConf::TriggerThreshold*> si_thresholds = thresholdConfig->getThresholdVector(L1DataDef::MBTSSI);
            unsigned int thresholds_a(0), thresholds_c(0);
            for (std::vector<TrigConf::TriggerThreshold*>::iterator iter(si_thresholds.begin()); iter != si_thresholds.end(); ++iter) {
               ATH_MSG_DEBUG("MBTSSI Threshold name=" << (*iter)->name());
               for (unsigned int i=0;i!=16;i++){
                  std::ostringstream osa;
                  osa <<"MBTS_A"<<i;
                  if ((*iter)->name() == osa.str()) 
                     thresholds_a++;
                  std::ostringstream osc;
                  osc <<"MBTS_C"<<i;
                  if ((*iter)->name() == osc.str()) 
                     thresholds_c++;
               }
            }
            if (thresholds_a != 16 || thresholds_c != 16) {
               m_doMBTSSI = false;
               ATH_MSG_INFO("MBTSSI thresholds not set properly:"
                            << " #a = " << thresholds_a << " #c = " << thresholds_c);
            }
         }
         if (m_doMBTS == false) {
            ATH_MSG_WARNING("Inputs from LVL1 MBTS systems switched off (system not properly configured)");
         } 
      }
   }

   if (m_doBCM) {
      const TrigConf::ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
      if (thresholdConfig == 0) {
         m_doBCM = false;
         ATH_MSG_WARNING("Can't get threshold configuration from Trig Config Service");
      } 

      else {
         std::vector<TrigConf::TriggerThreshold*> thresholds = thresholdConfig->getThresholdVector(L1DataDef::BCM);
         unsigned int threshold(0);
         for (std::vector<TrigConf::TriggerThreshold*>::iterator iter(thresholds.begin());
              iter != thresholds.end(); ++iter) {
            ATH_MSG_DEBUG("BCM Threshold name=" << (*iter)->name());
	  
            ++threshold;
         }
	
         thresholds = thresholdConfig->getThresholdVector(L1DataDef::BCMCMB);
         for (std::vector<TrigConf::TriggerThreshold*>::iterator iter(thresholds.begin());
              iter != thresholds.end(); ++iter) {
            ATH_MSG_DEBUG("BCM Threshold name=" << (*iter)->name());

            ++threshold;
         }

         if (threshold == 0) {
            m_doBCM = false;
            ATH_MSG_INFO("no BCM threshold found: " << threshold);
         }
      }
      if (m_doBCM == false) {
         ATH_MSG_WARNING("Inputs from LVL1 BCM systems switched off (system not properly configured)");
      } 
   }

   if (m_doLUCID) {
      const TrigConf::ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
      if (thresholdConfig == 0) {
         m_doLUCID = false;
         ATH_MSG_WARNING("Can't get threshold configuration from Trig Config Service");
      } 
      
      else {
         std::vector<TrigConf::TriggerThreshold*> thresholds = thresholdConfig->getThresholdVector(L1DataDef::LUCID);
         unsigned int threshold(0);
         for (std::vector<TrigConf::TriggerThreshold*>::iterator iter(thresholds.begin());
              iter != thresholds.end(); ++iter) {
            ATH_MSG_DEBUG("LUCID Threshold name=" << (*iter)->name());

            ++threshold;
         }

         if (threshold == 0) {
            m_doLUCID = false;
            ATH_MSG_WARNING("no LUCID threshold found: " << threshold);
         }
      }
      if (m_doLUCID == false) {
         ATH_MSG_WARNING("Inputs from LVL1 LUCID systems switched off (system not properly configured)");
      } 
   }
    
   if (m_doZDC) {
      const TrigConf::ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
      if (thresholdConfig == 0) {
         m_doZDC = false;
         ATH_MSG_WARNING("Can't get threshold configuration from Trig Config Service");
      } 

      else {
         std::vector<TrigConf::TriggerThreshold*> thresholds = thresholdConfig->getThresholdVector(L1DataDef::ZDC);
         unsigned int threshold(0);
         for (std::vector<TrigConf::TriggerThreshold*>::iterator iter(thresholds.begin());
              iter != thresholds.end(); ++iter) {
            ATH_MSG_DEBUG("ZDC Threshold name=" << (*iter)->name());

            ++threshold;
         }

         if (threshold == 0) {
            m_doZDC = false;
            ATH_MSG_WARNING("no ZDC threshold found: " << threshold);
         }
      }
      
      if (m_doZDC == false) {
         ATH_MSG_WARNING("Inputs from LVL1 ZDC systems switched off (system not properly configured)");
      } 
   }

   if (m_doNIM) {
      const TrigConf::ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
      if (thresholdConfig == 0) {

         m_doNIM = false;
         ATH_MSG_WARNING("Can't get threshold configuration from Trig Config Service");
      } 
     
      else {
         std::vector<TrigConf::TriggerThreshold*> thresholds = thresholdConfig->getThresholdVector(L1DataDef::NIM);
         unsigned int threshold(0);
         for (std::vector<TrigConf::TriggerThreshold*>::iterator iter(thresholds.begin());
              iter != thresholds.end(); ++iter) {
            if((*iter)==0) continue;
            ATH_MSG_DEBUG("NIM Threshold name=" << (*iter)->name());

            ++threshold;
         }

         if (threshold == 0) {
            m_doNIM = false;
            ATH_MSG_WARNING("no NIM threshold found: " << threshold);
         }
      }

      if (m_doNIM == false) {
         ATH_MSG_WARNING("Inputs from LVL1 NIM systems switched off (system not properly configured)");
      } 
   }

   if (m_doBPTX) {
      const TrigConf::ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
      if (thresholdConfig == 0) {
         m_doBPTX = false;
         ATH_MSG_WARNING("Can't get threshold configuration from Trig Config Service");
      } 
      
      else {
         std::vector<TrigConf::TriggerThreshold*> thresholds = thresholdConfig->getThresholdVector(L1DataDef::BPTX);
         unsigned int threshold(0);
         for (std::vector<TrigConf::TriggerThreshold*>::iterator iter(thresholds.begin());
              iter != thresholds.end(); ++iter) {
            ATH_MSG_DEBUG("BPTX Threshold name=" << (*iter)->name());

            ++threshold;
         }

         if (threshold == 0) {
            m_doBPTX = false;
            ATH_MSG_WARNING("no BPTX threshold found: " << threshold);
         }
      }
      
      if (m_doBPTX == false) {
         ATH_MSG_WARNING("Inputs from LVL1 BPTX systems switched off (system not properly configured)");
      } 
   }


   //
   // Construct the map between the configuration and decision threshold objects:
   //
   m_decisionMap = new ThresholdMap( m_configSvc->ctpConfig()->menu().thresholdVector(), 
                                     m_configSvc->ctpConfig()->menu().pitVector());

   ATH_MSG_DEBUG("Mapped decision threshold objects to configuration ones");
   ATH_MSG_DEBUG("          |--------------------------------------------------------|");
   ATH_MSG_DEBUG("          |              PIT layout for the thresholds             |");
   ATH_MSG_DEBUG("          |--------------------------------------------------------|");
   ATH_MSG_DEBUG("          |             Name         |   startbit   |    endbit    |");
   ATH_MSG_DEBUG("          |--------------------------------------------------------|");
      
   for( std::vector< TrigConf::TriggerThreshold* >::const_iterator threshold = m_configSvc->ctpConfig()->menu().thresholdVector().begin();
        threshold != m_configSvc->ctpConfig()->menu().thresholdVector().end(); ++threshold ) {
      ATH_MSG_DEBUG("REGTEST - |   " << std::setw( 20 ) << ( *threshold )->name() << "   |   " << std::setw( 8 )
                    << m_decisionMap->decision( *threshold )->pitStart() << "   |   " << std::setw( 8 )
                    << m_decisionMap->decision( *threshold )->pitEnd() << "   |");
   }
   ATH_MSG_DEBUG("          |--------------------------------------------------------|");

   BOOST_FOREACH (InternalTriggerMap::value_type internalThr, m_internalTrigger) {
      ATH_MSG_DEBUG("REGTEST - |   " << std::setw( 20 ) << L1DataDef::typeAsString(internalThr.first.first) << internalThr.first.second << "   |   " 
                    << std::setw( 8 ) << internalThr.second->pit() << "   |   " << setw( 8 ) << internalThr.second->pit() << "   |");
   }

   ATH_MSG_DEBUG("          |--------------------------------------------------------|");

   //
   // Construct the map between configuration and decision trigger item objects:
   //
   m_itemMap = new ItemMap( m_configSvc->ctpConfig()->menu().itemVector(), 
                            m_configSvc->ctpConfig()->prescaleSet(), 
                            ((m_prescaleMode == OFFSET)&&(m_doRNDM)) ? rndmEngine : 0 );
   ATH_MSG_DEBUG("Mapped decision trigger item objects to configuration ones");
   ATH_MSG_DEBUG("          |---------------------------------------------------------------------------------|");
   ATH_MSG_DEBUG("          |                             Layout of trigger items in result                   |");
   ATH_MSG_DEBUG("          |---------------------------------------------------------------------------------|");
   ATH_MSG_DEBUG("          |                                       Name  | Position | Prescale | TriggerType |");
   ATH_MSG_DEBUG("          |---------------------------------------------------------------------------------|");
      
   BOOST_FOREACH( TrigConf::TriggerItem* item, m_configSvc->ctpConfig()->menu().itemVector()) {
      ATH_MSG_DEBUG("REGTEST - |   " << std::setw( 40 ) << item->name() 
                    << "  | " << std::setw( 7 ) << m_itemMap->getItem( item )->itemPos() 
                    << "  | " << std::setw( 7 ) << m_itemMap->getItem( item )->prescale()
                    << "  |       0x" << std::hex << std::setfill('0') << std::setw( 2 ) << item->triggerType() << std::dec << std::setfill(' ')
                    << "  |");
   }
   ATH_MSG_DEBUG("          |---------------------------------------------------------------------------------|");

   // build prescale vector (for monitoring)
   m_prescales = m_configSvc->ctpConfig()->prescaleSet().prescales();

   return StatusCode::SUCCESS;
}


StatusCode
LVL1CTP::CTPSimulation::execute() {
    
   ATH_MSG_DEBUG("Executing CTPSimulation algorithm");

   ////////////////////////////////////////////////////////////////////////////
   //                                                                        //
   //                Extract informations about the event                    //
   //                                                                        //
   ////////////////////////////////////////////////////////////////////////////

   const DataHandle< ::EventInfo > eventInfo;

   StatusCode sc = evtStore()->retrieve( eventInfo );
   if( sc.isFailure() ) {

      ATH_MSG_ERROR("Couldn't retrieve event information from StoreGate");
      ATH_MSG_ERROR("Event information in output will be zeroed");

      EventInfo::instance().clear();

   } 

   else {
      
      ATH_MSG_DEBUG("Retrieved event information from StoreGate");
      
      EventInfo::instance().clear();

      if (eventInfo->event_ID() != 0) {
         const EventID* eventID = eventInfo->event_ID();

         ATH_MSG_DEBUG("Retrieved EventID: " << *eventID);

         EventInfo::instance().setTime( eventID->time_stamp() );
         EventInfo::instance().setTimeNs( eventID->time_stamp_ns_offset() );

         EventInfo::instance().setRunNumber( eventID->run_number() );
         EventInfo::instance().setEventNumber( eventID->event_number() );
         EventInfo::instance().setBCID( eventID->bunch_crossing_id() );
      } 
      else {
         ATH_MSG_INFO("Event ID part of event information has zero pointer");
      }

      if (eventInfo->trigger_info() != 0) {
         const TriggerInfo* triggerInfo = eventInfo->trigger_info();

         ATH_MSG_DEBUG("Retrieved TriggerInfo: " << *triggerInfo);

         EventInfo::instance().setExtendedL1ID( triggerInfo->extendedLevel1ID() );
         if (triggerInfo->level1TriggerType() != 0) {
            ATH_MSG_DEBUG("TriggerType not zero: "
                          << "0x" << std::hex << std::setw(8) << std::setfill('0') << eventInfo->trigger_info()->level1TriggerType() 
                          << std::dec << " Will be overwritten.");
         }

         EventInfo::instance().setEventType( 0 );
         if (triggerInfo->level1TriggerInfo().size() != 0) {
            ATH_MSG_DEBUG("TriggerInfo not zero: " << std::hex << std::setfill('0'));
            for (size_t i(0); i < triggerInfo->level1TriggerInfo().size(); ++i) {
               ATH_MSG_DEBUG("0x" << std::setw(8) << triggerInfo->level1TriggerInfo()[i]);
            }
            ATH_MSG_DEBUG(std::dec << " Will be overwritten.");
	  
         }
      } 
      else {
         ATH_MSG_INFO("Trigger Info part of event information has zero pointer");
      }
      ATH_MSG_DEBUG("EventInfo: " + EventInfo::instance().dump());
   }


   ////////////////////////////////////////////////////////////////////////////
   //                                                                        //
   //                   Retrieve the inputs to the CTP                       //
   //                                                                        //
   ////////////////////////////////////////////////////////////////////////////

   // Get the input from the MuCTPI:
   if (m_doMuon) {
      StatusCode sc = evtStore()->retrieve( m_muctpiCTP, m_muonCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve input from MuCTPI from StoreGate");
         ATH_MSG_WARNING("Setting muon inputs to CTP to zero");
      } else {
         ATH_MSG_DEBUG("Retrieved input from MuCTPI from StoreGate");
         ATH_MSG_DEBUG("MuCTPI word is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_muctpiCTP->muCTPIWord());
      }
   }

   // Get the Calo info:
   if (m_doCalo) {
      // Get the egamma input from calo:
      StatusCode sc = evtStore()->retrieve( m_emtauCTP, m_emTauCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve EM-TAU inputs from LVL1 calo simulation from StoreGate");
         ATH_MSG_WARNING("Setting calo EM-TAU inputs to CTP to zero");
      } else {
         ATH_MSG_DEBUG("Retrieved EM-TAU inputs from LVL1 calo simulation from StoreGate");
         ATH_MSG_DEBUG("EM-TAU cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_emtauCTP->cableWord0());
         ATH_MSG_DEBUG("EM-TAU cable word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_emtauCTP->cableWord1());
      }
      
      // Get the jet input from calo:
      sc = evtStore()->retrieve( m_jetCTP, m_jetCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve jet inputs from LVL1 calo simulation from StoreGate");
         ATH_MSG_WARNING("Setting calo jet inputs to CTP to zero");
      } else {
         ATH_MSG_DEBUG("Retrieved jet inputs from LVL1 calo simulation from StoreGate");
         ATH_MSG_DEBUG("Jet cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_jetCTP->cableWord0());
         ATH_MSG_DEBUG("Jet cable word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_jetCTP->cableWord1());
      }

      // Get the energy input from calo:
      sc = evtStore()->retrieve( m_energyCTP, m_energyCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve energy input from LVL1 calo simulation from StoreGate");
         ATH_MSG_WARNING("Setting calo energy input to CTP to zero");
      } else {
         ATH_MSG_DEBUG("Retrieved energy input from LVL1 calo simulation from StoreGate");
         ATH_MSG_DEBUG("Energy cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_energyCTP->cableWord0());
      }

      if (m_doMBTS) {
         // Get forward and backward input from MBTS: (two cable words)
         sc = evtStore()->retrieve( m_mbtsACTP, m_mbtsACTPLoc );
         if ( sc.isFailure() ) {
            ATH_MSG_WARNING("Couldn't retrieve input from LVL1 MBTSA simulation from StoreGate");
            ATH_MSG_WARNING("Setting MBTSA input to CTP to zero");
         } else {
            ATH_MSG_DEBUG("Retrieved input from LVL1 MBTSA simulation from StoreGate");
            ATH_MSG_DEBUG("MBTSA cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_mbtsACTP->cableWord0());
         }

         sc = evtStore()->retrieve( m_mbtsCCTP, m_mbtsCCTPLoc );
         if ( sc.isFailure() ) {
            ATH_MSG_WARNING("Couldn't retrieve input from LVL1 MBTSC simulation from StoreGate");
            ATH_MSG_WARNING("Setting MBTSC input to CTP to zero");
         } else {
            ATH_MSG_DEBUG("Retrieved input from LVL1 MBTSC simulation from StoreGate");
            ATH_MSG_DEBUG("MBTSC cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_mbtsCCTP->cableWord0());
         }
      }

   } // if (m_doCalo)
    
   if (m_doBCM) {
      // Get trigger input from BCM
      sc = evtStore()->retrieve( m_bcmCTP, m_bcmCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve input from LVL1 BCM simulation from StoreGate");
         ATH_MSG_WARNING("Setting BCM input to CTP to zero");
      } else {
         ATH_MSG_DEBUG("Retrieved input from LVL1 BCM simulation from StoreGate");
         ATH_MSG_DEBUG("BCM cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_bcmCTP->cableWord0());
      }
   }

   if (m_doLUCID) {
      // Get trigger input from LUCID
      sc = evtStore()->retrieve( m_lucidCTP, m_lucidCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve input from LVL1 LUCID simulation from StoreGate");
         ATH_MSG_WARNING("Setting LUCID input to CTP to zero");
      } else {
         ATH_MSG_DEBUG("Retrieved input from LVL1 LUCID simulation from StoreGate");
         ATH_MSG_DEBUG("LUCID cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_lucidCTP->cableWord0());
      }
   }

   if (m_doZDC) {
      // Get trigger input from ZDC
      sc = evtStore()->retrieve( m_zdcCTP, m_zdcCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve input from LVL1 ZDC simulation from StoreGate");
         ATH_MSG_WARNING("Setting ZDC input to CTP to zero");
      } else {
         ATH_MSG_DEBUG("Retrieved input from LVL1 ZDC simulation from StoreGate");
         ATH_MSG_DEBUG("ZDC cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_zdcCTP->cableWord0());
      }
   }

   if (m_doNIM) {
      // Get trigger input from NIM
      sc = evtStore()->retrieve( m_nimCTP, m_nimCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve input from LVL1 NIM simulation from StoreGate");
         ATH_MSG_WARNING("Setting NIM input to CTP to zero");
      } 
      
      else {
         ATH_MSG_DEBUG("Retrieved input from LVL1 NIM simulation from StoreGate");
	
         ATH_MSG_DEBUG("NIM cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_nimCTP->cableWord0());
         ATH_MSG_DEBUG("NIM cable word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_nimCTP->cableWord1());
         ATH_MSG_DEBUG("NIM cable word 2 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_nimCTP->cableWord2());
      }
   }
    
   if (m_doBPTX) {
      // Get trigger input from BPTX
      sc = evtStore()->retrieve( m_bptxCTP, m_bptxCTPLoc );
      if ( sc.isFailure() ) {
         ATH_MSG_WARNING("Couldn't retrieve input from LVL1 BPTX simulation from StoreGate");
         ATH_MSG_WARNING("Setting BPTX input to CTP to zero");
      } else {
         ATH_MSG_DEBUG("Retrieved input from LVL1 BPTX simulation from StoreGate");
         ATH_MSG_DEBUG("BPTX cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_bptxCTP->cableWord0());
      }
   }

   ////////////////////////////////////////////////////////////////////////////
   //                                                                        //
   //                        Create the CTP decision                         //
   //                                                                        //
   ////////////////////////////////////////////////////////////////////////////

   //
   // Zero all the multiplicities:
   //
   m_decisionMap->clear();

   //
   // Extract the multiplicities sent to the CTP:
   //
   sc = ( this->*m_extractFunction ) ();
   if ( sc.isFailure() ) {
      ATH_MSG_ERROR("Error in extracting threshold multiplicities");
      return sc;
   }

   //
   // Set up the objects for creating the decision:
   //
   for (InternalTriggerMap::iterator iter = m_internalTrigger.begin(); iter != m_internalTrigger.end(); ++iter) {
      iter->second->print();
      //use bcid for BGRP triggers when running over data
      
      if((m_IsData)&&(!iter->second->name().find("BGRP"))){
         ATH_MSG_DEBUG("Rederiving bunch group trigger decision with bcid " << EventInfo::instance().bcid());
         sc = iter->second->execute((int)EventInfo::instance().bcid());
      }

      else
         sc = iter->second->execute();
      
      if (sc.isFailure()) {
         ATH_MSG_ERROR("Error while executing internal trigger simulation " << iter->second->name());
      }
      ATH_MSG_DEBUG("REGTEST -  " << iter->second->print());
   }

  
   if ((m_doRNDM)&&(m_prescaleMode == RANDOM)){
      int event = EventInfo::instance().runNumber()+1;
      int run = EventInfo::instance().eventNumber()+1;
      ATH_MSG_DEBUG("Random Prescale Seeds " << event << " " << run);
      
      m_itemMap->updatePrescales( m_decisionMap, &m_internalTrigger, m_rndmSvc->setOnDefinedSeeds(event, run, this->name()) );
   }
   else {
      m_itemMap->updatePrescales( m_decisionMap, &m_internalTrigger );
   }

   //
   // Create the output objects:
   //
   m_resultBuilder = new ResultBuilder( m_configSvc->ctpConfig(), m_decisionMap, m_itemMap, &m_internalTrigger, m_readoutWindow);

   if (m_introduceArtificialTriggerOffsets) m_resultBuilder->readOffsetConfig(m_offsetConfigFile);

   // build extended level1 ID if not set before
   if (EventInfo::instance().extendedL1ID() == 0) {
      EventInfo::instance().setExtendedL1ID(m_resultBuilder->extendedLevel1ID());
   }
    
   // build CTP output objects
   const CTPSLink* roi_output = m_resultBuilder->constructRoIResult();
   const CTP_RDO*  rdo_output = m_resultBuilder->constructRDOResult();

   ////////////////////////////////////////////////////////////////////////////
   //                                                                        //
   //                        Store the CTP decision                          //
   //                                                                        //
   ////////////////////////////////////////////////////////////////////////////

  
   if(m_IsData){
      
      sc = evtStore()->record( rdo_output, m_rdoOutputLoc_Rerun );
      if( sc.isFailure() ) {
         ATH_MSG_ERROR("Couldn't store CTP_RDO_Rerun object in StoreGate with key: " << m_rdoOutputLoc_Rerun);
         return sc;
      } 
      else {
         ATH_MSG_DEBUG("Stored CTP_RDO_Rerun object in StoreGate with key: " << m_rdoOutputLoc_Rerun);
         ATH_MSG_DEBUG("Dump CTP_RDO_Rerun object: " << rdo_output->dump());
      }

      sc = evtStore()->record( roi_output, m_roiOutputLoc_Rerun );
      if( sc.isFailure() ) {
         ATH_MSG_ERROR("Couldn't store CTPSLink_Rerun object in StoreGate with key " << m_roiOutputLoc_Rerun);
         return sc;
      } 
      else {
         ATH_MSG_DEBUG("Stored CTPSLink_Rerun object in StoreGate with key: " << m_roiOutputLoc_Rerun);
         ATH_MSG_DEBUG("Dump CTPSLink_Rerun object: " << roi_output->dump());
      }
   }
    
   else if(!m_IsData){ //if this is MC
      
      sc = evtStore()->record( rdo_output, m_rdoOutputLoc );
      if( sc.isFailure() ) {
         ATH_MSG_ERROR("Couldn't store CTP_RDO object in StoreGate with key: " << m_rdoOutputLoc);
         return sc;
      } 
      else {
         ATH_MSG_DEBUG("Stored CTP_RDO object in StoreGate with key: " << m_rdoOutputLoc);
         ATH_MSG_DEBUG("Dump CTP_RDO object: " << rdo_output->dump());
      }

      sc = evtStore()->record( roi_output, m_roiOutputLoc );
      if( sc.isFailure() ) {
         ATH_MSG_ERROR("Couldn't store CTPSLink object in StoreGate with key " << m_roiOutputLoc);
         return sc;
      } 
      else {
         ATH_MSG_DEBUG("Stored CTPSLink object in StoreGate with key: " << m_roiOutputLoc);
         ATH_MSG_DEBUG("Dump CTPSLink object: " << roi_output->dump());
      }
      
      ////////////////////////////////////////////////////////////////////////////
      //                                                                        //
      //              Update TriggerInfo in EventInfo object (SG) (only for MC) //
      //                                                                        //
      ////////////////////////////////////////////////////////////////////////////
      
      // get EventInfo
      const ::EventInfo* constEventInfo(0);
      sc = evtStore()->retrieve(constEventInfo);
      if (sc.isFailure()) {
         ATH_MSG_FATAL("Can't get EventInfo object for update of TriggerInfo (LVL1 part)");
         return sc;
      }
      
      ATH_MSG_DEBUG("Updating TriggerInfo of EventInfo object");
      
      ::EventInfo* updateEventInfo = const_cast< ::EventInfo* >(constEventInfo);
      TriggerInfo* triggerInfo = updateEventInfo->trigger_info();
      
      // update the TriggerInfo part related to LVL1
      uint32_t lvl1Id = 0;
      uint32_t lvl1Type = 0;
      std::vector<uint32_t> lvl1Info;
      
      if ( triggerInfo != 0 ) {                           // get old information
         lvl1Id   = triggerInfo->extendedLevel1ID();
         lvl1Type = triggerInfo->level1TriggerType();
         lvl1Info = triggerInfo->level1TriggerInfo();
	
         ATH_MSG_DEBUG("Saved old TriggerInfo: " << *triggerInfo);
      } 
      
      else {                                           // create trigger info
         triggerInfo = new TriggerInfo();
         updateEventInfo->setTriggerInfo(triggerInfo);
	
         ATH_MSG_DEBUG("Created new TriggerInfo");
      }
      
      // maybe overwrite extended lvl1 id
      if (lvl1Id == 0) {
         lvl1Id = m_resultBuilder->extendedLevel1ID();
         ATH_MSG_DEBUG("Updated extendedLevel1ID: " << lvl1Id);
      }
      
      // overwrite trigger type
      lvl1Type = m_resultBuilder->triggerType();
      ATH_MSG_DEBUG("Updated level1TriggerType: " << lvl1Type);
      
      // overwrite lvl1 trigger result
      lvl1Info = m_resultBuilder->tbp();
      for (size_t i(0); i < m_resultBuilder->tap().size(); ++i) {
         lvl1Info.push_back(m_resultBuilder->tap()[i]);   // TAP
      }
      for (size_t i(0); i < m_resultBuilder->tav().size(); ++i) {
         lvl1Info.push_back(m_resultBuilder->tav()[i]);   // TAV
      }

      ATH_MSG_DEBUG("Updated level1TriggerInfo: " << CTPUtil::printHex(lvl1Info));
      
      // store updated information in TriggerInfo/EventInfo
      triggerInfo->setExtendedLevel1ID(lvl1Id);
      triggerInfo->setLevel1TriggerType(lvl1Type);
      triggerInfo->setLevel1TriggerInfo(lvl1Info);
      
      ATH_MSG_DEBUG("Updated TriggerInfo attached to EventInfo: " 
                    << *updateEventInfo->trigger_info());
   }

   //
   // do monitoring
   //
   ToolHandleArray<IMonitorToolBase>::iterator it;
   for ( it = m_monitors.begin(); it != m_monitors.end(); ++it ) {
      if (!(*it)->preSelector()) (*it)->fillHists().ignore();
   }

   if (m_resultBuilder) { delete m_resultBuilder; m_resultBuilder = 0;}
    
   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::CTPSimulation::extractMultiplicities() {
   
   BOOST_FOREACH ( TrigConf::TriggerThreshold* thr, m_configSvc->ctpConfig()->menu().thresholdVector() ) {

      int multiplicity = 0;

      if ( thr->cableName() == "" ) { 
         ATH_MSG_DEBUG("No cable name for what must be an internal threshold " << thr->name()); 
         continue; 
      } 

      if ( thr->cableName() == "MU" ) {

         if ( m_muctpiCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_muctpiCTP->muCTPIWord(), thr->cableStart(), thr->cableEnd() );
         }

      } 
      
      else if ( thr->cableName() == "CP1" ) {

         if ( m_emtauCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_emtauCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         }
	
      } 

      else if ( thr->cableName() == "CP2" ) {

         if ( m_emtauCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_emtauCTP->cableWord1(), thr->cableStart(), thr->cableEnd() );
         }
	
      } 

      else if ( thr->cableName() == "JEP1" ) {

         if ( m_jetCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_jetCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         }

      } 

      else if ( thr->cableName() == "JEP2" ) {

         if ( m_jetCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_jetCTP->cableWord1(), thr->cableStart(), thr->cableEnd() );
         }
	
      } 

      else if ( thr->cableName() == "JEP3" ) {
	
         if ( m_energyCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_energyCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         }

      } 
      
      else if ( thr->cableName() == TrigConf::L1DataDef::nimType() ) {
	
         if ( thr->type() == TrigConf::L1DataDef::mbtsType() ) {
            // Don't know what to do in place of string matching
            int n_a=thr->name().find("MBTS_A");
            int n_c=thr->name().find("MBTS_C");
            if (m_mbtsACTP.isValid() && n_a==0) {
               multiplicity = CTPUtil::getMult( m_mbtsACTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
               ATH_MSG_DEBUG("Extracted multi is: "<< multiplicity);
            }
            if (m_mbtsCCTP.isValid() && n_c==0) {
               multiplicity = CTPUtil::getMult( m_mbtsCCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
               ATH_MSG_DEBUG("Extracted multi is: "<< multiplicity);
            }
         }
	
         else if ( thr->type() == TrigConf::L1DataDef::mbtssiType() ) {
            int n_a = thr->name().find("MBTS_A");
            if (m_mbtsACTP.isValid() && n_a==0) {
               multiplicity = CTPUtil::getMult( m_mbtsACTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
               ATH_MSG_DEBUG("Extracted multi is: " << multiplicity);
            }
            int n_c = thr->name().find("MBTS_C");
            if (m_mbtsCCTP.isValid() && n_c==0) {
               multiplicity = CTPUtil::getMult( m_mbtsCCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
               ATH_MSG_DEBUG("Extracted multi is: "<<multiplicity);
            }	  	  
         }

         else if ( thr->type() == TrigConf::L1DataDef::bcmType()) {

            if ( m_bcmCTP.isValid() ) {
               multiplicity = CTPUtil::getMult( m_bcmCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            }
         }

         else if ( thr->type() == TrigConf::L1DataDef::bcmcmbType()) {

            if ( m_bcmCTP.isValid() ) {
               multiplicity = CTPUtil::getMult( m_bcmCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            }
         } 

         else if ( thr->type() == TrigConf::L1DataDef::lucidType()) {

            if ( m_lucidCTP.isValid() ) {
               multiplicity = CTPUtil::getMult( m_lucidCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            } 
         } 

         else if ( thr->type() == TrigConf::L1DataDef::zdcType()) {

            if ( m_zdcCTP.isValid() ) {
               multiplicity = CTPUtil::getMult( m_zdcCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            } 
         } 
	
         else if (!thr->name().find("BPTX") ) {	
            if ( m_bptxCTP.isValid() ) {
               multiplicity = CTPUtil::getMult( m_bptxCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            }
         }

         else if (!thr->name().find("NIM") ) {	
            if(m_nimCTP.isValid() ) {
               //rather nasty hack but we don't have a separation of NIM types in the data taking
               //manually separates nim slot/connector combinations and matches to cable words
               if ((thr->cableCtpin().find("SLOT8")!= std::string::npos)&& 
                   (thr->cableConnector().find("CON2")!= std::string::npos))
                  multiplicity = CTPUtil::getMult( m_nimCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
	  
               if ((thr->cableCtpin().find("SLOT9")!= std::string::npos)&& 
                   (thr->cableConnector().find("CON0")!= std::string::npos))
                  multiplicity = CTPUtil::getMult( m_nimCTP->cableWord1(), thr->cableStart(), thr->cableEnd() );
	  
               if ((thr->cableCtpin().find("SLOT9")!= std::string::npos)&& 
                   (thr->cableConnector().find("CON1")!= std::string::npos))
                  multiplicity = CTPUtil::getMult( m_nimCTP->cableWord2(), thr->cableStart(), thr->cableEnd() );
            }
         }
	
         else if ( thr->type() == TrigConf::L1DataDef::calreqType()) {
	  
            ATH_MSG_DEBUG("The CALREQ type is ignored offline");
            multiplicity = 0;
         }
      }
      
      else {
	
         ATH_MSG_ERROR("Threshold type " << thr->type() << " for threshold " << thr->name()
                       << " and cable name " << thr->cableName() << " not recognized");
         return StatusCode::FAILURE;
      }
      
      ATH_MSG_DEBUG(" ---> Threshold with name: " << std::setw( 8 ) << thr->name() << " gets multiplicity: "
                    << std::setw( 2 ) << multiplicity);
      m_decisionMap->decision( thr )->setValue( multiplicity );
      
   }
    
   return StatusCode::SUCCESS;
}


StatusCode
LVL1CTP::CTPSimulation::LoadBunchGroups() {
   ATH_MSG_DEBUG("Trying to access bunch group content from COOL");  

   TrigConf::BunchGroupSet *bunchGroupSet;
   const AthenaAttributeList* atrlist;
    
   //loading bunch groups from cool (data scenario)
   if((detStore()->contains<AthenaAttributeList>(m_BunchGroupLoc))&& 
      (StatusCode::SUCCESS==detStore()->retrieve(atrlist, m_BunchGroupLoc))){ 
      
      ATH_MSG_DEBUG("Reading bunch group information from COOL");
      std::vector<TrigConf::BunchGroup> bgs;
      bgs = TrigConfCoolL1PayloadConverters::readLvl1BGContent( *atrlist );
      bunchGroupSet = new TrigConf::BunchGroupSet(bgs);
      //bunchGroupSet->setId( m_lvl1BgKey ); // Update the BunchGroupSet object with the ID
   }
  
   //if folder not avaible from cool (MC scenario) take default bunchgroup settings from configuration
   else{
      ATH_MSG_DEBUG("Could not retrieve bunch group content from COOL, setting to default BunchID found in trigger configuration");
      bunchGroupSet=new TrigConf::BunchGroupSet(m_configSvc->ctpConfig()->bunchGroupSet());
   }
    
   if(!bunchGroupSet){
      ATH_MSG_WARNING("Null pointer for bunch group set returned");  
      return StatusCode::FAILURE;
   }

   //log << MSG::DEBUG << "Dumping bunch group content");  
   //bunchGroupSet->print();

   //translating bunch group set into a CTPSimulation readable format
   const std::vector<TrigConf::BunchGroup> bunchGroups(bunchGroupSet->bunchGroups());
   for (size_t i(0); i < bunchGroups.size(); ++i) {
      std::ostringstream message;
      ATH_MSG_DEBUG("BunchGroup " << i << " Name " << bunchGroups[i].name() << " Bunches:";
                    for (size_t j(0); j < bunchGroups[i].bunches().size(); ++j) message << " " << bunchGroups[i].bunches()[j]);
      ATH_MSG_DEBUG("REGTEST - " << message.str());
   }
    
   if (bunchGroups.empty()) {
      ATH_MSG_WARNING("No bunch group triggers defined.");
   } 

   else {
      ATH_MSG_DEBUG("Defining bunch group internal trigger");
      for (size_t i(0); i < bunchGroups.size(); ++i) {
         InternalTriggerMap::key_type trigtype = make_pair(TrigConf::L1DataDef::BGRP,i);

         //delete previous bunch group settings if there are any
         if(m_internalTrigger.find(trigtype)!=m_internalTrigger.end())
            delete m_internalTrigger[trigtype];
	
         //declare new bunch group trigger
         m_internalTrigger[trigtype] = new BunchGroupTrigger(i, bunchGroups[i].bunches());
      }
   }
   delete bunchGroupSet;
   return StatusCode::SUCCESS;
}


const LVL1CTP::ResultBuilder*
LVL1CTP::CTPSimulation::resultBuilder() const {
   return m_resultBuilder;
}

