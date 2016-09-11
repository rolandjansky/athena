/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CPANALYSISEXAMPLES_ERRORCHECK_H
#define CPANALYSISEXAMPLES_ERRORCHECK_H

#define CHECK( ARG )                                     \
   do {                                                  \
      const bool result = ARG;                           \
      if( ! result ) {                                   \
         ::Error( APP_NAME, "Failed to execute: \"%s\"", \
                  #ARG );                                \
         return 1;                                       \
      }                                                  \
   } while( false )

#endif // CPANALYSISEXAMPLES_ERRORCHECK_H

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"

// Tools
#include "IsolationSelection/IsolationCloseByCorrectionTool.h"
#include "IsolationSelection/IsolationSelectionTool.h"

/// For statistics
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

using namespace std;

const float GeV = 1000.;
const float iGeV = 0.001;
const float PI = 3.1416;

int main(int argc, char** argv ){

    gErrorIgnoreLevel = 0;
    const char* APP_NAME = "test_IsolationCloseByCorrectionTool";
    CHECK(xAOD::Init());

    bool produceOutput = true;
    
    TString outputTree = "output_testIsolationCloseByCorrectionTool_Zmumu_20160729.root";
    
    TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_Zmumu/147807/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v114._000187.1";
//     TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia_P2011C_ttbar/117050/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111._000001.4";
//     TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_Zee/147806/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_v114._000001.1";
//     TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_ggH125_gamgam/160009/valid1.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e2658_s1967_s1964_r5787_v114._000001.1";

    if(argc > 1) {
        fileName = argv[1];
    }

    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::auto_ptr< TFile > ifile(TFile::Open(fileName, "READ"));
    ifile.get();

    TFile* ofile;
    if(produceOutput) {
        ofile = TFile::Open(outputTree, "RECREATE");
    }

    TTree *tree = new TTree("otree", "otree");
    Int_t eventNumber = -999;
    
    std::vector<Int_t> muon_acceptLoose;
    std::vector<Int_t> muon_acceptCorrectedLoose;
    std::vector<Float_t> muon_ptcone20Corr;
    std::vector<Float_t> muon_ptcone30Corr;
    std::vector<Float_t> muon_ptcone40Corr;
    std::vector<Float_t> muon_ptvarcone20Corr;
    std::vector<Float_t> muon_ptvarcone30Corr;
    std::vector<Float_t> muon_ptvarcone40Corr;
    std::vector<Float_t> muon_topoetcone20Corr;
    std::vector<Float_t> muon_topoetcone30Corr;
    std::vector<Float_t> muon_topoetcone40Corr;
    
    std::vector<Int_t> electron_acceptLoose;
    std::vector<Int_t> electron_acceptCorrectedLoose;
    std::vector<Float_t> electron_ptcone20Corr;
    std::vector<Float_t> electron_ptcone30Corr;
    std::vector<Float_t> electron_ptcone40Corr;
    std::vector<Float_t> electron_ptvarcone20Corr;
    std::vector<Float_t> electron_ptvarcone30Corr;
    std::vector<Float_t> electron_ptvarcone40Corr;
    std::vector<Float_t> electron_topoetcone20Corr;
    std::vector<Float_t> electron_topoetcone30Corr;
    std::vector<Float_t> electron_topoetcone40Corr;
    
    std::vector<Int_t> photon_acceptFixedCutTightCaloOnly;
    std::vector<Int_t> photon_acceptCorrectedFixedCutTightCaloOnly;
    std::vector<Float_t> photon_ptcone20Corr;
    std::vector<Float_t> photon_ptcone30Corr;
    std::vector<Float_t> photon_ptcone40Corr;
    std::vector<Float_t> photon_ptvarcone20Corr;
    std::vector<Float_t> photon_ptvarcone30Corr;
    std::vector<Float_t> photon_ptvarcone40Corr;
    std::vector<Float_t> photon_topoetcone20Corr;
    std::vector<Float_t> photon_topoetcone30Corr;
    std::vector<Float_t> photon_topoetcone40Corr;

    muon_acceptLoose.clear();
    muon_acceptCorrectedLoose.clear();
    muon_ptcone20Corr.clear();
    muon_ptcone30Corr.clear();
    muon_ptcone40Corr.clear();
    muon_ptvarcone20Corr.clear();
    muon_ptvarcone30Corr.clear();
    muon_ptvarcone40Corr.clear();
    muon_topoetcone20Corr.clear();
    muon_topoetcone30Corr.clear();
    muon_topoetcone40Corr.clear();
    
    electron_acceptLoose.clear();
    electron_acceptCorrectedLoose.clear();
    electron_ptcone20Corr.clear();
    electron_ptcone30Corr.clear();
    electron_ptcone40Corr.clear();
    electron_ptvarcone20Corr.clear();
    electron_ptvarcone30Corr.clear();
    electron_ptvarcone40Corr.clear();
    electron_topoetcone20Corr.clear();
    electron_topoetcone30Corr.clear();
    electron_topoetcone40Corr.clear();
    
    photon_acceptFixedCutTightCaloOnly.clear();
    photon_acceptCorrectedFixedCutTightCaloOnly.clear();
    photon_ptcone20Corr.clear();
    photon_ptcone30Corr.clear();
    photon_ptcone40Corr.clear();
    photon_ptvarcone20Corr.clear();
    photon_ptvarcone30Corr.clear();
    photon_ptvarcone40Corr.clear();
    photon_topoetcone20Corr.clear();
    photon_topoetcone30Corr.clear();
    photon_topoetcone40Corr.clear();

    tree->Branch("eventNumber", &eventNumber);
    
    tree->Branch("muon_acceptLoose", &muon_acceptLoose);
    tree->Branch("muon_acceptCorrectedLoose", &muon_acceptCorrectedLoose);
    tree->Branch("muon_ptcone20Corr", &muon_ptcone20Corr);
    tree->Branch("muon_ptcone30Corr", &muon_ptcone30Corr);
    tree->Branch("muon_ptcone40Corr", &muon_ptcone40Corr);
    tree->Branch("muon_ptvarcone20Corr", &muon_ptvarcone20Corr);
    tree->Branch("muon_ptvarcone30Corr", &muon_ptvarcone30Corr);
    tree->Branch("muon_ptvarcone40Corr", &muon_ptvarcone40Corr);
    tree->Branch("muon_topoetcone20Corr", &muon_topoetcone20Corr);
    tree->Branch("muon_topoetcone30Corr", &muon_topoetcone30Corr);
    tree->Branch("muon_topoetcone40Corr", &muon_topoetcone40Corr);
    
    tree->Branch("electron_acceptLoose", &electron_acceptLoose);
    tree->Branch("electron_acceptCorrectedLoose", &electron_acceptCorrectedLoose);
    tree->Branch("electron_ptcone20Corr", &electron_ptcone20Corr);
    tree->Branch("electron_ptcone30Corr", &electron_ptcone30Corr);
    tree->Branch("electron_ptcone40Corr", &electron_ptcone40Corr);
    tree->Branch("electron_ptvarcone20Corr", &electron_ptvarcone20Corr);
    tree->Branch("electron_ptvarcone30Corr", &electron_ptvarcone30Corr);
    tree->Branch("electron_ptvarcone40Corr", &electron_ptvarcone40Corr);
    tree->Branch("electron_topoetcone20Corr", &electron_topoetcone20Corr);
    tree->Branch("electron_topoetcone30Corr", &electron_topoetcone30Corr);
    tree->Branch("electron_topoetcone40Corr", &electron_topoetcone40Corr);
    
    tree->Branch("photon_acceptLoose", &photon_acceptFixedCutTightCaloOnly);
    tree->Branch("photon_acceptCorrectedLoose", &photon_acceptCorrectedFixedCutTightCaloOnly);
    tree->Branch("photon_ptcone20Corr", &photon_ptcone20Corr);
    tree->Branch("photon_ptcone30Corr", &photon_ptcone30Corr);
    tree->Branch("photon_ptcone40Corr", &photon_ptcone40Corr);
    tree->Branch("photon_ptvarcone20Corr", &photon_ptvarcone20Corr);
    tree->Branch("photon_ptvarcone30Corr", &photon_ptvarcone30Corr);
    tree->Branch("photon_ptvarcone40Corr", &photon_ptvarcone40Corr);
    tree->Branch("photon_topoetcone20Corr", &photon_topoetcone20Corr);
    tree->Branch("photon_topoetcone30Corr", &photon_topoetcone30Corr);
    tree->Branch("photon_topoetcone40Corr", &photon_topoetcone40Corr);

    // Create a TEvent object:
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);
    CHECK(event.readFrom(ifile.get()));

    // Creating tools.
    CP::IsolationCloseByCorrectionTool* m_isoCloseByTool_Muon = 
        new CP::IsolationCloseByCorrectionTool("isoCloseByTool_Muon"); 
//     m_isoCloseByTool_Muon->msg().setLevel(MSG::DEBUG);
    m_isoCloseByTool_Muon->msg().setLevel(MSG::INFO);
    
    CP::IsolationSelectionTool* m_isoSelTool_Muon = new CP::IsolationSelectionTool("isoSelTool_Muon");
    CHECK(m_isoSelTool_Muon->setProperty("MuonWP", "LooseTrackOnly"));
    CHECK(m_isoSelTool_Muon->initialize());
    
    ToolHandle<CP::IIsolationSelectionTool> m_iIsoSelTool_Muon = m_isoSelTool_Muon;
    CHECK(m_isoCloseByTool_Muon->setProperty("IsolationSelectionTool", m_iIsoSelTool_Muon)); 
    CHECK(m_isoCloseByTool_Muon->initialize());

    CP::IsolationCloseByCorrectionTool* m_isoCloseByTool_Electron = 
        new CP::IsolationCloseByCorrectionTool("isoCloseByTool_Electron"); 
//     m_isoCloseByTool_Electron->msg().setLevel(MSG::DEBUG);
    m_isoCloseByTool_Electron->msg().setLevel(MSG::INFO);
    
    CP::IsolationSelectionTool* m_isoSelTool_Electron = new CP::IsolationSelectionTool("isoSelTool_Electron");
    CHECK(m_isoSelTool_Electron->setProperty("ElectronWP", "LooseTrackOnly"));
    CHECK(m_isoSelTool_Electron->initialize());
    
    ToolHandle<CP::IIsolationSelectionTool> m_iIsoSelTool_Electron = m_isoSelTool_Electron;
    CHECK(m_isoCloseByTool_Electron->setProperty("IsolationSelectionTool", m_iIsoSelTool_Electron)); 
    CHECK(m_isoCloseByTool_Electron->initialize());
    
    CP::IsolationCloseByCorrectionTool* m_isoCloseByTool_Photon = 
        new CP::IsolationCloseByCorrectionTool("isoCloseByTool_Photon"); 
//     m_isoCloseByTool_Photon->msg().setLevel(MSG::DEBUG);
    m_isoCloseByTool_Photon->msg().setLevel(MSG::INFO);
    
    CP::IsolationSelectionTool* m_isoSelTool_Photon = new CP::IsolationSelectionTool("isoSelTool_Photon");
    CHECK(m_isoSelTool_Photon->setProperty("PhotonWP", "FixedCutTightCaloOnly"));
    CHECK(m_isoSelTool_Photon->initialize());

    ToolHandle<CP::IIsolationSelectionTool> m_iIsoSelTool_Photon = m_isoSelTool_Photon;
    CHECK(m_isoCloseByTool_Photon->setProperty("IsolationSelectionTool", m_iIsoSelTool_Photon)); 
    CHECK(m_isoCloseByTool_Photon->initialize());


  
    vector<xAOD::Iso::IsolationType> types;
    vector<const char*> typeNames;
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

//   Long64_t maxEVT = 10;
    Long64_t maxEVT = -1;
    Long64_t entries = event.getEntries();
    if((entries < maxEVT) || (maxEVT <= 0)) {
        maxEVT = entries;
    }
    Info( APP_NAME, "%lld events found, %lld events will be processed.", entries, maxEVT);
    
    const int INTERVAL = maxEVT > 20000? 10000: maxEVT/10;
    
    for(Long64_t entry = 0; entry < maxEVT; ++entry) {
    
        event.getEntry(entry);
        const xAOD::EventInfo* ei = 0;
        CHECK(event.retrieve(ei, "EventInfo"));

        if(entry % INTERVAL == 0){
            Info(APP_NAME, "%lld events processed, on event %llu of run %u", 
                entry, ei->eventNumber(), ei->runNumber());
        }
        eventNumber = ei->eventNumber();

        muon_acceptLoose.clear();
        muon_acceptCorrectedLoose.clear();
        muon_ptcone20Corr.clear();
        muon_ptcone30Corr.clear();
        muon_ptcone40Corr.clear();
        muon_ptvarcone20Corr.clear();
        muon_ptvarcone30Corr.clear();
        muon_ptvarcone40Corr.clear();
        muon_topoetcone20Corr.clear();
        muon_topoetcone30Corr.clear();
        muon_topoetcone40Corr.clear();
        
        electron_acceptLoose.clear();
        electron_acceptCorrectedLoose.clear();
        electron_ptcone20Corr.clear();
        electron_ptcone30Corr.clear();
        electron_ptcone40Corr.clear();
        electron_ptvarcone20Corr.clear();
        electron_ptvarcone30Corr.clear();
        electron_ptvarcone40Corr.clear();
        electron_topoetcone20Corr.clear();
        electron_topoetcone30Corr.clear();
        electron_topoetcone40Corr.clear();
        
        photon_acceptFixedCutTightCaloOnly.clear();
        photon_acceptCorrectedFixedCutTightCaloOnly.clear();
        photon_ptcone20Corr.clear();
        photon_ptcone30Corr.clear();
        photon_ptcone40Corr.clear();
        photon_ptvarcone20Corr.clear();
        photon_ptvarcone30Corr.clear();
        photon_ptvarcone40Corr.clear();
        photon_topoetcone20Corr.clear();
        photon_topoetcone30Corr.clear();
        photon_topoetcone40Corr.clear();

        // get muon container of interest
        const xAOD::MuonContainer* muons = 0;
        CHECK(event.retrieve(muons, "Muons"));

        // Stores the muons in a vector.
        vector<const xAOD::IParticle*> muonsVec;
        for(auto muon: *muons) {
            muonsVec.push_back((const xAOD::IParticle*) muon);
        }

        // get electron container of interest
        const xAOD::ElectronContainer* electrons = 0;
        CHECK(event.retrieve(electrons, "Electrons"));

        // Stores the electrons in a vector.
        vector<const xAOD::IParticle*> electronsVec;
        for(auto electron: *electrons) {
            electronsVec.push_back((const xAOD::IParticle*) electron);
        }

        // get photon container of interest
        const xAOD::PhotonContainer* photons = 0;
        CHECK(event.retrieve(photons, "Photons"));

        // Stores the electrons in a vector.
        vector<const xAOD::IParticle*> photonsVec;
        for(auto photon: *photons) {
            photonsVec.push_back((const xAOD::IParticle*) photon);
        }

        for(auto muon: *muons){
        
            Info(APP_NAME, "---------NEW MUON -------");
        
            if (m_isoCloseByTool_Muon->acceptCorrected(*muon, muonsVec)) {
                Info(APP_NAME, "Muon passes Loose working point after correction.");
                muon_acceptCorrectedLoose.push_back(1);
            } else {
                Info(APP_NAME, "Muon does not pass Loose working point after correction.");
                muon_acceptCorrectedLoose.push_back(0);
            }

            if (m_isoSelTool_Muon->accept(*muon)) {
                Info(APP_NAME, "Muon passes Loose working point before correction.");
                muon_acceptLoose.push_back(1);
            } else {
                Info(APP_NAME, "Muon does not pass Loose working point before correction.");
                muon_acceptLoose.push_back(0);
            }

            // Calculates the corrections.
            vector<Float_t> corrections;

            m_isoCloseByTool_Muon->getCloseByCorrection(corrections, *muon, types, muonsVec);
      
            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < corrections.size()) {
                    float value = -999999;
                    muon->isolation(value, types.at(j));
                    float afterCorrection = value - corrections.at(j);
                    Info(APP_NAME, "Muon Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Muon Value, removal, corrected value: %f, %f, %f", value, corrections.at(j), afterCorrection);
                    switch(j) {
                        case 0:
                            muon_ptcone20Corr.push_back(corrections.at(j));
                            break;
                        case 1:
                            muon_ptcone30Corr.push_back(corrections.at(j));
                            break;
                        case 2:
                            muon_ptcone40Corr.push_back(corrections.at(j));
                            break;
                        case 3:
                            muon_ptvarcone20Corr.push_back(corrections.at(j));
                            break;
                        case 4:
                            muon_ptvarcone30Corr.push_back(corrections.at(j));
                            break;
                        case 5:
                            muon_ptvarcone40Corr.push_back(corrections.at(j));
                            break;
                        case 6:
                            muon_topoetcone20Corr.push_back(corrections.at(j));
                            break;
                        case 7:
                            muon_topoetcone30Corr.push_back(corrections.at(j));
                            break;
                        case 8:
                            muon_topoetcone40Corr.push_back(corrections.at(j));
                            break;
                        default:
                            Info(APP_NAME, "The muon isolation was not recognised.");
                            break;
                    }
                }
            }
        }
    
        for(auto electron: *electrons){
            Info(APP_NAME, "---------NEW ELECTRON -------");
            
            if (m_isoCloseByTool_Electron->acceptCorrected(*electron, electronsVec)) {
                Info(APP_NAME, "Electron passes Loose working point after correction.");
                electron_acceptCorrectedLoose.push_back(1);
            } else {
                Info(APP_NAME, "Electron does not pass Loose working point after correction.");
                electron_acceptCorrectedLoose.push_back(0);
            }

            if (m_isoSelTool_Electron->accept(*electron)) {
                Info(APP_NAME, "Electron passes Loose working point before correction.");
                electron_acceptLoose.push_back(1);
            } else {
                Info(APP_NAME, "Electron does not pass Loose working point before correction.");
                electron_acceptLoose.push_back(0);
            }

            // Calculates the corrections.
            vector<Float_t> corrections;

            m_isoCloseByTool_Electron->getCloseByCorrection(corrections, *electron, types, electronsVec);
      
            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < corrections.size()) {
                    float value = -999999;
                    electron->isolation(value, types.at(j));
                    float afterCorrection = value - corrections.at(j);
                    Info(APP_NAME, "Electron Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Electron Value, removal, corrected value: %f, %f, %f", value, corrections.at(j), afterCorrection);
                    switch(j) {
                        case 0:
                            electron_ptcone20Corr.push_back(corrections.at(j));
                            break;
                        case 1:
                            electron_ptcone30Corr.push_back(corrections.at(j));
                            break;
                        case 2:
                            electron_ptcone40Corr.push_back(corrections.at(j));
                            break;
                        case 3:
                            electron_ptvarcone20Corr.push_back(corrections.at(j));
                            break;
                        case 4:
                            electron_ptvarcone30Corr.push_back(corrections.at(j));
                            break;
                        case 5:
                            electron_ptvarcone40Corr.push_back(corrections.at(j));
                            break;
                        case 6:
                            electron_topoetcone20Corr.push_back(corrections.at(j));
                            break;
                        case 7:
                            electron_topoetcone30Corr.push_back(corrections.at(j));
                            break;
                        case 8:
                            electron_topoetcone40Corr.push_back(corrections.at(j));
                            break;
                        default:
                            Info(APP_NAME, "The electron isolation was not recognised.");
                            break;
                    }
                }
            }
        }
    
        for(auto photon: *photons){
            Info(APP_NAME, "---------NEW photon -------");
            
            if (m_isoCloseByTool_Photon->acceptCorrected(*photon, photonsVec)) {
                Info(APP_NAME, "Photon passes FixedCutTightCaloOnly working point after correction.");
                photon_acceptCorrectedFixedCutTightCaloOnly.push_back(1);
            } else {
                Info(APP_NAME, "Photon does not pass FixedCutTightCaloOnly working point after correction.");
                photon_acceptCorrectedFixedCutTightCaloOnly.push_back(0);
            }

            if (m_isoSelTool_Photon->accept(*photon)) {
                Info(APP_NAME, "Photon passes FixedCutTightCaloOnly working point before correction.");
                photon_acceptFixedCutTightCaloOnly.push_back(1);
            } else {
                Info(APP_NAME, "Photon does not pass FixedCutTightCaloOnly working point before correction.");
                photon_acceptFixedCutTightCaloOnly.push_back(0);
            }

            // Calculates the corrections.
            vector<Float_t> corrections;

            m_isoCloseByTool_Photon->getCloseByCorrection(corrections, *photon, types, photonsVec);
      
            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < corrections.size()) {
                    float value = -999999;
                    photon->isolation(value, types.at(j));
                    float afterCorrection = value - corrections.at(j);
                    Info(APP_NAME, "Photon Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Photon Value, removal, corrected value: %f, %f, %f", value, corrections.at(j), afterCorrection);
                    switch(j) {
                        case 0:
                            photon_ptcone20Corr.push_back(corrections.at(j));
                            break;
                        case 1:
                            photon_ptcone30Corr.push_back(corrections.at(j));
                            break;
                        case 2:
                            photon_ptcone40Corr.push_back(corrections.at(j));
                            break;
                        case 3:
                            photon_ptvarcone20Corr.push_back(corrections.at(j));
                            break;
                        case 4:
                            photon_ptvarcone30Corr.push_back(corrections.at(j));
                            break;
                        case 5:
                            photon_ptvarcone40Corr.push_back(corrections.at(j));
                            break;
                        case 6:
                            photon_topoetcone20Corr.push_back(corrections.at(j));
                            break;
                        case 7:
                            photon_topoetcone30Corr.push_back(corrections.at(j));
                            break;
                        case 8:
                            photon_topoetcone40Corr.push_back(corrections.at(j));
                            break;
                        default:
                            Info(APP_NAME, "The photon isolation was not recognised.");
                            break;
                    }
                }
            }
        }
    
        tree->Fill();
    
    }

    if(produceOutput) {
        ofile->cd();
        tree->Write();
        ofile->Close();
    }
  
    Info(APP_NAME, "Finished successfully!");

    xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    delete m_isoCloseByTool_Muon;
    delete m_isoCloseByTool_Electron;
    delete m_isoCloseByTool_Photon;

    return 0;
    
}