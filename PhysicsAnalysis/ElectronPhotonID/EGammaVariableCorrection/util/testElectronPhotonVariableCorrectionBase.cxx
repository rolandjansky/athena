/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief Test if ElectronPhotonVariableCorrectionBase runs fine on electrons and (un-)converted photons
 * @author Nils Gillwald (DESY) nils.gillwald@desy.de
 * @date February 2020
 **/

// ROOT includes
#include "TFile.h"
#include "TString.h"

//EDM includes
#include "xAODEgamma/ElectronContainer.h" 
#include "xAODEgamma/PhotonContainer.h" 
#include "xAODEgamma/PhotonAuxContainer.h"
#include "xAODEgamma/Electron.h" 
#include "xAODEgamma/Photon.h"

#include "EGammaVariableCorrection/ElectronPhotonVariableCorrectionBase.h"
//
#include "AsgMessaging/AsgMessaging.h"

////infrastructure includes
#ifdef ROOTCORE
#include "xAODRootAccess/TEvent.h"
//#include "xAODRootAccess/TStore.h"
#endif //ROOTCORE

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

    // Check if we want to process Electron/Photon shifts
    bool isPhoton = false;
    bool isElectron = false;   
    if (argc < 4)
    {
        ANA_MSG_INFO(APP_NAME << " By default looking at Photon" );
        isPhoton=true;
    }
    else
    {  
        int argv1 = atoi(argv[3]);
        if (argv1 == 0)
        { 
            isPhoton = true;
            ANA_MSG_INFO (APP_NAME << " We are looking at photon shifts" );
        }
        else if (argv1 == 1)
        {
            isElectron = true;
            ANA_MSG_INFO(APP_NAME << " We are looking at electron shifts");
        }
        else
        {
            ANA_MSG_ERROR ("Usage: %s [xAOD file name]  %d Num of events to process %d (0 photons , 1 electrons)");
            return EXIT_FAILURE;
        }
    }

    //open input file
    const TString fileName = argv[1];
    ANA_MSG_INFO(APP_NAME << " Opening file: " << fileName.Data());
    std::unique_ptr<TFile> inputFile(TFile::Open(fileName, "READ")); //probably rather "READ"?
    ANA_CHECK(inputFile.get());

    // Create a TEvent object (persistent store)
    xAOD::TEvent pers(xAOD::TEvent::kClassAccess);
    // Create a TStore object (transient store)
    xAOD::TStore trans;
    ANA_CHECK(pers.readFrom(inputFile.get()));
    //
    ANA_MSG_INFO("Number of events in the file: " << pers.getEntries());

    // Decide how many events to run over:
    Long64_t entries = pers.getEntries();
    if (argc > 2)
    {
        const Long64_t userInputEntries = atoll(argv[2]);
        if (userInputEntries < entries)
        {
            entries = userInputEntries;
            ANA_MSG_INFO("Running over " << userInputEntries << " events.");
        }
    }

    // ===============================================
    // Photon test
    // ===============================================
    if (isPhoton)
    {
        //initialise the tool
        //converted photons
        std::string configFilePathConverted = "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionBase_ExampleConvertedPhotonConf_Eratio.conf";
        ElectronPhotonVariableCorrectionBase CorrectConvertedPhotonTool("CorrectConvertedPhotonTool");
        ANA_CHECK(CorrectConvertedPhotonTool.setProperty("ConfigFile",configFilePathConverted));
        ANA_CHECK(CorrectConvertedPhotonTool.initialize());
        //unconverted photons
        std::string configFilePathUnconverted = "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionBase_ExampleUnconvertedPhotonConf_Eratio.conf";
        ElectronPhotonVariableCorrectionBase CorrectUnconvertedPhotonTool("CorrectUnconvertedPhotonTool");
        ANA_CHECK(CorrectUnconvertedPhotonTool.setProperty("ConfigFile",configFilePathUnconverted));
        ANA_CHECK(CorrectUnconvertedPhotonTool.initialize());

        std::string correctionVariable = CorrectConvertedPhotonTool.getCorrectionVariable();
        ANA_MSG_INFO("Correcting Variable: " << correctionVariable);

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

            // Make a deep copy of the photon container
            // Create the new container and its auxiliary store.
            auto photons_copy = std::make_unique<xAOD::PhotonContainer>();
            auto photons_copy_aux = std::make_unique<xAOD::AuxContainerBase>();
            photons_copy->setStore (photons_copy_aux.get()); //< Connect the two
            
            //copy photons over
            for (auto photon : *photons) {
                // Copy this photon to the output container:
                xAOD::Photon* photon_copy = new xAOD::Photon();
                photons_copy->push_back (photon_copy); // photon acquires the photon_copy auxstore
                *photon_copy = *photon; // copies auxdata from one auxstore to the other
            }
            // end make deep copy

            //loop over deep copy of photon container
            for (unsigned int photon_itr = 0; photon_itr < photons_copy->size(); photon_itr++)
            {
                ANA_MSG_INFO("---------------------------");
                ANA_MSG_INFO("Photon: " << photon_itr);
                xAOD::Photon* photon = photons_copy->at(photon_itr);

                //apply correction
                if (xAOD::EgammaHelpers::isConvertedPhoton(photon))
                {
                    ANA_MSG_INFO("Converted Photon.");
                    ANA_CHECK(CorrectConvertedPhotonTool.applyCorrection(*photon));
                }
                else
                {
                    ANA_MSG_INFO("Unconverted Photon.");
                    ANA_CHECK(CorrectUnconvertedPhotonTool.applyCorrection(*photon));
                }
                
                //get original and corrected value
                SG::AuxElement::Accessor<float> VariableToCorrect(correctionVariable + "_original");
                SG::AuxElement::Accessor<float> CorrectedVariable(correctionVariable);

                //print results
                ANA_MSG_INFO("Original value:  " << VariableToCorrect(*photon));
                ANA_MSG_INFO("Corrected value: " << CorrectedVariable(*photon));

            } // loop over deep copy of photon container
        } // loop over events
    } // if isPhoton

    // ===============================================
    // Electron test
    // ===============================================
    if (isElectron)
    {
        std::string configFilePath = "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionBase_ExampleElectronConf_Eratio.conf";
        ElectronPhotonVariableCorrectionBase CorrectElectronTool("CorrectElectronTool");
        ANA_CHECK(CorrectElectronTool.setProperty("ConfigFile",configFilePath));
        ANA_CHECK(CorrectElectronTool.initialize());

        std::string correctionVariable = CorrectElectronTool.getCorrectionVariable();
        ANA_MSG_INFO("Correcting Variable: " << correctionVariable);

        //loop over the events
        for (Long64_t entry = 0; entry < entries; entry++)
        {
            //get entry
            pers.getEntry(entry);
            ANA_MSG_INFO("============================");
            ANA_MSG_INFO("Event: " << entry);

            //get electron container
            const xAOD::ElectronContainer* electrons;
            ANA_CHECK(pers.retrieve(electrons, "Electrons"));

            // Make a deep copy of the electron container
            // Create the new container and its auxiliary store.
            auto electrons_copy = std::make_unique<xAOD::ElectronContainer>();
            auto electrons_copy_aux = std::make_unique<xAOD::AuxContainerBase>();
            electrons_copy->setStore (electrons_copy_aux.get()); //< Connect the two
            
            //copy electrons over
            for (auto electron : *electrons) {
                // Copy this electron to the output container:
                xAOD::Electron* electron_copy = new xAOD::Electron();
                electrons_copy->push_back (electron_copy); // electron acquires the electron_copy auxstore
                *electron_copy = *electron; // copies auxdata from one auxstore to the other
            }
            // end make deep copy

            //loop over deep copy of electron container
            for (unsigned int electron_itr = 0; electron_itr < electrons_copy->size(); electron_itr++)
            {
                ANA_MSG_INFO("---------------------------");
                ANA_MSG_INFO("Electron: " << electron_itr);
                xAOD::Electron* electron = electrons_copy->at(electron_itr);

                //apply correction
                ANA_CHECK(CorrectElectronTool.applyCorrection(*electron));

                //get original and corrected value
                SG::AuxElement::Accessor<float> VariableToCorrect(correctionVariable + "_original");
                SG::AuxElement::Accessor<float> CorrectedVariable(correctionVariable);

                //print results
                ANA_MSG_INFO("Original value:  " << VariableToCorrect(*electron));
                ANA_MSG_INFO("Corrected value: " << CorrectedVariable(*electron));

            } // loop over deep copy of electron container
        } // loop over events
    } // if isElectron

    return 0;
}
