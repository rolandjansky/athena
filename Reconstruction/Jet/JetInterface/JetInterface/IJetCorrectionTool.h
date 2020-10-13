/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETINTERFACE_IJETCORRECTIONTOOL_H //To avoid re-definition
#define JETINTERFACE_IJETCORRECTIONTOOL_H

// EDM includes
#include "AsgTools/IAsgTool.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"

class IJetCorrectionTool : virtual public asg::IAsgTool
{

    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IJetCorrectionTool )

    public:

        /// Virtual destructor
        virtual ~IJetCorrectionTool()=default;

        virtual StatusCode getMatchedTruthJet( xAOD::Jet& jet_reco, xAOD::Jet& jet_truth_matched) const = 0;
        //The other public functions from the tool that will be used by the tool are live inside "CP" so they are declared in "JetCPInterfaces/ICPJetCorrectionTool"

}; // class IJetCorrectionTool


#endif /* JETINTERFACE_IJETCORRECTIONTOOL_H  */
