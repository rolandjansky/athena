/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "TestIsolationToolAthenaAlg.h"

// EDM include(s):
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"

// other include(s):
#include <cmath>

const char* APP_NAME = "ISOTOOLTEST";

namespace CP 
{

  TestIsolationToolAthenaAlg::TestIsolationToolAthenaAlg( const std::string& name, ISvcLocator* svcLoc ) :
    AthAlgorithm( name, svcLoc )
  {  
    declareProperty( "IsoTool", m_isoHelper );
  }

  StatusCode TestIsolationToolAthenaAlg::initialize()
  {
    ATH_CHECK( m_isoHelper.retrieve() );
//     
//     m_isoHelper.setLevel( MSG::DEBUG );
//     m_isoHelper.setSelectorProperty("MuonWP", "Loose");
//     m_isoHelper.initialize();
    
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }

  StatusCode TestIsolationToolAthenaAlg::execute()
  {

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
     ATH_CHECK( evtStore()->retrieve(muons, "Muons") );
     
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
        
#ifdef XAOD_ANALYSIS
        if (m_isoHelper->acceptCorrected(*muon, muonsVec, types)) {
            Info(APP_NAME, "Muon passes Loose working point after correction.");
        } else {
            Info(APP_NAME, "Muon does not pass Loose working point after correction.");
        }
        
        if (m_isoHelper->acceptCorrected(*muon)) {
            Info(APP_NAME, "Muon passes Loose working point before correction.");
        } else {
            Info(APP_NAME, "Muon does not pass Loose working point before correction.");
        }
#endif // XAOD_ANALYSIS
      
        // Performs overlap removal.
        std::vector<Float_t> removals;
      
        m_isoHelper->removeOverlap(removals, *muon, types, muonsVec);
      
        for (unsigned int j = 0; j < types.size(); j++) {
          if(j < removals.size()) {
              float value = -999999;
              muon->isolation(value, types.at(j));
              float afterRemoval = value - removals.at(j);
              Info(APP_NAME, "Muon Isolation variable: %s", typeNames.at(j));
              Info(APP_NAME, "Muon Value, removal, corrected value: %f, %f, %f", value, removals.at(j), afterRemoval);
          }
        }
    }
    
    for(auto electron: *electrons){
       Info(APP_NAME, "---------NEW ELECTRON -------");
      
        // Performs overlap removal.
        std::vector<Float_t> removals;
      
        m_isoHelper->removeOverlap(removals, *electron, types, muonsVec);
      
        for (unsigned int j = 0; j < types.size(); j++) {
          if(j < removals.size()) {
              float value = -999999;
              electron->isolation(value, types.at(j));
              float afterRemoval = value - removals.at(j);
              Info(APP_NAME, "Electron Isolation variable: %s", typeNames.at(j));
              Info(APP_NAME, "Electron Value, removal, corrected value: %f, %f, %f", value, removals.at(j), afterRemoval);
          }
        }
    }
    
    for(auto photon: *photons){
       Info(APP_NAME, "---------NEW photon -------");
      
        // Performs overlap removal.
        std::vector<Float_t> removals;
      
        m_isoHelper->removeOverlap(removals, *photon, types, muonsVec);
      
        for (unsigned int j = 0; j < types.size(); j++) {
          if(j < removals.size()) {
              float value = -999999;
              photon->isolation(value, types.at(j));
              float afterRemoval = value - removals.at(j);
              Info(APP_NAME, "Photon Isolation variable: %s", typeNames.at(j));
              Info(APP_NAME, "Photon Value, removal, corrected value: %f, %f, %f", value, removals.at(j), afterRemoval);
          }
        }
    }
  
    Info(APP_NAME, "Finished successfully!");
    
    // Return gracefully:
    return StatusCode::SUCCESS;  
  }
}
