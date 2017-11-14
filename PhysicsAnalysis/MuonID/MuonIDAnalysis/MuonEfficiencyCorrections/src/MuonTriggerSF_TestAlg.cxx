/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */
#include "MuonEfficiencyCorrections/MuonSFTestHelper.h"
#include "PATInterfaces/SystematicsUtil.h"

#include "MuonTriggerSF_TestAlg.h"


namespace Trig{
    //=============================================================================
    MuonTriggerSF_TestAlg::MuonTriggerSF_TestAlg(const std::string& name,
                    ISvcLocator* pSvcLocator) :
                    AthAlgorithm(name, pSvcLocator),
                    m_outputStream(""),
                    m_inputContainerName(""),
                    m_triggers(),
                    m_branches(),
                    m_histSvc("THistSvc/THistSvc", name),
                    m_trigEff(""),
                    m_prwTool(""),
                    m_tree(nullptr),
                    m_runNumber(-1) {
        declareProperty("outputStream", m_outputStream);
        declareProperty("inputContainerName", m_inputContainerName);
        declareProperty("triggers" , m_triggers);
       
        declareProperty("TriggerEfficiencyTool", m_trigEff);
        declareProperty("prwTool", m_prwTool);
        
    }

    //=============================================================================
    StatusCode MuonTriggerSF_TestAlg::initialize() {
        CHECK(m_histSvc.retrieve());
        CHECK(m_trigEff.retrieve());
        CHECK(m_prwTool.retrieve());
        
        //Create the tree and register it to the hist-service
        std::string tree_key = "/TRIGGERSFTESTER/" + m_outputStream ;
        m_tree = new TTree (m_outputStream.c_str(), "TriggerSFTesterTree");
        std::cout<<"Deine Mama"<<std::endl;
        m_tree->Branch("runNumber", &m_runNumber);
        
        
        for (const auto& trig: m_triggers){
            m_branches.push_back(TestMuonSF::TriggerSFBranch_Ptr( new TestMuonSF::TriggerSFBranches(m_tree, m_trigEff, trig) ));
        }
        m_triggers.clear();
        for (const auto& branch : m_branches){
            if(!branch->init()) return StatusCode::FAILURE;
        }
        
        for (CP::SystematicVariation syst : m_trigEff->affectingSystematics()) {
           ATH_MSG_INFO("Recommended systematic: " << syst.name());
        }
        ATH_CHECK(m_histSvc->regTree(tree_key.c_str(), m_tree));
        return StatusCode::SUCCESS;
    }

    //=============================================================================
    StatusCode MuonTriggerSF_TestAlg::execute() {
        
        //Retrieve the EventInfo to apply the prwTool before activating the trigger tool
        const xAOD::EventInfo* Info;
        ATH_CHECK(evtStore()->retrieve(Info,"EventInfo"));
        ATH_CHECK(m_prwTool->apply(*Info));
        m_runNumber = m_prwTool->getRandomRunNumber(*Info);
        
        const xAOD::MuonContainer* Muons;
        ATH_CHECK(evtStore()->retrieve(Muons,"Muons"));
        if (Muons->empty()) return StatusCode::SUCCESS;
        
        for (const auto& branch : m_branches){
            if (branch->fill(Muons) != CP::CorrectionCode::Ok) return StatusCode::FAILURE;
        }
        m_tree->Fill();
        
        return StatusCode::SUCCESS;
    }

    //=============================================================================
    StatusCode MuonTriggerSF_TestAlg::finalize() {     
        return StatusCode::SUCCESS;
    }
}
