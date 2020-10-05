/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** R.Goncalo - 21/10/2007 - add tests for TrigDecisionTool:
 TrigDecisionChecker based on TrigDecisionMaker/TrigDecisionTest */

#include "TrigDecisionChecker.h"

// EDM
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "TrigParticle/TrigTauContainer.h"
#include "TrigCaloEvent/TrigTauClusterContainer.h"
#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"

#include "TrigSteeringEvent/Lvl1Item.h"

// Muon includes
#include "xAODMuon/MuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"

// include these tau navigation check
#include "TrigParticle/TrigTau.h"
//#include "TrigCaloEvent/TrigTauCluster.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/Tau1P3PDetails.h"
#include "tauEvent/TauRecDetails.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigger/TrigPassBits.h"

// bjet includes
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertex.h"

#include "TrigParticle/TrigEFBjetContainer.h"
#include "Particle/TrackParticleContainer.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"

#include "TrigConfHLTData/HLTTriggerElement.h"

#include "CxxUtils/crc64.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "xAODTrigMinBias/TrigSpacePointCounts.h"
#include "xAODTrigMinBias/TrigT2MbtsBits.h"
#include "xAODTrigMinBias/TrigVertexCounts.h"
#include "xAODTrigMinBias/TrigTrackCounts.h"

// tools
#include "MuonCombinedToolInterfaces/IMuonPrintingTool.h"

#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

TrigDecisionChecker::TrigDecisionChecker(const std::string &name, ISvcLocator *pSvcLocator)
: AthAlgorithm(name, pSvcLocator),
  m_smk(0),
  m_l1psk(0),
  m_hltpsk(0),
  m_printout_file(""),
  m_first_event(true),
  m_event_number(0),
  m_mu_sum(0.0),
  m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_configSvc( "TrigConf::TrigConfigSvc/TrigConfigSvc", name ),
  m_muonPrinter("Rec::MuonPrintingTool/MuonPrintingTool")
{
    
    declareProperty("TrigDecisionKey",   m_trigDecisionKey = "TrigDecision");
    declareProperty("TrigDecisionTool",  m_trigDec, "The tool to access TrigDecision");
    declareProperty("MonitoredChains",   m_count_sig_names );
    //declareProperty("MonitoredTauChain", m_tauItem = "tau20_r1medium1");
    declareProperty("MonitoringBlock",   m_monitoring_block_size = 100);
    declareProperty("WriteEventDecision",m_event_decision_printout = true);
    declareProperty("WriteOutCounts",    m_printout_counts   = false);
    declareProperty("WriteOutFilename",  m_printout_filename = "trigger_counts.log");
    declareProperty("EventInfoName",     m_eventInfoName="", "The name of the EventInfo container" );
    declareProperty("SMK",               m_smKey=0, "The super master key to use" );
    declareProperty("MuonItems",         m_muonItems, "Muon triggers to test");
    declareProperty("BjetItems",         m_bjetItems, "Bjet triggers to test");
    declareProperty("BphysicsItems",         m_bphysItems, "Bphysics triggers to test");
    declareProperty("ElectronItems",     m_electronItems, "Electron triggers to test");
    declareProperty("PhotonItems",       m_photonItems, "Photon triggers to test");
    declareProperty("TauItems",       m_TauItems, "Tau triggers to test");
    declareProperty("MinBiasItems",      m_minBiasItems, "MinBias triggers to test");
    declareProperty("JetItems",       m_jetItems, "Jet triggers to test");
    declareProperty("MetItems",       m_metItems, "Met triggers to test");
    declareProperty("CheckTrigPassBits",       m_checkBits=false, "TrigPassBits retrieval from TDT");
    
}


TrigDecisionChecker::~TrigDecisionChecker() {}


StatusCode TrigDecisionChecker::initialize()
{
    // reset event counters
    m_first_event=true;
    m_event_number=0;
    m_mu_sum=0.0;
    
    // Reset keys
    m_smk=0;
    m_l1psk=0;
    m_hltpsk=0;
    
    // print out properties
    ATH_MSG_INFO("Initializing...");
    ATH_MSG_INFO("TrigDecisionKey:    " << m_trigDecisionKey);
    ATH_MSG_INFO("MonitoredChains:    " << m_count_sig_names);
    //ATH_MSG_INFO("MonitoredTauItem:   " << m_tauItem);
    ATH_MSG_INFO("MonitoringBlock:    " << m_monitoring_block_size);
    ATH_MSG_INFO("PrintOutCounts:     " << m_printout_counts);
    if (m_printout_counts) {
      ATH_MSG_INFO("PrintOutFilename:   " << m_printout_filename);
    }
    ATH_MSG_INFO("SuperMasterKey:     " << m_smKey);
    
    // Retrieve the trigger configuration service
    // get handle to trigger configuration
    ATH_CHECK(m_configSvc.retrieve());
    
    // get handle to TrigDecisionTool
    ATH_CHECK(m_trigDec.retrieve());
    m_trigDec->ExperimentalAndExpertMethods()->enable();
    
    // reserve space for vectors
    m_summary.reserve(700);
    m_summary_chain_passraw.reserve(700);
    m_summary_chain_passraw.reserve(700);
    m_summary_chain_PT.reserve(700);
    m_summary_chain_PS.reserve(700);
    m_summary_passraw.reserve(700);
    m_summary_pass.reserve(700);
    m_summary_passphys.reserve(700);
    m_chain_prescales.reserve(700);
    m_chain_prescales_calculated.reserve(700);
    m_chain_passthrough.reserve(700);
    m_chain_passthrough_calculated.reserve(700);
    m_lower_chain_accept.reserve(700);
    
    // initialize vectors for chains to monitor
    ATH_MSG_INFO("Monitoring number of events passing these chains (blocks of "
                 << m_monitoring_block_size << " events):");
    for (unsigned int i=0; i < m_count_sig_names.size(); ++i) {
        ATH_MSG_INFO("Initializing monitoring counters for " << m_count_sig_names[i]);
        m_count_sigs.push_back(new std::vector<int>());
        m_run_count_sigs.push_back(0);
    }
    
    // retrieve muon printing tool
    ATH_CHECK(m_muonPrinter.retrieve());
    
    ATH_MSG_INFO("Initialization successful");    
    
    return StatusCode::SUCCESS;
}


StatusCode TrigDecisionChecker::finalize()
{
    
    // print summary of trigger decisions for each level
    msg(MSG::INFO) << "==========================================================" << endmsg;
    msg(MSG::INFO) << "TrigDecisionTool summary:" << endmsg;
    msg(MSG::INFO) << "==========================================================" << endmsg;
    
    // LVL1
    std::map<std::string,int>::iterator itL1,itL1End=m_L1_summary.end();
    for ( itL1=m_L1_summary.begin(); itL1!=itL1End; ++itL1 ) {
        msg(MSG::INFO) << "REGTEST item " << (*itL1).first << " accepted events=" << (*itL1).second << endmsg;
    }
    
    // HLT
    for ( unsigned int i=0; i<m_summary.size(); ++i) {
        msg(MSG::INFO) << "REGTEST chain " << m_summary[i] << " accepted events= " << m_summary_passphys[i]
        <<" ( PS: " << m_summary_chain_PS[i] << " , PT: " << m_summary_chain_PT[i] << ")" << endmsg;
    }
    
    // print out nr. of events passed in blocks of N events for specific chains (configurable)
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("TrigDecisionTool tests: monitored chain efficiency");
    ATH_MSG_INFO("==================================================");
    msg(MSG::INFO) << "REGTEST  Nr.events: ";
    for (unsigned int k=0; k<m_count_event.size();++k) {
        msg() << m_count_event[k] << " ";
    }
    msg() << endmsg;
    for (unsigned int i=0; i<m_count_sig_names.size();++i) {
        msg(MSG::INFO) << "REGTEST " << m_count_sig_names[i] << " : ";
        for (unsigned int j=0; j<(m_count_sigs[i])->size();++j) {
            msg() << (*m_count_sigs[i])[j] << " ";
        }
        msg() << endmsg;
    }
    
    // compare prescale and passthrough fractions from configuration and by counting events
    ATH_MSG_INFO(" ");
    ATH_MSG_INFO("TrigDecisionTool tests: chain prescale/passthrough ");
    ATH_MSG_INFO("==================================================");
    // HLT
    msg(MSG::INFO) << "REGTEST Chain (passed raw) PS(conf) PS(counts) PT(conf) PT(counts) (Lower chain)";
    for ( unsigned int i=0; i<m_summary.size(); ++i) {
        if (m_lower_chain_accept[i]!=0) {
            msg(MSG::INFO) << "REGTEST " << m_summary[i] << " (" << m_summary_chain_PT[i] << ") \t"
            << " \t" << m_chain_prescales[i] << " \t" << m_chain_prescales_calculated[i] ///m_lower_chain_accept[i]
            << " \t" << m_chain_passthrough[i] << " \t" << m_chain_passthrough_calculated[i] ///m_lower_chain_accept[i]
            << " (" << m_lower_chain_accept[i] << ")" << endmsg;
        } else {
            msg(MSG::INFO) << "REGTEST " << m_summary[i] << " (" << m_summary_chain_PT[i] << ") \t"
            << " \t" << m_chain_prescales[i] << "   ---   "
            << " \t" << m_chain_passthrough[i] << "   ---   "
            << " (lower chain passed no events)" << endmsg;
        }
    }
    ATH_MSG_INFO("REGTEST ran on " << m_event_number << " events");
    ATH_MSG_INFO("Average mu value " << m_mu_sum/float(m_event_number));
    
    // open output file and write out counts
    if (m_printout_counts) {
        ATH_MSG_INFO("==================================================");
        ATH_MSG_INFO("Opening " << m_printout_filename << " for writing trigger counts");
        m_printout_file.open(m_printout_filename.c_str());
        
        /*
         m_L1_summary - isPassed for L1
         m_summary_chain_passraw  - chainPassedRaw for HLT     
         m_summary_pass     - chain Passed for HLT     
         m_summary_passphys - chain Physics Passed for HLT     
         m_summary_chain_PS - chainPassedRaw && !isPrescaled for HLT     
         m_summary_chain_PT - (chainPassedRaw && !isPrescaled) || isPassedThrough for HLT     
         */
        
        m_printout_file  << std::setiosflags(std::ios::left) << std::setw(25) << "* L1 item"
        << std::setw(15) << " #Passed " << std::endl;
        std::map<std::string,int>::iterator l1,l1End=m_L1_summary.end();
        for ( l1=m_L1_summary.begin(); l1!=l1End; ++l1 ) {
            m_printout_file << std::setiosflags(std::ios::left) << (*l1).first 
            << std::setw(10) << (*l1).second << std::endl;
        }
        
        m_printout_file  << std::setiosflags(std::ios::left) << std::setw(25) << "* Chain name"
        << std::setw(15) << " #Passed Raw "
        << std::setw(15) << " #passed  "
        << std::setw(15) << " #Physics Passed "<< std::endl;
        for ( unsigned int i=0; i<m_summary.size(); ++i) {
            m_printout_file << std::setiosflags(std::ios::left) << std::setw(25) << m_summary[i] 
            << std::setw(15) << m_summary_passraw[i] 
            << std::setw(15) << m_summary_pass[i] 
            << std::setw(15) << m_summary_passphys[i] << std::endl;
            
            //    std::cout << std::setiosflags(std::ios::dec) << std::setw(6) << i << "|"
            // 	      << std::setiosflags(std::ios::dec) << std::setw(4) << (*(m_elink_vec[i]))->algorithmId() << "|"
            // 	      << std::setw(11) << *(m_elink_vec[i]) << "|";
        }
        ATH_MSG_INFO("Closing output file " << m_printout_filename);
        m_printout_file.close();
    }
    
    // cleanup newed vectors
    ATH_MSG_INFO("==================================================");
    ATH_MSG_INFO("Cleaning up counters...");
    for (unsigned int i=0; i<m_count_sigs.size(); ++i) {
        delete m_count_sigs[i];
    }
    ATH_MSG_INFO("Finalised successfully");
    
    return StatusCode::SUCCESS;
}

uint32_t TrigDecisionChecker_old_smk=0;

StatusCode TrigDecisionChecker::execute()
{
    // Fill the variables:
    m_smk = m_configSvc->masterKey();
    m_l1psk = m_configSvc->lvl1PrescaleKey();
    m_hltpsk = m_configSvc->hltPrescaleKey();
    
    // If the keys returned by the configuration service don't seem to make sense,
    // use something else as the SMK. (Needed mostly for MC test jobs.)
    if( ( ( m_smk == 0 ) && ( m_l1psk == 0 ) && ( m_hltpsk == 0 ) ) ||
       ( static_cast< int >( m_smk )    < 0 ) ||
       ( static_cast< int >( m_l1psk )  < 0 ) ||
       ( static_cast< int >( m_hltpsk ) < 0 ) ) {

        m_smk = CxxUtils::crc64( m_configSvc->configurationSource() ) & 0xffff;
        m_l1psk = 0;
        m_hltpsk = 0;
    }
    
    if(TrigDecisionChecker_old_smk!=m_smk) {
        ATH_MSG_INFO("New SMK found = " << m_smk);
        TrigDecisionChecker_old_smk=m_smk;
    }
    ATH_MSG_DEBUG("SMK = " << m_smk);
    
    // Check to see whether this is an event which we should process
    if(m_smk!=m_smKey && m_smKey!=0) {
        // We do not have a matching super master key so skip the event and return success
        return StatusCode::SUCCESS;
    }
    
    m_event_number++;
    
    // check mu value
    const EventInfo* eventInfo;
    //#sc = evtStore()->retrieve(eventInfo, m_eventInfoName);
    StatusCode sc;
    if (m_eventInfoName == "") {
        sc=evtStore()->retrieve(eventInfo);
    } else {
        sc=evtStore()->retrieve(eventInfo ,m_eventInfoName);
    }
    if ( sc.isFailure() || !eventInfo )
    {
      ATH_MSG_INFO("Container '" << m_eventInfoName << "' could not be retrieved from StoreGate !");
      sc = StatusCode::SUCCESS;
      return sc;
    } else {
      ATH_MSG_DEBUG("Container '" << m_eventInfoName << "' retrieved from StoreGate");
    }
    
    if ( eventInfo ) {
        float mu = eventInfo->actualInteractionsPerCrossing();
        float muave =  eventInfo->averageInteractionsPerCrossing(); 
        msg(MSG::INFO) << "run number  " << eventInfo->event_ID()->run_number() << " event number " << eventInfo->event_ID()->event_number() << 
        " lumi block " << eventInfo->event_ID()->lumi_block() << endmsg;
        msg(MSG::INFO) << "mu value " << mu << " average mu value " << muave <<  " event number " << m_event_number <<  endmsg;
        m_mu_sum = m_mu_sum + eventInfo->actualInteractionsPerCrossing();
    }
    
    
    for(const auto& tauItem : m_TauItems) {
        ATH_MSG_INFO("Check tau items " << tauItem);
        sc = checkTauEDM(tauItem);
        if ( sc.isFailure() ) {
            msg(MSG::ERROR) << "Could not finish checkTauEDM test for chain " <<tauItem << endmsg;
            return sc;
        }
        if(m_checkBits) {
            if(checkEDM<xAOD::TauJetContainer>(tauItem).isFailure()) 
                ATH_MSG_ERROR("Could not finish checkTauJetEDM test for chain " << tauItem);
        }
    }
    
    for(const auto& muonItem : m_muonItems) {
        sc = checkMuonEDM(muonItem);
        if ( sc.isFailure() ) {
            msg(MSG::ERROR) << "Could not finish checkMuonEDM test for chain " << muonItem << endmsg;
            return sc;
        }
        if(m_checkBits) {
            if(checkEDM<xAOD::MuonContainer>(muonItem).isFailure()) 
                ATH_MSG_ERROR("Could not finish checkMuonEDM test for chain " << muonItem);
            if(checkEDM<xAOD::L2CombinedMuonContainer>(muonItem).isFailure())
                ATH_MSG_ERROR("Could not finish checkMuonEDM test for chain " << muonItem);
            if(checkEDM<xAOD::L2StandAloneMuonContainer>(muonItem).isFailure())
                ATH_MSG_ERROR("Could not finish checkMuonEDM test for chain " << muonItem);
        }
    }
    
    for(const auto& bjetItem : m_bjetItems) {
        sc = checkBjetEDM(bjetItem);
        if ( sc.isFailure() ) {
            msg(MSG::ERROR) << "Could not finish checkBjetEDM test for chain " << bjetItem << endmsg;
            return sc;
        }
        if(m_checkBits) {
            if(checkEDM<xAOD::BTaggingContainer>(bjetItem).isFailure()) 
                ATH_MSG_ERROR("Could not finish checkBjetEDM test for chain " << bjetItem);
        }
    }
    
    for(const auto& bphysItem : m_bphysItems) {
        sc = checkBphysEDM(bphysItem);
        if ( sc.isFailure() ) {
            msg(MSG::ERROR) << "Could not finish checkBphysEDM test for chain " << bphysItem << endmsg;
            // return sc; try not to return for other tests to run
        }
        if(m_checkBits) {
            if(checkEDM<xAOD::TrigBphysContainer>(bphysItem).isFailure()) 
                ATH_MSG_ERROR("Could not finish checkJetEDM test for chain " << bphysItem);
        }
    }
    
    for(const auto& electronItem : m_electronItems) {
        ATH_MSG_INFO("Check Electron items " << electronItem);
        sc = checkElectronEDM(electronItem);
        if ( sc.isFailure() ) {
            msg(MSG::ERROR) << "Could not finish checkElectronEDM test for chain " << electronItem << endmsg;
        }
        if(m_checkBits) {
            if(checkEDM<xAOD::ElectronContainer>(electronItem).isFailure()) 
                ATH_MSG_ERROR("Could not finish checkElectronEDM test for chain " << electronItem);
            if(checkEDM<xAOD::CaloClusterContainer>(electronItem).isFailure()) 
                ATH_MSG_ERROR("Could not finish checkElectronEDM test for chain " << electronItem);
            if(checkEDM<xAOD::TrigElectronContainer>(electronItem).isFailure()) 
                ATH_MSG_ERROR("Could not finish checkElectronEDM test for chain " << electronItem);
        }
    }
    
    for(const auto& photonItem : m_photonItems) {
        sc = checkPhotonEDM(photonItem);
        if ( sc.isFailure() ) {
          ATH_MSG_ERROR("Could not finish checkPhotonEDM test for chain " << photonItem);
        }
        if(m_checkBits) {
            if(checkEDM<xAOD::PhotonContainer>(photonItem).isFailure()) 
              ATH_MSG_ERROR("Could not finish checkPhotonEDM test for chain " << photonItem);
            if(checkEDM<xAOD::CaloClusterContainer>(photonItem).isFailure()) 
              ATH_MSG_ERROR("Could not finish checkPhotonEDM test for chain " << photonItem);
        }
    }
    
    for(const auto& minBiasItem : m_minBiasItems) {
        sc = checkMinBiasEDM(minBiasItem);
        if ( sc.isFailure() ) {
            msg(MSG::ERROR) << "Could not finish checkMinBiasEDM test for chain " << minBiasItem << endmsg;
            return sc;
        }
    }
    
    ATH_MSG_INFO("REGTEST ==========START of Jet EDM/Navigation check===========");
    for(const auto& jetItem : m_jetItems) {
        sc = checkJetEDM(jetItem);
        if ( sc.isFailure() ) {
            ATH_MSG_INFO("REGTEST Could not finish checkJetEDM test for chain " << jetItem);
            return sc;
        }
        if(m_checkBits) {
            if(checkEDM<xAOD::JetContainer>(jetItem).isFailure()) 
                ATH_MSG_ERROR("Could not finish checkJetEDM test for chain " << jetItem);
        }
    }
    ATH_MSG_INFO("REGTEST ==========END of Jet EDM/Navigation check===========");
    
    ATH_MSG_INFO("REGTEST ==========START of Met EDM/Navigation check===========");
    for(const auto& metItem : m_metItems) {
        sc = checkMetEDM(metItem);
        if ( sc.isFailure() ) {
            ATH_MSG_INFO("REGTEST Could not finish checkMetEDM test for chain " << metItem);
            return sc;
        }
    }
    ATH_MSG_INFO("REGTEST =========END of Met EDM/Navigation check============");
    
    if (m_event_decision_printout) {
        msg(MSG::INFO) << "TrigDecisionChecker::execute" << endmsg;
        
        msg(MSG::INFO) << "Pass state     = " << m_trigDec->isPassed("EF_.*") << endmsg;
        msg(MSG::INFO) << "Pass state L1  = " << m_trigDec->isPassed("L1_.*") << endmsg;
        msg(MSG::INFO) << "Pass state L2  = " << m_trigDec->isPassed("L2_.*") << endmsg;
        msg(MSG::INFO) << "Pass state EF  = " << m_trigDec->isPassed("EF_.*") << endmsg;
        msg(MSG::INFO) << "Pass state HLT = " << m_trigDec->isPassed("HLT_.*") << endmsg;
    }
    
    Trig::ExpertMethods* em = m_trigDec->ExperimentalAndExpertMethods();
    
    // L1
    std::vector<std::string> allItems = m_trigDec->getListOfTriggers("L1_.*");
    if (!allItems.empty()) {
        if (m_event_decision_printout) msg(MSG::INFO) << "Items : " << allItems.size() << endmsg;
        
        for (std::vector<std::string>::const_iterator itemIt = allItems.begin();
             itemIt != allItems.end(); ++itemIt) {
            
            const LVL1CTP::Lvl1Item* aItem = em->getItemDetails(*itemIt);
            
            if (!aItem) continue;
            if (aItem->name()=="") continue;
            
            if (m_event_decision_printout) msg(MSG::INFO) << "Item " << aItem->name() << " : Item ID " << aItem->hashId() << " : " << aItem->isPassed() << endmsg;
            
            // fill bookkeeping map with zeros if first event
            std::string item_name = aItem->name();
            if (m_first_event) m_L1_summary[item_name] = 0;
            
            // increment counter for L1 summary
            if (aItem->isPassed()) m_L1_summary[item_name] = (m_L1_summary[item_name]+1);
            int count = (m_L1_summary.find(item_name))->second;
            if (m_event_decision_printout) msg(MSG::INFO) << "L1_map[" << item_name << "] = " << count << endmsg;
            
            
        }
    } else {
      ATH_MSG_WARNING("Could not retrieve L1 Items !!");
    }
    
    
    // HLT Chains
    
    // Get all configured chain names from config
    std::vector<std::string> confChains = m_trigDec->getListOfTriggers("L2_.*, EF_.*, HLT_.*");
    msg(MSG::INFO) << "Configuring for " << confChains.size() << " HLT chain counters" << endmsg;
    
    // resize & initialise counters in first event
    if (m_first_event) {
        m_summary.resize(0); // vector for chain names
        m_summary_passraw.resize(confChains.size(),0);
        m_summary_pass.resize(confChains.size(),0);
        m_summary_passphys.resize(confChains.size(),0);
        m_summary_chain_passraw.resize(confChains.size(),0);
        m_summary_chain_pass.resize(confChains.size(),0);
        m_summary_chain_passphys.resize(confChains.size(),0);
        m_summary_chain_PT.resize(confChains.size(),0);
        m_summary_chain_PS.resize(confChains.size(),0);
        m_chain_prescales.resize(confChains.size(),0);
        m_chain_prescales_calculated.resize(confChains.size(),0);
        m_chain_passthrough.resize(confChains.size(),0);
        m_chain_passthrough_calculated.resize(confChains.size(),0);
        m_lower_chain_accept.resize(confChains.size(),0);
        
        // make vector of names and sort alphabetically
        std::map<std::string,float> t_pt_map;
        std::map<std::string,float> t_ps_map;
        
        for(std::vector<std::string>::iterator chIter = confChains.begin(); chIter != confChains.end(); ++chIter) {
            const TrigConf::HLTChain * ch = em->getChainConfigurationDetails(*chIter);
            std::string name = *chIter;
            m_summary.push_back(name);
            t_pt_map[name] = ch->pass_through();
            t_ps_map[name] = ch->prescale();
            m_lower_chain_map[name] = ch->lower_chain_name();
            ATH_MSG_DEBUG("Configured chain: " << name
                          << "; prescale=" << ch->prescale()
                          << "; passthrough=" << ch->pass_through()
                          << "; lower chain=" << ch->lower_chain_name());
        }
        // sort by chain names to group together L2 and EF chains: do this for
        // first event only and use ordering in m_summary for later processing
        std::sort(m_summary.begin(),m_summary.end());
        
        // store prescaled factor from *configuration* chain
        for (unsigned int k=0; k<m_summary.size(); ++k) {
            // prescales
            std::map<std::string,float>::iterator psIt=t_ps_map.find(m_summary[k]);
            float ps = -1;
            if (psIt!=t_ps_map.end()) ps = (*psIt).second;
            m_chain_prescales[k]=ps;
            // passthrough
            std::map<std::string,float>::iterator ptIt=t_pt_map.find(m_summary[k]);
            float pt = -1;
            if (ptIt!=t_pt_map.end()) pt = (*ptIt).second;
            m_chain_passthrough[k]=pt;
        }
    }
    
    // all events: loop over names of configured chains, find their status,
    //   increment counters
    for (unsigned int i=0; i<m_summary.size(); ++i){
        std::string name = m_summary[i];
        ATH_MSG_VERBOSE("Testing chain: " << name);
        
        const HLT::Chain* aChain = em->getChainDetails(name);
        if (! aChain) { // inactive chain
            continue;
        }
        
        // use TrigDecisionTool methods directly
        if ( m_trigDec->isPassed(name, TrigDefs::allowResurrectedDecision | TrigDefs::requireDecision) ) {
            ATH_MSG_VERBOSE("chain: " << name << " Passed RAW");
            ++m_summary_passraw[i];
        }
        if ( m_trigDec->isPassed(name) ) {
            ATH_MSG_VERBOSE("chain: " << name << " Passed");
            ++m_summary_pass[i];
        }
        if ( m_trigDec->isPassed(name) ) {
            ATH_MSG_VERBOSE("chain: " << name << " Passed PHYSICS");
            ++m_summary_passphys[i];
        }
        
        
        // Reproduce the definitions of raw, with PS, with PT, as used in monitoring:
        // http://alxr.usatlas.bnl.gov/lxr/source/atlas/Trigger/TrigMonitoring/TrigSteerMonitor/src/TrigChainMoni.cxx#380
        // chainPassed == (chainPassedRaw() && !aChain->isPrescaled()) || isPassedThrough
        if (aChain->chainPassedRaw()){
            ++m_summary_chain_passraw[i];
        }
        if (aChain->chainPassedRaw() && !aChain->isPrescaled()){
            ++m_summary_chain_PS[i];
        }
        if (aChain->chainPassed()){
            ++m_summary_chain_PT[i];
        }
        if (!(aChain->isPrescaled())){ // get prescale fraction by counting events
            ++m_chain_prescales_calculated[i];
        }
        if (aChain->isPassedThrough()){ // get passthrough fraction by counting events
            ++m_chain_passthrough_calculated[i];
        }
        // events accepted by the lower chain
        std::map<std::string,std::string>::iterator lcIt=m_lower_chain_map.find(m_summary[i]);
        if (lcIt!=m_lower_chain_map.end()) {
            if ( !m_trigDec->getListOfTriggers((*lcIt).second).empty() &&m_trigDec->isPassed((*lcIt).second) ) {
                ++m_lower_chain_accept[i];
            }
        }
        // print info for each event
        if (m_event_decision_printout){
            msg(MSG::INFO) << "chain " << name << " = "
            << m_summary_chain_passraw[i] << endmsg;
        }
    }
    
    // TrigDecisionTool tests on a few specific sigs:
    for (unsigned int i=0; i < m_count_sig_names.size(); ++i) {
        ATH_MSG_DEBUG("Monitoring " << m_count_sig_names[i]);
        if ( !m_trigDec->getListOfTriggers(m_count_sig_names[i]).empty() ) {
            if ( m_trigDec->isPassed(m_count_sig_names[i]) )
                m_run_count_sigs[i] = m_run_count_sigs[i]+1;
        } else {
            msg(MSG::WARNING) << m_count_sig_names[i] << " not configured!" << endmsg;
        }
    }
    
    if (m_event_number%m_monitoring_block_size == 0) {
        m_count_event.push_back(m_event_number);
        
        for (unsigned int i=0; i < m_count_sig_names.size(); ++i) {
            (m_count_sigs[i])->push_back(m_run_count_sigs[i]);
            m_run_count_sigs[i]=0;
        }
    }
    
    // reset first event flag
    if (m_first_event) m_first_event = false;
    
    return StatusCode::SUCCESS;
}

template <class T>
StatusCode TrigDecisionChecker::checkEDM(const std::string& trigItem){
    ATH_MSG_INFO("REGTEST Check TrigPassBits for " << trigItem << " and type " << ClassID_traits< T >::typeName() );
    m_trigDec->isPassed(trigItem) ? ATH_MSG_INFO("REGTEST " << trigItem << " Passes ") : ATH_MSG_INFO("REGTEST " << trigItem << " Fails");
    const auto fc = m_trigDec->features(trigItem,TrigDefs::alsoDeactivateTEs);
    const auto vec = fc.get<T>();
    const auto vecbits = fc.get<xAOD::TrigPassBits>();
    std::string label="";
    for(const auto& feat:vecbits){
        const auto *xbits=feat.cptr();
        TrigConf::HLTTriggerElement::getLabel(feat.te()->getId(), label );
        const auto *cont=(m_trigDec->ancestor<T>(feat.te())).cptr();
        if(cont){
            ATH_MSG_INFO("REGTEST TrigPassBits for TE " << label  
                    <<  " size " << cont->size());

            if(cont->size()!=xbits->size()) {
                ATH_MSG_WARNING("REGTEST Container and bit size not equal, continue");
                continue;
            }
            int npassed=0;
            for(const auto &ptr:*cont){
                if(xbits->isPassing(ptr,cont))
                    npassed++;
            }
            ATH_MSG_INFO("REGTEST TrigPassBits for container type " << ClassID_traits< T >::typeName() 
                         <<  " size " << cont->size() << " xbits " << xbits->size() << " selected " << npassed);
        }
    }



    for(const auto& feat:vec){
        const auto *cont=feat.cptr();
        const TrigPassBits *bits=(m_trigDec->ancestor<TrigPassBits>(feat.te())).cptr();
        const xAOD::TrigPassBits *xbits=(m_trigDec->ancestor<xAOD::TrigPassBits>(feat.te())).cptr();
        TrigConf::HLTTriggerElement::getLabel(feat.te()->getId(), label );
        if(!cont){
            ATH_MSG_WARNING(label << "  " << ClassID_traits< T >::typeName() << "REGTEST: TrigPassBits container is null ");
            continue;
        }
        if(!bits){
            ATH_MSG_WARNING(label << " " << ClassID_traits< T >::typeName() << " REGTEST: TrigPassBits old bits null ");
        }
        if(!xbits){
            ATH_MSG_WARNING(label << " " << ClassID_traits< T >::typeName() << " REGTEST: TrigPassBits xbits null ");
            continue;
        }
        //
        const size_t bitlen = ( (cont->size() - 1)/32 ) + 1;
        if(bits) 
            ATH_MSG_INFO("REGTEST Retrieved container type " << ClassID_traits< T >::typeName() <<  " size " << cont->size() 
                    << " bits " << bits->size() << " Expect vector of bits " << bitlen);
        int npassed=0;
        if(xbits){
            for(const auto &ptr:*cont){
                if(xbits->isPassing(ptr,cont))
                    npassed++;
            }
            ATH_MSG_INFO("REGTEST TrigPassBits for container type " << ClassID_traits< T >::typeName() 
                    <<  " size " << cont->size() << " xbits " << xbits->size() << " selected " << npassed);
        }
    }
    
    return StatusCode::SUCCESS;
}

StatusCode TrigDecisionChecker::checkBjetEDM(const std::string& trigItem){
    
    ATH_MSG_INFO("REGTEST ==========START of bjet EDM/Navigation check for chain " << trigItem << " ===========");
    
    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    // Get feature container
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    
    // Get online combinations
    const std::vector< Trig::Combination >& bjetCombs = fc.getCombinations();
    ATH_MSG_INFO("REGTEST - RETRIEVED " << bjetCombs.size() << " COMBINATIONS FOR "  << trigItem);
    
    // Loop on combinations
    std::vector< Trig::Combination >::const_iterator bjetComb;
    for( bjetComb = bjetCombs.begin(); bjetComb != bjetCombs.end(); ++bjetComb ) {
        
        const Trig::Combination& comb = *bjetComb;
        ATH_MSG_INFO("REGTEST - ------------ NEW COMBINATION ------------");
        
        // Get online pv - histo
        const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs_histo = comb.get<xAOD::VertexContainer>("EFHistoPrmVtx");
        ATH_MSG_INFO("REGTEST - RETRIEVED PV (HISTO) -   size: " << onlinepvs_histo.size());
        if(onlinepvs_histo.size()) {
            const xAOD::VertexContainer* onlinepv_histo = onlinepvs_histo[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nVert: " << onlinepv_histo->size());
            if(onlinepv_histo->size()) {
                ATH_MSG_INFO("REGTEST -                  -   z[0]: " << (*(onlinepv_histo))[0]->z());
            }
        }
        
        // Get online pv - id tracking
        const std::vector< Trig::Feature<xAOD::VertexContainer> > onlinepvs_id = comb.get<xAOD::VertexContainer>("xPrimVx");
        ATH_MSG_INFO("REGTEST - RETRIEVED PV (IDTRK) -   size: " << onlinepvs_id.size());
        if(onlinepvs_id.size()) {
            const xAOD::VertexContainer* onlinepv_id = onlinepvs_id[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nVert: " << onlinepv_id->size());
            if(onlinepv_id->size()) {
                ATH_MSG_INFO("REGTEST -                  -   z[0]: " << (*(onlinepv_id))[0]->z());
            }
        }
        
        // Get online jet - from EFBjetSequenceAllTE
        const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets_efbjet = comb.get<xAOD::JetContainer>("TrigJetRec");
        ATH_MSG_INFO("REGTEST - RETRIEVED JETS (EFBJET)  -   size: " << onlinejets_efbjet.size());
        if(onlinejets_efbjet.size()) {
            const xAOD::JetContainer* onlinejet_efbjet = onlinejets_efbjet[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nJet: " << onlinejet_efbjet->size());
            for(const auto* jet_efbjet : *onlinejet_efbjet) {
                // Dump
                ATH_MSG_INFO("REGTEST -                  -   eta/phi: " << jet_efbjet->eta() << " / " << jet_efbjet->phi());
            }
        }
        
        // Get online jet - from JetSplitter
        const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets_split = comb.get<xAOD::JetContainer>("SplitJet");
        ATH_MSG_INFO("REGTEST - RETRIEVED JETS (SPLIT)  -   size: " << onlinejets_split.size());
        if(onlinejets_split.size()) {
            const xAOD::JetContainer* onlinejet_split = onlinejets_split[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nJet: " << onlinejet_split->size());
            for(const auto* jet_split : *onlinejet_split) {
                // Dump
                ATH_MSG_INFO("REGTEST -                  -   eta/phi: " << jet_split->eta() << " / " << jet_split->phi());
            }
        }
        
        // Get online jet - from SuperRoiBuilder (will this work for superROI ??)
        const std::vector< Trig::Feature<xAOD::JetContainer> > onlinejets_super = comb.get<xAOD::JetContainer>("SuperRoi");
        ATH_MSG_INFO("REGTEST - RETRIEVED JETS (SUPER)  -   size: " << onlinejets_super.size());
        if(onlinejets_super.size()) {
            const xAOD::JetContainer* onlinejet_super = onlinejets_super[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nJet: " << onlinejet_super->size());
            for(const auto* jet_super : *onlinejet_super) {
                // Dump
                ATH_MSG_INFO("REGTEST -                  -   eta/phi: " << jet_super->eta() << " / " << jet_super->phi());
            }
        }
        
        // Get online track particles (InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF)
        const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks_prmvtx = comb.get<xAOD::TrackParticleContainer>("InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF");
        ATH_MSG_INFO("REGTEST - RETRIEVED TRACKS (BJETPRMVTX_FTF) -   size: " << onlinetracks_prmvtx.size());
        if(onlinetracks_prmvtx.size()) {
            const xAOD::TrackParticleContainer* onlinetrack_prmvtx = onlinetracks_prmvtx[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nTrack: " << onlinetrack_prmvtx->size());
            for(const auto* trk : *onlinetrack_prmvtx) {
                // Dump
                uint8_t nInn=0, nNext=0, nPix=0, nSCT=0;
                trk->summaryValue(nInn,  xAOD::numberOfInnermostPixelLayerHits);
                trk->summaryValue(nNext, xAOD::numberOfNextToInnermostPixelLayerHits);
                trk->summaryValue(nPix,  xAOD::numberOfPixelHits);
                trk->summaryValue(nSCT,  xAOD::numberOfSCTHits);
                ATH_MSG_INFO("REGTEST -                  -   inn/next/pix/sct: " << (int)nInn << " / " << (int)nNext << " / " << (int)nPix << " / " << (int)nSCT);
            }
        }
        
        // Get online track particles (InDetTrigTrackingxAODCnv_Bjet_FTF)
        const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks_bjetftf = comb.get<xAOD::TrackParticleContainer>("InDetTrigTrackingxAODCnv_Bjet_FTF");
        ATH_MSG_INFO("REGTEST - RETRIEVED TRACKS (BJET_FTF) -   size: " << onlinetracks_bjetftf.size());
        if(onlinetracks_bjetftf.size()) {
            const xAOD::TrackParticleContainer* onlinetrack_bjetftf = onlinetracks_bjetftf[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nTrack: " << onlinetrack_bjetftf->size());
            for(const auto* trk : *onlinetrack_bjetftf) {
                // Dump
                uint8_t nInn=0, nNext=0, nPix=0, nSCT=0;
                trk->summaryValue(nInn,  xAOD::numberOfInnermostPixelLayerHits);
                trk->summaryValue(nNext, xAOD::numberOfNextToInnermostPixelLayerHits);
                trk->summaryValue(nPix,  xAOD::numberOfPixelHits);
                trk->summaryValue(nSCT,  xAOD::numberOfSCTHits);
                ATH_MSG_INFO("REGTEST -                  -   inn/next/pix/sct: " << (int)nInn << " / " << (int)nNext << " / " << (int)nPix << " / " << (int)nSCT);
            }
        }
        
        // Get online track particles (InDetTrigTrackingxAODCnv_Bjet_IDTrig)
        const std::vector< Trig::Feature<xAOD::TrackParticleContainer> > onlinetracks_bjetprec = comb.get<xAOD::TrackParticleContainer>("InDetTrigTrackingxAODCnv_Bjet_IDTrig");
        ATH_MSG_INFO("REGTEST - RETRIEVED TRACKS (BJET_IDTRIG) -   size: " << onlinetracks_bjetprec.size());
        if(onlinetracks_bjetprec.size()) {
            const xAOD::TrackParticleContainer* onlinetrack_bjetprec = onlinetracks_bjetprec[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nTrack: " << onlinetrack_bjetprec->size());
            for(const auto* trk : *onlinetrack_bjetprec) {
                // Dump
                uint8_t nInn=0, nNext=0, nPix=0, nSCT=0;
                trk->summaryValue(nInn,  xAOD::numberOfInnermostPixelLayerHits);
                trk->summaryValue(nNext, xAOD::numberOfNextToInnermostPixelLayerHits);
                trk->summaryValue(nPix,  xAOD::numberOfPixelHits);
                trk->summaryValue(nSCT,  xAOD::numberOfSCTHits);
                ATH_MSG_INFO("REGTEST -                  -   inn/next/pix/sct: " << (int)nInn << " / " << (int)nNext << " / " << (int)nPix << " / " << (int)nSCT);
            }
        }
        
        // Get online bjet
        const std::vector< Trig::Feature<xAOD::BTaggingContainer> > onlinebjets = comb.get<xAOD::BTaggingContainer>();
        ATH_MSG_INFO("REGTEST - RETRIEVED BJETS  -   size: " << onlinebjets.size());
        if(onlinebjets.size()) {
            const xAOD::BTaggingContainer* onlinebjet = onlinebjets[0].cptr();
            ATH_MSG_INFO("REGTEST -                  -   nBjet: " << onlinebjet->size());
            for(const auto* bjet : *onlinebjet) {
                // Dump
                double wIP2D, wIP3D, wSV1, wCOMB, wMV1;
                bjet->loglikelihoodratio("IP2D", wIP2D);
                bjet->loglikelihoodratio("IP3D", wIP3D);
                bjet->loglikelihoodratio("SV1", wSV1);
                wCOMB = wIP3D+wSV1;
                wMV1 = bjet->MV1_discriminant();
                ATH_MSG_INFO("REGTEST -                  -   IP2D/IP3D/SV1/IP3D+SV1/MV1: " << wIP2D << " / " << wIP3D << " / " << wSV1 << " / " << wCOMB << " / " << wMV1);
            }
        }
        
    }
    
    msg(MSG::INFO) << "REGTEST ==========END of bjet EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    return StatusCode::SUCCESS;
}//checkBjetEDM

StatusCode TrigDecisionChecker::checkMuonEDM(const std::string& trigItem){
    
    msg(MSG::INFO) << "REGTEST ==========START of muon EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    
    const std::vector< Trig::Feature<xAOD::MuonContainer> > vec_muons = fc.get<xAOD::MuonContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::MuonContainer> > = " << vec_muons.size());
    
    for( const auto& mufeat : vec_muons ) {
        ATH_MSG_INFO("REGTEST Got muon container, size = " << mufeat.cptr()->size());
        for(auto muItr : *(mufeat.cptr())) {      
            ATH_MSG_INFO("REGTEST MuonFeature with pt, eta, phi = " << muItr->pt() << ", " << muItr->eta() << ", " << muItr->phi());
        }
    }// loop over muon features
    
    const std::vector< Trig::Feature<xAOD::L2CombinedMuonContainer> > vec_L2CBmuons = fc.get<xAOD::L2CombinedMuonContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::L2CombinedMuonContainer> > = " << vec_L2CBmuons.size());
    
    for( const auto& l2cbmufeat : vec_L2CBmuons) {
        ATH_MSG_INFO("REGTEST Got muon container, size = " << l2cbmufeat.cptr()->size());
        for(auto l2cbmuItr : *(l2cbmufeat.cptr())) {      
            ATH_MSG_INFO("REGTEST MuonFeature with pt, eta, phi = " << l2cbmuItr->pt() << ", " << l2cbmuItr->eta() << ", " << l2cbmuItr->phi());
        }
    }// loop over muon L2 CB features
    
    const std::vector< Trig::Feature<xAOD::L2StandAloneMuonContainer> > vec_L2SAmuons = fc.get<xAOD::L2StandAloneMuonContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::L2StandAloneMuonContainer> > = " << vec_L2SAmuons.size());
    
    for( const auto& l2samufeat : vec_L2SAmuons) {
        ATH_MSG_INFO("REGTEST Got muon container, size = " << l2samufeat.cptr()->size());
        for(auto l2samuItr : *(l2samufeat.cptr())) {      
            ATH_MSG_INFO("REGTEST MuonFeature with pt, eta, phi = " << l2samuItr->pt() << ", " << l2samuItr->eta() << ", " << l2samuItr->phi());
        }
    }// loop over muon L2 SA features
    
    msg(MSG::INFO) << "REGTEST ==========END of muon EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    return StatusCode::SUCCESS;
}//checkMuonEDM

StatusCode TrigDecisionChecker::checkTauEDM(const std::string& trigItem){
    msg(MSG::INFO)<< "REGTEST ==========START of tau EDM/Navigation check for chain " << trigItem<< "===========" << endmsg;
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    const std::vector< Trig::Feature<xAOD::TauJetContainer> > vec_tauHLTClust = fc.get<xAOD::TauJetContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::TauJetContainer> > = " << vec_tauHLTClust.size());
    for(const auto& cont_tau : vec_tauHLTClust) {
        ATH_MSG_INFO("REGTEST Got Tau container, size = " << cont_tau.cptr()->size());
        
        for(auto tauItr : *(cont_tau.cptr())) {
            
            msg(MSG::INFO) << "REGTEST "<<" HLT tau number of tracks: "<<tauItr->nTracks()<<endmsg;
            msg(MSG::INFO)  << "REGTEST "<<" HLT tau pt : "<<tauItr->pt()<<endmsg;
            msg(MSG::INFO)  << "REGTEST "<<" HLT tau phi : "<<tauItr->phi()<<endmsg;
            msg(MSG::INFO)  << "REGTEST "<<" HLT tau eta : "<<tauItr->eta()<<endmsg;
            if( !tauItr->jetLink().isValid() ) {
                ATH_MSG_WARNING("tau does not have jet seed");
                return StatusCode::SUCCESS;
            }
            
            const xAOD::Jet* pJetSeed = *(tauItr->jetLink());   
            xAOD::JetConstituentVector::const_iterator clusItr  = pJetSeed->getConstituents().begin();
            xAOD::JetConstituentVector::const_iterator clusItrE = pJetSeed->getConstituents().end();   
            for (int clusCount = 0; clusItr != clusItrE; ++clusItr, ++clusCount) {     
                ATH_MSG_INFO( "REGTEST Tau Cluster " << clusCount << " pt = " << (*clusItr)->pt()<< " eta = " << (*clusItr)->eta()<< " phi = " << (*clusItr)->phi() );     
            }  
            for (unsigned int trackNum = 0;  trackNum < tauItr->nTracks(); ++trackNum) {
                
	        const xAOD::TrackParticle *linkTrack = tauItr->track(trackNum)->track();
                if (!linkTrack) {
                    ATH_MSG_WARNING("can't get tau linked track");
                    return StatusCode::SUCCESS;
                } else {
                    ATH_MSG_DEBUG("Got the tau linked track");
                }     
                ATH_MSG_INFO( "REGTEST Tau linked track " << trackNum << " pt = " << linkTrack->pt()<< " eta = " << linkTrack->eta() << " phi = " << linkTrack->phi() );
            }
            
        }
    }
    
    msg(MSG::INFO) << "REGTEST ==========END of Tau EDM/Navigation check ===========" << endmsg;
    return StatusCode::SUCCESS;        
}



StatusCode TrigDecisionChecker::checkBphysEDM(const std::string& trigItem){
    
    msg(MSG::INFO) << "REGTEST ==========START of Bphysics EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    
    const std::vector< Trig::Feature<xAOD::TrigBphysContainer> > fc_bphys = fc.get<xAOD::TrigBphysContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::TrigBphysContainer> > = " << fc_bphys.size());
    
    for( const auto& cont_bphys : fc_bphys ) {
        ATH_MSG_INFO("REGTEST Got Bphysics container, size = " << cont_bphys.cptr()->size());
        for ( auto bphys:  *(cont_bphys.cptr()) )  {
            ATH_MSG_INFO("REGTEST  Bphysics Item mass, fitmass, secVx, nTP: "
                         << bphys->mass()*0.001 << " , " << bphys->fitmass() * 0.001 << " , "
                         << bphys->secondaryDecay() << " , " << bphys->nTrackParticles()
                         );
        } // for
        
    }// loop over bphys features
    
    //    const std::vector< Trig::Feature<CombinedMuonFeature> > vec_cbmufeats = fc.get<CombinedMuonFeature>();
    //    ATH_MSG_INFO("Size of vector< Trig::Feature<CombinedMuonFeature> > = " << vec_cbmufeats.size());
    //
    //    for( auto cbmufeat : vec_cbmufeats) {
    //        ATH_MSG_INFO("REGTEST CombinedMuonFeature with pt, eta, phi = " << cbmufeat.cptr()->pt() << ", " << cbmufeat.cptr()->eta() << ", " << cbmufeat.cptr()->phi());
    //    }
    //    
    ATH_MSG_INFO("REGTEST ==========END of Bphysics EDM/Navigation check for chain " << trigItem << " ===========");
    
    return StatusCode::SUCCESS;
}//checkBphysEDM






StatusCode TrigDecisionChecker::checkElectronEDM(const std::string& trigItem){
    msg(MSG::INFO) << "REGTEST ==========START of Electron EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    const std::vector< Trig::Feature<xAOD::ElectronContainer> > vec_el = fc.get<xAOD::ElectronContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::ElectronContainer> > = " << vec_el.size());
    float val_float=-99.;
    for(const auto& elfeat : vec_el){
        ATH_MSG_INFO("REGTEST: Got electron container, size = " << elfeat.cptr()->size());
        const xAOD::ElectronContainer *elCont = elfeat.cptr();
        
        for(const auto& eg : *elCont){
            if (eg) {
                ATH_MSG_INFO(" REGTEST: egamma energy: " << eg->e() );
                ATH_MSG_INFO(" REGTEST: egamma eta: " << eg->eta() );
                ATH_MSG_INFO(" REGTEST: egamma phi: " << eg->phi() );
            } else{
                ATH_MSG_INFO(" REGTEST: problems with egamma pointer" );
                return StatusCode::SUCCESS;
            }
            ATH_MSG_INFO(" REGTEST: caloCluster variables ");
            if (eg->caloCluster()) {
                ATH_MSG_INFO(" REGTEST: egamma cluster transverse energy: " << eg->caloCluster()->et() );
                ATH_MSG_INFO(" REGTEST: egamma cluster eta: " << eg->caloCluster()->eta() );
                ATH_MSG_INFO(" REGTEST: egamma cluster phi: " << eg->caloCluster()->phi() );
                double tmpeta = -999.;
                double tmpphi = -999.;
                eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
                eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi); 
                ATH_MSG_INFO(" REGTEST: egamma cluster calo-frame coords. etaCalo = " << tmpeta); 
                ATH_MSG_INFO(" REGTEST: egamma cluster calo-frame coords. phiCalo = " << tmpphi);
            } else{
                ATH_MSG_INFO(" REGTEST: problems with egamma cluster pointer" );
            }
            ATH_MSG_INFO(" REGTEST: trackmatch variables ");
            if(eg->trackParticle()){
                ATH_MSG_INFO(" REGTEST: pt=  " << eg->trackParticle()->pt());
                ATH_MSG_INFO(" REGTEST: charge=  " << eg->trackParticle()->charge());
                ATH_MSG_INFO(" REGTEST: E/p=  " << eg->caloCluster()->et() / eg->trackParticle()->pt() );
                eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaEta1);
                ATH_MSG_INFO(" REGTEST: Delta eta 1st sampling=  " << val_float);
                eg->trackCaloMatchValue(val_float,xAOD::EgammaParameters::deltaPhi2);
                ATH_MSG_INFO(" REGTEST: Delta phi 2nd sampling=  " << val_float);
            } else{
                ATH_MSG_INFO(" REGTEST: no electron eg->trackParticle() pointer");
            }
        }
    }
    
    msg(MSG::INFO) << "REGTEST ==========END of Electron EDM/Navigation check ===========" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode TrigDecisionChecker::checkPhotonEDM(const std::string& trigItem){
    msg(MSG::INFO) << "REGTEST ==========START of Photon EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    const std::vector< Trig::Feature<xAOD::PhotonContainer> > vec_ph = fc.get<xAOD::PhotonContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::PhotonContainer> > = " << vec_ph.size());
    //float val_float=-99.;
    for(const auto& phfeat : vec_ph){
        ATH_MSG_INFO("REGTEST: Got photon container, size = " << phfeat.cptr()->size());
        const xAOD::PhotonContainer *phCont = phfeat.cptr();
        
        for(const auto& eg : *phCont){
            if (eg) {
                ATH_MSG_INFO(" REGTEST: egamma energy: " << eg->e() );
                ATH_MSG_INFO(" REGTEST: egamma eta: " << eg->eta() );
                ATH_MSG_INFO(" REGTEST: egamma phi: " << eg->phi() );
            } else{
                ATH_MSG_INFO(" REGTEST: problems with egamma pointer" );
                return StatusCode::SUCCESS;
            }
            ATH_MSG_INFO(" REGTEST: caloCluster variables ");
            if (eg->caloCluster()) {
                ATH_MSG_INFO(" REGTEST: egamma cluster transverse energy: " << eg->caloCluster()->et() );
                ATH_MSG_INFO(" REGTEST: egamma cluster eta: " << eg->caloCluster()->eta() );
                ATH_MSG_INFO(" REGTEST: egamma cluster phi: " << eg->caloCluster()->phi() );
                double tmpeta = -999.;
                double tmpphi = -999.;
                eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
                eg->caloCluster()->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi); 
                ATH_MSG_INFO(" REGTEST: egamma cluster calo-frame coords. etaCalo = " << tmpeta); 
                ATH_MSG_INFO(" REGTEST: egamma cluster calo-frame coords. phiCalo = " << tmpphi);
            } else{
                ATH_MSG_INFO(" REGTEST: problems with egamma cluster pointer" );
            }
        }
    }
    
    msg(MSG::INFO) << "REGTEST ==========END of Photon EDM/Navigation check ===========" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode TrigDecisionChecker::checkMinBiasEDM(const std::string& trigItem){
    msg(MSG::INFO) << "REGTEST ==========START of MinBias EDM/Navigation check for chain " << trigItem << " ===========" << endmsg;
    
    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    
    checkTrigSpacePointCounts(fc);
    checkTrigT2MBTSBits(fc);
    checkTrigVertexCounts(fc);	
    checkTrigTrackCounts(fc);
    
    msg(MSG::INFO) << "REGTEST ==========END of MinBias EDM/Navigation check ===========" << endmsg;
    return StatusCode::SUCCESS;
}

void TrigDecisionChecker::checkTrigSpacePointCounts(const Trig::FeatureContainer& fc){
    
    const std::vector< Trig::Feature<xAOD::TrigSpacePointCounts> > vec_sp = fc.get<xAOD::TrigSpacePointCounts>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::TrigSpacePointCounts> > = " << vec_sp.size());
    
    float sum;
    auto fun = [&sum](const std::vector<float>& vec){sum = 0.; for (const auto &i: vec) sum += i; return sum;};
    
    for(const auto& spfeat : vec_sp){
        const xAOD::TrigSpacePointCounts *spCounts = spfeat.cptr();
        if (spCounts){
            ATH_MSG_INFO(" REGTEST: SUM of contentsPixelClusEndcapC: " << fun(spCounts->contentsPixelClusEndcapC()) );
            ATH_MSG_INFO(" REGTEST: SUM of contentsPixelClusBarrel: " << fun(spCounts->contentsPixelClusBarrel()) );
            ATH_MSG_INFO(" REGTEST: SUM of contentsPixelClusEndcapA: " << fun(spCounts->contentsPixelClusEndcapA()) );
            
            ATH_MSG_INFO(" REGTEST: pixelClusTotBins: " << spCounts->pixelClusTotBins() );
            ATH_MSG_INFO(" REGTEST: pixelClusTotMin: " << spCounts->pixelClusTotMin() );
            ATH_MSG_INFO(" REGTEST: pixelClusTotMax: " << spCounts->pixelClusTotMax() );
            ATH_MSG_INFO(" REGTEST: pixelClusSizeBins: " << spCounts->pixelClusSizeBins() );
            ATH_MSG_INFO(" REGTEST: pixelClusSizeMin: " << spCounts->pixelClusSizeMin() );
            ATH_MSG_INFO(" REGTEST: pixelClusSizeMax: " << spCounts->pixelClusSizeMax() );
            ATH_MSG_INFO(" REGTEST: sctSpEndcapC: " << spCounts->sctSpEndcapC() );
            ATH_MSG_INFO(" REGTEST: sctSpBarrel: " << spCounts->sctSpBarrel() );	  
            ATH_MSG_INFO(" REGTEST: sctSpEndcapA: " << spCounts->sctSpEndcapA() );
        } else{
            ATH_MSG_INFO(" REGTEST: problems with TrigSpacePointCounts pointer" );
            return;
        }
    }
}

void TrigDecisionChecker::checkTrigT2MBTSBits(const Trig::FeatureContainer& fc){
    
    const std::vector< Trig::Feature<xAOD::TrigT2MbtsBits> > vec_mbts = fc.get<xAOD::TrigT2MbtsBits>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::TrigT2MbtsBits> > = " << vec_mbts.size());
    
    float sum;
    auto fun = [&sum](const std::vector<float>& vec){sum = 0.; for (const auto &i: vec) sum += i; return sum;};
    
    for(const auto& mbtsfeat : vec_mbts){
        const xAOD::TrigT2MbtsBits *mbtsBits = mbtsfeat.cptr();
        if (mbtsBits){
            ATH_MSG_INFO(" REGTEST: SUM of triggerEnergies: " << fun(mbtsBits->triggerEnergies()) );
            ATH_MSG_INFO(" REGTEST: SUM of triggerTimes: " << fun(mbtsBits->triggerTimes()) );
        } else{
            ATH_MSG_INFO(" REGTEST: problems with TrigT2MBTSBits pointer" );
            return;
        }
    }
}

void TrigDecisionChecker::checkTrigVertexCounts(const Trig::FeatureContainer& fc){
    
    const std::vector< Trig::Feature<xAOD::TrigVertexCounts> > vec_v = fc.get<xAOD::TrigVertexCounts>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::TrigVertexCounts> > = " << vec_v.size());
    
    auto funu = [](const std::vector<uint>& vec){uint sum = 0; for (const auto &i: vec) sum += i; return sum;};
    auto funf = [](const std::vector<float>& vec){float sum = 0.; for (const auto &i: vec) sum += i; return sum;};
    
    for(const auto& vfeat : vec_v){
        const xAOD::TrigVertexCounts *vCounts = vfeat.cptr();
        if (vCounts){
            ATH_MSG_INFO(" REGTEST: SUM of vtxNtrks: " << funu(vCounts->vtxNtrks()) );
            ATH_MSG_INFO(" REGTEST: SUM of vtxTrkPtSqSum: " << funf(vCounts->vtxTrkPtSqSum()) );
        } else{
            ATH_MSG_INFO(" REGTEST: problems with TrigVertexCounts pointer" );
            return;
        }
    }
}

void TrigDecisionChecker::checkTrigTrackCounts(const Trig::FeatureContainer& fc){
    
    const std::vector< Trig::Feature<xAOD::TrigTrackCounts> > vec_t = fc.get<xAOD::TrigTrackCounts>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::TrigTrackCounts> > = " << vec_t.size());
    
    float sum;
    auto fun = [&sum](const std::vector<float>& vec){sum = 0.; for (const auto &i: vec) sum += i; return sum;};
    
    for(const auto& tfeat : vec_t){
        const xAOD::TrigTrackCounts *tCounts = tfeat.cptr();
        if (tCounts){
            ATH_MSG_INFO(" REGTEST: SUM of z0_pt: " << fun(tCounts->z0_pt()) );
            ATH_MSG_INFO(" REGTEST: SUM of eta_phi: " << fun(tCounts->eta_phi()) );
            
            ATH_MSG_INFO(" REGTEST: z0Bins: " << tCounts->z0Bins() );
            ATH_MSG_INFO(" REGTEST: z0Min: " << tCounts->z0Min() );
            ATH_MSG_INFO(" REGTEST: z0Max: " << tCounts->z0Max() );
            ATH_MSG_INFO(" REGTEST: ptBins: " << tCounts->ptBins() );
            ATH_MSG_INFO(" REGTEST: ptMin: " << tCounts->ptMin() );
            ATH_MSG_INFO(" REGTEST: ptMax: " << tCounts->ptMax() );
            ATH_MSG_INFO(" REGTEST: etaBins: " << tCounts->etaBins() );
            ATH_MSG_INFO(" REGTEST: etaMin: " << tCounts->etaMin() );
            ATH_MSG_INFO(" REGTEST: etaMax: " << tCounts->etaMax() );
        } else{
            ATH_MSG_INFO(" REGTEST: problems with TrigTrackCounts pointer" );
            return;
        }
    }
}

StatusCode TrigDecisionChecker::checkJetEDM(const std::string& trigItem){
    ATH_MSG_DEBUG("in checkJetEDM()");
    
    ATH_MSG_INFO("REGTEST =====For chain " << trigItem << "=====");
    
    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));
    
    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    const std::vector< Trig::Feature<xAOD::JetContainer> > vec_jet = fc.get<xAOD::JetContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::JetContainer> > = " << vec_jet.size());
    for(const auto& jetfeat : vec_jet){
        const xAOD::JetContainer * jetCont = jetfeat.cptr();
        
        int jetContsize = jetCont->size();
        ATH_MSG_INFO("REGTEST Got jet container, size: " << jetContsize);
        
        int i = 0;
        for(const auto & thisjet : *jetCont){
            ++i;
            ATH_MSG_INFO("REGTEST Looking at jet " << i);
            if (thisjet) {
                //checks jet variables
                ATH_MSG_DEBUG("REGTEST    Checking jet variables");
                ATH_MSG_INFO("REGTEST    pt: " << thisjet->pt() );
                ATH_MSG_INFO("REGTEST    eta: " << thisjet->eta() );
                ATH_MSG_INFO("REGTEST    phi: " << thisjet->phi() );
                ATH_MSG_INFO("REGTEST    m: " << thisjet->m() );
                ATH_MSG_INFO("REGTEST    e: " << thisjet->e() );
                ATH_MSG_INFO("REGTEST    px: " << thisjet->px() );
                ATH_MSG_INFO("REGTEST    py: " << thisjet->py() );
                ATH_MSG_INFO("REGTEST    pz: " << thisjet->pz() );
                ATH_MSG_INFO("REGTEST    type: " << thisjet->type() );
                ATH_MSG_INFO("REGTEST    algorithm (kt: 0, cam: 1, antikt: 2, ...): " << thisjet->getAlgorithmType() );
                ATH_MSG_INFO("REGTEST    size parameter: " << thisjet->getSizeParameter() );
                ATH_MSG_INFO("REGTEST    input (LCTopo: 0, EMTopo: 1, ...): " << thisjet->getInputType() );
                ATH_MSG_INFO("REGTEST    constituents signal state (uncalibrated: 0, calibrated: 1): " << thisjet->getConstituentsSignalState() );
                ATH_MSG_INFO("REGTEST    number of constituents: " << thisjet->numConstituents() );
            }
            else{
                ATH_MSG_ERROR("REGTEST Problem with jet pointer" );
                return StatusCode::FAILURE;
            }
        }
        if (jetContsize == i) ATH_MSG_INFO("REGTEST size of jet container == number of displayed jets: " << (jetContsize == i) );
        else ATH_MSG_WARNING("REGTEST Problem with displaying jets");
    }
    
    ATH_MSG_DEBUG("leaving checkJetEDM()");
    
    return StatusCode::SUCCESS;
}

StatusCode TrigDecisionChecker::checkMetEDM(const std::string& trigItem){
    ATH_MSG_DEBUG("in checkMetEDM()");

    ATH_MSG_INFO("REGTEST =====For chain " << trigItem << "=====");

    ATH_MSG_INFO("Chain passed = " << m_trigDec->isPassed(trigItem));

    Trig::FeatureContainer fc = m_trigDec->features(trigItem);
    const std::vector< Trig::Feature<xAOD::TrigMissingETContainer> > vec_met = fc.get<xAOD::TrigMissingETContainer>();
    ATH_MSG_INFO("Size of vector< Trig::Feature<xAOD::TrigMissingETContainer> > = " << vec_met.size());
    for(const auto& metfeat : vec_met){
        const xAOD::TrigMissingETContainer * metCont = metfeat.cptr();

        ATH_MSG_INFO("REGTEST Fot trigMet container, size: " << metCont->size());

        const xAOD::TrigMissingET * metObj = metCont->front(); // the first object in the container typically holds the total MET, other objects hold components

        if (metObj){
            ATH_MSG_DEBUG("REGTEST    Checking met variables");
            ATH_MSG_INFO("REGTEST    ex: " << metObj->ex() );
            ATH_MSG_INFO("REGTEST    ey: " << metObj->ey() );
            ATH_MSG_INFO("REGTEST    sumEt: " << metObj->sumEt() );
            ATH_MSG_INFO("REGTEST    ez: " << metObj->ez() );
            ATH_MSG_INFO("REGTEST    sumE: " << metObj->sumE() );
            ATH_MSG_INFO("REGTEST    number of components: " << metObj->getNumberOfComponents() );
        }
        else{
            ATH_MSG_ERROR("REGTEST Problem with met pointer" );
            return StatusCode::FAILURE;
        }
    }
    
    ATH_MSG_DEBUG("leaving checkMetEDM()");

    return StatusCode::SUCCESS;
}
