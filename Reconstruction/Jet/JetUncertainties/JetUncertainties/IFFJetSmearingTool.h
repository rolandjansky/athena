/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FFJETSMEARINGTOOL_IFFJETSMEARINGTOOL_H //To avoid re-definition
#define FFJETSMEARINGTOOL_IFFJETSMEARINGTOOL_H

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


class IFFJetSmearingTool : public virtual CP::ISystematicsTool
{

    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( IFFJetSmearingTool )

    public:

        /// Virtual destructor
        virtual ~IFFJetSmearingTool(){};

        virtual CP::CorrectionCode applyCorrection(xAOD::Jet* jet_reco) = 0;

}; // class IFFJetSmearingTool


#endif /* FFJETSMEARINGTOOL_IFFJETSMEARINGTOOL_H  */
