/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCPINTERFACES_ICPJETUNCERTAINTIESTOOL_H
#define JETCPINTERFACES_ICPJETUNCERTAINTIESTOOL_H

#include "JetInterface/IJetUncertaintiesTool.h"

#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"

class ICPJetUncertaintiesTool : virtual public IJetUncertaintiesTool,
                                virtual public CP::ISystematicsTool
{
    // Interface declaration
    ASG_TOOL_INTERFACE(ICPJetUncertaintiesTool)

    public:
        ICPJetUncertaintiesTool& operator=( ICPJetUncertaintiesTool&& ) { return *this; }

        // Apply a systematic variation or get a new copy
        virtual CP::CorrectionCode applyCorrection(xAOD::Jet& input) const = 0;
        virtual CP::CorrectionCode applyCorrection(xAOD::Jet& input, const xAOD::EventInfo& eInfo) const = 0;
        virtual CP::CorrectionCode correctedCopy(const xAOD::Jet& input, xAOD::Jet*& output) const = 0;
        virtual CP::CorrectionCode correctedCopy(const xAOD::Jet& input, xAOD::Jet*& output, const xAOD::EventInfo& eInfo) const = 0;
        virtual CP::CorrectionCode applyContainerCorrection(xAOD::JetContainer& inputs) const = 0;
        virtual CP::CorrectionCode applyContainerCorrection(xAOD::JetContainer& inputs, const xAOD::EventInfo& eInfo) const = 0;

};

#endif

