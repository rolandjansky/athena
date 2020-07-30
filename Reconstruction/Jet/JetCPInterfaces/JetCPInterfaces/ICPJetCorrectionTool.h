/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 *   */

#ifndef JETCPINTERFACES_ICPJETCORRECTIONTOOL_H
#define JETCPINTERFACES_ICPJETCORRECTIONTOOL_H

#include "JetInterface/IJetCorrectionTool.h"

#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

/// Interface class for smearing the jet mass scale and resolution
///
/// For information, see the Twiki:
/// https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/FFJetSmearingTool
///
/// If you find any bug, please, contact <alberto.prades.ibanez@cern.ch>

class ICPJetCorrectionTool : virtual public IJetCorrectionTool,
                             virtual public CP::ISystematicsTool
{
    // Interface declaration
    ASG_TOOL_INTERFACE(ICPJetCorrectionTool)

    public:
        ICPJetCorrectionTool& operator=( ICPJetCorrectionTool&& ) { return *this; }

        // Apply a systematic variation 
        virtual CP::CorrectionCode applyCorrection(xAOD::Jet& jet_reco) = 0;

}; // class ICPJetCorrectionTool

#endif /* JETINTERFACE_IJETCPCORRECTIONTOOL_H  */
