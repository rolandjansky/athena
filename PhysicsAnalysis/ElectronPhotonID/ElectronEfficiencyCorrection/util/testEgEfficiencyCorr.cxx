/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// System include(s):
#include <memory>
#include <cstdlib>
#include "Messaging.h"

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/Egamma.h"
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "xAODCore/ShallowCopy.h"
#include "AsgTools/AsgMessaging.h"


#include <string>
#include "PATInterfaces/SystematicsUtil.h"

int main( int argc, char* argv[] ) {

    // The application's name:
    const char* APP_NAME = argv[ 0 ];

    MSG::Level mylevel=MSG::DEBUG;
    MSGHELPERS::getMsgStream().msg().setLevel(mylevel);
    MSGHELPERS::getMsgStream().msg().setName(APP_NAME);

    // Check if we received a file name:
    if( argc < 2 ) {
        Error( APP_NAME, "No file name received!" );
        Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
        return 1;
    }

    // Initialise the application:
    CHECK( xAOD::Init( APP_NAME ) );

    // Open the input file:
    const TString fileName = argv[ 1 ];
    Info( APP_NAME, "Opening file: %s", fileName.Data() );
    std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
    CHECK( ifile.get() );

    // Create a TEvent object:
    //xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
    xAOD::TEvent event( xAOD::TEvent::kClassAccess );
    CHECK( event.readFrom( ifile.get() ) );
    Info( APP_NAME, "Number of events in the file: %i",
            static_cast< int >( event.getEntries() ) );

    std::cout << "=="<<std::endl;


    // Decide how many events to run over:
    Long64_t entries = event.getEntries();
    if( argc > 2 ) {
        const Long64_t e = atoll( argv[ 2 ] );
        if( e < entries ) {
            entries = e;
        }
    }

    //Likelihood
    AsgElectronEfficiencyCorrectionTool myEgCorrections ("myEgCorrections");
    //std::vector<std::string> inputFiles{"ElectronEfficiencyCorrection/efficiencySF.offline.Loose.2012.8TeV.rel17p2.v07.root"} ;
    std::vector<std::string> inputFiles{"efficiencySF.offline.MediumLLH_d0z0_v11.root"} ;
    CHECK( myEgCorrections.setProperty("CorrectionFileNameList",inputFiles) );
    myEgCorrections.msg().setLevel(mylevel);

    CHECK( myEgCorrections.setProperty("ForceDataType",3) );
    CHECK( myEgCorrections.setProperty("CorrelationModel", "SIMPLIFIED" ));
    CHECK( myEgCorrections.setProperty("UseRandomRunNumber", false ));
    CHECK( myEgCorrections.setProperty("DefaultRandomRunNumber", 299999));

    CHECK( myEgCorrections.initialize() );

    // Get a list of systematics
    CP::SystematicSet recSysts = myEgCorrections.recommendedSystematics();
    // Convert into a simple list
    std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recSysts);
    std::cout << "=="<<std::endl;
    // Loop over the events:
    Long64_t entry = 10;
    entries = entry+1;
    for(  ; entry < entries; ++entry ) {

        // Tell the object which entry to look at:
        event.getEntry( entry );

        std::cout << "=================NEXT EVENT==========================" << std::endl;
        //  Info (APP_NAME,"Electron 6" );


        const xAOD::EventInfo* event_info = 0;  
        CHECK( event.retrieve( event_info, "EventInfo" ) ); 

        const xAOD::ElectronContainer* electrons = 0;  
        //CHECK( event.retrieve(electrons, "ElectronCollection") ); //For DAOD
        CHECK( event.retrieve(electrons, "Electrons") );

        // Loop over systematics
        for(const auto& sys : sysList){

            Info(APP_NAME, "Processing syst: %s", sys.name().c_str());

            // Configure the tool for this systematic
            CHECK( myEgCorrections.applySystematicVariation(sys) );
            Info(APP_NAME, "Applied syst: %s", 
                    myEgCorrections.appliedSystematics().name().c_str());

            // Create shallow copy for this systematic
            std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy =
                xAOD::shallowCopyContainer( *electrons );

            //Iterate over the shallow copy
            xAOD::ElectronContainer* elsCorr = electrons_shallowCopy.first;
            xAOD::ElectronContainer::iterator el_it      = elsCorr->begin();
            xAOD::ElectronContainer::iterator el_it_last      = elsCorr->end();

            unsigned int i = 0;
            double SF = 0; 
            for (; el_it != el_it_last; ++el_it, ++i) { 

                xAOD::Electron* el = *el_it;
                if(el->pt() < 7000) continue;//skip electrons outside of recommendations

                std::cout << "Electron " << i << std::endl; 
                std::cout << "xAOD/raw pt = " << el->pt() << ", eta: "
                    << el->caloCluster()->etaBE(2) << std::endl; 

                Info (APP_NAME,"Electron #%d", i); 

                int sysreg = myEgCorrections.systUncorrVariationIndex(*el);
                Info (APP_NAME,"sysregion %d ", sysreg);


                if(myEgCorrections.getEfficiencyScaleFactor(*el,SF) != CP::CorrectionCode::Ok){
                    Error( APP_NAME, "Problem in getEfficiencyScaleFactor");
                    return EXIT_FAILURE;
                }

                if(myEgCorrections.applyEfficiencyScaleFactor(*el) != CP::CorrectionCode::Ok){
                    Error( APP_NAME, "Problem in applyEfficiencyScaleFactor");
                    return EXIT_FAILURE;
                }

                Info( APP_NAME, "===>>> Resulting SF (from get function) %f, (from apply function) %f",
                        SF, el->auxdata< float >("SF"));       

            }

        }

        Info( APP_NAME,
                "===>>>  done processing event #%lld ",entry);

    }


    CHECK( myEgCorrections.finalize() );

    // Return gracefully:
    return 0;
}
