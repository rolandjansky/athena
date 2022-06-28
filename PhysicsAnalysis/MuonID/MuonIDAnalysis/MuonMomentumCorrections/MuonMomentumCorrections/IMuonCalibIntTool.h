/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_IMuonCalibIntTool_H
#define MCAST_IMuonCalibIntTool_H

/// Framework include(s):
#include "AsgTools/IAsgTool.h"

/// EDM include(s):
#include "MuonMomentumCorrections/MuonObj.h"
#include "MuonMomentumCorrections/EnumDef.h"

/// Local include(s):
#include "AsgMessaging/StatusCode.h"
#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicSet.h"
#include "PATInterfaces/SystematicVariation.h"

namespace CP {

    // This is an interface class for all internal MCP sub-tools
    class IMuonCalibIntTool : public virtual asg::IAsgTool, virtual public CP::ISystematicsTool {
        /// Declare the interface that the class provides
        ASG_TOOL_INTERFACE(CP::IMuonCalibIntTool)

    public:
        /// Apply the correction on a modifyable object
        virtual CorrectionCode applyCorrection(MCP::MuonObj& mu) const = 0;

    };  /// class IMuonCalibIntTool


    // Special internal interface for the scaleSmear tool since it needs to make another function public through its interface
    class IMuonCalibIntScaleSmearTool : public virtual IMuonCalibIntTool {
        /// Declare the interface that the class provides
        ASG_TOOL_INTERFACE(CP::IMuonCalibIntScaleSmearTool)
    public:
        /// Apply the correction on a modifyable object
        virtual double getExpectedResolution(const int &DetType, double pT, double eta, double phi, MCP::DataYear year, bool addMCCorrectionSmearing) const = 0;

    };  /// class IMuonCalibIntScaleSmearTool



}  // namespace CP

#endif  /// MCAST_IMuonCalibIntTool_H
