/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
    @brief Test code to compare output of ElectronPhotonVariableCorrectionBase correction to the isolation correction tool
    @author Nils Gillwald (DESY) nils.gillwald@desy.de
    @date February 2020
**/

// ROOT includes
#include "TFile.h"
#include "TString.h"

//EDM includes
#include "xAODEgamma/PhotonContainer.h" 
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Photon.h"

// must be changed to not be a relative path!
#include "EGammaVariableCorrection/ElectronPhotonVariableCorrectionBase.h"
//
#include "AsgMessaging/AsgMessaging.h"

//infrastructure includes
#ifdef ROOTCORE
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif //ROOTCORE
#include "IsolationCorrections/IsolationCorrectionTool.h"
#include "xAODCore/ShallowCopy.h"

//main test code

int main (int argc, char* argv[])
{
    using namespace asg::msgUserCode;
    // The application's name:
    const char* APP_NAME = argv[0];
    // Check if we received a file name:
    if (argc < 2)
    {
        ANA_MSG_ERROR("No file name received!" );
        ANA_MSG_ERROR( "  Usage: %s [xAOD file name]  %d Num of events to process %d (0 photons , 1 electrons)");
        return EXIT_FAILURE;
    }

    //open input file
    const TString fileName = argv[1];
    ANA_MSG_INFO(APP_NAME << " Opening file: " << fileName.Data());
    std::unique_ptr<TFile> inputFile(TFile::Open(fileName, "READ"));
    ANA_CHECK(inputFile.get());

    // Create a TEvent object (persistent store)
    xAOD::TEvent pers(xAOD::TEvent::kClassAccess);
    // Create a TStore object (transient store)
    xAOD::TStore trans;
    ANA_CHECK(pers.readFrom(inputFile.get()));

    ANA_MSG_INFO("Number of events in the file: " << pers.getEntries());

    // Decide how many events to run over:
    Long64_t entries = pers.getEntries();
    if (argc > 2)
    {
        const Long64_t userInputEntries = atoll(argv[2]);
        if (userInputEntries < entries)
        {
            entries = userInputEntries;
        }
    }

    //initialise the tool
    ElectronPhotonVariableCorrectionBase fudgeTool("fudgeTool");
    ANA_CHECK(fudgeTool.setProperty("ConfigFile","EGammaVariableCorrection/ElectronPhotonVariableCorrectionBase_ExampleIsoCorrectionConf.conf"));
    ANA_CHECK(fudgeTool.initialize());

    //crosscheck with isolationcorrectiontool
    // check these StatusCodes once update to a newer AnalysisBase version (not 21.2.56, probably > 21.2.94)
    CP::IsolationCorrectionTool isoCorrToolFull("isoCorrToolFull");
    ANA_CHECK(isoCorrToolFull.setProperty("Apply_etaEDPar_mc_correction", true));
    ANA_CHECK(isoCorrToolFull.setProperty("Apply_etaEDParPU_correction", true));
    ANA_CHECK(isoCorrToolFull.initialize());

    CP::IsolationCorrectionTool isoCorrToolStep("isoCorrToolStep");
    ANA_CHECK(isoCorrToolStep.setProperty("Apply_etaEDPar_mc_correction", false));
    ANA_CHECK(isoCorrToolStep.setProperty("Apply_etaEDParPU_correction", true));
    ANA_CHECK(isoCorrToolStep.initialize());
    
    //loop over the events
    for (Long64_t entry = 0; entry < entries; entry++)
    {
        //get entry
        pers.getEntry(entry);
        ANA_MSG_INFO("============================");
        ANA_MSG_INFO("Event: " << entry);

        //get photon container
        const xAOD::PhotonContainer* photons;
        ANA_CHECK(pers.retrieve(photons, "Photons"));
        std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > photons_isocorr = xAOD::shallowCopyContainer( *photons );
        std::pair< xAOD::PhotonContainer*, xAOD::ShallowAuxContainer* > photons_fudge = xAOD::shallowCopyContainer( *photons );

        //loop over photon container
        for (unsigned int idx = 0; idx < photons->size(); idx++)
        {
            ANA_MSG_INFO("---------------------------");
            ANA_MSG_INFO("Photon: " << idx);
            ANA_CHECK(isoCorrToolFull.applyCorrection(*(photons_isocorr.first->at(idx))));
            ANA_CHECK(isoCorrToolStep.applyCorrection(*(photons_fudge.first->at(idx))));
            ANA_CHECK(fudgeTool.applyCorrection(*(photons_fudge.first->at(idx))));
            ANA_MSG_INFO("topoetcone40 fudge before applyCorrection: " << photons_fudge.first->at(idx)->auxdata<float>("topoetcone40_original"));
            ANA_MSG_INFO("topoetcone40 fudge after  applyCorrection : " << photons_fudge.first->at(idx)->auxdata<float>("topoetcone40"));
            ANA_MSG_INFO("topoetcone40 with IsolationCorrectionTool : " << photons_isocorr.first->at(idx)->auxdata<float>("topoetcone40"));
        } // loop over photon container
    } // loop over events

    return 0;
}
