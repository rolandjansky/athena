/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 *  IMuonTriggerScaleFactors.h
 *
 *  Created on: Oct. 22, 2014
 *      Author: Kota Kasahara <kota.kasahara@cern.ch>
 *
 *  Updates for 2016: Jun 20, 2016
 *      Author: Lidia Dell'Asta <dellasta@cern.ch> 
 */

#ifndef IMUONTRIGGERSCALEFACTORS_H_
#define IMUONTRIGGERSCALEFACTORS_H_

#include "AsgTools/IAsgTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"

#include "PATInterfaces/CorrectionCode.h"
#include <string>
#include <vector>
#include <map>

namespace TrigMuonEff {

    /**
     *  strcuture to hold variables to configure behaviour of the tool
     */
    struct Configuration {
            mutable bool isData;
            int replicaIndex; /**<  index of replicated histograms to access */

            Configuration(const bool isData_ = true,
                            const int replicaIndex_ = -1) :
                            isData(isData_),
                            replicaIndex(replicaIndex_) {
            }
    };
}

/**
 * Helper enumeration for systematics study
 */

namespace CP {

    class IMuonTriggerScaleFactors: public virtual asg::IAsgTool, virtual public CP::ISystematicsTool {
          
            ASG_TOOL_INTERFACE (CP::IMuonTriggerScaleFactors)

        public:
            virtual StatusCode initialize(void) = 0;          

            virtual CorrectionCode getTriggerScaleFactor(const xAOD::MuonContainer& mucont,
                            Double_t& triggersf,
                            const std::string& trigger) = 0;

            virtual CorrectionCode getTriggerEfficiency(const xAOD::Muon& mu,
                            Double_t& efficiency,
                            const std::string& trigger,
                            Bool_t dataType) = 0;
          
    };
// class IMuonEfficiencyTool

}// namespace CP

#endif /* IMUONTRIGGERSCALEFACTORS_H_ */
