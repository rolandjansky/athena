/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
    @brief Test code to test ElectronPhotonVariableCorrectionTool Dictionaries
    @author Nils Gillwald (DESY) nils.gillwald@desy.de
    @date February 2020
**/

// ROOT include(s):
#include <TError.h>

// EDM include(s):
#include "AsgTools/AnaToolHandle.h"

#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"

#define MSGSOURCE "testElectronPhotonVariableCorrectionTool_DictionaryToolHandle"

int main(/*int argc, char* argv[]*/) //unused variable warnings!!
{
    Info(MSGSOURCE, "Configuring the ElectronPhotonVariableCorrectionTool");
    asg::AnaToolHandle<IElectronPhotonShowerShapeFudgeTool> myTool("ElectronPhotonVariableCorrectionTool/myTool");
    myTool.setProperty("ConfigFile", "EGammaVariableCorrection/EGammaVariableCorrectionTool_ExampleConf.conf").ignore();
    if(myTool.initialize() != StatusCode::SUCCESS)
    {
        Error(MSGSOURCE, "Unable to initialize the ElectronPhotonVariableCorrectionTool!");
        return 1;
    }
    else
    {
        Info(MSGSOURCE, "Initialized the ElectronPhotonVariableCorrectionTool!");
    }
    

    return 0;
} //end main
