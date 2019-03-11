/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s)
#include <TFile.h>
#include <TError.h>

// Infrastructure include(s)
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODCore/ShallowCopy.h"
#include "xAODBase/IParticleHelpers.h"

// EDM include(s)
//#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

// Local include(s)
#include "FsrUtils/FsrPhotonTool.h"
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

// Error checking macro
#define CHECK( ARG )                                            \
    do {                                                        \
        const bool result = ARG;                                \
        if(!result) {                                           \
            ::Error(APP_NAME, "Failed to execute: \"%s\"",      \
                    #ARG );                                     \
            return 1;                                           \
        }                                                       \
    } while( false )


//
// FsrPhotonTester - a simple RootCore executable for testing the FsrUtils
//
// This code compiles automatically when you compile the package
// with RootCore. I.e., simply do
// > rc find_packages
// > rc compile
//
// Run the code with
// FsrPhotonTester XAODFILE
// where XAODFILE is a path to an xAOD file
//

int main( int argc, char* argv[] )
{

    // The application's name
    const char* APP_NAME = argv[ 0 ];

    // Check if we received a file name
    if(argc < 2) {
        Error( APP_NAME, "No file name received!" );
        Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
        return 1;
    }

    // Initialise the application
    CHECK( xAOD::Init(APP_NAME) );
    StatusCode::enableFailure();

    // Open the input file
    const TString fileName = argv[ 1 ];
    Info(APP_NAME, "Opening file: %s", fileName.Data());
    std::auto_ptr<TFile> ifile(TFile::Open(fileName, "READ"));
    CHECK( ifile.get() );

    // Create a TEvent object
    xAOD::TEvent event(xAOD::TEvent::kClassAccess);
    CHECK( event.readFrom(ifile.get()) );
    Info(APP_NAME, "Number of events in the file: %i",
         static_cast<int>(event.getEntries()));

    // Create a Store object for shallow copies
    xAOD::TStore* store = new xAOD::TStore();

    
    // Decide how many events to run over
    Long64_t entries = event.getEntries();
    if(argc > 2) {
        const Long64_t e = atoll(argv[2]);
        if(e < entries) {
            entries = e;
        }
    }

    // Must provide and egamma calib tool to the FSRTool
    CP::EgammaCalibrationAndSmearingTool * energyRescaler = 
        new CP::EgammaCalibrationAndSmearingTool("EgammaCalibrationAndSmearingTool");
    std::string esModel = "es2017_R21_v1";  // move to v1, 2018/09/04
    CHECK( energyRescaler->setProperty( "ESModel", esModel) );
    CHECK( energyRescaler->initialize() );
    ToolHandle<CP::IEgammaCalibrationAndSmearingTool> energyRescalerTool(energyRescaler->name());
    
    // Create and configure the tool
    FSR::FsrPhotonTool fsrTool("FsrPhotonTool");
    CHECK( fsrTool.setProperty<double>("far_fsr_drcut", 0.12) );
    fsrTool.msg().setLevel(MSG::DEBUG);
    CHECK( fsrTool.setProperty( "egCalibToolName", energyRescaler->name()) );
    CHECK( fsrTool.initialize() );

    
    FSR::FsrCandidate candidate;

    // Loop over the events
    for(Long64_t entry = 0; entry < entries; ++entry) {

        store->clear(); // must clear store each event
        
        // Tell the object which entry to look at
        event.getEntry(entry);

        // Print some event information for fun
        // Need to link against xAODEventInfo for this to work
        //const xAOD::EventInfo* ei = 0;
        //CHECK( event.retrieve(ei, "EventInfo") );
        //Info( APP_NAME,
        //      "===>>>  start processing event #%i, "
        //      "run #%i %i events processed so far  <<<===",
        //      static_cast<int>(ei->eventNumber()),
        //      static_cast<int>(ei->runNumber()),
        //      static_cast<int>(entry));

        // Retrieve the muons
        const xAOD::MuonContainer* muons = 0;
        CHECK( event.retrieve( muons, "Muons" ) );

        // get ElectronContainer and clone it in order to apply corrections
        const xAOD::ElectronContainer* els = 0;
        CHECK( event.retrieve( els, "Electrons" ) );

        std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy =
            xAOD::shallowCopyContainer( *els );

        // Record then in the Store
        CHECK( store->record( electrons_shallowCopy.first, "ElectronsCorr" ) );
        CHECK( store->record( electrons_shallowCopy.second, "ElectronsCorrAux." ) );

        xAOD::ElectronContainer* elsCorr = electrons_shallowCopy.first;

        // Must set links between shallow copy and original container
        if (!xAOD::setOriginalObjectLink( *els, *elsCorr )) {
            Error(APP_NAME, "Unable to setOriginalObjectLink for electron containers ");
            return 1;
        }

        // get PhotonContainer and clone it in order to apply corrections
        const xAOD::PhotonContainer* phs = 0;
        CHECK( event.retrieve( phs, "Photons" ) );

        std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > photons_shallowCopy = 
            xAOD::shallowCopyContainer( *phs );

        // Record then in the Store
        CHECK( store->record( photons_shallowCopy.first, "PhotonsCorr" ) );
        CHECK( store->record( photons_shallowCopy.second, "PhotonsCorrAux." ) );
        xAOD::PhotonContainer* phsCorr = photons_shallowCopy.first;
        // Must set links between shallow copy and original container
        if (!xAOD::setOriginalObjectLink( *phs, *phsCorr )) {
            Error(APP_NAME, "Unable to setOriginalObjectLink for photon containers ");
            return 1;
        }
        
      
        //std::vector<const xAOD::Muon*> selectedMuons;
        double tmp_energy = -999.;
        double fsr_energy = 0.;
        TLorentzVector fsr_tlv;

        // Loop over muons
        for( auto muon : *muons ) {

            if (CP::CorrectionCode::Ok != fsrTool.getFsrPhoton(muon, candidate, phsCorr, elsCorr))
                {
                    Info( APP_NAME, " FSR candidate found !!!!!!!! ");
                    Info( APP_NAME, " container = %s deltaR = %f Et = %f f1 = %f"
                          " eta = %f phi = %f phIso = %f fsrtype = %i",
                          candidate.container.c_str(),
                          candidate.deltaR, candidate.Et, candidate.f1,
                          candidate.eta, candidate.phi,
                          candidate.phIso, candidate.type );
                }

            if (candidate.container == "photon" ) {
                const xAOD::Photon* photon = dynamic_cast<const xAOD::Photon*>(candidate.particle);
                if (photon) fsr_energy = photon->e();
            } 
            else if (candidate.container == "electron" ) {
                const xAOD::Electron* electron = dynamic_cast<const xAOD::Electron*>(candidate.particle);
                if (electron) fsr_energy = electron->e();
            } else
                Info(APP_NAME, " FSR candidate particle is unknown " );

            if ( fsr_energy > tmp_energy ) {
                tmp_energy = fsr_energy;
                fsr_tlv.SetPtEtaPhiE(candidate.Et, candidate.eta, candidate.phi, fsr_energy);
            }

        }

        xAOD::IOStats::instance().stats().printSmartSlimmingBranchList();

    }

    delete store;
    
    return EXIT_SUCCESS;

}
