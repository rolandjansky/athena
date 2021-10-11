/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef IMUONTRIGGERSCALEFACTORS_H_
#define IMUONTRIGGERSCALEFACTORS_H_

#include <map>
#include <string>
#include <vector>

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

namespace TrigMuonEff {

    /**
     *  strcuture to hold variables to configure behaviour of the tool
     */
    struct Configuration {
        mutable bool isData;
        int replicaIndex; /**<  index of replicated histograms to access */

        Configuration(const bool isData_ = true, const int replicaIndex_ = -1) : isData(isData_), replicaIndex(replicaIndex_) {}
    };
}  // namespace TrigMuonEff

/**
 * Helper enumeration for systematics study
 */

namespace CP {

    class IMuonTriggerScaleFactors : public virtual asg::IAsgTool, virtual public CP::ISystematicsTool {
        ASG_TOOL_INTERFACE(CP::IMuonTriggerScaleFactors)

    public:
        virtual StatusCode initialize(void) = 0;

        virtual CorrectionCode getTriggerScaleFactor(const xAOD::MuonContainer& mucont, Double_t& triggersf,
                                                     const std::string& trigger) const = 0;

        virtual CorrectionCode getTriggerScaleFactor(const xAOD::Muon& muon, Double_t& triggersf, const std::string& trigger) const = 0;

        virtual CorrectionCode getTriggerEfficiency(const xAOD::Muon& mu, Double_t& efficiency, const std::string& trigger,
                                                    Bool_t dataType) const = 0;

        virtual int getBinNumber(const xAOD::Muon& muon, const std::string& trigger) const = 0;
        /// Returns whether the trigger is supported in the current data-period
        virtual bool isTriggerSupported(const std::string& trigger) const = 0;
    };
    // class IMuonEfficiencyTool

}  // namespace CP

#endif /* IMUONTRIGGERSCALEFACTORS_H_ */
