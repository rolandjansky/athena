/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// a simple testing macro for the MuonEfficiencyCorrections_xAOD package
/// shamelessly stolen from CPToolTests.cxx

// System include(s):
#include <memory>
#include <cstdlib>
#include <string>
#include <map>

// ROOT include(s):
#include <TFile.h>
#include <TError.h>
#include <TStopwatch.h>
#include <TString.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/tools/ReturnCheck.h"
#   include "AsgTools/Check.h"
#endif // ROOTCORE

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

// Local include(s):
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "PATInterfaces/ISystematicsTool.h"

#define CHECK_CPCorr(Arg) \
    if (Arg.code() == CP::CorrectionCode::Error){    \
        Error(#Arg,"Correction Code 'Error' (returned in line %i) ",__LINE__); \
        return 1;   \
    }
#define CHECK_CPSys(Arg) \
    if (Arg.code() == CP::SystematicCode::Unsupported){    \
        Warning(#Arg,"Unsupported systematic (in line %i) ",__LINE__); \
    }      

/// Example of how to run the MuonEfficiencyCorrections package to obtain information from muons

int main( int argc, char* argv[] ) {

    // force strict checking of return codes
    //	CP::SystematicCode::enableFailure();
    //	xAOD::TReturnCode::enableFailure();

    StatusCode::enableFailure();
    // The application's name:
    const char* APP_NAME = argv[ 0 ];

    // check( if we received a file name:
    if( argc < 2 ) {
        Error( APP_NAME, "No file name received!" );
        Error( APP_NAME, "  Usage: %s [xAOD file name]", APP_NAME );
        return 1;
    }

    // Initialise the application:
//     ATH_CHECK( xAOD::Init( APP_NAME ) );
     RETURN_CHECK(APP_NAME,xAOD::Init( APP_NAME ));

    // Open the input file:
    const TString fileName = argv[ 1 ];
    Info( APP_NAME, "Opening file: %s", fileName.Data() );
    std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
    if (!ifile.get()){
        Error(APP_NAME," Unable to load xAOD input file");
    }

    // Create a TEvent object:
    xAOD::TEvent event;
    RETURN_CHECK(APP_NAME,event.readFrom( ifile.get(), xAOD::TEvent::kClassAccess ));
    Info( APP_NAME, "Number of events in the file: %i",
            static_cast< int >( event.getEntries() ) );

    // Decide how many events to run over:
    Long64_t entries = event.getEntries();
    if( argc > 2 ) {
        const Long64_t e = atoll( argv[ 2 ] );
        if( e < entries ) {
            entries = e;
        }
    }

    // instantiate the MCP tool
    CP::MuonEfficiencyScaleFactors m_effi_corr("TestSFClass");

    // Configure it :

    ASG_CHECK_SA(APP_NAME,m_effi_corr.setProperty("WorkingPoint","CBandST"));
    ASG_CHECK_SA(APP_NAME, m_effi_corr.setProperty("DataPeriod","2012"));
    // test audit trail
    ASG_CHECK_SA(APP_NAME,m_effi_corr.setProperty("doAudit",true));		// audit trail functionality.

    // test if the tool is robust against nonexistent properties being set
    //    m_effi_corr.setProperty("Foo","Bar");

    //     set custom lumis (optional)
    std::map<std::string, double> custom_lumis;
    //    custom_lumis["A"] = 1;
    //    custom_lumis["B"] = 42;
    //    custom_lumis["E"] = 1337;
    //    m_effi_corr.setProperty("LumiWeights",custom_lumis);

    TStopwatch tsw;
    tsw.Start();
    ASG_CHECK_SA(APP_NAME,m_effi_corr.initialize());
    double t_init = tsw.CpuTime();
    tsw.Reset();

    // try out systematics support - define a few sets to run
    CP::SystematicSet statup;
    statup.insert (CP::SystematicVariation ("MUONSFSTAT", 1));
    // make sure the tool is not affected by other unsupported systematics in the same set
    statup.insert (CP::SystematicVariation ("TÖRÖÖÖÖ", 1));
    // running two variations affecting the MCP tool in the same set should result in a useful error message
    //	statup.insert (CP::SystematicVariation ("MUONSFSTAT", -1));  // uncomment to test (will cause abort at first muon)

    CP::SystematicSet statdown;
    statdown.insert (CP::SystematicVariation ("MUONSFSTAT", -1));

    CP::SystematicSet sysup;
    sysup.insert (CP::SystematicVariation ("MUONSFSYS", 1));

    CP::SystematicSet sysdown;
    sysdown.insert (CP::SystematicVariation ("MUONSFSYS", -1));




    // Loop over the events:
    tsw.Start();

    // prepare a vector to hold SF replicas
    // the size will tell the tool how many you want
    // -> we will try 50!
    std::vector<float> replicas(50);
    for( Long64_t entry = 0; entry < entries; ++entry ) {

        // Tell the object which entry to look at:
        event.getEntry( entry );

        // Print some event information for fun:
        const xAOD::EventInfo* ei = 0;
        RETURN_CHECK(APP_NAME, event.retrieve( ei, "EventInfo" ) );
        Info( APP_NAME,
                "===>>>  start processing event #%i, "
                "run #%i %i events processed so far  <<<===",
                static_cast< int >( ei->eventNumber() ),
                static_cast< int >( ei->runNumber() ),
                static_cast< int >( entry ) );

        // Get the Muons from the event:
        const xAOD::MuonContainer* muons = 0;
        RETURN_CHECK(APP_NAME, event.retrieve( muons, "Muons" ));
        Info( APP_NAME, "Number of muons: %i",
                static_cast< int >( muons->size() ) );

        // Print their properties, using the tools:
        xAOD::MuonContainer::const_iterator mu_itr = muons->begin();
        xAOD::MuonContainer::const_iterator mu_end = muons->end();



        for( ; mu_itr != mu_end; ++mu_itr ) {

            // Print some info about the selected muon:
            Info( APP_NAME, "  Selected muon: eta = %g, phi = %g, pt = %g",
                    ( *mu_itr )->eta(), ( *mu_itr )->phi(), ( *mu_itr )->pt() );

            // Use the "simple interface" of the tool(s):
            float eff = 0.0, sf = 0.0;

            // directly obtain some efficiencies and SF

            CHECK_CPCorr( m_effi_corr.getRecoEfficiency( **mu_itr, eff ) );
            Info( APP_NAME, "        efficiency = %g", eff );

            CHECK_CPSys(m_effi_corr.applySystematicVariation(CP::SystematicSet()));
            CHECK_CPCorr( m_effi_corr.getEfficiencyScaleFactor( **mu_itr, sf ) );
            Info( APP_NAME, "       Central scaleFactor = %g", sf );
            // if in audit mode, this should return a true
            Info( APP_NAME, "    Already applied this SF: %d", m_effi_corr.AlreadyApplied(**mu_itr) );
            CHECK_CPSys(m_effi_corr.applySystematicVariation(statup));
            // and this a false (since we are looking at a different systematic)
            Info( APP_NAME, "    Already applied this SF: %d", m_effi_corr.AlreadyApplied(**mu_itr) );
            //         }
            CHECK_CPCorr( m_effi_corr.getEfficiencyScaleFactor( **mu_itr, sf ) );
            Info( APP_NAME, "           Stat Up scaleFactor = %g", sf );
            CHECK_CPSys(m_effi_corr.applySystematicVariation(statdown));
            CHECK_CPCorr( m_effi_corr.getEfficiencyScaleFactor( **mu_itr, sf ) );
            Info( APP_NAME, "           Stat Down scaleFactor = %g", sf );
            CHECK_CPSys(m_effi_corr.applySystematicVariation(sysup));
            CHECK_CPCorr( m_effi_corr.getEfficiencyScaleFactor( **mu_itr, sf ) );
            Info( APP_NAME, "           Sys Up scaleFactor = %g", sf );
            CHECK_CPSys(m_effi_corr.applySystematicVariation(sysdown));
            CHECK_CPCorr( m_effi_corr.getEfficiencyScaleFactor( **mu_itr, sf ) );
            Info( APP_NAME, "           Sys Down scaleFactor = %g", sf );
            CHECK_CPSys(m_effi_corr.applySystematicVariation(CP::SystematicSet()));

            // uncomment to try out replica genration (commented as it produces a lot of text)
            CHECK_CPCorr( m_effi_corr.getEfficiencyScaleFactorReplicas( **mu_itr, replicas ) );
            //
            //
            //         for (size_t t =0; t < replicas.size();t++){
            //        	 Info( APP_NAME, "       scaleFactor Replica %d = %.8f",static_cast<int>(t), replicas[t] );
            //         }


            // also run in decorating mode... Note that here we use our non-const copy created further up
            CHECK_CPCorr( m_effi_corr.applyEfficiencyScaleFactor( **mu_itr ) );
            CHECK_CPCorr( m_effi_corr.applyRecoEfficiency( **mu_itr ) );
            // now we can retrieve the info from the muon directly:
            Info( APP_NAME, "       efficiency from decorated muon = %g",
                    (**mu_itr).auxdataConst< float >( "Efficiency" ) );
            Info( APP_NAME, "       SF from decorated muon = %g",
                    (**mu_itr).auxdataConst< float >( "EfficiencyScaleFactor" ) );

            // if we run in audit trail mode, we get some info
            //             Info( APP_NAME,"    Muon Audit info: MuonEfficiencyCorrections = %d, MuonEfficiencyCorrectionsVersion = %s, AppliedCorrections = %s",
            //												 (**mu_itr).auxdataConst< bool >( "MuonEfficiencyCorrections" ),
            //												 (**mu_itr).auxdataConst< std::string >( "MuonEfficiencyCorrectionsVersion" ).c_str(),
            //												 (**mu_itr).auxdataConst< std::string >( "AppliedCorrections" ).c_str());
        }

        // Close with a message:
        Info( APP_NAME,
                "===>>>  done processing event #%i, "
                "run #%i %i events processed so far  <<<===",
                static_cast< int >( ei->eventNumber() ),
                static_cast< int >( ei->runNumber() ),
                static_cast< int >( entry + 1 ) );
    }
    double t_run = tsw.CpuTime() / entries;
    Info( APP_NAME, " MCP init took %g s",t_init);
    Info( APP_NAME, " time per event: %g s",t_run);

    // Return gracefully:
    return 0;
}
