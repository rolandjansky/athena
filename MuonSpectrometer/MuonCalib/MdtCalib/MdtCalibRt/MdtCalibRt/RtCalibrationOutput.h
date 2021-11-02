/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RTCALIBRATIONOUTPUT_H
#define RTCALIBRATIONOUTPUT_H

#include <memory>

#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/RtFullInfo.h"
#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
namespace MuonCalib {
    /**
    @class RtCalibrationOutput
    Class for communication between event loop and rt calibration algorithm
    contains only a rt relation for now.

    */

    class RtCalibrationOutput : public IMdtCalibrationOutput {
    public:
        RtCalibrationOutput(std::shared_ptr<const IRtRelation> rt_rel, std::shared_ptr<const RtFullInfo> fi) :
            IMdtCalibrationOutput("RtCalibrationOutput"), m_rtRelation(rt_rel), m_fullInfo(fi) {}
        virtual ~RtCalibrationOutput() = default;
        /** access to private attributes */
        std::shared_ptr<const IRtRelation> rt() const { return m_rtRelation; }
        std::shared_ptr<const RtFullInfo> fullInfo() const { return m_fullInfo; }

    private:
        // pointer to a IRtRelation instance
        std::shared_ptr<const IRtRelation> m_rtRelation;
        /** additonal info for validation */
        std::shared_ptr<const RtFullInfo> m_fullInfo;
    };
}  // namespace MuonCalib
#endif
