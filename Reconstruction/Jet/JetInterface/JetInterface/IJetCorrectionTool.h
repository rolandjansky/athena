/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETINTERFACE_IJETCORRECTIONTOOL_H //To avoid re-definition
#define JETINTERFACE_IJETCORRECTIONTOOL_H

// EDM includes
#include "xAODJet/Jet.h"

// Other includes
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"


/// Interface class for smearing the jet mass scale and resolution
///
/// For information, see the Twiki:
/// https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/FFJetSmearingTool
///
/// If you find any bug, please, contact <alberto.prades.ibanez@cern.ch>


class IJetCorrectionTool : public virtual CP::ISystematicsTool
{

    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IJetCorrectionTool )

    public:

        /// Virtual destructor
        virtual ~IJetCorrectionTool(){};

        virtual CP::CorrectionCode applyCorrection(xAOD::Jet* jet_reco) = 0;

}; // class IJetCorrectionTool


#endif /* JETINTERFACE_IJETCORRECTIONTOOL_H  */
