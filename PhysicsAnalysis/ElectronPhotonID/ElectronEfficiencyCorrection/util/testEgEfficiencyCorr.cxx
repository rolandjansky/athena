/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    MSG_INFO("Opening file: " << fileName.Data() );
    std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
    CHECK( ifile.get() );

    // Create a TEvent object:
    //xAOD::TEvent event( xAOD::TEvent::kBranchAccess );
    xAOD::TEvent event( xAOD::TEvent::kClassAccess );
    CHECK( event.readFrom( ifile.get() ) );
    MSG_INFO( "Number of events in the file:  " <<
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

    AsgElectronEfficiencyCorrectionTool ElEffCorrectionTool ("ElEffCorrectionTool");   
    CHECK( ElEffCorrectionTool.setProperty("IdKey", "Medium"));
    CHECK( ElEffCorrectionTool.setProperty("ForceDataType",3 ));
    CHECK( ElEffCorrectionTool.setProperty("OutputLevel", mylevel ));
    CHECK( ElEffCorrectionTool.setProperty("CorrelationModel", "SIMPLIFIED" )); 
    CHECK( ElEffCorrectionTool.setProperty("UseRandomRunNumber", false ));
    CHECK( ElEffCorrectionTool.setProperty("DefaultRandomRunNumber", 301000));
    CHECK( ElEffCorrectionTool.initialize());  
    asg::ToolStore::dumpToolConfig();

    // Get a list of systematics
    CP::SystematicSet recSysts = ElEffCorrectionTool.recommendedSystematics();
    // Convert into a simple list
    std::vector<CP::SystematicSet> sysList = CP::make_systematics_vector(recSysts);
    std::cout << "=="<<std::endl;
    // Loop over the events:
    Long64_t entry = 10;
    entries = entry+1;
    for(  ; entry < entries; ++entry ) {

        // Tell the object which entry to look at:
        event.getEntry( entry );

        MSG_INFO( "=================NEXT EVENT==========================");
        //  Info (APP_NAME,"Electron 6" );


        const xAOD::EventInfo* event_info = 0;  
        CHECK( event.retrieve( event_info, "EventInfo" ) ); 

        const xAOD::ElectronContainer* electrons = 0;  
        //CHECK( event.retrieve(electrons, "ElectronCollection") ); //For DAOD
        CHECK( event.retrieve(electrons, "Electrons") );

        // Loop over systematics
        for(const auto& sys : sysList){

            MSG_INFO("Processing syst: " << sys.name().c_str());

            // Configure the tool for this systematic
            CHECK( ElEffCorrectionTool.applySystematicVariation(sys) );
            MSG_INFO("Applied syst: "<< 
                    ElEffCorrectionTool.appliedSystematics().name().c_str());

            // Create shallow copy for this systematic
            std::pair< xAOD::ElectronContainer*, xAOD::ShallowAuxContainer* > electrons_shallowCopy =
                xAOD::shallowCopyContainer( *electrons );

            //Iterate over the shallow copy
            xAOD::ElectronContainer* elsCorr = electrons_shallowCopy.first;
            xAOD::ElectronContainer::iterator el_it = elsCorr->begin();
            xAOD::ElectronContainer::iterator el_it_last = elsCorr->end();

            unsigned int i = 0;
            double SF = 0; 
            for (; el_it != el_it_last; ++el_it, ++i) { 
                xAOD::Electron* el = *el_it;
                if(!el->caloCluster()){
                    MSG_ERROR( "No cluster associated to the electron");
                    return EXIT_FAILURE;
                }
                if(el->pt() < 7000) continue;//skip electrons outside of recommendations

                MSG_INFO("Electron " << i); 
                MSG_INFO("xAOD/raw pt = " << el->pt() << ", eta: "
                        << el->caloCluster()->etaBE(2));
                MSG_INFO("Electron # "<< i); 

                int sysreg = ElEffCorrectionTool.systUncorrVariationIndex(*el);
                MSG_INFO( "sysregion " <<sysreg);

                if(ElEffCorrectionTool.getEfficiencyScaleFactor(*el,SF) != CP::CorrectionCode::Ok){
                    MSG_ERROR("Problem in getEfficiencyScaleFactor");
                    return EXIT_FAILURE;
                }

                if(ElEffCorrectionTool.applyEfficiencyScaleFactor(*el) != CP::CorrectionCode::Ok){
                    MSG_INFO( "Problem in applyEfficiencyScaleFactor");
                    return EXIT_FAILURE;
                }
                MSG_INFO( "===>>> Resulting SF (from get function) " <<SF <<" (from apply function) " <<
                        el->auxdata< float >("SF"));       
            }
        }
        MSG_INFO("===>>>  done processing event # " <<entry);

    }
    CHECK( ElEffCorrectionTool.finalize() );
    // Return gracefully:
    return 0;
}
