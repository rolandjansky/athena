/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//// Developed by Arthur Lesage (arthur.lesage@cern.ch)

// Local include(s):
#include "TestIsolationCloseByCorrAthenaAlg.h"

// EDM include(s):
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

const char* APP_NAME = "ISOLATIONCLOSEBYCORRTEST";

namespace CP {

    TestIsolationCloseByCorrAthenaAlg::TestIsolationCloseByCorrAthenaAlg(const std::string& name, ISvcLocator* svcLoc) :
        AthAlgorithm(name, svcLoc) {  
        declareProperty("IsoCloseByCorrTool", m_isoCloseByCorrTool);
        declareProperty("IsoSelectorTool", m_isoSelectorTool);
    }

    StatusCode TestIsolationCloseByCorrAthenaAlg::initialize() {
        ATH_CHECK(m_isoSelectorTool.retrieve());
        ATH_CHECK(m_isoCloseByCorrTool.retrieve());
        return StatusCode::SUCCESS;  
    }

    StatusCode TestIsolationCloseByCorrAthenaAlg::execute() {

        std::vector<xAOD::Iso::IsolationType> types;
        std::vector<const char*> typeNames;
        types.push_back(xAOD::Iso::IsolationType::ptcone20);
        typeNames.push_back("ptcone20");
        types.push_back(xAOD::Iso::IsolationType::ptcone30);
        typeNames.push_back("ptcone30");
        types.push_back(xAOD::Iso::IsolationType::ptcone40);
        typeNames.push_back("ptcone40");
        types.push_back(xAOD::Iso::IsolationType::ptvarcone20);
        typeNames.push_back("ptvarcone20");
        types.push_back(xAOD::Iso::IsolationType::ptvarcone30);
        typeNames.push_back("ptvarcone30");
        types.push_back(xAOD::Iso::IsolationType::ptvarcone40);
        typeNames.push_back("ptvarcone40");
        types.push_back(xAOD::Iso::IsolationType::topoetcone20);
        typeNames.push_back("topoetcone20");
        types.push_back(xAOD::Iso::IsolationType::topoetcone30);
        typeNames.push_back("topoetcone30");
        types.push_back(xAOD::Iso::IsolationType::topoetcone40);
        typeNames.push_back("topoetcone40");
//         types.push_back(xAOD::Iso::IsolationType::etcone20);
//         typeNames.push_back("etcone20");
//         types.push_back(xAOD::Iso::IsolationType::etcone30);
//         typeNames.push_back("etcone30");
//         types.push_back(xAOD::Iso::IsolationType::etcone40);
//         typeNames.push_back("etcone40");

        // get muon container of interest
        const xAOD::MuonContainer* muons = 0;
        ATH_CHECK(evtStore()->retrieve(muons, "Muons"));
     
        // Stores the muons in a vector.
        std::vector<const xAOD::IParticle*> muonsVec;
        for(auto muon: *muons) {
            muonsVec.push_back((const xAOD::IParticle*) muon);
        }
     
        // get electron container of interest
        const xAOD::ElectronContainer* electrons = 0;
        ATH_CHECK( evtStore()->retrieve(electrons, "Electrons") );
     
        // Stores the electrons in a vector.
        std::vector<const xAOD::IParticle*> electronsVec;
        for(auto electron: *electrons) {
            electronsVec.push_back((const xAOD::IParticle*) electron);
        }
     
        // get photon container of interest
        const xAOD::PhotonContainer* photons = 0;
        ATH_CHECK( evtStore()->retrieve(photons, "Photons") );
     
        // Stores the electrons in a vector.
        std::vector<const xAOD::IParticle*> photonsVec;
        for(auto photon: *photons) {
            photonsVec.push_back((const xAOD::IParticle*) photon);
        }

        for(auto muon: *muons){
            Info(APP_NAME, "---------NEW MUON -------");
        
            if (m_isoCloseByCorrTool->acceptCorrected(*muon, muonsVec)) {
                Info(APP_NAME, "Muon passes Loose working point after correction.");
            } else {
                Info(APP_NAME, "Muon does not pass Loose working point after correction.");
            }
        
            if (m_isoSelectorTool->accept(*muon)) {
                Info(APP_NAME, "Muon passes Loose working point before correction.");
            } else {
                Info(APP_NAME, "Muon does not pass Loose working point before correction.");
            }
      
            // Calculates the corrections.
            std::vector<Float_t> corrections;
      
            m_isoCloseByCorrTool->getCloseByCorrection(corrections, *muon, types, muonsVec);
      
            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < corrections.size()) {
                    float value = -999999;
                    muon->isolation(value, types.at(j));
                    float afterCorrection = value - corrections.at(j);
                    Info(APP_NAME, "Muon Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Muon Value, correction, corrected value: %f, %f, %f", value, corrections.at(j), afterCorrection);
                }
            }
        }
    
        for(auto electron: *electrons){
            Info(APP_NAME, "---------NEW ELECTRON -------");
        
            if (m_isoCloseByCorrTool->acceptCorrected(*electron, electronsVec)) {
                Info(APP_NAME, "Electron passes Loose working point after correction.");
            } else {
                Info(APP_NAME, "Electron does not pass Loose working point after correction.");
            }
        
            if (m_isoSelectorTool->accept(*electron)) {
                Info(APP_NAME, "Electron passes Loose working point before correction.");
            } else {
                Info(APP_NAME, "Electron does not pass Loose working point before correction.");
            }
      
            // Calculates the corrections.
            std::vector<Float_t> corrections;
      
            m_isoCloseByCorrTool->getCloseByCorrection(corrections, *electron, types, electronsVec);
      
            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < corrections.size()) {
                    float value = -999999;
                    electron->isolation(value, types.at(j));
                    float afterCorrection = value - corrections.at(j);
                    Info(APP_NAME, "Electron Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Electron Value, correction, corrected value: %f, %f, %f", value, corrections.at(j), afterCorrection);
                }
            }
        }
        
        for(auto photon: *photons){
            Info(APP_NAME, "---------NEW PHOTON -------");
        
            if (m_isoCloseByCorrTool->acceptCorrected(*photon, photonsVec)) {
                Info(APP_NAME, "Photon passes FixedCutTightCaloOnly working point after correction.");
            } else {
                Info(APP_NAME, "Photon does not pass FixedCutTightCaloOnly working point after correction.");
            }
        
            if (m_isoSelectorTool->accept(*photon)) {
                Info(APP_NAME, "Photon passes FixedCutTightCaloOnly working point before correction.");
            } else {
                Info(APP_NAME, "Photon does not pass FixedCutTightCaloOnly working point before correction.");
            }
      
            // Calculates the corrections.
            std::vector<Float_t> corrections;
      
            m_isoCloseByCorrTool->getCloseByCorrection(corrections, *photon, types, photonsVec);
      
            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < corrections.size()) {
                    float value = -999999;
                    photon->isolation(value, types.at(j));
                    float afterCorrection = value - corrections.at(j);
                    Info(APP_NAME, "Photon Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Photon Value, correction, corrected value: %f, %f, %f", value, corrections.at(j), afterCorrection);
                }
            }
        }
  
        Info(APP_NAME, "Finished successfully!");
    
        return StatusCode::SUCCESS;  
    }
}
