/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETINTERFACE_IJETCORRECTIONTOOL_H //To avoid re-definition
#define JETINTERFACE_IJETCORRECTIONTOOL_H

// EDM includes
#include "xAODJet/Jet.h"

class IJetCorrectionTool : virtual public asg::IAsgTool
{

    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IJetCorrectionTool )

    public:

        /// Virtual destructor
        virtual ~IJetCorrectionTool(){};


        //All the public functions from the tool that will be used by the tool are live inside "CP" so they are declared in "JetCPInterfaces/ICPJetCorrectionTool"

}; // class IJetCorrectionTool


#endif /* JETINTERFACE_IJETCORRECTIONTOOL_H  */
