/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes:
#include <vector>
#include <iomanip>

#include "TH1.h"
#include "GaudiKernel/ITHistSvc.h"

// Athena/Gaudi includes:
#include "StoreGate/StoreGateSvc.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AthenaMonitoring/IMonitorToolBase.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "PathResolver/PathResolver.h"

// Trigger configuration interface includes:
#include "L1TopoConfig/L1TopoMenu.h"
#include "L1TopoConfig/L1TopoConfigOutputList.h"

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
#include "TrigT1Interfaces/FrontPanelCTP.h"

// CTP DAQ output:
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTPResult.h"
#include "TrigT1Result/CTP_Decoder.h"

// Include for the configuration service:
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/CTPConfig.h"

// Local includes:
#include "TrigT1CTP/CTPSimulation.h"
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/CTPTriggerThreshold.h"
#include "TrigT1CTP/CTPTriggerItemNode.h"
#include "TrigT1CTP/CTPUtil.h"
#include "TrigT1CTP/ResultBuilder.h"
#include "TrigT1CTP/EventInfo.h"
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

#include "boost/lexical_cast.hpp"

using namespace std;
using namespace TrigConf;

namespace LVL1CTP {
	//! enumeration for prescale mode
	enum PrescaleMode { DEFAULT = 0, OFFSET, RANDOM };
}

LVL1CTP::CTPSimulation::CTPSimulation( const std::string& name, ISvcLocator* pSvcLocator ) :
   AthAlgorithm( name, pSvcLocator ), 
   m_histSvc("THistSvc", name),
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
	declareProperty( "CtpVersion",  m_ctpVersion  = 0, "Version of the CTP to be used: 3 - latest version of run-I, 4 - initial version for run-II. For explanation of the other version see CTPfragment/schema/L1CoreSpecifications.xml." );
	declareProperty( "DoL1Topo",  m_doL1Topo  = true, "Use inputs from L1Topo system" );
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
	declareProperty( "PrescaleMode", m_prescaleMode = RANDOM, 
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
	declareProperty( "TopoCTPLocation", m_topoCTPLoc = LVL1::DEFAULT_L1TopoCTPLocation, 
                     "StoreGate location of topo inputs" );
	
	// Properties setting up the locations of the output objects:
	declareProperty( "RoIOutputLocation",       m_roiOutputLoc = LVL1CTP::DEFAULT_CTPSLinkLocation, "StoreGate location of CTP RoI");
	declareProperty( "RoIOutputLocation_Rerun", m_roiOutputLoc_Rerun = LVL1CTP::DEFAULT_CTPSLinkLocation_Rerun, "StoreGate location of rerun CTP RoI");
	declareProperty( "RDOOutputLocation",       m_rdoOutputLoc = LVL1CTP::DEFAULT_RDOOutputLocation, "StoreGate location of CTP RDO" );
	declareProperty( "RDOOutputRerunLocation",  m_rdoOutputLoc_Rerun = LVL1CTP::DEFAULT_RDOOutputLocation_Rerun, "StoreGate location of rerun CTP RDO" );
	
	// Properties for adding readout window and trigger offsets (as in data)
	declareProperty("ReadoutWindow", m_readoutWindow = 1, 
									"Size of readout window in BCs to be introduced into the bytestream data.");
	declareProperty("IntroduceArtificialTriggerOffsets", m_introduceArtificialTriggerOffsets = false, 
									"(De-)Activate artificial trigger offsets");
	declareProperty("OffsetConfigFile", m_offsetConfigFile = "TriggerOffsets.cfg", 
									"Configuration file for trigger offsets");
	
	// declare monitoring tools
	declareProperty( "AthenaMonTools",  m_monitors, "List of monitoring tools to be run within this instance, if incorrect then tool is silently skipped.");
	
	declareProperty("HistBase", m_histbase, "/<stream>/<subdir>");
	
	// declare monitoring variables
	declareMonitoredCustomVariable("TIP", new CustomBit(this, &ResultBuilder::tip)); // custom monitoring: TIP
	// 	declareMonitoredCustomVariable("TBP", new CustomBit(this, &ResultBuilder::tbp)); // custom monitoring: TBP
	// 	declareMonitoredCustomVariable("TAP", new CustomBit(this, &ResultBuilder::tap)); // custom monitoring: TAP
	// 	declareMonitoredCustomVariable("TAV", new CustomBit(this, &ResultBuilder::tav)); // custom monitoring: TAV
	
}

LVL1CTP::CTPSimulation::~CTPSimulation()
{}


StatusCode
LVL1CTP::CTPSimulation::initialize() {
	
   ATH_MSG_INFO("Initializing - package version " << PACKAGE_VERSION);
	
   // TrigConfigSvc for the trigger configuration
   CHECK(m_configSvc.retrieve());

	
   //
   // Set up the logger object:
   //
   MsgWriter::instance()->setName( this->name() );
	
   //
   // Print system info
   //
   if (m_doL1Topo == false) {
      ATH_MSG_INFO("Inputs from L1Topo systems switched off");
   }
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
   if (m_doRNDM == false) {
      ATH_MSG_INFO("Inputs from LVL1 RNDM systems switched off");
   }
	
   m_extractFunction = &CTPSimulation::extractMultiplicities;
	
   // Random Number Service
   if(m_doRNDM == true){
      CHECK(m_rndmSvc.retrieve());
   }
  
   // Monitoring Service
   CHECK(m_monitors.retrieve());

   CHECK(m_histSvc.retrieve());

   // registering with IOVDBSvc to get informed about BG changes
   if (m_applyBunchGroup == true) { 
      // registering callback for bunch group settings
      const DataHandle<AthenaAttributeList>  dataHandle;
      if(detStore()->contains<AthenaAttributeList>(m_BunchGroupLoc)){
         ATH_MSG_DEBUG("Registering callback for " << m_BunchGroupLoc );
         if (StatusCode::SUCCESS!=detStore()->regFcn(&CTPSimulation::callback, this,
                                                     dataHandle, m_BunchGroupLoc))
            ATH_MSG_DEBUG("Cannot register callback for bunchgroup content");
      }
      else{
         if (StatusCode::SUCCESS!=LoadBunchGroups())
            ATH_MSG_DEBUG("Unable to correctly load bunch groups");
      }
   }

   ATH_MSG_DEBUG("Done initializing");
	
   return StatusCode::SUCCESS;
}



StatusCode
LVL1CTP::CTPSimulation::callback(IOVSVC_CALLBACK_ARGS_P(/*idx*/,/*keys*/)) {
   ATH_MSG_DEBUG("Callback for " << m_BunchGroupLoc << ": Overriding bunch group settings with new bunch group");
   if (StatusCode::SUCCESS!=LoadBunchGroups())
      ATH_MSG_DEBUG("Unable to correctly load bunch groups in callback");
   return StatusCode::SUCCESS;
}



StatusCode
LVL1CTP::CTPSimulation::finalize() {
	
	ATH_MSG_INFO("Finalizing " << name() << " - package version " << PACKAGE_VERSION);

    for(const TriggerItem * item : m_configSvc->ctpConfig()->menu().itemVector()) {
       if(item==nullptr) continue;
       ATH_MSG_DEBUG("REGTEST " << item->name() << " TBP " << m_countsBP[item->ctpId()] << " TAP " << m_countsAP[item->ctpId()] << " TAV " << m_countsAV[item->ctpId()]);
    }
	
	// finalize monitoring
	for ( auto & mt : m_monitors ) {
		mt->finalHists().ignore();
	}

	for (auto & mapentry : m_internalTrigger ) {
		delete mapentry.second;
	}
	
	delete m_decisionMap; m_decisionMap = nullptr;
	delete m_itemMap; m_itemMap = nullptr;
  	delete m_ctpDataformat; m_ctpDataformat = nullptr;
	
	return StatusCode::SUCCESS;
}


namespace {
   void setThresholdHistLabels(TH1I * hist, TH1I * histtot, const vector<TriggerThreshold*> & thrV, uint maxMult, uint first=0, uint last=100) {
      uint bin = 1;
      for(const TriggerThreshold * thr: thrV) {
         if(thr->mapping()<0 || (uint)thr->mapping()<first || (uint)thr->mapping()>=last)
            continue;
         for(uint m=0; m<maxMult;++m)
            hist->GetXaxis()->SetBinLabel(bin++, (boost::lexical_cast<string,int>(m) + " x " + thr->name()).c_str() );
      }
      for(const TriggerThreshold * thr: thrV) {
         histtot->GetXaxis()->SetBinLabel(thr->mapping()+1, thr->name().c_str() );
      }
   }
}


StatusCode
LVL1CTP::CTPSimulation::start() {
   ATH_MSG_DEBUG("Start");
   return StatusCode::SUCCESS;
}


StatusCode
LVL1CTP::CTPSimulation::bookHists() {


  string histstream ( m_histbase );
  
  //Check that we didn't already do this, and just return if yes ...
  if (m_histSvc->exists(histstream + "CTPSimulation/L1TopoDecisionCable0")){
    ATH_MSG_INFO(histstream+" histograms already exist, returning ...");
    return StatusCode::SUCCESS;
  }
  

   size_t runNrPos = histstream.find("RUNNR");
   if( runNrPos != string::npos ) {

      string runnr = "0";

      const DataHandle< ::EventInfo> evt;
      const DataHandle< ::EventInfo> evtEnd;
      StatusCode sc = evtStore()->retrieve( evt, evtEnd );
      if( sc.isSuccess() ) {
         runnr = boost::lexical_cast<string, unsigned int>(evt->event_ID()->run_number());
      }
      histstream.replace(runNrPos, 5, runnr);
   }

   const TrigConf::ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();

   // booking histograms
   // Topo input
   m_HistL1TopoDecisionCable0 = new TH1I("L1TopoDecision0","L1Topo Decision Cable 0", 64, 0, 64);
   m_HistL1TopoDecisionCable1 = new TH1I("L1TopoDecision1","L1Topo Decision Cable 1", 64, 0, 64);

   for(const TIP * tip : m_configSvc->ctpConfig()->menu().tipVector() ) {
      if ( tip->tipNumber() < 384 )
         continue;
      unsigned int tipNumber = (unsigned int) ( tip->tipNumber() - 384 );
      switch(tipNumber / 64) {
      case 0:
         m_HistL1TopoDecisionCable0->GetXaxis()->SetBinLabel(1+ tipNumber % 64, tip->thresholdName().c_str() );
         break;
      case 1:
         m_HistL1TopoDecisionCable1->GetXaxis()->SetBinLabel(1+ tipNumber % 64, tip->thresholdName().c_str() );
         break;
      default:
         break;
      }
   }
   

   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/L1TopoDecisionCable0", m_HistL1TopoDecisionCable0));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/L1TopoDecisionCable1", m_HistL1TopoDecisionCable1));

   // threshold input
   m_thrEMTot   = new TH1I("ThrEM", "Total threshold count EM", 16, 0, 16);
   m_thrHATot   = new TH1I("ThrHA", "Total threshold count HA", 16, 0, 16);
   m_thrJETTot  = new TH1I("ThrJET", "Total threshold count JET", 25, 0, 25);
   m_thrMUTot   = new TH1I("ThrMU", "Total threshold count MU", 6, 0, 6);
   m_thrTETot   = new TH1I("ThrTE", "Total threshold count TE", 16, 0, 16);
   m_thrXETot   = new TH1I("ThrXE", "Total threshold count XE", 16, 0, 16);
   m_thrXSTot   = new TH1I("ThrXS", "Total threshold count XS", 8, 0, 8);

   m_thrEMMult   = new TH1I("ThrMultEM", "Cumulative input threshold multiplicity EM", 128, 0, 128);
   m_thrHAMult   = new TH1I("ThrMultHA", "Cumulative input threshold multiplicity HA", 128, 0, 128);
   m_thrJET1Mult = new TH1I("ThrMultJET3b", "Cumulative input threshold multiplicity JET 3b", 80, 0, 80);
   m_thrJET2Mult = new TH1I("ThrMultJET2b", "Cumulative input threshold multiplicity JET 2b", 60, 0, 60);
   m_thrMUMult   = new TH1I("ThrMultMU", "Cumulative input threshold multiplicity MU", 48, 0, 48);
   m_thrTEMult   = new TH1I("ThrMultTE", "Cumulative input threshold multiplicity TE", 32, 0, 32);
   m_thrXEMult   = new TH1I("ThrMultXE", "Cumulative input threshold multiplicity XE", 32, 0, 32);
   m_thrXSMult   = new TH1I("ThrMultXS", "Cumulative input threshold multiplicity XS", 32, 0, 32);

   setThresholdHistLabels(m_thrEMMult, m_thrEMTot, thresholdConfig->getThresholdVector(L1DataDef::EM), 8);
   setThresholdHistLabels(m_thrHAMult, m_thrHATot, thresholdConfig->getThresholdVector(L1DataDef::TAU), 8);
   setThresholdHistLabels(m_thrMUMult, m_thrMUTot, thresholdConfig->getThresholdVector(L1DataDef::MUON), 8);
   setThresholdHistLabels(m_thrJET1Mult, m_thrJETTot, thresholdConfig->getThresholdVector(L1DataDef::JET), 8,0,10);
   setThresholdHistLabels(m_thrJET2Mult, m_thrJETTot, thresholdConfig->getThresholdVector(L1DataDef::JET), 4,10,25);
   setThresholdHistLabels(m_thrTEMult, m_thrTETot, thresholdConfig->getThresholdVector(L1DataDef::TE), 2);
   setThresholdHistLabels(m_thrXEMult, m_thrXETot, thresholdConfig->getThresholdVector(L1DataDef::XE), 2);
   setThresholdHistLabels(m_thrXSMult, m_thrXSTot, thresholdConfig->getThresholdVector(L1DataDef::XS), 2);

   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrEM", m_thrEMTot ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrHA", m_thrHATot ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrJET", m_thrJETTot ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMU", m_thrMUTot ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrTE", m_thrTETot ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrXE", m_thrXETot ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrXS", m_thrXSTot ));

   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMultEM", m_thrEMMult ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMultHA", m_thrHAMult ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMultJET1", m_thrJET1Mult ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMultJET2", m_thrJET2Mult ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMultMU", m_thrMUMult ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMultTE", m_thrTEMult ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMultXE", m_thrXEMult ));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/ThrMultXS", m_thrXSMult ));

   // Items
   m_itemAcceptBP = new TH1I("L1ItemsBP","L1 Items before prescale", 512, 0, 512);
   m_itemAcceptAP = new TH1I("L1ItemsAP","L1 Items after prescale", 512, 0, 512);
   m_itemAcceptAV = new TH1I("L1ItemsAV","L1 Items after veto", 512, 0, 512);
   for(const TriggerItem * item : m_configSvc->ctpConfig()->menu().itemVector()) {
      if(item==nullptr) continue;
      string label = item->name() + " (CTP ID " + boost::lexical_cast<string,int>(item->ctpId())+ ")";
      m_itemAcceptBP->GetXaxis()->SetBinLabel(item->ctpId()+1,label.c_str());
      m_itemAcceptAP->GetXaxis()->SetBinLabel(item->ctpId()+1,label.c_str());
      m_itemAcceptAV->GetXaxis()->SetBinLabel(item->ctpId()+1,label.c_str());
   }
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/L1ItemsBP", m_itemAcceptBP));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/L1ItemsAP", m_itemAcceptAP));
   CHECK(m_histSvc->regHist( histstream + "CTPSimulation/L1ItemsAV", m_itemAcceptAV));

   return StatusCode::SUCCESS;
}



StatusCode
LVL1CTP::CTPSimulation::beginRun() {
   ATH_MSG_DEBUG("beginRun()");

   //
   // monitoring
   //
   bookHists().ignore();

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

   //
   // Set up the CTP version - cant be done before, callback to DSConfigSvc needs to complete first
   //

   unsigned int ctpVersion = ( m_ctpVersion != 0 ? m_ctpVersion : m_configSvc->ctpConfig()->ctpVersion() );
	
   ATH_MSG_INFO("SMK to be simulated: " << m_configSvc->masterKey());
   ATH_MSG_INFO("CTP version from the menu: " << ctpVersion);
   
   m_ctpDataformat = new CTPdataformatVersion(ctpVersion);
   ATH_MSG_DEBUG("Going to use the following for the CTP (version " << ctpVersion << " ):\n" << m_ctpDataformat->dump());

   m_countsBP.resize(m_ctpDataformat->getMaxTrigItems());
   m_countsAP.resize(m_ctpDataformat->getMaxTrigItems());
   m_countsAV.resize(m_ctpDataformat->getMaxTrigItems());

   // This printout can only be done here, as it relies on the CTP version
   if (m_doPSCL == false) {
      ATH_MSG_INFO("Inputs from LVL1 PSCL systems switched off");
   } else {
      if (ctpVersion>3) {
         ATH_MSG_DEBUG("There are no prescaled clock triggers in this version of the CTP (" << ctpVersion << ") any more. Setting m_doPSCL to false.");
         m_doPSCL = false;
      }
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
	
	
   // get random engine
   CLHEP::HepRandomEngine* rndmEngine=0;

   if(m_doRNDM == true) {
      rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
      if (rndmEngine == 0) {
         ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
         return StatusCode::FAILURE;
      }
		
      const TrigConf::Random random(m_configSvc->ctpConfig()->random());
    
      if (ctpVersion<4) {
         std::ostringstream message;
         message << "Random trigger definition: " << random.name() << std::setw(8) << random.rate1() << std::setw(8) << random.rate2();
      
         ATH_MSG_DEBUG(message.str());
      
      
         if (random.rate1() < 0) {
            ATH_MSG_INFO("Rate factor for random trigger RNDM0 below zero (" << random.rate1() << "): only possible in simulation");
         }
      
         unsigned int rate1 = (0x1 << (8+random.rate1())) - 1;
         ATH_MSG_INFO("REGTEST - Rate for random trigger RNDM0: " << rate1 << " / " << 40080./rate1 << " Hz");
         m_internalTrigger[ make_pair(TrigConf::L1DataDef::RNDM,0)] = new RandomTrigger(0, rate1, ctpVersion, rndmEngine);
      
         if (random.rate2() < 0) {
            ATH_MSG_INFO("Rate factor for random trigger RNDM1 below zero (" << random.rate2() << "): only possible in simulation");
         }
      
         unsigned int rate2 = (0x1 << (8+random.rate2())) - 1;
         ATH_MSG_INFO("REGTEST - Rate for random trigger RNDM1: " << rate2 << " / " << 40080./rate2 << " Hz");
         m_internalTrigger[ make_pair(TrigConf::L1DataDef::RNDM,1)] = new RandomTrigger(1, rate2, ctpVersion, rndmEngine);
      
      } else {//XXX How to treat random triggers in run-II?
         uint32_t cut0 = random.cuts(0);
         uint32_t cut1 = random.cuts(1);
         uint32_t cut2 = random.cuts(2);
         uint32_t cut3 = random.cuts(3);
      
         std::ostringstream message;
         message << "Random trigger definition: " << random.name() << std::setw(8) << hex << cut0 << std::setw(8) << hex << cut1
                 << std::setw(8) << hex << cut2 << std::setw(8) << hex << cut3;
      
         ATH_MSG_DEBUG(message.str());
      

         for(int rndmIdx = 0; rndmIdx<4; rndmIdx++) {
            uint32_t cut = random.cuts(rndmIdx);
            if(cut>=0x1000000) { cut = 0xFFFFFF; }
            if(cut==0) { cut = 0x1; }
            double prescale = double(0xFFFFFF) / (0x1000000-cut);

            ATH_MSG_INFO("REGTEST - Cut for random trigger  RNDM " << rndmIdx << " : " << "0x" << hex << cut << dec << " (" << cut << ")");
            ATH_MSG_INFO("REGTEST - PS (from 40.08MHz)           " << rndmIdx << " : " << prescale);
            ATH_MSG_INFO("REGTEST - Rate                         " << rndmIdx << " : " << 40080./prescale << " kHz");
            m_internalTrigger[ make_pair(TrigConf::L1DataDef::RNDM,rndmIdx)] = new RandomTrigger(rndmIdx, (unsigned int)prescale, ctpVersion, rndmEngine);
         }
      }
   }
	
   if(ctpVersion<4 && m_doPSCL == true){
      const TrigConf::PrescaledClock prescaledClock(m_configSvc->ctpConfig()->prescaledClock());
		
      ATH_MSG_DEBUG("REGTEST - Prescaled clock trigger definition: " << prescaledClock.name() 
                    << std::setw(8) << prescaledClock.clock1() << std::setw(8) << prescaledClock.clock2());	
      if (prescaledClock.clock1() > 0) {
         m_internalTrigger[ make_pair(TrigConf::L1DataDef::PCLK,0)] = new PrescaledClockTrigger(0, prescaledClock.clock1(), ctpVersion);
      } else {
         ATH_MSG_WARNING("No prescaled clock trigger PCLK0 defined.");
      }
      if (prescaledClock.clock2() > 0) {
         m_internalTrigger[ make_pair(TrigConf::L1DataDef::PCLK,1)] = new PrescaledClockTrigger(1, prescaledClock.clock2(), ctpVersion);
      } else {
         ATH_MSG_WARNING("No prescaled clock trigger PCLK1 defined.");
      }
   }else if (ctpVersion>3 && m_doPSCL == true) {
      ATH_MSG_DEBUG("There are no prescaled clock triggers in this version of the CTP (" << ctpVersion << ") any more. Setting m_doPSCL to false.");
      m_doPSCL=false;
   }else {
      ATH_MSG_DEBUG("Simulation of prescaled clock triggers is switched off.");
   }
	
   //XXX Maybe remove this since it is probably never gonna be used?
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
         for( TrigConf::TriggerThreshold* thr : thresholds) {
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

            for ( TrigConf::TriggerThreshold* thr : si_thresholds ) {
               ATH_MSG_DEBUG("MBTSSI Threshold name=" << thr->name());
               for (unsigned int i=0;i!=16;i++){
                  std::ostringstream osa;
                  osa <<"MBTS_A"<<i;
                  if ( thr->name() == osa.str()) 
                     thresholds_a++;
                  std::ostringstream osc;
                  osc <<"MBTS_C"<<i;
                  if ( thr->name() == osc.str()) 
                     thresholds_c++;
               }
            }
            if ( (thresholds_a != 16 && thresholds_a != 12) || ( thresholds_a != thresholds_c ) ) {
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
   if (ctpVersion<4) { //for versions of run-I there are only inputs via PIT bus
		
      if (m_configSvc->ctpConfig()->menu().pitVector().size()==0) { //assign PIT bits manually
         m_decisionMap = new ThresholdMap( m_configSvc->ctpConfig()->menu().thresholdVector());
      }else{ //take PIT assignment from configuration
         m_decisionMap = new ThresholdMap( m_configSvc->ctpConfig()->menu().thresholdVector(), 
                                           m_configSvc->ctpConfig()->menu().pitVector());
      }
   }else {// in newer version there are also direct inputs via CTPCORE+ front panel
		
      if (m_configSvc->ctpConfig()->menu().tipVector().size()==0) { //assign TIP bits manually
         m_decisionMap = new ThresholdMap( m_configSvc->ctpConfig()->menu().thresholdVector());
      } else { //take TIP assignment from configuration
         m_decisionMap = new ThresholdMap( m_configSvc->ctpConfig()->menu().thresholdVector(), 
                                           m_configSvc->ctpConfig()->menu().tipVector());
      }
   }
	
   ATH_MSG_DEBUG("Mapped decision threshold objects to configuration ones");
   ATH_MSG_DEBUG("          |--------------------------------------------------------|");
   ATH_MSG_DEBUG("          |              TIP layout for the thresholds             |");
   ATH_MSG_DEBUG("          |--------------------------------------------------------|");
   ATH_MSG_DEBUG("          |             Name         |   startbit   |    endbit    |");
   ATH_MSG_DEBUG("          |--------------------------------------------------------|");
   
   for( TrigConf::TriggerThreshold * threshold : m_configSvc->ctpConfig()->menu().thresholdVector() ) {
      ATH_MSG_DEBUG( "REGTEST - |   " << std::setw( 20 ) << threshold->name() << "   |   "
                     << std::setw( 8 ) << m_decisionMap->decision( threshold )->startBit() << "   |   "
                     << std::setw( 8 ) << m_decisionMap->decision( threshold )->endBit() << "   |");
   }
   ATH_MSG_DEBUG("          |--------------------------------------------------------|");
	
   for (InternalTriggerMap::value_type internalThr : m_internalTrigger) {
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
	
   for( TrigConf::TriggerItem* item : m_configSvc->ctpConfig()->menu().itemVector()) {
      ATH_MSG_DEBUG("REGTEST - |   " << std::setw( 40 ) << item->name() 
                    << "  | " << std::setw( 7 ) << m_itemMap->getItem( item )->itemPos() 
                    << "  | " << std::setw( 7 ) << m_itemMap->getItem( item )->prescale()
                    << "  |       0x" << std::hex << std::setfill('0') << std::setw( 2 ) << item->triggerType() << std::dec << std::setfill(' ')
                    << "  |");
   }
   ATH_MSG_DEBUG("          |---------------------------------------------------------------------------------|");
	
   // build prescale vector (for monitoring)
   m_prescales = m_configSvc->ctpConfig()->prescaleSet().prescales_float();
	
   ATH_MSG_INFO("done beginRun()");

   return StatusCode::SUCCESS;
}


StatusCode
LVL1CTP::CTPSimulation::execute() {
   
   ATH_MSG_DEBUG("Executing CTPSimulation algorithm");
 
   unsigned int ctpVersion = ( m_ctpVersion != 0 ? m_ctpVersion : m_configSvc->ctpConfig()->ctpVersion() );

	
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

   } else {
		
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
                          << std::setfill(' ') << std::dec << " Will be overwritten.");
         }
			
         EventInfo::instance().setEventType( 0 );
         if (triggerInfo->level1TriggerInfo().size() != 0) {
            ATH_MSG_DEBUG("TriggerInfo not zero: " << std::hex << std::setfill('0'));
            for (size_t i(0); i < triggerInfo->level1TriggerInfo().size(); ++i) {
               ATH_MSG_DEBUG("0x" << std::setw(8) << triggerInfo->level1TriggerInfo()[i]);
            }
            ATH_MSG_DEBUG(std::setfill(' ') << std::dec << " Will be overwritten.");
				
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
         ATH_MSG_DEBUG("Retrieved EM-TAU inputs from LVL1 calo simulation from StoreGate with SG key " << m_emTauCTPLoc );
         ATH_MSG_DEBUG("EM-TAU cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_emtauCTP->cableWord0());
         ATH_MSG_DEBUG("EM-TAU cable word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_emtauCTP->cableWord1());
         ATH_MSG_DEBUG("EM-TAU cable word 2 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_emtauCTP->cableWord2());
         ATH_MSG_DEBUG("EM-TAU cable word 3 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_emtauCTP->cableWord3());
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
         ATH_MSG_DEBUG("Energy cable word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_energyCTP->cableWord1());
      }
		
      if (m_doMBTS) {
         // Get forward and backward input from MBTS: (two cable words)
         sc = evtStore()->retrieve( m_mbtsACTP, m_mbtsACTPLoc );
         if ( sc.isFailure() ) {
            ATH_MSG_WARNING("Couldn't retrieve input from LVL1 MBTSA simulation from StoreGate " << m_mbtsACTPLoc);
            ATH_MSG_WARNING("Setting MBTSA input to CTP to zero");
         } else {
            ATH_MSG_DEBUG("Retrieved input from LVL1 MBTSA simulation from StoreGate " << m_mbtsACTPLoc);
            ATH_MSG_DEBUG("MBTSA cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_mbtsACTP->cableWord0());
         }
			
         sc = evtStore()->retrieve( m_mbtsCCTP, m_mbtsCCTPLoc );
         if ( sc.isFailure() ) {
            ATH_MSG_WARNING("Couldn't retrieve input from LVL1 MBTSC simulation from StoreGate " << m_mbtsCCTPLoc);
            ATH_MSG_WARNING("Setting MBTSC input to CTP to zero");
         } else {
            ATH_MSG_DEBUG("Retrieved input from LVL1 MBTSC simulation from StoreGate " << m_mbtsCCTPLoc);
            ATH_MSG_DEBUG("MBTSC cable word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_mbtsCCTP->cableWord0());
         }
      }
		
   } // if (m_doCalo)
	
   // Do something similar for L1Topo 
   if (m_doL1Topo) {
      if ( evtStore()->retrieve( m_topoCTP, m_topoCTPLoc ).isSuccess() ) {
         for(unsigned int i=0; i<32; ++i) {
            uint32_t mask = 0x1; mask <<= i;
            if( (m_topoCTP->cableWord1(0) & mask) != 0 ) m_HistL1TopoDecisionCable0->Fill(i); // cable 0, clock 0
            if( (m_topoCTP->cableWord1(1) & mask) != 0 ) m_HistL1TopoDecisionCable0->Fill(32 + i); // cable 0, clock 1
            if( (m_topoCTP->cableWord2(0) & mask) != 0 ) m_HistL1TopoDecisionCable1->Fill(i); // cable 1, clock 0
            if( (m_topoCTP->cableWord2(1) & mask) != 0 ) m_HistL1TopoDecisionCable1->Fill(32 + i); // cable 1, clock 1
         }
         ATH_MSG_DEBUG("Retrieved input from L1Topo from StoreGate with key " << m_topoCTPLoc);
         ATH_MSG_DEBUG("ALFA    word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_topoCTP->cableWord0(0));
         ATH_MSG_DEBUG("ALFA    word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_topoCTP->cableWord0(1));
         ATH_MSG_DEBUG("L1Topo0 word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_topoCTP->cableWord1(0));
         ATH_MSG_DEBUG("L1Topo0 word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_topoCTP->cableWord1(1));
         ATH_MSG_DEBUG("L1Topo1 word 0 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_topoCTP->cableWord2(0));
         ATH_MSG_DEBUG("L1Topo1 word 1 is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << m_topoCTP->cableWord2(1));

      } else {
         ATH_MSG_WARNING("Couldn't retrieve input from L1Topo from StoreGate");
         ATH_MSG_WARNING("Setting L1Topo inputs to CTP to zero");
      }
   }
	
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

         sc = iter->second->execute((int)EventInfo::instance().bcid());
         ATH_MSG_DEBUG("Rederiving BG " << iter->second->name() << " decision for bcid " << EventInfo::instance().bcid() << ": " << ( iter->second->evaluate() ? "ACTIVE" : "INACTIVE" ) );

      } else {
         sc = iter->second->execute();
      }

      if (sc.isFailure()) {
         ATH_MSG_ERROR("Error while executing internal trigger simulation " << iter->second->name());
      }

      ATH_MSG_DEBUG("REGTEST -  " << iter->second->print());

   }

   if ((m_doRNDM)&&(m_prescaleMode == RANDOM)){
      int event = EventInfo::instance().runNumber()+1;
      int run = EventInfo::instance().eventNumber()+1;
      ATH_MSG_DEBUG("Random Prescale Seeds " << event << " " << run);
		
      m_itemMap->updatePrescaleCounters( m_decisionMap, &m_internalTrigger, m_rndmSvc->setOnDefinedSeeds(event, run, this->name()) );
   }
   else {
      m_itemMap->updatePrescaleCounters( m_decisionMap, &m_internalTrigger );
   }
	
   //
   // Create the output objects:
   //
   m_resultBuilder = new ResultBuilder( ctpVersion, m_configSvc->ctpConfig(), m_decisionMap, m_itemMap, &m_internalTrigger, m_readoutWindow);

   collectStatistic();
      
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

   for ( TrigConf::TriggerThreshold* thr : m_configSvc->ctpConfig()->menu().thresholdVector() ) {
		
      int multiplicity = 0;
      
      if ( thr->cableName() == "" ) { 
         ATH_MSG_DEBUG("No cable name for what must be an internal threshold " << thr->name()); 
         continue; 
      }

      if ( thr->ttype() == L1DataDef::ZB ) { 
         ATH_MSG_DEBUG("No simulation of zero bias triggers, ignoring " << thr->name()); 
         continue; 
      }
      
      if ( thr->cableName() == "MU" || thr->cableName() == "MUCTPI" ) {
			
         if ( m_muctpiCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_muctpiCTP->muCTPIWord(), thr->cableStart(), thr->cableEnd() );
            for(int x=0; x<=multiplicity; x++)
               m_thrMUMult->AddBinContent(1+ 8 * thr->mapping() + x);
            m_thrMUTot->AddBinContent(1+ thr->mapping(), multiplicity);
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

      else if ( thr->cableName() == "EM1" ) {
         if ( m_emtauCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_emtauCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            for(int x=0; x<=multiplicity; x++)
               m_thrEMMult->AddBinContent(1+ 8 * thr->mapping() + x);
            m_thrEMTot->AddBinContent(1+ thr->mapping(), multiplicity);
         }
      }

      else if ( thr->cableName() == "EM2" ) {
         if ( m_emtauCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_emtauCTP->cableWord1(), thr->cableStart(), thr->cableEnd() );
            for(int x=0; x<=multiplicity; x++)
               m_thrEMMult->AddBinContent(1+ 8 * thr->mapping() + x);
            m_thrEMTot->AddBinContent(1+ thr->mapping(), multiplicity);
         }
      }

      else if ( thr->cableName() == "TAU1" ) {
         if ( m_emtauCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_emtauCTP->cableWord2(), thr->cableStart(), thr->cableEnd() );
            for(int x=0; x<=multiplicity; x++)
               m_thrHAMult->AddBinContent(1+ 8 * thr->mapping() + x);
            m_thrHATot->AddBinContent(1+ thr->mapping(), multiplicity);
         }
      }

      else if ( thr->cableName() == "TAU2" ) {
         if ( m_emtauCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_emtauCTP->cableWord3(), thr->cableStart(), thr->cableEnd() );
            for(int x=0; x<=multiplicity; x++)
               m_thrHAMult->AddBinContent(1+ 8 * thr->mapping() + x);
            m_thrHATot->AddBinContent(1+ thr->mapping(), multiplicity);
         }
      }

		
      else if ( thr->cableName() == "JEP1" || thr->cableName() == "JET1" ) {
			
         if ( m_jetCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_jetCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            for(int x=0; x<=multiplicity; x++)
               m_thrJET1Mult->AddBinContent(1+ 8 * thr->mapping() + x);
            m_thrJETTot->AddBinContent(1+ thr->mapping(), multiplicity);
         }
			
      } 
		
      else if ( thr->cableName() == "JEP2" || thr->cableName() == "JET2" ) {
			
         if ( m_jetCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_jetCTP->cableWord1(), thr->cableStart(), thr->cableEnd() );
            for(int x=0; x<=multiplicity; x++)
               m_thrJET2Mult->AddBinContent(1+ 4 * (thr->mapping()-10) + x);
            m_thrJETTot->AddBinContent(1+ thr->mapping(), multiplicity);
         }
			
      } 
		
      else if ( thr->cableName() == "JEP3" || thr->cableName() == "EN1") {
         if ( m_energyCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_energyCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );

            TH1I * hMult(0), * hTot(0);
            switch(thr->ttype()) {
            case L1DataDef::TE:
               hMult = m_thrTEMult; hTot = m_thrTETot; break;
            case L1DataDef::XE:
               hMult = m_thrXEMult; hTot = m_thrXETot; break;
            case L1DataDef::XS:
               hMult = m_thrXSMult; hTot = m_thrXSTot; break;
            default:
               break;
            }
            if(hMult) {
               for(int x=0; x<=multiplicity; x++)
                  hMult->AddBinContent(1+ 2 * thr->mapping() + x);
               hTot->AddBinContent(1+ thr->mapping(), multiplicity);
            }
         }
      }  

      else if ( thr->cableName() == "EN2") {
         if ( m_energyCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_energyCTP->cableWord1(), thr->cableStart(), thr->cableEnd() );

            TH1I * hMult(0), * hTot(0);
            switch(thr->ttype()) {
            case L1DataDef::TE:
               hMult = m_thrTEMult; hTot = m_thrTETot; break;
            case L1DataDef::XE:
               hMult = m_thrXEMult; hTot = m_thrXETot; break;
            default:
               break;
            }
            if(hMult) {
               for(int x=0; x<=multiplicity; x++)
                  hMult->AddBinContent(1+ 2 * thr->mapping() + x);
               hTot->AddBinContent(1+ thr->mapping(), multiplicity);
            }
         }
      }  

      else if ( thr->cableName() == "NIM1" ) {

         if ( thr->type() == TrigConf::L1DataDef::mbtsType() ||
              thr->type() == TrigConf::L1DataDef::mbtssiType() ) {
            // Don't know what to do in place of string matching
            int n_a=thr->name().find("MBTS_A");
            if (m_mbtsACTP.isValid() && n_a==0) {
               multiplicity = CTPUtil::getMult( m_mbtsACTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
               //ATH_MSG_DEBUG("Extracted multi is: "<< multiplicity);
            }
         }
			
			
      }  

      else if ( thr->cableName() == "NIM2" ) {

         if ( thr->type() == TrigConf::L1DataDef::mbtsType() ||
              thr->type() == TrigConf::L1DataDef::mbtssiType() ) {
            // Don't know what to do in place of string matching
            int n_c=thr->name().find("MBTS_C");
            if (m_mbtsCCTP.isValid() && n_c==0) {
               multiplicity = CTPUtil::getMult( m_mbtsCCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
               //ATH_MSG_DEBUG("Extracted multi is: "<< multiplicity);
            }
         }
			
      }  

      //Is there a better way to deal with overclocking at front panel?
      else if ( thr->cableName() == "TOPO1" ) {

         if ( m_topoCTP.isValid() ) {
            uint64_t topoCable1 = m_topoCTP->cableWord1( 1 );
            topoCable1 <<= 32;
            topoCable1 += m_topoCTP->cableWord1( 0 );

            ATH_MSG_DEBUG( " ---> Topo input " << dec << thr->name() << " on module TOPO1 with clock " << thr->clock()
                           << ", cable start " << thr->cableStart() << " and end " << thr->cableEnd() 
                           << " double word 0x" << setw(16) << setfill('0') << hex << topoCable1 << dec << setfill(' ') 
                           );

            multiplicity = CTPUtil::getMultTopo( topoCable1, thr->cableStart(), thr->cableEnd(), thr->clock() );
         }

      }   
		
      else if ( thr->cableName() == "TOPO2" ) {
		
         if ( m_topoCTP.isValid() ) {

            uint64_t topoCable2 = m_topoCTP->cableWord2( 1 );
            topoCable2 <<= 32;
            topoCable2 += m_topoCTP->cableWord2( 0 );

            ATH_MSG_DEBUG( " ---> Topo input " << dec << thr->name() << " on module TOPO2 with clock " << thr->clock()
                           << ", cable start " << thr->cableStart() << " and end " << thr->cableEnd()
                           << " double word 0x" << setw(16) << setfill('0') << hex << topoCable2 << dec << setfill(' ') 
                           );

            multiplicity = CTPUtil::getMultTopo( topoCable2, thr->cableStart(), thr->cableEnd(), thr->clock() );

         }
      }    

      else if ( thr->cableName() == "ALFA" ) {
         if ( m_topoCTP.isValid() ) {
            ATH_MSG_DEBUG( " ---> ALFA input " << dec << thr->name() << " on module ALFA with clock " << thr->clock() << ", cable start "
                           << thr->cableStart() << " and end " << thr->cableEnd() << " word 0x" << setw(8) << setfill('0') << hex << m_topoCTP->cableWord0( thr->clock() ) << dec << setfill(' ') 
                           );

            uint64_t alfaCable = m_topoCTP->cableWord0( 1 );
            alfaCable <<= 32;
            alfaCable += m_topoCTP->cableWord0( 0 );

//             unsigned int counter = 2*thr->cableStart() + thr->clock();
//             multiplicity = CTPUtil::getMult( alfaCable, counter, counter );

            multiplicity = CTPUtil::getMultTopo( alfaCable, thr->cableStart(), thr->cableEnd(), thr->clock() );
         }

      }    

      else if ( thr->ttype() == TrigConf::L1DataDef::BCM ) {
         
         if ( m_bcmCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_bcmCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         }
      }

      else if ( thr->ttype() == TrigConf::L1DataDef::LUCID ) {
				
         if ( m_lucidCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_lucidCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         } 
      } 

      else if ( thr->ttype() == TrigConf::L1DataDef::BPTX ) {

         if ( m_bptxCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_bptxCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         }
         
      }

      else if ( thr->ttype() == TrigConf::L1DataDef::BCMCMB ) {
         
         if ( m_bcmCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_bcmCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         }
      } 
      
      else if ( thr->ttype() == TrigConf::L1DataDef::ZDC ) {
         
         if ( m_zdcCTP.isValid() ) {
            multiplicity = CTPUtil::getMult( m_zdcCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         } 
      } 

      else if ( thr->ttype() == TrigConf::L1DataDef::CALREQ ) {
				
         ATH_MSG_DEBUG("The CALREQ type is ignored offline");
         multiplicity = 0;
      }
   
      else if ( thr->cableName() == "CTPCAL" ) {
         // should not get here
         ATH_MSG_WARNING("Cable CTPCAL has threshold " << thr->name() << " of type " << thr->ttype() << " which is not handled by the CTP simulation");
      }  

      else if ( thr->type() == TrigConf::L1DataDef::mbtsType() ) {
         // Don't know what to do in place of string matching
         int n_a=thr->name().find("MBTS_A");
         int n_c=thr->name().find("MBTS_C");
         if (m_mbtsACTP.isValid() && n_a==0) {
            multiplicity = CTPUtil::getMult( m_mbtsACTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            //ATH_MSG_DEBUG("Extracted multi is: "<< multiplicity);
         }
         if (m_mbtsCCTP.isValid() && n_c==0) {
            multiplicity = CTPUtil::getMult( m_mbtsCCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
            //ATH_MSG_DEBUG("Extracted multi is: "<< multiplicity);
         }
      }
			
      else if ( thr->type() == TrigConf::L1DataDef::mbtssiType() ) {
         int n_a = thr->name().find("MBTS_A");
         if (m_mbtsACTP.isValid() && n_a==0) {
            multiplicity = CTPUtil::getMult( m_mbtsACTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
         }
         int n_c = thr->name().find("MBTS_C");
         if (m_mbtsCCTP.isValid() && n_c==0) {
            multiplicity = CTPUtil::getMult( m_mbtsCCTP->cableWord0(), thr->cableStart(), thr->cableEnd() );
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
		
      else {
			
         ATH_MSG_ERROR("Threshold type " << thr->type() << " for threshold " << thr->name()
                       << " and cable name " << thr->cableName() << " not recognized");
         return StatusCode::FAILURE;
      }
		
      ATH_MSG_DEBUG(" ---> Threshold with name: " << std::setfill(' ') << std::setw( 8 ) << thr->name() << " gets multiplicity: "
                    << std::setw( 2 ) << multiplicity);
      m_decisionMap->decision( thr )->setValue( multiplicity );
		
   }
	
   return StatusCode::SUCCESS;
}


StatusCode
LVL1CTP::CTPSimulation::LoadBunchGroups() {
   ATH_MSG_DEBUG("Trying to access bunch group content from COOL");  
	
   unsigned int ctpVersion = ( m_ctpVersion != 0 ? m_ctpVersion : m_configSvc->ctpConfig()->ctpVersion() );
	
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
   if (msgLvl(MSG::DEBUG)) {
     for (size_t i(0); i < bunchGroups.size(); ++i) {
       std::ostringstream message;
       ATH_MSG_DEBUG("BunchGroup " << i << " Name " << bunchGroups[i].name() << " Bunches:" << bunchGroups[i].bunches().size());
       for (size_t j(0); j < bunchGroups[i].bunches().size(); ++j) {
          message << " " << bunchGroups[i].bunches()[j];
       }
       ATH_MSG_VERBOSE("REGTEST - " << message.str());
     }
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
         m_internalTrigger[trigtype] = new BunchGroupTrigger(i, bunchGroups[i].bunches(),ctpVersion);
      }
   }
   delete bunchGroupSet;
   return StatusCode::SUCCESS;
}


const LVL1CTP::ResultBuilder*
LVL1CTP::CTPSimulation::resultBuilder() const {
	return m_resultBuilder;
}


void
LVL1CTP::CTPSimulation::collectStatistic() {

   const uint32_t mask = 0x1;

   unsigned int idx(0);
   for(uint32_t w : resultBuilder()->tbp()) {
      for(unsigned int bit=0; bit<32; ++bit) {
         if( (mask<<bit & w) != 0 ) { // fired
            m_countsBP[idx] += 1;
            m_itemAcceptBP->AddBinContent(idx+1);
         }
         idx++;
      }
   }
   
   idx = 0;
   for(uint32_t w : resultBuilder()->tap()) {
      for(unsigned int bit=0; bit<32; ++bit) {
         if( (mask<<bit & w) != 0 ) { // fired
            m_countsAP[idx] += 1;
            m_itemAcceptAP->AddBinContent(idx+1);
         }
         idx++;
      }
   }

   idx = 0;
   for(uint32_t w : resultBuilder()->tav()) {
      for(unsigned int bit=0; bit<32; ++bit) {
         if( (mask<<bit & w) != 0 ) { // fired
            m_countsAV[idx] += 1;
            m_itemAcceptAV->AddBinContent(idx+1);
         }
         idx++;
      }
   }
}
