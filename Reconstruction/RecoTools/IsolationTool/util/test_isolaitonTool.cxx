/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Mindlessly copied from CPAnalysisExamples
#ifndef CPANALYSISEXAMPLES_ERRORCHECK_H
#define CPANALYSISEXAMPLES_ERRORCHECK_H

#define CHECK( ARG )                                        \
    do {                                                    \
        const bool result = ARG;                            \
        if( ! result ) {                                    \
            ::Error( APP_NAME, "Failed to execute: \"%s\"", \
                #ARG );                                     \
            return 1;                                       \
        }                                                   \
    } while( false )

#endif // CPANALYSISEXAMPLES_ERRORCHECK_H

#include <iostream>
#include <memory>
#include <cstdlib>

// ROOT include(s):
#include <TFile.h>
#include <TTree.h>
#include <TError.h>
#include <TString.h>
#include <TH1F.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/ReturnCheck.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"

/// for testing
#include "IsolationTool/CaloIsolationTool.h"
#include "IsolationTool/TrackIsolationTool.h"
#include "IsolationTool/IsolationHelper.h"
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

/// For statistics
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

using namespace std;

const float GeV = 1000.;
const float iGeV = 0.001;
const float PI = 3.1416;

int main(int argc, char** argv ){

    gErrorIgnoreLevel=0;
    const char* APP_NAME = "ISOTEST";
    CHECK( xAOD::Init());

    bool produceOutput = true;
    TString outputTree = "output_test_Zmumu.root";
//     TString fileName = "/afs/cern.ch/user/k/kaplan/public/DAOD_SUSY5.output.pool.root";
//     TString fileName = "root://eosatlas.cern.ch//eos/atlas/user/m/mdobre/forRTTmc15/MC15_207.AOD.pool.root";
    TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_Zmumu/147807/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s1964_r5787_v114._000187.1";
//     TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia_P2011C_ttbar/117050/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111/valid1.117050.PowhegPythia_P2011C_ttbar.recon.AOD.e2658_s1967_s1964_r5787_v111._000001.4";
//     TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_Zee/147806/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.147806.PowhegPythia8_AU2CT10_Zee.recon.AOD.e2658_s1967_s1964_r5787_v114._000001.1";
//     TString fileName = "root://eosatlas//eos/atlas/atlastier0/tzero/prod/valid1/PowhegPythia8_AU2CT10_ggH125_gamgam/160009/valid1.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e2658_s1967_s1964_r5787_v114/valid1.160009.PowhegPythia8_AU2CT10_ggH125_gamgam.recon.AOD.e2658_s1967_s1964_r5787_v114._000001.1";
    if(argc>1) {
        fileName = argv[1];
    }

    const TString tag = "t2_";
    Info( APP_NAME, "Opening file: %s", fileName.Data() );
    std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
    ifile.get();

    TFile* ofile;
    if(produceOutput) {
        ofile = TFile::Open(outputTree, "RECREATE");
    }

    TTree *tree = new TTree("otree", "otree");
    Int_t eventNumber = -999;
    std::vector<Int_t> muon_acceptLoose;
    std::vector<Int_t> muon_acceptCorrectedLoose;
    std::vector<Float_t> muon_ptcone20Rem;
    std::vector<Float_t> muon_ptcone30Rem;
    std::vector<Float_t> muon_ptcone40Rem;
    std::vector<Float_t> muon_ptvarcone20Rem;
    std::vector<Float_t> muon_ptvarcone30Rem;
    std::vector<Float_t> muon_ptvarcone40Rem;
    std::vector<Float_t> muon_topoetcone20Rem;
    std::vector<Float_t> muon_topoetcone30Rem;
    std::vector<Float_t> muon_topoetcone40Rem;
    std::vector<Int_t> electron_acceptLoose;
    std::vector<Int_t> electron_acceptCorrectedLoose;
    std::vector<Float_t> electron_ptcone20Rem;
    std::vector<Float_t> electron_ptcone30Rem;
    std::vector<Float_t> electron_ptcone40Rem;
    std::vector<Float_t> electron_ptvarcone20Rem;
    std::vector<Float_t> electron_ptvarcone30Rem;
    std::vector<Float_t> electron_ptvarcone40Rem;
    std::vector<Float_t> electron_topoetcone20Rem;
    std::vector<Float_t> electron_topoetcone30Rem;
    std::vector<Float_t> electron_topoetcone40Rem;
//   std::vector<Int_t> photon_acceptLoose;
//   std::vector<Int_t> photon_acceptCorrectedLoose;
    std::vector<Float_t> photon_ptcone20Rem;
    std::vector<Float_t> photon_ptcone30Rem;
    std::vector<Float_t> photon_ptcone40Rem;
    std::vector<Float_t> photon_ptvarcone20Rem;
    std::vector<Float_t> photon_ptvarcone30Rem;
    std::vector<Float_t> photon_ptvarcone40Rem;
    std::vector<Float_t> photon_topoetcone20Rem;
    std::vector<Float_t> photon_topoetcone30Rem;
    std::vector<Float_t> photon_topoetcone40Rem;

    muon_acceptLoose.clear();
    muon_acceptCorrectedLoose.clear();
    muon_ptcone20Rem.clear();
    muon_ptcone30Rem.clear();
    muon_ptcone40Rem.clear();
    muon_ptvarcone20Rem.clear();
    muon_ptvarcone30Rem.clear();
    muon_ptvarcone40Rem.clear();
    muon_topoetcone20Rem.clear();
    muon_topoetcone30Rem.clear();
    muon_topoetcone40Rem.clear();
    electron_acceptLoose.clear();
    electron_acceptCorrectedLoose.clear();
    electron_ptcone20Rem.clear();
    electron_ptcone30Rem.clear();
    electron_ptcone40Rem.clear();
    electron_ptvarcone20Rem.clear();
    electron_ptvarcone30Rem.clear();
    electron_ptvarcone40Rem.clear();
    electron_topoetcone20Rem.clear();
    electron_topoetcone30Rem.clear();
    electron_topoetcone40Rem.clear();
//   photon_acceptLoose.clear();
//   photon_acceptCorrectedLoose.clear();
    photon_ptcone20Rem.clear();
    photon_ptcone30Rem.clear();
    photon_ptcone40Rem.clear();
    photon_ptvarcone20Rem.clear();
    photon_ptvarcone30Rem.clear();
    photon_ptvarcone40Rem.clear();
    photon_topoetcone20Rem.clear();
    photon_topoetcone30Rem.clear();
    photon_topoetcone40Rem.clear();

    tree->Branch("eventNumber", &eventNumber);
    tree->Branch("muon_acceptLoose", &muon_acceptLoose);
    tree->Branch("muon_acceptCorrectedLoose", &muon_acceptCorrectedLoose);
    tree->Branch("muon_ptcone20Rem", &muon_ptcone20Rem);
    tree->Branch("muon_ptcone30Rem", &muon_ptcone30Rem);
    tree->Branch("muon_ptcone40Rem", &muon_ptcone40Rem);
    tree->Branch("muon_ptvarcone20Rem", &muon_ptvarcone20Rem);
    tree->Branch("muon_ptvarcone30Rem", &muon_ptvarcone30Rem);
    tree->Branch("muon_ptvarcone40Rem", &muon_ptvarcone40Rem);
    tree->Branch("muon_topoetcone20Rem", &muon_topoetcone20Rem);
    tree->Branch("muon_topoetcone30Rem", &muon_topoetcone30Rem);
    tree->Branch("muon_topoetcone40Rem", &muon_topoetcone40Rem);
    tree->Branch("electron_acceptLoose", &electron_acceptLoose);
    tree->Branch("electron_acceptCorrectedLoose", &electron_acceptCorrectedLoose);
    tree->Branch("electron_ptcone20Rem", &electron_ptcone20Rem);
    tree->Branch("electron_ptcone30Rem", &electron_ptcone30Rem);
    tree->Branch("electron_ptcone40Rem", &electron_ptcone40Rem);
    tree->Branch("electron_ptvarcone20Rem", &electron_ptvarcone20Rem);
    tree->Branch("electron_ptvarcone30Rem", &electron_ptvarcone30Rem);
    tree->Branch("electron_ptvarcone40Rem", &electron_ptvarcone40Rem);
    tree->Branch("electron_topoetcone20Rem", &electron_topoetcone20Rem);
    tree->Branch("electron_topoetcone30Rem", &electron_topoetcone30Rem);
    tree->Branch("electron_topoetcone40Rem", &electron_topoetcone40Rem);
//   tree->Branch("photon_acceptLoose", &photon_acceptLoose);
//   tree->Branch("photon_acceptCorrectedLoose", &photon_acceptCorrectedLoose);
    tree->Branch("photon_ptcone20Rem", &photon_ptcone20Rem);
    tree->Branch("photon_ptcone30Rem", &photon_ptcone30Rem);
    tree->Branch("photon_ptcone40Rem", &photon_ptcone40Rem);
    tree->Branch("photon_ptvarcone20Rem", &photon_ptvarcone20Rem);
    tree->Branch("photon_ptvarcone30Rem", &photon_ptvarcone30Rem);
    tree->Branch("photon_ptvarcone40Rem", &photon_ptvarcone40Rem);
    tree->Branch("photon_topoetcone20Rem", &photon_topoetcone20Rem);
    tree->Branch("photon_topoetcone30Rem", &photon_topoetcone30Rem);
    tree->Branch("photon_topoetcone40Rem", &photon_topoetcone40Rem);

    // Create a TEvent object:
    xAOD::TEvent event( xAOD::TEvent::kClassAccess );
    CHECK( event.readFrom( ifile.get() ));

    /// creating tool
    CP::IsolationHelper* m_isoHelper_Muon = new CP::IsolationHelper("isoTester_Muon"); 
    m_isoHelper_Muon->msg().setLevel( MSG::DEBUG );
    if (!m_isoHelper_Muon->setSelectorProperty("MuonWP", "Loose")) {
        Info( APP_NAME, "Could not set muon isolation selector "
        "loose working point.");
    }
    m_isoHelper_Muon->msg().setLevel( MSG::INFO );
    if(!m_isoHelper_Muon->initialize().isSuccess()){
    Info( APP_NAME, "m_isoHelper_Muon.initialize() failed");
    }

    CP::IsolationHelper* m_isoHelper_Electron = new CP::IsolationHelper("isoTester_Electron"); 
    m_isoHelper_Electron->msg().setLevel( MSG::DEBUG );
    if (!m_isoHelper_Electron->setSelectorProperty("ElectronWP", "Loose")) {
        Info( APP_NAME, "Could not set electron isolation selector "
        "loose working point.");
    }
    m_isoHelper_Electron->msg().setLevel( MSG::INFO );
    if(!m_isoHelper_Electron->initialize().isSuccess()){
    Info( APP_NAME, "m_isoHelper_Electron.initialize() failed");
    }

    CP::IsolationHelper* m_isoHelper_Photon = new CP::IsolationHelper("isoTester_Photon"); 
    m_isoHelper_Photon->msg().setLevel( MSG::DEBUG );
//   if (!m_isoHelper_Photon->setSelectorProperty("PhotonWP", "Loose")) {
//         Info( APP_NAME, "Could not set Photon isolation selector "
//         "loose working point.");
//   }
    m_isoHelper_Photon->msg().setLevel( MSG::INFO );
    if(!m_isoHelper_Photon->initialize().isSuccess()){
    Info( APP_NAME, "m_isoHelper_Photon.initialize() failed");
    }
  
//   xAOD::CaloIsolationTool m_caloIsoTool("CaloIsolationTool");
// //   m_caloIsoTool.msg().setLevel( MSG::DEBUG );
// //   m_caloIsoTool.msg().setLevel( MSG::INFO );
//   InDet::InDetTrackSelectionTool selTool( "TrackSelection" );
//   selTool.setProperty("maxZ0SinTheta", 3.);
//   selTool.setProperty("minPt", 1000.);
//   selTool.setProperty("CutLevel", "Loose");
// 
// 
//   xAOD::TrackIsolationTool m_trkIsoTool("TrackIsolationTool");
// //   m_trkIsoTool.msg().setLevel( MSG::INFO );
// //   m_trkIsoTool.msg().setLevel( MSG::DEBUG );
//   m_trkIsoTool.setProperty( "TrackSelectionTool", ToolHandle< InDet::IInDetTrackSelectionTool >(selTool.name()) );
// 
//   if(!m_caloIsoTool.initialize().isSuccess()){
//     Info( APP_NAME, "m_caloIsoTool.initialize() failed");
//   }
// 
//   if(!selTool.initialize().isSuccess()){
//     Info( APP_NAME, "selTool.initialize() failed");
//   }
// 
//   if(!m_trkIsoTool.initialize().isSuccess()){
//     Info( APP_NAME, "m_trkIsoTool.initialize() failed");
//   }

//   vector<xAOD::Iso::IsolationType> isoTypes;
//   isoTypes.push_back(xAOD::Iso::ptcone40);
//   isoTypes.push_back(xAOD::Iso::ptcone30);
//   isoTypes.push_back(xAOD::Iso::ptcone20);
//   xAOD::TrackCorrection corrlist;
//   corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::coreTrackPtr));
// 
//   vector<xAOD::Iso::IsolationType> isoTypesCalo;
//   isoTypesCalo.push_back(xAOD::Iso::topoetcone40);
//   isoTypesCalo.push_back(xAOD::Iso::topoetcone30);
//   isoTypesCalo.push_back(xAOD::Iso::topoetcone20);
//   xAOD::CaloCorrection corrlist2;
//   corrlist2.calobitset.set(static_cast<unsigned int>(xAOD::Iso::coreCone));
//   corrlist2.calobitset.set(static_cast<unsigned int>(xAOD::Iso::pileupCorrection));
// 
//   vector<xAOD::Iso::IsolationType> isoTypesPFlow;
//   isoTypesPFlow.push_back(xAOD::Iso::neflowisol40);
//   isoTypesPFlow.push_back(xAOD::Iso::neflowisol30);
//   isoTypesPFlow.push_back(xAOD::Iso::neflowisol20);
//   xAOD::CaloCorrection corrlist3;
//   corrlist3.calobitset.set(static_cast<unsigned int>(xAOD::Iso::coreCone));
//   corrlist3.calobitset.set(static_cast<unsigned int>(xAOD::Iso::pileupCorrection));
  
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
    for( Long64_t entry = 0; entry < maxEVT; ++entry ) {
        event.getEntry( entry );
        const xAOD::EventInfo* ei = 0;
        CHECK( event.retrieve( ei, "EventInfo" ) );

        // Print some event information for fun:
        if(entry%INTERVAL==0){
            Info( APP_NAME, "%lld events processed, on event %llu of run %u", entry, ei->eventNumber(), ei->runNumber() );
        }
        eventNumber = ei->eventNumber();

        muon_acceptLoose.clear();
        muon_acceptCorrectedLoose.clear();
        muon_ptcone20Rem.clear();
        muon_ptcone30Rem.clear();
        muon_ptcone40Rem.clear();
        muon_ptvarcone20Rem.clear();
        muon_ptvarcone30Rem.clear();
        muon_ptvarcone40Rem.clear();
        muon_topoetcone20Rem.clear();
        muon_topoetcone30Rem.clear();
        muon_topoetcone40Rem.clear();
        electron_acceptLoose.clear();
        electron_acceptCorrectedLoose.clear();
        electron_ptcone20Rem.clear();
        electron_ptcone30Rem.clear();
        electron_ptcone40Rem.clear();
        electron_ptvarcone20Rem.clear();
        electron_ptvarcone30Rem.clear();
        electron_ptvarcone40Rem.clear();
        electron_topoetcone20Rem.clear();
        electron_topoetcone30Rem.clear();
        electron_topoetcone40Rem.clear();
    //   photon_acceptLoose.clear();
    //   photon_acceptCorrectedLoose.clear();
        photon_ptcone20Rem.clear();
        photon_ptcone30Rem.clear();
        photon_ptcone40Rem.clear();
        photon_ptvarcone20Rem.clear();
        photon_ptvarcone30Rem.clear();
        photon_ptvarcone40Rem.clear();
        photon_topoetcone20Rem.clear();
        photon_topoetcone30Rem.clear();
        photon_topoetcone40Rem.clear();

        // get muon container of interest
        const xAOD::MuonContainer* muons = 0;
        CHECK( event.retrieve( muons, "Muons" ) );

        // Stores the muons in a vector.
        vector<const xAOD::IParticle*> muonsVec;
        for(auto muon: *muons) {
            muonsVec.push_back((const xAOD::IParticle*) muon);
        }

        // get electron container of interest
        const xAOD::ElectronContainer* electrons = 0;
        CHECK( event.retrieve( electrons, "Electrons" ) );

        // Stores the electrons in a vector.
        vector<const xAOD::IParticle*> electronsVec;
        for(auto electron: *electrons) {
            electronsVec.push_back((const xAOD::IParticle*) electron);
        }

        // get photon container of interest
        const xAOD::PhotonContainer* photons = 0;
        CHECK( event.retrieve( photons, "Photons" ) );

        // Stores the electrons in a vector.
        vector<const xAOD::IParticle*> photonsVec;
        for(auto photon: *photons) {
            photonsVec.push_back((const xAOD::IParticle*) photon);
        }

        for(auto muon: *muons){
            Info(APP_NAME, "---------NEW MUON -------");
//        xAOD::TrackIsolation result;
//        if(m_trkIsoTool.trackIsolation(result, *muon, isoTypes, corrlist)){
//          for(unsigned int i=0; i<isoTypes.size(); i++){
//            float iso = result.ptcones[i];
//            float isoV = result.ptvarcones_10GeVDivPt[i];
//            float value0 = -999;
//            if(!m_isoHelper_Muon->isolation(value0, *muon, isoTypes[i])) Info(APP_NAME, "Muon default track iso not got.");
//            Info(APP_NAME, "Muon track isolation %d is %f and %f (=%f?)", i, iso, isoV, value0);
//           }
//         }
//        xAOD::CaloIsolation result2;
//        if(m_caloIsoTool.caloTopoClusterIsolation(result2, *muon, isoTypesCalo, corrlist2)){
//          for(unsigned int i=0; i<isoTypesCalo.size(); i++){
//            float value0 = -999;
//            if(!m_isoHelper_Muon->isolation(value0, *muon, isoTypesCalo[i], corrlist2.calobitset)) Info(APP_NAME, "Muon coreCone Calo iso not got.");
//            Info(APP_NAME, "Muon topo isolation %d is %f (=%f?)", i, result2.etcones[i], value0);
//           }
//         }
//        xAOD::CaloIsolation result3;
//        if(m_caloIsoTool.neutralEflowIsolation(result3, *muon, isoTypesPFlow, corrlist3)){
//          for(unsigned int i=0; i<isoTypesPFlow.size(); i++){
//            float value0 = -999;
//            if(!m_isoHelper_Muon->isolation(value0, *muon, isoTypesPFlow[i])) Info(APP_NAME, "Muon default pflow iso not got.");
//            Info(APP_NAME, "Muon neflow isolation %d is %f (=%f?)", i, result3.etcones[i], value0);
//           }
//         }
        
            if (m_isoHelper_Muon->acceptCorrected(*muon, muonsVec, types)) {
                Info(APP_NAME, "Muon passes Loose working point after correction.");
                muon_acceptCorrectedLoose.push_back(1);
            } else {
                Info(APP_NAME, "Muon does not pass Loose working point after correction.");
                muon_acceptCorrectedLoose.push_back(0);
            }

            if (m_isoHelper_Muon->acceptCorrected(*muon)) {
                Info(APP_NAME, "Muon passes Loose working point before correction.");
                muon_acceptLoose.push_back(1);
            } else {
                Info(APP_NAME, "Muon does not pass Loose working point before correction.");
                muon_acceptLoose.push_back(0);
            }

            // Performs overlap removal.
            vector<Float_t> removals;

            m_isoHelper_Muon->removeOverlap(removals, *muon, types, muonsVec);
      
            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < removals.size()) {
                    float value = -999999;
                    muon->isolation(value, types.at(j));
                    float afterRemoval = value - removals.at(j);
                    Info(APP_NAME, "Muon Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Muon Value, removal, corrected value: %f, %f, %f", value, removals.at(j), afterRemoval);
                    switch(j) {
                        case 0:
                            muon_ptcone20Rem.push_back(removals.at(j));
                            break;
                        case 1:
                            muon_ptcone30Rem.push_back(removals.at(j));
                            break;
                        case 2:
                            muon_ptcone40Rem.push_back(removals.at(j));
                            break;
                        case 3:
                            muon_ptvarcone20Rem.push_back(removals.at(j));
                            break;
                        case 4:
                            muon_ptvarcone30Rem.push_back(removals.at(j));
                            break;
                        case 5:
                            muon_ptvarcone40Rem.push_back(removals.at(j));
                            break;
                        case 6:
                            muon_topoetcone20Rem.push_back(removals.at(j));
                            break;
                        case 7:
                            muon_topoetcone30Rem.push_back(removals.at(j));
                            break;
                        case 8:
                            muon_topoetcone40Rem.push_back(removals.at(j));
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
//        xAOD::TrackIsolation result;
//        if(m_trkIsoTool.trackIsolation(result, *electron, isoTypes, corrlist)){
//          for(unsigned int i=0; i<isoTypes.size(); i++){
//            float iso = result.ptcones[i];
//            float isoV = result.ptvarcones_10GeVDivPt[i];
//            float value0 = -999;
//            if(!m_isoHelper_Muon->isolation(value0, *electron, isoTypes[i])) Info(APP_NAME, "Electron default track iso not got.");
//            Info(APP_NAME, "Electron track isolation %d is %f and %f (=%f?)", i, iso, isoV, value0);
//           }
//         }
//        xAOD::CaloIsolation result2;
//        if(m_caloIsoTool.caloTopoClusterIsolation(result2, *electron, isoTypesCalo, corrlist2)){
//          for(unsigned int i=0; i<isoTypesCalo.size(); i++){
//            float value0 = -999;
//            if(!m_isoHelper_Muon->isolation(value0, *electron, isoTypesCalo[i], corrlist2.calobitset)) Info(APP_NAME, "Electron coreCone Calo iso not got.");
//            Info(APP_NAME, "Electron topo isolation %d is %f (=%f?)", i, result2.etcones[i], value0);
//           }
//         }
//        xAOD::CaloIsolation result3;

            if (m_isoHelper_Electron->acceptCorrected(*electron, electronsVec, types)) {
                Info(APP_NAME, "Electron passes Loose working point after correction.");
                electron_acceptCorrectedLoose.push_back(1);
            } else {
                Info(APP_NAME, "Electron does not pass Loose working point after correction.");
                electron_acceptCorrectedLoose.push_back(0);
            }

            if (m_isoHelper_Electron->acceptCorrected(*electron)) {
                Info(APP_NAME, "Electron passes Loose working point before correction.");
                electron_acceptLoose.push_back(1);
            } else {
                Info(APP_NAME, "Electron does not pass Loose working point before correction.");
                electron_acceptLoose.push_back(0);
            }

            // Performs overlap removal.
            vector<Float_t> removals;

            m_isoHelper_Electron->removeOverlap(removals, *electron, types, electronsVec);

            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < removals.size()) {
                    float value = -999999;
                    electron->isolation(value, types.at(j));
                    float afterRemoval = value - removals.at(j);
                    Info(APP_NAME, "Electron Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Electron Value, removal, corrected value: %f, %f, %f", value, removals.at(j), afterRemoval);
                    switch(j) {
                        case 0:
                            electron_ptcone20Rem.push_back(removals.at(j));
                            break;
                        case 1:
                            electron_ptcone30Rem.push_back(removals.at(j));
                            break;
                        case 2:
                            electron_ptcone40Rem.push_back(removals.at(j));
                            break;
                        case 3:
                            electron_ptvarcone20Rem.push_back(removals.at(j));
                            break;
                        case 4:
                            electron_ptvarcone30Rem.push_back(removals.at(j));
                            break;
                        case 5:
                            electron_ptvarcone40Rem.push_back(removals.at(j));
                            break;
                        case 6:
                            electron_topoetcone20Rem.push_back(removals.at(j));
                            break;
                        case 7:
                            electron_topoetcone30Rem.push_back(removals.at(j));
                            break;
                        case 8:
                            electron_topoetcone40Rem.push_back(removals.at(j));
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
//        xAOD::TrackIsolation result;
//        if(m_trkIsoTool.trackIsolation(result, *photon, isoTypes, corrlist)){
//          for(unsigned int i=0; i<isoTypes.size(); i++){
//            float iso = result.ptcones[i];
//            float isoV = result.ptvarcones_10GeVDivPt[i];
//            float value0 = -999;
//            if(!m_isoHelper_Muon->isolation(value0, *photon, isoTypes[i])) Info(APP_NAME, "photon default track iso not got.");
//            Info(APP_NAME, "Photon track isolation %d is %f and %f (=%f?)", i, iso, isoV, value0);
//           }
//         }
//        xAOD::CaloIsolation result2;
//        if(m_caloIsoTool.caloTopoClusterIsolation(result2, *photon, isoTypesCalo, corrlist2)){
//          for(unsigned int i=0; i<isoTypesCalo.size(); i++){
//            float value0 = -999;
//            if(!m_isoHelper_Muon->isolation(value0, *photon, isoTypesCalo[i], corrlist2.calobitset)) Info(APP_NAME, "photon coreCone Calo iso not got.");
//            Info(APP_NAME, "Photon topo isolation %d is %f (=%f?)", i, result2.etcones[i], value0);
//           }
//         }
//        xAOD::CaloIsolation result3;

//         if (m_isoHelper_Photon->acceptCorrected(*photon, photonsVec, types)) {
//             Info(APP_NAME, "Photon passes Loose working point after correction.");
//                photon_acceptCorrectedLoose.push_back(1);
//         } else {
//             Info(APP_NAME, "Photon does not pass Loose working point after correction.");
//                photon_acceptCorrectedLoose.push_back(0);
//         }
//         
//         if (m_isoHelper_Photon->acceptCorrected(*Photon)) {
//             Info(APP_NAME, "Photon passes Loose working point before correction.");
//                photon_acceptLoose.push_back(1);
//         } else {
//             Info(APP_NAME, "Photon does not pass Loose working point before correction.");
//                photon_acceptLoose.push_back(0);
//         }
      
            // Performs overlap removal.
            vector<Float_t> removals;

            m_isoHelper_Photon->removeOverlap(removals, *photon, types, photonsVec);

            for (unsigned int j = 0; j < types.size(); j++) {
                if(j < removals.size()) {
                    float value = -999999;
                    photon->isolation(value, types.at(j));
                    float afterRemoval = value - removals.at(j);
                    Info(APP_NAME, "Photon Isolation variable: %s", typeNames.at(j));
                    Info(APP_NAME, "Photon Value, removal, corrected value: %f, %f, %f", value, removals.at(j), afterRemoval);
                    switch(j) {
                        case 0:
                            photon_ptcone20Rem.push_back(removals.at(j));
                            break;
                        case 1:
                            photon_ptcone30Rem.push_back(removals.at(j));
                            break;
                        case 2:
                            photon_ptcone40Rem.push_back(removals.at(j));
                            break;
                        case 3:
                            photon_ptvarcone20Rem.push_back(removals.at(j));
                            break;
                        case 4:
                            photon_ptvarcone30Rem.push_back(removals.at(j));
                            break;
                        case 5:
                            photon_ptvarcone40Rem.push_back(removals.at(j));
                            break;
                        case 6:
                            photon_topoetcone20Rem.push_back(removals.at(j));
                            break;
                        case 7:
                            photon_topoetcone30Rem.push_back(removals.at(j));
                            break;
                        case 8:
                            photon_topoetcone40Rem.push_back(removals.at(j));
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

//     xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    delete m_isoHelper_Muon;

    return 0;
    
}