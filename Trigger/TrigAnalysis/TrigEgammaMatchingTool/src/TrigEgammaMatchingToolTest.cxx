/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TrigEgammaMatchingToolTest.cxx 

#include "TrigEgammaMatchingToolTest.h"
#include "GaudiKernel/Property.h"
using std::string;

//**********************************************************************
namespace Trig{
    TrigEgammaMatchingToolTest::
        TrigEgammaMatchingToolTest(const std::string& name, 
                ISvcLocator* pSvcLocator )
        : ::AthAlgorithm( name, pSvcLocator ),
        m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
        m_matchTool("Trig::TrigEgammaMatchingTool/TrigEgammaMatchingTool",this)
            //m_matchTool2("TriggerMatchingTool/MatchingTool",this)
    {
        declareProperty("TrigEgammaMatchingTool",m_matchTool);
        //declareProperty("TriggerMatchingTool",m_matchTool2);
        declareProperty("TriggerList",m_triggerList);
        declareProperty("CombinedTriggerList",m_cmbTriggerList);
    }

    //**********************************************************************

    TrigEgammaMatchingToolTest::~TrigEgammaMatchingToolTest() { }

    //**********************************************************************

    StatusCode TrigEgammaMatchingToolTest::initialize() {
        ATH_MSG_INFO("Initializing " << name() << "...");
        ATH_MSG_INFO("Retrieving tools...");
        if ( (m_trigdec.retrieve()).isFailure() ){
            ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
            return StatusCode::FAILURE;
        }
        StatusCode sc = service("StoreGateSvc", m_storeGate);
        if(sc.isFailure()) {
            ATH_MSG_ERROR( "Unable to locate Service StoreGateSvc" );
            return sc;
        }
        return StatusCode::SUCCESS;
    }

    //**********************************************************************

    StatusCode TrigEgammaMatchingToolTest::finalize() {
        ATH_MSG_INFO ("Finalizing " << name() << "...");
        for (auto iter = m_counterBits.begin(); iter != m_counterBits.end(); iter++) {
            ATH_MSG_DEBUG(iter->first << " == " << iter->second);
        }
        for (auto iter = m_counterMatch3Bits.begin(); iter != m_counterMatch3Bits.end(); iter++) {
            ATH_MSG_INFO("REGTEST: " << iter->first << " " << iter->second << " " << m_counterMatch2Bits[iter->first] << " " <<  m_counterMatch1Bits[iter->first] << " Generic match " << m_counterMatch4Bits[iter->first]); 
        }
        for (auto iter = m_counterCmbMatchBits.begin(); iter != m_counterCmbMatchBits.end(); iter++) {
            ATH_MSG_INFO("REGTEST: " << iter->first << " == " << iter->second);
        }

        return StatusCode::SUCCESS;
    }

    //**********************************************************************

    void TrigEgammaMatchingToolTest::match(const std::string trigger,const xAOD::Egamma *eg){

        
        if (eg) {
            if(m_matchTool->match(eg,trigger)){
                ATH_MSG_DEBUG("REGTEST:: Method 1 Matched Electron with tool for " << trigger);
                m_counterMatch1Bits[trigger]++;
            }
            else ATH_MSG_DEBUG("REGTEST::Fails method 1 " << trigger);
#ifdef XAOD_ANALYSIS
            const HLT::TriggerElement *finalFC; 
            if(m_matchTool->match(eg,trigger,finalFC)){
                ATH_MSG_DEBUG("REGTEST:: Method 2 Matched Electron with tool for " << trigger);
                if ( finalFC != NULL ){ 
                    if ( (m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC)).te() != NULL ){ 
                        if( (m_trigdec->ancestor<xAOD::ElectronContainer>(finalFC)).te()->getActiveState()){
                            ATH_MSG_DEBUG("REGTEST::Passed Matching method 2 for " << trigger);
                            m_counterMatch2Bits[trigger]++;
                        }
                        else ATH_MSG_DEBUG("REGTEST::Fails method 2");
                    }
                }
            }
#endif            
            if(m_matchTool->matchHLT(eg,trigger)){
                ATH_MSG_DEBUG("REGTEST:: Method 3 Matched Electron with tool for " << trigger);
                m_counterMatch3Bits[trigger]++;
            }
            else ATH_MSG_DEBUG("REGTEST::Fails method 3");
            /*std::vector<const xAOD::IParticle *> reco;
            reco.push_back(eg);
            if(m_matchTool2->match(reco,trigger)){
                ATH_MSG_DEBUG("REGTEST::  Generic Matched Electron with tool for " << trigger);
                m_counterMatch4Bits[trigger]++;
            }
            else ATH_MSG_DEBUG("REGTEST::Fails generic tool");*/
        }
        else ATH_MSG_DEBUG("REGTEST: eg pointer null!");
    }

    StatusCode TrigEgammaMatchingToolTest::execute() {   
        ATH_MSG_INFO ("Executing " << name() << "...");
        ATH_MSG_VERBOSE( "L1: " << m_trigdec->isPassed( "L1_.*" )
                << ", L2: " << m_trigdec->isPassed( "L2_.*" )
                << ", EF: " << m_trigdec->isPassed( "EF_.*" )
                << ", HLT: " << m_trigdec->isPassed( "HLT_.*" ) );
        auto chainGroups = m_trigdec->getChainGroup("HLT_e.*");

        for(auto &trig : chainGroups->getListOfTriggers()) {
            if(m_trigdec->isPassed(trig))
                ATH_MSG_VERBOSE("Passed: " << trig);
            m_counterBits[trig]+=m_trigdec->isPassed(trig);
        }

        //Check Containers

        const xAOD::ElectronContainer *offElectrons = 0;
        const xAOD::TauJetContainer *taus = 0;
        const xAOD::MuonContainer *muons = 0;
        if ( (m_storeGate->retrieve(offElectrons,"Electrons")).isFailure() ){
            ATH_MSG_DEBUG("Failed to retrieve offline Electrons ");
        }
        if ( (m_storeGate->retrieve(taus,"")).isFailure() ){
            ATH_MSG_DEBUG("Failed to retrieve offline Taus ");
        }
        if ( (m_storeGate->retrieve(muons,"Muons")).isFailure() ){
            ATH_MSG_DEBUG("Failed to retrieve offline Muons ");
        }
        if(offElectrons) ATH_MSG_INFO("Offline Electron container size " << offElectrons->size());
        if(muons) ATH_MSG_INFO("Offline Muon container size " << muons->size());
        if(taus) ATH_MSG_INFO("Offline Tau container size " << taus->size());
        for(const auto &trigger : m_triggerList){
            if(!offElectrons) continue;
            for(const auto& eg : *offElectrons){
                match(trigger,eg);
            } //End loop of offline electrons
        } // End loop over trigger list
        
        // Test combined triggers with generic tool
        /*for(const auto &trigger : m_cmbTriggerList){
            std::vector<const xAOD::IParticle *> reco;
            
            for(const auto& eg : *offElectrons){
                // e-e
                for(const auto& eg2 : *offElectrons){
                    if(eg==eg2) continue;
                    reco.clear();
                    reco.push_back(eg);
                    reco.push_back(eg2);
                    if(m_matchTool2->match(reco,trigger)){
                        ATH_MSG_DEBUG("REGTEST::Passed combined matching method for " << trigger);
                        m_counterCmbMatchBits[trigger]++;
                    }
                    else ATH_MSG_DEBUG("REGTEST::Failed combined method for " << trigger);
                    
                }
                // e-mu
                if(muons){
                    for(const auto& mu : *muons){
                        reco.clear();
                        reco.push_back(eg);
                        reco.push_back(mu);
                        if(m_matchTool2->match(reco,trigger)) {
                            ATH_MSG_DEBUG("REGTEST::Passed combined matching method for " << trigger);
                            m_counterCmbMatchBits[trigger]++;
                        }
                        else ATH_MSG_DEBUG("REGTEST::Failed combined method for " << trigger);

                    }
                }
                // e-tau
                if(taus){
                    for(const auto& tau : *taus){
                        reco.clear();
                        reco.push_back(eg);
                        reco.push_back(tau);
                        if(m_matchTool2->match(reco,trigger)){
                            ATH_MSG_DEBUG("REGTEST::Passed combined matching method for " << trigger);
                            m_counterCmbMatchBits[trigger]++;
                        }
                        else ATH_MSG_DEBUG("REGTEST::Failed combined method for " << trigger);

                    }
                }
            }
        }*/


        return StatusCode::SUCCESS;
    } // End execute
} //End namespace Trig
//**********************************************************************
